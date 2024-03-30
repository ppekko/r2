#include "gl.h"


void r2_gl_shape_upload(r2_gl* gfx, r2_shape* shape)
{
        INFO("uploading shape...");
        glGenVertexArrays(1, &shape->VAO);
        glGenBuffers(1, &shape->VBO);
        glBindVertexArray(shape->VAO);
        glBindBuffer(GL_ARRAY_BUFFER, shape->VBO);
        glBufferData(GL_ARRAY_BUFFER, shape->vsize * sizeof(float), shape->vertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
        INFO("shape uploaded");
}

void r2_gl_shape_update(r2_gl* gfx, r2_shape* shape)
{
        glBindVertexArray(shape->VAO);
        glBindBuffer(GL_ARRAY_BUFFER, shape->VBO);
        glBufferData(GL_ARRAY_BUFFER, shape->vsize * sizeof(float), shape->vertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
}

void r2_gl_shape_destroy(r2_shape* shape)
{
        INFO("destroying shape...");
        glDeleteVertexArrays(1, &shape->VAO);
        glDeleteBuffers(1, &shape->VBO);
        INFO("shape destroyed");
}

void r2_gl_render_shape(r2_gl* gfx, r2_shape* shape, r2_shdr* shdr, Vec2 pos, Vec2 siz, 
                        float rot, Vec2 rpos)
{
        r2_gl_use_shader(shdr);
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
        GLint modelLoc = glGetUniformLocation(shdr->id, "model");
        GLint projLoc = glGetUniformLocation(shdr->id, "projection");
        glUniformMatrix4fv(modelLoc, 1, false, (const GLfloat*)model);
        glUniformMatrix4fv(projLoc, 1, false, (const GLfloat*)gfx->proj);
        glBindVertexArray(shape->VAO);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, shape->vsize/4);
        glBindVertexArray(0);
}