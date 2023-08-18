#include "pch.h"

struct GameLooper : Engine<GameLooper> {
    CharTexCache ctc;
    FpsViewer fv;
    XY mousePos;

    EM_BOOL OnMouseMove(EmscriptenMouseEvent const& e) {
        mousePos = { (float)e.targetX - w / 2, h - (float)e.targetY - h / 2 };
        return EM_TRUE;
    }

    void Init() {
        w = 600, h = 300;
    }

    xx::Task<> MainTask() {
        ctc.Init();
        co_return;
    }

    void Draw() {
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
