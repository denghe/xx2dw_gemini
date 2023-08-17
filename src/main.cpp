#include "pch.h"

struct CharTexInfo {
    xx::Shared<GLTexture> tex;
    Rect rect;
};

struct CharTexCache {
    constexpr static float texWidth = 4096, texHeight = 4096;
    std::vector<xx::Shared<GLTexture>> texs;
    float px{}, py{};

    static constexpr int charSize = 32, canvasWidth = charSize * 1.2, canvasHeight = charSize * 1.2;
    std::array<CharTexInfo, 256> bases;
    std::unordered_map<char32_t, CharTexInfo> extras;

    // need ogl frame env
    void Init() {
        init_gCanvas(canvasWidth, canvasHeight);
        
        texs.emplace_back(FrameBuffer::MakeTexture({ (uint32_t)texWidth, (uint32_t)texHeight }));

        char buf[16];
        for (char32_t c = 0; c < 256; ++c) {
            MakeCharInfo(c);
        }
    }

    CharTexInfo& MakeCharInfo(char32_t c) {
        auto& t = texs.back();

        char buf[16];
        buf[xx::Char32ToUtf8(c, buf)] = '\0';
        auto ct = xx::Make<GLTexture>(GLGenTextures<false>(), canvasWidth, canvasHeight, std::to_string((int)c));




        //CharTexInfo ci;
        //ci.rect.wh.x = upload_unicode_char_to_texture(charSize, buf);
        //ci.rect.wh.y = canvasHeight;

        
        //if (c < 256) {
        //    bases[c] = std::move(ci);
        //    return bases[c];
        //} else {
        //    auto rtv = extras.insert(std::make_pair(c, std::move(ci)));
        //    return rtv.first->second;
        //}
    }

    CharTexInfo& Find(char32_t c) {
        CharTexInfo* ci;
        if (c < 256) {
            return bases[c];
        } else {
            if (auto iter = extras.find(c); iter != extras.end()) {
                return iter->second;
            } else {
                return MakeCharInfo(c);
            }
        }
    }

};

struct GameLooper : Engine<GameLooper> {
    CharPainter cp;
    FpsViewer fv;
    xx::Shared<GLTexture> tex;
    std::u32string_view str = U"🚫💘💓💔💕💖💗💙💚💛💜💝💞💟"sv;
    float strWidth{};

    void Init() {
        w = 1280;
        h = 720;
        tasks.Add([this]()->xx::Task<>{
            cp.Init();

            // fill tex
            strWidth = cp.Measure(str);
            tex = FrameBuffer::MakeTexture({(uint32_t)strWidth, (uint32_t)cp.canvasHeight});
            FrameBuffer(true).DrawTo(tex, RGBA8{111,111,111,111}, [&]{
                cp.Draw({-strWidth / 2, 0}, str);
            });

            co_return;
        });
    }

    void Draw() {
        // draw tex
        Quad().SetTexture(tex).Draw();

        cp.Draw({ -strWidth / 2, 100 }, U"🚫💘💓💔💕💖💗💙💚💛💜💝💞💟");

        // draw fps
        fv.Draw(cp);
    }
};

inline GameLooper gLooper;

int main() {
    emscripten_request_animation_frame_loop([](double ms, void*)->EM_BOOL {
        return gLooper.JsLoopCallback(ms);
    }, nullptr);
}
