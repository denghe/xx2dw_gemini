#include "pch.h"

template<typename T> concept Has_Init = requires(T t) { t.Init(); };
template<typename T> concept Has_Init_ = requires(T t) { t.Init_(); };
template<typename T> concept Has_Update = requires(T t) { t.Update(); };
template<typename T> concept Has_Draw = requires(T t) { t.Draw(); };

template<typename Derived>
struct Engine {
    float fps = 60, frameDelay = 1.f / fps, maxFrameDelay = frameDelay * 3;
    float w = 800, h = 600;

    double dpr = emscripten_get_device_pixel_ratio();
    double nowSecs{}, delta{};                        // fill by js callback

    double timePool{};
    int frameNumber{};
    EM_BOOL running{ EM_TRUE };
    xx::Tasks tasks;

    EM_BOOL JsLoopCallback() {
        if (delta > maxFrameDelay) {
            delta = maxFrameDelay;
        }
        timePool += delta;
        while (timePool >= frameDelay) {
            timePool -= frameDelay;
            ++frameNumber;
            if constexpr(Has_Update<Derived>) {
                ((Derived*)this)->Update();
            }
            tasks();
        }
        if constexpr(Has_Draw<Derived>) {
            ((Derived*)this)->Draw();
        }
        return running;
    }

    Engine() {
        if constexpr(Has_Init<Derived>) {
            ((Derived*)this)->Init();
        }
        // todo: init js, gl
        if constexpr(Has_Init_<Derived>) {
            ((Derived*)this)->Init_();
        }
    }
};


struct GameLooper : Engine<GameLooper> {
    void Init() {
        w = 1280;
        h = 720;
        tasks.Add(MainLogic());
    }
    xx::Task<> MainLogic() {
        // todo: logic here
        for (int i = 0; i < 120; ++i) {
            std::cout << nowSecs << " : " << frameNumber << std::endl;
            co_yield 0;
        }
        running = false;
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
