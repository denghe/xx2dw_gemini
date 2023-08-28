#include "pch.h"
#include "main.h"

// refine from https://github.com/Relintai/texture_packer/tree/master/rectpack2D
namespace RectPacker2D {

	struct LTRB {
		int l{}, t{}, r{}, b{};
		int w() const { return r - l; }
		void w(int v) { r = l + v; }
		int h() const { return b - t; }
		void h(int v) { b = t + v; }
		int Area() const { return w() * h(); }
		int Perimeter() const { return 2 * w() + 2 * h(); }
	};

	struct WH {
		int w{}, h{};
		int Area() { return w * h; }
		int Perimeter() { return 2 * w + 2 * h; }
		// 0 - no, 1 - yes, 2 - flipped, 3 - perfectly, 4 perfectly flipped
		int Fits(const WH& r, bool allowFlip) const {
			if (w == r.w && h == r.h) return 3;
			if (allowFlip && h == r.w && w == r.h) return 4;
			if (w <= r.w && h <= r.h) return 1;
			if (allowFlip && h <= r.w && w <= r.h) return 2;
			return 0;
		}
	};

	struct XYWH : WH {
		int x{}, y{};
		XYWH& Set(const LTRB& r_) {
			x = r_.l;
			y = r_.t;
			b(r_.b);
			r(r_.r);
			return *this;
		}
		operator LTRB() {
			LTRB r_{ x, y, 0, 0 };
			r_.w(w);
			r_.h(h);
			return r_;
		}
		int b() const { return y + h; }
		void b(int p_bottom) { h = p_bottom - y; }
		int r() const { return x + w; }
		void r(int p_right) { w = p_right - x; }
	};

	struct XYWHF : XYWH {
		bool flipped{};
		void Flip() {
			flipped = !flipped;
			std::swap(w, h);
		}
		//Ref<AtlasTexture> atlas_texture;
		//Ref<Texture> original_texture;
	};

	struct Group {
		WH size;
		std::vector<XYWHF*> rects;
	};

	struct Node {
		struct PNode {
			Node* pn = nullptr;
			bool fill = false;

			void Set(int l, int t, int r, int b) {
				if (!pn) {
					pn = new Node(LTRB(l, t, r, b));
				} else {
					(*pn).rc = LTRB(l, t, r, b);
					(*pn).id = false;
				}
				fill = true;
			}
		};

		PNode c[2];
		LTRB rc;
		bool id = false;
		Node(LTRB rect_rc = LTRB()) : rc(rect_rc) {}

		void reset(const WH& r) {
			id = false;
			rc = LTRB(0, 0, r.w, r.h);
			delcheck();
		}

		Node* insert(XYWHF& img, bool allowFlip) {
			if (c[0].pn && c[0].fill) {
				if (auto newn = c[0].pn->insert(img, allowFlip)) return newn;
				return c[1].pn->insert(img, allowFlip);
			}

			if (id) return 0;
			int f = img.Fits(XYWH().Set(rc), allowFlip);

			switch (f) {
			case 0: return 0;
			case 1: img.flipped = false; break;
			case 2: img.flipped = true; break;
			case 3:
				id = true;
				img.flipped = false;
				return this;
			case 4:
				id = true;
				img.flipped = true;
				return this;
			}

			int iw = (img.flipped ? img.h : img.w), ih = (img.flipped ? img.w : img.h);

			if (rc.w() - iw > rc.h() - ih) {
				c[0].Set(rc.l, rc.t, rc.l + iw, rc.b);
				c[1].Set(rc.l + iw, rc.t, rc.r, rc.b);
			} else {
				c[0].Set(rc.l, rc.t, rc.r, rc.t + ih);
				c[1].Set(rc.l, rc.t + ih, rc.r, rc.b);
			}

			return c[0].pn->insert(img, allowFlip);
		}

		void delcheck() {
			if (c[0].pn) {
				c[0].fill = false;
				c[0].pn->delcheck();
			}
			if (c[1].pn) {
				c[1].fill = false;
				c[1].pn->delcheck();
			}
		}

		~Node() {
			if (c[0].pn) delete c[0].pn;
			if (c[1].pn) delete c[1].pn;
		}
	};


	/***************************************************************************************************************************/

	constexpr int discard_step = 128;

	inline bool Area(XYWHF* a, XYWHF* b) { return a->Area() > b->Area(); }
	inline bool Perimeter(XYWHF* a, XYWHF* b) { return a->Perimeter() > b->Perimeter(); }
	inline bool MaxSide(XYWHF* a, XYWHF* b) { return std::max(a->w, a->h) > std::max(b->w, b->h); }
	inline bool MaxWidth(XYWHF* a, XYWHF* b) { return a->w > b->w; }
	inline bool MaxHeight(XYWHF* a, XYWHF* b) { return a->h > b->h; }
	constexpr bool (*cmpf[])(XYWHF*, XYWHF*) = {
		Area,
		Perimeter,
		MaxSide,
		MaxWidth,
		MaxHeight
	};


	inline WH Calc(XYWHF* const* v, int n, int max_s, bool allowFlip, std::vector<XYWHF*>& succ, std::vector<XYWHF*>& unsucc) {
		Node root;

		constexpr int numFuncs = (sizeof(cmpf) / sizeof(bool (*)(XYWHF*, XYWHF*)));

		XYWHF** order[numFuncs];

		for (int f = 0; f < numFuncs; ++f) {
			order[f] = new XYWHF * [n];
			std::memcpy(order[f], v, sizeof(XYWHF*) * n);
			std::sort(order[f], order[f] + n, cmpf[f]);
		}

		WH min_bin = WH(max_s, max_s);
		int min_func = -1, best_func = 0, best_area = 0, _area = 0, step, fit, i;

		bool fail = false;

		for (int f = 0; f < numFuncs; ++f) {
			v = order[f];
			step = min_bin.w / 2;
			root.reset(min_bin);

			while (true) {
				if (root.rc.w() > min_bin.w) {
					if (min_func > -1) break;
					_area = 0;

					root.reset(min_bin);
					for (i = 0; i < n; ++i) {
						if (root.insert(*v[i], allowFlip)) {
							_area += v[i]->Area();
						}
					}

					fail = true;
					break;
				}

				fit = -1;

				for (i = 0; i < n; ++i) {
					if (!root.insert(*v[i], allowFlip)) {
						fit = 1;
						break;
					}
				}

				if (fit == -1 && step <= discard_step) break;

				root.reset(WH(root.rc.w() + fit * step, root.rc.h() + fit * step));

				step /= 2;
				if (!step) {
					step = 1;
				}
			}

			if (!fail && (min_bin.Area() >= root.rc.Area())) {
				min_bin = WH{ root.rc.w(), root.rc.h() };
				min_func = f;
			} else if (fail && (_area > best_area)) {
				best_area = _area;
				best_func = f;
			}
			fail = false;
		}

		v = order[min_func == -1 ? best_func : min_func];

		int clip_x = 0, clip_y = 0;

		root.reset(min_bin);

		for (i = 0; i < n; ++i) {
			if (auto ret = root.insert(*v[i], allowFlip)) {
				v[i]->x = ret->rc.l;
				v[i]->y = ret->rc.t;

				if (v[i]->flipped) {
					v[i]->flipped = false;
					v[i]->Flip();
				}

				clip_x = std::max(clip_x, ret->rc.r);
				clip_y = std::max(clip_y, ret->rc.b);

				succ.push_back(v[i]);
			} else {
				unsucc.push_back(v[i]);

				v[i]->flipped = false;
			}
		}

		for (int f = 0; f < numFuncs; ++f)
			delete[] order[f];

		return WH(clip_x, clip_y);
	}


	bool Pack(XYWHF* const* v, int n, int max_s, bool allowFlip, std::vector<Group>& groups) {
		WH _rect(max_s, max_s);

		for (int i = 0; i < n; ++i) {
			if (!v[i]->Fits(_rect, allowFlip)) return false;
		}

		std::vector<XYWHF*> vec[2], * p[2] = { vec, vec + 1 };
		vec[0].resize(n);
		vec[1].clear();
		std::memcpy(&vec[0][0], v, sizeof(XYWHF*) * n);

		while (true) {
			auto& b = groups.emplace_back();

			b.size = Calc(&((*p[0])[0]), int(p[0]->size()), max_s, allowFlip, b.rects, *p[1]);
			p[0]->clear();

			if (!p[1]->size()) break;

			std::swap(p[0], p[1]);
		}

		return true;
	}

}






























int main() {
	{
		// test rect pack
		using namespace RectPacker2D;
		std::vector<XYWHF> rects;
		rects.push_back(XYWHF{ 0, 0, 100, 100 });
		rects.push_back(XYWHF{ 0, 0, 100, 100 });
		rects.push_back(XYWHF{ 0, 0, 100, 100 });
		// todo fill some rect
		std::vector<XYWHF*> rectptrs;
		for (auto& o : rects) rectptrs.push_back(&o);
		std::vector<Group> groups;
		auto b = Pack(rectptrs.data(), rects.size(), 2048, false, groups);
		printf("%d %d", (int)b, (int)groups.size());
		// todo: more print
	}


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
