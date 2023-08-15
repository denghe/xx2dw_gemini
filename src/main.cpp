#include "pch.h"

struct GameLooper : Engine<GameLooper> {
    xx::Shared<GLTexture> texTree, texTiles;
    bool readyForDraw = false;

    void Init() {
        w = 1280;
        h = 720;
        tasks.Add([this]()->xx::Task<> {
            texTree = co_await AsyncLoadTextureFromUrl("res/tree.png");
        });
        tasks.Add([this]()->xx::Task<> {
            texTiles = co_await AsyncLoadTextureFromUrl("res/tiles.png");
        });
        tasks.Add([this]()->xx::Task<> {
            do {
                co_yield 0;
            } while(!texTree || !texTiles);
            readyForDraw = true;
        });
    }

    void Draw() {
        if (!readyForDraw) return;
        Quad q;
        q.SetTexture(texTree).Draw(shader)
        .SetTexture(texTiles).Draw(shader);
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
