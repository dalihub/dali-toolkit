/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

// CLASS HEADER
#include "constants-wrapper.h"

// EXTERNAL INCLUDES
#include <dali/public-api/common/constants.h>
#include <dali/public-api/math/vector3.h>
#include <dali/public-api/animation/animation.h>
#include <dali/public-api/images/pixel.h>
#include <dali/public-api/images/image.h>
#include <dali/public-api/images/resource-image.h>
#include <dali/public-api/actors/actor-enumerations.h>
#include <dali/public-api/actors/draw-mode.h>
#include <dali/public-api/actors/image-actor.h>
#include <dali/public-api/actors/blending.h>
#include <dali/public-api/actors/camera-actor.h>
#include <dali/public-api/actors/sampling.h>
#include <dali/public-api/render-tasks/render-task.h>
#include <dali/public-api/common/loading-state.h>
#include <dali/devel-api/rendering/renderer.h>
#include <dali/devel-api/rendering/geometry.h>
#include <dali-toolkit/public-api/controls/scrollable/item-view/default-item-layout.h>
#include <dali-toolkit/public-api/controls/scrollable/scroll-view/scroll-view.h>

namespace Dali
{

namespace V8Plugin
{

namespace
{

struct Vector3Pair
{
  const char* name;
  const Vector3& value;
};

struct Vector4Pair
{
  const char* name;
  const Vector4& value;
};

struct IntegerPair
{
  const char* name;
  const int value;
};


const Vector3Pair Vector3Table[] =
{
    { "TOP_LEFT"            , ParentOrigin::TOP_LEFT          },
    { "TOP_CENTER"          , ParentOrigin::TOP_CENTER        },
    { "TOP_RIGHT"           , ParentOrigin::TOP_RIGHT         },
    { "CENTER_LEFT"         , ParentOrigin::CENTER_LEFT       },
    { "CENTER"              , ParentOrigin::CENTER            },
    { "CENTER_RIGHT"        , ParentOrigin::CENTER_RIGHT      },
    { "BOTTOM_LEFT"         , ParentOrigin::BOTTOM_LEFT       },
    { "BOTTOM_CENTER"       , ParentOrigin::BOTTOM_CENTER     },
    { "BOTTOM_RIGHT"        , ParentOrigin::BOTTOM_RIGHT      },

    { "VECTOR3_ONE"                  ,      Vector3::ONE                        },
    { "VECTOR3_XAXIS"                ,      Vector3::XAXIS                      },
    { "VECTOR3_YAXIS"                ,      Vector3::YAXIS                      },
    { "VECTOR3_ZAXIS"                ,      Vector3::ZAXIS                      },
    { "VECTOR3_NEGATIVE_XAXIS"       ,      Vector3::NEGATIVE_XAXIS             },
    { "VECTOR3_NEGATIVE_YAXIS"       ,      Vector3::NEGATIVE_YAXIS             },
    { "VECTOR3_NEGATIVE_ZAXIS"       ,      Vector3::NEGATIVE_ZAXIS             },
    { "VECTOR3_ZERO"                 ,      Vector3::ZERO                       }


};

const unsigned int Vector3TableCount = sizeof(Vector3Table)/sizeof(Vector3Table[0]);

const Vector4Pair Vector4Table[] =
{
    { "COLOR_BLACK"                  , Color::BLACK       },
    { "COLOR_WHITE"                  , Color::WHITE       },
    { "COLOR_RED"                    , Color::RED         },
    { "COLOR_GREEN"                  , Color::GREEN       },
    { "COLOR_BLUE"                   , Color::BLUE        },
    { "COLOR_YELLOW"                 , Color::YELLOW      },
    { "COLOR_MAGENTA"                , Color::MAGENTA     },
    { "COLOR_CYAN"                   , Color::CYAN        },
    { "COLOR_TRANSPARENT"            , Color::TRANSPARENT }

};

const unsigned int Vector4TableCount = sizeof(Vector4Table)/sizeof(Vector4Table[0]);

const IntegerPair EnumTable[] =
{

    { "ANIMATION_BAKE",                                       Animation::Bake                    },
    { "ANIMATION_DISCARD",                                    Animation::Discard                 },
    { "ANIMATION_BAKE_FINAL",                                 Animation::BakeFinal               },
    { "REFRESH_ONCE",                                         RenderTask::REFRESH_ONCE           },
    { "REFRESH_ALWAYS",                                       RenderTask::REFRESH_ALWAYS         },

    { "PIXEL_FORMAT_A8",                                       Pixel::A8,                        },
    { "PIXEL_FORMAT_L8",                                       Pixel::L8,                        },
    { "PIXEL_FORMAT_LA88",                                     Pixel::LA88,                      },
    { "PIXEL_FORMAT_RGB565",                                   Pixel::RGB565,                    },
    { "PIXEL_FORMAT_BGR565",                                   Pixel::BGR565,                    },
    { "PIXEL_FORMAT_RGBA4444",                                 Pixel::RGBA4444,                  },
    { "PIXEL_FORMAT_BGRA4444",                                 Pixel::BGRA4444,                  },
    { "PIXEL_FORMAT_RGBA5551",                                 Pixel::RGBA5551,                   },
    { "PIXEL_FORMAT_BGRA5551",                                 Pixel::BGRA5551,                  },
    { "PIXEL_FORMAT_RGB888",                                   Pixel::RGB888,                    },
    { "PIXEL_FORMAT_RGB8888",                                  Pixel::RGB8888,                   },
    { "PIXEL_FORMAT_BGR8888",                                  Pixel::BGR8888,                   },
    { "PIXEL_FORMAT_RGBA8888",                                 Pixel::RGBA8888,                  },
    { "PIXEL_FORMAT_BGRA8888",                                 Pixel::BGRA8888,                  },

    { "PIXEL_FORMAT_COMPRESSED_R11_EAC",                       Pixel::COMPRESSED_R11_EAC,        },
    { "PIXEL_FORMAT_COMPRESSED_SIGNED_R11_EAC",                Pixel::COMPRESSED_SIGNED_R11_EAC,                },
    { "PIXEL_FORMAT_COMPRESSED_RG11_EAC",                      Pixel::COMPRESSED_RG11_EAC,                      },
    { "PIXEL_FORMAT_COMPRESSED_SIGNED_RG11_EAC",               Pixel::COMPRESSED_SIGNED_RG11_EAC,               },
    { "PIXEL_FORMAT_COMPRESSED_RGB8_ETC2",                     Pixel::COMPRESSED_RGB8_ETC2,                     },
    { "PIXEL_FORMAT_COMPRESSED_SRGB8_ETC2",                    Pixel::COMPRESSED_SRGB8_ETC2,                    },
    { "PIXEL_FORMAT_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2", Pixel::COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2, },
    { "PIXEL_FORMAT_COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2",Pixel::COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2},
    { "PIXEL_FORMAT_COMPRESSED_RGBA8_ETC2_EAC",                Pixel::COMPRESSED_RGBA8_ETC2_EAC,                },
    { "PIXEL_FORMAT_COMPRESSED_SRGB8_ALPHA8_ETC2_EAC",         Pixel::COMPRESSED_SRGB8_ALPHA8_ETC2_EAC,         },
    { "PIXEL_FORMAT_COMPRESSED_RGB8_ETC1",                     Pixel::COMPRESSED_RGB8_ETC1,                     },
    { "PIXEL_FORMAT_COMPRESSED_RGB_PVRTC_4BPPV1",              Pixel::COMPRESSED_RGB_PVRTC_4BPPV1               },


    { "COLOR_MODE_USE_OWN_COLOR",                              USE_OWN_COLOR                    },
    { "COLOR_MODE_USE_PARENT_COLOR",                           USE_PARENT_COLOR                 },
    { "COLOR_MODE_USE_OWN_MULTIPLY_PARENT_COLOR",              USE_OWN_MULTIPLY_PARENT_COLOR    },
    { "COLOR_MODE_USE_OWN_MULTIPLY_PARENT_ALPHA",              USE_OWN_MULTIPLY_PARENT_ALPHA    },


    { "POSITION_INHERITANCE_INHERIT_PARENT_POSITION",                    INHERIT_PARENT_POSITION                    },
    { "POSITION_INHERITANCE_USE_PARENT_POSITION",                        USE_PARENT_POSITION                        },
    { "POSITION_INHERITANCE_USE_PARENT_POSITION_PLUS_LOCAL_POSITION",    USE_PARENT_POSITION_PLUS_LOCAL_POSITION    },
    { "POSITION_INHERITANCE_DONT_INHERIT_POSITION",                      DONT_INHERIT_POSITION                      },

    { "DRAW_MODE_NORMAL",                                   DrawMode::NORMAL     },
    { "DRAW_MODE_OVERLAY_2D",                               DrawMode::OVERLAY_2D },
    { "DRAW_MODE_STENCIL",                                  DrawMode::STENCIL    },

    { "RESOURCE_LOADING",                                   Dali::ResourceLoading },
    { "RESOURCE_LOADING_SUCCEEDED",                         Dali::ResourceLoadingSucceeded  },
    { "RESOURCE_LOADING_FAILED",                            Dali::ResourceLoadingFailed  },

    { "FITTING_MODE_SHRINK_TO_FIT",     FittingMode::SHRINK_TO_FIT },
    { "FITTING_MODE_SCALE_TO_FILL",     FittingMode::SCALE_TO_FILL },
    { "FITTING_MODE_FIT_WIDTH",         FittingMode::FIT_WIDTH },
    { "FITTING_MODE_FIT_HEIGHT",        FittingMode::FIT_HEIGHT },

    { "SAMPLING_MODE_BOX",              SamplingMode::BOX },
    { "SAMPLING_MODE_NEAREST",          SamplingMode::NEAREST },
    { "SAMPLING_MODE_LINEAR",           SamplingMode::LINEAR },
    { "SAMPLING_MODE_BOX_THEN_NEAREST", SamplingMode::BOX_THEN_NEAREST },
    { "SAMPLING_MODE_BOX_THEN_LINEAR",  SamplingMode::BOX_THEN_LINEAR },
    { "SAMPLING_MODE_NO_FILTER",        SamplingMode::NO_FILTER },
    { "SAMPLING_MODE_DONT_CARE",        SamplingMode::DONT_CARE },

    { "BLEND_FACTOR_ZERO",                                  BlendingFactor::ZERO                    },
    { "BLEND_FACTOR_ONE",                                   BlendingFactor::ONE                     },
    { "BLEND_FACTOR_SRC_COLOR",                             BlendingFactor::SRC_COLOR               },
    { "BLEND_FACTOR_ONE_MINUS_SRC_COLOR",                   BlendingFactor::ONE_MINUS_SRC_COLOR     },
    { "BLEND_FACTOR_SRC_ALPHA",                             BlendingFactor::SRC_ALPHA               },
    { "BLEND_FACTOR_ONE_MINUS_SRC_ALPHA",                   BlendingFactor::ONE_MINUS_SRC_ALPHA     },
    { "BLEND_FACTOR_DST_ALPHA",                             BlendingFactor::DST_ALPHA                },
    { "BLEND_FACTOR_ONE_MINUS_DST_ALPHA",                   BlendingFactor::ONE_MINUS_DST_ALPHA      },
    { "BLEND_FACTOR_DST_COLOR",                             BlendingFactor::DST_COLOR                },
    { "BLEND_FACTOR_ONE_MINUS_DST_COLOR",                   BlendingFactor::ONE_MINUS_DST_COLOR      },
    { "BLEND_FACTOR_SRC_ALPHA_SATURATE",                    BlendingFactor::SRC_ALPHA_SATURATE       },
    { "BLEND_FACTOR_CONSTANT_COLOR",                        BlendingFactor::CONSTANT_COLOR           },
    { "BLEND_FACTOR_ONE_MINUS_CONSTANT_COLOR",              BlendingFactor::ONE_MINUS_CONSTANT_COLOR },
    { "BLEND_FACTOR_CONSTANT_ALPHA" ,                       BlendingFactor::CONSTANT_ALPHA           },
    { "BLEND_FACTOR_ONE_MINUS_CONSTANT_ALPHA",              BlendingFactor::ONE_MINUS_CONSTANT_ALPHA },

    { "BLENDING_OFF",                                      BlendingMode::OFF             },
    { "BLENDING_AUTO",                                     BlendingMode::AUTO            },
    { "BLENDING_ON",                                       BlendingMode::ON              },

    { "BLEND_EQUATION_ADD",                                BlendingEquation::ADD              },
    { "BLEND_EQUATION_SUBTRACT",                           BlendingEquation::SUBTRACT         },
    { "BLEND_EQUATION_REVERSE_SUBTRACT",                   BlendingEquation::REVERSE_SUBTRACT },

    { "CAMERA_FREE_LOOK"             ,      Camera::FREE_LOOK                   },
    { "CAMERA_LOOK_AT_TARGET"        ,      Camera::LOOK_AT_TARGET              },
    { "CAMERA_PERSPECTIVE_PROJECTION" ,     Camera::PERSPECTIVE_PROJECTION      },
    { "CAMERA_ORTHOGRAPHIC_PROJECTION",     Camera::ORTHOGRAPHIC_PROJECTION     },

    { "FILTER_MODE_NONE",                     FilterMode::NONE           },
    { "FILTER_MODE_DEFAULT",                  FilterMode::DEFAULT        },
    { "FILTER_MODE_NEAREST",                  FilterMode::NEAREST        },
    { "FILTER_MODE_LINEAR",                   FilterMode::LINEAR         },

    { "WRAP_MODE_DEFAULT",                    WrapMode::DEFAULT          },
    { "WRAP_MODE_CLAMP_TO_EDGE",              WrapMode::CLAMP_TO_EDGE    },
    { "WRAP_MODE_REPEAT",                     WrapMode::REPEAT           },
    { "WRAP_MODE_MIRRORED_REPEAT",            WrapMode::MIRRORED_REPEAT  },

    { "MATERIAL_NONE",                        Renderer::NONE                      },
    { "MATERIAL_CULL_BACK",                   Renderer::CULL_BACK                 },
    { "MATERIAL_CULL_FRONT",                  Renderer::CULL_FRONT                },
    { "MATERIAL_CULL_BACK_AND_FRONT",         Renderer::CULL_BACK_AND_FRONT       },

    { "GEOMETRY_POINTS",                      Geometry::POINTS          },
    { "GEOMETRY_LINES",                       Geometry::LINES           },
    { "GEOMETRY_LINE_LOOP",                   Geometry::LINE_LOOP       },
    { "GEOMETRY_LINE_STRIP",                  Geometry::LINE_STRIP      },
    { "GEOMETRY_TRIANGLES",                   Geometry::TRIANGLES       },
    { "GEOMETRY_TRIANGLE_FAN",                Geometry::TRIANGLE_FAN    },
    { "GEOMETRY_TRIANGLE_STRIP",              Geometry::TRIANGLE_STRIP  },

    { "PROPERTY_NONE",                        Property::NONE           },
    { "PROPERTY_BOOLEAN",                     Property::BOOLEAN        },
    { "PROPERTY_FLOAT",                       Property::FLOAT          },
    { "PROPERTY_INTEGER",                     Property::INTEGER        },
    { "PROPERTY_VECTOR2",                     Property::VECTOR2        },
    { "PROPERTY_VECTOR3",                     Property::VECTOR3        },
    { "PROPERTY_VECTOR4",                     Property::VECTOR4        },
    { "PROPERTY_MATRIX3",                     Property::MATRIX3        },
    { "PROPERTY_MATRIX",                      Property::MATRIX         },
    { "PROPERTY_RECTANGLE",                   Property::RECTANGLE      },
    { "PROPERTY_ROTATION",                    Property::ROTATION       },
    { "PROPERTY_STRING",                      Property::STRING         },
    { "PROPERTY_ARRAY",                       Property::ARRAY          },
    { "PROPERTY_MAP",                         Property::MAP            },
    { "PROPERTY_INVALID_INDEX",               Property::INVALID_INDEX  },
    { "PROPERTY_READ_ONLY",                   Property::READ_ONLY      },
    { "PROPERTY_READ_WRITE",                  Property::READ_WRITE     },
    { "PROPERTY_ANIMATABLE",                  Property::ANIMATABLE     },

    { "DIMENSION_WIDTH",                      Dimension::WIDTH   },
    { "DIMENSION_HEIGHT",                     Dimension::HEIGHT  },

    { "ITEM_LAYOUT_LIST",                     Toolkit::DefaultItemLayout::LIST     },
    { "ITEM_LAYOUT_GRID",                     Toolkit::DefaultItemLayout::GRID     },

    { "DIRECTION_BIAS_NONE",                  Toolkit::DirectionBiasNone   },
    { "DIRECTION_BIAS_LEFT",                  Toolkit::DirectionBiasLeft   },
    { "DIRECTION_BIAS_RIGHT",                 Toolkit::DirectionBiasRight  },

    { "ALPHA_FUNCTION_DEFAULT",               AlphaFunction::DEFAULT              },
    { "ALPHA_FUNCTION_LINEAR",                AlphaFunction::LINEAR               },
    { "ALPHA_FUNCTION_REVERSE",               AlphaFunction::REVERSE              },
    { "ALPHA_FUNCTION_EASE_IN_SQUARE",        AlphaFunction::EASE_IN_SQUARE       },
    { "ALPHA_FUNCTION_EASE_OUT_SQUARE",       AlphaFunction::EASE_OUT_SQUARE      },
    { "ALPHA_FUNCTION_EASE_IN",               AlphaFunction::EASE_IN              },
    { "ALPHA_FUNCTION_EASE_OUT",              AlphaFunction::EASE_OUT             },
    { "ALPHA_FUNCTION_EASE_IN_OUT",           AlphaFunction::EASE_IN_OUT          },
    { "ALPHA_FUNCTION_EASE_IN_SINE",          AlphaFunction::EASE_IN_SINE         },
    { "ALPHA_FUNCTION_EASE_OUT_SINE",         AlphaFunction::EASE_OUT_SINE        },
    { "ALPHA_FUNCTION_EASE_IN_OUT_SINE",      AlphaFunction::EASE_IN_OUT_SINE     },
    { "ALPHA_FUNCTION_BOUNCE",                AlphaFunction::BOUNCE               },
    { "ALPHA_FUNCTION_SIN",                   AlphaFunction::SIN                  },
    { "ALPHA_FUNCTION_EASE_OUT_BACK",         AlphaFunction::EASE_OUT_BACK        },

};
const unsigned int EnumTableCount = sizeof(EnumTable)/sizeof(EnumTable[0]);






} // un-named name space


void ConstantsWrapper::AddDaliConstants( v8::Isolate* isolate, v8::Local<v8::Object >& obj  )
{
  v8::EscapableHandleScope handleScope( isolate );


  for( unsigned int i = 0; i < EnumTableCount; ++i )
  {
    obj->Set( v8::String::NewFromUtf8( isolate, EnumTable[i].name ),
                      v8::Integer::New(isolate, EnumTable[i].value ) );
  }

  for( unsigned int i = 0; i < Vector3TableCount; ++i )
  {
    // create a JavaScript array object with 3 entries.
    v8::Local<v8::Array> array = v8::Array::New( isolate ,3 );
    array->Set( 0 , v8::Number::New(isolate, Vector3Table[i].value.x ));
    array->Set( 1 , v8::Number::New(isolate, Vector3Table[i].value.y ));
    array->Set( 2 , v8::Number::New(isolate, Vector3Table[i].value.z ));

    obj->Set( v8::String::NewFromUtf8( isolate, Vector3Table[i].name),
              array);
  }


  for( unsigned int i = 0; i < Vector4TableCount; ++i )
  {
    // create a JavaScript array object with 3 entries.
    v8::Local<v8::Array> array = v8::Array::New( isolate ,4 );
    array->Set( 0 , v8::Number::New(isolate, Vector4Table[i].value.x ));
    array->Set( 1 , v8::Number::New(isolate, Vector4Table[i].value.y ));
    array->Set( 2 , v8::Number::New(isolate, Vector4Table[i].value.z ));
    array->Set( 3 , v8::Number::New(isolate, Vector4Table[i].value.w ));

    obj->Set( v8::String::NewFromUtf8( isolate, Vector4Table[i].name),
              array);
  }

}

} // namespace V8Plugin

} // namespace Dali
