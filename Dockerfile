# Base image.
FROM debian:stable-slim

COPY * ./

# Install dependencies.
RUN apt update && apt install -y \
    make \
    gcc \
    git-core \
    libhidapi-dev \
    python3-dev \
    python3 \
    python3-venv \ 
    python3-build
