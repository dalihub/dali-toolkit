/**
 *
<h3> Dali Constants </h3>

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
|DRAW_MODE_OVERLAY                        | integer value |
|DRAW_MODE_STENCIL                        | integer value |

|**Image load policy  **| |
|IMAGE_LOAD_POLICY_IMMEDIATE              | integer value |
|IMAGE_LOAD_POLICY_ON_DEMAND              | integer value |

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

|**actor.SetBlendEquation()  ** | |
|BLEND_EQUATION_ADD                         | integer value  |
|BLEND_EQUATION_SUBTRACT                    | integer value  |
|BLEND_EQUATION_REVERSE_SUBTRACT            | integer value  |

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
</table>

 * @class Constants
 */
