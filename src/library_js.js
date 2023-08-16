mergeInto(LibraryManager.library, {

  init_canvas_for_char: function(charWidth, charHeight) {
    if (window['gCanvas'] === undefined) {
      var canvas = document.createElement('canvas');
      canvas.width = charWidth;
      canvas.height = charHeight;
      var ctx = canvas.getContext('2d');
      ctx.globalAlpha = 1;
      ctx.fillStyle = 'white';
      ctx.font = charWidth + 'px Arial Unicode';
      window['gCanvas'] = canvas;
      window['gCanvasCtx'] = ctx;
    }
  },

  upload_unicode_char_to_texture: function(unicodeChar) {
    var canvas = window['gCanvas'];
    var ctx = window['gCanvasCtx'];
    ctx.clearRect(0, 0, canvas.width, canvas.height);
    ctx.fillText(String.fromCharCode(unicodeChar), 0, canvas.height);
    GLctx.texImage2D(GLctx.TEXTURE_2D, 0, GLctx.RGBA, GLctx.RGBA, GLctx.UNSIGNED_BYTE, canvas);
  },

  load_texture_from_url: function(glTexture, url, outW, outH) {
    var img = new Image();
    img.onload = () => {
      HEAPU32[outW>>2] = img.width;
      HEAPU32[outH>>2] = img.height;
      GLctx.bindTexture(GLctx.TEXTURE_2D, GL.textures[glTexture]);
      GLctx.texImage2D(GLctx.TEXTURE_2D, 0, GLctx.RGBA, GLctx.RGBA, GLctx.UNSIGNED_BYTE, img);
    };
  img.src = UTF8ToString(url);
  }

});
