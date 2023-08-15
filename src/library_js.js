mergeInto(LibraryManager.library, {
  upload_unicode_char_to_texture: function(unicodeChar, charSize, applyShadow) {
    var canvas = document.createElement('canvas');
    canvas.height = charSize;
    canvas.width = charSize * 0.75;
   document.body.appendChild(canvas); // Enable for debugging
    var ctx = canvas.getContext('2d');
    ctx.fillStyle = 'black';
    ctx['globalCompositeOperator'] = 'copy';
    ctx.globalAlpha = 0;
    ctx.fillRect(0, 0, canvas.width, canvas.height);
    ctx.globalAlpha = 1;
    ctx.fillStyle = 'white';
    ctx.font = charSize + 'px Arial Unicode';
    if (applyShadow) {
      ctx.shadowColor = 'black';
      ctx.shadowOffsetX = 2;
      ctx.shadowOffsetY = 2;
      ctx.shadowBlur = 3;
      ctx.strokeStyle = 'gray';
      ctx.strokeText(String.fromCharCode(unicodeChar), 0, canvas.width);
    }
    ctx.fillText(String.fromCharCode(unicodeChar), 0, canvas.width);
    GLctx.texImage2D(0xDE1/*GLctx.TEXTURE_2D*/, 0, 0x1908/*GLctx.RGBA*/, 0x1908/*GLctx.RGBA*/, 0x1401/*GLctx.UNSIGNED_BYTE*/, canvas);
  },
  load_texture_from_url: function(glTexture, url, outW, outH) {
    var img = new Image();
    img.onload = () => {
      HEAPU32[outW>>2] = img.width;
      HEAPU32[outH>>2] = img.height;
      GLctx.bindTexture(0xDE1/*GLctx.TEXTURE_2D*/, GL.textures[glTexture]);
      GLctx.texImage2D(0xDE1/*GLctx.TEXTURE_2D*/, 0, 0x1908/*GLctx.RGBA*/, 0x1908/*GLctx.RGBA*/, 0x1401/*GLctx.UNSIGNED_BYTE*/, img);
    };
    img.src = UTF8ToString(url);
  }
});
