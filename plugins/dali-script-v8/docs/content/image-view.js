/**
 *
## ImageView API

   DALi ImageView can be used to display an image.

   It is preferable to set a valid size to the image for efficiency. However, do not set size
   that is bigger than the actual image size, as the up-scaling is not available, the content
   of the area not covered by actual image is undefined and it will not be cleared.

  
### Simple example of creating an ImageView

```
var imageView = new dali.Control("ImageView");
  
imageView.image = "myImage.jpg";
imageView.parentOrigin = dali.CENTER;
imageView.anchorPoint = dali.CENTER;
  
dali.stage.add( imageView );
```

### Advanced example of creating an ImageView and applying a GL-ES shader to it

```
// vertexShader = "";  // vertex shader   ( optional)
// fragmentShader = "";  // fragment shader   ( optional)
// hints =   // shader hints   ( optional)
//       [ "requiresSelfDepthTest",  // Expects depth testing enabled
//         "outputIsTransparent",    // Might generate transparent alpha from opaque inputs
//         "outputIsOpaque",         // Outputs opaque colors even if the inputs are transparent
//         "modifiesGeometry" ];     // Might change position of vertices, this option disables any culling optimizations
  
var shader = {
    "vertexShader": myVertexShader,
    "fragmentShader": myFragmentShader,
    "hints" : myHints
};
  
var imageView = new dali.Control("ImageView");
imageView.parentOrigin = dali.CENTER;
imageView.anchorPoint = dali.CENTER;
imageView.size = imageViewSize; // If not set, it will use either the natural image size or the desired image size if any
dali.stage.add( imageView );
  
var image = {
    "rendererType" : "image",
    "url" : "myImage.jpg",
    "desiredWidth" : desiredWidth,   // The desired image width while loading (optional but preferable to set for efficiency)
    "desiredHeight" : desiredHeight,   // The desired image height while loading (optional but preferable to set for efficiency)
    "shader" : shader  // Optional
};
  
imageView.image = image; // assign the image and shader to imageView

```


 @class ImageView
 @extends Actor

*/
