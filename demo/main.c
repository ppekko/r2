#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_video.h>
#include <gl.h>
#include <stdbool.h>
#include <tex.h>
#include <util.h>
#include <SDL2/SDL.h>
#include "glad/glad.h"
#include <font.h>


r2_framebuffer fb; // dont do this in a real program
void resize(r2_gl* gfx) {
        r2_gl_resize_framebuffer(&fb, gfx->internalres[0], gfx->internalres[1]);
}

int main()
{
#ifdef SDL_HINT_VIDEO_X11_NET_WM_BYPASS_COMPOSITOR
        SDL_SetHint(SDL_HINT_VIDEO_X11_NET_WM_BYPASS_COMPOSITOR, "0");
#endif
        SDL_Window *win = SDL_CreateWindow("demo", 0, 0, 1280, 720, SDL_WINDOW_RESIZABLE | 
                                    SDL_WINDOW_OPENGL);
        SDL_GLContext ctx;
        if (!win) {
                DIE("failed to create SDL window: %s", SDL_GetError());
        }
        ctx = SDL_GL_CreateContext(win);
        if (!ctx) {
                DIE("failed to create opengl context: %s", SDL_GetError());
                SDL_DestroyWindow(win);
        }
        gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress);
        if (!&gladLoadGLLoader) {
                DIE("failed to load opengl functions with glad");
                SDL_GL_DeleteContext(ctx);
                SDL_DestroyWindow(win);
        }
        SDL_GL_SetSwapInterval(1);
        //
        r2_gl gfx;
        r2_gl_init(&gfx, (Vec2){1280.f, 720.f}, R2_KEEP_ASPECT_RATIO);
        r2_gl_create_framebuffer(&fb, 1280, 720);
        r2_tex image1;
        r2_tex_load(&image1, "rsc/smile2.qoi");
        r2_tex_upload(&image1, CR2D_TEX_RGBA, CR2D_FILTER_LINEAR);
        r2_tex image2;
        r2_tex_load(&image2, "rsc/smile.qoi");
        r2_tex_upload(&image2, CR2D_TEX_RGBA, CR2D_FILTER_NEAREST);
        r2_font* a = r2_font_load("rsc/monaspace.ttf", 16);
        r2_font_upload(a);
        r2_shape shp;
        shp.vsize = 4*6;
        shp.vertices = (float[]){
                0.0f, 0.0f, 0.0f, 0.0f, //bottom left
                1.0f, 0.0f, 1.0f, 0.0f, //bottom right
                0.0f, 1.0f, 0.0f, 1.0f, //top left
                0.0f, 1.0f, 0.0f, 1.0f, //top left
                1.0f, 0.0f, 1.0f, 0.0f, //bottom right
                1.0f, 1.0f, 1.0f, 1.0f  //top right
        };
        r2_gl_shape_upload(&gfx, &shp);

        bool running = true;
        while (running) {
                SDL_Event e;
                while (SDL_PollEvent(&e)) {
                        switch (e.type) {
                                case SDL_QUIT:
                                running = false;
                                break;
                                case SDL_WINDOWEVENT:
                                if (e.window.event == SDL_WINDOWEVENT_RESIZED) {
                                r2_gl_rzevnt(&gfx, resize, (Vec2){e.window.data1,
                                                                  e.window.data2});
                                }
                                break;
                        }
                }
                r2_gl_bind_framebuffer(&fb);
                r2_gl_clear((Vec4){0.2f, 0.2f, 0.2f, 1.0f});
                r2_gl_render_col_center(&gfx, (Vec2){500, 100}, (Vec2){100, 100}, (Vec4){
                        sin(0.1 * (SDL_GetTicks() / 100.f) + 0.f) * 0.5f + 0.5f, 
                        sin(0.1 * (SDL_GetTicks() / 100.f) + 2.f * 3.14159f / 3.f) * 0.5f
                                                                                   + 0.5f,
                        sin(0.1 * (SDL_GetTicks() / 100.f) + 4.f * 3.14159f / 3.f) * 0.5f
                                                                                   + 0.5f,
                        1}, 0.0);
                Vec2 pos = {100.f,100.f};
                Vec2 size = {image1.desc.width * 5., image1.desc.height * 5.};
                r2_gl_render_tex_center(&gfx, &image1, pos, size, true,
                                                               SDL_GetTicks() / 10.f);
                r2_gl_render_tex(&gfx, &image2, (Vec2){200.f, 200.f}, (Vec2){100.f, 100.f},
                                 (Vec4){1.f, 1.f, 1.f, 1.f}, 0, (Vec2){0.f,0.f});
                r2_gl_render_circle(&gfx, (Vec2){300.f, 300.f}, (Vec2){100.f, 100.f},
                                    (Vec4){1.f, 1.f, 1.f, 1.f}, 0.f, (Vec2){0.f,0.f},
                                    500.f);
                r2_gl_render_wavy_text(&gfx, a,
                                       "the quick brown fox jumped \nover the lazy dog",
                                       (Vec2){100, 500}, 0.25, (Vec2){0.4, 1.f},
                                       (Vec4){1.f, 1.f, 1.f, 1.f}, 0.f, 5.f, 2.f,
                                       SDL_GetTicks() / 500.f);
                //
                shp.vertices[1] = sin(SDL_GetTicks() / 1000.f) * 0.5f + 0.5f;
                shp.vertices[9] = cos(SDL_GetTicks() / 1000.f) * 0.5f + 0.5f;
                shp.vertices[13] = 0.2+ cos(SDL_GetTicks() / 1000.f) * 0.5f + 1.0f;
                r2_gl_shape_update(&gfx, &shp);
                r2_gl_render_shape(&gfx, &shp, &gfx.colshd, (Vec2){100.f, 100.f},
                                                            (Vec2){100.f, 100.f},
                                                            0.f, (Vec2){0.f,0.f});
                //
                r2_gl_unbind_framebuffer(&fb);
                r2_gl_render_framebuffer(&gfx, &fb.tex);
                SDL_GL_SwapWindow(win);
        }
        r2_font_destroy(a);
        r2_font_gl_destroy(a);
        r2_tex_destroy(&image1);
        r2_tex_gl_destroy(&image1);
        r2_tex_destroy(&image2);
        r2_tex_gl_destroy(&image2);
        r2_gl_shape_destroy(&shp);
        r2_gl_delete_framebuffer(&fb);
        r2_gl_destroy(&gfx);
        exit(0);
}
