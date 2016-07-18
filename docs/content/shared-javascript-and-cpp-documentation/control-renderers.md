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
 + [Image](@ref image-renderers)
 + [Border](@ref border-renderer)
 + [Mesh](@ref mesh-renderer)
 
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
| mixColor      | VECTOR4 | Yes      | The solid color required. |

### Usage

~~~{.cpp}
// C++
Dali::Toolkit::Control control = Dali::Toolkit::Control::New();

Dali::Property::Map map;
map[ "rendererType" ] = "color";
map[ "mixColor" ] = Color::RED;

control.SetProperty( Dali::Toolkit::Control::Property::BACKGROUND, map );
~~~

~~~{.js}
// JavaScript
var control = new dali.Control( "Control" );

control.background =
{
  rendererType : "color",
  mixColor : dali.COLOR_RED
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

| Property Name                                        | Type             | Required   | Description                                                              |
|------------------------------------------------------|:----------------:|:----------:|--------------------------------------------------------------------------|
| startPosition                                        | VECTOR2          | For Linear | The start position of the linear gradient.                               |
| endPosition                                          | VECTOR2          | For Linear | The end position of the linear gradient.                                 |
| center                                               | VECTOR2          | For Radial | The center point of the gradient.                                        |
| radius                                               | FLOAT            | For Radial | The size of the radius.                                                  |
| stopOffset                                           | ARRAY of FLOAT   | No         | All the stop offsets. If not supplied default is 0.0 and 1.0.            |
| stopColor                                            | ARRAY of VECTOR4 | Yes        | The color at those stop offsets. At least 2 required to show a gradient. |
| [units](@ref gradient-renderer-units)                | STRING           | No         | *OBJECT_BOUNDING_BOX* or *USER_SPACE*. Default: *OBJECT_BOUNDING_BOX*.   |
| [spreadMethod](@ref gradient-renderer-spread-method) | STRING           | No         | *PAD*, *REFLECT* or *REPEAT*. Default: *PAD*.                            |

If the *stopOffset* and *stopColor* arrays do not have the same number of elements, then the minimum of the two is used as the stop points.

### Units {#gradient-renderer-units}

Defines the coordinate system for the attributes:
 + Start (x1, y1) and End (x2 and y2) points of a line if using a linear gradient.
 + Center point (cx, cy) and radius (r) of a circle if using a radial gradient.
 
| Value               | Description                                                                                                                                    |
|---------------------|------------------------------------------------------------------------------------------------------------------------------------------------|
| OBJECT_BOUNDING_BOX | *Default*. Uses the normals for the start, end & center points, i.e. top-left is (-0.5, -0.5) and bottom-right is (0.5, 0.5).                  |
| USER_SPACE          | Uses the user coordinates for the start, end & center points, i.e. in a 200 by 200 control, top-left is (0, 0) and bottom-right is (200, 200). |

### Spread Method {#gradient-renderer-spread-method}

Indicates what happens if the gradient starts or ends inside the bounds of the target rectangle.

| Value   | Description                                                                                          |
|---------|------------------------------------------------------------------------------------------------------|
| PAD     | *Default*. Uses the terminal colors of the gradient to fill the remainder of the quad.               |
| REFLECT | Reflect the gradient pattern start-to-end, end-to-start, start-to-end etc. until the quad is filled. |
| REPEAT  | Repeat the gradient pattern start-to-end, start-to-end, start-to-end etc. until the quad is filled.  |

### Usage

**Linear:**
~~~{.cpp}
// C++
Dali::Toolkit::Control control = Dali::Toolkit::Control::New();

Dali::Property::Map map;
map[ "rendererType" ] = "gradient";
map[ "startPosition" ] = Vector2( 0.5f, 0.5f );
map[ "endPosition" ] = Vector2( -0.5f, -0.5f );

Dali::Property::Array stopOffsets;
stopOffsets.PushBack( 0.0f );
stopOffsets.PushBack( 0.3f );
stopOffsets.PushBack( 0.6f );
stopOffsets.PushBack( 0.8f );
stopOffsets.PushBack( 1.f );
map[ "stopOffset" ] = stopOffsets;

Dali::Property::Array stopColors;
stopColors.PushBack( Vector4( 129.f, 198.f, 193.f, 255.f )/255.f );
stopColors.PushBack( Vector4( 196.f, 198.f, 71.f, 122.f )/255.f );
stopColors.PushBack( Vector4( 214.f, 37.f, 139.f, 191.f )/255.f );
stopColors.PushBack( Vector4( 129.f, 198.f, 193.f, 150.f )/255.f );
stopColors.PushBack( Color::YELLOW );
map[ "stopColor" ] = stopColors;

control.SetProperty( Dali::Toolkit::Control::Property::BACKGROUND, map );
~~~

~~~{.js}
// JavaScript
var control = new dali.Control( "Control" );

control.background =
{
  rendererType : "gradient",
  startPosition : [ 0.5, 0.5 ],
  endPosition : [ -0.5, -0.5 ],
  stopOffset : [ 0.0, 0.3, 0.6, 0.8, 1.0 ],
  stopColor : [
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
map[ "center" ] = Vector2( 0.5f, 0.5f );
map[ "radius" ] = 1.414f;

Dali::Property::Array stopOffsets;
stopOffsets.PushBack( 0.0f );
stopOffsets.PushBack( 0.3f );
stopOffsets.PushBack( 0.6f );
stopOffsets.PushBack( 0.8f );
stopOffsets.PushBack( 1.f );
map[ "stopOffset" ] = stopOffsets;

Dali::Property::Array stopColors;
stopColors.PushBack( Vector4( 129.f, 198.f, 193.f, 255.f )/255.f );
stopColors.PushBack( Vector4( 196.f, 198.f, 71.f, 122.f )/255.f );
stopColors.PushBack( Vector4( 214.f, 37.f, 139.f, 191.f )/255.f );
stopColors.PushBack( Vector4( 129.f, 198.f, 193.f, 150.f )/255.f );
stopColors.PushBack( Color::YELLOW );
map[ "stopColor" ] = stopColors;

control.SetProperty( Dali::Toolkit::Control::Property::BACKGROUND, map );
~~~

~~~{.js}
// JavaScript
var control = new dali.Control( "Control" );

control.background =
{
  rendererType : "gradient",
  center : [ 0.5, 0.5 ],
  radius : 1.414,
  stopOffset : [ 0.0, 0.3, 0.6, 0.8, 1.0 ],
  stopColor : [
    [ 129 / 255, 198 / 255, 193 / 255, 255 / 255 ],
    [ 196 / 255, 198 / 255,  71 / 255, 122 / 255 ],
    [ 214 / 255,  37 / 255, 139 / 255, 191 / 255 ],
    [ 129 / 255, 198 / 255, 193 / 255, 150 / 255 ],
    dali.COLOR_YELLOW
  ]
};
~~~
___________________________________________________________________________________________________

## Image Renderers {#image-renderers}

Renders an image into the control's quad.
 
Depending on the extension of the image, different renderer is provided to render the image onto the screen.
 
 + [Normal](@ref image-renderer)
 + [N-Patch](@ref n-patch-renderer)
 + [SVG](@ref svg-renderer)
 
___________________________
 
### Normal {#image-renderer}
 
Renders a raster image ( jpg, png etc.) into the control's quad.
 
![ ](../assets/img/renderers/image-renderer.png)
![ ](renderers/image-renderer.png)

#### Properties Supported

**RendererType:** "image"

| Property Name                                        | Type     | Required | Description                                                                                                    |
|------------------------------------------------------|:--------:|:--------:|----------------------------------------------------------------------------------------------------------------|
| url                                                  | STRING   | Yes      | The URL of the image.                                                                                          |
| [fittingMode](@ref resourceimagescaling-fittingmode) | STRING   | No       | *SHRINK_TO_FIT*, *SCALE_TO_FILL*, *FIT_WIDTH* or *FIT_HEIGHT*. Default: *SHRINK_TO_FIT*.                       |
| [samplingMode](@ref resourceimagescaling-scaling)    | STRING   | No       | *BOX*, *NEAREST*, *LINEAR*, *BOX_THEN_NEAREST*, *BOX_THEN_LINEAR*, *NO_FILTER* or *DONT_CARE*. Default: *BOX*. |
| desiredWidth                                         | INT      | No       | The desired image width. Will use actual image width if not specified.                                         |
| desiredHeight                                        | INT      | No       | The desired image height. Will use actual image height if not specified.                                       |

#### Usage

~~~{.cpp}
// C++
Dali::Toolkit::Control control = Dali::Toolkit::Control::New();

Dali::Property::Map map;
map[ "rendererType" ] = "image";
map[ "url" ] = "path-to-image.jpg";

control.SetProperty( Dali::Toolkit::Control::Property::BACKGROUND, map );
~~~

~~~{.js}
// JavaScript
var control = new dali.Control( "Control" );

control.background =
{
  rendererType : "image",
  url : "path-to-image.jpg"
};
~~~
___________________________________________________________________________________________________

### N-Patch {#n-patch-renderer}

Renders an n-patch or a 9-patch image into the control's quad.
 
![ ](../assets/img/renderers/n-patch-renderer.png)
![ ](renderers/n-patch-renderer.png)

#### Properties Supported

**RendererType:** "image"

| Property Name | Type    | Required | Description                      |
|---------------|:-------:|:--------:|----------------------------------|
| url           | STRING  | Yes      | The URL of the n-patch image.    |
| borderOnly    | BOOLEAN | No       | If true, only draws the borders. |

#### Usage

~~~{.cpp}
// C++
Dali::Toolkit::Control control = Dali::Toolkit::Control::New();

Dali::Property::Map map;

map[ "rendererType" ] = "image";
map[ "url" ] = "path-to-image.9.png";

control.SetProperty( Dali::Toolkit::Control::Property::BACKGROUND, map );
~~~

~~~{.js}
// JavaScript
var control = new dali.Control( "Control" );

control.background =
{
  rendererType : "image",
  url : "path-to-image.9.png"
};
~~~

___________________________________________________________________________________________________

### SVG {#svg-renderer}

Renders a svg image into the control's quad.
 
#### Features: SVG Tiny 1.2 specification

**supported:**
 
  * basic shapes
  * paths
  * solid color fill
  * gradient color fill
  * solid color stroke
 
**not supported:**
 
  * gradient color stroke
  * dash array stroke
  * view box
  * text
  * clip path

<div style="width:300px">
 
![ ](../assets/img/renderers/svg-renderer.svg)
 
</div>
 
<div style="width:300px">
 
![ ](renderers/svg-renderer.svg)
 
</div>

 
#### Properties Supported

**RendererType:** "image"

| Property Name | Type    | Required | Description                      |
|---------------|:-------:|:--------:|----------------------------------|
| url           | STRING  | Yes      | The URL of the SVG image.    |

#### Usage

~~~{.cpp}
// C++
Dali::Toolkit::Control control = Dali::Toolkit::Control::New();

Dali::Property::Map map;

map[ "rendererType" ] = "image";
map[ "url" ] = "path-to-image.svg";

control.SetSize( 200.f, 200.f );
control.SetProperty( Dali::Toolkit::Control::Property::BACKGROUND, map );
~~~

~~~{.js}
// JavaScript
var control = new dali.Control( "Control" );

control.background =
{
  rendererType : "image",
  url : "path-to-image.svg"
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

___________________________________________________________________________________________________

## Mesh Renderer {#mesh-renderer}

Renders a mesh using a .obj file, optionally with textures provided by a mtl file. Scaled to fit the control.

![ ](../assets/img/renderers/mesh-renderer.png)
![ ](renderers/mesh-renderer.png)

### Properties Supported

**RendererType** "mesh"

| Property Name                                | Type    | Required           | Description                                                                    |
|----------------------------------------------|:-------:|:------------------:|--------------------------------------------------------------------------------|
| objectUrl                                    | STRING  | Yes                | The location of the ".obj" file.                                               |
| materialUrl                                  | STRING  | No                 | The location of the ".mtl" file. Leave blank for a textureless object.         |
| texturesPath                                 | STRING  | If using material  | Path to the directory the textures (including gloss and normal) are stored in. |
| [shaderType](@ref mesh-renderer-shader-type) | STRING  | No                 | Sets the type of shader to be used with the mesh.                              |

### Shader Type {#mesh-renderer-shader-type}

When specifying the shader type, if anything the shader requires is missing, a simpler type that can be handled with what has been supplied will be used instead.
 
**Possible values:**
 
| String Value    | Description                                    |
|-----------------|------------------------------------------------|
| TEXTURELESS     | *Simplest*. A flat color with shading is used. |
| DIFFUSE_TEXTURE | Textured.                                      |
| ALL_TEXTURES    | Has a gloss, normal map and texture map.       |

### Usage

~~~{.cpp}
// C++
Dali::Toolkit::Control control = Dali::Toolkit::Control::New();

Dali::Property::Map map;

map[ "rendererType" ] = "mesh";
map[ "objectUrl"    ] = "home/models/Dino.obj";
map[ "materialUrl"  ] = "home/models/Dino.mtl";
map[ "texturesPath" ] = "home/images/";

control.SetProperty( Dali::Toolkit::Control::Property::BACKGROUND, map );
~~~

@class _Guide_Control_Renderers

*/
