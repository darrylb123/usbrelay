# Base image.
FROM debian:latest-slim

COPY * .

# Install dependencies.
RUN apt update && apt install -y \
    make \
    gcc \
    git-core \
    libhidapi-dev \
    python3-dev \
    python3
