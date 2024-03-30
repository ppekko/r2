#include "shader.h"
#include "util.h"
#include <stddef.h>

void r2_gl_compile_shader(r2_shdr* shader, const char* vtx, const char* fra)
{
        u16 vtxshd, frgshd;
        int success;
        char infl[512];
        vtxshd = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vtxshd, 1, &vtx, NULL);
        glCompileShader(vtxshd);
        glGetShaderiv(vtxshd, GL_COMPILE_STATUS, &success);
        if (!success) {
                glGetShaderInfoLog(vtxshd, 512, NULL, infl);
                ERROR("failed to compile vertex shader\n%s", infl);
        }
        frgshd = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(frgshd, 1, &fra, NULL);
        glCompileShader(frgshd);
        glGetShaderiv(frgshd, GL_COMPILE_STATUS, &success);
        if (!success) {
                glGetShaderInfoLog(frgshd, 512, NULL, infl);
                ERROR("failed to compile fragment shader\n%s", infl);
        }
        shader->id = glCreateProgram();
        glAttachShader(shader->id, vtxshd);
        glAttachShader(shader->id, frgshd);
        glLinkProgram(shader->id);
        glGetProgramiv(shader->id, GL_LINK_STATUS, &success);
        if (!success) {
                glGetProgramInfoLog(shader->id, 512, NULL, infl);
                ERROR("failed to link shader\n%s", infl);
        }
        glDeleteShader(vtxshd);
        glDeleteShader(frgshd);
        CHK(success, "shader compiling (vertex hash: %lx, fragment hash: %lx)", hash(vtx, strlen(vtx), 0.), hash(fra, strlen(fra), 0.));
}

void r2_gl_destroy_shader(r2_shdr* shader)
{
        glDeleteProgram(shader->id);
}

void r2_gl_use_shader(r2_shdr* shader)
{
        glUseProgram(shader->id);
}