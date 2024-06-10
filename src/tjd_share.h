//
// Created by tjdic on 6/9/2024.
//

#ifndef TJD_SHARE_H
#define TJD_SHARE_H


#include <cstdint>
#include <cstdlib>
#include <cstring>

#define LOGINF(...) printf(__VA_ARGS__)
#define LOGERR(...) printf(__VA_ARGS__)


typedef int8_t   s8;
typedef int16_t  s16;
typedef int32_t  s32;
typedef int64_t  s64;
typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef double   f64;
typedef float    f32;


static u16 swap_bytes(const u16 value) {
    return (value & 0x00ff) << 8 | (value & 0xff00) >> 8;
}

static u32 swap_bytes(const u32 value) {
    return (value & 0xff) << 24 | (value & 0xff0000) >> 8 | (value & 0xff00) << 8 | (value & 0xff000000) >> 24;
}


typedef struct Vector4f32_t {
    union {
        f32 min_x;
        f32 x;
        f32 r;
    };

    union {
        f32 min_y;
        f32 y;
        f32 g;
    };

    union {
        f32 max_x;
        f32 z;
        f32 b;
    };

    union {
        f32 max_y;
        f32 w;
        f32 a;
    };
} v4f32;

typedef struct Vector2f64_t {
    f64 x, y;
} v2f64;

typedef struct Vector2s32_t {
    s32 x, y;
} v2s32;

typedef struct Vector2f32_t {
    union {
        f32 x;
        f32 lon;
    };

    union {
        f32 y;
        f32 lat;
    };
} v2f32;

typedef v4f32 color4;


struct BufferInfo {
    void *data;
    u32   position;
    u32   length;
};

static void init_buffer(BufferInfo *buffer, void *data, const u32 length) {
    buffer->data     = nullptr;
    buffer->position = 0;
    buffer->length   = 0;

    if (data != nullptr) {
        buffer->data   = data;
        buffer->length = length;
    }
}

static void *get_buffer_marker(const BufferInfo *buffer) {
    return (void *)((char *)buffer->data + buffer->position);
}

static bool read_from_buffer(void *dest, BufferInfo *buffer, const s32 length) {
    if (buffer->position + length <= buffer->length) {
        memcpy(dest, (void *)((char *)buffer->data + buffer->position), length);
        buffer->position += length;

        return true;
    }

    return false;
}

static bool seek_buffer(BufferInfo *buffer, const s32 jmp) {
    if (buffer->position + jmp < buffer->length) {
        buffer->position += jmp;
        return true;
    }

    return false;
}

static bool set_buffer_pos(BufferInfo *buffer, const u32 position) {
    if (position >= 0 && position < buffer->length) {
        buffer->position = position;
        return true;
    }

    return false;
}

static unsigned char peek_buffer(const BufferInfo *buffer, const s32 jump) {
    if (buffer->data != nullptr && (buffer->position + jump) < buffer->length) {
        return *(((unsigned char *)buffer->data + buffer->position) + jump);
    }

    return 0;
}

static void free_buffer(BufferInfo *buffer) {
    if (buffer->data != nullptr) {
        free(buffer->data);
        buffer->data = nullptr;
    }
}

#endif // TJD_SHARE_H
