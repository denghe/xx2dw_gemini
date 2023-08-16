#include "pch.h"

struct GameLooper : Engine<GameLooper> {
    CharPainter cp;
    FpsViewer fv;
    xx::Shared<GLTexture> tex;

    void Init() {
        w = 1280;
        h = 720;
        tasks.Add([this]()->xx::Task<>{
            cp.Init();

            // fill tex
            auto str = U"💘💓💔💕💖💗💙💚💛💜💝💞💟";
            auto strWidth = cp.Measure(this, str);
            tex = FrameBuffer::MakeTexture({(uint32_t)strWidth, 128});
            FrameBuffer(true).DrawTo(this, tex, RGBA8{111,111,111,111}, [&]{
                cp.Draw(this, {-strWidth / 2, 0}, str);
            });

            co_return;
        });
    }

    void Draw() {
        // draw tex
        Quad().SetTexture(tex).Draw(this);

        // draw fps
        fv.Draw(this, cp, delta, { -w / 2, -h / 2});
    }
};

inline GameLooper gLooper;

int main() {
    emscripten_request_animation_frame_loop([](double ms, void*)->EM_BOOL {
        return gLooper.JsLoopCallback(ms);
    }, nullptr);
}
