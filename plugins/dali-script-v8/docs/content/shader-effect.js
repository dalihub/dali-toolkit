/**
 *

## Shader Effects API

Shader effects provide a visual effect for actors.

For a Custom shader you can provide the vertex and fragment shader code as strings.
Each shader is provided with default uniforms and attributes.
For a vertex shader this part contains the following code:
```
precision highp float;
attribute vec3  aPosition;
attribute vec2  aTexCoord;
uniform   mat4  uMvpMatrix;
uniform   mat4  uModelMatrix;
uniform   mat4  uViewMatrix;
uniform   mat4  uModelView;
uniform   mat3  uNormalMatrix;
uniform   mat4  uProjection;
uniform   vec4  uColor;
varying   vec2  vTexCoord;
```
The custom shader part is expected to output the vertex position and texture coordinate.
A basic custom vertex shader would contain the following code:
```
void main()
{
    gl_Position = uProjection * uModelView * vec4(aPosition, 1.0);
    vTexCoord = aTexCoord;
}
```
For an Image fragment shader the default attributes and uniforms are:
```
precision mediump float;
uniform   sampler2D sTexture;
uniform   sampler2D sEffect;
uniform   vec4      uColor;
varying   vec2      vTexCoord;
```
The custom shader is expected to output the fragment color.
The basic fragment shader for images would contain:
```
void main()
{
  gl_FragColor = texture2D( sTexture, vTexCoord ) * uColor;
}
```
The API supports functionality such as:

+ new dali.{{#crossLink "ShaderEffect/ShaderEffect:method"}}{{/crossLink}}
+ {{#crossLink "ShaderEffect/setUniform:method"}}{{/crossLink}}

### Example of using a custom uniform to brighten an Image (Fragment Shader)

<img src="../assets/img/fragment-shader-color.png">
```
createColorEffect = function()
{
  
  // add uColorShift to the pixel color
  
  var fragShader =
    "uniform lowp vec4 uColorShift; \
    \
    void main()             \
    {                  \
      gl_FragColor = texture2D( sTexture, vTexCoord ) * uColor + uColorShift; \
    }"
  
    var shaderOptions = {
        geometryType: "image",
        fragmentShader: fragShader
    };
  
  // create a new shader effect
    var shader = new dali.ShaderEffect(shaderOptions);
  
  // add the color shift uniform so we can modify and animate it
    shader.setUniform("uColorShift", [0.0, 0.0, 0.0, 0]);
    return shader;
}
// create an image actor and add an image to it
var image = new dali.ResourceImage( { url: getImageDirectory() +"gallery-medium-50.jpg"});
var imageActor = new dali.ImageActor( image );
imageActor.parentOrigin = dali.CENTER;
dali.stage.add( imageActor );
  
var colorShift = createColorEffect();
  
colorShift.setUniform( "uColorShift", [0.5,0.5,0.5,0.0] );  // increase RGB by 50%
  
imageActor.setShaderEffect( colorShift );
```

For an example of animating we can just do:
```
var shaderAnim = new dali.Animation(10);
  
var animOptions = {
    alpha: "doubleEaseInOutSine60",
};
  
// animate the color uniform to full white
shaderAnim.animateTo( colorShift, "uColorShift", dali.COLOR_WHITE, animOptions);
  
shaderAnim.play();
```
Like all animatable properties we can also use keyframes to animate the value.
* * *

### Example of animating a custom uniform to perform a mask operation (Fragment Shader)

In this example we are using the OpenGL discard function to draw an image with a circular mask.

<img src="../assets/img/fragment-shader-reveal.png">
```
createRevealEffect = function()
{
// texture co-ordinate is from 0..1
// we shift the texture co-ordinate to -0.5 to 0.5 to center it
// then work out the radius from the centre, using ( a^2 + b^2) = c^2
// we use the dot product to perform the a^2 + b^2
// then just perform uRadius * uRadius to workout c^2
  
    var fragShader =
      " uniform lowp float uRadius; \
      \
       void main()             \
       {                  \
         lowp vec2 pos= vec2(vTexCoord.x-0.5,vTexCoord.y-0.5);    \
         lowp float radius = dot(pos, pos ) ; \
         if( radius > (uRadius*uRadius) )\
           discard; \
         gl_FragColor = texture2D( sTexture, vTexCoord ) * uColor ; \
       }"

      var shaderOptions = {
          geometryType: "image",
          fragmentShader: fragShader
      };
  
    // create a new shader effect
      var shader = new dali.ShaderEffect(shaderOptions);
  
      // set the uniform
      shader.setUniform("uRadius",0.0);
      return shader;
}
// create an image actor and add an image to it
var filename = getImageDirectory() +"gallery-medium-50.jpg;
var image = new dali.ResourceImage( {url: filename });
var imageActor = new dali.ImageActor( image );
imageActor.parentOrigin = dali.CENTER;
dali.stage.add( imageActor );
  
var revealEffect = createRevealEffect();
  
imageActor.setShaderEffect( revealEffect );
  
var shaderAnim = new dali.Animation(5);
  
var animOptions = {
          alpha: "easeInOutSine",
      };
  
// animate up to radius = 0.5 ( a full circle )
// if we go to 1.0 then it will go to a full box
shaderAnim.animateTo( revealEffect, "uRadius", 0.5,animOptions);
shaderAnim.play();

* * *
### Example of paper twisting in the wind with color (Vertex + Fragment Shader)

<img src="../assets/img/vertex-shader.png"><br>

The example does the following:

+ Creates a varying variable called vColor in the vertex shader.
 + The vColor is set to the position of the vertex, so the color changes depending on its position
+ Create a uniform called uOffset which modifies the xPosition of each vertex, creating a twist effect

An ImageActor normally only has 4 vertices ( quad ). To allow the image to twist and bend we need it to have more
vertices. To do this we can break the image into a grid using the gridX and gridY geometry hints.

<img src="../assets/img/shader-grid-hint.png">


createTwistEffect = function()
{

    // do some maths on the x-position to move it based on y,z pos + uOffset
    var vertexShader =
    " \
      varying lowp vec4  vColor; \
      uniform lowp float uOffset; \
      \
      void main() \
      {   \
        vec4 pos = uProjection * uModelView * vec4(aPosition, 1.0); \
        pos.x= 3*pos.z*(sin(1.57+uOffset+pos.y/1000.0)); \
        gl_Position =pos; \
          vColor = pos/500.0;\
          vTexCoord = aTexCoord; \
    }"
  
    // add the vColor to the pixel color to tint it
    var fragShader =
      " \
       varying lowp vec4 vColor; \
       void main()               \
       {                         \
         gl_FragColor = texture2D( sTexture, vTexCoord ) *uColor + vColor*0.2;  \
       }"
  
    var shaderOptions = {
          geometryType: "image",
          vertexShader: vertexShader,
          fragmentShader: fragShader,
          geometryHints: ["grid"]
    };
  
    // create a new shader effect
    var shader = new dali.ShaderEffect(shaderOptions);
  
    // set the uniform
    shader.setUniform("uOffset",0.0);
    return shader;
}
  

dali.stage.setBackgroundColor( dali.COLOR_WHITE);

var image = new dali.ResourceImage( { url:getImageDirectory() +"gallery-medium-50.jpg"});
var imageActor = new dali.ImageActor( image );
imageActor.parentOrigin = dali.CENTER;
imageActor.setCullFace( dali.CULL_FACE_DISABLE ); // disable face culling so we can see both sides
dali.stage.add( imageActor );
  
// start it of tilted around the y-axis
imageActor.rotation=new dali.Rotation(90, 0, 1, 0);
  
var twistEffect = createTwistEffect();
imageActor.setShaderEffect( twistEffect );
  
var shaderAnim = new dali.Animation(1);
var animOptions = {
          alpha: "bounce",
          duration:20
      };
shaderAnim.animateTo( twistEffect, "uOffset",20,animOptions);
  
shaderAnim.play();
```
Note: In order for fade and color animations to work, the fragment shader needs to multiply the fragment color
with the uniform color "uColor" of the node

@class ShaderEffect
*/