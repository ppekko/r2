#include "fb.h"

void r2_gl_create_framebuffer(r2_framebuffer *fb, int width, int height)
{
        glGenFramebuffers(1, &fb->fbo);
        glGenFramebuffers(1, &fb->msfbo);
        glGenRenderbuffers(1, &fb->rbo);
        glBindFramebuffer(GL_FRAMEBUFFER, fb->msfbo);
        glBindRenderbuffer(GL_RENDERBUFFER, fb->rbo);
        glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_RGB, width, height);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, 
                                  fb->rbo);
        glBindFramebuffer(GL_FRAMEBUFFER, fb->fbo);
        glGenTextures(1, &fb->tex);
        glBindTexture(GL_TEXTURE_2D, fb->tex);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, 
                     NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, 
                               fb->tex, 0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        fb->size[0] = width;
        fb->size[1] = height;
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
                ERROR("r2_framebuffer not complete");
}
void r2_gl_bind_framebuffer(r2_framebuffer *fb)
{
        glBindFramebuffer(GL_FRAMEBUFFER, fb->msfbo);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glViewport(0, 0, fb->size[0], fb->size[1]);

}
void r2_gl_unbind_framebuffer(r2_framebuffer *fb)
{
        glBindFramebuffer(GL_READ_FRAMEBUFFER, fb->msfbo);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fb->fbo);
        glBlitFramebuffer(0, fb->size[1], fb->size[0], 0, 0, 0, fb->size[0], fb->size[1],
                          GL_COLOR_BUFFER_BIT, GL_NEAREST);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void r2_gl_delete_framebuffer(r2_framebuffer *fb)
{
        glDeleteFramebuffers(1, &fb->fbo);
        glDeleteTextures(1, &fb->tex);
        glDeleteRenderbuffers(1, &fb->rbo);
}
void r2_gl_resize_framebuffer(r2_framebuffer *fb, int width, int height)
{
        r2_gl_delete_framebuffer(fb);
        r2_gl_create_framebuffer(fb, width, height);
}
