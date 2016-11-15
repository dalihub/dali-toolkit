<!--
/**-->

[TOC]

# Resource Image Scaling {#resourceimagescaling}
  
## Introduction {#resourceimagescaling-introduction}
  
Resource Image Scaling provides automatic image resizing (without changing aspect) based on settings provided by the developer.
This operation is performed at load time.
  
### Developer options:
* A target size of the image - this could be the full screen size for example.
* A Fitting mMde - This determines how the image is fitted to the target dimensions. If necessary the image will be cropped, or have borders added automatically.
* A Sampling Mode - This determines the quality of the scaling (by specifying the type of filtering to use).
  
### Benefits of Resource Image Scaling:
* Scaled image will typically be 1-to-1 size ratio with on screen pixels, giving quality benefits.
* Scaling performed at load time, so run time speed is improved.
* Ease of use allows applications handling lots of images of different sizes to be created quickly and easily.
  
## Use-Case Example {#resourceimagescaling-basicexample}
While common uses of images in DALi applications involve fixed sized images under the developer's control, e.g. for button backgrounds, in other cases such as galleries and wallpapers an application must display a variety of images and adapt to different screen sizes and densities.

There are more code examples later in this document under [API usage](#resourceimagescaling-apidetails). For now we will just give one full code example to show how this feature is used..
  
Let's say we are writing a home-screen application for a smartphone.
Here we have a large, square image that we want to set as the wallpaper on a tall and narrow phone screen.
We want to fill the screen without distorting the image or having black borders, and wasting as few pixels from the source image as possible.
  
![ ](../assets/img/image-scaling/example-scale-to-fill-problem.jpg)
![ ](example-scale-to-fill-problem.jpg)
  
DALi provides the concept of a `FittingMode` to specify how a source image is mapped into a target rectangle, and the one we need here is `FittingMode::SCALE_TO_FILL` as it guarrentees to cover all of the pixels of the target dimensions specified.
A second concept of a `SamplingMode` controls how source image pixels are combined during the scaling and allows the developer to trade speed for quality.
Since our image is to be loaded once and reused, we use `SamplingMode::BOX_THEN_LINEAR` which is the highest quality option.
  
In this case, `SCALE_TO_FILL` will perform this sequence of operations:
  
![ ](../assets/img/image-scaling/example-scale-to-fill-sequence.jpg) ![ ](example-scale-to-fill-sequence.jpg)
  
We can pass the stage dimensions to the `ResourceImage` creator function as the desired rectangle and ask it to map the image to the screen as shown here:
  
~~~{.cpp}
 // C++
 ResourceImage image = ResourceImage::New(
  "gallery-large-12.jpg",
  Dali::ImageDimensions( stage.GetSize().x, stage.GetSize().y ),
  Dali::FittingMode::SCALE_TO_FILL,
  Dali::SamplingMode::BOX_THEN_LINEAR );
~~~
~~~{.js}
// JavaScript
// First get stage dimensions into stageX and stageY ...
var image = new dali.ResourceImage( {
  url: "gallery-large-12.jpg",
  width: stageX,
  height: stageY,
  fittingMode: "SCALE_TO_FILL",
  samplingMode: "BOX_THEN_LINEAR"
});
~~~
  

## Workflow {#resourceimagescaling-workflow}
  
![ ](../assets/img/image-scaling/workflow-main.png) ![ ](workflow-main.png)
  
The workflow for achieving the final scaled image is (in order):
  
- Target Size: Determine target size (from source image size and any user specified target dimensions).
- Target Image Dimensions: Determine the size the image should be scaled to (taking Fitting Mode into account)
- Scaling: Perform a scale to target image dimensions using the specified Sampling mode.
- Crop or Add Borders: Automatically perfomed as necessary to maintain final target aspect (actual stored data size could be smaller).
  


### Determine Target Dimensions {#resourceimagescaling-targetdimensions}
  
![ ](../assets/img/image-scaling/workflow-1.png) ![ ](workflow-1.png)
  
An application has several options for specifying the target rectangle for the image to be fitted to.
The application may request dimensions through `ResourceImage::New()`:
  
  - `Not specifying either dimension`: IE. Width and Height set to 0 - The target dimensions become the same as the source.

  - `Just one dimension specified, Width OR Height (the other dimension set to 0)`:
    The unspecified dimension will be derived from the specified one whilst maintaining the aspect of the source image. The specified and calculated dimensions become the target dimensions. See more on this case [below](#resourceimagescalingzerodimensions).
     
  - `Width AND Height both specified` The requested dimensions pass straight through to become the target for fitting.
  
![ ](../assets/img/image-scaling/scaling-fitting-target-dimensions.png) ![ ](scaling-fitting-target-dimensions.png)

The result of this process is an `(x, y)` target size to fit the image in the next step.
  


### Target Image Dimensions {#resourceimagescaling-targetimagedimensions}

![ ](../assets/img/image-scaling/workflow-2.png) ![ ](workflow-2.png)
  
#### Fitting Mode {#resourceimagescaling-fittingmode}
  
DALi provides a number of strategies for mapping the pixels of an image onto the target box derived above.
It provides a `FittingMode` enumeration to the developer to select a mapping or fitting approach.
These are `SCALE_TO_FILL`, `SHRINK_TO_FIT`, `FIT_WIDTH`, and `FIT_HEIGHT` and their effect is best appreciated visually:
  
The operation of each of these modes is as follows:
  
| `FittingMode` | **Operation** |
| ------------- | ------------- |
| `SCALE_TO_FILL` | Centers the image on the target box and uniformly scales it so that it matches the target in one dimension and extends outside the target in the other. Chooses the dimension to match that results in the fewest pixels outside the target. Trims away the parts of the image outside the target box so as to match it exactly. This guarentees all of the target area is filled. |
| `SHRINK_TO_FIT` | Centers the image on the target box and uniformly scales it so that it matches the target in one dimension and fits inside it in the other. This guarentees that all of the source image area is visible. |
| `FIT_WIDTH` | Centers the image on the target box and uniformly scales it so that it matches the target width without regard for the target height. |
| `FIT_HEIGHT` | Centers the image on the target box and uniformly scales it so that it matches the target in height without regard for the target width. |
  

![ ](../assets/img/image-scaling/fitting-mode-options.png) ![ ](fitting-mode-options.png)
  
<sub> **Fitting modes**: *The top row shows the effect of each mode when a tall target rectangle is applied to a square image. The middle row applies a wide target to a square raw image. The bottom row uses a target with the same aspect ratio as the raw image. This example shows that `SCALE_TO_FILL` is the only option for which the dimensions of the fitted image result fill all the area of the target. Others would be letterboxed with borders. `SHRINK_TO_FIT` is always equal to one of `FIT_WIDTH` or `FIT_HEIGHT`: in each case it is the minimum of them. As a special case, where the aspect ratio of raw image and target match, all fitting modes generate an exact match final image and are equivalent to each other.* </sub>
  

Note: The image is scaled to the same aspect and shrunk to fit depending on fitting mode. It is not upscaled. See: [Upscaling](#resourceimagescalingupscaling).
  
  

### Scaling {#resourceimagescaling-scaling}
  
![ ](../assets/img/image-scaling/workflow-3.png) ![ ](workflow-3.png)
  
To perform the scaling stage, the source image is scaled to a (factor of) the target image size using the specified Sampling Mode/
  
The process of scaling an image can be expensive in CPU cycles and add latency to the loading of each resource.
To allow the developer to trade-off speed against quality for different use cases, DALi provides the `SamplingMode` enum, which can be passed to `ResourceImage::New()`.
Two of these modes produce bitmaps which differ from the dimensions calculated by the fitting algorithm and so have a memory trade-off as well. The full set of modes is explained below.
  
| `SamplingMode` | **Operation** |
| ------------- | --------- |
| `NEAREST` | Use simple point sampling when scaling. For each pixel in output image, just one pixel is chosen from the input image. This is the fastest, crudest option but suffers the worst from aliasing artifacts so should only be used for fast previews, or where the source image is known to have very low-frequency features. |
| `LINEAR` | Uses a weighted bilinear filter with a `(2,2)` footprint when scaling. For each output pixel, four input pixels are averaged from the input image. This is a good quality option, equivalent to the GPU's filtering and works well at least down to a `0.5` scaling. |
| `BOX` | Uses an iterated `(2,2)` box filter to repeatedly halve the image in both dimensions, averaging adjacent pixels until the the result is approximately right for the fitting target rectangle. For each output pixel some number of pixels from the sequence `[4,16,64,256,1024,...]` are averaged from the input image, where the number averaged depends on the degree of scaling requested. This provides a very high quality result and is free from aliasing artifacts because of the iterated averaging. *The resulting bitmap will not exactly match the dimensions calculated by the fitting mode but it will be within a factor of two of it and have the same aspect ratio as it.*   |
| `BOX_THEN_NEAREST` | Applies the `BOX` mode to get within a factor of two of the fitted dimensions, and then finishes off with `NEAREST` to reach the exact dimensions. |
| `BOX_THEN_LINEAR` | Applies the `BOX` mode to get within a factor of two of the fitted dimensions, and then finishes off with `LINEAR` to reach the exact dimensions. This is the slowest option and of equivalent quality to `BOX`. It is superior to `BOX` in that is uses an average of 62% of the memory and exactly matches the dimensions calculated by fitting. **This is the best mode for most use cases**.  |
| `NO_FILTER` | Disables scaling altogether. In conjunction with `SCALE_TO_FILL` mode this can be useful as the edge trimming of that fitting mode is still applied. An example would be a gallery application, where a database of prescaled thumbnails of approximately the correct size need to be displayed in a regular grid of equal-sized cells, while being loaded at maximum speed. |
  

Here are all the modes applied to scaling-down a `(640,720)` line art and text JPEG image to a `(218, 227)` thumbnail:
  
|  |  | |
| ---- | ---- | --- |
| ![ ](../assets/img/image-scaling/sampling_modes_no_filter.png) ![ ](sampling_modes_no_filter.png) | ![ ](../assets/img/image-scaling/sampling_modes_nearest.png) ![ ](sampling_modes_nearest.png) | ![ ](../assets/img/image-scaling/sampling_modes_linear.png) ![ ](sampling_modes_linear.png) |
| **NO_FILTER** | **NEAREST** | **LINEAR** |
| ![ ](../assets/img/image-scaling/sampling_modes_box.png) ![ ](sampling_modes_box.png) | ![ ](../assets/img/image-scaling/sampling_modes_box_then_nearest.png) ![ ](sampling_modes_box_then_nearest.png) | ![ ](../assets/img/image-scaling/sampling_modes_box_then_linear.png) ![ ](sampling_modes_box_then_linear.png) |
| **BOX** | **BOX_THEN_NEAREST** | **BOX_THEN_LINEAR** |
  
These are screenshots, showing how the images are rendered in a DALi demo.
There is an additional level of GPU bilinear filtering happening at render time.
The best way to get a feel for the best sampling mode for different image types is to play with the [examples](#resourceimagescaling-samplingmodesdemoexamples).
  
  

### Crop or Add Borders {#resourceimagescaling-croporaddborders}
  
![ ](../assets/img/image-scaling/workflow-4.png) ![ ](workflow-4.png)
  
Lastly, the image data will be cropped, or have borders added automatically as necessary.
This is done to ensure the image correctly fits the aspect of the target window, whilst maintaining the aspect of the source image.
  
Images that have an alpha channel will be given transparent borders. Otherwise black is used.
  
  

## Using the API (With source code examples) {#resourceimagescaling-apidetails}
  
This section contains more detail about using the API to setup the desired behaviour.

`ResourceImage` :: New has the following parameters:
- **path**: Identifier for the image (allows raw image width and height to be retrieved).
- **requested dimensions**: These are either `(0,0)`, a width, a height, or a (width, height) pair and either directly, or after reading the image raw dimensions and doing some math, define a target rectangle to fit the image to.
- **fitting mode**: one of four strategies for mapping images onto the target rectangle.
- **sampling mode** Different quality options for the scaling.
  
### Code Examples {#resourceimagescaling-targetdimensionsexamples}
If we have a `(320, 240)` image called "flower.jpg", we use these options in code as below.
  
**Case 1**: In these two equivalent loads, the target dimensions are not specified, so will be `(320, 240)` so the image will be loaded at its raw dimensions without modification.
~~~{.cpp}
// C++
ResourceImage image1 = ResourceImage::New( "flower.png" );
ResourceImage image2 = ResourceImage::New( "flower.png", ImageDimensions( 0, 0 ) );
~~~
~~~{.js}
// JavaScript
var image1 = new dali.ResourceImage( { url:"flower.png" } );
var image2 = new dali.ResourceImage( { url:"flower.png", width:0, height:0 } );
~~~
  

**Case 2**: In these loads, the target dimensions will be `(160, 120)` as the zero dimension is derived from the aspect ratio of the raw image.
~~~{.cpp}
// C++
ResourceImage image1 = ResourceImage::New( "flower.png", ImageDimensions( 160, 0 ) );
ResourceImage image2 = ResourceImage::New( "flower.png", ImageDimensions( 0, 120 ) );
~~~
~~~{.js}
// JavaScript
var image1 = new dali.ResourceImage( { url:"flower.png", width:160, height:0 } );
var image2 = new dali.ResourceImage( { url:"flower.png", width:0, height:120 } );
~~~
  

**Case 3**: In this load, the target dimensions will be `(111, 233)`.
~~~{.cpp}
// C++
ResourceImage image = ResourceImage::New( "flower.png", ImageDimensions( 111, 233 ) );
~~~
~~~{.js}
// JavaScript
var image = new dali.ResourceImage( { url:"flower.png", width:111, height:233 } );
~~~
  

### Fitting an image's dimensions to the target box {#resourceimagescaling-codeexamplesfittingmodes}
  
The result of the fitting modes defined [here](#resourceimagescaling-targetimagedimensions) only differ when the target box has a different aspect ratio than the source image.
Images may still be scaled down, depending on the target dimensions, but the specified fitting mode will not have an effect.
  
EG:
~~~{.cpp}
// C++
// Image on 'disk' is 320x240.
ResourceImage image = ResourceImage::New( "flower.png", ImageDimensions( 32, 24 ) );
// Image will be loaded at (32, 24), regardless of fitting mode.
~~~
~~~{.js}
// JavaScript
// Image on 'disk' is 320x240.
var image = new dali.ResourceImage( { url:"flower.png", width:32, height:24});
// Image will be loaded at (32, 24), regardless of fitting mode.
~~~
  

### Passing a Zero Dimension {#resourceimagescalingzerodimensions}
  
Passing in a single zero dimension is equivalent to specifying `FIT_WIDTH` or `FIT_HEIGHT` `FittingMode`s. When a non-zero width and zero height are specified, the fitting done will be identical to the result using `FittingMode` `FIT_WIDTH`. When passing a zero width and non-zero height, the effect of applying the chosen `FittingMode` to the calculated target dimensions is always identical to applying the `FIT_HEIGHT` mode.
  
* `ResourceImage::New( ImageDimensions( x, 0 ), <ANY_FITTING_MODE> )` =
  `ResourceImage::New( ImageDimensions( x, <ANYTHING> ), FittingMode::FIT_WIDTH )`
* `ResourceImage::New( ImageDimensions( 0, y ), <ANY_FITTING_MODE> )` =
  `ResourceImage::New( ImageDimensions( <ANYTHING>, y), FittingMode::FIT_HEIGHT )`
  
This falls out of the the fact that the fitting modes are strategies for the case when the aspect ratio of the raw image differs from the aspect ratio of the target dimensions, but the zero dimension behavior always ensures that the target dimensions have the same aspect ratio as the raw image's so the fitting modes are all equivalent.
  
Therefore, if `(x!=0, y=0)`, fittingMode = `FIT_WIDTH`,
and if `(x=0, y=!0)`, fittingMode = `FIT_HEIGHT`, irrespective of fitting mode passed by the application (if any).
This shortcut is provided as a convenience to the developer and allows FIT_WIDTH or FIT_HEIGHT to be specified compactly:
~~~{.cpp}
// C++
// FIT_WIDTH:
ResourceImage image = ResourceImage::New("flower.png", ImageDimensions(x, 0));
// FIT_HEIGHT:
ResourceImage image = ResourceImage::New("flower.png", ImageDimensions(0, y));
~~~
~~~{.js}
// JavaScript
// FIT_WIDTH:
var image = new dali.ResourceImage( {
  url: "flower.png",
  width: x,
  height: 0
});
// FIT_HEIGHT:
var image = new dali.ResourceImage( {
  url: "flower.png",
  width: 0,
  height: y
});
~~~
  
Note:
- If both values are specified as 0, both dimensions are taken from the source image.
- If both dimensions are not 0, this value becomes the 'natural size' even if it differs from the actual pixel dimensions loaded. [This requires some care in rendering to avoid distortion](#resourceimagescaling-samplingmodesrendernaturalsize).
  

### Code Examples for Sampling Modes  {#resourceimagescaling-codeexamplessamplingmodes}
  

In the following code example an image is loaded to be a thumbnail but with differing quality, speed, and memory implications.
~~~{.cpp}
// C++
ResourceImage image1 = ResourceImage::New( "flower.png",
    ImageDimensions( 240, 240 ), FittingMode::SCALE_TO_FILL, SamplingMode::NEAREST );
  
ResourceImage image2 = ResourceImage::New( "flower.png",
    ImageDimensions( 240, 240 ), FittingMode::SCALE_TO_FILL, SamplingMode::NO_FILTER );
  
ResourceImage image3 = ResourceImage::New( "flower.png",
    ImageDimensions( 240, 240 ), FittingMode::SCALE_TO_FILL, SamplingMode::BOX );
  
ResourceImage image4 = ResourceImage::New( "flower.png",
    ImageDimensions( 240, 240 ), FittingMode::SCALE_TO_FILL, SamplingMode::BOX_THEN_LINEAR );
~~~
~~~{.js}
// JavaScript
var image1 = new dali.ResourceImage( {
  url:"flower.png", width:240, height:240,
  fittingMode:"SCALE_TO_FILL", samplingMode:"NEAREST"
} );

var image2 = new dali.ResourceImage( {
  url:"flower.png", width:240, height:240,
  fittingMode:"SCALE_TO_FILL", samplingMode:"NO_FILTER"
} );

var image3 = new dali.ResourceImage( {
  url:"flower.png", width:240, height:240,
  fittingMode:"SCALE_TO_FILL", samplingMode:"BOX"
} );

var image4 = new dali.ResourceImage( {
  url:"flower.png", width:240, height:240,
  fittingMode:"SCALE_TO_FILL", samplingMode:"BOX_THEN_LINEAR"
} );
~~~
  
  


### Notes on speed VS quality {#resourceimagescaling-speedvsquality}
  
If we imagine flower.jpg is a 560*512 photo with high frequency details, the results of this are (image references are from above example):
* `image1` loads fast, uses minimal space, has poor quality.
* `image2` loads even faster, uses 4.6 * minimal space, has good quality.
* `image3` loads moderately slow, uses 1.3 * minimal space, has good quality.
* `image4` loads slowest, uses minimal space, has good quality.
  
Note that `BOX`, `BOX_THEN_NEAREST` and `BOX_THEN_LINEAR` can work particularly well for JPEG images as they can use fast downscaling typically built-in to the JPEG codec on supported platforms on the fly while decoding. In this case the caveats about using them having a speed trade-off given above do not apply.
  
  


## Demo Examples {#resourceimagescaling-samplingmodesdemoexamples}
  
Load time image scaling is spread throughout the DALi examples.
Search for `"ImageDimensions"` in the dali-demo project to see it used.
There is also a specific demo to show all of the fitting and scaling modes.
which lives in the demo project at `examples/image-scaling-and-filtering`.
  
![ ](../assets/img/image-scaling/demo-fitting-sampling.png) ![ ](./demo-fitting-sampling.png)
  
Touch the arrows in the top corners to changes image.
Drag the resize handle in the corner of the image to change the requested size and trigger an immediate image reload.
Use the buttons at the bottom of the screen to select any of the fitting and sampling modes from the popups which appear.
This demo does not take any of the special measures [described here](#resourceimagescaling-naturalsizecompensation) to correct for the natural size != pixel dimensions discrepancy so all fitting modes other than `SCALE_TO_FILL` show distortion.
  
A second specific demo shows the effect of a filter mode on a single image loaded into various requested rectangles side by side.
It can be found under `examples/image-scaling-irregular-grid`.
  
![ ](../assets/img/image-scaling/demo-sampling-modes.jpg) ![ ](./demo-sampling-modes.jpg)
  
Touch the button at top-left to change image.
The button at top-right changes sampling mode.
You will see strong differences between sampling modes where the image contains high frequency details such as hair and in the large black and white image, but much less in some others such as the Statue of Liberty which is mostly covered by a smooth gradient.
  
  

## Further Notes {#resourceimagescaling-furthernotes}
  
### Upscaling {#resourceimagescaling-upscaling}
  
DALi refuses to upscale images at load time in order to conserve memory.
If the application requests an image size the specified fitting mode) would require scaling up, DALi will instead return an image with the same aspect ratio but limited to the largest dimensions that do not exceed the raw ones.
EG. The actual image could be a fraction of the size of the target image dimensions.
Upscaling can still be effected at render time by setting the size of an actor to the desired size.
  
  

### Compressed Textures and Scaling {#resourceimagescaling-compressedtextures}
  
Compressed textures cannot be scaled at load time as their formats are designed to be uploaded directly to GPU memory. To achieve scaling of compressed textures, set the desired size on the attached `ImageView` for scaling at render time instead.
  
  

### Compensation for Natural Size != Pixel Width / Height {#resourceimagescaling-naturalsizecompensation}
  
Because the *natural size* of an image is
[taken from the requested dimensions](#resourceimagescaling-samplingmodesdimensionflow)
passed to `ResourceImage::New()` rather than passing through the same calculations that result in the eventual pixel width and height loaded,
the *natural size* and pixel dimensions of an image will differ when loaded with scaling.
It is inherent in the definition of fitting modes other than `SCALE_TO_FILL` not to match the requested dimensions, so in general, images loaded with them must have this mismatch between *natural size* and actual pixel width.
  
It is not possible in general to draw a scaled resource image using its natural size as the `ImageView`'s size without it appearing stretched in one dimension.
This is the case for example by default with size negotiation in effect or when an image is simply passed to an actor at creation time.
  
There are circumstance, however, in which the the natural size of a resource image loaded will exactly match its post-load pixel dimensions:
  
- No scaling is requested.
- The application chooses a combination of requested dimensions, fitting mode, and sampling mode which the scaling sub-system can match exactly. This is the case:
   *  For all downscaling using `SCALE_TO_FILL` fitting mode and not using `BOX` or `NO_FILTER` sampling modes.
   * The app uses `SHRINK_TO_FIT`, `FIT_WIDTH`, or `FIT_HEIGHT` and the requested dimensions passed-in are both smaller than the raw ones and have the same aspect ratio as them, and it is not using `BOX` or `NO_FILTER` sampling modes.
  
In these cases the image may be used freely in layouts controlled by size negotiation.
Additionally, if the requested size has the same aspect ratio as the eventual pixel array loaded, and the fitting mode is `SCALE_TO_FILL` or `BOX` and `NO_FILTER` sampling modes are avoided, even if they don't match in dimensions exactly, the eventual image will be drawn without aspect ratio distortion although it will be scaled at render time.
  
The fitting and scaling modes [demo](#resourceimagescalingsamplingmodesdemoexamples) allows this behavior to be be explored dynamically when the fitting mode is changed from `SCALE_TO_FILL`.
  
The application can of course only pass dimensions which are just right if it happens to know the raw dimensions or if it accesses the the image resource and reads the raw dimensions from its header.
  
The application can get a scaled resource image rendered correctly to screen with one of three strategies:
  
  1. Use one of the special cases above.
  2. Read the image header from disk, recreate the dimension deriving, fitting, and sampling logic described in this document, and use that to generate a pair of requested dimensions which match the eventual image dimensions.
  3. Use the requested dimensions it really wants to but then read the image header from disk, recreate the dimension deriving, fitting, and sampling logic described in this document, and set the size of an `ImageView` to that size explicitly rather than relying on the *natural size* of the image.
  


@class _Guide_Resource_Image_Scaling
*/
