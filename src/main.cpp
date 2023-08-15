#include "pch.h"

struct GameLooper : Engine<GameLooper> {
    Quad q;

    void Init() {
        q.SetTexture( xx::Make<GLTexture>( LoadTextureFromUrl("res/moon.png") ) );

        tasks.Add([this]()->xx::Task<> {
            // todo
            for (int i = 0; i < 120; ++i) {
                std::cout << nowSecs << " : " << frameNumber << std::endl;
                co_yield 0;
            }
            running = false;
        });
    }


    void Draw() {
        // todo
    }
};

inline GameLooper gLooper;

/**********************************************************************************************************************************/
/**********************************************************************************************************************************/

int main() {
    emscripten_request_animation_frame_loop([](double nowMS, void*)->EM_BOOL {
        auto nowSecs = nowMS / 1000;
        gLooper.delta = nowSecs - gLooper.nowSecs;
        gLooper.nowSecs = nowSecs;
        return gLooper.JsLoopCallback();
    }, nullptr);
}
