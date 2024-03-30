#include "font.h"
#include "shader.h"
#include "util.h"
#include <cglm/vec2.h>
#include <stdio.h>

#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"

#include <stdlib.h>
#include <stdio.h>



r2_font* r2_font_load(const char* filename, float sz)
{
        INFO("loading font: %s", filename);
        FILE* file = fopen(filename, "rb");
        if (!file) {
                ERROR("Failed to open font file: %s\n", filename);
                return NULL;
        }
        fseek(file, 0, SEEK_END);
        size_t filesize = ftell(file);
        fseek(file, 0, SEEK_SET);
        unsigned char* buf = malloc(filesize);
        fread(buf, filesize, 1, file);
        fclose(file);
        r2_font* font = malloc(sizeof(r2_font));
        font->sz = sz;
        stbtt_InitFont(&font->info, buf, 0);
        float scale = stbtt_ScaleForPixelHeight(&font->info, sz)*10.;
        stbtt_GetFontVMetrics(&font->info, &font->asc, &font->des, 
                              &font->linegap);
        font->newline = font->asc - font->des + font->linegap;
        font->newline *= scale;
        for (int i = 32; i != 128; i++) {
                CRINFO("loading font [%i/%i] %i%%", i, 128, (i*100)/128);
                r2_glyph* glyph = malloc(sizeof(r2_glyph));
                glyph->bitmap = stbtt_GetCodepointSDF(&font->info, scale, i, 5, 180,
                                                      10., &glyph->w, &glyph->h,
                                                      &glyph->xoff, &glyph->yoff);
                stbtt_GetCodepointHMetrics(&font->info, i, &glyph->xadvance, NULL);
                stbtt_GetCodepointBitmapBox(&font->info, i, scale, scale,
                                            &glyph->xmin, &glyph->xmax, &glyph->bearx,
                                            &glyph->beary);
                font->glyphs[i] = glyph;
                if (glyph->bitmap == NULL){
                        CRWARN("empty SDF generated for character code %i\n", i);
                }
        }
        fputs("\033[A\033[2K",stdout);
        rewind(stdout);
        CRINFO("font loaded\n");
        return font;
}

r2_font* r2_font_destroy(r2_font* font)
{
        INFO("destroying font...");
        for (int i = 32; i < 128; i++) {
                r2_glyph* glyph = font->glyphs[i];
                free(glyph->bitmap);
                free(glyph);
        }
        free(font);
        return NULL;
}

void r2_font_upload(r2_font* font)
{
        INFO("uploading font to gpu...");
        if(font == NULL) {
                ERROR("font is empty (did you forget to call r2_font_load?)");
                return;
        }
        for (int i = 32; i < 128; i++) {
                r2_glyph* glyph = font->glyphs[i];
                glGenTextures(1, &glyph->id);
                glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
                glBindTexture(GL_TEXTURE_2D, glyph->id);
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, glyph->w, glyph->h, 0, 
                             GL_RED, GL_UNSIGNED_BYTE, glyph->bitmap);
                if (glGetError() != GL_NO_ERROR){
                        WARN("failed to upload glyph to gpu: %i '%c'", i, i);
                        glDeleteTextures(1, &glyph->id);
                }else {
                        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
                                        GL_CLAMP_TO_EDGE);
                        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,
                                        GL_CLAMP_TO_EDGE);
                        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                        glBindTexture(GL_TEXTURE_2D, 0);
                }
        }
        INFO("font uploaded");
}

void r2_font_gl_destroy(r2_font* font)
{
        INFO("destroying font from gpu...");
        for (int i = 32; i < 128; i++) {
                r2_glyph* glyph = font->glyphs[i];
                glDeleteTextures(1, &glyph->id);
        }
}

void r2_gl_render_glyph(r2_gl* gfx, r2_glyph* glyph, Vec2 pos, Vec2 sz, Vec4 color)
{
        Vec2 rpos = {0.f, 0.f};
        if (glyph == NULL)
                return;
        r2_gl_use_shader(&gfx->textshd);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, glyph->id);
        mat4 model;
        glm_mat4_identity(model);
        glm_translate(model, (vec3){pos[0], pos[1], 0.f});
        vec3 scaleVec = {sz[0], sz[1], 1.f};
        glm_scale(model, scaleVec);
        glm_translate(model, (vec3){rpos[0], rpos[1], 0.f});
        glUniformMatrix4fv(glGetUniformLocation(gfx->textshd.id, "model"), 1, false,
                                                (const GLfloat*)model);
        glUniformMatrix4fv(glGetUniformLocation(gfx->textshd.id, "projection"), 1, false,
                                                (const GLfloat*)gfx->proj);
        glUniform4f(glGetUniformLocation(gfx->textshd.id, "textColor"), color[0],
                                         color[1], color[2], color[3]);
        glBindVertexArray(gfx->bVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);
        glBindVertexArray(0);
}

void r2_gl_render_text(r2_gl* gfx, r2_font* font, const char* text, Vec2 pos,
                       float scale, Vec2 spacing, Vec4 color, 
                       float max_x)
{
        if(font == NULL){
                ERROR("font is empty (did you forget to call r2_font_load?)");
                return;
        }
        Vec2 cursor = {pos[0], pos[1]};
        int text_length = strlen(text);
        for (int i = 0; i < text_length; i++) {
                r2_glyph* glyph = font->glyphs[(int)text[i]];
                if (text[i] == '\n') {
                        cursor[0] = pos[0];
                        cursor[1] += font->newline * scale * spacing[1];
                        continue;
                }else if (text[i] == '\t') {
                        cursor[0] += font->glyphs[(int)' ']->xadvance * scale * 4;
                        continue;
                }
                Vec2 ssz = {glyph->w*scale, glyph->h*scale};
                Vec2 sof = {glyph->xoff*scale, glyph->yoff*scale};
                Vec2 spc;
                spc[0] = ((font->sz*glyph->xadvance) >> 6)  * scale * 
                                        spacing[0];
                spc[1] = font->newline * scale * spacing[1];
                Vec2 glyphPos = {cursor[0] + sof[0], cursor[1] + sof[1]};
                if (glyphPos[0] + ssz[0] + spc[0] > max_x && 
                    max_x != 0.) {
                        cursor[0] = pos[0];
                        cursor[1] += font->newline * scale * spacing[1];
                        glyphPos[0] = cursor[0] + sof[0];
                        glyphPos[1] = cursor[1] + sof[1];
                }
                r2_gl_render_glyph(gfx, glyph, glyphPos, ssz, color);
                cursor[0] += spc[0];
        }
}

void r2_gl_render_wavy_text(r2_gl* gfx, r2_font* font, const char* text, Vec2 pos,
                            float scale, Vec2 spacing, Vec4 color, float max_x,
                            float amp, float freq, float pha)
{
        if(font == NULL){
                ERROR("font is empty (did you forget to call r2_font_load?)");
                return;
        }
        Vec2 cursor = {pos[0], pos[1]};
        int text_length = strlen(text);
        for (int i = 0; i < text_length; i++) {
                r2_glyph* glyph = font->glyphs[(int)text[i]];
                if (text[i] == '\n') {
                        cursor[0] = pos[0];
                        cursor[1] += font->newline * scale * spacing[1];
                        continue;
                }else if (text[i] == '\t') {
                        cursor[0] += font->glyphs[(int)' ']->xadvance * scale * 4;
                        continue;
                }
                Vec2 ssz = {glyph->w*scale, glyph->h*scale};
                Vec2 sof = {glyph->xoff*scale, glyph->yoff*scale};
                Vec2 spc;
                spc[0] = ((font->sz*glyph->xadvance) >> 6)  * scale * 
                                        spacing[0];
                spc[1] = font->newline * scale * spacing[1];
                Vec2 glyphPos = {cursor[0] + sof[0], cursor[1] + sof[1]};
                if (glyphPos[0] + ssz[0] + spc[0] > max_x && 
                    max_x != 0.) {
                        cursor[0] = pos[0];
                        cursor[1] += font->newline * scale * spacing[1];
                        glyphPos[0] = cursor[0] + sof[0];
                        glyphPos[1] = cursor[1] + sof[1];
                }
                glyphPos[1] += amp * sin(freq * (glyphPos[0] + pha));
                r2_gl_render_glyph(gfx, glyph, glyphPos, ssz, color);
                cursor[0] += spc[0];
        }
}



