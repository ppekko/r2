#pragma once
#include "util.h"
#include "gl.h"
#include <stdlib.h>
#include <math.h>

#include "stb_truetype.h"

#define MAX_GLYPHS 128
#define SDF_SIZE 128

typedef struct r2_glyph {
        unsigned int id;
        int w, h;
        int xoff, yoff;
        int bearx, beary;
        int xmin, xmax;
        int xadvance;
        unsigned char* bitmap;
} r2_glyph;

typedef struct r2_font {
        r2_glyph* glyphs[128];
        int asc, des, linegap;
        int newline;
        int sz;
        stbtt_fontinfo info;
} r2_font;


void r2_font_gl_destroy(r2_font* font);
r2_font* r2_font_destroy(r2_font* font);
r2_font* r2_font_load(const char* filename, float font_size);
void r2_font_upload(r2_font* font);
void r2_gl_render_glyph(r2_gl* gfx, r2_glyph* glyph, Vec2 pos, Vec2 size, Vec4 color);
void r2_gl_render_text(r2_gl* gfx, r2_font* font, const char* text, Vec2 pos,
                         float scale, Vec2 spacing, Vec4 color, float max_x_length);
void r2_gl_render_wavy_text(r2_gl* gfx, r2_font* font, const char* text, Vec2 pos,
                            float scale, Vec2 spacing, Vec4 color, float max_x,
                            float amplitude, float frequency, float phase);