#include "pch.h"

struct GameLooper : Engine<GameLooper> {
    Quad q;
    bool readyForDraw = false;

    void Init() {
        tasks.Add([this]()->xx::Task<> {
            auto tex = co_await AsyncLoadTextureFromUrl("res/tree.png");
            q.SetTexture( tex );
            readyForDraw = true;
        });
    }

    void Draw() {
        if (!readyForDraw) return;
        q.Draw(shader);
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


//            for (int i = 0; i < 1200; ++i) {
//                std::cout << nowSecs << " : " << frameNumber << std::endl;
//                co_yield 0;
//            }
//            running = false;
