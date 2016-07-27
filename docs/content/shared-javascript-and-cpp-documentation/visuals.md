<!--
/**-->

# Visuals {#visuals}

Visuals provide reusable rendering logic which can be used by all controls.
This means that custom controls do not have to create actors, they can just reuse the existing visuals which increases performance.
 
Visuals reuse geometry, shaders etc. across controls and manages the renderer and material to exist only when the control is on-stage.
Additionaly, they respond to actor size and color change, while also providing clipping at the renderer level.
 
DALi provides the following visuals:
 + [Color](@ref color-visual)
 + [Gradient](@ref gradient-visual)
 + [Image](@ref image-visuals)
 + [Border](@ref border-visual)
 + [Mesh](@ref mesh-visual)
 + [Primitive](@ref primitive-visual)
 
Controls can provide properties that allow users to specify the visual type ( rendererType ).
Setting visual properties are done via a property map.
The **rendererType** field in the property map specifies the visual to use/create.
This is required to avoid ambiguity as multiple visuals may be capable of rendering the same contents.
___________________________________________________________________________________________________

## Color Visual {#color-visual}

Renders a solid color to the control's quad.
 
![ ](../assets/img/visuals/color-visual.png)
![ ](visuals/color-visual.png)

### Properties Supported

**RendererType:** "COLOR"

| Property Name | Type    | Required | Description               |
|---------------|:-------:|:--------:|---------------------------|
| mixColor      | VECTOR4 | Yes      | The solid color required. |

### Usage

~~~{.cpp}
// C++
Dali::Toolkit::Control control = Dali::Toolkit::Control::New();

Dali::Property::Map map;
map[ "rendererType" ] = "COLOR";
map[ "mixColor" ] = Color::RED;

control.SetProperty( Dali::Toolkit::Control::Property::BACKGROUND, map );
~~~

~~~{.js}
// JavaScript
var control = new dali.Control( "Control" );

control.background =
{
  rendererType : "COLOR",
  mixColor : dali.COLOR_RED
};
~~~
___________________________________________________________________________________________________

## Gradient Visual {#gradient-visual}

Renders a smooth transition of colors to the control's quad.
 
Both Linear and Radial gradients are supported.

| Linear | Radial |
|--------|--------|
| ![ ](../assets/img/visuals/linear-gradient-visual.png) ![ ](visuals/linear-gradient-visual.png) | ![ ](../assets/img/visuals/radial-gradient-visual.png) ![ ](visuals/radial-gradient-visual.png) |

### Properties Supported

**RendererType:** "GRADIENT"

| Property Name                                        | Type             | Required   | Description                                                              |
|------------------------------------------------------|:----------------:|:----------:|--------------------------------------------------------------------------|
| startPosition                                        | VECTOR2          | For Linear | The start position of the linear gradient.                               |
| endPosition                                          | VECTOR2          | For Linear | The end position of the linear gradient.                                 |
| center                                               | VECTOR2          | For Radial | The center point of the gradient.                                        |
| radius                                               | FLOAT            | For Radial | The size of the radius.                                                  |
| stopOffset                                           | ARRAY of FLOAT   | No         | All the stop offsets. If not supplied default is 0.0 and 1.0.            |
| stopColor                                            | ARRAY of VECTOR4 | Yes        | The color at those stop offsets. At least 2 required to show a gradient. |
| [units](@ref gradient-visual-units)                | STRING           | No         | *OBJECT_BOUNDING_BOX* or *USER_SPACE*. Default: *OBJECT_BOUNDING_BOX*.   |
| [spreadMethod](@ref gradient-visual-spread-method) | STRING           | No         | *PAD*, *REFLECT* or *REPEAT*. Default: *PAD*.                            |

If the *stopOffset* and *stopColor* arrays do not have the same number of elements, then the minimum of the two is used as the stop points.

### Units {#gradient-visual-units}

Defines the coordinate system for the attributes:
 + Start (x1, y1) and End (x2 and y2) points of a line if using a linear gradient.
 + Center point (cx, cy) and radius (r) of a circle if using a radial gradient.
 
| Value               | Description                                                                                                                                    |
|---------------------|------------------------------------------------------------------------------------------------------------------------------------------------|
| OBJECT_BOUNDING_BOX | *Default*. Uses the normals for the start, end & center points, i.e. top-left is (-0.5, -0.5) and bottom-right is (0.5, 0.5).                  |
| USER_SPACE          | Uses the user coordinates for the start, end & center points, i.e. in a 200 by 200 control, top-left is (0, 0) and bottom-right is (200, 200). |

### Spread Method {#gradient-visual-spread-method}

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
map[ "rendererType" ] = "GRADIENT";
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
  rendererType : "GRADIENT",
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
map[ "rendererType" ] = "GRADIENT";
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
  rendererType : "GRADIENT",
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

Renders an image into the control's quad.
 
Depending on the extension of the image, a different visual is provided to render the image onto the screen.
 
 + [Normal](@ref image-visual)
 + [N-Patch](@ref n-patch-visual)
 + [SVG](@ref svg-visual)
 
___________________________
 
### Normal {#image-visual}
 
Renders a raster image ( jpg, png etc.) into the control's quad.
 
![ ](../assets/img/visuals/image-visual.png)
![ ](visuals/image-visual.png)

#### Properties Supported

**RendererType:** "IMAGE"

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
map[ "rendererType" ] = "IMAGE";
map[ "url" ] = "path-to-image.jpg";

control.SetProperty( Dali::Toolkit::Control::Property::BACKGROUND, map );
~~~

~~~{.js}
// JavaScript
var control = new dali.Control( "Control" );

control.background =
{
  rendererType : "IMAGE",
  url : "path-to-image.jpg"
};
~~~
___________________________________________________________________________________________________

### N-Patch {#n-patch-visual}

Renders an n-patch or a 9-patch image into the control's quad.
 
![ ](../assets/img/visuals/n-patch-visual.png)
![ ](visuals/n-patch-visual.png)

#### Properties Supported

**RendererType:** "IMAGE"

| Property Name | Type    | Required | Description                      |
|---------------|:-------:|:--------:|----------------------------------|
| url           | STRING  | Yes      | The URL of the n-patch image.    |
| borderOnly    | BOOLEAN | No       | If true, only draws the borders. |

#### Usage

~~~{.cpp}
// C++
Dali::Toolkit::Control control = Dali::Toolkit::Control::New();

Dali::Property::Map map;

map[ "rendererType" ] = "IMAGE";
map[ "url" ] = "path-to-image.9.png";

control.SetProperty( Dali::Toolkit::Control::Property::BACKGROUND, map );
~~~

~~~{.js}
// JavaScript
var control = new dali.Control( "Control" );

control.background =
{
  rendererType : "IMAGE",
  url : "path-to-image.9.png"
};
~~~

___________________________________________________________________________________________________

### SVG {#svg-visual}

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
 
![ ](../assets/img/visuals/svg-visual.svg)
 
</div>
 
<div style="width:300px">
 
![ ](visuals/svg-visual.svg)
 
</div>

 
#### Properties Supported

**RendererType:** "IMAGE"

| Property Name | Type    | Required | Description                      |
|---------------|:-------:|:--------:|----------------------------------|
| url           | STRING  | Yes      | The URL of the SVG image.    |

#### Usage

~~~{.cpp}
// C++
Dali::Toolkit::Control control = Dali::Toolkit::Control::New();

Dali::Property::Map map;

map[ "rendererType" ] = "IMAGE";
map[ "url" ] = "path-to-image.svg";

control.SetSize( 200.f, 200.f );
control.SetProperty( Dali::Toolkit::Control::Property::BACKGROUND, map );
~~~

~~~{.js}
// JavaScript
var control = new dali.Control( "Control" );

control.background =
{
  rendererType : "IMAGE",
  url : "path-to-image.svg"
};
~~~
___________________________________________________________________________________________________

## Border Visual {#border-visual}

Renders a solid color as an internal border to the control's quad.
 
![ ](../assets/img/visuals/border-visual.png)
![ ](visuals/border-visual.png)

### Properties Supported

**RendererType:** "BORDER"

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

map[ "rendererType" ] = "BORDER";
map[ "borderColor"  ] = Color::BLUE;
map[ "borderSize"   ] = 5.0f;

control.SetProperty( Dali::Toolkit::Control::Property::BACKGROUND, map );
~~~

~~~{.js}
// JavaScript
var control = new dali.Control( "Control" );

control.background =
{
  rendererType : "BORDER",
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

**RendererType** "MESH"

| Property Name                                | Type    | Required           | Description                                                                                |
|----------------------------------------------|:-------:|:------------------:|--------------------------------------------------------------------------------------------|
| objectUrl                                    | STRING  | Yes                | The location of the ".obj" file.                                                           |
| materialUrl                                  | STRING  | No                 | The location of the ".mtl" file. Leave blank for a textureless object.                     |
| texturesPath                                 | STRING  | If using material  | Path to the directory the textures (including gloss and normal) are stored in.             |
| [shaderType](@ref mesh-visual-shader-type) | STRING  | No                 | Sets the type of shader to be used with the mesh.                                          |
| useMipmapping                                | BOOLEAN | No                 | Flag for whether to use mipmaps for textures or not. Default true.                         |
| useSoftNormals                               | BOOLEAN | No                 | Flag for whether to average normals at each point to smooth textures or not. Default true. |
| lightPosition                                | VECTOR3 | No                 | The position, in stage space, of the point light that applies lighting to the model. This is based off the stage's dimensions, so using the width and height of the stage halved will correspond to the center, and using all zeroes will place the light at the top left. Note that this corresponds to a shader property, so it can be registered and set in the actor as well. |


### Shader Type {#mesh-visual-shader-type}

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
Dali::Stage stage = Dali::Stage::GetCurrent();
Dali::Toolkit::Control control = Dali::Toolkit::Control::New();

Dali::Property::Map map;

map[ "rendererType"  ] = "MESH";
map[ "objectUrl"     ] = "home/models/Dino.obj";
map[ "materialUrl"   ] = "home/models/Dino.mtl";
map[ "texturesPath"  ] = "home/images/";

control.SetProperty( Dali::Toolkit::Control::Property::BACKGROUND, map );
~~~

___________________________________________________________________________________________________

## Primitive Visual {#primitive-visual}

Renders a simple 3D shape, such as a cube or sphere. Scaled to fit the control.

The shapes are generated with clockwise winding and back-face culling on by default.

![ ](../assets/img/visuals/cube.png)
![ ](visuals/cube.png)

### Properties Supported

**RendererType** "PRIMITIVE"

| Property Name                         | Type    | Description                                                                     | Default Value        | Range                          |
|---------------------------------------|:-------:|---------------------------------------------------------------------------------|:--------------------:|:------------------------------:|
| [shape](@ref shape-details)           | STRING  | The specific shape to render.                                                   | "SPHERE"             | [See list](@ref shape-details) |
| shapeColor                            | VECTOR4 | The color of the shape.                                                         | (0.5, 0.5, 0.5, 1.0) | 0.0 - 1.0 for each             |
| [slices](@ref slices-details)         | INT     | The number of slices as you go around the shape.                                | 128                  | 1 - 255                        |
| [stacks](@ref stacks-details)         | INT     | The number of stacks as you go down the shape.                                  | 128                  | 1 - 255                        |
| scaleTopRadius                        | FLOAT   | The scale of the radius of the top circle of a conical frustrum.                | 1.0                  | ≥ 0.0                          |
| scaleBottomRadius                     | FLOAT   | The scale of the radius of the bottom circle of a conical frustrum.             | 1.5                  | ≥ 0.0                          |
| scaleHeight                           | FLOAT   | The scale of the height of a conic.                                             | 3.0                  | > 0.0                          |
| scaleRadius                           | FLOAT   | The scale of the radius of a cylinder.                                          | 1.0                  | > 0.0                          |
| scaleDimensions                       | VECTOR3 | The dimensions of a cuboid. Scales in the same fashion as a 9-patch image.      | (1.0, 1.0, 1.0)      | > 0.0 for each                 |
| [bevelPercentage](@ref bevel-details) | FLOAT   | Determines how bevelled the cuboid should be, based off the smallest dimension. | 0.0 (no bevel)       | 0.0 - 1.0                      |
| bevelSmoothness                       | FLOAT   | Defines how smooth the bevelled edges should be.                                | 0.0 (sharp edges)    | 0.0 - 1.0                      |
| lightPosition                        | VECTOR3 | The position, in stage space, of the point light that applies lighting to the model. This is based off the stage's dimensions, so using the width and height of the stage halved will correspond to the center, and using all zeroes will place the light at the top left. Note that this corresponds to a shader property, so it can be registered and set in the actor as well. | (Offset outwards from the center of the screen.) | Unlimited |

### Shapes {#shape-details}

There are six shapes that can be chosen, some of which are simplified specialisations of another.

| Value            | Description                                                                       | Parameters                                                    |
|------------------|-----------------------------------------------------------------------------------|---------------------------------------------------------------|
| SPHERE           | *Default*.                                                                        | color, slices, stacks                                         |
| CONICAL_FRUSTRUM | The area bound between two circles, i.e. a cone with the tip removed.             | color, scaleTopRadius, scaleBottomRadius, scaleHeight, slices |
| CONE             | Equivalent to a conical frustrum with top radius of zero.                         | color, scaleBottomRadius, scaleHeight, slices                 |
| CYLINDER         | Equivalent to a conical frustrum with equal radii for the top and bottom circles. | color, scaleRadius, scaleHeight, slices                       |
| CUBE             | Equivalent to a bevelled cube with a bevel percentage of zero.                    | color, scaleDimensions                                        |
| OCTAHEDRON       | Equivalent to a bevelled cube with a bevel percentage of one.                     | color, scaleDimensions                                        |
| BEVELLED_CUBE    | A cube/cuboid with all edges flattened to some degree.                            | color, scaleDimensions, bevelPercentage, bevelSmoothness      |

Examples below:

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

map[ "rendererType" ] = "PRIMITIVE";
map[ "shape"        ] = "SPHERE";
map[ "shapeColor"   ] = Vector4( 1.0, 0.5, 0.0, 1.0 );

control.SetProperty( Dali::Toolkit::Control::Property::BACKGROUND, map );
~~~

**conical frustrum**

~~~{.cpp}
// C++
Dali::Toolkit::Control control = Dali::Toolkit::Control::New();

Dali::Property::Map map;

map[ "rendererType"      ] = "PRIMITIVE";
map[ "shape"             ] = "CONICAL_FRUSTRUM";
map[ "shapeColor"        ] = Vector4( 1.0, 0.5, 0.0, 1.0 );
map[ "scaleTopRadius"    ] = 1.0f;
map[ "scaleBottomRadius" ] = 1.5f;
map[ "scaleHeight"       ] = 3.0f;

control.SetProperty( Dali::Toolkit::Control::Property::BACKGROUND, map );
~~~

**bevelled cube**

~~~{.cpp}
// C++
Dali::Toolkit::Control control = Dali::Toolkit::Control::New();

Dali::Property::Map map;

map[ "rendererType"    ] = "PRIMITIVE";
map[ "shape"           ] = "BEVELLED_CUBE";
map[ "shapeColor"      ] = Vector4( 1.0, 0.5, 0.0, 1.0 );
map[ "bevelPercentage" ] = 0.4f;

control.SetProperty( Dali::Toolkit::Control::Property::BACKGROUND, map );
~~~

@class _Guide_Control_Visuals

*/
