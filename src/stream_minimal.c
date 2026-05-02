#include "stream_minimal.h"
#include <stdlib.h>

int stream_open(stream_t *ctx, const char *filename, int is_reading) {
    if (!ctx || !filename) return 0;
    
    ctx->f = fopen(filename, is_reading ? "rb" : "wb");
    if (!ctx->f) return 0;
    
    if (is_reading) {
#ifdef _WIN32
        _fseeki64(ctx->f, 0, SEEK_END);
        ctx->fileSize = _ftelli64(ctx->f);
        _fseeki64(ctx->f, 0, SEEK_SET);
#else
        fseek(ctx->f, 0, SEEK_END);
        ctx->fileSize = ftell(ctx->f);
        fseek(ctx->f, 0, SEEK_SET);
#endif
    } else {
        ctx->fileSize = 0;
    }
    
    ctx->readSize = 0;
    ctx->buffer = NULL;
    return 1;
}

int stream_read(stream_t *ctx, char *buf, int size) {
    if (!ctx || !ctx->f) return 0;
    int r = fread(buf, 1, size, ctx->f);
    if (r > 0) ctx->readSize += r;
    return r;
}

int stream_status(stream_t *ctx) {
    if (!ctx || !ctx->f || ctx->fileSize == 0) return 0;
    return (int)((ctx->readSize * 100) / ctx->fileSize);
}

void stream_close(stream_t *ctx) {
    if (ctx && ctx->f) {
        fclose(ctx->f);
        ctx->f = NULL;
    }
}
