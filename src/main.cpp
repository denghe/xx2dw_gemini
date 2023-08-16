#include "pch.h"

struct GameLooper : Engine<GameLooper> {
    CharPainter cp;
    FpsViewer fv;
    xx::Shared<GLTexture> tex;
    std::u32string_view str = U"🚫💘💓💔💕💖💗💙💚💛💜💝💞💟"sv;
    float strWidth{};

    void Init() {
        w = 1280;
        h = 720;
        tasks.Add([this]()->xx::Task<>{
            cp.Init();

            // fill tex
            strWidth = cp.Measure(str);
            tex = FrameBuffer::MakeTexture({(uint32_t)strWidth, (uint32_t)cp.canvasHeight});
            FrameBuffer(true).DrawTo(tex, RGBA8{111,111,111,111}, [&]{
                cp.Draw({-strWidth / 2, 0}, str);
            });

            co_return;
        });
    }

    void Draw() {
        // draw tex
        Quad().SetTexture(tex).Draw();

        cp.Draw({ -strWidth / 2, 100 }, U"🚫💘💓💔💕💖💗💙💚💛💜💝💞💟");

        // draw fps
        fv.Draw(cp);
    }
};

inline GameLooper gLooper;

int main() {
    emscripten_request_animation_frame_loop([](double ms, void*)->EM_BOOL {
        return gLooper.JsLoopCallback(ms);
    }, nullptr);
}
 