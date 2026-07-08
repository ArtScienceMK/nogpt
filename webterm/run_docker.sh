#!/usr/bin/env bash
# Запуск через Docker + cloudflared quick tunnel.
# Использование:
#   ./run_docker.sh /path/to/your_binary
set -euo pipefail

if [[ $# -lt 1 ]]; then
  echo "Usage: $0 /path/to/binary"
  exit 1
fi

BIN_SRC="$1"
cd "$(dirname "$0")"

cp "$BIN_SRC" ./my_program
chmod +x ./my_program

echo "→ building image..."
docker build -t webterm:latest .

echo "→ starting container on :8080"
docker rm -f webterm >/dev/null 2>&1 || true
docker run -d --name webterm -p 127.0.0.1:8080:8080 webterm:latest

# cloudflared
if ! command -v cloudflared >/dev/null 2>&1 && [[ ! -x ./cloudflared ]]; then
  echo "→ downloading cloudflared..."
  ARCH=$(uname -m)
  case "$ARCH" in
    x86_64) CF_ARCH=amd64 ;;
    aarch64|arm64) CF_ARCH=arm64 ;;
    *) echo "unsupported arch: $ARCH"; exit 1 ;;
  esac
  curl -sSL -o cloudflared \
    "https://github.com/cloudflare/cloudflared/releases/latest/download/cloudflared-linux-${CF_ARCH}"
  chmod +x cloudflared
fi
CFD=$(command -v cloudflared || echo ./cloudflared)

trap 'echo; echo "→ stopping container..."; docker rm -f webterm >/dev/null 2>&1 || true' EXIT

echo "→ starting cloudflared tunnel (public URL появится ниже)"
"$CFD" tunnel --protocol "${CF_PROTO:-http2}" --url http://localhost:8080
