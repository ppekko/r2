#pragma once
#include "util.h"
#include "qoi.h"
#include "glad/glad.h"

typedef struct  {
        qoi_desc desc;
        void *rgba_pixels;
        GLuint id;
        char loc[32];
} r2_tex;

enum {
        CR2D_TEX_RGBA,
        CR2D_TEX_RGB,
};
enum {
        CR2D_FILTER_NEAREST,
        CR2D_FILTER_LINEAR,
};

void r2_tex_load(r2_tex *img, const char *loc);
void r2_tex_upload(r2_tex* img, int fmt, int filter);
void r2_tex_destroy(r2_tex *img);
void r2_tex_gl_destroy(r2_tex *img);