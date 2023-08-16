#include "pch.h"

struct GameLooper : Engine<GameLooper> {
    FpsViewer fv;

    void Init() {
        w = 1280;
        h = 720;
        tasks.Add([this]()->xx::Task<>{
            // inits
            auto secs = xx::NowSteadyEpochSeconds();
            for (int i = 0; i < 100; ++i) {
                fv.Init();
            }
            std::cout << xx::NowSteadyEpochSeconds(secs) << std::endl;
            co_return;
        });
    }

    void Draw() {

        // draw fps
        fv.Draw(delta, { -w / 2, -h / 2}, shader);
    }
};

inline GameLooper gLooper;

int main() {
    emscripten_request_animation_frame_loop([](double ms, void*)->EM_BOOL {
        return gLooper.JsLoopCallback(ms);
    }, nullptr);
}
