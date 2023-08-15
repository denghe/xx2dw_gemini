#pragma once
#ifndef XX2DW_GEMINI_PCH_H_
#define XX2DW_GEMINI_PCH_H_

#include <xx_task.h>
#include <xx_queue.h>

#include <emscripten/emscripten.h>
#include <emscripten/html5.h>
#include <webgl/webgl2.h>

// code at library_js.js
extern "C" {
void upload_unicode_char_to_texture(int unicodeChar, int charSize, int applyShadow);
void load_texture_from_url(GLuint texture, const char *url, int *outWidth, int *outHeight);
}

/**********************************************************************************************************************************/
/**********************************************************************************************************************************/

#ifndef NDEBUG
inline void CheckGLErrorAt(const char* file, int line, const char* func) {
		if (auto e = glGetError(); e != GL_NO_ERROR) {
			throw std::runtime_error(std::string("OpenGL error: ") + file + std::to_string(line) + func);
		}
	}
#define CheckGLError() CheckGLErrorAt(__FILE__, __LINE__, __FUNCTION__)
#else
#define CheckGLError() ((void)0)
#endif

/**********************************************************************************************************************************/
/**********************************************************************************************************************************/

enum class GLResTypes {
    Shader, Program, VertexArrays, Buffer, Texture, FrameBuffer
};

template<GLResTypes T, typename...VS>
struct GLRes {

    std::tuple<GLuint, VS...> vs;

    GLRes(GLuint i) : vs(std::make_tuple(i)) {}

    template<typename...Args>
    GLRes(GLuint i, Args&&... args) : vs(std::make_tuple(i, std::forward<Args>(args)...)) {}

    operator GLuint const& () const { return std::get<0>(vs); }
    GLuint* Get() { return &std::get<0>(vs); }

    GLRes(GLRes const&) = delete;
    GLRes& operator=(GLRes const&) = delete;
    GLRes() = default;
    GLRes(GLRes&& o) noexcept {
        std::swap(vs, o.vs);
    }
    GLRes& operator=(GLRes&& o) noexcept {
        std::swap(vs, o.vs);
        return *this;
    }

    ~GLRes() {
        if (!std::get<0>(vs)) return;
        if constexpr (T == GLResTypes::Shader) {
            glDeleteShader(std::get<0>(vs));
        }
        if constexpr (T == GLResTypes::Program) {
            glDeleteProgram(std::get<0>(vs));
        }
        if constexpr (T == GLResTypes::VertexArrays) {
            glDeleteVertexArrays(1, &std::get<0>(vs));
        }
        if constexpr (T == GLResTypes::Buffer) {
            glDeleteBuffers(1, &std::get<0>(vs));
        }
        if constexpr (T == GLResTypes::Texture) {
            glDeleteTextures(1, &std::get<0>(vs));
        }
        if constexpr (T == GLResTypes::FrameBuffer) {
            glDeleteFramebuffers(1, &std::get<0>(vs));
        }
        std::get<0>(vs) = 0;
    }
};

using GLShader = GLRes<GLResTypes::Shader>;

using GLProgram = GLRes<GLResTypes::Program>;

using GLVertexArrays = GLRes<GLResTypes::VertexArrays>;

using GLBuffer = GLRes<GLResTypes::Buffer>;

using GLFrameBuffer = GLRes<GLResTypes::FrameBuffer>;

using GLTextureCore = GLRes<GLResTypes::Texture>;


template<GLuint filter = GL_NEAREST /* GL_LINEAR */, GLuint wraper = GL_CLAMP_TO_EDGE /* GL_REPEAT */>
void GLTexParameteri() {
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wraper);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wraper);
}

template<bool exitBind0 = false, GLuint filter = GL_NEAREST /* GL_LINEAR */, GLuint wraper = GL_CLAMP_TO_EDGE /* GL_REPEAT */>
GLuint GLGenTextures() {
    GLuint t{};
    glGenTextures(1, &t);
    glBindTexture(GL_TEXTURE_2D, t);
    GLTexParameteri<filter, wraper>();
    if constexpr(exitBind0) {
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    return t;
}

struct GLTexture : GLRes<GLResTypes::Texture, GLsizei, GLsizei, std::string> {
    using BT = GLRes<GLResTypes::Texture, GLsizei, GLsizei, std::string>;
    using BT::BT;

    template<GLuint filter = GL_NEAREST /* GL_LINEAR */, GLuint wraper = GL_CLAMP_TO_EDGE /* GL_REPEAT */>
    void SetGLTexParm() {
        glBindTexture(GL_TEXTURE_2D, std::get<0>(vs));
        GLTexParameteri<filter, wraper>();
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    auto const& Width() const { return std::get<1>(vs); }
    auto const& Height() const { return std::get<2>(vs); }
    auto const& FileName() const { return std::get<3>(vs); }
};


/**********************************************************************************************************************************/
/**********************************************************************************************************************************/

inline GLShader LoadGLShader(GLenum const& type, std::initializer_list<std::string_view>&& codes_) {
    assert(codes_.size() && (type == GL_VERTEX_SHADER || type == GL_FRAGMENT_SHADER));
    auto&& shader = glCreateShader(type);
    if (!shader)
        throw std::logic_error(std::string("glCreateShader(") + std::to_string(type) + ") failed.");
    std::vector<GLchar const*> codes;
    codes.resize(codes_.size());
    std::vector<GLint> codeLens;
    codeLens.resize(codes_.size());
    auto ss = codes_.begin();
    for (size_t i = 0; i < codes.size(); ++i) {
        codes[i] = (GLchar const*)ss[i].data();
        codeLens[i] = (GLint)ss[i].size();
    }
    glShaderSource(shader, (GLsizei)codes_.size(), codes.data(), codeLens.data());
    glCompileShader(shader);
    GLint r = GL_FALSE;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &r);
    if (!r) {
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &r);	// fill txt len into r
        std::string s;
        if (r) {
            s.resize(r);
            glGetShaderInfoLog(shader, r, nullptr, s.data());	// copy txt to s
        }
        throw std::logic_error("glCompileShader failed: err msg = " + s);
    }
    return GLShader(shader);
}

inline GLShader LoadGLVertexShader(std::initializer_list<std::string_view>&& codes_) {
    return LoadGLShader(GL_VERTEX_SHADER, std::move(codes_));
}

inline GLShader LoadGLFragmentShader(std::initializer_list<std::string_view>&& codes_) {
    return LoadGLShader(GL_FRAGMENT_SHADER, std::move(codes_));
}

inline GLProgram LinkGLProgram(GLuint const& vs, GLuint const& fs) {
    auto program = glCreateProgram();
    if (!program)
        throw std::logic_error("glCreateProgram failed.");
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    GLint r = GL_FALSE;
    glGetProgramiv(program, GL_LINK_STATUS, &r);
    if (!r) {
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &r);
        std::string s;
        if (r) {
            s.resize(r);
            glGetProgramInfoLog(program, r, nullptr, s.data());
        }
        throw std::logic_error("glLinkProgram failed: err msg = " + s);
    }
    return GLProgram(program);
}

/**********************************************************************************************************************************/
/**********************************************************************************************************************************/

inline GLFrameBuffer MakeGLFrameBuffer() {
    GLuint f{};
    glGenFramebuffers(1, &f);
    glBindFramebuffer(GL_FRAMEBUFFER, f);
    CheckGLError();
    return GLFrameBuffer(f);
}

template<GLuint filter = GL_NEAREST /* GL_LINEAR */, GLuint wraper = GL_CLAMP_TO_EDGE /* GL_REPEAT */>
GLTexture MakeGLFrameBufferTexture(uint32_t const& w, uint32_t const& h, bool const& hasAlpha) {
    auto t = GLGenTextures<false, filter, wraper>();
    auto c = hasAlpha ? GL_RGBA : GL_RGB;
    glTexImage2D(GL_TEXTURE_2D, 0, c, w, h, 0, c, GL_UNSIGNED_BYTE, {});
    glBindTexture(GL_TEXTURE_2D, 0);
    return GLTexture(t, w, h, "");
}

inline void BindGLFrameBufferTexture(GLuint const& f, GLuint const& t) {
    glBindFramebuffer(GL_FRAMEBUFFER, f);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, t, 0);
    assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
}

inline void UnbindGLFrameBuffer() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

/**********************************************************************************************************************************/
/**********************************************************************************************************************************/

template<typename T>
concept HasFieldXY = requires { T::x; T::y; };

template<typename T>
concept IsArithmetic = std::is_arithmetic_v<T>;

template<typename T = int32_t>
struct Vec2 {
    T x, y;

    // -x
    Vec2 operator-() const {
        return { -x, -y };
    }

    // + - * /
    Vec2 operator+(HasFieldXY auto const& v) const {
        return { T(x + v.x), T(y + v.y) };
    }
    Vec2 operator-(HasFieldXY auto const& v) const {
        return { T(x - v.x), T(y - v.y) };
    }
    Vec2 operator*(HasFieldXY auto const& v) const {
        return { T(x * v.x), T(y * v.y) };
    }
    Vec2 operator/(HasFieldXY auto const& v) const {
        return { T(x / v.x), T(y / v.y) };
    }

    Vec2 operator+(IsArithmetic auto const& v) const {
        return { T(x + v), T(y + v) };
    }
    Vec2 operator-(IsArithmetic auto const& v) const {
        return { T(x - v), T(y - v) };
    }
    Vec2 operator*(IsArithmetic auto const& v) const {
        return { T(x * v), T(y * v) };
    }
    Vec2 operator/(IsArithmetic auto const& v) const {
        return { T(x / v), T(y / v) };
    }

    // += -= *= /=
    Vec2& operator+=(HasFieldXY auto const& v) {
        x = T(x + v.x);
        y = T(y + v.y);
        return *this;
    }
    Vec2& operator-=(HasFieldXY auto const& v) {
        x = T(x - v.x);
        y = T(y - v.y);
        return *this;
    }
    Vec2& operator*=(HasFieldXY auto const& v) {
        x = T(x * v.x);
        y = T(y * v.y);
        return *this;
    }
    Vec2& operator/=(HasFieldXY auto const& v) {
        x = T(x / v.x);
        y = T(y / v.y);
        return *this;
    }

    Vec2& operator+=(IsArithmetic auto const& v) {
        x = T(x + v);
        y = T(y + v);
        return *this;
    }
    Vec2 operator-=(IsArithmetic auto const& v) {
        x = T(x - v);
        y = T(y - v);
        return *this;
    }
    Vec2& operator*=(IsArithmetic auto const& v) {
        x = T(x * v);
        y = T(y * v);
        return *this;
    }
    Vec2 operator/=(IsArithmetic auto const& v) {
        x = T(x / v);
        y = T(y / v);
        return *this;
    }

    // == !=
    bool operator==(HasFieldXY auto const& v) const {
        return x == v.x && y == v.y;
    }
    bool operator!=(HasFieldXY auto const& v) const {
        return x != v.x || y != v.y;
    }

    Vec2 MakeAdd(IsArithmetic auto const& vx, IsArithmetic auto const& vy) const {
        return { x + vx, y + vy };
    }

    void Set(HasFieldXY auto const& v) {
        x = T(v.x);
        y = T(v.y);
    }

    template<typename U = float>
    auto As() const -> Vec2<U> {
        return { (U)x, (U)y };
    }

    bool IsZero() const {
        return x == T{} && y == T{};
    }

    void Clear() {
        x = {};
        y = {};
    }

    template<typename U = float>
    Vec2& Normalize() {
        auto v = std::sqrt(U(x * x + y * y));
        assert(v);
        x = T(x / v);
        y = T(y / v);
        return *this;
    }
    template<typename R = T, typename U = float>
    auto MakeNormalize() -> Vec2<R> {
        auto v = std::sqrt(U(x * x + y * y));
        assert(v);
        return { R(x / v), R(y / v) };
    }

    Vec2& FlipY() {
        y = -y;
        return *this;
    }
    template<typename R = T>
    auto MakeFlipY() -> Vec2<R> {
        return { R(x), R(-y) };
    }
};

// pos
using XY = Vec2<float>;

// texture uv mapping pos
struct UV {
    uint16_t u, v;
};

// 4 bytes color
struct RGBA8 {
    uint8_t r, g, b, a;
    bool operator==(RGBA8 const&) const = default;
    bool operator!=(RGBA8 const&) const = default;
};

// 4 floats color
struct RGBA {
    float r, g, b, a;

    operator RGBA8() const {
        return { uint8_t(r * 255), uint8_t(g * 255), uint8_t(b * 255), uint8_t(a * 255) };
    }

    RGBA operator+(RGBA const& v) const {
        return { r + v.r, g + v.g, b + v.b, a + v.a };
    }
    RGBA operator-(RGBA const& v) const {
        return { r - v.r, g - v.g, b - v.b, a - v.a };
    }

    RGBA operator*(IsArithmetic auto const& v) const {
        return { r * v, g * v, b * v, a * v };
    }
    RGBA operator/(IsArithmetic auto const& v) const {
        return { r / v, g / v, b / v, a / v };
    }

    RGBA& operator+=(RGBA const& v) {
        r += v.r;
        g += v.g;
        b += v.b;
        a += v.a;
        return *this;
    }
};

// pos + size
struct Rect : XY {
    XY wh;
};



/**********************************************************************************************************************************/
/**********************************************************************************************************************************/

struct Shader {
    GLShader v, f;
    GLProgram p;
    Shader() = default;
    Shader(Shader const&) = delete;
    Shader& operator=(Shader const&) = delete;
};

struct QuadInstanceData {
    XY pos{}, anchor{ 0.5, 0.5 }, scale{ 1, 1 };
    float radians{};
    RGBA8 color{ 255, 255, 255, 255 };
    uint16_t texRectX{}, texRectY{}, texRectW{}, texRectH{};
};

struct Shader_QuadInstance : Shader {
    using Shader::Shader;
    GLint uCxy = -1, uTex0 = -1, aVert = -1, aPosAnchor = -1, aScaleRadians = -1, aColor = -1, aTexRect = -1;
    GLVertexArrays va;
    GLBuffer vb, ib;

    static const size_t maxQuadNums = 200000;
    GLuint lastTextureId = 0;
    std::unique_ptr<QuadInstanceData[]> quadInstanceDatas = std::make_unique<QuadInstanceData[]>(maxQuadNums);
    size_t quadCount = 0;

    void Init() {
        v = LoadGLVertexShader({ R"(#version 300 es
uniform vec2 uCxy;	// screen center coordinate

in vec2 aVert;	// fans index { 0, 0 }, { 0, 1.f }, { 1.f, 0 }, { 1.f, 1.f }

in vec4 aPosAnchor;
in vec3 aScaleRadians;
in vec4 aColor;
in vec4 aTexRect;

out vec2 vTexCoord;
out vec4 vColor;

void main() {
    vec2 pos = aPosAnchor.xy;
	vec2 anchor = aPosAnchor.zw;
    vec2 scale = vec2(aScaleRadians.x * aTexRect.z, aScaleRadians.y * aTexRect.w);
	float radians = aScaleRadians.z;
    vec2 offset = vec2((aVert.x - anchor.x) * scale.x, (aVert.y - anchor.y) * scale.y);

    float c = cos(radians);
    float s = sin(radians);
    vec2 v = pos + vec2(
       dot(offset, vec2(c, s)),
       dot(offset, vec2(-s, c))
    );

    gl_Position = vec4(v * uCxy, 0, 1);
	vColor = aColor;
	vTexCoord = vec2(aTexRect.x + aVert.x * aTexRect.z, aTexRect.y + aTexRect.w - aVert.y * aTexRect.w);
})"sv });

        f = LoadGLFragmentShader({ R"(#version 300 es
precision highp float;
uniform sampler2D uTex0;

in vec4 vColor;
in vec2 vTexCoord;

out vec4 oColor;

void main() {
	oColor = vColor * texture(uTex0, vTexCoord / vec2(textureSize(uTex0, 0)));
})"sv });

        p = LinkGLProgram(v, f);

        uCxy = glGetUniformLocation(p, "uCxy");
        uTex0 = glGetUniformLocation(p, "uTex0");

        aVert = glGetAttribLocation(p, "aVert");
        aPosAnchor = glGetAttribLocation(p, "aPosAnchor");
        aScaleRadians = glGetAttribLocation(p, "aScaleRadians");
        aColor = glGetAttribLocation(p, "aColor");
        aTexRect = glGetAttribLocation(p, "aTexRect");
        CheckGLError();

        glGenVertexArrays(1, va.Get());
        glBindVertexArray(va);

        glGenBuffers(1, (GLuint*)&ib);
        static const XY verts[4] = { { 0, 0 }, { 0, 1.f }, { 1.f, 0 }, { 1.f, 1.f } };
        glBindBuffer(GL_ARRAY_BUFFER, ib);
        glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
        glVertexAttribPointer(aVert, 2, GL_FLOAT, GL_FALSE, sizeof(XY), 0);
        glEnableVertexAttribArray(aVert);

        glGenBuffers(1, (GLuint*)&vb);
        glBindBuffer(GL_ARRAY_BUFFER, vb);

        glVertexAttribPointer(aPosAnchor, 4, GL_FLOAT, GL_FALSE, sizeof(QuadInstanceData), 0);
        glVertexAttribDivisor(aPosAnchor, 1);
        glEnableVertexAttribArray(aPosAnchor);

        glVertexAttribPointer(aScaleRadians, 3, GL_FLOAT, GL_FALSE, sizeof(QuadInstanceData), (GLvoid*)offsetof(QuadInstanceData, scale));
        glVertexAttribDivisor(aScaleRadians, 1);
        glEnableVertexAttribArray(aScaleRadians);

        glVertexAttribPointer(aColor, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(QuadInstanceData), (GLvoid*)offsetof(QuadInstanceData, color));
        glVertexAttribDivisor(aColor, 1);
        glEnableVertexAttribArray(aColor);

        glVertexAttribPointer(aTexRect, 4, GL_UNSIGNED_SHORT, GL_FALSE, sizeof(QuadInstanceData), (GLvoid*)offsetof(QuadInstanceData, texRectX));
        glVertexAttribDivisor(aTexRect, 1);
        glEnableVertexAttribArray(aTexRect);

        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        CheckGLError();
    }

    void Begin(float w, float h) {
        glUseProgram(p);
        glActiveTexture(GL_TEXTURE0/* + textureUnit*/);
        glUniform1i(uTex0, 0);
        glUniform2f(uCxy, 2 / w, 2 / h);
        glBindVertexArray(va);
    }

    void End() {
        if (quadCount) {
            Commit();
        }
    }

    void Commit() {
        glBindBuffer(GL_ARRAY_BUFFER, vb);
        glBufferData(GL_ARRAY_BUFFER, sizeof(QuadInstanceData) * quadCount, quadInstanceDatas.get(), GL_STREAM_DRAW);

        glBindTexture(GL_TEXTURE_2D, lastTextureId);
        glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, quadCount);
        CheckGLError();

//        sm->drawVerts += quadCount * 6;
//        sm->drawCall += 1;

        lastTextureId = 0;
        quadCount = 0;
    }

    QuadInstanceData* Draw(GLTexture const& tex, int numQuads) {
        assert(numQuads <= maxQuadNums);
        if (quadCount + numQuads > maxQuadNums || (lastTextureId && lastTextureId != tex)) {
            Commit();
        }
        lastTextureId = tex;
        auto r = &quadInstanceDatas[quadCount];
        quadCount += numQuads;
        return r;
    }

    void Draw(GLTexture const& tex, QuadInstanceData const& qv) {
        memcpy(Draw(tex, 1), &qv, sizeof(QuadInstanceData));
    }
};

/**********************************************************************************************************************************/
/**********************************************************************************************************************************/

// sprite frame
struct Frame {
    xx::Shared<GLTexture> tex;
    std::string key;
    // std::vector<std::string> aliases;	// unused
    std::optional<XY> anchor;
    XY spriteOffset{};
    XY spriteSize{};		// content size
    XY spriteSourceSize{};	// original pic size
    Rect textureRect{};
    size_t ud{};   // user data
    bool textureRotated{};
    std::vector<uint16_t> triangles;
    std::vector<float> vertices;
    std::vector<float> verticesUV;

    // single texture -> frame
    inline xx::Shared<Frame> static Create(xx::Shared<GLTexture> t) {
        auto f = xx::Make<Frame>();
        f->anchor = { 0.5, 0.5 };
        f->textureRotated = false;
        f->spriteSize = f->spriteSourceSize = { (float)t->Width(), (float)t->Height() };
        f->spriteOffset = {};
        f->textureRect = { 0, 0, f->spriteSize.x, f->spriteSize.y };
        f->tex = std::move(t);
        return f;
    }
};

// sprite
struct Quad : QuadInstanceData {
    xx::Shared<GLTexture> tex;

    Quad& SetTexture(xx::Shared<GLTexture> const& t) {
        tex = t;
        texRectX = 0;
        texRectY = 0;
        texRectW = t->Width();
        texRectH = t->Height();
        return *this;
    }
    Quad& SetFrame(xx::Shared<Frame> const& f) {
        assert(f && !f->textureRotated);	// known issue: not support now
        tex = f->tex;
        texRectX = f->textureRect.x;
        texRectY = f->textureRect.y;
        texRectW = (uint16_t)f->textureRect.wh.x;
        texRectH = (uint16_t)f->textureRect.wh.y;
        return *this;
    }
    template<typename T = float>
    XY Size() const {
        assert(tex);
        return { (T)texRectW, (T)texRectH };
    }
    Quad& SetAnchor(XY const& a) {
        anchor = a;
        return *this;
    }
    Quad& SetRotate(float const& r) {
        radians = r;
        return *this;
    }
    Quad& AddRotate(float const& r) {
        radians += r;
        return *this;
    }
    Quad& SetScale(XY const& s) {
        scale = s;
        return *this;
    }
    Quad& SetScale(float const& s) {
        scale = { s, s };
        return *this;
    }
    Quad& SetPosition(XY const& p) {
        pos = p;
        return *this;
    }
    Quad& SetPositionX(float const& x) {
        pos.x = x;
        return *this;
    }
    Quad& SetPositionY(float const& y) {
        pos.y = y;
        return *this;
    }
    Quad& AddPosition(XY const& p) {
        pos += p;
        return *this;
    }
    Quad& AddPositionX(float const& x) {
        pos.x += x;
        return *this;
    }
    Quad& AddPositionY(float const& y) {
        pos.y += y;
        return *this;
    }
    Quad& SetColor(RGBA8 const& c) {
        color = c;
        return *this;
    }
    Quad& SetColorA(uint8_t const& a) {
        color.a = a;
        return *this;
    }
    Quad& SetColorAf(float const& a) {
        color.a = 255 * a;
        return *this;
    }
    Quad& Draw(Shader_QuadInstance& s) {
        s.Draw(*tex, *this);
        return *this;
    }
};

/**********************************************************************************************************************************/
/**********************************************************************************************************************************/

struct EngineBase {
    inline static float w = 800, h = 600;          // can change at Init()
    EMSCRIPTEN_WEBGL_CONTEXT_HANDLE glContext;
    double devicePixelRatio{};
    Shader_QuadInstance shader;

    void GLInit() {
        double dpr = emscripten_get_device_pixel_ratio();
        emscripten_set_element_css_size("canvas", w / dpr, h / dpr);
        emscripten_set_canvas_element_size("canvas", w, h);

        EmscriptenWebGLContextAttributes attrs;
        emscripten_webgl_init_context_attributes(&attrs);
        attrs.alpha = 0;
        attrs.majorVersion = 2;
        glContext = emscripten_webgl_create_context("canvas", &attrs);
        xx_assert(glContext);
        emscripten_webgl_make_context_current(glContext);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        shader.Init();
    }

    void GLUpdate() {
        glViewport(0, 0, w, h);
        glClearColor(0, 0, 0, 0);
        glClear(GL_COLOR_BUFFER_BIT);

        //sm.ClearCounter();
        shader.Begin(w, h);
    }

    void GLUpdateEnd() {
        shader.End();
    }
};

/**********************************************************************************************************************************/
/**********************************************************************************************************************************/

template<typename T> concept Has_Init = requires(T t) { t.Init(); };
template<typename T> concept Has_AfterInit = requires(T t) { t.AfterInit(); };
template<typename T> concept Has_Update = requires(T t) { t.Update(); };
template<typename T> concept Has_Draw = requires(T t) { t.Draw(); };

template<typename Derived>
struct Engine : EngineBase {
    static constexpr float fps = 60, frameDelay = 1.f / fps, maxFrameDelay = frameDelay * 3;

    double nowSecs{}, delta{};
    double timePool{};
    int frameNumber{};
    EM_BOOL running{ EM_TRUE };
    xx::Tasks tasks;

    Engine() {
        if constexpr(Has_Init<Derived>) {
            ((Derived*)this)->Init();
        }
        GLInit();

        if constexpr(Has_AfterInit<Derived>) {
            ((Derived*)this)->AfterInit();
        }
    }

    EM_BOOL JsLoopCallback() {
        GLUpdate();

        if (delta > maxFrameDelay) {
            delta = maxFrameDelay;
        }
        timePool += delta;
        while (timePool >= frameDelay) {
            timePool -= frameDelay;
            ++frameNumber;
            if constexpr(Has_Update<Derived>) {
                ((Derived*)this)->Update();
            }
            tasks();
        }
        if constexpr(Has_Draw<Derived>) {
            ((Derived*)this)->Draw();
        }

        GLUpdateEnd();
        return running;
    }

    // task utils
    xx::Task<> Sleep(double secs) {
        auto e = nowSecs + secs;
        do {
            co_yield 0;
        } while (nowSecs < e);
    }

    template<bool showLog = false, int timeoutSeconds = 10>
    xx::Task<xx::Shared<GLTexture>> AsyncLoadTextureFromUrl(char const* url) {
        if constexpr(showLog) {
            std::cout << "LoadTextureFromUrl( " << url << " ) : begin. nowSecs = " << nowSecs << std::endl;
        }
        auto i = GLGenTextures<true>();
        int tw{}, th{};
        load_texture_from_url(i, url, &tw, &th);
        auto elapsedSecs = nowSecs + timeoutSeconds;
        while(nowSecs < elapsedSecs) {
            co_yield 0;
            if (tw) {
                if constexpr(showLog) {
                    std::cout << "LoadTextureFromUrl( " << url << " ) : loaded. nowSecs = " << nowSecs << ", size = " << tw << "," << th << std::endl;
                }
                co_return xx::Make<GLTexture>(i, tw, th, url);
            }
        }
        if constexpr(showLog) {
            std::cout << "LoadTextureFromUrl( " << url << " ) : timeout. timeoutSeconds = " << timeoutSeconds << std::endl;
        }
        co_return xx::Shared<GLTexture>{};
    }

};


#endif
