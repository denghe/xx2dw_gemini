#include "pch.h"

struct GameLooper : Engine<GameLooper> {
    CharTexCache ctc;
    FpsViewer fv;
    XY mousePos;
    xx::Shared<TexturePacker> tp;

    EM_BOOL OnMouseMove(EmscriptenMouseEvent const& e) {
        mousePos = { (float)e.targetX - w / 2, h - (float)e.targetY - h / 2 };
        return EM_TRUE;
    }

    void Init() {
        w = 600, h = 300;
    }

    xx::Task<> MainTask() {
        ctc.Init();

        tp = co_await AsyncLoadTexturePackerFromUrl("res/gemini.plist");
        xx_assert(tp);

        co_return;
    }

    void Draw() {
        if (tp) {   // wait async load
            Quad().SetTexture(tp->tex).Draw();
        }
        // draw text at mouse pos
        ctc.Draw(mousePos, U"🚫💘💓💔💕💖💗💙💚💛💜💝💞💟"sv);
        fv.Draw(ctc);       // draw fps at corner
    }
};

GameLooper gLooper;
int main() {
    emscripten_request_animation_frame_loop([](double ms, void*)->EM_BOOL {
        return gLooper.JsLoopCallback(ms);
    }, nullptr);
}
