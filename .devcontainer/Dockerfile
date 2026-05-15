FROM ubuntu:22.04
RUN apt-get update && apt-get install -y \
    build-essential cmake gdb clang clangd \
    git valgrind \
    && rm -rf /var/lib/apt/lists/*