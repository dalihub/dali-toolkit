/**
 *
## ResourceImage (extends Image)

A resource image extends the basic {{#crossLink "image"}}Image class{{/crossLink}} with
the ability to load an image from a file.

### Simple example
```

// Create a resource image with a given URL
var image = new dali.ResourceImage( {url: "my_image.png"} );
  
// Create a material that specifies the visual properties of the renderer.
var material = new dali.Material();
  
// Add the image as texture to be used by the material
material.addTexture( image, "sTexture" );
  
```
### Advanced example
```

function imageLoaded( image )
{
  log("image loaded " +image.url + "\n");
}
  
var image = new dali.ResourceImage( {url: "my_image.png"} );
  
image.on("imageLoadingFinished", finished );
  
// Create a material and add the image as texture to be used by the material.
var material = new dali.Material();
material.addTexture( image, "sTexture" );

```
@class ResourceImage
@extends Image
 */
