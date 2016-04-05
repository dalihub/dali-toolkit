<!--
/**-->

# Control Renderers  {#control-renderers}

Control Renderers provide reusable renderering logic which can be used by all controls.
This means that custom controls do not have to create actors, they can just reuse the existing renderers which increases performance as well.
 
Control Renderers reuse geometry, shader etc. across controls and manages the renderer and material to exist only when the control is on-stage.
Additionaly, they respond to actor size and color change, while also providing clipping at the renderer level.
 
DALi provides the following renderers:
 + [Color](@ref color-renderer)
 + [Gradient](@ref gradient-renderer)
 + [Image](@ref image-renderer)
 + [N-Patch](@ref n-patch-renderer)
 + [SVG](@ref svg-renderer)
 + [Border](@ref border-renderer)
 
Controls can provide properties that allow users to specify the renderer type.
Setting renderer properties are done via a property map.
The **rendererType** field in the property map specifies the renderer to use/create.
This is required to avoid ambiguity as multiple renderers may be capable of rendering the same contents.
___________________________________________________________________________________________________

## Color Renderer {#color-renderer}

Renders a solid color to the control's quad.
 
![ ](../assets/img/renderers/color-renderer.png)
![ ](renderers/color-renderer.png)

### Properties Supported

**RendererType:** "color"

| Property Name | Type    | Required | Description               |
|---------------|:-------:|:--------:|---------------------------|
| blendColor    | VECTOR4 | Yes      | The solid color required. |

### Usage

~~~{.cpp}
// C++
Dali::Toolkit::Control control = Dali::Toolkit::Control::New();

Dali::Property::Map map;
map[ "rendererType" ] = "color";
map[ "blendColor" ] = Color::RED;

control.SetProperty( Dali::Toolkit::Control::Property::BACKGROUND, map );
~~~

~~~{.js}
// JavaScript
var control = new dali.Control( "Control" );

control.background =
{
  rendererType : "color",
  blendColor : dali.COLOR_RED
};
~~~
___________________________________________________________________________________________________

## Gradient Renderer {#gradient-renderer}

Renders a smooth transition of colors to the control's quad.
 
Both Linear and Radial gradients are supported.

| Linear | Radial |
|--------|--------|
| ![ ](../assets/img/renderers/linear-gradient-renderer.png) ![ ](renderers/linear-gradient-renderer.png) | ![ ](../assets/img/renderers/radial-gradient-renderer.png) ![ ](renderers/radial-gradient-renderer.png) |

### Properties Supported

**RendererType:** "gradient"

| Property Name                                                | Type             | Required   | Description                                                       |
|--------------------------------------------------------------|:----------------:|:----------:|-------------------------------------------------------------------|
| gradientStartPosition                                        | VECTOR2          | For Linear | The start position of the linear gradient.                        |
| gradientEndPosition                                          | VECTOR2          | For Linear | The end position of the linear gradient.                          |
| gradientCenter                                               | VECTOR2          | For Radial | The center point of the gradient.                                 |
| gradientRadius                                               | FLOAT            | For Radial | The size of the radius.                                           |
| gradientStopOffset                                           | ARRAY of FLOAT   | Yes        | All the stop offsets.                                             |
| gradientStopColor                                            | ARRAY of VECTOR4 | Yes        | The color at those stop offsets.                                  |
| [gradientUnits](@ref gradient-renderer-units)                | STRING           | No         | *objectBoundingBox* or *userSpace*. Default: *objectBoundingBox*. |
| [gradientSpreadMethod](@ref gradient-renderer-spread-method) | STRING           | No         | *pad*, *repeat* or *reflect*. Default: *pad*                      |

If the *gradientStopOffset* and *gradientStopColor* arrays do not have the same number of elements, then the minimum of the two is used as the stop points.

### Units {#gradient-renderer-units}

Defines the coordinate system for the attributes:
 + Start (x1, y1) and End (x2 and y2) points of a line if using a linear gradient.
 + Center point (cx, cy) and radius (r) of a circle if using a radial gradient.
 
| Value             | Description                                                                                                                                    |
|-------------------|------------------------------------------------------------------------------------------------------------------------------------------------|
| objectBoundingBox | *Default*. Uses the normals for the start, end & center points, i.e. top-left is (-0.5, -0.5) and bottom-right it (0.5, 0.5).                  |
| userSpace         | Uses the user coordinates for the start, end & center points, i.e. in a 200 by 200 control, top-left is (0, 0) and bottom-right is (200, 200). |

### Spread Method {#gradient-renderer-spread-method}

Indicates what happens if the gradient starts or ends inside the bounds of the target rectangle.

| Value   | Description                                                                                          |
|---------|------------------------------------------------------------------------------------------------------|
| pad     | *Default*. Uses the terminal colors of the gradient to fill the remainder of the quad.               |
| reflect | Reflect the gradient pattern start-to-end, end-to-start, start-to-end etc. until the quad is filled. |
| repeat  | Repeat the gradient pattern start-to-end, start-to-end, start-to-end until the quad is filled.       |

### Usage

**Linear:**
~~~{.cpp}
// C++
Dali::Toolkit::Control control = Dali::Toolkit::Control::New();

Dali::Property::Map map;
map[ "rendererType" ] = "gradient";
map[ "gradientStartPosition" ] = Vector2( 0.5f, 0.5f );
map[ "gradientEndPosition" ] = Vector2( -0.5f, -0.5f );

Dali::Property::Array stopOffsets;
stopOffsets.PushBack( 0.0f );
stopOffsets.PushBack( 0.3f );
stopOffsets.PushBack( 0.6f );
stopOffsets.PushBack( 0.8f );
stopOffsets.PushBack( 1.f );
map[ "gradientStopOffset" ] = stopOffsets;

Dali::Property::Array stopColors;
stopColors.PushBack( Vector4( 129.f, 198.f, 193.f, 255.f )/255.f );
stopColors.PushBack( Vector4( 196.f, 198.f, 71.f, 122.f )/255.f );
stopColors.PushBack( Vector4( 214.f, 37.f, 139.f, 191.f )/255.f );
stopColors.PushBack( Vector4( 129.f, 198.f, 193.f, 150.f )/255.f );
stopColors.PushBack( Color::YELLOW );
map[ "gradientStopColor" ] = stopColors;

control.SetProperty( Dali::Toolkit::Control::Property::BACKGROUND, map );
~~~

~~~{.js}
// JavaScript
var control = new dali.Control( "Control" );

control.background =
{
  rendererType : "gradient",
  gradientStartPosition : [ 0.5, 0.5 ],
  gradientEndPosition : [ -0.5, -0.5 ],
  gradientStopOffset : [ 0.0, 0.3, 0.6, 0.8, 1.0 ],
  gradientStopColor : [
    [ 129 / 255, 198 / 255, 193 / 255, 255 / 255 ],
    [ 196 / 255, 198 / 255,  71 / 255, 122 / 255 ],
    [ 214 / 255,  37 / 255, 139 / 255, 191 / 255 ],
    [ 129 / 255, 198 / 255, 193 / 255, 150 / 255 ],
    dali.COLOR_YELLOW
  ]
};
~~~

**Radial:**
~~~{.cpp}
// C++
Dali::Toolkit::Control control = Dali::Toolkit::Control::New();

Dali::Property::Map map;
map[ "rendererType" ] = "gradient";
map[ "gradientCenter" ] = Vector2( 0.5f, 0.5f );
map[ "gradientRadius" ] = 1.414f;

Dali::Property::Array stopOffsets;
stopOffsets.PushBack( 0.0f );
stopOffsets.PushBack( 0.3f );
stopOffsets.PushBack( 0.6f );
stopOffsets.PushBack( 0.8f );
stopOffsets.PushBack( 1.f );
map[ "gradientStopOffset" ] = stopOffsets;

Dali::Property::Array stopColors;
stopColors.PushBack( Vector4( 129.f, 198.f, 193.f, 255.f )/255.f );
stopColors.PushBack( Vector4( 196.f, 198.f, 71.f, 122.f )/255.f );
stopColors.PushBack( Vector4( 214.f, 37.f, 139.f, 191.f )/255.f );
stopColors.PushBack( Vector4( 129.f, 198.f, 193.f, 150.f )/255.f );
stopColors.PushBack( Color::YELLOW );
map[ "gradientStopColor" ] = stopColors;

control.SetProperty( Dali::Toolkit::Control::Property::BACKGROUND, map );
~~~

~~~{.js}
// JavaScript
var control = new dali.Control( "Control" );

control.background =
{
  rendererType : "gradient",
  gradientCenter : [ 0.5, 0.5 ],
  gradientRadius : 1.414,
  gradientStopOffset : [ 0.0, 0.3, 0.6, 0.8, 1.0 ],
  gradientStopColor : [
    [ 129 / 255, 198 / 255, 193 / 255, 255 / 255 ],
    [ 196 / 255, 198 / 255,  71 / 255, 122 / 255 ],
    [ 214 / 255,  37 / 255, 139 / 255, 191 / 255 ],
    [ 129 / 255, 198 / 255, 193 / 255, 150 / 255 ],
    dali.COLOR_YELLOW
  ]
};
~~~
___________________________________________________________________________________________________

## Image Renderer {#image-renderer}

Renders an image into the control's quad.
 
![ ](../assets/img/renderers/image-renderer.png)
![ ](renderers/image-renderer.png)

### Properties Supported

**RendererType:** "image"

| Property Name      | Type     | Required | Description                                                                                                                                     |
|--------------------|:--------:|:--------:|-------------------------------------------------------------------------------------------------------------------------------------------------|
| imageUrl           | STRING   | Yes      | The URL of the image.                                                                                                                           |
| [imageFittingMode](@ref resourceimagescaling-fittingmode) | STRING   | No       | *shrinkToFit*, *scaleToFill*, *fitWidth* or *fitHeight*. Default: *shrinkToFit*.                         |
| [imageSamplingMode](@ref resourceimagescaling-scaling)    | STRING   | No       | *box*, *nearest*, *linear*, *boxThenNearest*, *boxThenLinear*, *noFilter* or *dontCare*. Default: *box*. |
| imageDesiredWidth  | INT      | No       | The desired image width. Will use actual image width if not specified.                                                                          |
| imageDesiredHeight | INT      | No       | The desired image height. Will use actual image height if not specified.                                                                        |

### Usage

~~~{.cpp}
// C++
Dali::Toolkit::Control control = Dali::Toolkit::Control::New();

Dali::Property::Map map;
map[ "rendererType" ] = "image";
map[ "imageUrl" ] = "path-to-image.jpg";

control.SetProperty( Dali::Toolkit::Control::Property::BACKGROUND, map );
~~~

~~~{.js}
// JavaScript
var control = new dali.Control( "Control" );

control.background =
{
  rendererType : "image",
  imageUrl : "path-to-image.jpg"
};
~~~
___________________________________________________________________________________________________

## N-Patch Renderer {#n-patch-renderer}

Renders an n-patch or a 9-patch image into the control's quad.
 
![ ](../assets/img/renderers/n-patch-renderer.png)
![ ](renderers/n-patch-renderer.png)

### Properties Supported

**RendererType:** "nPatch"

| Property Name | Type    | Required | Description                      |
|---------------|:-------:|:--------:|----------------------------------|
| imageUrl      | STRING  | Yes      | The URL of the n-patch image.    |
| borderOnly    | BOOLEAN | No       | If true, only draws the borders. |

### Usage

~~~{.cpp}
// C++
Dali::Toolkit::Control control = Dali::Toolkit::Control::New();

Dali::Property::Map map;

map[ "rendererType" ] = "nPatch";
map[ "imageUrl" ] = "path-to-image.9.png";

control.SetProperty( Dali::Toolkit::Control::Property::BACKGROUND, map );
~~~

~~~{.js}
// JavaScript
var control = new dali.Control( "Control" );

control.background =
{
  rendererType : "nPatch",
  imageUrl : "path-to-image.9.png"
};
~~~

___________________________________________________________________________________________________

## SVG Renderer {#svg-renderer}

Renders a svg image into the control's quad.
 
<div style="width:300px">
 
![ ](../assets/img/renderers/svg-renderer.svg)
 
</div>
 
<div style="width:300px">
 
![ ](renderers/svg-renderer.svg)
 
</div>
 
### Properties Supported

**RendererType:** "svg"

| Property Name | Type    | Required | Description                      |
|---------------|:-------:|:--------:|----------------------------------|
| imageUrl      | STRING  | Yes      | The URL of the SVG image.    |

### Usage

~~~{.cpp}
// C++
Dali::Toolkit::Control control = Dali::Toolkit::Control::New();

Dali::Property::Map map;

map[ "rendererType" ] = "svg";
map[ "imageUrl" ] = "path-to-image.svg";

control.SetSize( 200.f, 200.f );
control.SetProperty( Dali::Toolkit::Control::Property::BACKGROUND, map );
~~~

~~~{.js}
// JavaScript
var control = new dali.Control( "Control" );

control.background =
{
  rendererType : "svg",
  imageUrl : "path-to-image.svg"
};
~~~
___________________________________________________________________________________________________

## Border Renderer {#border-renderer}

Renders a solid color as an internal border to the control's quad.
 
![ ](../assets/img/renderers/border-renderer.png)
![ ](renderers/border-renderer.png)

### Properties Supported

**RendererType:** "border"

| Property Name | Type    | Required | Description                                      |
|---------------|:-------:|:--------:|--------------------------------------------------|
| borderColor   | VECTOR4 | Yes      | The color of the border.                         |
| borderSize    | FLOAT   | Yes      | The width of the border (in pixels).             |
| antiAliasing  | BOOLEAN | No       | Whether anti-aliasing of the border is required. |

### Usage

~~~{.cpp}
// C++
Dali::Toolkit::Control control = Dali::Toolkit::Control::New();

Dali::Property::Map map;

map[ "rendererType" ] = "border";
map[ "borderColor"  ] = Color::BLUE;
map[ "borderSize"   ] = 5.0f;

control.SetProperty( Dali::Toolkit::Control::Property::BACKGROUND, map );
~~~

~~~{.js}
// JavaScript
var control = new dali.Control( "Control" );

control.background =
{
  rendererType : "border",
  borderColor : dali.COLOR_BLUE,
  borderSize = 5
};
~~~

@class _Guide_Control_Renderers

*/
