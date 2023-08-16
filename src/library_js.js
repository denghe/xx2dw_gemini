mergeInto(LibraryManager.library, {

  init_gCanvas: function(width, height) {
    if (window['gCanvas'] === undefined) {
      var canvas = document.createElement('canvas');
      canvas.width = width;
      canvas.height = height;
      var ctx = canvas.getContext('2d');
      ctx.globalAlpha = 1;
      ctx.fillStyle = 'white';
      window['gCanvas'] = canvas;
      window['gCanvasCtx'] = ctx;
    }
  },

  upload_unicode_char_to_texture: function(charSize, utf8Char) {
    var canvas = window['gCanvas'];
    var ctx = window['gCanvasCtx'];
    ctx.clearRect(0, 0, canvas.width, canvas.height);
    ctx.font = charSize + 'px Consolas';
    var str = UTF8ToString(utf8Char);
    var strPixelWidth = ctx.measureText(str).width;
    ctx.fillText(str, 0, canvas.height);
    GLctx.texImage2D(GLctx.TEXTURE_2D, 0, GLctx.RGBA, GLctx.RGBA, GLctx.UNSIGNED_BYTE, canvas);
    return strPixelWidth;
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
