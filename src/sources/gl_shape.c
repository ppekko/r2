#include "gl.h"

void r2_gl_render_col(r2_gl* gfx, Vec2 pos, Vec2 siz, Vec4 col, float rot, Vec2 rpos)
{
        r2_gl_use_shader(&gfx->colshd);
        rpos[0] = -rpos[0];
        rpos[1] = -rpos[1];
        mat4 model;
        glm_mat4_identity(model);
        glm_translate(model, (vec3){-rpos[0], -rpos[1], 0.f});
        glm_translate(model, (vec3){pos[0], pos[1], 0.f});
        glm_rotate(model, glm_rad(rot), (vec3){0.f, 0.f, 0.f});
        vec3 scaleVec = {siz[0], siz[1], 1.f};
        glm_scale(model, scaleVec);
        glm_translate(model, (vec3){rpos[0], rpos[1], 0.f});
        GLint colorLoc = glGetUniformLocation(gfx->colshd.id, "color");
        glUniformMatrix4fv(gfx->colshdval.model, 1, false, (const GLfloat*)model);
        glUniformMatrix4fv(gfx->colshdval.proj, 1, false, (const GLfloat*)gfx->proj);
        glUniform4fv(colorLoc, 1, col);
        glBindVertexArray(gfx->bVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);
}

void r2_gl_render_col_center(r2_gl* gfx, Vec2 pos, Vec2 siz, Vec4 col, float rot)
{
        r2_gl_render_col(gfx, (Vec2){pos[0], pos[1]}, siz, col, rot, 
                         (Vec2){0.5f, 0.5f});
}


void r2_gl_render_circle(r2_gl* gfx, Vec2 pos, Vec2 siz, Vec4 col, float rot, Vec2 rpos, 
                         float smooth)
{
        r2_gl_use_shader(&gfx->cirshd);
        rpos[0] = -rpos[0];
        rpos[1] = -rpos[1];
        mat4 model;
        glm_mat4_identity(model);
        glm_translate(model, (vec3){-rpos[0], -rpos[1], 0.f});
        glm_translate(model, (vec3){pos[0], pos[1], 0.f});
        glm_rotate(model, glm_rad(rot), (vec3){0.f, 0.f, 0.f});
        vec3 scaleVec = {siz[0], siz[1], 1.f};
        glm_scale(model, scaleVec);
        glm_translate(model, (vec3){rpos[0], rpos[1], 0.f});
        GLint colorLoc = glGetUniformLocation(gfx->cirshd.id, "ccolor");
        GLint smoothLoc = glGetUniformLocation(gfx->cirshd.id, "smoo");
        glUniformMatrix4fv(gfx->cirshdval.model, 1, false, (const GLfloat*)model);
        glUniformMatrix4fv(gfx->cirshdval.proj, 1, false, (const GLfloat*)gfx->proj);
        glUniform4fv(colorLoc, 1, col);
        glUniform1f(smoothLoc, smooth);
        glBindVertexArray(gfx->bVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);
}

