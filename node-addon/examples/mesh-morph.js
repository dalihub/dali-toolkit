 var window= {
           x:0,
           y:0,
           width:1920,
           height: 1080,
           transparent: false,
           name:'mesh-morph-app'
 };

 var viewMode={
       'stereoscopic-mode':'mono', // stereo-horizontal, stereo-vertical, stereo-interlaced,
       'stereo-base': 65 // Distance in millimeters between left/right cameras typically between (50-70mm)
 };

 var options= {
    'window': window,
    'view-mode': viewMode,
 }

// desktop
//var dali = require('../build/Release/dali')( options );

// target
var dali = require('dali')( options );

var daliApp = {};

daliApp.createMeshActor = function() {

      // Create shader
      var vertShader =
                  "attribute mediump vec2    aInitPos;\
                attribute mediump vec2    aFinalPos;\
                attribute mediump vec3    aColor;\
                uniform   mediump mat4    uMvpMatrix;\
                uniform   mediump vec3    uSize;\
                uniform   mediump float   uDelta;\
                uniform   lowp    vec4    uColor;\
                varying   lowp    vec4    vColor;\
                void main()\
                {\
                  mediump vec4 vertexPosition = vec4(mix(aInitPos, aFinalPos, uDelta), 0.0, 1.0);\
                  vertexPosition.xyz *= uSize;\
                  vertexPosition = uMvpMatrix * vertexPosition;\
                  gl_Position = vertexPosition;\
                  vColor = vec4(aColor, 0.) * uColor;\
                }";

      var fragShader =
               "varying lowp vec4 vColor;\
                void main()\
                {\
                  gl_FragColor = vColor;\
                }";

      var shaderOptions = {
              vertexShader : vertShader,
              fragmentShader : fragShader
          };

      var shader = new dali.Shader(shaderOptions);

      var material = new dali.Material( shader );

      // Create vertex buffer for initial positions
      var initialPositionVertexFormat = { "aInitPos" : dali.PROPERTY_VECTOR2 };

      var initialPositionVertexData = [
                                       // yellow
                                       -0.50, -0.50,
                                         0.0,   0.0,
                                       -0.50,  0.50,

                                       // green
                                       -0.50, -0.50,
                                        0.50, -0.50,
                                         0.0,   0.0,

                                       // blue
                                        0.50, -0.50,
                                        0.50,   0.0,
                                        0.25, -0.25,

                                       // red
                                        0.25, -0.25,
                                        0.50,   0.0,
                                        0.25,  0.25,
                                        0.25,  0.25,
                                         0.0,   0.0,
                                        0.25, -0.25,

                                       // cyan
                                         0.0,   0.0,
                                        0.25,  0.25,
                                       -0.25,  0.25,

                                       // magenta
                                       -0.25,  0.25,
                                        0.25,  0.25,
                                         0.0,  0.50,
                                         0.0,  0.50,
                                       -0.50,  0.50,
                                       -0.25,  0.25,

                                        // orange
                                        0.50,   0.0,
                                        0.50,  0.50,
                                         0.0,  0.50
                                      ];

      var initialPositionVertexDataArray = new Float32Array(initialPositionVertexData.length);
      initialPositionVertexDataArray.set(initialPositionVertexData, 0);
      var initialPositionVertices = new dali.PropertyBuffer(initialPositionVertexFormat);
      initialPositionVertices.setData(initialPositionVertexDataArray, 27);

      // Create vertex buffer for final positions
      var finalPositionVertexFormat = { "aFinalPos" : dali.PROPERTY_VECTOR2 };

      var bigSide = 0.707106781;
      var side = bigSide * 0.5;

      var finalPositionVertexData = [
                                     // yellow
                                               side,                        0.25, // pA
                                               side,              0.25 + bigSide, // pB
                                     side - bigSide,              0.25 + bigSide, // pC

                                     // green
                                        side - 0.50,                       -0.25, // pD
                                               side,                        0.25, // pA
                                        side - 0.50,                        0.75, // pE

                                     // blue
                                        side - 0.50,                       -0.75, // pJ
                                        side - 0.50,                       -0.25, // pD
                                        side - 0.75,                       -0.50, // pI

                                     // red
                                        side - 0.75,                       -0.50, // pI
                                        side - 0.50,                       -0.25, // pD
                                        side - 0.75,                         0.0, // pG
                                        side - 0.75,                         0.0, // pG
                                         side - 1.0,                       -0.25, // pH
                                        side - 0.75,                       -0.50, // pI

                                     // cyan
                                        side - 0.75,                       -0.50, // pI
                                         side - 1.0,                       -0.25, // pH
                                         side - 1.0,                       -0.75, // pK

                                     // magenta
                                               side,       0.25 + bigSide - side, // pL
                                         side * 2.0, 0.25 + bigSide - side * 2.0, // pM
                                         side * 2.0,       0.25 + bigSide - side, // pN
                                         side * 2.0,       0.25 + bigSide - side, // pN
                                               side,              0.25 + bigSide, // pB
                                               side,       0.25 + bigSide - side, // pL

                                     // orange
                                        side - 0.50,                       -0.25, // pD
                                        side - 0.50,             -0.25 + bigSide, // pF2
                                             - 0.50,                -0.25 + side  // pF
                                    ];

      var finalPositionVertexDataArray = new Float32Array(finalPositionVertexData.length);
      finalPositionVertexDataArray.set(finalPositionVertexData, 0);
      var finalPositionVertices = new dali.PropertyBuffer(finalPositionVertexFormat);
      finalPositionVertices.setData(finalPositionVertexDataArray, 27);

      // Create vertex buffer for color
      var colorVertexFormat = { "aColor" : dali.PROPERTY_VECTOR3 };

      var colorVertexData = [
                             // yellow
                             1.0, 1.0, 0.0,
                             1.0, 1.0, 0.0,
                             1.0, 1.0, 0.0,

                             // green
                             0.0, 1.0, 0.0,
                             0.0, 1.0, 0.0,
                             0.0, 1.0, 0.0,

                             // blue
                             0.0, 0.0, 1.0,
                             0.0, 0.0, 1.0,
                             0.0, 0.0, 1.0,

                             // red
                             1.0, 0.0, 0.0,
                             1.0, 0.0, 0.0,
                             1.0, 0.0, 0.0,
                             1.0, 0.0, 0.0,
                             1.0, 0.0, 0.0,
                             1.0, 0.0, 0.0,

                             // cyan
                             0.0, 1.0, 1.0,
                             0.0, 1.0, 1.0,
                             0.0, 1.0, 1.0,

                             // magenta
                             1.0, 0.0, 1.0,
                             1.0, 0.0, 1.0,
                             1.0, 0.0, 1.0,
                             1.0, 0.0, 1.0,
                             1.0, 0.0, 1.0,
                             1.0, 0.0, 1.0,

                             // orange
                             1.0, 0.5, 0.0,
                             1.0, 0.5, 0.0,
                             1.0, 0.5, 0.0
                            ];

      var colorVertexDataArray = new Float32Array(colorVertexData.length);
      colorVertexDataArray.set(colorVertexData, 0);
      var colorVertices = new dali.PropertyBuffer(colorVertexFormat);
      colorVertices.setData(colorVertexDataArray, 27);

      // Create geometry
      var geometry = new dali.Geometry();
      geometry.addVertexBuffer( initialPositionVertices );
      geometry.addVertexBuffer( finalPositionVertices );
      geometry.addVertexBuffer( colorVertices );

      var renderer = new dali.Renderer(geometry, material);
      renderer.depthIndex = 0;

      // Create mesh actor
      var meshActor = new dali.Actor();
      meshActor.addRenderer( renderer );
      meshActor.size = [400, 400, 0];
      meshActor.parentOrigin = dali.CENTER;
      meshActor.anchorPoint = dali.CENTER;
      meshActor.registerAnimatableProperty("uDelta", 0.0);

      dali.stage.add( meshActor );

      // Animate the mesh actor
      var animation = new dali.Animation(5);

      var animOptions = { alpha:dali.ALPHA_FUNCTION_SIN, delay:0, duration:5};

      animation.animateTo( meshActor, "uDelta", 1.0, animOptions );
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

