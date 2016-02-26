/**
 *
## Animation API

   DALi Animation can be used to animate the properties of any number of objects, typically Actors.

The API supports functionality such as:

 - {{#crossLink "animation/play:method"}}{{/crossLink}}
 - {{#crossLink "animation/pause:method"}}{{/crossLink}}
 - {{#crossLink "animation/stop:method"}}{{/crossLink}}
 - {{#crossLink "animation/setLooping:method"}}{{/crossLink}}, set whether the animation should loop
 - {{#crossLink "animation/setSpeedFactor:method"}}{{/crossLink}}, speeds / slows down an animation
 - {{#crossLink "animation/setPlayRange:method"}}{{/crossLink}}, only play part of the animation between a set range
 - Key frame support. See {{#crossLink "animation/animateBetween:method"}}{{/crossLink}}
 - Path Animations. See {{#crossLink "path"}}Path {{/crossLink}}
 - Signals to be informed when an animation has finished.
 - animate multiple properties, owned by multiple objects with a single animation object
  
### Simple example of moving an actor to a set position

```
var myActor = new dali.TextActor( "hello world" );
  
myActor.parentOrigin = dali.CENTER;
dali.stage.add( myActor );
  
var anim = new dali.Animation( 2 ); // 2 seconds
  
// we're animation the property position of the actor.
anim.animateTo( myActor, "position", [100, 100, 0] );
  
function finished( animation )
{
  log("Animation finished \n");
}
  
anim.on("finished", finished );
  
anim.play();
```

### Multiple actor example

```
// Following demonstrates:
// - aimating multiple properties on an object (actor properties in this example)
// - animating multiple objects at the same time (2 actors in the example)
// - using the optional, animation options object to set a delay time and alpha function (easing)
  
// Sets the original position to be rotated and pushed into the distance
  
var myActor1 = new dali.TextActor( "Hello" );
var myActor2 = new dali.TextActor( "from DALi" );
  
// centre both actors to the middle of the screen
myActor1.parentOrigin = dali.CENTER;
myActor2.parentOrigin =  dali.CENTER;
myActor1.scale=[2,2,1]; // scale up x and y by 2
myActor2.scale=[2,2,1]; // scale up x and y by 2

  
// reposition them to the left / right, and push them away from the camera
myActor1.position=[-100,0,-2000];  // x = -100, y = 0 , z = -2000
myActor2.position=[ 100,0,-2000];  // x = 100, y = 0 , z = -2000
  
// start with actor rotated by 180 about x & y axis, so they can twist into place
function createAnimation() {
  
  var startRotation = new dali.Rotation(180, -180, 0);
  myActor1.orientation = startRotation;
  myActor2.orientation = startRotation;
  
  dali.stage.add( myActor1 );
  dali.stage.add( myActor2 );
  

  var anim = new dali.Animation(1); // default duration is increased if length of all animations is greater than it.
  
  var animOptions = {
      alpha: dali.ALPHA_FUNCTION_LINEAR,
      delay: 0.5,     // used to delay the start of the animation
      duration: 3,    // duration of the animation
      };
  
  // move myActor1 z position back to 0
  anim.animateTo(myActor1, "positionZ", 0, animOptions);
  
  //  rotate back to correct orientation
  var endRotation = new dali.Rotation(0,0,0);
  
  animOptions.alpha = dali.ALPHA_FUNCTION_EASE_IN_OUT_SINE;
  anim.animateTo(myActor1, "orientation", endRotation, animOptions);
  
  // Delay the myActor2  by  a second
  animOptions.delay = 0.0;
  animOptions.alpha = dali.ALPHA_FUNCTION_LINEAR;
  anim.animateTo(myActor2, "positionZ", 0, animOptions);
  
  //  rotate back to correct orientation
  animOptions.alpha = dali.ALPHA_FUNCTION_EASE_IN_OUT_SINE;
  anim.animateTo(myActor2, "orientation", endRotation, animOptions);

  return anim;
}


var anim = createAnimation();

anim.play();

```

### GL-ES shader animation example

The example below does the following with a single animation object:

 - rotates the image view
 - magnifies and color shifts the image using a fragment shader
  
![ ](../assets/img/shaders/shader-animation.png)

  

```
// Creates an image view in the centre of the stage
createImageView = function() {
  
  var imageView = new dali.Control("ImageView");
  imageView.parentOrigin = dali.CENTER;
  dali.stage.add( imageView );
  
  return imageView;
}
  
// Creates a simple fragment shader that has 2 uniforms.
// uColorShift which can add a color to pixel
// uScale which can simulate zooming into the texture
  
createColorShiftAndZoomEffect = function() {
  
  var fragShader =
      "varying vec2 vTexCoord;\
       uniform sampler2D sTexture;\
       uniform vec4 uColor;\
       uniform lowp vec4 uColorShift;\
       uniform lowp vec2 uScale;\
       void main() \
       {           \
         gl_FragColor = texture2D( sTexture, vTexCoord * uScale ) * uColor + uColorShift; \
       }";
  
  // vertexShader = "";  // vertex shader   ( optional)
  // fragmentShader = "";  // fragment shader   ( optional)
  // hints =   // shader hints   ( optional)
  //       [ "requiresSelfDepthTest",  // Expects depth testing enabled
  //         "outputIsTransparent",    // Might generate transparent alpha from opaque inputs
  //         "outputIsOpaque",         // Outputs opaque colors even if the inputs are transparent
  //         "modifiesGeometry" ];     // Might change position of vertices, this option disables any culling optimizations
  
  var shader = {
      "fragmentShader": fragShader,
      "hints" : "outputIsTransparent"
  };
  
  return shader;
}

createShaderAnimation = function( imageView, color, zoom, duration, delay )
{
  var shaderAnim = new dali.Animation(duration+delay);

  var animOptions = {
     alpha: dali.ALPHA_FUNCTION_EASE_IN_OUT_SINE,
     delay: delay,
     duration: duration,
  };

  // animate the color uniform
  shaderAnim.animateTo( imageView, "uColorShift", color, animOptions);

  // zoom in and out of the image while applying the color shift
  shaderAnim.animateTo( imageView, "uScale", zoom, animOptions);

  return shaderAnim;
}
  
var imageView = createImageView();
  
var shader = createColorShiftAndZoomEffect();
  
var image = {
    "rendererType" : "image",
    "imageUrl" : getImageDirectory()+"gallery-medium-50.jpg",
    "shader" : shader
};
  
imageView.image = image; // assign the shader to imageView
  
// register the color shift property so we can animate it
// default the color shift to zero, so it has no effect
imageView.registerAnimatableProperty("uColorShift", [0, 0, 0, 0]);
  
// register the zoom property so we can animate it
// default to 1,1 so no zoom is applied
imageView.registerAnimatableProperty("uScale", [1, 1]);
  
// create the shader animation
var zoom = [0.5,0.5];  // zoom into the image by 2
var color = dali.COLOR_BLUE; // color shift the image to blue
var duration = 5; // 5 seconds
var delay = 5; // wait 1 second before starting
var shaderAnim = createShaderAnimation( imageView, color,zoom, duration, delay);
  
// also rotate the imageView 90 degrees at the same time.
var rotation = new dali.Rotation(90,0,0,1);
shaderAnim.animateTo(imageView, "orientation", rotation, { alpha:dali.ALPHA_FUNCTION_LINEAR, duration:duration, delay:delay });


shaderAnim.play();

```


### Animation alpha functions

| Name                             | Description    |
|----------------------------------|----------------|
|ALPHA_FUNCTION_DEFAULT            |Linear          |
|ALPHA_FUNCTION_LINEAR             |Linear          |
|ALPHA_FUNCTION_REVERSE            |Reverse linear  |
|ALPHA_FUNCTION_EASE_IN_SQUARE     |Speeds up and comes to a sudden stop (Square) |
|ALPHA_FUNCTION_EASE_OUT_SQUARE    |Sudden start and slows to a gradual stop (Square) |
|ALPHA_FUNCTION_EASE_IN            |Speeds up and comes to a sudden stop |
|ALPHA_FUNCTION_EASE_OUT           |Sudden start and slows to a gradual stop|
|ALPHA_FUNCTION_EASE_IN_OUT        |Speeds up and slows to a gradual stop|
|ALPHA_FUNCTION_EASE_IN_SINE       |Speeds up and comes to a sudden stop|
|ALPHA_FUNCTION_EASE_OUT_SINE      |Sudden start and slows to a gradual stop|
|ALPHA_FUNCTION_EASE_IN_OUT_SINE   |Speeds up and slows to a gradual stop |
|ALPHA_FUNCTION_BOUNCE             |Sudden start, loses momentum and ** Returns to start position ** |
|ALPHA_FUNCTION_SIN                |full 360 revolution ** Returns to start position ** |
|ALPHA_FUNCTION_EASE_OUT_BACK      |Sudden start, exceed end position and return to a gradual stop|



 @class Animation

*/
