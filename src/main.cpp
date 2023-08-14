#include "pch.h"

template<typename T> concept Has_Init = requires(T t) { t.Init(); };
template<typename T> concept Has_Update = requires(T t) { t.Update(); };
template<typename T> concept Has_Draw = requires(T t) { t.Draw(); };

template<typename Derived>
struct Engine {
    static constexpr float fps = 60, frameDelay = 1.f / fps, maxFrameDelay = frameDelay * 3;
    static constexpr float w = 1280, h = 720;

    EMSCRIPTEN_WEBGL_CONTEXT_HANDLE glContext;
    double devicePixelRatio{};

    double nowSecs{}, delta{};
    double timePool{};
    int frameNumber{};
    EM_BOOL running{ EM_TRUE };
    xx::Tasks tasks;

    EM_BOOL JsLoopCallback() {
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
        return running;
    }

    template<GLenum shaderType>
    inline static GLuint CompileShader(const char *src) {
        GLuint shader = glCreateShader(shaderType);
        glShaderSource(shader, 1, &src, nullptr);
        glCompileShader(shader);
        return shader;
    }

    inline static GLuint create_program(GLuint vertexShader, GLuint fragmentShader) {
        GLuint program = glCreateProgram();
        glAttachShader(program, vertexShader);
        glAttachShader(program, fragmentShader);
        glBindAttribLocation(program, 0, "pos");
        glLinkProgram(program);
        glUseProgram(program);
        return program;
    }

    static GLuint create_texture() {
        GLuint texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        return texture;
    }


    Engine() {
        // todo: init js, gl

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

        if constexpr(Has_Init<Derived>) {
            ((Derived*)this)->Init();
        }
    }
};


struct GameLooper : Engine<GameLooper> {
    void Init() {
        tasks.Add([this]()->xx::Task<> {
            // todo
            for (int i = 0; i < 120; ++i) {
                std::cout << nowSecs << " : " << frameNumber << std::endl;
                co_yield 0;
            }
            running = false;
        });
    }

    void Draw() {
        // todo
    }
};

inline GameLooper gLooper;

int main() {
    emscripten_request_animation_frame_loop([](double nowMS, void*)->EM_BOOL {
        auto nowSecs = nowMS / 1000;
        gLooper.delta = nowSecs - gLooper.nowSecs;
        gLooper.nowSecs = nowSecs;
        return gLooper.JsLoopCallback();
    }, nullptr);
}
