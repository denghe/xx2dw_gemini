#include "pch.h"

struct Foo {
    XY pos;
};

struct GameLooper : Engine<GameLooper> {
    FpsViewer fv;

    xx::Shared<GLTexture> texTree, texTiles;
    int numOfUnloadedTexs{2};

    std::vector<Foo> foos;

    void Init() {
        w = 1280;
        h = 720;
        tasks.Add([this]()->xx::Task<>{
            // inits
            fv.Init();

            // async load pngs
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

            // init foos
            foos.resize(400000);
            for(auto& foo : foos) {
                foo.pos = { float(rand() % (int)w), float(rand() % (int)h) };
            }
        });
    }

    void Draw() {
        if (numOfUnloadedTexs > 0) {
            // todo: display "loading..." ?
            return;
        }

        Quad q;
        q.SetTexture(texTree);
        XY basePos{ -w / 2, -h / 2 };
        for(auto& foo : foos) {
            q.SetPosition(basePos + foo.pos).Draw(shader);
        }

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
