#ifndef _STREAM_H
#define _STREAM_H

#include <stdio.h>
#include <stdint.h>

typedef struct {
    FILE *f;
    uint64_t fileSize;
    uint64_t readSize;
    char *buffer;
} stream_t;

int stream_open(stream_t *ctx, const char *filename, int is_reading);
int stream_read(stream_t *ctx, char *buf, int size);
int stream_status(stream_t *ctx);
void stream_close(stream_t *ctx);

#endif
