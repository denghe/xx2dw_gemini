#include "pch.h"

int main();

struct Bunny;
struct GameLooper : Engine<GameLooper> {
    CharTexCache<24,32,32,2048,2048> ctc24;
    TexBatcher<2048, 2048> tb;
    FpsViewer fv;
    std::vector<Bunny> bunnies;
    bool ready{};

	void Init();
	void Update();
	xx::Task<> MainTask();
	void Draw();
};
extern GameLooper gLooper;


struct Bounds {
    float left, right, top, bottom;
};

struct Bunny : Quad {
    xx::Shared<Bounds> bounds;
    XY speed;
    float gravity{};
    Bunny(xx::Shared<Frame> frame_, xx::Shared<Bounds> bounds_);
    void Update();
};
