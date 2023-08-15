#include "pch.h"

#define go tasks.Add([this]()->xx::Task<>

struct GameLooper : Engine<GameLooper> {
    xx::Shared<GLTexture> texTree, texTiles;
    int numOfUnloadedTexs{2};

    void Init() {
        w = 1280;
        h = 720;
        go {
            go {
                texTree = co_await AsyncLoadTextureFromUrl("res/tree.png");
                --numOfUnloadedTexs;
            });
            go {
                texTiles = co_await AsyncLoadTextureFromUrl("res/tiles.png");
               --numOfUnloadedTexs;
            });
            while (numOfUnloadedTexs > 0) co_yield 0;
            std::cout << "all tex loaded\n";
        });
    }

    void Draw() {
        if (numOfUnloadedTexs > 0) return;
        Quad().SetTexture(texTree).Draw(shader)
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
