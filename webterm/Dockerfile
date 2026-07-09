FROM python:3.12-slim

WORKDIR /app

# Системные зависимости, если бинарник их требует (glibc, libstdc++ и т.п.)
# добавьте нужные пакеты сюда:
# RUN apt-get update && apt-get install -y --no-install-recommends libstdc++6 && rm -rf /var/lib/apt/lists/*

COPY requirements.txt .
RUN pip install --no-cache-dir -r requirements.txt

COPY server.py .
COPY static ./static
COPY my_program .
RUN chmod +x ./my_program

ENV BINARY_PATH=./my_program
ENV PORT=8080
EXPOSE 8080

CMD ["sh", "-c", "uvicorn server:app --host 0.0.0.0 --port ${PORT}"]
