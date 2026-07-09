# Web wrapper for an interactive CLI binary + Cloudflare Tunnel

Оборачивает интерактивную CLI-программу в веб-терминал (xterm.js) через
WebSocket + PTY и публикует её наружу через **Cloudflare Quick Tunnel**
(бесплатно, без регистрации, живёт пока запущен процесс — идеально для 1–2 дней).

Работает с программами, которые несколько раз спрашивают stdin.

---

## TL;DR

```bash
cd webterm
./run_local.sh /path/to/your_binary        # без Docker
# или
./run_docker.sh /path/to/your_binary       # в контейнере
```

Скрипт сам:
1. кладёт бинарник рядом как `./my_program`;
2. ставит зависимости (или собирает Docker-образ);
3. поднимает uvicorn на `127.0.0.1:8080`;
4. скачивает `cloudflared`, если его нет;
5. запускает `cloudflared tunnel --url http://localhost:8080` и печатает публичный HTTPS-URL.

Ссылка в выводе выглядит примерно так:

```
Your quick Tunnel has been created! Visit it at:
https://random-words-1234.trycloudflare.com
```

Открываете её в браузере — сразу видите терминал с вашей программой.
Ctrl-C в консоли останавливает всё (uvicorn/контейнер + туннель).

---

## Что нужно от вашего бинарника

* Собран под **Linux**, желательно **x86_64** (Cloudflare/Fly/Render — всё amd64).
* Если он под **glibc** — `run_local.sh` и текущий `Dockerfile` (`python:3.12-slim`) подойдут.
* Если под **musl** — либо перекомпилируйте, либо в `Dockerfile` замените базовый образ на `python:3.12-alpine` и допишите `RUN apk add --no-cache libstdc++`.
* Если нужны сторонние `.so` — доставьте пакеты в `Dockerfile` через `apt-get install ...`.

Аргументы программе передаются через `BINARY_ARGS` (для `run_local.sh` — просто вторым и далее аргументом).

---

## Ручной путь (если хотите без скриптов)

```bash
cd webterm
cp /path/to/your_binary ./my_program
chmod +x ./my_program

python3 -m venv .venv && source .venv/bin/activate
pip install -r requirements.txt
BINARY_PATH=./my_program uvicorn server:app --host 127.0.0.1 --port 8080 &

# отдельно — публичный URL
cloudflared tunnel --url http://localhost:8080
```

Установка `cloudflared` (одноразово):

```bash
# Debian/Ubuntu (репозиторий Cloudflare)
curl -L https://pkg.cloudflare.com/cloudflare-main.gpg | sudo tee /usr/share/keyrings/cloudflare-main.gpg >/dev/null
echo "deb [signed-by=/usr/share/keyrings/cloudflare-main.gpg] https://pkg.cloudflare.com/cloudflared any main" \
  | sudo tee /etc/apt/sources.list.d/cloudflared.list
sudo apt update && sudo apt install cloudflared

# или просто скачать бинарник
curl -sSL -o cloudflared \
  https://github.com/cloudflare/cloudflared/releases/latest/download/cloudflared-linux-amd64
chmod +x cloudflared && sudo mv cloudflared /usr/local/bin/
```

---

## Про Cloudflare Quick Tunnel

* Регистрация **не нужна**. Домен свой **не нужен**.
* URL временный (`*.trycloudflare.com`) и меняется при каждом рестарте.
* Живёт, пока крутится процесс `cloudflared` (то есть ровно ваши «день-два»).
* HTTPS + HTTP/2 + WebSocket — всё работает из коробки, наш `/ws` пойдёт как надо.
* Есть лимиты на RPS/трафик, но для демо-интерактива их не заметите.

Если захочется постоянный URL с вашим доменом — заведите бесплатный аккаунт Cloudflare, добавите домен и один раз сделаете `cloudflared tunnel create ...` + `cloudflared tunnel route dns ...`. Но для «на день-два» это лишнее.

---

## Безопасность (важно!)

Публичный URL = **кто угодно запускает вашу программу** в вашей среде. Минимум:

* Не давайте ссылку в открытый интернет без нужды — только тем, кому она реально нужна.
* Запускайте в Docker (`run_docker.sh`) — изоляция от вашей ФС.
* Добавьте basic-auth. В `server.py` перед объявлением роутов:

  ```python
  import secrets
  from fastapi import Depends, HTTPException, status
  from fastapi.security import HTTPBasic, HTTPBasicCredentials

  security = HTTPBasic()
  USER, PASS = "demo", "changeme"

  def auth(c: HTTPBasicCredentials = Depends(security)):
      ok = secrets.compare_digest(c.username, USER) and secrets.compare_digest(c.password, PASS)
      if not ok:
          raise HTTPException(status.HTTP_401_UNAUTHORIZED,
                              headers={"WWW-Authenticate": "Basic"})
  ```

  и повесьте `dependencies=[Depends(auth)]` на `@app.get("/")` и `@app.websocket("/ws")`.

---

## Структура

```
webterm/
├── server.py           FastAPI + WebSocket + PTY
├── static/index.html   xterm.js UI
├── requirements.txt
├── Dockerfile
├── run_local.sh        нативный запуск + cloudflared
├── run_docker.sh       docker build/run + cloudflared
└── README.md
```
