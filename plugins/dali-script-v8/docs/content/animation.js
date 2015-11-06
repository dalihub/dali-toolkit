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
  
anim.connect("finished", finished );
  
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
      alpha: "linear",
      delay: 0.5,     // used to delay the start of the animation
      duration: 3,    // duration of the animation
      };
  
  // move myActor1 z position back to 0
  anim.animateTo(myActor1, "positionZ", 0, animOptions);
  
  //  rotate back to correct orientation
  var endRotation = new dali.Rotation(0,0,0);
  
  animOptions.alpha = "easeInOutSine";
  anim.animateTo(myActor1, "orientation", endRotation, animOptions);
  
  // Delay the myActor2  by  a second
  animOptions.delay = 0.0;
  animOptions.alpha = "linear";
  anim.animateTo(myActor2, "positionZ", 0, animOptions);
  
  //  rotate back to correct orientation
  animOptions.alpha = "easeInOutSine";
  anim.animateTo(myActor2, "orientation", endRotation, animOptions);

  return anim;
}


var anim = createAnimation();

anim.play();

```

### GL-ES shader animation example

The example below does the following with a single animation object:

 - rotates the image actor
 - magnifies and color shifts the image using a fragment shader
  
![ ](../assets/img/shaders/shader-animation.png)

  

```
// create an image actor in the centre of the stage
createImageActor = function() {
  
  var image = new dali.ResourceImage({ url:getImageDirectory()+"gallery-medium-50.jpg"});
  var imageActor = new dali.ImageActor( image );
  imageActor.parentOrigin = dali.CENTER;
  dali.stage.add( imageActor );
  
  return imageActor;
}
  
// Creates a simple fragment shader that has 2 uniforms.
// uColorShift which can add a color to pixel
// uScale which can simulate zooming into the texture
  
createColorShiftAndZoomEffect = function() {
  
    var fragShader =
  " uniform lowp vec4 uColorShift; \
    uniform lowp vec2 uScale;    \
                     \
    void main() \
    {           \
      gl_FragColor = texture2D( sTexture, vTexCoord * uScale ) * uColor + uColorShift; \
    }"
  
  // Shader API
  // geometryType = "image", "text", "mesh", "texturedMesh"
  // fragmentPrefex ="" // prefix             ( optional)
  // fragmentShader = ""  // fragment shader   ( optional)
  // geometryHints = [ "gridX", "gridY", "grid","depthBuffer","blending" ]   ( optional)
  //
  var shaderOptions = {
      geometryType: "image",
      fragmentShader: fragShader,
      geometryHints: ["blending"]
  };
  
  // create a new shader effect
  var shader = new dali.ShaderEffect(shaderOptions);
  
  // add the color shift uniform so we can animate it
  // default the color shift to zero, so it has no effect
  shader.setUniform("uColorShift", [0, 0, 0, 0]);
  
  // add the zoom uniform so we can animate it
  // default to 1,1 so no zoom is applied
  var scale = new dali.Vector2([1, 1]);
  shader.setUniform("uScale", scale);
  
  return shader;
}

createShaderAnimation = function( shader, color, zoom, duration, delay )
{
    var shaderAnim = new dali.Animation(duration+delay);

    var animOptions = {
        alpha: "doubleEaseInOutSine60",
        delay: delay,
        duration: duration,
    };

    // animate the color uniform
    shaderAnim.animateTo( shader, "uColorShift", color, animOptions);

    // zoom in and out of the image while applying the color shift
    shaderAnim.animateTo( shader, "uScale", zoom, animOptions);

    return shaderAnim;
}
  
var imageActor = createImageActor();
var shaderEffect = createColorShiftAndZoomEffect();
  
// assign the shader effect to the actor ( it can be assigned to multiple actors).
imageActor.setShaderEffect( shaderEffect );
  
// create the shader animation
var zoom = [0.5,0.5];  // zoom into the image by 2
var color = dali.COLOR_BLUE; // color shift the image to blue
var duration = 5; // 5 seconds
var delay = 5; // wait 1 second before starting
var shaderAnim = createShaderAnimation( shaderEffect, color,zoom, duration, delay);
  
// also rotate the imageActor 90 degrees at the same time.
var rotation = new dali.Rotation(90,0,0,1);
shaderAnim.animateTo(imageActor, "orientation", rotation, { alpha:"linear", duration:duration, delay:delay });


shaderAnim.play();

```


### Animation alpha functions

| Name               | Description  |
|--------------------|--------------|
|default             |Linear          |
|linear              |Linear          |
|square              |Square (x^2)    |
|reverse             |Reverse linear  |
|easeIn              |Speeds up and comes to a sudden stop |
|easeOut             |Sudden start and slows to a gradual stop|
|easeInOut           |Speeds up and slows to a gradual stop|
|easeInSine          |Speeds up and comes to a sudden stop|
|easeOutSine         |Sudden start and slows to a gradual stop|
|easeInOutSine       |Speeds up and slows to a gradual stop |
|easeInSine33        |Speeds up and comes to a sudden stop  |
|easeOutSine33       |Sudden start and slows to a gradual stop |
|easeInOutSine33     |Speeds up and slows to a gradual stop |
|easeInOutSine50     |Speeds up and slows to a gradual stop |
|easeInOutSine60     |Speeds up and slows to a gradual stop |
|easeInOutSine70     |Speeds up and slows to a gradual stop |
|easeInOutSine80     |Speeds up and slows to a gradual stop |
|easeInOutSine90     |Speeds up and slows to a gradual stop |
|doubleEaseInOutSine6|Speeds up and slows to a gradual stop, then speeds up again and slows to a gradual stop |
|easeOutQuint50      |Sudden start and slows to a gradual stop  |
|easeOutQuint80      |Sudden start and slows to a gradual stop  |
|bounce              |Sudden start, loses momentum and ** Returns to start position ** |
|bounceBack          |Sudden start, loses momentum and returns to exceed start position ** Returns to start position ** |
|easeInBack          |Slow start, exceed start position and quickly reach destination |
|easeOutBack         |Sudden start, exceed end position and return to a gradual stop|
|easeInOutBack       |Slow start, exceed start position, fast middle, exceed end position and return to a gradual stop|
|sin                 |full 360 revolution ** Returns to start position ** |
|sin2x               |full 720 revolution ** Returns to start position ** |



 @class Animation

*/
