/**
 *
## Image API

An Image object represents a resource that can be added to ImageActors and ShaderEffects.
  
The image is discarded when all ImageActors using the Image object are discarded or in case they
were created with dali.IMAGE_RELEASE_POLICY_UNUSED, taken off stage.
  
Note: if a resource was shared between Image objects it exists until its last reference is gone.
  
Image objects are responsible for the underlying resource's lifetime.
  
### ReleasePolicies

 - dali.IMAGE_RELEASE_POLICY_UNUSED: release resource once ImageActor using it is taken off stage.
 - dali.IMAGE_RELEASE_POLICY_NEVER: keep resource alive until Image object is thrown away ( default).
  
#### Resolution of conflicting policies
 - If the same image is created more than once with conflicting policies, ReleasePolicy "Never" overrides "Unused".
  

### The hierarchy of Image is:

- {{#crossLink "image"}}Image base class {{/crossLink}} provides basic functionality
 - {{#crossLink "ResourceImage"}}ResourceImage {{/crossLink}} used for loading image files and nine-patch files
  - {{#crossLink "NinePatchImage"}}NinePatch {{/crossLink}} used just for nine patch files
 - {{#crossLink "BufferImage"}}BufferImage {{/crossLink}} used for generating your own image
 - {{#crossLink "FrameBufferImage"}}FrameBufferImage {{/crossLink}} contains the result of an 'off screen' render pass of a RenderTask.
 - {{#crossLink "NativeImage"}}NativeImage {{/crossLink}} Its data is provided by native resources, such as shared bitmap memory or pixmap from X11 or ECORE-X11, etc.

  
### Example:
```
var image = new dali.ResourceImage( {url: "background.png"} );
  
var imageActor1 = new dali.ImageActor( image );
var imageActor2 = new dali.ImageActor( image );
  
shaderEffect.setEffectImage( image );


```
The API supports functionality such as:

+ {{#crossLink "image/Image:method"}}new dali.ResourceImage{{/crossLink}}
+ {{#crossLink "image/getWidth:method"}}{{/crossLink}}
+ {{#crossLink "image/getHeight:method"}}{{/crossLink}}
  
### 9 patch images

The Image class also has support for loading 9 patch Images if the filename contains .9.
e.g.
```
var blackFrame = new dali.ResourceImage( url:"black-frame.9.png"});
var borderNinePatch = new dali.ResourceImage( {url:"border.9.png"} );
// or if the image doesn't have a .9 filename, you can use the NinePatchImage class
var image = new dali.NinePatchImage( {url:"my_image.png"})
```
The nine patch image will scale automatically with the size of the actor.
  
Tool for making 9 patches
  
  http://romannurik.github.io/AndroidAssetStudio/nine-patches.html
  
More information on them:
  
  http://radleymarx.com/blog/simple-guide-to-9-patch/
  
  http://developer.android.com/tools/help/draw9patch.html
  


 @class Image
 */
