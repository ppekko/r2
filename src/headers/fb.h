#pragma once
#include "glad/glad.h"
#include "util.h" 

typedef struct r2_framebuffer
{
    char name[32];
    GLuint fbo;
    GLuint msfbo;
    GLuint tex;
    GLuint rbo;
    GLuint *shader;
    Vec2 size;
} r2_framebuffer;

void r2_gl_create_framebuffer(r2_framebuffer *fb, int width, int height);
void r2_gl_bind_framebuffer(r2_framebuffer *fb);
void r2_gl_unbind_framebuffer(r2_framebuffer *fb);
void r2_gl_delete_framebuffer(r2_framebuffer *fb);
void r2_gl_resize_framebuffer(r2_framebuffer *fb, int width, int height);