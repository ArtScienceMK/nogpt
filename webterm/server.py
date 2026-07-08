"""
Минимальный веб-обёртка для интерактивной CLI-программы.
Стек: FastAPI + WebSocket + PTY (модуль ptyprocess).
Каждое подключение = отдельный процесс программы в своём PTY.
"""
import asyncio
import os
import signal
import termios
from pathlib import Path

from fastapi import FastAPI, WebSocket, WebSocketDisconnect
from fastapi.responses import HTMLResponse
from fastapi.staticfiles import StaticFiles
from ptyprocess import PtyProcessUnicode

# Путь к вашему бинарнику (можно переопределить через переменную окружения)
BINARY = os.environ.get("BINARY_PATH", "./my_program")
BINARY_ARGS = os.environ.get("BINARY_ARGS", "").split() if os.environ.get("BINARY_ARGS") else []

app = FastAPI()
app.mount("/static", StaticFiles(directory="static"), name="static")


@app.get("/")
async def index():
    return HTMLResponse(Path("static/index.html").read_text(encoding="utf-8"))


@app.websocket("/ws")
async def ws_endpoint(ws: WebSocket):
    await ws.accept()

    # Запускаем программу в PTY. UTF-8 локаль обязательна,
    # иначе C-программы (scanf/getchar/fgets) режут русские буквы.
    env = {
        **os.environ,
        "LANG": "C.UTF-8",
        "LC_ALL": "C.UTF-8",
        "PYTHONIOENCODING": "utf-8",
        "TERM": "xterm-256color",
    }
    proc = PtyProcessUnicode.spawn(
        [BINARY, *BINARY_ARGS],
        dimensions=(30, 100),
        env=env,
    )

    # Включаем IUTF8: ядро сообщает line discipline, что stdin — UTF-8,
    # и правильно обрабатывает backspace/erase на многобайтовых символах.
    try:
        attrs = termios.tcgetattr(proc.fd)
        # iflag имеет индекс 0 в списке
        attrs[0] |= termios.IUTF8
        termios.tcsetattr(proc.fd, termios.TCSANOW, attrs)
    except Exception as e:
        print(f"[warn] could not set IUTF8: {e}")

    async def pty_to_ws():
        loop = asyncio.get_event_loop()
        try:
            while True:
                # read блокирующий -> уводим в thread executor
                data = await loop.run_in_executor(None, proc.read, 1024)
                # ВАЖНО: PtyProcessUnicode использует incremental UTF-8 decoder.
                # Когда чтение попадает между байтами многобайтового символа
                # (кириллица = 2 байта, эмодзи = 4), decode() возвращает "".
                # Это НЕ EOF — просто ждём следующей порции. EOF выкидывает EOFError.
                if not data:
                    continue
                await ws.send_text(data)
        except (EOFError, OSError):
            pass
        finally:
            try:
                await ws.send_text("\r\n[process exited]\r\n")
                await ws.close()
            except Exception:
                pass

    async def ws_to_pty():
        try:
            while True:
                msg = await ws.receive_text()
                # можно поддержать спец-сообщения ресайза {"resize":[rows,cols]}
                if msg.startswith("__RESIZE__:"):
                    _, dims = msg.split(":", 1)
                    r, c = dims.split(",")
                    try:
                        proc.setwinsize(int(r), int(c))
                    except Exception:
                        pass
                    continue
                # Диагностика: что реально приходит от браузера
                if os.environ.get("WEBTERM_DEBUG"):
                    print(f"[stdin] {msg!r} = {msg.encode('utf-8').hex()}")
                proc.write(msg)
        except WebSocketDisconnect:
            pass
        finally:
            try:
                proc.kill(signal.SIGTERM)
            except Exception:
                pass

    await asyncio.gather(pty_to_ws(), ws_to_pty())
