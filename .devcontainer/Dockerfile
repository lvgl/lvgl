FROM mcr.microsoft.com/devcontainers/universal:2

RUN git clone --depth 1 https://github.com/emscripten-core/emsdk.git /usr/local/emsdk \
    && cd /usr/local/emsdk \
    && ./emsdk install latest \
    && ./emsdk activate latest \
    && chmod -R 755 /usr/local/emsdk
