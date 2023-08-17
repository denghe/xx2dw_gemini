#include "pch.h"

struct GameLooper : Engine<GameLooper> {
    CharTexCache ctc;
    FpsViewer fv;

    void Init() {
        w = 600;
        h = 300;
        tasks.Add([this]()->xx::Task<>{
            ctc.Init();
            co_return;
        });
    }

    void Draw() {
        // draw text
        std::u32string_view str = U"🚫💘💓💔💕💖💗💙💚💛💜💝💞💟"sv;
        auto strWidth = ctc.Measure(str);
        ctc.Draw({ -strWidth / 2, 0 }, str);

        // draw fps
        fv.Draw(ctc);
    }
};

inline GameLooper gLooper;

int main() {
    emscripten_request_animation_frame_loop([](double ms, void*)->EM_BOOL {
        return gLooper.JsLoopCallback(ms);
    }, nullptr);
}
