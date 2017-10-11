/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
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

#include <iostream>

#include <stdlib.h>
#include <limits>

#include <dali-toolkit-test-suite-utils.h>
#include <dali-toolkit/dali-toolkit.h>
#include <toolkit-text-utils.h>
#include <dali-toolkit/internal/text/markup-processor.h>
#include <dali-toolkit/internal/text/markup-processor-helper-functions.h>
#include <dali-toolkit/internal/text/color-run.h>
#include <dali-toolkit/internal/text/font-description-run.h>
#include <dali-toolkit/internal/text/text-definitions.h>
#include <dali-toolkit/internal/text/text-io.h>

using namespace Dali;
using namespace Toolkit;
using namespace Text;

namespace
{
  ///////////////////////////////////////////////////////////

  struct TokenComparisonData
  {
    std::string description;
    std::string string1; ///< must be in lower case!!!!
    std::string string2;
    bool expectedResult;
  };

  bool TokenComparisonTest( const TokenComparisonData& data )
  {
    std::cout << "  testing " << data.description << std::endl;

    const bool result = TokenComparison( data.string1,
                                         data.string2.c_str(),
                                         data.string2.size() );

    if( result != data.expectedResult )
    {
      std::cout << "  different conparison result : " << result << ", expected : " << data.expectedResult << std::endl;
      std::cout << "  comparing : [" << data.string1 << "] and [" << data.string2 << "]" << std::endl;

      return false;
    }

    return true;
  }

  ///////////////////////////////////////////////////////////

  struct ColorStringToVector4Data
  {
    std::string description;
    std::string colorStr;
    Vector4 expectedColor;
  };

  bool ColorStringToVector4Test( const ColorStringToVector4Data& data )
  {
    std::cout << "  testing " << data.description << std::endl;

    Vector4 color;
    ColorStringToVector4( data.colorStr.c_str(), data.colorStr.size(), color );

    if( color != data.expectedColor )
    {
      std::cout << "  different color : " << color << ", expected : " << data.expectedColor << std::endl;
      return false;
    }

    return true;
  }

  ///////////////////////////////////////////////////////////

  struct Vector4ToColorStringData
  {
    std::string description;
    Vector4 color;
    std::string expectedColorStr;
  };

  bool Vector4ToColorStringTest( const Vector4ToColorStringData& data )
  {
    std::cout << "  testing " << data.description << std::endl;

    std::string colorStr;
    Vector4ToColorString( data.color, colorStr );

    if( colorStr != data.expectedColorStr )
    {
      std::cout << "  different color : [" << colorStr << "], expected : [" << data.expectedColorStr << "]" << std::endl;
      return false;
    }

    return true;
  }

  ///////////////////////////////////////////////////////////

  struct StringToVector2Data
  {
    std::string description;
    std::string vector2Str;
    Vector2 expectedVector2;
  };

  bool StringToVector2Test( const StringToVector2Data& data )
  {
    std::cout << "  testing " << data.description << std::endl;

    Vector2 vector2;
    StringToVector2( data.vector2Str.c_str(), data.vector2Str.size(), vector2 );

    if( vector2 != data.expectedVector2 )
    {
      std::cout << "  different vector2 : " << vector2 << ", expected : " << data.expectedVector2 << std::endl;
      return false;
    }

    return true;
  }

  ///////////////////////////////////////////////////////////


  struct Vector2ToStringData
  {
    std::string description;
    Vector2 vector2;
    std::string expectedVector2Str;
  };

  bool Vector2ToStringTest( const Vector2ToStringData& data )
  {
    std::cout << "  testing " << data.description << std::endl;

    std::string vector2Str;
    Vector2ToString( data.vector2, vector2Str );

    if( vector2Str != data.expectedVector2Str )
    {
      std::cout << "  different vector2 : [" << vector2Str << "], expected : [" << data.expectedVector2Str << "]" << std::endl;
      return false;
    }

    return true;
  }

} // namespace

int UtcDaliTextTokenComparison(void)
{
  tet_infoline(" UtcDaliTextTokenComparison");

  const TokenComparisonData data[] =
  {
    {
      "void texts",
      "",
      "",
      true
    },
    {
      "different size text",
      "hello",
      "world!",
      false
    },
    {
      "different texts",
      "hello",
      "world",
      false
    },
    {
      "same texts",
      "world",
      "wOrLD",
      true
    },
    {
      "some punctuation characters, numbers, ...",
      "hello0123456789.![?]",
      "Hello0123456789.![?]",
      true
    }

  };
  const unsigned int numberOfTests = 5u;

  for( unsigned int index = 0u; index < numberOfTests; ++index )
  {
    ToolkitTestApplication application;
    if( !TokenComparisonTest( data[index] ) )
    {
      tet_result(TET_FAIL);
    }
  }

  tet_result(TET_PASS);
  END_TEST;
}

int UtcDaliTextColorStringToVector4(void)
{
  tet_infoline(" UtcDaliTextColorStringToVector4");

  const ColorStringToVector4Data data[] =
  {
    {
      "black string",
      "bLack",
      Color::BLACK
    },
    {
      "white string",
      "White",
      Color::WHITE
    },
    {
      "red string",
      "reD",
      Color::RED
    },
    {
      "green string",
      "green",
      Color::GREEN
    },
    {
      "blue string",
      "blue",
      Color::BLUE
    },
    {
      "yellow string",
      "yeLloW",
      Color::YELLOW
    },
    {
      "magenta string",
      "MagEnta",
      Color::MAGENTA
    },
    {
      "cyan string",
      "CyaN",
      Color::CYAN
    },
    {
      "transparent string",
      "transparent",
      Color::TRANSPARENT
    },
    {
      "3 component web color",
      "#F00",
      Color::RED
    },
    {
      "6 component web color",
      "#fF0000",
      Color::RED
    },
    {
      "hex color red (ARGB)",
      "0xffff0000",
      Color::RED
    },
    {
      "hex color green (ARGB)",
      "0xFf00FF00",
      Color::GREEN
    },
    {
      "undefined color",
      "undefined",
      Vector4::ZERO
    },
  };
  const unsigned int numberOfTests = 14u;

  for( unsigned int index = 0u; index < numberOfTests; ++index )
  {
    ToolkitTestApplication application;
    if( !ColorStringToVector4Test( data[index] ) )
    {
      tet_result(TET_FAIL);
    }
  }

  tet_result(TET_PASS);
  END_TEST;
}

int UtcDaliTextVector4ToColorString(void)
{
  tet_infoline(" UtcDaliTextVector4ToColorString");

  const Vector4ToColorStringData data[] =
  {
    {
      "black color",
      Color::BLACK,
      "black"
    },
    {
      "white string",
      Color::WHITE,
      "white"
    },
    {
      "red string",
      Color::RED,
      "red"
    },
    {
      "green string",
      Color::GREEN,
      "green"
    },
    {
      "blue string",
      Color::BLUE,
      "blue"
    },
    {
      "yellow string",
      Color::YELLOW,
      "yellow"
    },
    {
      "magenta string",
      Color::MAGENTA,
      "magenta",
    },
    {
      "cyan string",
      Color::CYAN,
      "cyan"
    },
    {
      "transparent string",
      Color::TRANSPARENT,
      "transparent"
    },
    {
      "hex color",
      Vector4( 0.4f, 0.5f, 0.6f, 1.f ),
      "0xff667f99"
    },
  };
  const unsigned int numberOfTests = 10u;

  for( unsigned int index = 0u; index < numberOfTests; ++index )
  {
    ToolkitTestApplication application;
    if( !Vector4ToColorStringTest( data[index] ) )
    {
      tet_result(TET_FAIL);
    }
  }

  tet_result(TET_PASS);
  END_TEST;
}

int UtcDaliTextStringToVector2(void)
{
  tet_infoline(" UtcDaliTextStringToVector2");
  const StringToVector2Data data[] =
  {
    {
      "void text",
      "",
      Vector2::ZERO
    },
    {
      "zero zero",
      "0 0",
      Vector2::ZERO
    },
    {
      "five four",
      "5 4",
      Vector2(5.f, 4.f)
    }
  };
  const unsigned int numberOfTests = 3u;

  for( unsigned int index = 0u; index < numberOfTests; ++index )
  {
    ToolkitTestApplication application;
    if( !StringToVector2Test( data[index] ) )
    {
      tet_result(TET_FAIL);
    }
  }

  tet_result(TET_PASS);
  END_TEST;
}

int UtcDaliTextVector2ToString(void)
{
  tet_infoline(" UtcDaliTextVector2ToString");
  const Vector2ToStringData data[] =
  {
    {
      "zero zero",
      Vector2::ZERO,
      "0 0",
    },
    {
      "five four",
      Vector2(5.f, 4.f),
      "5 4",
    }
  };
  const unsigned int numberOfTests = 2u;

  for( unsigned int index = 0u; index < numberOfTests; ++index )
  {
    ToolkitTestApplication application;
    if( !Vector2ToStringTest( data[index] ) )
    {
      tet_result(TET_FAIL);
    }
  }

  tet_result(TET_PASS);
  END_TEST;
}
