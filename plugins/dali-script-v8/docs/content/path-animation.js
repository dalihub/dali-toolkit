/**
 *
## Path Animation API

Paths can be used to animate position and orientation of actors.

![ ](../assets/img/path/path.png)
  
Example

```
log("Path animation test\n");
  
var point0 = [ 0.0, 80.0, 0.0 ];
var point1 = [ 200.0, 200.0, 0.0 ];
var point2 = [ 400.0, 80.0, 0.0 ];
  
//Create a path
var myPath = new dali.Path();
myPath.points = [ point0, point1, point2 ];
myPath.generateControlPoints( 0.25 );
  
function begin()
{
  
  var imageView = new dali.Control("ImageView");
  imageView.image = "gallery-small-45.jpg";
  
  imageView.position = [100,100,0];
  dali.stage.add( imageView );
  
  var animation = new dali.Animation(2.0);
  var animOptions = { alpha:dali.ALPHA_FUNCTION_EASE_IN_OUT_SINE, delay:0.5, duration:1.5 };
  var forward = new dali.Vector3(1,0,0);
  animation.animate( imageView, myPath, forward, animOptions );
  
  animation.setLooping( true );
  animation.play();
}
  
begin();
```
@class Path

*/