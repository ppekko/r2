#pragma once
#include <SDL2/SDL.h>
#include "glad/glad.h"
#include <stdbool.h>
#include "util.h" 
#include "tex.h"
#include <cglm/call.h> 
#include <stdbool.h>
#include "fb.h"
#include "shader.h"

typedef struct {
        unsigned int model;
        unsigned int proj;
        unsigned int col;
        unsigned int tex;
} r2_internal_glval;

typedef enum {
        R2_STRETCH_TO_WINDOW,
        R2_KEEP_ASPECT_RATIO,
        R2_NO_SCALING
} r2_stretch_flags;

typedef struct {
        Vec2 winsize;
        Vec2 internalres;

        GLuint bVAO;
        mat4 proj;

        r2_stretch_flags strmd;
        r2_shdr imgshd;
        r2_shdr textshd;
        r2_shdr colshd;
        r2_shdr cirshd;
        r2_internal_glval imgshdval;
        r2_internal_glval textshdval;
        r2_internal_glval colshdval;
        r2_internal_glval cirshdval;

} r2_gl;

typedef struct {
        int vsize;
        float* vertices;
        GLuint VBO, VAO;
} r2_shape;

void r2_gl_init(r2_gl *gfx, Vec2 size, Uint32 flags) ;
void r2_gl_destroy(r2_gl *gfx);
void r2_gl_rzevnt(r2_gl* gfx, void (*fbcallback)(r2_gl*), Vec2 nres);
void r2_gl_clear(Vec4 col);
void r2_gl_render_tex(r2_gl* gfx, r2_tex *img, Vec2 pos, Vec2 siz, bool flip, 
                        float rot, Vec2 rpos);
void r2_gl_render_gltex(r2_gl* gfx, GLuint *img, Vec2 pos, Vec2 siz, bool flip,
                          float rot, Vec2 rpos);
void r2_gl_render_framebuffer(r2_gl* gfx, GLuint *img);
void r2_gl_render_col(r2_gl* gfx, Vec2 pos, Vec2 siz, Vec4 col, float rot, Vec2 rpos);
void r2_gl_render_col_center(r2_gl* gfx, Vec2 pos, Vec2 siz, Vec4 col, float rot);
void r2_gl_render_tex_center(r2_gl* gfx, r2_tex *img, Vec2 pos, Vec2 siz, bool flip,
                               float rot);
void r2_gl_render_circle(r2_gl* gfx, Vec2 pos, Vec2 siz, Vec4 col, float rot, Vec2 rpos,
                         float smooth);
void r2_gl_render_cshd(r2_gl* gfx, r2_shdr* custom_shader, Vec2 pos, Vec2 siz, float rot,
                       Vec2 rpos);
void r2_gl_render_center_cshd(r2_gl* gfx, r2_shdr* custom_shader, Vec2 pos, Vec2 siz,
                              float rot);
void r2_gl_shape_upload(r2_gl* gfx, r2_shape* shape);
void r2_gl_render_shape(r2_gl* gfx, r2_shape* shape, r2_shdr* shdr, Vec2 pos, Vec2 siz,
                        float rot, Vec2 rpos);
void r2_gl_shape_update(r2_gl* gfx, r2_shape* shape);
void r2_gl_shape_destroy(r2_shape* shape);