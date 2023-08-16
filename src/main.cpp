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


//#include "pch.h"
//
//struct GameLooper : Engine<GameLooper> {
//    CharPainter cp;
//    FpsViewer fv;
//    xx::Shared<GLTexture> tex;
//
//    void Init() {
//        w = 1280;
//        h = 720;
//        tasks.Add([this]()->xx::Task<>{
//            cp.Init();
//
//            // fill tex
//            auto str = U"💘💓💔💕💖💗💙💚💛💜💝💞💟";
//            auto strWidth = cp.Measure(this, str);
//            tex = FrameBuffer::MakeTexture({(uint32_t)strWidth, 128});
//            FrameBuffer(true).DrawTo(this, tex, RGBA8{111,111,111,111}, [&]{
//                cp.Draw(this, {-strWidth / 2, 0}, str);
//            });
//
//            co_return;
//        });
//    }
//
//    void Draw() {
//        // draw tex
//        Quad().SetTexture(tex).Draw(this);
//
//        // draw fps
//        fv.Draw(this, cp, delta, { -w / 2, -h / 2});
//    }
//};
//
//inline GameLooper gLooper;
//
//int main() {
//    emscripten_request_animation_frame_loop([](double ms, void*)->EM_BOOL {
//        return gLooper.JsLoopCallback(ms);
//    }, nullptr);
//}
