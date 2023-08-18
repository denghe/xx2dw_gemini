#include "pch.h"

struct GameLooper : Engine<GameLooper> {
    CharTexCache ctc;
    FpsViewer fv;

    void Init() {
        w = 600, h = 300;
    }

    xx::Task<> MainTask() {
        ctc.Init();
        co_return;
    }

    void Draw() {
        auto str = U"🚫💘💓💔💕💖💗💙💚💛💜💝💞💟"sv;
        auto width = ctc.Measure(str);
        ctc.Draw({ -width / 2, 0 }, str);        // draw text at center

        fv.Draw(ctc);       // draw fps at corner
    }
};

GameLooper gLooper;
int main() {
    emscripten_request_animation_frame_loop([](double ms, void*)->EM_BOOL {
        return gLooper.JsLoopCallback(ms);
    }, nullptr);
}
