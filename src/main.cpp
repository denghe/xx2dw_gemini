#include "pch.h"

struct Engine {
    static constexpr float fps = 60, frameDelay = 1.f / fps, maxFrameDelay = frameDelay * 3;
    inline static double nowSecs{}, delta{};                        // fill by js callback
    double timePool{};
    int frameNumber{};
    EM_BOOL running{ EM_TRUE };
    xx::Tasks tasks;

    EM_BOOL Update() {
        if (delta > maxFrameDelay) {
            delta = maxFrameDelay;
        }
        timePool += delta;
        while (timePool >= frameDelay) {
            timePool -= frameDelay;
            ++frameNumber;
            FixedUpdate();
        }
        Draw();
        return running;
    }

    void FixedUpdate() {
        tasks();
    }

    Engine() {
        tasks.Add([this]()->xx::Task<>{
            // todo: logic here
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
Engine gEngine;

int main() {
    emscripten_request_animation_frame_loop([](double nowMS, void*)->EM_BOOL {
        auto nowSecs = nowMS / 1000;
        Engine::delta = nowSecs - Engine::nowSecs;
        Engine::nowSecs = nowSecs;
        return gEngine.Update();
    }, nullptr);
}
