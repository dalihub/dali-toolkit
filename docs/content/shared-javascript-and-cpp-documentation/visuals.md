<!--
/**-->

# Visuals {#visuals}

Visuals provide reusable rendering logic which can be used by all controls.
This means that custom controls do not have to create actors, they can just reuse the existing visuals which increases performance.
 
Visuals reuse geometry, shaders etc. across controls and manages the renderer and texture to exist only when the control is on-stage.
Additionally, they respond to actor size and color change, while also providing clipping at the renderer level.
 
DALi provides the following visuals:
 + [Color](@ref color-visual)
 + [Gradient](@ref gradient-visual)
 + [Image](@ref image-visuals)
 + [Border](@ref border-visual)
 + [Mesh](@ref mesh-visual)
 + [Primitive](@ref primitive-visual)
 + [Text](@ref text-visual)
 + [Wireframe](@ref wireframe-visual)
 
Controls can provide properties that allow users to specify the visual type ( visualType ).
Setting visual properties are done via a property map.
The **visualType** field in the property map specifies the visual to use/create.
This is required to avoid ambiguity as multiple visuals may be capable of rendering the same contents.

Visuals have a **transform** field in the property map to allow layouting within a control. If this field is not set, then the visual defaults to filling the control. The **transform** field has a property map with the following keys:

| Property                                                       | String       | Type              | Required | Description                                                                                 |
|----------------------------------------------------------------|--------------|:-----------------:|:--------:|---------------------------------------------------------------------------------------------|
| Dali::Toolkit::DevelVisual::Transform::Property::OFFSET        | offset       | VECTOR2           | No       | The offset of the visual.                                                                   |
| Dali::Toolkit::DevelVisual::Transform::Property::SIZE          | size         | VECTOR2           | No       | The size of the visual.                                                                     |
| Dali::Toolkit::DevelVisual::Transform::Property::OFFSET_POLICY | offsetPolicy | VECTOR4           | No       | Whether the offset components are Relative or Absolute [More info](@ref offset-size-policy) |
| Dali::Toolkit::DevelVisual::Transform::Property::SIZE_POLICY   | sizePolicy   | VECTOR4           | No       | Whether the size components are Relative or Absolute [More info](@ref offset-size-policy)   |
| Dali::Toolkit::DevelVisual::Transform::Property::ORIGIN        | origin       | INTEGER or STRING | No       | The origin of the visual within the control's area. [More info](@ref align-type)            |
| Dali::Toolkit::DevelVisual::Transform::Property::ANCHOR_POINT  | anchorPoint  | INTEGER or STRING | No       | The anchor point of the visual. [More info](@ref align-type)                                |
 

## Offset & Size Policy  {#offset-size-policy}

The offset and size policies can be either Relative or Absolute.

| Enumeration                                             | String   | Description                                                                   |
|---------------------------------------------------------|----------|-------------------------------------------------------------------------------|
| Dali::Toolkit::DevelVisual::Transform::Policy::RELATIVE | RELATIVE | *Default*. The size or offset value represents a ratio of the control's size  |
| Dali::Toolkit::DevelVisual::Transform::Policy::ABSOLUTE | ABSOLUTE | The size or offset value represents world units (pixels)                      |

For example, an offsetPolicy of [ RELATIVE, RELATIVE ], a sizePolicy of [ ABSOLUTE, ABSOLUTE ], an offset of ( 0, 0.25 ) and a size of ( 20, 20 ) means the visual will be 20 pixels by 20 pixels in size, positioned 25% above the center of the control.

## Alignment  {#align-type}
| Enumeration                                          | String  | Description                                                                                          |
|------------------------------------------------------|---------|------------------------------------------------------------------------------------------------------|
| Dali::Toolkit::Align::TOP_BEGIN | TOP_BEGIN | Aligns to the top of the vertical axis and the beginning of the horizontal axis (The left or right edge in Left-To-Right or Right-to-Left layouts, respectively) |
| Dali::Toolkit::Align::TOP_CENTER | TOP_CENTER | Aligns to the top of the vertical axis and the center of the horizontal axis |
| Dali::Toolkit::Align::TOP_END | TOP_END | Aligns to the top of the vertical axis and end of the horizontal axis (The right or left edge in Left-To-Right or Right-to-Left layouts, respectively) |
| Dali::Toolkit::Align::CENTER_BEGIN | CENTER_BEGIN | Aligns to the center of the vertical axis and the beginning of the horizontal axis|
| Dali::Toolkit::Align::CENTER | CENTER | Aligns to the center of the control |
| Dali::Toolkit::Align::CENTER_END | CENTER_END | Aligns to the center of the vertical axis and end of the horizontal axis |
| Dali::Toolkit::Align::BOTTOM_BEGIN | BOTTOM_BEGIN | Aligns to the bottom of the vertical axis and the beginning of the horizontal axis|
| Dali::Toolkit::Align::BOTTOM_CENTER | BOTTOM_CENTER | Aligns to the bottom of the vertical axis and the center of the horizontal axis
| Dali::Toolkit::Align::BOTTOM_END | BOTTOM_END | Aligns to the bottom of the vertical axis and end of the horizontal axis |
 
Visuals also have a custom **shader** property. Whilst it's possible to change the shader, please note that some visuals rely on the vertex shader to perform certain functions. For example, the NPatch visual uses the vertex shader to perform the stretching. The **shader** property is a Property::Map with the following keys:


| Property                                                  | String         | Type                       | Required | Description                                                                                |
|-----------------------------------------------------------|----------------|:--------------------------:|:--------:|--------------------------------------------------------------------------------------------|
| Dali::Toolkit::Visual::Shader::Property::VERTEX_SHADER    | vertexShader   | STRING or ARRAY of STRING  | No       | The vertex shader code. Can use an array of strings to split shader over multiple lines.   |
| Dali::Toolkit::Visual::Shader::Property::FRAGMENT_SHADER  | fragmentShader | STRING or ARRAY of STRING  | No       | The fragment shader code. Can use an array of strings to split shader over multiple lines. |
| Dali::Toolkit::Visual::Shader::Property::SUBDIVIDE_GRID_X | subdivideGridX | INTEGER                    | No       | How to subdivide the grid along the X-Axis. Defaults to 1.                                 |
| Dali::Toolkit::Visual::Shader::Property::SUBDIVIDE_GRID_Y | subdivideGridY | INTEGER                    | No       | How to subdivide the grid along the Y-Axis. Defaults to 1.                                 |
| Dali::Toolkit::Visual::Shader::Property::HINTS            | hints          | INTEGER or ARRAY of STRING | No       | Shader hints bitmask [More info](@ref shader-hints)                                        |

## Shader hints {#shader-hints}

This is a bitmask giving hints to the renderer about what the shader does, in order to help the rendering system optimise it's rendering.

The bitmask can have the following values:

| Value | Description |
|-------------------------------------------|----------------------------------------|
| Dali::Shader::Hint::NONE | No hints |
| Dali::Shader::Hint::OUTPUT_IS_TRANSPARENT | Might generate transparent alpha from opaque inputs |
| Dali::Shader::Hint::MODIFIES_GEOMETRY | Might change the position of vertices - this disables culling optimizations |


See also Dali::Shader::Hint::Value enumeration.

___________________________________________________________________________________________________

## Color Visual {#color-visual}

Renders a color to the visual's quad geometry.
 
![ ](../assets/img/visuals/color-visual.png)
![ ](visuals/color-visual.png)

### Properties Supported

**VisualType:** Dali::Toolkit::Visual::COLOR, "COLOR"

| Property                                        | String   | Type    | Required | Description               |
|-------------------------------------------------|----------|:-------:|:--------:|---------------------------|
| Dali::Toolkit::ColorVisual::Property::MIX_COLOR | mixColor | VECTOR4 | Yes      | The color required. |

### Usage

~~~{.cpp}
// C++
Dali::Toolkit::Control control = Dali::Toolkit::Control::New();

Dali::Property::Map map;
map[ Visual::Property::TYPE ] = Dali::Toolkit::Visual::COLOR;
map[ ColorVisual::Property::MIX_COLOR ] = Color::RED;

control.SetProperty( Control::Property::BACKGROUND, map );
~~~

~~~{.js}
// JavaScript
var control = new dali.Control( "Control" );

control.background =
{
  visualType : "COLOR",
  mixColor : dali.COLOR_RED
};
~~~
___________________________________________________________________________________________________

## Gradient Visual {#gradient-visual}

Renders a smooth transition of colors to the visual's quad geometry.
 
Both Linear and Radial gradients are supported.

| Linear | Radial |
|--------|--------|
| ![ ](../assets/img/visuals/linear-gradient-visual.png) ![ ](visuals/linear-gradient-visual.png) | ![ ](../assets/img/visuals/radial-gradient-visual.png) ![ ](visuals/radial-gradient-visual.png) |

### Properties Supported

**VisualType:** Dali::Toolkit::Visual::GRADIENT, "GRADIENT"

| Property                                                | String        | Type              | Required   | Description                                                                                                      |
|---------------------------------------------------------|---------------|:-----------------:|:----------:|------------------------------------------------------------------------------------------------------------------|
| Dali::Toolkit::GradientVisual::Property::START_POSITION | startPosition | VECTOR2           | For Linear | The start position of the linear gradient.                                                                       |
| Dali::Toolkit::GradientVisual::Property::END_POSITION   | endPosition   | VECTOR2           | For Linear | The end position of the linear gradient.                                                                         |
| Dali::Toolkit::GradientVisual::Property::CENTER         | center        | VECTOR2           | For Radial | The center point of the gradient.                                                                                |
| Dali::Toolkit::GradientVisual::Property::RADIUS         | radius        | FLOAT             | For Radial | The size of the radius.                                                                                          |
| Dali::Toolkit::GradientVisual::Property::STOP_OFFSET    | stopOffset    | ARRAY of FLOAT    | No         | All the stop offsets. If not supplied default is 0.0 and 1.0.                                                    |
| Dali::Toolkit::GradientVisual::Property::STOP_COLOR     | stopColor     | ARRAY of VECTOR4  | Yes        | The color at those stop offsets. At least 2 required to show a gradient.                                         |
| Dali::Toolkit::GradientVisual::Property::UNITS          | units         | INTEGER or STRING | No         | Defines the coordinate system. [More info](@ref gradient-visual-units)                                           |
| Dali::Toolkit::GradientVisual::Property::SPREAD_METHOD  | spreadMethod  | INTEGER or STRING | No         | Indicates what happens if gradient starts or ends inside bounds. [More info](@ref gradient-visual-spread-method) |

If the *stopOffset* and *stopColor* arrays do not have the same number of elements, then the minimum of the two is used as the stop points.

### Units {#gradient-visual-units}

Defines the coordinate system for the attributes:
 + Start (x1, y1) and End (x2 and y2) points of a line if using a linear gradient.
 + Center point (cx, cy) and radius (r) of a circle if using a radial gradient.
 
| Enumeration                                               | String              | Description                                                                                                                                    |
|-----------------------------------------------------------|---------------------|------------------------------------------------------------------------------------------------------------------------------------------------|
| Dali::Toolkit::GradientVisual::Units::OBJECT_BOUNDING_BOX | OBJECT_BOUNDING_BOX | *Default*. Uses the normals for the start, end & center points, i.e. top-left is (-0.5, -0.5) and bottom-right is (0.5, 0.5).                  |
| Dali::Toolkit::GradientVisual::Units::USER_SPACE          | USER_SPACE          | Uses the user coordinates for the start, end & center points, i.e. in a 200 by 200 control, top-left is (0, 0) and bottom-right is (200, 200). |

### Spread Method {#gradient-visual-spread-method}

Indicates what happens if the gradient starts or ends inside the bounds of the target rectangle.

| Enumeration                                          | String  | Description                                                                                          |
|------------------------------------------------------|---------|------------------------------------------------------------------------------------------------------|
| Dali::Toolkit::GradientVisual::SpreadMethod::PAD     | PAD     | *Default*. Uses the terminal colors of the gradient to fill the remainder of the quad geometry.               |
| Dali::Toolkit::GradientVisual::SpreadMethod::REFLECT | REFLECT | Reflect the gradient pattern start-to-end, end-to-start, start-to-end etc. until the quad geometry is filled. |
| Dali::Toolkit::GradientVisual::SpreadMethod::REPEAT  | REPEAT  | Repeat the gradient pattern start-to-end, start-to-end, start-to-end etc. until the quad geometry is filled.  |

### Usage

**Linear:**
~~~{.cpp}
// C++
Dali::Toolkit::Control control = Dali::Toolkit::Control::New();

Dali::Property::Map map;
map[ Visual::Property::TYPE ] = Dali::Toolkit::Visual::GRADIENT;
map[ GradientVisual::Property::START_POSITION ] = Vector2( 0.5f, 0.5f );
map[ GradientVisual::Property::END_POSITION ] = Vector2( -0.5f, -0.5f );

Dali::Property::Array stopOffsets;
stopOffsets.PushBack( 0.0f );
stopOffsets.PushBack( 0.3f );
stopOffsets.PushBack( 0.6f );
stopOffsets.PushBack( 0.8f );
stopOffsets.PushBack( 1.f );
map[ GradientVisual::Property::STOP_OFFSET ] = stopOffsets;

Dali::Property::Array stopColors;
stopColors.PushBack( Vector4( 129.f, 198.f, 193.f, 255.f )/255.f );
stopColors.PushBack( Vector4( 196.f, 198.f, 71.f, 122.f )/255.f );
stopColors.PushBack( Vector4( 214.f, 37.f, 139.f, 191.f )/255.f );
stopColors.PushBack( Vector4( 129.f, 198.f, 193.f, 150.f )/255.f );
stopColors.PushBack( Color::YELLOW );
map[ GradientVisual::Property::STOP_COLOR ] = stopColors;

control.SetProperty( Control::Property::BACKGROUND, map );
~~~

~~~{.js}
// JavaScript
var control = new dali.Control( "Control" );

control.background =
{
  visualType : "GRADIENT",
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
map[ Visual::Property::TYPE ] = Dali::Toolkit::Visual::GRADIENT;
map[ GradientVisual::Property::CENTER ] = Vector2( 0.5f, 0.5f );
map[ GradientVisual::Property::RADIUS ] = 1.414f;

Dali::Property::Array stopOffsets;
stopOffsets.PushBack( 0.0f );
stopOffsets.PushBack( 0.3f );
stopOffsets.PushBack( 0.6f );
stopOffsets.PushBack( 0.8f );
stopOffsets.PushBack( 1.f );
map[ GradientVisual::Property::STOP_OFFSET ] = stopOffsets;

Dali::Property::Array stopColors;
stopColors.PushBack( Vector4( 129.f, 198.f, 193.f, 255.f )/255.f );
stopColors.PushBack( Vector4( 196.f, 198.f, 71.f, 122.f )/255.f );
stopColors.PushBack( Vector4( 214.f, 37.f, 139.f, 191.f )/255.f );
stopColors.PushBack( Vector4( 129.f, 198.f, 193.f, 150.f )/255.f );
stopColors.PushBack( Color::YELLOW );
map[ GradientVisual::Property::STOP_COLOR ] = stopColors;

control.SetProperty( Control::Property::BACKGROUND, map );
~~~

~~~{.js}
// JavaScript
var control = new dali.Control( "Control" );

control.background =
{
  visualType : "GRADIENT",
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

## Image Visual {#image-visuals}

Renders an image into the visual's geometry.
 
Depending on the extension of the image, a different visual is provided to render the image onto the screen.
 
 + [Normal (Quad)](@ref image-visual)
 + [N-Patch](@ref n-patch-visual)
 + [SVG](@ref svg-visual)
 + [Animated Image]( @ref animated-image-visual )
 
___________________________
 
### Normal {#image-visual}
 
Renders a raster image ( jpg, png etc.) into the visual's quad geometry.
 
![ ](../assets/img/visuals/image-visual.png)
![ ](visuals/image-visual.png)

#### Properties Supported

**VisualType:** Dali::Toolkit::Visual::IMAGE, "IMAGE"

| Property                                             | String        | Type              | Required | Description                                                                                                              |
|------------------------------------------------------|---------------|:-----------------:|:--------:|--------------------------------------------------------------------------------------------------------------------------|
| Dali::Toolkit::ImageVisual::Property::URL            | url           | STRING            | Yes      | The URL of the image.                                                                                                    |
| Dali::Toolkit::ImageVisual::Property::FITTING_MODE   | fittingMode   | INTEGER or STRING | No       | Fitting options, used when resizing images to fit desired dimensions. [More info](@ref resourceimagescaling-fittingmode) |
| Dali::Toolkit::ImageVisual::Property::SAMPLING_MODE  | samplingMode  | INTEGER or STRING | No       | Filtering options, used when resizing images to sample original pixels. [More info](@ref resourceimagescaling-scaling)   |
| Dali::Toolkit::ImageVisual::Property::DESIRED_WIDTH  | desiredWidth  | INT               | No       | The desired image width. Will use actual image width if not specified.                                                   |
| Dali::Toolkit::ImageVisual::Property::DESIRED_HEIGHT | desiredHeight | INT               | No       | The desired image height. Will use actual image height if not specified.                                                 |
| Dali::Toolkit::ImageVisual::Property::PIXEL_AREA     | pixelArea     | VECTOR4           | No       | The image area to be displayed, default value is [0.0, 0.0, 1.0, 1.0]                                                    |
| Dali::Toolkit::ImageVisual::Property::WRAP_MODE_U    | wrapModeU     | INTEGER or STRING | No       | Wrap mode for u coordinate, valid values are CLAMP_TO_EDGE(default), REPEAT, MIRRORED_REPEAT                               |
| Dali::Toolkit::ImageVisual::Property::WRAP_MODE_V    | wrapModeV     | INTEGER or STRING | No       | Wrap mode for v coordinate, valid values are CLAMP_TO_EDGE(default), REPEAT, MIRRORED_REPEAT                               |

#### Usage

~~~{.cpp}
// C++
Dali::Toolkit::Control control = Dali::Toolkit::Control::New();

Dali::Property::Map map;
map[ Visual::Property::TYPE ] = Dali::Toolkit::Visual::IMAGE;
map[ ImageVisual::Property::URL ] = "path-to-image.jpg";

control.SetProperty( Control::Property::BACKGROUND, map );
~~~

~~~{.js}
// JavaScript
var control = new dali.Control( "Control" );

control.background =
{
  visualType : "IMAGE",
  url : "path-to-image.jpg"
};
~~~
___________________________________________________________________________________________________

### N-Patch {#n-patch-visual}

Renders an n-patch or a 9-patch image. Uses non-quad geometry. Both geometry and texture are cached to reduce memory consumption if the same n-patch image is used elsewhere.
 
![ ](../assets/img/visuals/n-patch-visual.png)
![ ](visuals/n-patch-visual.png)

#### Properties Supported

**VisualType:** Dali::Toolkit::Visual::IMAGE, "IMAGE"

| Property                                          | String        | Type    | Required | Description                      |
|---------------------------------------------------|---------------|:-------:|:--------:|----------------------------------|
| Dali::Toolkit::ImageVisual::Property::URL         | url           | STRING  | Yes      | The URL of the n-patch image.    |
| Dali::Toolkit::ImageVisual::Property::BORDER_ONLY | borderOnly    | BOOLEAN | No       | If true, only draws the borders. |

#### Usage

~~~{.cpp}
// C++
Dali::Toolkit::Control control = Dali::Toolkit::Control::New();

Dali::Property::Map map;

map[ Visual::Property::TYPE ] = Dali::Toolkit::Visual::IMAGE;
map[ Dali::Toolkit::ImageVisual::Property::URL ] = "path-to-image.9.png";

control.SetProperty( Control::Property::BACKGROUND, map );
~~~

~~~{.js}
// JavaScript
var control = new dali.Control( "Control" );

control.background =
{
  visualType : "IMAGE",
  url : "path-to-image.9.png"
};
~~~

___________________________________________________________________________________________________

### SVG {#svg-visual}

Renders a svg image into the visual's quad geometry.
 
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
 
![ ](../assets/img/visuals/svg-visual.svg)
 
</div>
 
<div style="width:300px">
 
![ ](visuals/svg-visual.svg)
 
</div>

 
#### Properties Supported

**VisualType:** Dali::Toolkit::Visual::IMAGE, "IMAGE"

| Property                                  | String | Type    | Required | Description                      |
|-------------------------------------------|--------|:-------:|:--------:|----------------------------------|
| Dali::Toolkit::ImageVisual::Property::URL | url    | STRING  | Yes      | The URL of the SVG image.    |

#### Usage

~~~{.cpp}
// C++
Dali::Toolkit::Control control = Dali::Toolkit::Control::New();

Dali::Property::Map map;

map[ Visual::Property::TYPE ] = Dali::Toolkit::Visual::IMAGE;
map[ Dali::Toolkit::ImageVisual::Property::URL ] = "path-to-image.svg";

control.SetSize( 200.f, 200.f );
control.SetProperty( Control::Property::BACKGROUND, map );
~~~

~~~{.js}
// JavaScript
var control = new dali.Control( "Control" );

control.background =
{
  visualType : "IMAGE",
  url : "path-to-image.svg"
};
~~~

___________________________________________________________________________________________________

## Animated Image Visual {#animated-image-visual}

Renders an animated image into the visual's quad geometry. Currently, only the GIF format is supported.

![ ](../assets/img/visuals/animated-image-visual.gif)
![ ](animated-image-visual.gif)

#### Properties Supported

**VisualType:** Dali::Toolkit::Visual::IMAGE, "IMAGE"

| Property                                          | String     | Type              | Required | Description                                                                                  |
|---------------------------------------------------|------------|:-----------------:|:--------:|----------------------------------------------------------------------------------------------|
| Dali::Toolkit::ImageVisual::Property::URL         | url        | STRING            | Yes      | The URL of the animated image.                                                               |
| Dali::Toolkit::ImageVisual::Property::PIXEL_AREA  | pixelArea  | VECTOR4           | No       | The image area to be displayed, default value is [0.0, 0.0, 1.0, 1.0]                        |
| Dali::Toolkit::ImageVisual::Property::WRAP_MODE_U | wrapModeU  | INTEGER or STRING | No       | Wrap mode for u coordinate, valid values are CLAMP_TO_EDGE(default), REPEAT, MIRRORED_REPEAT |
| Dali::Toolkit::ImageVisual::Property::WRAP_MODE_V | wrapModeV  | INTEGER or STRING | No       | Wrap mode for v coordinate, valid values are CLAMP_TO_EDGE(default), REPEAT, MIRRORED_REPEAT |

#### Usage

~~~{.cpp}
// C++
Dali::Toolkit::Control control = Dali::Toolkit::Control::New();

control.SetProperty( Control::Property::BACKGROUND,
                     Property::Map().Add( Visual::Property::TYPE, Dali::Toolkit::Visual::IMAGE )
                                    .Add( Dali::Toolkit::ImageVisual::Property::URL, "path-to-image.gif" ) );
~~~

~~~{.js}
// JavaScript
var control = new dali.Control( "Control" );

control.background =
{
  visualType : "IMAGE",
  url : "path-to-image.gif"
};
~~~
___________________________________________________________________________________________________

## Border Visual {#border-visual}

Renders a color as an internal border to the visual's geometry.
 
![ ](../assets/img/visuals/border-visual.png)
![ ](visuals/border-visual.png)

### Properties Supported

**VisualType:** Dali::Toolkit::Visual::BORDER, "BORDER"

| Property                                             | String        | Type    | Required | Description                                      |
|------------------------------------------------------|---------------|:-------:|:--------:|--------------------------------------------------|
| Dali::Toolkit::BorderVisual::Property::COLOR         | borderColor   | VECTOR4 | Yes      | The color of the border.                         |
| Dali::Toolkit::BorderVisual::Property::SIZE          | borderSize    | FLOAT   | Yes      | The width of the border (in pixels).             |
| Dali::Toolkit::BorderVisual::Property::ANTI_ALIASING | antiAliasing  | BOOLEAN | No       | Whether anti-aliasing of the border is required. |

### Usage

~~~{.cpp}
// C++
Dali::Toolkit::Control control = Dali::Toolkit::Control::New();

Dali::Property::Map map;

map[ Visual::Property::TYPE ] = Dali::Toolkit::Visual::BORDER;
map[ BorderVisual::Property::COLOR ] = Color::BLUE;
map[ BorderVisual::Property::SIZE ] = 5.0f;

control.SetProperty( Control::Property::BACKGROUND, map );
~~~

~~~{.js}
// JavaScript
var control = new dali.Control( "Control" );

control.background =
{
  visualType : "BORDER",
  borderColor : dali.COLOR_BLUE,
  borderSize = 5
};
~~~

___________________________________________________________________________________________________

## Mesh Visual {#mesh-visual}

Renders a mesh using a .obj file, optionally with textures provided by a mtl file. Scaled to fit the control.
 
![ ](../assets/img/visuals/mesh-visual.png)
![ ](visuals/mesh-visual.png)
 
### Properties Supported
 
**VisualType:** Dali::Toolkit::Visual::MESH, "MESH"

| Property                                              | String         | Type               | Required          | Description                                                                                      |
|-------------------------------------------------------|----------------|:------------------:|:-----------------:|--------------------------------------------------------------------------------------------------|
| Dali::Toolkit::MeshVisual::Property::OBJECT_URL       | objectUrl      | STRING             | Yes               | The location of the ".obj" file.                                                                 |
| Dali::Toolkit::MeshVisual::Property::MATERIAL_URL     | materialUrl    | STRING             | No                | The location of the ".mtl" file. Leave blank for a textureless object.                           |
| Dali::Toolkit::MeshVisual::Property::TEXTURES_PATH    | texturesPath   | STRING             | If using material | Path to the directory the textures (including gloss and normal) are stored in.                   |
| Dali::Toolkit::MeshVisual::Property::SHADING_MODE     | shadingMode    | INTEGER or STRING  | No                | Sets the type of shading mode that the mesh will use. [More info](@ref mesh-visual-shading-mode) |
| Dali::Toolkit::MeshVisual::Property::USE_MIPMAPPING   | useMipmapping  | BOOLEAN            | No                | Flag for whether to use mipmaps for textures or not. Default true.                               |
| Dali::Toolkit::MeshVisual::Property::USE_SOFT_NORMALS | useSoftNormals | BOOLEAN            | No                | Flag for whether to average normals at each point to smooth textures or not. Default true.       |
| Dali::Toolkit::MeshVisual::Property::LIGHT_POSITION   | lightPosition  | VECTOR3            | No                | The position, in stage space, of the point light that applies lighting to the model.             |
 
### Shading Mode {#mesh-visual-shading-mode}

When specifying the shading mode, if anything the mode requires is missing, a simpler mode that can be handled with what has been supplied will be used instead.
 
**Possible values:**
 
| Enumeration                                                                     | String                                   | Description                                                                                                             |
|---------------------------------------------------------------------------------|------------------------------------------|-------------------------------------------------------------------------------------------------------------------------|
| Dali::Toolkit::MeshVisual::ShaderType::TEXTURELESS_WITH_DIFFUSE_LIGHTING        | TEXTURELESS_WITH_DIFFUSE_LIGHTING        | *Simplest*. One color that is lit by ambient and diffuse lighting.                                                      |
| Dali::Toolkit::MeshVisual::ShaderType::TEXTURED_WITH_SPECULAR_LIGHTING          | TEXTURED_WITH_SPECULAR_LIGHTING          | Uses only the visual image textures provided with specular lighting in addition to ambient and diffuse lighting.        |
| Dali::Toolkit::MeshVisual::ShaderType::TEXTURED_WITH_DETAILED_SPECULAR_LIGHTING | TEXTURED_WITH_DETAILED_SPECULAR_LIGHTING | Uses all textures provided including a gloss, normal and texture map along with specular, ambient and diffuse lighting. |

### Usage

~~~{.cpp}
// C++
Dali::Stage stage = Dali::Stage::GetCurrent();
Dali::Toolkit::Control control = Dali::Toolkit::Control::New();

Dali::Property::Map map;

map[ Visual::Property::TYPE  ] = Dali::Toolkit::Visual::MESH;
map[ MeshVisual::Property::OBJECT_URL ] = "home/models/Dino.obj";
map[ MeshVisual::Property::MATERIAL_URL ] = "home/models/Dino.mtl";
map[ MeshVisual::Property::TEXTURES_PATH ] = "home/images/";

control.SetProperty( Control::Property::BACKGROUND, map );
~~~

___________________________________________________________________________________________________

## Primitive Visual {#primitive-visual}

Renders a simple 3D shape, such as a cube or sphere. Scaled to fit the control.

The shapes are generated with clockwise winding and back-face culling on by default.

![ ](../assets/img/visuals/cube.png)
![ ](visuals/cube.png)
 
### Properties Supported

**VisualType:** Dali::Toolkit::Visual::PRIMITIVE, "PRIMITIVE"

| Property                                                      | String            | Type               | Description                                                                                                     | Default Value                                           | Range                          |
|---------------------------------------------------------------|-------------------|:------------------:|-----------------------------------------------------------------------------------------------------------------|:-------------------------------------------------------:|:------------------------------:|
| Dali::Toolkit::PrimitiveVisual::Property::SHAPE               | shape             | INTEGER or STRING  | The specific shape to render. [More info](@ref shape-details)                                                   | Dali::Toolkit::PrimitiveVisual::Shape::SPHERE, "SPHERE" | [See list](@ref shape-details) |
| Dali::Toolkit::PrimitiveVisual::Property::MIX_COLOR           | mixColor          | VECTOR4            | The color of the shape.                                                                                         | (0.5, 0.5, 0.5, 1.0)                                    | 0.0 - 1.0 for each             |
| Dali::Toolkit::PrimitiveVisual::Property::SLICES              | slices            | INTEGER            | The number of slices as you go around the shape. [More info](@ref slices-details)                               | 128                                                     | 1 - 255                        |
| Dali::Toolkit::PrimitiveVisual::Property::STACKS              | stacks            | INTEGER            | The number of stacks as you go down the shape. [More info](@ref stacks-details)                                 | 128                                                     | 1 - 255                        |
| Dali::Toolkit::PrimitiveVisual::Property::SCALE_TOP_RADIUS    | scaleTopRadius    | FLOAT              | The scale of the radius of the top circle of a conical frustrum.                                                | 1.0                                                     | ≥ 0.0                          |
| Dali::Toolkit::PrimitiveVisual::Property::SCALE_BOTTOM_RADIUS | scaleBottomRadius | FLOAT              | The scale of the radius of the bottom circle of a conical frustrum.                                             | 1.5                                                     | ≥ 0.0                          |
| Dali::Toolkit::PrimitiveVisual::Property::SCALE_HEIGHT        | scaleHeight       | FLOAT              | The scale of the height of a conic.                                                                             | 3.0                                                     | > 0.0                          |
| Dali::Toolkit::PrimitiveVisual::Property::SCALE_RADIUS        | scaleRadius       | FLOAT              | The scale of the radius of a cylinder.                                                                          | 1.0                                                     | > 0.0                          |
| Dali::Toolkit::PrimitiveVisual::Property::SCALE_DIMENSIONS    | scaleDimensions   | VECTOR3            | The dimensions of a cuboid. Scales in the same fashion as a 9-patch image.                                      | Vector3::ONE                                            | > 0.0 for each                 |
| Dali::Toolkit::PrimitiveVisual::Property::BEVEL_PERCENTAGE    | bevelPercentage   | FLOAT              | Determines how bevelled the cuboid should be, based off the smallest dimension. [More info](@ref bevel-details) | 0.0 (no bevel)                                          | 0.0 - 1.0                      |
| Dali::Toolkit::PrimitiveVisual::Property::BEVEL_SMOOTHNESS    | bevelSmoothness   | FLOAT              | Defines how smooth the bevelled edges should be.                                                                | 0.0 (sharp edges)                                       | 0.0 - 1.0                      |
| Dali::Toolkit::PrimitiveVisual::Property::LIGHT_POSITION      | lightPosition     | VECTOR3            | The position, in stage space, of the point light that applies lighting to the model.                            | (Offset outwards from the center of the screen.)        | Unlimited                      |

### Shapes {#shape-details}

There are six shapes that can be chosen, some of which are simplified specialisations of another.

| Enumeration                                             | String           | Description                                                                       | Parameters                                                    |
|---------------------------------------------------------|------------------|-----------------------------------------------------------------------------------|---------------------------------------------------------------|
| Dali::Toolkit::PrimitiveVisual::Shape::SPHERE           | SPHERE           | *Default*.                                                                        | color, slices, stacks                                         |
| Dali::Toolkit::PrimitiveVisual::Shape::CONICAL_FRUSTRUM | CONICAL_FRUSTRUM | The area bound between two circles, i.e. a cone with the tip removed.             | color, scaleTopRadius, scaleBottomRadius, scaleHeight, slices |
| Dali::Toolkit::PrimitiveVisual::Shape::CONE             | CONE             | Equivalent to a conical frustrum with top radius of zero.                         | color, scaleBottomRadius, scaleHeight, slices                 |
| Dali::Toolkit::PrimitiveVisual::Shape::CYLINDER         | CYLINDER         | Equivalent to a conical frustrum with equal radii for the top and bottom circles. | color, scaleRadius, scaleHeight, slices                       |
| Dali::Toolkit::PrimitiveVisual::Shape::CUBE             | CUBE             | Equivalent to a bevelled cube with a bevel percentage of zero.                    | color, scaleDimensions                                        |
| Dali::Toolkit::PrimitiveVisual::Shape::OCTAHEDRON       | OCTAHEDRON       | Equivalent to a bevelled cube with a bevel percentage of one.                     | color, scaleDimensions                                        |
| Dali::Toolkit::PrimitiveVisual::Shape::BEVELLED_CUBE    | BEVELLED_CUBE    | A cube/cuboid with all edges flattened to some degree.                            | color, scaleDimensions, bevelPercentage, bevelSmoothness      |

#### Examples below:

**sphere:**
 
![ ](../assets/img/visuals/sphere.png)
![ ](visuals/sphere.png)
 
**conics:**
 
| Frustrum | Cone | Cylinder |
|----------|------|----------|
| ![ ](../assets/img/visuals/conical-frustrum.png) ![ ](visuals/conical-frustrum.png) | ![ ](../assets/img/visuals/cone.png) ![ ](visuals/cone.png) | ![ ](../assets/img/visuals/cylinder.png) ![ ](visuals/cylinder.png) |
 
### Bevel {#bevel-details}
 
Bevel percentage ranges from 0.0 to 1.0. It affects the ratio of the outer face widths to the width of the overall cube, as shown:
 
| 0.0 ( cube) | 0.3 | 0.7 | 1.0 (octahedron) |
|-------------|-----|-----|------------------|
| ![ ](../assets/img/visuals/cube.png) ![ ](visuals/cube.png) | ![ ](../assets/img/visuals/bevelled-cube-low.png) ![ ](visuals/bevelled-cube-low.png) | ![ ](../assets/img/visuals/bevelled-cube-high.png) ![ ](visuals/bevelled-cube-high.png) | ![ ](../assets/img/visuals/octahedron.png) ![ ](visuals/octahedron.png) |
 
### Slices {#slices-details}
 
For spheres and conical frustrums, 'slices' determines how many divisions there are as you go around the object.
 
![ ](../assets/img/visuals/slices.png)
![ ](visuals/slices.png)
 
### Stacks {#stacks-details}
 
For spheres, 'stacks' determines how many layers there are as you go down the object.
 
![ ](../assets/img/visuals/stacks.png)
![ ](visuals/stacks.png)
 
### Usage
 
**sphere**
 
~~~{.cpp}
// C++
Dali::Toolkit::Control control = Dali::Toolkit::Control::New();

Dali::Property::Map map;

map[ Visual::Property::TYPE ] = Dali::Toolkit::Visual::PRIMITIVE;
map[ PrimitiveVisual::Property::SHAPE ] = PrimitiveVisual::Shape::SPHERE;
map[ PrimitiveVisual::Property::MIX_COLOR ] = Vector4( 1.0, 0.5, 0.0, 1.0 );

control.SetProperty( Control::Property::BACKGROUND, map );
~~~

**conical frustrum**

~~~{.cpp}
// C++
Dali::Toolkit::Control control = Dali::Toolkit::Control::New();

Dali::Property::Map map;

map[ Visual::Property::TYPE ] = Dali::Toolkit::Visual::PRIMITIVE;
map[ PrimitiveVisual::Property::SHAPE ] = PrimitiveVisual::Shape::CONICAL_FRUSTRUM;
map[ PrimitiveVisual::Property::MIX_COLOR ] = Vector4( 1.0, 0.5, 0.0, 1.0 );
map[ PrimitiveVisual::Property::SCALE_TOP_RADIUS ] = 1.0f;
map[ PrimitiveVisual::Property::SCALE_BOTTOM_RADIUS ] = 1.5f;
map[ PrimitiveVisual::Property::SCALE_HEIGHT ] = 3.0f;

control.SetProperty( Control::Property::BACKGROUND, map );
~~~

**bevelled cube**

~~~{.cpp}
// C++
Dali::Toolkit::Control control = Dali::Toolkit::Control::New();

Dali::Property::Map map;

map[ Visual::Property::TYPE ] = Dali::Toolkit::Visual::PRIMITIVE;
map[ PrimitiveVisual::Property::SHAPE ] = PrimitiveVisual::Shape::BEVELLED_CUBE;
map[ PrimitiveVisual::Property::MIX_COLOR ] = Vector4( 1.0, 0.5, 0.0, 1.0 );
map[ PrimitiveVisual::Property::BEVEL_PERCENTAGE ] = 0.4f;

control.SetProperty( Control::Property::BACKGROUND, map );
~~~
___________________________________________________________________________________________________

## Text Visual {#text-visual}

Renders text within a control.

![ ](../assets/img/visuals/HelloWorld.png)
![ ](visuals/HelloWorld.png)

### Properties

**VisualType:** Dali::Toolkit::Visual::TEXT, "TEXT"

| Property                                                    | String              | Type          | Required | Description                                                                   | Default                |
|-------------------------------------------------------------|---------------------|:-------------:|:--------:|-------------------------------------------------------------------------------|------------------------|
| Dali::Toolkit::TextVisual::Property::TEXT                   | text                | STRING        | Yes      | The text to display in UTF-8 format                                           |                        |
| Dali::Toolkit::TextVisual::Property::FONT_FAMILY            | fontFamily          | STRING        | No       | The requested font family to use                                              |                        |
| Dali::Toolkit::TextVisual::Property::FONT_STYLE             | fontStyle           | MAP           | No       | The requested font style to use                                               |                        |
| Dali::Toolkit::TextVisual::Property::POINT_SIZE             | pointSize           | FLOAT         | Yes      | The size of font in points                                                    |                        |
| Dali::Toolkit::TextVisual::Property::MULTI_LINE             | multiLine           | BOOLEAN       | No       | The single-line or multi-line layout option                                   | false                  |
| Dali::Toolkit::TextVisual::Property::HORIZONTAL_ALIGNMENT   | horizontalAlignment | STRING        | No       | The line horizontal alignment: "BEGIN", "CENTER", "END"                       | "BEGIN"                |
| Dali::Toolkit::TextVisual::Property::VERTICAL_ALIGNMENT     | verticalAlignment   | STRING        | No       | The line vertical alignment: "TOP",   "CENTER", "BOTTOM"                      | "TOP"                  |
| Dali::Toolkit::TextVisual::Property::TEXT_COLOR             | textColor           | VECTOR4       | No       | The color of the text                                                         | Color::BLACK           |
| Dali::Toolkit::TextVisual::Property::ENABLE_MARKUP         | enableMarkup           | BOOL       | No       | If mark up should be enabled |                                                        |

### Usage

~~~{.cpp}
    // C++
    Dali::Stage stage = Dali::Stage::GetCurrent();
    stage.SetBackgroundColor( Dali::Color::WHITE );

    Dali::Toolkit::Control control = Dali::Toolkit::Control::New();
    control.SetParentOrigin( ParentOrigin::CENTER );

    Dali::Property::Map map;
    map[ Dali::Toolkit::Visual::Property::TYPE ] = Dali::Toolkit::Visual::TEXT;
    map[ Dali::Toolkit::TextVisual::Property::TEXT ] = "Hello world";
    map[ Dali::Toolkit::TextVisual::Property::TEXT_COLOR ] = Dali::Color::BLACK;
    map[ Dali::Toolkit::TextVisual::Property::FONT_FAMILY ] = "Sans";
    map[ Dali::Toolkit::TextVisual::Property::POINT_SIZE ] = 30.f;
    map[ Dali::Toolkit::TextVisual::Property::HORIZONTAL_ALIGNMENT ] = "CENTER";
    map[ Dali::Toolkit::TextVisual::Property::VERTICAL_ALIGNMENT ] = "CENTER";

    control.SetProperty( Dali::Toolkit::Control::Property::BACKGROUND, map );

    stage.Add( control );
~~~

## Wireframe Visual {#wireframe-visual}

Renders a wireframe around a quad geometry.
Is mainly used for debugging and is the visual that replaces all other visuals when [Visual Debug Rendering](@ref debugrendering) is turned on.
 
![ ](../assets/img/visuals/wireframe-visual.png)
![ ](visuals/wireframe-visual.png)

### Properties

**VisualType:** Dali::Toolkit::Visual::WIREFRAME, "WIREFRAME"

### Usage

~~~{.cpp}
// C++
Dali::Toolkit::Control control = Dali::Toolkit::Control::New();

Dali::Property::Map map;
map[ Visual::Property::TYPE ] = Dali::Toolkit::Visual::WIREFRAME;

control.SetProperty( Control::Property::BACKGROUND, map );
~~~

~~~{.js}
// JavaScript
var control = new dali.Control( "Control" );

control.background =
{
  visualType : "WIREFRAME"
};
~~~


@class _Guide_Control_Visuals

*/
