 var window= {
           x:0,
           y:0,
           width:1920,
           height: 1080,
           transparent: false,
           name:'texture-mesh-app'
 };

 var viewMode={
       'stereoscopic-mode':'mono', // stereo-horizontal, stereo-vertical, stereo-interlaced,
       'stereo-base': 65 // Distance in millimeters between left/right cameras typically between (50-70mm)
 };

 var options= {
    'window': window,
    'view-mode': viewMode,
 }

var imageDir = "./images/";


try {
  // target
  var dali = require('dali')( options );
}
catch(err) {
  // desktop
  var dali = require('../build/Release/dali')( options );
}

var daliApp = {};

daliApp.createMeshActor = function() {

      // Create shader
      var vertShader =
               "attribute mediump vec2    aPosition;\
                attribute highp   vec2    aTexCoord;\
                varying   mediump vec2    vTexCoord;\
                uniform   mediump mat4    uMvpMatrix;\
                uniform   mediump vec3    uSize;\
                uniform   lowp    vec4    uFadeColor;\
                void main()\
                {\
                  mediump vec4 vertexPosition = vec4(aPosition, 0.0, 1.0);\
                  vertexPosition.xyz *= uSize;\
                  vertexPosition = uMvpMatrix * vertexPosition;\
                  vTexCoord = aTexCoord;\
                  gl_Position = vertexPosition;\
                }";

      var fragShader =
               "varying mediump vec2  vTexCoord;\
                uniform lowp    vec4  uColor;\
                uniform sampler2D     sTexture;\
                uniform lowp    vec4  uFadeColor;\
                void main()\
                {\
                  gl_FragColor = texture2D( sTexture, vTexCoord ) * uColor * uFadeColor;\
                }";

      var shaderOptions = {
              vertexShader : vertShader,
              fragmentShader: fragShader
          };

      var shader = new dali.Shader(shaderOptions);

      var textureSet = new dali.TextureSet;
      var image = new dali.ResourceImage( {url: imageDir + "image-1.jpg"} );
      textureSet.setImage(0, image);

      var textureSet2 = new dali.TextureSet;
      var image2 = new dali.ResourceImage( {url: imageDir + "image-2.jpg"} );
      textureSet2.setImage(0, image2);

      // Create vertex buffer
      var texturedQuadVertexFormat ={ "aPosition" : dali.PROPERTY_VECTOR2,
                                        "aTexCoord" : dali.PROPERTY_VECTOR2 };

      var halfQuadSize = 0.5;
      var texturedQuadVertexData = [ -halfQuadSize, -halfQuadSize, 0.0, 0.0,
                                      halfQuadSize, -halfQuadSize, 1.0, 0.0,
                                     -halfQuadSize,  halfQuadSize, 0.0, 1.0,
                                      halfQuadSize,  halfQuadSize, 1.0, 1.0 ];

      var texturedQuadVertexDataArray = new Float32Array(texturedQuadVertexData.length);
      texturedQuadVertexDataArray.set(texturedQuadVertexData, 0);
      var texturedQuadVertices = new dali.PropertyBuffer(texturedQuadVertexFormat);
      texturedQuadVertices.setData(texturedQuadVertexDataArray, 4);

      var indexFormat ={ "indices" : dali.PROPERTY_INTEGER };

      var indexData = [0, 3, 1, 0, 2, 3];

      var indexDataArray = new Uint32Array(indexData.length);
      indexDataArray.set(indexData, 0);

      // Create geometry
      var geometry = new dali.Geometry();
      geometry.addVertexBuffer( texturedQuadVertices );
      geometry.setIndexBuffer( indexDataArray, 6 );

      var renderer = new dali.Renderer(geometry, shader);
      renderer.setTextures(textureSet);
      renderer.depthIndex = 0;

      var meshActor = new dali.Actor();
      meshActor.addRenderer( renderer );
      meshActor.size = [530, 530, 0];
      meshActor.parentOrigin = dali.TOP_CENTER;
      meshActor.anchorPoint = dali.TOP_CENTER;
      meshActor.registerAnimatableProperty("uFadeColor", [1.0, 0.0, 1.0, 1.0]);

      dali.stage.add( meshActor );

      var renderer2 = new dali.Renderer(geometry, shader);
      renderer2.setTextures(textureSet2);
      renderer2.depthIndex = 0;

      var meshActor2 = new dali.Actor();
      meshActor2.addRenderer( renderer2 );
      meshActor2.size = [530, 530, 0];
      meshActor2.parentOrigin = dali.BOTTOM_CENTER;
      meshActor2.anchorPoint = dali.BOTTOM_CENTER;
      meshActor2.registerAnimatableProperty("uFadeColor", [0.0, 0.0, 1.0, 1.0]);

      dali.stage.add( meshActor2 );

      // Animate the mesh actors
      var animation = new dali.Animation(5);

      var keyFramesIndex = [{progress:0.0, value: [0.0, 0.0, 0.0, 0.0]}, {progress:1.0, value:[0.0, 1.0, 0.0, 1.0]}];
      var animOptions = { alpha:dali.ALPHA_FUNCTION_SIN, delay:0, duration:5};

      var keyFramesIndex2 = [{progress:0.0, value: [0.0, 0.0, 0.0, 0.0]}, {progress:1.0, value:[1.0, 0.0, 1.0, 1.0]}];
      var animOptions2 = { alpha:dali.ALPHA_FUNCTION_SIN, delay:0, duration:5};

      animation.animateBetween( meshActor, "uFadeColor", keyFramesIndex, animOptions );
      animation.animateBetween( meshActor2, "uFadeColor", keyFramesIndex2, animOptions2 );
      animation.setLooping(true);
      animation.play();
}

function startup()
{
  daliApp.init();
}

daliApp.init = function()
{
  daliApp.createMeshActor();
}


startup();

