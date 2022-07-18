# Table of contents
   * [Overview](#overview)
   * [`scenes`](#scenes)
   * [`nodes`](#nodes)
      * [Transformations](#transformations)
      * [Size](#size)
      * [Visibility](#visibility)
      * [`children`](#children)
      * [`customization`](#customization)
      * [Renderables](#renderables)
         * [`model`](#model)
         * [`arc`](#arc)
      * [`inverseBindPoseMatrix`](#inversebindposematrix)
   * [`materials`](#materials)
   * [`meshes`](#meshes)
   * [`shaders`](#shaders)
      * [`rendererState`](#rendererState)
      * [`defines`](#defines)
      * [`hints`](#hints)
      * [Uniforms](#uniforms)
   * [`skeletons`](#skeletons)
   * [`cameras`](#cameras)
      * [Perspective cameras](#perspective-cameras)
      * [Orthographic cameras](#orthographic-cameras)
   * [`lights`](#lights)
   * [`animations`](#animations)
      * [`properties`](#properties)
         * [`keyFramesBin`](#keyframesbin)
         * [`keyFrames`](#keyFrames)
         * [`value`](#value)
   * [`animationGroups`](#animationGroups)

# Overview
DLI is a JSON based format for representing 3D scenes.
   * Like glTF, the DLI document defines a set of scenes, which in turn define a hierarchical structure of nodes, and the additional data required to render them - meshes, geometry.
   * Unlike glTF, it allows definitions of shaders, environment maps, and lighting parameters as well;
   * Unlike glTF, DLI does not concern itself with buffers, buffer views and accessors;
   * It supports customizations, which allow replacing parts of the scene based on customization tags and options, without reloading the whole scene.
   * It supports the processing of custom categories, which can be scheduled to take place prior to or after the processing of the scene data, as well as custom node and animation processors.

# `scenes`
The "scenes" element is an array of JSON objects, each of which must define a `nodes` array with the index of the definition of the root node.

:warning: The array must not be empty. Only the first element is used.
An optional `scene` element with an integer value may be defined to specify the index of the first scene to be created.
The rest of the scenes are created in the order of their definition (from index 0 to the highest, skipping the default - already created - scene).
```js
{
    "scene": 0,
    "scenes": [ { "nodes": [ 0 ] } ],
    "nodes": [ {
        "name": "some_unique_name"
    } ]
}
```

# `nodes`
The 3D scene is built using a hierarchy of nodes, which are used to position the objects to render.

:warning: Each node must have a `name` string that 1, is not an empty string and 2, is unique within the DLI document. The use of alpha-numeric characters and underscore only is highly recommeneded.

## Transformations
There are two ways to define the local transform of each nodes. Both are optional, defaulting to a position at the origin, a scale of one and no rotation.
   * A `matrix` array of 16 numerical values defining a column-major 4x4 matrix;
   * A `position` array of 3 numerical values defining a 3D vector and an `angle` array of 3 numerical values defining euler angles of rotation along the 3 axes.

## Size
The size of the bounding box of a node may be specified using either of the optional `size` or `bounds` properties, as arrays of 2 or 3 numerical values. Its default value is the unit vector.

## Visibility
The `visible` optional boolean property defines whether a node and its children are rendered (`true`) or not (`false`).

## `children`
An array of 0 or more indices into the top level `nodes` array, which shall inherit the transform and visibility of their parent node.

:warning: Nodes are processed in the order they are encountered during the depth-first traversal of the hierarchy.
```js
  "nodes": [ {
    "name": "hip",
    "children": [ 3, 1, 2 ]
  }, {
    "name": "spine"
  }, {
    "name": "left leg"
  }, {
    "name": "right leg"
  } ]
```

## `customization`
Customizations may allow creating a different sub-tree of each node that define them, based on application specific configuration settings known at the time of creating the scene.
The definition of a `customization` is a single string tag: 
```js
  "nodes": [ {
    "name": "Soup du jour",
    "customization": "flavor", // this one
    "children": [ 1, 2, 3 ]
  }, {
    "name": "Broccoli and Stilton",
  }, {
    "name": "Butternut Squash",
  }, {
    "name": "Strawberry and Cream",
  } ]
```
:warning: Customizations and renderables are mutually exclusive on the same node.

## Renderables
There is support for two types of nodes that define renderable content.
The definition of these renderables come in sub-objects.
All of them support a `color` property, which is an array of 3 or 4 numerical values for RGB or RGBA components. For the alpha value to take effect, alpha blending must be enabled; this is controlled by the [material](#materiaL).

:warning: Customizations and renderables are mutually exclusive on the same node.

### `model`
Provides definition for a 3D object, which requires a `mesh`, `shader` and `material`.
Each of these are provided in form of an integer index into the related top-level array of the DLI document.

:warning: `mesh` must be provided; the rest are optional and default to 0.
```js
  "nodes": [ {
    "name": "Sphere",
    "model": {
      "mesh": 0, // required
      "shader": 0, // optional, defaults to 0
      "material": 1 // optional, defaults to 0
    }
  } ]
```

### `arc`
Arc is a specialisation of a model that allows the rendering of circular progress bars. As such, it also must provide a `mesh` ID.
```js
  "nodes": [ {
    "name": "Any Name",
    "arc": {
      "mesh": 0, // required
      "shader": 0, // optional, defaults to 0
      "material": 1 // optional, defaults to 0
      "antiAliasing": true,
      "radius": -0.928,
      "startAngle": -81.0,
      "endAngle": 261
    }
  } ]
```
   * `startAngle` and `endAngle` are the angular positions where the arc starts and ends, in degrees.
   * `radius` is the inner radius of the arc, the outer radius being defined by the [`size`](#size) of the node.
   * `antiAliasing` defines whether the edges of the arc should be smoothed.
   
## `inverseBindPoseMatrix`
Nodes that serve as joints of a [skeleton](#skeleton) must define this property as an array of 16 numerical values for a column major 4x4 matrix.
```js
  "nodes": [ {
    "name" : "l_shoulder_JNT",
    "inverseBindPoseMatrix" : [ 0.996081, -0.0407448, 0.0785079, 0.0, 0.0273643, 0.985992, 0.164531, 0.0, -0.0841121, -0.161738, 0.983242, 0.0, -0.0637747, -1.16091, -0.161038, 1.0 ]
  } ]
```

# `environment`
An array of environment map definitions, which have the following format:
```js
  "environment": [{
    "cubeSpecular": "Studio_001/Radiance.ktx",
    "cubeDiffuse": "Studio_001/Irradiance.ktx",
    "cubeInitialOrientation": [ 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0 ] 
  } ]
```
`cubeSpecular` and `cubeDiffuse` are the names of the respective cube map files that will be attempted to be located from the _environment_ path, which is up to the application. A 1x1x1 white RGB888 cubemap is created in place of either that was omitted.
`cubeInitialOrientation` may be used to inform the (PBR) shader of a matrix which describes the initial orientation of the cube map. Defaults to the identity matrix.

# `materials`
Defines configurations of textures (and their samplers) that form materials. These can be created from single values or image files (in the formats that DALi supports).
```js
  "materials": [ {
    "environment": 0,
    "albedoMap": "A.png",
    "normal": "N.png",
    "metallicRoughnessMap": "MR.png",
    "color": [ 1.0, 0.8, 0.7, 0.5 ]
  } ]
```
   * `environment`: The index of the [environment map](#environments) to use. Single integer, defaults to `0`.
   * `mipmap`: A boolean to speficy if the creation and sampling of mipmaps should be enabled. Off by default.
   * `color`: Base color, which the color of the node gets multiplied by. Defaults to white.
   * `metallic` and `roughness`: Properties for PBR materials; both are expected to be a single numerical value and default to `1.0`.

## Texture maps
`albedoMap` / `albedoMetallicMap` / `normalMap` / `normalRoughnessMap` / `metallicRoughnessMap` / `subsurfaceMap`: define various texture semantics, i.e. the role of the texture, which shall be loaded from an image _inside the materials path_, which is up to the application. All of them are optional.

:warning: Semantics shall not overlap within the same material, e.g. multiple albedo definitions, or albedo and albedoMetallic.

# `meshes`
Defines an array of meshes which are used to access geometry data stored in a binary file. The `uri` property is used to locate each file _inside the mesh path_, which is up to the application; alternatively it can be set to `"quad"`, resulting in the creation of a unit quad.
Those models loaded from a file may provide an accessor, and flag its presence in the `attributes` bitmask property. The following are supported:
|Attribute Name|Bit|Decimal value|Type|Remarks|
|-|-|-|-|-|
|`indices`  |0|  1|unsigned short||
|`positions`|1|  2|Vector3||
|`normals`  |2|  4|Vector3||
|`textures` |3|  8|Vector2|UVs|
|`tangents` |4| 16|Vector3||
|           |5| 32||Ignored, but reserved for bitangents|
|`joints0`  |6| 64|Vector4|Joint IDs for skinned meshes|
|`weights0` |7|128|Vector4|Joint weights for skinned meshes|

E.g. if positions, normals and tangents are provided, the `attributes` property must have a value of 2 + 4 + 16 = 22.
Each attribute must provide a `byteOffset` and `byteLength` property, which must be correctly sized for the type of the given attribute.
Finally, to specify what primitives should the geometry be rendered as, a `primitive` property may be provided with one of the following values: `TRIANGLES` (default), `LINES`, `POINTS`.

## Skinned meshes
DLI supports meshes that allow deformation by skeletal animation. These must define a few additional properties:
   * `joints0` and `weights0` attributes, as above.
   * A [`skeleton`](#skeletons) ID, to specify which (joint) nodes' transformations affect the mesh.

:warning: The maximum number of bones supported by DALi Scene3D is `64`.

## Blend shapes
Blend shapes provide alternate configurations of vertex `positions`, `normals` and/or `tangents` that may be blended with the same attributes of the base mesh, controlled by an animatable `weight`.
```js
  "meshes": [ {
      "uri": "example.bin",
      "attributes": 2,
      "positions": {
          "byteOffset": 0,
          "byteLength": 12000
      },
      "blendShapesHeader": {
          "version": "2.0",
          "byteOffset": 12000,
          "byteLEngth": 4
      },
      "blendShapes": [ {
          "weight": 0.0,
          "positions": {
              byteOffset: 12004,
              byteLength: 12000
          }
      } ]
  } ]
```
A `blendShapesHeader`, if present, must define:
   * the `version` of the blend shapes; supported values are `1.0` and `2.0`. The difference between the versions is that v1.0 requires a per-blend shape definition of an un-normalization factor.
   * the `byteOffset` and `byteLength` of a buffer in the binary which defines the width (2 bytes) and height (2 bytes) of the texture that dali-scene3d creates for blend shape data.

The `blendShapes` array then defines the shapes that are available to blend between, comprising of:
   * An initial `weight` numerical, the default is 0;
   * `position` / `normals` / `tangents` attributes, which must be also present in the base mesh and are the same format (`byteOffset` / `byteLength`);

:warning: The size of the attributes of the blend shape must match that of the base mesh (and each other) - i.e. they must define the same number of vertices.

# `shaders`
Provides an array of shader programs that renderables may use for rendering.
For each shader, `vertex` and `fragment` are required string properties pointing to the shader files _inside the shader path_, which is up to the application.
   * `rendererState`: This string defines the options for configuring the settings of the renderer. Refer to public-api/renderer-state.h for details.
   * `defines`: An optional array of strings which will be used to create #defines into the vertex and fragment shaders.
   * `hints`: An optional array of strings that map to `Dali::Shader::Hint` values. Therefore two values are supported - `MODIFIES_GEOMETRY` and `OUTPUT_IS_TRANSPARENT`.

## Uniforms
Every property that is not one of the reserved keys above, will be attempted to be registered as a uniform of the same name.

:warning: Boolean values will be converted to floating point 1.0 (for `true`) or 0.0 (for `false`).

:warning: Integer values will be converted to floating point.

:warning: Arrays of numerical values will be treated as one of the vec2 / vec3 / vec4 / mat3 / mat4 types, depending on what do they define sufficient components for.
```js
  "shaders": [ {
    "vertex": "dli_pbr.vsh",
    "fragment": "dli_pbr.fsh",
    "defines": [ "HIGHP", SKINNING" ],
    "rendererState": "DEPTH_WRITE|DEPTH_TEST|CULL_BACK",
    "hints": 
    "uMaxLOD": 6
  } ]
```

# `skeletons`
Skeletons in DLI simply define the name of a `node` that shall serve as the _root joint_ of the given skeleton.
```js
  "skeletons": [ {
    "node": "hipJoint"
  } ]
```
The Joint IDs in skinned mesh data relate to the descendants of the [node](#nodes) identified as the root joint that are joints, i.e. define [`inverseBindPoseMatrix`](#inversebindposematrix), in depth-first traversal order.

# `cameras`
Define the transformation of viewers and the projection used by them.
All cameras may define:
   * `matrix`: an array of 16 numerical values for the transform matrix
   * `near` and `far` values for the position of the respective clipping planes. These default to `0.1` and `1000.0`, respectively.

## Perspective cameras
This projection type - and a vertical field of view of `60` degrees - is the default.
The (V)FOV can be specified in the `fov` property, as a single numerical value.

## Orthographic cameras
If the `orthographic` is defined with an array of four numerical values for the left, right, bottom and top clipping planes (in this order), then orthographic projection is used, and `fov` is ignored.

# `lights`
Define parameters for a single light source - the implementation is up to the application. The following properties are supported.
   * `transform`: matrix of 16 numerical values for the positioning / directing of the light;
   * `color`: array of 3 components for RGB;
   * `intensity`: single float;
   * `shadowIntensity`: single float;
   * `shadowMapSize`: unsigned integer size (same width & height) of shadow map.
   * `orthographicSize`: single float to define the size (same width & height) of orthographic position.

# `animations`
Animations provide a way to change properties of nodes (and those of their renderables) over time.
```js
  "animations": [ {
     "name": "Idle",
     "loopCount": 0,
     "duration": 4.0,
     "endAction": "DISCARD",
     "disconnectAction": "DISCARD",
     "properties": []
  } ]
```
   * `name`: the identifier to look the animation up by;
   * `loopCount`: the number of times that the animation should be played. The default is `1`; `0` signifies infinity repetitions.
   * `duration`: the duration of the animation in seconds. If not provided, it will be calculated from the properties.
   * `endAction` and `disconnectAction`: the supported values, defaults and their meaning are described in the `Dali::Animation::EndAction` enum;

## `properties`
An array of animation property definitions.
   * `node`: the name of the node whose property shall be animated;
   * `property`: the name that the property was registered under.
   * `alphaFunction`: the name of an alpha function which shall be used in animating a value, or between key frames;
   * `timePeriod`: `delay` (defaults to `0.0`) and `duration` (defaults to the `duration` of the animation) of the property animation, in sceonds;

The property may be animated using one of the following methods. They are listed in priority order; if e.g. a property defines `keyFramesBin` and `keyFrames`, then only `keyFramesBin` is used. 

### `keyFramesBin`
JSON object that defines a keyframe animation in a binary buffer, with the following properties:
   * `url` the path to the file containing the buffer;
   * `numKeys`: the number of keys.
   * `byteOffset`: offset to the start of the buffer

The size of the buffer depends on the property being animated, where  the property value for each frame follows a 4 byte floating point value determining progress (between 0 and 1).

:warning: Only `position` (3D vector of floats, 12 bytes), `rotation` (Quaternion, 12 bytes), and `scale` (3D vector, 12 bytes) properties are supported.

### `keyFrames`
JSON array of keyframe objects defined with the following properties:
   * `progress`: a scalar between `0` and `1` to apply to the duration to get the time stamp of the frame;
   * `value`: array of 3 or 4 numerical values depending on which property is being animated;

:warning: Only `position`, `rotation`, and `scale` properties are supported.

### `value`
Value animations animate a property using a single value that may be absolute or relative.
The properties it supports are the following:
   * `value`: the value to animate to (if absolute) or by (if `relative`);
   * `relative`: whether `value` is a target, or an offset;

# `animationGroups`
Animation groups simply group animations together by name, under another name, which may be used to trigger them at the same time.
```js
  "animationGroups": [ {
    "name": "Idle",
    "animations": [ "IdleBody", "IdleFace" ]
  } ]
```
