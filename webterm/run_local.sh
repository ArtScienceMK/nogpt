#!/usr/bin/env bash
# Запуск без Docker: venv + uvicorn + cloudflared quick tunnel.
# Использование:
#   ./run_local.sh /path/to/your_binary [arg1 arg2 ...]
set -euo pipefail

if [[ $# -lt 1 ]]; then
  echo "Usage: $0 /path/to/binary [args...]"
  exit 1
fi

BIN_SRC="$1"; shift
BIN_ARGS="$*"

cd "$(dirname "$0")"

# 1. Копируем бинарник рядом
cp "$BIN_SRC" ./my_program
chmod +x ./my_program

# 2. venv + зависимости
if [[ ! -f .venv/bin/activate ]]; then
  rm -rf .venv
  if ! python3 -m venv .venv 2>/tmp/venv.err; then
    echo "!!! python3 -m venv не сработал:"
    cat /tmp/venv.err
    echo
    echo "Скорее всего не хватает пакета python3-venv. Установите:"
    echo "    sudo apt install -y python3-venv python3-pip"
    exit 1
  fi
  if [[ ! -f .venv/bin/activate ]]; then
    echo "!!! .venv создан, но activate отсутствует. Установите python3-venv:"
    echo "    sudo apt install -y python3-venv python3-pip"
    exit 1
  fi
fi
# shellcheck disable=SC1091
source .venv/bin/activate
pip install -q -r requirements.txt

# 3. cloudflared (скачиваем локально, если нет)
if ! command -v cloudflared >/dev/null 2>&1 && [[ ! -x ./cloudflared ]]; then
  echo "→ downloading cloudflared..."
  ARCH=$(uname -m)
  case "$ARCH" in
    x86_64)  CF_ARCH=amd64 ;;
    aarch64|arm64) CF_ARCH=arm64 ;;
    *) echo "unsupported arch: $ARCH"; exit 1 ;;
  esac
  curl -sSL -o cloudflared \
    "https://github.com/cloudflare/cloudflared/releases/latest/download/cloudflared-linux-${CF_ARCH}"
  chmod +x cloudflared
fi
CFD=$(command -v cloudflared || echo ./cloudflared)

# 4. Запускаем uvicorn в фоне
echo "→ starting uvicorn on :8080"
BINARY_PATH=./my_program BINARY_ARGS="$BIN_ARGS" \
  uvicorn server:app --host 127.0.0.1 --port 8080 &
UVICORN_PID=$!

CFD_PID=""
SSH_PID=""
cleanup() {
  echo
  echo "→ stopping..."
  [[ -n "$CFD_PID" ]] && kill "$CFD_PID" 2>/dev/null || true
  [[ -n "$SSH_PID" ]] && kill "$SSH_PID" 2>/dev/null || true
  kill "$UVICORN_PID" 2>/dev/null || true
}
trap cleanup EXIT INT TERM

sleep 2

# 5. Публичный URL
# Порядок провайдеров: cloudflared(http2) → serveo → localhost.run.
# Можно принудительно выбрать: TUNNEL=cloudflared|serveo|localhost.run ./run_local.sh ...
PROVIDERS=(${TUNNEL:-cloudflared serveo localhost.run})
LOGDIR=$(mktemp -d)

check_url() {
  # $1 = url, ждём 200/101/301/302/401/403 в течение таймаута
  local url="$1" deadline=$(( $(date +%s) + 25 ))
  while (( $(date +%s) < deadline )); do
    local code
    code=$(curl -k -s -o /dev/null -m 5 -w "%{http_code}" "$url" || echo 000)
    case "$code" in
      200|101|301|302|401|403) return 0 ;;
    esac
    sleep 2
  done
  return 1
}

try_cloudflared() {
  echo "→ trying cloudflared (${CF_PROTO:-http2})..."
  local log="$LOGDIR/cf.log"
  "$CFD" tunnel --protocol "${CF_PROTO:-http2}" --url http://localhost:8080 \
    >"$log" 2>&1 &
  CFD_PID=$!

  # Ждём URL в логе
  local url="" deadline=$(( $(date +%s) + 20 ))
  while (( $(date +%s) < deadline )); do
    url=$(grep -oE 'https://[a-z0-9-]+\.trycloudflare\.com' "$log" | head -n1 || true)
    [[ -n "$url" ]] && break
    sleep 1
  done
  if [[ -z "$url" ]]; then
    echo "   не получили URL, лог:"
    tail -n 20 "$log" | sed 's/^/     /'
    kill "$CFD_PID" 2>/dev/null || true; CFD_PID=""
    return 1
  fi
  echo "   URL: $url — проверяю доступность..."
  if check_url "$url"; then
    echo
    echo "════════════════════════════════════════════════"
    echo "  ПУБЛИЧНЫЙ URL:  $url"
    echo "════════════════════════════════════════════════"
    return 0
  fi
  echo "   cloudflared не отвечает через публичный URL (скорее всего провайдер режет)."
  kill "$CFD_PID" 2>/dev/null || true; CFD_PID=""
  return 1
}

try_ssh_tunnel() {
  # $1 = host (serveo.net | localhost.run), $2 = ssh user (пусто = дефолт)
  local host="$1" user="${2:-}"
  local target="${user:+${user}@}${host}"
  echo "→ trying ${host} (ssh -R)..."

  if ! command -v ssh >/dev/null 2>&1; then
    echo "   ssh не установлен: sudo apt install -y openssh-client"
    return 1
  fi

  local log="$LOGDIR/${host}.log"
  # StrictHostKeyChecking=accept-new — не спросит про fingerprint интерактивно
  ssh -o StrictHostKeyChecking=accept-new \
      -o ServerAliveInterval=30 \
      -o ExitOnForwardFailure=yes \
      -R 80:localhost:8080 "$target" \
      >"$log" 2>&1 &
  SSH_PID=$!

  local url="" deadline=$(( $(date +%s) + 20 ))
  while (( $(date +%s) < deadline )); do
    url=$(grep -oE 'https://[a-zA-Z0-9.-]+\.(serveo\.net|lhr\.life|localhost\.run)' "$log" | head -n1 || true)
    [[ -n "$url" ]] && break
    sleep 1
  done
  if [[ -z "$url" ]]; then
    echo "   не получили URL, лог:"
    tail -n 20 "$log" | sed 's/^/     /'
    kill "$SSH_PID" 2>/dev/null || true; SSH_PID=""
    return 1
  fi
  echo "   URL: $url — проверяю доступность..."
  if check_url "$url"; then
    echo
    echo "════════════════════════════════════════════════"
    echo "  ПУБЛИЧНЫЙ URL:  $url"
    echo "════════════════════════════════════════════════"
    return 0
  fi
  echo "   $host не отвечает через публичный URL."
  kill "$SSH_PID" 2>/dev/null || true; SSH_PID=""
  return 1
}

ok=0
for p in "${PROVIDERS[@]}"; do
  case "$p" in
    cloudflared)   try_cloudflared           && ok=1 && break ;;
    serveo)        try_ssh_tunnel serveo.net && ok=1 && break ;;
    localhost.run) try_ssh_tunnel localhost.run nokey && ok=1 && break ;;
    *) echo "unknown provider: $p" ;;
  esac
done

if [[ $ok -ne 1 ]]; then
  echo
  echo "!!! Ни один туннель не поднялся. Проверьте интернет / firewall."
  echo "    Логи туннелей: $LOGDIR"
  exit 1
fi

# Держим скрипт живым, пока живёт активный туннель
if [[ -n "$CFD_PID" ]]; then
  wait "$CFD_PID"
elif [[ -n "$SSH_PID" ]]; then
  wait "$SSH_PID"
fi
