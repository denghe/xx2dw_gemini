#include "pch.h"

struct GameLooper : Engine<GameLooper> {
    std::array<xx::Shared<GLTexture>, 256> charTexs;

    xx::Shared<GLTexture> texTree, texTiles;
    int numOfUnloadedTexs{2};

    void Init() {
        w = 1280;
        h = 720;
        tasks.Add([this]()->xx::Task<>{
            int charSize = 64;
            for (size_t c = 0; c < 256; ++c) {
                charTexs[c] = xx::Make<GLTexture>(GLGenTextures<false>(), charSize, charSize * 1.5, std::to_string(c));
                upload_unicode_char_to_texture(c, charSize, false);
            }
            tasks.Add([this]()->xx::Task<>{
                texTree = co_await AsyncLoadTextureFromUrl("res/tree.png");
                --numOfUnloadedTexs;
            });
            tasks.Add([this]()->xx::Task<>{
                texTiles = co_await AsyncLoadTextureFromUrl("res/tiles.png");
                --numOfUnloadedTexs;
            });
            while (numOfUnloadedTexs > 0) co_yield 0;
            std::cout << "all tex loaded\n";
        });
    }

    void Draw() {
        if (numOfUnloadedTexs > 0) return;
        Quad()
        .SetTexture(texTree).Draw(shader)
        .SetTexture(texTiles).Draw(shader)
        .SetTexture(charTexs[65]).Draw(shader);
    }
};

inline GameLooper gLooper;

int main() {
    emscripten_request_animation_frame_loop([](double ms, void*)->EM_BOOL {
        return gLooper.JsLoopCallback(ms);
    }, nullptr);
}
