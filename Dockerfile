# Base image.
FROM debian:10

# Install dependencies.
RUN apt update && apt install -y \
    make \
    gcc \
    git-core \
    libhidapi-dev \
    python3-dev \
    python3
