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

// desktop
//var dali = require('./build/Release/dali')( options );

// target
var dali = require('dali')( options );

var daliApp = {};

daliApp.createMeshActor = function() {

      // Create shader
      var vertShader =
               "attribute mediump vec2    aPosition1;\
                attribute mediump vec2    aPosition2;\
                uniform   mediump mat4    uMvpMatrix;\
                uniform   mediump vec3    uSize;\
                uniform   mediump float   uMorphAmount;\
                void main()\
                {\
                  mediump vec2 morphPosition = mix(aPosition1, aPosition2, uMorphAmount);\
                  mediump vec4 vertexPosition = vec4(morphPosition, 0.0, 1.0);\
                  vertexPosition.xyz *= uSize;\
                  vertexPosition = uMvpMatrix * vertexPosition;\
                  gl_Position = vertexPosition;\
                }";

      var fragShader =
               "uniform lowp  vec4    uColor;\
                uniform sampler2D     sTexture;\
                void main()\
                {\
                  gl_FragColor = uColor;\
                }";

      var shaderOptions = {
              vertexShader : vertShader,
              fragmentShader: fragShader
          };

      var shader = new dali.Shader(shaderOptions);

      var material = new dali.Material( shader );

      // Create vertex buffer
      var pentagonVertexFormat ={ "aPosition1" : dali.PROPERTY_VECTOR2 };

      var pentagonVertexData = [    0,     1,
                                -0.95,  0.31,
                                -0.59, -0.81,
                                 0.59, -0.81,
                                 0.95,  0.31];

      var pentagonVertexDataArray = new Float32Array(pentagonVertexData.length);
      pentagonVertexDataArray.set(pentagonVertexData, 0);
      var pentagonVertices = new dali.PropertyBuffer(pentagonVertexFormat, 5);
      pentagonVertices.setData(pentagonVertexDataArray);

      var pentacleVertexFormat ={ "aPosition2" : dali.PROPERTY_VECTOR2};

      var pentacleVertexData = [    0,    -1,
                                 0.59,  0.81,
                                -0.95, -0.31,
                                 0.95, -0.31,
                                -0.59,  0.81];

      var pentacleVertexDataArray = new Float32Array(pentacleVertexData.length);
      pentacleVertexDataArray.set(pentacleVertexData, 0);
      var pentacleVertices = new dali.PropertyBuffer(pentacleVertexFormat, 5);
      pentacleVertices.setData(pentacleVertexDataArray);

      var indexFormat ={ "indices" : dali.PROPERTY_INTEGER };

      var indexData = [0, 1, 1, 2, 2, 3, 3, 4, 4, 0];

      var indexDataArray = new Uint32Array(indexData.length);
      indexDataArray.set(indexData, 0);
      var indices = new dali.PropertyBuffer(indexFormat, 10);
      indices.setData(indexDataArray);

      // Create geometry
      var geometry = new dali.Geometry();
      geometry.addVertexBuffer( pentagonVertices );
      geometry.addVertexBuffer( pentacleVertices );
      geometry.setIndexBuffer( indices );
      geometry.setGeometryType( dali.GEOMETRY_LINES );

      var renderer = new dali.Renderer(geometry, material);
      renderer.depthIndex = 0;

      // Create mesh actor
      var meshActor = new dali.Actor();
      meshActor.addRenderer( renderer );
      meshActor.size = [300, 300, 0];
      meshActor.parentOrigin = dali.CENTER;
      meshActor.anchorPoint = dali.CENTER;
      meshActor.registerAnimatableProperty("uMorphAmount", 0.0);

      dali.stage.add( meshActor );

      // Animate the mesh actor
      var animation = new dali.Animation(5);

      var keyFramesIndex = [{progress:0.0, value: 0.0}, {progress:1.0, value:1.0}];
      var animOptions = { alpha:"sin", delay:0, duration:5};

      animation.animateBetween( meshActor, "uMorphAmount", keyFramesIndex, animOptions );
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

