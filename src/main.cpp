#include "pch.h"

struct GameLooper : Engine<GameLooper> {
    Quad q;

    void Init() {
        tasks.Add([this]()->xx::Task<> {
            auto&& tex = co_await AsyncLoadTextureFromUrl("res/tree.png");
            if (!tex) co_return;
            q.SetTexture( tex );

            for (int i = 0; i < 120; ++i) {
                std::cout << nowSecs << " : " << frameNumber << std::endl;
                co_yield 0;
            }
            running = false;
        });
    }

    void Draw() {
        if (q.tex) {
            q.Draw(shader);
        }
    }
};

inline GameLooper gLooper;

int main() {
    emscripten_request_animation_frame_loop([](double nowMS, void*)->EM_BOOL {
        auto nowSecs = nowMS / 1000;
        gLooper.delta = nowSecs - gLooper.nowSecs;
        gLooper.nowSecs = nowSecs;
        return gLooper.JsLoopCallback();
    }, nullptr);
}
