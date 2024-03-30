#pragma once
#include "glad/glad.h"

typedef struct {
        unsigned int id;
}r2_shdr;

void r2_gl_compile_shader(r2_shdr* shader, const char* vtx, const char* fra);
void r2_gl_destroy_shader(r2_shdr* shader);
void r2_gl_use_shader(r2_shdr* shader);