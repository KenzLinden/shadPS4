#include "emulator.h"

#include "Core/PS4/HLE/Graphics/video_out.h"

namespace Emulator {

static WindowCtx* g_window_ctx = nullptr;

void emuInit(u32 width, u32 height) {
    g_window_ctx = new WindowCtx;

    g_window_ctx->m_graphic_ctx.screen_width = width;
    g_window_ctx->m_graphic_ctx.screen_height = height;
}

static void CreateSdlWindow(WindowCtx* ctx) {
    int width = static_cast<int>(ctx->m_graphic_ctx.screen_width);
    int height = static_cast<int>(ctx->m_graphic_ctx.screen_height);

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("%s\n", SDL_GetError());
        std::_Exit(0);
    }

    ctx->m_window = SDL_CreateWindowWithPosition("shadps4", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height,
                                     (static_cast<uint32_t>(SDL_WINDOW_HIDDEN) | static_cast<uint32_t>(SDL_WINDOW_VULKAN)));

    ctx->is_window_hidden = true;//hide window until we need to show something (should draw something in buffers)

    if (ctx->m_window == nullptr) {
        printf("%s\n", SDL_GetError());
        std::_Exit(0);
    }

    SDL_SetWindowResizable(ctx->m_window, SDL_FALSE);//we don't support resizable atm
    SDL_ShowWindow(g_window_ctx->m_window);//TODO should be removed just left it over to make it fancy :D
}
void emuRun() {
    g_window_ctx->m_mutex.LockMutex();
    {
        // init window and wait until init finishes
        CreateSdlWindow(g_window_ctx);
        g_window_ctx->m_is_graphic_initialized = true;
        g_window_ctx->m_graphic_initialized_cond.SignalCondVar();
    }
    g_window_ctx->m_mutex.UnlockMutex();

    for (;;) {
        HLE::Libs::Graphics::VideoOut::videoOutFlip(100000);  // flip every 0.1 sec
    }
    std::_Exit(0);
}
}  // namespace Emulator