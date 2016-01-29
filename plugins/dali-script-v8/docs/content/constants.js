/**
 *
### Dali Constants

Constants accessible under the dali global object.
  
    actor.parentOrigin = dali.TOP_LEFT;
    actor.color = dali.COLOR_RED;
    actor.setBlendFunc( dali.BLEND_FACTOR_ONE_MINUS_SRC_COLOR, dali.BLEND_FACTOR_SRC_ALPHA_SATURATE );


| Constant Name | value |
|---------------|-------|
|  ** actor.parentOrigin and actor.anchorPoint  **      |                      |
|TOP_LEFT            |  (0.0f, 0.0f, 0.5f)  |
|TOP_CENTER          |  (0.5f, 0.0f, 0.5f)  |
|TOP_RIGHT          |  (1.0f, 0.0f, 0.5f)  |
|CENTER_LEFT         |  (0.0f, 0.5f, 0.5f)  |
|CENTER              |  (0.5f, 0.5f, 0.5f)  |
|CENTER_RIGHT        |  (1.0f, 0.5f, 0.5f)  |
|BOTTOM_LEFT         |  (0.0f, 1.0f, 0.5f)  |
|BOTTOM_CENTER       |  (0.5f, 1.0f, 0.5f)  |
|BOTTOM_RIGHT        |  (1.0f, 1.0f, 0.5f)  |


|** Vector3 ** | |
|VECTOR3_ONE                  |  (1.0f, 1.0f, 1.0f)         |
|VECTOR3_XAXIS                |  (1.0f, 0.0f, 0.0f)         |
|VECTOR3_YAXIS                |  (0.0f, 1.0f, 0.0f)         |
|VECTOR3_ZAXIS                |  (0.0f, 0.0f, 1.0f)         |
|VECTOR3_NEGATIVE_XAXIS       |  (-1.0f, 0.0f, 0.0f)        |
|VECTOR3_NEGATIVE_YAXIS       |   (0.0f, -1.0f, 0.0f)       |
|VECTOR3_NEGATIVE_ZAXIS       |   (0.0f, 0.0f, -1.0f)       |
|VECTOR3_ZERO                 |   (0.0f, 0.0f, 0.0f)        |

|**  Colors **| -|
|COLOR_BLACK              |    ( 0.0f, 0.0f, 0.0f, 1.0f )     |
|COLOR_WHITE              |    ( 1.0f, 1.0f, 1.0f, 1.0f )     |
|COLOR_RED                |    (   1.0f, 0.0f, 0.0f, 1.0f )   |
|COLOR_GREEN              |    ( 0.0f, 1.0f, 0.0f, 1.0f )     |
|COLOR_BLUE               |    (  0.0f, 0.0f, 1.0f, 1.0f )    |
|COLOR_YELLOW             |    (  1.0f, 1.0f, 0.0f, 1.0f )    |
|COLOR_MAGENTA            |     ( 1.0f, 0.0f, 1.0f, 1.0f )    |
|COLOR_CYAN               |    (    0.0f, 1.0f, 1.0f, 1.0f )  |
|COLOR_TRANSPARENT        |  ( 0.0f, 0.0f, 0.0f, 0.0f )       |

| ** actor.colorMode constants  **  | -|
| COLOR_MODE_USE_OWN_COLOR,                  | integer value  |
| COLOR_MODE_USE_PARENT_COLOR,               | integer value  |
| COLOR_MODE_USE_OWN_MULTIPLY_PARENT_COLOR,  | integer value  |
| COLOR_MODE_USE_OWN_MULTIPLY_PARENT_ALPHA,  | integer value  |

|**actor.positionInheritance  **| |
|POSITION_INHERITANCE_INHERIT_PARENT_POSITION,       | integer value |
|POSITION_INHERITANCE_USE_PARENT_POSITION,           | integer value |
|POSITION_INHERITANCE_USE_PARENT_POSITION_PLUS_LOCAL_POSITION, | integer value |
|POSITION_INHERITANCE_DONT_INHERIT_POSITION,         | integer value |

|**actor.drawMode  **| |
|DRAW_MODE_NORMAL                         | integer value |
|DRAW_MODE_OVERLAY_2D                     | integer value |
|DRAW_MODE_STENCIL                        | integer value |

|**Image load policy  **| |
|**actor.SetBlendFunc() ** | |
|BLEND_FACTOR_ZERO                        | integer value |
|BLEND_FACTOR_ONE                         | integer value |
|BLEND_FACTOR_SRC_COLOR                   | integer value |
|BLEND_FACTOR_ONE_MINUS_SRC_COLOR         | integer value |
|BLEND_FACTOR_SRC_ALPHA                   | integer value |
|BLEND_FACTOR_ONE_MINUS_SRC_ALPHA         | integer value |
|BLEND_FACTOR_DST_ALPHA                   | integer value |
|BLEND_FACTOR_ONE_MINUS_DST_ALPHA         | integer value |
|BLEND_FACTOR_DST_COLOR                   | integer value |
|BLEND_FACTOR_ONE_MINUS_DST_COLOR         | integer value |
|BLEND_FACTOR_SRC_ALPHA_SATURATE          | integer value |
|BLEND_FACTOR_CONSTANT_COLOR              | integer value |
|BLEND_FACTOR_ONE_MINUS_CONSTANT_COLOR    | integer value |
|BLEND_FACTOR_CONSTANT_ALPHA              | integer value |
|BLEND_FACTOR_ONE_MINUS_CONSTANT_ALPHA    | integer value |

|**actor.SetBlendMode()  **| |
|BLENDING_OFF                              | integer value |
|BLENDING_AUTO                             | integer value |
|BLENDING_ON                               | integer value |

|**actor.SetCullFace()** | |
|CULL_FACE_DISABLE          | integer value  |
|CULL_FRONT_FACE           | integer value  |
|CULL_BACK_FACE            | integer value  |
|CULL_FRONT_AND_BACK_FACE  | integer value  |

|**actor.sizeMode** | |
|USE_OWN_SIZE                   | integer value  |
|SIZE_EQUAL_TO_PARENT           | integer value  |
|SIZE_RELATIVE_TO_PARENT        | integer value  |
|SIZE_FIXED_OFFSET_FROM_PARENT  | integer value  |


|**animation.SetEndAction( mode ) ** | |
|ANIMATION_BAKE             | integer value  |
|ANIMATION_DISCARD          | integer value  |
|ANIMATION_BAKE_FINAL       | integer value  |

|**camera.type ** | |
|CAMERA_FREE_LOOK             | integer value  |
|CAMERA_LOOK_AT_TARGET        | integer value  |

|**camera.projectionMode ** | |
|CAMERA_PERSPECTIVE_PROJECTION  | integer value  |
|CAMERA_ORTHOGRAPHIC_PROJECTION | integer value  |


|** Pixel formats     **          |  |
| PIXEL_FORMAT_A8                            | integer value  |
| PIXEL_FORMAT_L8                            | integer value  |
| PIXEL_FORMAT_LA88                          | integer value  |
| PIXEL_FORMAT_RGB565                        | integer value  |
| PIXEL_FORMAT_BGR565                        | integer value  |
| PIXEL_FORMAT_RGBA4444                      | integer value  |
| PIXEL_FORMAT_BGRA4444                      | integer value  |
| PIXEL_FORMAT_RGBA5551                      | integer value  |
| PIXEL_FORMAT_BGRA5551                      | integer value  |
| PIXEL_FORMAT_RGB888                        | integer value  |
| PIXEL_FORMAT_RGB8888                       | integer value  |
| PIXEL_FORMAT_BGR8888                       | integer value  |
| PIXEL_FORMAT_RGBA8888                      | integer value  |
| PIXEL_FORMAT_BGRA8888                      | integer value  |

| **Pixel Compressed formats                 ** |  |
| PIXEL_FORMAT_COMPRESSED_R11_EAC                 | integer value           |
| PIXEL_FORMAT_COMPRESSED_SIGNED_R11_EAC          | integer value           |
| PIXEL_FORMAT_COMPRESSED_RG11_EAC                | integer value           |
| PIXEL_FORMAT_COMPRESSED_SIGNED_RG11_EAC         | integer value           |
| PIXEL_FORMAT_COMPRESSED_RGB8_ETC2               | integer value           |
| PIXEL_FORMAT_COMPRESSED_SRGB8_ETC2              | integer value           |
| PIXEL_FORMAT_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2    | integer value  |
| PIXEL_FORMAT_COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2   | integer value  |
| PIXEL_FORMAT_COMPRESSED_RGBA8_ETC2_EAC                   | integer value  |
| PIXEL_FORMAT_COMPRESSED_SRGB8_ALPHA8_ETC2_EAC            | integer value  |
| PIXEL_FORMAT_COMPRESSED_RGB8_ETC1                        | integer value  |
| PIXEL_FORMAT_COMPRESSED_RGB_PVRTC_4BPPV1                 | integer value  |

|**Texture filtering mode ** | |
|FILTER_MODE_NONE                     | integer value  |
|FILTER_MODE_DEFAULT                  | integer value  |
|FILTER_MODE_NEAREST                  | integer value  |
|FILTER_MODE_LINEAR                   | integer value  |

|**Sampler wrap mode ** | |
|WRAP_MODE_DEFAULT                    | integer value  |
|WRAP_MODE_CLAMP_TO_EDGE              | integer value  |
|WRAP_MODE_REPEAT                     | integer value  |
|WRAP_MODE_MIRRORED_REPEAT            | integer value  |

|**Material face culling mode ** | |
|MATERIAL_NONE                        | integer value  |
|MATERIAL_CULL_BACK                   | integer value  |
|MATERIAL_CULL_FRONT                  | integer value  |
|MATERIAL_CULL_BACK_AND_FRONT         | integer value  |

|**Geometry primitives type ** | |
|GEOMETRY_POINTS                      | integer value  |
|GEOMETRY_LINES                       | integer value  |
|GEOMETRY_LINE_LOOP                   | integer value  |
|GEOMETRY_LINE_STRIP                  | integer value  |
|GEOMETRY_TRIANGLES                   | integer value  |
|GEOMETRY_TRIANGLE_FAN                | integer value  |
|GEOMETRY_TRIANGLE_STRIP              | integer value  |

|**Property type ** | |
|PROPERTY_NONE                        | none  |
|PROPERTY_BOOLEAN                     | integer value  |
|PROPERTY_FLOAT                       | float value  |
|PROPERTY_INTEGER                     | integer value  |
|PROPERTY_VECTOR2                     | Array[ float, float ] |
|PROPERTY_VECTOR3                     | Array[ float, float, float ]  |
|PROPERTY_VECTOR4                     | Array[ float, float, float, float ]  |
|PROPERTY_MATRIX3                     | Object         |
|PROPERTY_MATRIX                      | Object         |
|PROPERTY_RECTANGLE                   | Object |
|PROPERTY_ORIENTATION                 | Object  |
|PROPERTY_STRING                      | String  |
|PROPERTY_ARRAY                       | Object   |
|PROPERTY_MAP                         | Object  |

|**Layout dimensions ** | |
|DIMENSION_WIDTH                      | integer value  |
|DIMENSION_HEIGHT                     | integer value  |

|**Item layout type ** | |
|ITEM_LAYOUT_LIST                     | integer value  |
|ITEM_LAYOUT_GRID                     | integer value  |

|**Scroll direction bias ** | |
|DIRECTION_BIAS_NONE                  | integer value  |
|DIRECTION_BIAS_LEFT                  | integer value  |
|DIRECTION_BIAS_RIGHT                 | integer value  |

|**Animation alpha function ** | |
|ALPHA_FUNCTION_DEFAULT               | integer value  |
|ALPHA_FUNCTION_LINEAR                | integer value  |
|ALPHA_FUNCTION_REVERSE               | integer value  |
|ALPHA_FUNCTION_EASE_IN_SQUARE        | integer value  |
|ALPHA_FUNCTION_EASE_OUT_SQUARE       | integer value  |
|ALPHA_FUNCTION_EASE_IN               | integer value  |
|ALPHA_FUNCTION_EASE_OUT              | integer value  |
|ALPHA_FUNCTION_EASE_IN_OUT           | integer value  |
|ALPHA_FUNCTION_EASE_IN_SINE          | integer value  |
|ALPHA_FUNCTION_EASE_OUT_SINE         | integer value  |
|ALPHA_FUNCTION_EASE_IN_OUT_SINE      | integer value  |
|ALPHA_FUNCTION_BOUNCE                | integer value  |
|ALPHA_FUNCTION_SIN                   | integer value  |
|ALPHA_FUNCTION_EASE_OUT_BACK         | integer value  |

 * @class Constants
 */
