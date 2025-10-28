FROM ubuntu:24.04
RUN apt-get update && apt-get install -y --no-install-recommends \
    build-essential cmake git file ca-certificates gdb \
 && rm -rf /var/lib/apt/lists/*
WORKDIR /src