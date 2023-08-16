#include "pch.h"

struct Foo {
    char32_t c;
    XY pos;
};

struct GameLooper : Engine<GameLooper> {
    CharPainter cp;
    FpsViewer fv;
    std::vector<Foo> foos;

    void Init() {
        w = 1280;
        h = 720;
        tasks.Add([this]()->xx::Task<>{
            // inits
            cp.Init();

            // init foos
            foos.resize(10000);
            for(auto& foo : foos) {
                foo.c = rand() % 256;
                foo.pos = { float(rand() % (int)w), float(rand() % (int)h) };
            }

            co_return;
        });
    }

    void Draw() {
        Quad q;
        q.SetColor({127,127,127,127});
        XY basePos{ -w / 2, -h / 2 };
        for(auto& foo : foos) {
            q.SetTexture(cp.Find(foo.c).tex).SetPosition(basePos + foo.pos).Draw(this);
        }

        // draw fps
        fv.Draw(this, cp, delta, { -w / 2, -h / 2});
    }
};

inline GameLooper gLooper;

int main() {
    emscripten_request_animation_frame_loop([](double ms, void*)->EM_BOOL {
        return gLooper.JsLoopCallback(ms);
    }, nullptr);
}

/*
struct
    xx::Shared<GLTexture> texTree, texTiles;
    int numOfUnloadedTexs{2};
task
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

draw
    if (numOfUnloadedTexs > 0) {
        // todo: display "loading..." ?
        return;
    }
*/
