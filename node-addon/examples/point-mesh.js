 var window= {
           x:0,
           y:0,
           width:1920,
           height: 1080,
           transparent: false,
           name:'line-mesh-app'
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
                attribute highp   float   aHue;\
                varying   mediump vec2    vTexCoord;\
                uniform   mediump mat4    uMvpMatrix;\
                uniform   mediump vec3    uSize;\
                uniform   mediump float   uPointSize;\
                uniform   lowp    vec4    uFadeColor;\
                varying   mediump vec3    vVertexColor;\
                varying   mediump float   vHue;\
                vec3 hsv2rgb(vec3 c)\
                {\
                  vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);\
                  vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);\
                  return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);\
                }\
                void main()\
                {\
                  mediump vec4 vertexPosition = vec4(aPosition, 0.0, 1.0);\
                  vertexPosition.xyz *= (uSize-uPointSize);\
                  vertexPosition = uMvpMatrix * vertexPosition;\
                  vVertexColor = hsv2rgb( vec3( aHue, 0.7, 1.0 ) );\
                  vHue = aHue;\
                  gl_PointSize = uPointSize;\
                  gl_Position = vertexPosition;\
                }";

      var fragShader =
               "varying mediump vec3  vVertexColor;\
                varying mediump float vHue;\
                uniform lowp  vec4    uColor;\
                uniform sampler2D     sTexture1;\
                uniform sampler2D     sTexture2;\
                uniform lowp vec4     uFadeColor;\
                void main()\
                {\
                  mediump vec4 texCol1 = texture2D(sTexture1, gl_PointCoord);\
                  mediump vec4 texCol2 = texture2D(sTexture2, gl_PointCoord);\
                  gl_FragColor = vec4(vVertexColor, 1.0) * ((texCol1*vHue) + (texCol2*(1.0-vHue)));\
                }";

      var shaderOptions = {
              vertexShader : vertShader,
              fragmentShader: fragShader
          };

      var shader = new dali.Shader(shaderOptions);

      var image = new dali.ResourceImage( {url: imageDir + "image-1.jpg"} );
      var textureSet = new dali.TextureSet;
      textureSet.setImage(0, image);

      // Create vertex buffer
      var polyhedraVertexFormat ={ "aPosition" : dali.PROPERTY_VECTOR2,
                                     "aHue"      : dali.PROPERTY_FLOAT    };

      var numSides = 20;
      var angle = 0;
      var sectorAngle = 2.0 * Math.PI / numSides;

      var polyhedraVertexData = [];
      for(i=0; i<numSides; ++i)
      {
        var positionX = Math.sin(angle) * 0.5;
        var positionY = Math.cos(angle) * 0.5;
        var hue = angle / ( 2.0 * Math.PI);

        polyhedraVertexData[i*3+0] = positionX;
        polyhedraVertexData[i*3+1] = positionY;
        polyhedraVertexData[i*3+2] = hue;

        angle += sectorAngle;
      }

      var polyhedraVertexDataArray = new Float32Array(polyhedraVertexData.length);
      polyhedraVertexDataArray.set(polyhedraVertexData, 0);
      var polyhedraVertices = new dali.PropertyBuffer(polyhedraVertexFormat);
      polyhedraVertices.setData(polyhedraVertexDataArray, numSides);

      // Create geometry
      var geometry = new dali.Geometry();
      geometry.addVertexBuffer( polyhedraVertices );
      geometry.setGeometryType( dali.GEOMETRY_POINTS );

      var renderer = new dali.Renderer(geometry, shader);
      renderer.setTextures(textureSet);
      renderer.registerAnimatableProperty("uFadeColor", [1.0, 0.0, 1.0, 1.0]); // Green
      renderer.registerAnimatableProperty("uPointSize", 80.0);
      renderer.depthIndex = 0;

      // Create mesh actor
      var meshActor = new dali.Actor();
      meshActor.addRenderer( renderer );
      meshActor.size = [400, 400, 0];
      meshActor.parentOrigin = dali.CENTER;
      meshActor.anchorPoint = dali.CENTER;
      meshActor.registerAnimatableProperty("uFadeColor", [0.0, 1.0, 0.0, 1.0]); // Magenta

      dali.stage.add( meshActor );

      // Animate the mesh actor
      var animation = new dali.Animation(5);
      var animOptions = { alpha:dali.ALPHA_FUNCTION_LINEAR, delay:0, duration:5};
      var rotation = new dali.Rotation(360,0,0,1);
      animation.animateBy( meshActor, "orientation", rotation, animOptions );
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

