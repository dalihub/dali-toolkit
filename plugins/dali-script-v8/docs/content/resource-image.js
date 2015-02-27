/**
 *
## ResourceImage (extends Image)

A resource image extends the basic {{#crossLink "image"}}Image class{{/crossLink}} with
the ability to load an image from a file.

ResourceImage supports two types of load policies

- dali.IMAGE_LOAD_POLICY_IMMEDIATE //  load image once it is created (default)
- dali.IMAGE_LOAD_POLICY_ON_DEMAND // delay loading until the image is being used (a related actor is added to Stage)
  
### Simple example
```

var image = new dali.ResourceImage( { url:"my_image.png" } );

var imageActor = new dali.ImageActor( image );

```
### Advanced example
```

function imageLoaded( image )
{
  log("image loaded " +image.url + "\n");
}

var options = {
  url:"my_image.png",
  loadPolicy:dali.IMAGE_LOAD_POLICY_IMMEDIATE,
  releasePolicy: dali.IMAGE_RELEASE_POLICY_UNUSED
}
var image = new dali.ResourceImage( options );

image.connect("image-loading-finished", finished );

var imageActor = new dali.ImageActor( image );

```
@class ResourceImage
@extends Image
 */