#include "tex.h"
#include "util.h"

void r2_tex_load(r2_tex* img, const char *loc) 
{
        img->rgba_pixels = qoi_read(loc, &img->desc, 4);
        MCHK(img->rgba_pixels != NULL, "load qoi file: %s", loc);
        if (img->rgba_pixels) {
                strncpy(img->loc, loc, 32);
        }
}

void r2_tex_upload(r2_tex* img, int fmt, int filter) 
{
        INFO("uploading texture %s", img->loc);
        if(strlen(img->loc) == 0)
                WARN("uploading an uninitialized texture \
                                                  (did you forget to call r2_tex_load?)");
        glGenTextures(1, &img->id);
        glBindTexture(GL_TEXTURE_2D, img->id);
        glTexImage2D(GL_TEXTURE_2D, 0, fmt == CR2D_TEX_RGB ? GL_RGB : GL_RGBA,
                     img->desc.width, img->desc.height, 0, GL_RGBA, GL_UNSIGNED_BYTE,
                     img->rgba_pixels);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,  
                                  filter == CR2D_FILTER_NEAREST ? GL_NEAREST : GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, 
                                  filter == CR2D_FILTER_NEAREST ? GL_NEAREST : GL_LINEAR);
        glGenerateMipmap(GL_TEXTURE_2D);
}

void r2_tex_destroy(r2_tex *img)
{
        if (strlen(img->loc) == 0)
                WARN("trying to free an uninitialized texture");
        INFO("destroying texture %s", img->loc);
        free(img->rgba_pixels);
}

void r2_tex_gl_destroy(r2_tex *img)
{
        if (strlen(img->loc) == 0)
                WARN("trying to free an uninitialized texture");
        INFO("destroying texture from gpu %s", img->loc);
        glDeleteTextures(1, &img->id);
}