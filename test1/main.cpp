#include "pch.h"
#include "main.h"

int main() {
	printf("main()\n");
    emscripten_request_animation_frame_loop([](double ms, void*)->EM_BOOL {
        return gLooper.JsLoopCallback(ms);
        }, nullptr);
}
GameLooper gLooper;


void GameLooper::Init() {
    printf("GameLooper::Init()\n");
    w = 1280;
    h = 720;
}

xx::Task<> GameLooper::MainTask() {
    printf("GameLooper::MainTask()\n");
    ctc24.Init();
    tb.Init();

    constexpr int maxCount = 1'000'000, amount = 5'000;
    int count = 0;
    bunnies.reserve(maxCount);

    auto frames = co_await AsyncLoadBatchFramesFromUrls(tb, {
        "res/rabbitv3.png"
        , "res/rabbitv3_ash.png"
        , "res/rabbitv3_batman.png"
        , "res/rabbitv3_bb8.png"
        , "res/rabbitv3_frankenstein.png"
        , "res/rabbitv3_neo.png"
        , "res/rabbitv3_sonic.png"
        , "res/rabbitv3_spidey.png"
        , "res/rabbitv3_stormtrooper.png"
        , "res/rabbitv3_superman.png"
        , "res/rabbitv3_tron.png"
        , "res/rabbitv3_wolverine.png"
    });
    ready = true;
    
    auto bounds = xx::Make<Bounds>(Bounds{ -w/2, w/2, -h/2, h/2 });
    while (count < maxCount) {
        for (int i = 0; i < amount; i++) {
            auto&& b = bunnies.emplace_back(frames[count % frames.size()], bounds);
            b.pos.x = (count % 2) * w - w / 2;
            ++count;
        }
        co_yield 0;
    }
}

void GameLooper::Update() {
	for (auto& o : bunnies) { o.Update(); }
}

void GameLooper::Draw() {
    if (ready) {
        for (auto& o : bunnies) { o.Draw(); }
    }
	fv.Draw(ctc24);       // draw fps at corner
}

Bunny::Bunny(xx::Shared<Frame> frame_, xx::Shared<Bounds> bounds_) {
    SetFrame(frame_);
    gravity = 0.75;
    speed.x = gLooper.rnd.Next<float>() * 10;
    speed.y = (gLooper.rnd.Next<float>() * 10) - 5;
    bounds = bounds_;
    anchor.x = 0.5;
    anchor.y = 1;
}

void Bunny::Update() {
    pos += speed;
    speed.y += gravity;

    if (pos.x > bounds->right) {
        speed.x *= -1;
        pos.x = bounds->right;
    } else if (pos.x < bounds->left) {
        speed.x *= -1;
        pos.x = bounds->left;
    }
    if (pos.y > bounds->bottom) {
        speed.y *= -0.85;
        pos.y = bounds->bottom;
        if (gLooper.rnd.Next<bool>()) {
            speed.y -= gLooper.rnd.Next<float>() * 6;
        }
    } else if (pos.y < bounds->top) {
        speed.y = 0;
        pos.y = bounds->top;
    }
}
