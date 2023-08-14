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
