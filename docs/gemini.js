var e=Module,f,l,m,p,q,t=new TextDecoder("utf8"),u=(a,b)=>{if(!a)return"";b=a+b;for(var c=a;!(c>=b)&&l[c];)++c;return t.decode(l.subarray(a,c))},w=0,x=0,y=[],z=[0,document,window],B=1,C=[],D=[],F=[],G=[],H=[],I=[],M,N=["default","low-power","high-performance"],Q=(a,b,c)=>{var d=l;if(!(0<c))return 0;var g=b;c=b+c-1;for(var k=0;k<a.length;++k){var h=a.charCodeAt(k);if(55296<=h&&57343>=h){var n=a.charCodeAt(++k);h=65536+((h&1023)<<10)|n&1023}if(127>=h){if(b>=c)break;d[b++]=h}else{if(2047>=h){if(b+1>=
c)break;d[b++]=192|h>>6}else{if(65535>=h){if(b+2>=c)break;d[b++]=224|h>>12}else{if(b+3>=c)break;d[b++]=240|h>>18;d[b++]=128|h>>12&63}d[b++]=128|h>>6&63}d[b++]=128|h&63}}d[b]=0;return b-g},P,T;function v(a){this.ca=a-24;this.sa=function(b){m[this.ca+4>>2]=b};this.ra=function(b){m[this.ca+8>>2]=b};this.oa=function(b,c){this.pa();this.sa(b);this.ra(c)};this.pa=function(){m[this.ca+16>>2]=0}}function A(a){a=2<a?u(a):a;return z[a]||document.querySelector(a)}
function J(a){for(var b=B++,c=a.length;c<b;c++)a[c]=null;return b}function K(a,b){a.ca||(a.ca=a.getContext,a.getContext=function(d,g){g=a.ca(d,g);return"webgl"==d==g instanceof WebGLRenderingContext?g:null});var c=a.getContext("webgl2",b);return c?L(c,b):0}function L(a,b){var c=J(I);b={handle:c,attributes:b,version:b.ta,ma:a};a.canvas&&(a.canvas.ua=b);I[c]=b;return c}function O(a,b,c,d){for(var g=0;g<a;g++){var k=P[c](),h=k&&J(d);k&&(k.name=h,d[h]=k);f[b+4*g>>2]=h}}
function R(a){return"]"==a.slice(-1)&&a.lastIndexOf("[")}function S(a){var b=P.na,c=b.da[a];"number"==typeof c&&(b.da[a]=c=P.getUniformLocation(b,b.ka[a]+(0<c?"["+c+"]":"")));return c}
WebAssembly.instantiate(e.wasm,{a:{w:(a,b,c,d)=>{throw`Assertion failed: ${u(a)}, at: `+[b?u(b):"unknown filename",c,d?u(d):"unknown function"];},a:function(a,b,c){(new v(a)).oa(b,c);w=a;x++;throw w;},m:()=>{throw"";},U:function(){return devicePixelRatio},A:(a,b,c)=>l.copyWithin(a,b,b+c),Z:function(a,b){function c(d){var g=y[a];g||(a>=y.length&&(y.length=a+1),y[a]=g=q.get(a));g(d,b)&&requestAnimationFrame(c)}return requestAnimationFrame(c)},y:()=>!1,J:function(a,b,c){a=A(a);if(!a)return-4;a.width=
b;a.height=c;return 0},R:function(a,b,c){a=A(a);if(!a)return-4;a.style.width=b+"px";a.style.height=c+"px";return 0},x:function(a,b){b>>=2;b={alpha:!!f[b],depth:!!f[b+1],stencil:!!f[b+2],antialias:!!f[b+3],premultipliedAlpha:!!f[b+4],preserveDrawingBuffer:!!f[b+5],powerPreference:N[f[b+6]],failIfMajorPerformanceCaveat:!!f[b+7],ta:f[b+8],xa:f[b+9],wa:f[b+10],qa:f[b+11],ya:f[b+12],za:f[b+13]};a=A(a);return!a||b.qa?0:K(a,b)},z:function(a){a>>=2;for(var b=0;14>b;++b)f[a+b]=0;f[a]=f[a+1]=f[a+3]=f[a+4]=
f[a+8]=f[a+10]=1},v:function(a){M=I[a];e.va=P=M&&M.ma;return!a||P?0:-5},E:function(a){P.activeTexture(a)},q:function(a,b){P.attachShader(D[a],G[b])},b:function(a,b){35051==a?P.ia=b:35052==a&&(P.ja=b);P.bindBuffer(a,C[b])},j:function(a,b){P.bindTexture(a,F[b])},l:function(a){P.bindVertexArray(H[a])},Y:function(a,b){P.blendFunc(a,b)},g:function(a,b,c,d){c&&b?P.bufferData(a,l,d,c,b):P.bufferData(a,b,d)},G:function(a){P.clear(a)},H:function(a,b,c,d){P.clearColor(a,b,c,d)},M:function(a){P.compileShader(G[a])},
S:function(){var a=J(D),b=P.createProgram();b.name=a;b.ga=b.ea=b.fa=0;b.ha=1;D[a]=b;return a},O:function(a){var b=J(G);G[b]=P.createShader(a);return b},n:function(a,b){for(var c=0;c<a;c++){var d=f[b+4*c>>2],g=C[d];g&&(P.deleteBuffer(g),g.name=0,C[d]=null,d==P.ia&&(P.ia=0),d==P.ja&&(P.ja=0))}},t:function(a){if(a){var b=D[a];b&&(P.deleteProgram(b),b.name=0,D[a]=null)}},h:function(a){if(a){var b=G[a];b&&(P.deleteShader(b),G[a]=null)}},V:function(a,b){for(var c=0;c<a;c++){var d=f[b+4*c>>2],g=F[d];g&&
(P.deleteTexture(g),g.name=0,F[d]=null)}},K:function(a,b){for(var c=0;c<a;c++){var d=f[b+4*c>>2];P.deleteVertexArray(H[d]);H[d]=null}},k:function(a,b,c,d){P.drawArraysInstanced(a,b,c,d)},u:function(a){P.enable(a)},c:function(a){P.enableVertexAttribArray(a)},r:function(a,b){O(a,b,"createBuffer",C)},X:function(a,b){O(a,b,"createTexture",F)},T:function(a,b){O(a,b,"createVertexArray",H)},e:function(a,b){return P.getAttribLocation(D[a],u(b))},P:function(a,b,c,d){a=P.getProgramInfoLog(D[a]);b=0<b&&d?Q(a,
d,b):0;c&&(f[c>>2]=b)},p:function(a,b,c){if(c&&!(a>=B))if(a=D[a],35716==b)f[c>>2]=P.getProgramInfoLog(a).length+1;else if(35719==b){if(!a.ga)for(b=0;b<P.getProgramParameter(a,35718);++b)a.ga=Math.max(a.ga,P.getActiveUniform(a,b).name.length+1);f[c>>2]=a.ga}else if(35722==b){if(!a.ea)for(b=0;b<P.getProgramParameter(a,35721);++b)a.ea=Math.max(a.ea,P.getActiveAttrib(a,b).name.length+1);f[c>>2]=a.ea}else if(35381==b){if(!a.fa)for(b=0;b<P.getProgramParameter(a,35382);++b)a.fa=Math.max(a.fa,P.getActiveUniformBlockName(a,
b).length+1);f[c>>2]=a.fa}else f[c>>2]=P.getProgramParameter(a,b)},L:function(a,b,c,d){a=P.getShaderInfoLog(G[a]);b=0<b&&d?Q(a,d,b):0;c&&(f[c>>2]=b)},o:function(a,b,c){c&&(35716==b?(a=P.getShaderInfoLog(G[a]),f[c>>2]=a?a.length+1:0):35720==b?(a=P.getShaderSource(G[a]),f[c>>2]=a?a.length+1:0):f[c>>2]=P.getShaderParameter(G[a],b))},s:function(a,b){b=u(b);if(a=D[a]){var c=a,d=c.da,g=c.la,k;if(!d)for(c.da=d={},c.ka={},k=0;k<P.getProgramParameter(c,35718);++k){var h=P.getActiveUniform(c,k),n=h.name;h=
h.size;var r=R(n);r=0<r?n.slice(0,r):n;var E=c.ha;c.ha+=h;g[r]=[h,E];for(n=0;n<h;++n)d[E]=n,c.ka[E++]=r}c=a.da;d=0;g=b;k=R(b);0<k&&(d=parseInt(b.slice(k+1))>>>0,g=b.slice(0,k));if((g=a.la[g])&&d<g[0]&&(d+=g[1],c[d]=c[d]||P.getUniformLocation(a,b)))return d}return-1},Q:function(a){a=D[a];P.linkProgram(a);a.da=0;a.la={}},N:function(a,b,c,d){for(var g="",k=0;k<b;++k){var h=d?f[d+4*k>>2]:-1;g+=u(f[c+4*k>>2],0>h?void 0:h)}P.shaderSource(G[a],g)},i:function(a,b,c){P.texParameteri(a,b,c)},D:function(a,b){P.uniform1i(S(a),
b)},C:function(a,b,c){P.uniform2f(S(a),b,c)},F:function(a){a=D[a];P.useProgram(a);P.na=a},f:function(a,b){P.vertexAttribDivisor(a,b)},d:function(a,b,c,d,g,k){P.vertexAttribPointer(a,b,c,!!d,g,k)},I:function(a,b,c,d){P.viewport(a,b,c,d)},B:function(a,b){if(void 0===window.gCanvas){var c=document.createElement("canvas");c.width=a;c.height=b;a=c.getContext("2d");a.globalAlpha=1;a.fillStyle="white";window.gCanvas=c;window.gCanvasCtx=a}},W:function(a,b){var c=window.gCanvas,d=window.gCanvasCtx;d.clearRect(0,
0,c.width,c.height);d.font=a+"px Consolas";a=u(b);b=d.measureText(a).width;d.fillText(a,0,c.height);P.texImage2D(P.TEXTURE_2D,0,P.RGBA,P.RGBA,P.UNSIGNED_BYTE,c);return b}}}).then(a=>{a=a.instance.exports;T=a.aa;q=a.ba;p=a._;var b=p.buffer;f=new Int32Array(b);l=new Uint8Array(b);m=new Uint32Array(b);a.$();T()});
