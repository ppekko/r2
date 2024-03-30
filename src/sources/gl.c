#include "gl.h"
#include "shader.h"
#include "tex.h"
#include "util.h"
#include <SDL2/SDL_stdinc.h>
#include <SDL2/SDL_timer.h>
#include <cglm/cam.h>
#include <cglm/mat4.h>
#include <stdbool.h>
#include <string.h>

const char* r2_vtxshdsrc =  "#version 330 core\n" 
"layout (location = 0) in vec4 vertex;\n" 
"out vec2 TexCoords;\n" 
"uniform mat4 model;\n" 
"uniform mat4 projection;\n" 
"void main()\n" 
"{\n" 
"    TexCoords = vertex.zw;\n" 
"    gl_Position = projection * model * vec4(vertex.xy, 0.0, 1.0);\n" 
"}\n";


const char* r2_imgshdsrc = "#version 330 core\n"
"in vec2 TexCoords;\n"
"out vec4 color;\n"
"uniform sampler2D image;\n"
"void main()\n"
"{\n"
"    color = texture(image, TexCoords);\n"
"}\n";

const char* r2_textshdsrc = "#version 330 core\n"
"in vec2 TexCoords;\n"
"out vec4 color;\n"
"uniform sampler2D text;\n"
"uniform vec4 textColor;\n"
"void main()\n"
"{\n"
"    float sdf = texture(text, TexCoords).r;\n"
"    float smoothness = fwidth(sdf) * 0.5;\n"
"    float edge = smoothstep(0.5 - smoothness, 0.5 + smoothness, sdf);\n"
"    color = vec4(textColor.rgb, textColor.a * edge);\n"
"}\n";

const char* r2_circleshdsrc = "#version 330 core\n"
"in vec2 TexCoords;\n"
"out vec4 color;\n"
"uniform vec4 ccolor;\n"
"uniform float smoo;\n"
"void main()\n"
"{\n"
"    vec4 p = ccolor;\n"
"    vec2 ranged = TexCoords * 2.0 - 1.0;\n"
"    float edge = 1.0 - length(ranged);\n"
"    edge *= smoo;\n"
"    smoothstep(0.f, 1.0, edge);\n"
"    p *= edge;\n"
"    color = p;\n"
"}\n";

const char* r2_colshdsrc = "#version 330 core\n"
"out vec4 FragColor;\n"
"uniform vec4 color;\n"
"void main()\n"
"{\n"
"    FragColor = color;\n"
"}\n";

void r2_gl_init(r2_gl *gfx, Vec2 size, Uint32 strmd) 
{
        INFO("r2 init");
        gfx->winsize[0] = size[0];
        gfx->winsize[1] = size[1];
        gfx->internalres[0] = size[0];
        gfx->internalres[1] = size[1];
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 
        INFO("compiling shaders...");
        r2_gl_compile_shader(&gfx->imgshd, r2_vtxshdsrc, r2_imgshdsrc);
        r2_gl_compile_shader(&gfx->colshd, r2_vtxshdsrc, r2_colshdsrc);
        r2_gl_compile_shader(&gfx->textshd, r2_vtxshdsrc, r2_textshdsrc);
        r2_gl_compile_shader(&gfx->cirshd, r2_vtxshdsrc, r2_circleshdsrc);
        // horrible but necessary for efficiency
        gfx->imgshdval.model = glGetUniformLocation(gfx->imgshd.id, "model");
        gfx->imgshdval.proj = glGetUniformLocation(gfx->imgshd.id, "projection");
        gfx->imgshdval.tex = glGetUniformLocation(gfx->imgshd.id, "image");
        gfx->imgshdval.col = glGetUniformLocation(gfx->colshd.id, "color");
        gfx->colshdval.model = glGetUniformLocation(gfx->colshd.id, "model");
        gfx->colshdval.proj = glGetUniformLocation(gfx->colshd.id, "projection");
        gfx->colshdval.col = glGetUniformLocation(gfx->colshd.id, "color");
        gfx->textshdval.model = glGetUniformLocation(gfx->textshd.id, "model");
        gfx->textshdval.proj = glGetUniformLocation(gfx->textshd.id, "projection");
        gfx->textshdval.tex = glGetUniformLocation(gfx->textshd.id, "text");
        gfx->textshdval.col = glGetUniformLocation(gfx->textshd.id, "textColor");
        gfx->cirshdval.model = glGetUniformLocation(gfx->cirshd.id, "model");
        gfx->cirshdval.proj = glGetUniformLocation(gfx->cirshd.id, "projection");
        gfx->cirshdval.col = glGetUniformLocation(gfx->cirshd.id, "color");
        INFO("shaders compiled");
        INFO("creating vertex buffers...");
        unsigned int VBO;
        static float vertices[] = { 
            0.f, 1.f, 0.f, 1.f,
            1.f, 0.f, 1.f, 0.f,
            0.f, 0.f, 0.f, 0.f, 
        
            0.f, 1.f, 0.f, 1.f,
            1.f, 1.f, 1.f, 1.f,
            1.f, 0.f, 1.f, 0.f
        };
        glGenVertexArrays(1, &gfx->bVAO);
        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, 
                     GL_STATIC_DRAW);
        glBindVertexArray(gfx->bVAO);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 
                              (void*)0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);  
        glBindVertexArray(0);
        gfx->strmd = strmd;
        glm_ortho(0.f, gfx->internalres[0], 0.f, gfx->internalres[1], -1.f, 1.f, gfx->proj);
        glDeleteBuffers(1, &VBO);

        INFO("opengl init complete");
}


void r2_gl_clear(Vec4 col){
        glClearColor(col[0], col[1], col[2], col[3]);
        glClear(GL_COLOR_BUFFER_BIT);
}

void r2_gl_rzevnt(r2_gl* gfx, void (*fbcallback)(r2_gl*), Vec2 nres)
{
        if(nres[0] < 1 || nres[1] < 1) {
                return;
        }
        memcpy(gfx->winsize, nres, sizeof(Vec2));
        switch(gfx->strmd) {
        case R2_STRETCH_TO_WINDOW:
        case R2_KEEP_ASPECT_RATIO:
        glm_ortho(0.f, gfx->internalres[0], 0.f, gfx->internalres[1], -1.f, 1.f,
                  gfx->proj);
        break;
        case R2_NO_SCALING:
        glm_ortho(0.f, gfx->winsize[0], 0.f, gfx->winsize[1], -1.f, 1.f, gfx->proj);
        gfx->internalres[0] = gfx->winsize[0];
        gfx->internalres[1] = gfx->winsize[1];
        if(fbcallback != NULL) {
                fbcallback(gfx);
        }
        break;
        }
}


void r2_gl_destroy(r2_gl* gfx)
{
        INFO("destroying shaders...");
        r2_gl_destroy_shader(&gfx->imgshd);
        r2_gl_destroy_shader(&gfx->colshd);
        r2_gl_destroy_shader(&gfx->textshd);
        INFO("destroying opengl...");
        glDeleteVertexArrays(1, &gfx->bVAO);
        INFO("opengl destroyed");
}

void r2_gl_render_tex(r2_gl* gfx, r2_tex *img, Vec2 pos, Vec2 siz, bool flip,
                        float rot, Vec2 rpos)
{
        if(img->id == 0){
                ERROR("image is empty (did you forget to call r2_tex_load?)");
                return;
        }
        rpos[0] = -rpos[0];
        rpos[1] = -rpos[1];
        r2_gl_use_shader(&gfx->imgshd);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, img->id);
        mat4 model;
        glm_mat4_identity(model);
        glm_translate(model, (vec3){-rpos[0], -rpos[1], 0.f});
        glm_translate(model, (vec3){flip ? pos[0] + siz[0] : pos[0], pos[1], 0.f});
        glm_rotate(model, glm_rad(rot), (vec3){0.f, 0.f, 1.f});
        vec3 scaleVec = {flip ? -siz[0] : siz[0], siz[1], 1.f};
        glm_scale(model, scaleVec);
        glm_translate(model, (vec3){rpos[0], rpos[1], 0.f});
        glUniformMatrix4fv(gfx->imgshdval.model, 1, false, (const GLfloat*)model);
        glUniformMatrix4fv(gfx->imgshdval.proj, 1, false, (const GLfloat*)gfx->proj);
        glBindVertexArray(gfx->bVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);
        glBindVertexArray(0);
}

void r2_gl_render_gltex(r2_gl* gfx, GLuint *img, Vec2 pos, Vec2 siz, bool flip,
                          float rot, Vec2 rpos)
{
        r2_gl_use_shader(&gfx->imgshd);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, *img);
        mat4 model;
        rpos[0] = -rpos[0];
        rpos[1] = -rpos[1];
        glm_mat4_identity(model);
        glm_translate(model, (vec3){-rpos[0], -rpos[1], -rpos[2]});
        glm_translate(model, (vec3){flip ? pos[0] + siz[0] : pos[0], pos[1], 0.f});
        glm_rotate(model, glm_rad(rot), (vec3){0.f, 0.f, 1.f});
        vec3 scaleVec = {flip ? -siz[0] : siz[0], siz[1], 1.f};
        glm_scale(model, scaleVec);
        glm_translate(model, rpos);
        glUniformMatrix4fv(gfx->imgshdval.model, 1, false, (const GLfloat*)model);
        glUniformMatrix4fv(gfx->imgshdval.proj, 1, false, (const GLfloat*)gfx->proj);
        glBindVertexArray(gfx->bVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);
        glBindVertexArray(0);
}

void r2_gl_render_tex_center(r2_gl* gfx, r2_tex *img, Vec2 pos, Vec2 siz, bool flip, 
                               float rot)
{
        r2_gl_render_tex(gfx, img, (Vec2){pos[0], pos[1]}, siz, flip, rot, 
                         (Vec2){0.5, 0.5});
}

void r2_gl_render_framebuffer(r2_gl* gfx, GLuint *img)
{
        glViewport(0, 0, gfx->winsize[0], gfx->winsize[1]);
        glClearColor(0.f, 0.f, 0.f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT);
        r2_gl_use_shader(&gfx->imgshd);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, *img);
        mat4 proj;
        mat4 ident;
        glm_mat4_identity(ident);
        glm_mat4_identity(proj);
        switch(gfx->strmd) {
        case R2_KEEP_ASPECT_RATIO: {
        float asp = (float)gfx->winsize[0] / (float)gfx->winsize[1];
        float size_x = 1.f;
        float size_y = 1.f;
        if (asp > gfx->internalres[0] / gfx->internalres[1]) {
                size_x = asp * gfx->internalres[1] / gfx->internalres[0];
        } else {
                size_y = gfx->internalres[0] / (asp * gfx->internalres[1]);
        }
        float rlszx = (-size_x + 1)/2.;
        float rlszy = (-size_y + 1)/2.;
        glm_ortho(rlszx, -rlszx+1, rlszy, -rlszy+1, -1.f, 1.f, proj);
        break;
        }
        case R2_NO_SCALING: {
        glm_ortho(0.f, gfx->internalres[0], 0.f, gfx->internalres[1], -1.f, 1.f, 
                  proj);
        }
        default:
        glm_ortho(0.f, 1.f, 0.f, 1.f, -1.f, 1.f, proj);
        break;
        }
        GLint modelLoc = glGetUniformLocation(gfx->imgshd.id, "model");
        GLint projLoc = glGetUniformLocation(gfx->imgshd.id, "projection");
        glUniformMatrix4fv(modelLoc, 1, false, (const GLfloat*)ident);
        glUniformMatrix4fv(projLoc, 1, false, (const GLfloat*)proj);
        glBindVertexArray(gfx->bVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);
}


void r2_gl_render_cshd(r2_gl* gfx, r2_shdr* custom_shader, Vec2 pos, Vec2 siz, float rot,
                       Vec2 rpos) 
{
    r2_gl_use_shader(custom_shader);
    rpos[0] = -rpos[0];
    rpos[1] = -rpos[1];
    mat4 model;
    glm_mat4_identity(model);
    glm_translate(model, (vec3){-rpos[0], -rpos[1], 0.f});
    glm_translate(model, (vec3){pos[0], pos[1], 0.f});
    glm_rotate(model, glm_rad(rot), (vec3){0.f, 0.f, 1.f});
    vec3 scaleVec = {siz[0], siz[1], 1.f};
    glm_scale(model, scaleVec);
    glm_translate(model, (vec3){rpos[0], rpos[1], 0.f});
    GLint modelLoc = glGetUniformLocation(custom_shader->id, "model");
    GLint projLoc = glGetUniformLocation(custom_shader->id, "projection");
    glUniformMatrix4fv(modelLoc, 1, false, (const GLfloat*)model);
    glUniformMatrix4fv(projLoc, 1, false, (const GLfloat*)gfx->proj);
    glBindVertexArray(gfx->bVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

void r2_gl_render_center_cshd(r2_gl* gfx, r2_shdr* custom_shader, Vec2 pos, Vec2 siz, 
                              float rot)
{
        r2_gl_render_cshd(gfx, custom_shader,
                          (Vec2){pos[0], pos[1]}, siz, rot,
                          (Vec2){0.5, 0.5});
}
