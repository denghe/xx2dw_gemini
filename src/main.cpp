#include "pch.h"

struct GameLooper : Engine<GameLooper> {
    CharPainter cp;
    FpsViewer fv;

    void Init() {
        w = 1280;
        h = 720;
        tasks.Add([this]()->xx::Task<>{
            cp.Init();

            co_return;
        });
    }

    void Draw() {
        cp.Draw(U"💘💓💔💕💖💗💙💚💛💜💝💞💟", {}, shader);

        // draw fps
        fv.Draw(cp, delta, { -w / 2, -h / 2}, shader);
    }
};

inline GameLooper gLooper;

int main() {
    emscripten_request_animation_frame_loop([](double ms, void*)->EM_BOOL {
        return gLooper.JsLoopCallback(ms);
    }, nullptr);
}
