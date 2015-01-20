/**
## Image Actor API ( extends Actor API)

An actor for displaying images.

Allows the developer to add an actor to stage which displays the content of an Image object.

By default CullFaceMode is set to CullNone to enable the ImageActor to be viewed from all angles.

If an ImageActor is created without setting size, then the actor takes the size of the image -
this is the natural size.
Setting a size on the ImageActor, e.g through the SetSize api or through an animation will
stop the natural size being used.

Such a set size can be changed back to the image's size by calling {{#crossLink "ImageActor/setToNaturalSize:method"}}{{/crossLink}}  .

If a pixel area is set on an ImageActor with natural size, the actor size will change
to match the pixel area. If a pixel area is set on an ImageActor that has had it's size set,
then the size doesn't change, and the partial image will be stretched to fill the set size.

Clearing the pixel area on an Image actor with natural size will cause the actor to show the
whole image again, and will change size back to that of the image.

Clearing the pixel area on an Image actor with a set size will cause the actor to show the
whole image again, but will not change the image size.

### Simple example
```
var image = new dali.ResourceImage( {url:"background.png"} );
var imageActor = new dali.ImageActor( image );
  
// by default an actor is anchored to the top-left of it's parent actor
// change it to the middle
imageActor.parentOrigin = dali.CENTER;
  
// scale it up by 2 times  in x,y
imageActor.scale = [ 2, 2, 1  ];
  
// add to the stage
dali.stage.add( imageActor );
```

### Example using a pixel area ( needed for displaying images from a Texture Atlas )

```
var imageAtlas = new dali.ResourceImage( {url:"atlas.png"} )
```
<img src="../assets/img/example-javascript-code.png">

### Image Actor Specific Properties


|Name                   |    Type    | Writable     | Animatable|
|-----------------------|------------|--------------|-----------|
| pixelArea             | RECTANGE   | &#10004;     | &#10008;  |
| fadeIn                | BOOLEAN    | &#10004;     | &#10008;  |
| fadeInDuration        | FLOAT      | &#10004;     | &#10008;  |
| style                 | dali.IMAGE_ACTOR_STYLE_QUAD, dali.IMAGE_ACTOR_STYLE_NINE_PATCH, dali.IMAGE_ACTOR_STYLE_NINE_PATCH_NO_CENTER  | &#10004;     | &#10008;  |
| border                | VECTOR4    | &#10004;     | &#10008;  |


@class ImageActor
@extends RenderableActor
 */