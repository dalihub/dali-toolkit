/**
 *
## Image API

An Image object represents a resource that can be added to Material.
  
The image is discarded when all Materials using the Image object are discarded.
  
Note: if a resource was shared between Image objects it exists until its last reference is gone.
  
Image objects are responsible for the underlying resource's lifetime.
  
### The hierarchy of Image is:

- {{#crossLink "image"}}Image base class {{/crossLink}} provides basic functionality
 - {{#crossLink "ResourceImage"}}ResourceImage {{/crossLink}} used for loading image files and nine-patch files
 - {{#crossLink "BufferImage"}}BufferImage {{/crossLink}} used for generating your own image
 - {{#crossLink "FrameBufferImage"}}FrameBufferImage {{/crossLink}} contains the result of an 'off screen' render pass of a RenderTask.

  
### Example:
```
// Create a resource image with a given URL
var image = new dali.ResourceImage( {url: "background.png"} );
  
// Create materials that specifie the visual properties of the renderer.
var material1 = new dali.Material();
var material2 = new dali.Material();
  
// Add images as texture to be used by the materials
material1.addTexture( image, "sTexture" );
material2.addTexture( image, "sTexture" );

```
The API supports functionality such as:

+ {{#crossLink "image/Image:method"}}new dali.ResourceImage{{/crossLink}}
+ {{#crossLink "image/getWidth:method"}}{{/crossLink}}
+ {{#crossLink "image/getHeight:method"}}{{/crossLink}}
  
### 9 patch images

The Image class also has support for loading 9 patch Images if the filename contains .9.
e.g.
```
var blackFrame = new dali.ResourceImage( {url:"black-frame.9.png"} );
var borderNinePatch = new dali.ResourceImage( {url:"border.9.png"} );
```
The nine patch image will scale automatically with the size of the actor.
  
Tool for making 9 patches
  
  http://romannurik.github.io/AndroidAssetStudio/nine-patches.html
  
More information on them:
  
  http://radleymarx.com/blog/simple-guide-to-9-patch/
  
  http://developer.android.com/tools/help/draw9patch.html
  


 @class Image
 */
