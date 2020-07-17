/*
 * Copyright (c) 2020 Samsung Electronics Co., Ltd.
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
#include <fstream>
#include <stdlib.h>
#include <unistd.h>

#include <dali-toolkit-test-suite-utils.h>
#include <dali-toolkit/dali-toolkit.h>
#include <toolkit-text-utils.h>

// EXTERNAL INCLUDES
#include <dali-toolkit/devel-api/text/bitmap-font.h>
#include <dali/devel-api/text-abstraction/bitmap-font.h>
#include <dali-toolkit/devel-api/text/text-utils-devel.h>
#include <devel-api/adaptor-framework/image-loading.h>



using namespace std;
using namespace Dali;
using namespace Toolkit;
using namespace Text;

namespace
{
const std::string TEST_IMAGE_FILE_NAME1 =  TEST_RESOURCE_DIR  "/application-icon-20.png";
const std::string TEST_IMAGE_FILE_NAME2 =  TEST_RESOURCE_DIR  "/application-icon-26.png";

const std::vector<std::string> embeddedItems = { TEST_IMAGE_FILE_NAME2, TEST_IMAGE_FILE_NAME2, TEST_IMAGE_FILE_NAME2, TEST_IMAGE_FILE_NAME2, TEST_IMAGE_FILE_NAME2 };

struct CircularTextData
{
  std::string                      description;
  std::string                      text;
  DevelText::RendererParameters&   textParameters;
  const std::vector<std::string>&  embeddedItems;
  bool                             blend:1;
};


bool CircularRenderTest( const CircularTextData& data )
{
  bool ret = true;

  Dali::Vector<Dali::Toolkit::DevelText::EmbeddedItemInfo> embeddedItemLayout;

  Devel::PixelBuffer pixelBuffer = Toolkit::DevelText::Render( data.textParameters, embeddedItemLayout );

   const int dstWidth = static_cast<int>( pixelBuffer.GetWidth() );
   const int dstHeight = static_cast<int>( pixelBuffer.GetHeight() );

   unsigned int index = 0u;
   for( const auto& itemLayout : embeddedItemLayout )
   {
     int width = static_cast<int>( itemLayout.size.width );
     int height = static_cast<int>( itemLayout.size.height );
     int x = static_cast<int>( itemLayout.position.x );
     int y = static_cast<int>( itemLayout.position.y );

     Dali::Devel::PixelBuffer itemPixelBuffer = Dali::LoadImageFromFile( data.embeddedItems[index++] );
     itemPixelBuffer.Resize( width, height );
     itemPixelBuffer.Rotate( itemLayout.angle );

     width = static_cast<int>( itemPixelBuffer.GetWidth() );
     height = static_cast<int>( itemPixelBuffer.GetHeight() );

     Dali::Pixel::Format itemPixelFormat = itemPixelBuffer.GetPixelFormat();

     // Check if the item is out of the buffer.
     if( ( x + width < 0 ) ||
         ( x > dstWidth ) ||
         ( y < 0 ) ||
         ( y - height > dstHeight ) )
     {
       // The embedded item is completely out of the buffer.
       continue;
     }

     // Crop if it exceeds the boundaries of the destination buffer.
     int layoutX = 0;
     int layoutY = 0;
     int cropX = 0;
     int cropY = 0;
     int newWidth = width;
     int newHeight = height;

     bool crop = false;

     if( 0 > x )
     {
       newWidth += x;
       cropX = std::abs( x );
       crop = true;
     }
     else
     {
       layoutX = x;
     }

     if( cropX + newWidth > dstWidth )
     {
       crop = true;
       newWidth -= ( ( cropX + newWidth ) - dstWidth );
     }

     layoutY = y;
     if( 0.f > layoutY )
     {
       newHeight += layoutY;
       cropY = std::abs(layoutY);
       crop = true;
     }

     if( cropY + newHeight > dstHeight )
     {
       crop = true;
       newHeight -= ( ( cropY + newHeight ) - dstHeight );
     }

     uint16_t uiCropX = static_cast<uint16_t>(cropX);
     uint16_t uiCropY = static_cast<uint16_t>(cropY);
     uint16_t uiNewWidth = static_cast<uint16_t>(newWidth);
     uint16_t uiNewHeight = static_cast<uint16_t>(newHeight);

     if( crop )
     {
       itemPixelBuffer.Crop( uiCropX, uiCropY, uiNewWidth, uiNewHeight );
     }

     // Blend the item pixel buffer with the text's color according its blending mode.
     if( Dali::TextAbstraction::ColorBlendingMode::MULTIPLY == itemLayout.colorBlendingMode )
     {
       Dali::Devel::PixelBuffer buffer = Dali::Devel::PixelBuffer::New( uiNewWidth,
                                                                        uiNewHeight,
                                                                        itemPixelFormat );

       unsigned char* bufferPtr = buffer.GetBuffer();
       const unsigned char* itemBufferPtr = itemPixelBuffer.GetBuffer();
       const unsigned int bytesPerPixel = Dali::Pixel::GetBytesPerPixel(itemPixelFormat);
       const unsigned int size = uiNewWidth * uiNewHeight * bytesPerPixel;

       for (unsigned int i = 0u; i < size; i += bytesPerPixel)
       {
         *(bufferPtr + 0u) = static_cast<unsigned char>( static_cast<float>( *(itemBufferPtr + 0u) ) * data.textParameters.textColor.r );
         *(bufferPtr + 1u) = static_cast<unsigned char>( static_cast<float>( *(itemBufferPtr + 1u) ) * data.textParameters.textColor.g );
         *(bufferPtr + 2u) = static_cast<unsigned char>( static_cast<float>( *(itemBufferPtr + 2u) ) * data.textParameters.textColor.b );
         *(bufferPtr + 3u) = static_cast<unsigned char>( static_cast<float>( *(itemBufferPtr + 3u) ) * data.textParameters.textColor.a );

         itemBufferPtr += bytesPerPixel;
         bufferPtr += bytesPerPixel;
       }

       itemPixelBuffer = buffer;
     }

     Dali::Toolkit::DevelText::UpdateBuffer(itemPixelBuffer, pixelBuffer, layoutX, layoutY, data.blend);
   }

  return  ret;
}

} // namespace

int UtcDaliTextCircularBitmapFont(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliTextCircularBitmapFont");

  Dali::Toolkit::DevelText::BitmapFontDescription description;
  Dali::Toolkit::DevelText::Glyph glyph;
  glyph.url = "BitmapFontUrl";
  glyph.utf8[0u] = 0u;
  glyph.utf8[1u] = 0u;
  glyph.utf8[2u] = 0u;
  glyph.utf8[3u] = 0u;
  glyph.ascender = 1.f;
  glyph.descender = 1.f;
  description.glyphs.push_back( glyph );

  TextAbstraction::BitmapFont bitmapFont;
  Dali::Toolkit::DevelText::CreateBitmapFont( description, bitmapFont );

  for( const auto& bitmapGlyph : bitmapFont.glyphs )
  {
    if( glyph.url != bitmapGlyph.url )
    {
      std::cout << "  different output string : " << bitmapGlyph.url << ", expected : " << glyph.url << " " << std::endl;
      tet_result(TET_FAIL);
    }
  }

  tet_result(TET_PASS);
  END_TEST;
}

int UtcDaliTextCircularShadowText(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliTextCircularShadowText");

  Dali::Toolkit::DevelText::ShadowParameters shadowParameters;
  Devel::PixelBuffer outPixelBuffer;
  shadowParameters.input = Devel::PixelBuffer::New( 100, 100, Pixel::RGBA8888 );
  shadowParameters.textColor = Color::BLACK;
  shadowParameters.color =  Color::BLACK;
  shadowParameters.offset.x = 10u;
  shadowParameters.offset.y = 10u;
  shadowParameters.blendShadow = true;
  outPixelBuffer = Dali::Toolkit::DevelText::CreateShadow( shadowParameters );
  DALI_TEST_CHECK( outPixelBuffer );
  DALI_TEST_EQUALS( outPixelBuffer.GetPixelFormat(), Pixel::RGBA8888, TEST_LOCATION  );

  shadowParameters.blendShadow = false;
  outPixelBuffer = Dali::Toolkit::DevelText::CreateShadow( shadowParameters );
  DALI_TEST_CHECK( outPixelBuffer );
  DALI_TEST_EQUALS( outPixelBuffer.GetPixelFormat(), Pixel::RGBA8888, TEST_LOCATION  );

  shadowParameters.input = Devel::PixelBuffer::New( 100, 100, Pixel::A8 );
  outPixelBuffer = Dali::Toolkit::DevelText::CreateShadow( shadowParameters );
  DALI_TEST_CHECK( outPixelBuffer );
  DALI_TEST_EQUALS( outPixelBuffer.GetPixelFormat(), Pixel::RGBA8888, TEST_LOCATION  );

  tet_result(TET_PASS);
  END_TEST;
}

int UtcDaliTextCircularPixelBufferText(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliTextCircularPixelBufferText");

  Devel::PixelBuffer pixbuf = Devel::PixelBuffer::New( 10, 10, Pixel::A8 );
  Vector4 color;
  Devel::PixelBuffer pixelBufferRgba = Dali::Toolkit::DevelText::ConvertToRgba8888( pixbuf, color, true );
  pixelBufferRgba = Dali::Toolkit::DevelText::ConvertToRgba8888( pixbuf, color, false );
  DALI_TEST_CHECK( pixelBufferRgba );
  DALI_TEST_EQUALS( pixelBufferRgba.GetPixelFormat(), Pixel::RGBA8888, TEST_LOCATION  );

  pixbuf = Devel::PixelBuffer::New( 10, 10, Pixel::RGBA8888 );
  pixelBufferRgba = Dali::Toolkit::DevelText::ConvertToRgba8888( pixbuf, color, false );
  DALI_TEST_CHECK( pixelBufferRgba );
  DALI_TEST_EQUALS( pixelBufferRgba.GetPixelFormat(), Pixel::RGBA8888, TEST_LOCATION  );

  tet_result(TET_PASS);
  END_TEST;
}

int UtcDaliTextCircularNoText(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliTextCircularNoText");

  Dali::Toolkit::DevelText::RendererParameters textParameters;
  textParameters.text = "";
  textParameters.fontSize = 25.f;
  textParameters.textWidth = 360u;
  textParameters.textHeight = 360u;

  CircularTextData data =
  {
      "No text",
      "",
      textParameters,
      embeddedItems,
      true
  };

  if( !CircularRenderTest( data ) )
  {
    tet_result(TET_FAIL);
  }

  textParameters.text = "<item 'width'=26 'height'=26/>";
  textParameters.markupEnabled = true;
  if( !CircularRenderTest( data ) )
  {
    tet_result(TET_FAIL);
  }

  tet_result(TET_PASS);
  END_TEST;
}

int UtcDaliTextCircularIncrementAngle(void)
{

  ToolkitTestApplication application;
  tet_infoline(" UtcDaliTextCircularIncrementAngle");

  const std::string image1 = "<item 'width'=26 'height'=26 'url'='" + TEST_IMAGE_FILE_NAME1 + "'/>";
  const std::string image2 = "<item 'width'=26 'height'=26/>";

  Dali::Toolkit::DevelText::RendererParameters textParameters;
  textParameters.text = "Hello " + image1 + " world " + image2 + " this " + image1 + " is " + image2 + " a " + image1 + " demo " + image2 + " of " + image1 + " circular " + image2 + " text " + image1 + " width " + image2 + " icons.";
  textParameters.horizontalAlignment = "center";
  textParameters.verticalAlignment = "center";
  textParameters.circularAlignment = "center";
  textParameters.fontFamily = "SamsungUI";
  textParameters.fontWeight = "";
  textParameters.fontWidth = "";
  textParameters.fontSlant = "";
  textParameters.layout = "circular";
  textParameters.textColor = Color::BLACK;
  textParameters.fontSize = 25.f;
  textParameters.textWidth = 360u;
  textParameters.textHeight = 360u;
  textParameters.radius = 180u;
  textParameters.beginAngle = 15.f;
  textParameters.incrementAngle = 0.f;
  textParameters.ellipsisEnabled = true;
  textParameters.markupEnabled = true;


  CircularTextData data =
  {
      "IncrementAngle",
      "",
      textParameters,
      embeddedItems,
      true
  };

  if( !CircularRenderTest( data ) )
  {
    tet_result(TET_FAIL);
  }

  tet_result(TET_PASS);
  END_TEST;
}



int UtcDaliTextCircularMarkup(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliTextCircularMarkup");

  Dali::Toolkit::DevelText::RendererParameters textParameters;
  textParameters.text = "Hello World Hello World Hello World Hello World Hello World Hello World Hello World Hello World Hello World Hello World Hello World Hello World Hello World";
  textParameters.horizontalAlignment = "center";
  textParameters.verticalAlignment = "center";
  textParameters.circularAlignment = "center";
  textParameters.fontFamily = "SamsungUI";
  textParameters.fontWeight = "";
  textParameters.fontWidth = "";
  textParameters.fontSlant = "";
  textParameters.layout = "circular";
  textParameters.textColor = Color::BLACK;
  textParameters.fontSize = 25.f;
  textParameters.textWidth = 360u;
  textParameters.textHeight = 360u;
  textParameters.radius = 180u;
  textParameters.beginAngle = 15.f;
  textParameters.incrementAngle = 360.f;
  textParameters.ellipsisEnabled = true;
  textParameters.markupEnabled = false;

  CircularTextData data =
  {
      "Markup",
      "",
      textParameters,
      embeddedItems,
      true
  };

  if( !CircularRenderTest( data ) )
  {
    tet_result(TET_FAIL);
  }

  tet_result(TET_PASS);
  END_TEST;
}

int UtcDaliTextCircularFont(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliTextCircularFont");

  const std::string image1 = "<item 'width'=26 'height'=26 'url'='" + TEST_IMAGE_FILE_NAME1 + "'/>";
  const std::string image2 = "<item 'width'=26 'height'=26/>";

  Dali::Toolkit::DevelText::RendererParameters textParameters;
  textParameters.text = "Hello " + image1 + " world " + image2 + " this " + image1 + " is " + image2 + " a " + image1 + " demo " + image2 + " of " + image1 + " circular " + image2 + " text " + image1 + " width " + image2 + " icons.";
  textParameters.horizontalAlignment = "center";
  textParameters.verticalAlignment = "center";
  textParameters.circularAlignment = "center";
  textParameters.fontFamily = "SamsungUI";
  textParameters.fontWeight = "thin";
  textParameters.fontWidth = "condensed";
  textParameters.fontSlant = "normal";
  textParameters.layout = "circular";
  textParameters.textColor = Color::BLACK;
  textParameters.fontSize = 25.f;
  textParameters.textWidth = 360u;
  textParameters.textHeight = 360u;
  textParameters.radius = 180u;
  textParameters.beginAngle = 15.f;
  textParameters.incrementAngle = 360.f;
  textParameters.ellipsisEnabled = true;
  textParameters.markupEnabled = true;

  CircularTextData data =
  {
      "Font",
      "",
      textParameters,
      embeddedItems,
      true
  };

  if( !CircularRenderTest( data ) )
  {
    tet_result(TET_FAIL);
  }

  tet_result(TET_PASS);
  END_TEST;
}

int UtcDaliTextCircularAlignment(void)
{

  ToolkitTestApplication application;
  tet_infoline(" UtcDaliTextCircularAlignment");

  const std::string image1 = "<item 'width'=26 'height'=26 'url'='" + TEST_IMAGE_FILE_NAME1 + "'/>";
  const std::string image2 = "<item 'width'=26 'height'=26/>";

  Dali::Toolkit::DevelText::RendererParameters textParameters;
  textParameters.text = "Hello " + image1 + " world " + image2 + " this " + image1 + " is " + image2 + " a " + image1 + " demo " + image2 + " of " + image1 + " circular " + image2 + " text " + image1 + " width " + image2 + " icons.";
  textParameters.horizontalAlignment = "center";
  textParameters.verticalAlignment = "center";
  textParameters.circularAlignment = "center";
  textParameters.fontFamily = "SamsungUI";
  textParameters.fontWeight = "";
  textParameters.fontWidth = "";
  textParameters.fontSlant = "";
  textParameters.layout = "circular";
  textParameters.textColor = Color::BLACK;
  textParameters.fontSize = 25.f;
  textParameters.textWidth = 360u;
  textParameters.textHeight = 360u;
  textParameters.radius = 180u;
  textParameters.beginAngle = 15.f;
  textParameters.incrementAngle = 360.f;
  textParameters.ellipsisEnabled = true;
  textParameters.markupEnabled = true;

  CircularTextData data =
  {
      "Alignment",
      "",
      textParameters,
      embeddedItems,
      true
  };

  if( !CircularRenderTest( data ) )
  {
    tet_result(TET_FAIL);
  }

  textParameters.horizontalAlignment = "begin";
  textParameters.verticalAlignment = "top";
  textParameters.circularAlignment = "begin";

  if( !CircularRenderTest( data ) )
  {
    tet_result(TET_FAIL);
  }

  textParameters.horizontalAlignment = "end";
  textParameters.verticalAlignment = "bottom";
  textParameters.circularAlignment = "end";

  if( !CircularRenderTest( data ) )
  {
    tet_result(TET_FAIL);
  }

  tet_result(TET_PASS);
  END_TEST;
}

int UtcDaliTextCircularRTL(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliTextCircularRTL");

  const std::string image1 = "<item 'width'=26 'height'=26 'url'='" + TEST_IMAGE_FILE_NAME1 + "'/>";
  const std::string image2 = "<item 'width'=26 'height'=26/>";

  Dali::Toolkit::DevelText::RendererParameters textParameters;
  textParameters.text = "مرحبا بالعالم" + image1 + " hello world " + image2 + " this " + image1 + " is " + image2 + " a " + image1 + " demo " + image2 + " of " + image1 + " circular " + image2 + " text " + image1 + " width " + image2 + " icons.";
  textParameters.horizontalAlignment = "center";
  textParameters.verticalAlignment = "center";
  textParameters.circularAlignment = "center";
  textParameters.fontFamily = "SamsungUI";
  textParameters.fontWeight = "";
  textParameters.fontWidth = "";
  textParameters.fontSlant = "";
  textParameters.layout = "circular";
  textParameters.textColor = Color::BLACK;
  textParameters.fontSize = 25.f;
  textParameters.textWidth = 360u;
  textParameters.textHeight = 360u;
  textParameters.radius = 180u;
  textParameters.beginAngle = 15.f;
  textParameters.incrementAngle = 360.f;
  textParameters.ellipsisEnabled = true;
  textParameters.markupEnabled = true;

  CircularTextData data =
  {
      "RTL",
      "",
      textParameters,
      embeddedItems,
      true
  };

  if( !CircularRenderTest( data ) )
  {
    tet_result(TET_FAIL);
  }

  textParameters.circularAlignment = "begin";
  if( !CircularRenderTest( data ) )
  {
    tet_result(TET_FAIL);
  }

  textParameters.circularAlignment = "end";
  if( !CircularRenderTest( data ) )
  {
    tet_result(TET_FAIL);
  }

  textParameters.text = "שלום עולם مرحبا بالعالم שלום עולם مرحبا بالعالم שלום עולם مرحبا بالعالم";
  textParameters.layout = "singleLine";
  textParameters.horizontalAlignment = "end";
  textParameters.fontSize = 90.f;
  if( !CircularRenderTest( data ) )
  {
    tet_result(TET_FAIL);
  }

  tet_result(TET_PASS);
  END_TEST;
}

int UtcDaliTextCircularN(void)
{

  ToolkitTestApplication application;
  tet_infoline(" UtcDaliTextCircularN");

  const std::string image1 = "<item 'width'=26 'height'=26 'url'='" + TEST_IMAGE_FILE_NAME1 + "'/>";
  const std::string image2 = "<item 'width'=26 'height'=26/>";

  Dali::Toolkit::DevelText::RendererParameters textParameters;
  textParameters.text = "hello " + image1 + " world " + image2 + " this " + image1 + " is " + image2 + " a " + image1 + " demo " + image2 + " of " + image1 + " circular " + image2 + " text " + image1 + " width " + image2 + " icons.";
  textParameters.horizontalAlignment = "center";
  textParameters.verticalAlignment = "center";
  textParameters.circularAlignment = "center";
  textParameters.fontFamily = "SamsungUI";
  textParameters.fontWeight = "";
  textParameters.fontWidth = "";
  textParameters.fontSlant = "";
  textParameters.layout = "singleLine";
  textParameters.textColor = Color::BLACK;
  textParameters.fontSize = 25.f;
  textParameters.textWidth = 360u;
  textParameters.textHeight = 360u;
  textParameters.radius = 180u;
  textParameters.beginAngle = 15.f;
  textParameters.incrementAngle = 360.f;
  textParameters.ellipsisEnabled = true;
  textParameters.markupEnabled = true;

  CircularTextData data =
  {
      "singleLine",
      "",
      textParameters,
      embeddedItems,
      true
  };

  if( !CircularRenderTest( data ) )
  {
    tet_result(TET_FAIL);
  }

  textParameters.verticalAlignment = "top";
  if( !CircularRenderTest( data ) )
  {
    tet_result(TET_FAIL);
  }

  textParameters.verticalAlignment = "bottom";
  if( !CircularRenderTest( data ) )
  {
    tet_result(TET_FAIL);
  }

  textParameters.textWidth = 90u;
  if( !CircularRenderTest( data ) )
  {
    tet_result(TET_FAIL);
  }

  tet_result(TET_PASS);
  END_TEST;
}

int UtcDaliTextCircularBlend(void)
{
  tet_infoline(" UtcDaliTextCircularN");

  ToolkitTestApplication application;
  auto scene = application.GetScene();
  scene.SetBackgroundColor( Color::WHITE );
  scene.SetBackgroundColor( Vector4( 0.04f, 0.345f, 0.392f, 1.0f ) );


  const std::string image1 = "<item 'width'=26 'height'=26 'url'='" + TEST_IMAGE_FILE_NAME1 + "'/>";
  const std::string image2 = "<item 'width'=26 'height'=26/>";

  Dali::Toolkit::DevelText::RendererParameters textParameters;
  textParameters.text = "hello " + image1 + " world " + image2 + " this " + image1 + " is " + image2 + " a " + image1 + " demo " + image2 + " of " + image1 + " circular " + image2 + " text " + image1 + " width " + image2 + " icons.";
  textParameters.horizontalAlignment = "center";
  textParameters.verticalAlignment = "center";
  textParameters.circularAlignment = "center";
  textParameters.fontFamily = "SamsungUI";
  textParameters.fontWeight = "";
  textParameters.fontWidth = "";
  textParameters.fontSlant = "";
  textParameters.layout = "circular";
  textParameters.textColor = Color::BLACK;
  textParameters.fontSize = 25.f;
  textParameters.textWidth = 360u;
  textParameters.textHeight = 360u;
  textParameters.radius = 180u;
  textParameters.beginAngle = 15.f;
  textParameters.incrementAngle = 360.f;
  textParameters.ellipsisEnabled = true;
  textParameters.markupEnabled = true;

  CircularTextData data =
  {
      "blend",
      "",
      textParameters,
      embeddedItems,
      false
  };

  if( !CircularRenderTest( data ) )
  {
    tet_result(TET_FAIL);
  }

  tet_result(TET_PASS);
  END_TEST;
}

int UtcDaliTextCircularEllipsis(void)
{
  tet_infoline(" UtcDaliTextCircularEllipsis");

  ToolkitTestApplication application;

  const std::string image1 = "<item 'width'=26 'height'=26 'url'='" + TEST_IMAGE_FILE_NAME1 + "'/>";
  const std::string image2 = "<item 'width'=26 'height'=26/>";

  Dali::Toolkit::DevelText::RendererParameters textParameters;
  textParameters.text = "hello " + image1 + " world " + image2 + " this " + image1 + " is " + image2 + " a " + image1 + " demo " + image2 + " of " + image1 + " circular " + image2 + " text " + image1 + " width " + image2 + " icons.";
  textParameters.horizontalAlignment = "center";
  textParameters.verticalAlignment = "center";
  textParameters.circularAlignment = "center";
  textParameters.fontFamily = "SamsungUI";
  textParameters.fontWeight = "";
  textParameters.fontWidth = "";
  textParameters.fontSlant = "";
  textParameters.layout = "circular";
  textParameters.textColor = Color::BLACK;
  textParameters.fontSize = 25.f;
  textParameters.textWidth = 360u;
  textParameters.textHeight = 360u;
  textParameters.radius = 180u;
  textParameters.beginAngle = 15.f;
  textParameters.incrementAngle = 360.f;
  textParameters.ellipsisEnabled = false;
  textParameters.markupEnabled = true;

  CircularTextData data =
  {
      "ellipsis",
      "",
      textParameters,
      embeddedItems,
      true
  };

  if( !CircularRenderTest( data ) )
  {
    tet_result(TET_FAIL);
  }

  textParameters.layout = "singleLine";
  textParameters.textHeight = 50u;
  textParameters.ellipsisEnabled = true;

  if( !CircularRenderTest( data ) )
  {
    tet_result(TET_FAIL);
  }

  tet_result(TET_PASS);
  END_TEST;
}

int UtcDaliTextCircularEmoji(void)
{
  tet_infoline(" UtcDaliTextCircularEmoji");

  ToolkitTestApplication application;

  const std::string image1 = "<item 'width'=26 'height'=26 'url'='" + TEST_IMAGE_FILE_NAME1 + "'/>";
  const std::string image2 = "<item 'width'=26 'height'=26/>";

  Dali::Toolkit::DevelText::RendererParameters textParameters;
  textParameters.text = "<font family='BreezeColorEmoji' size='60'>\xF0\x9F\x98\x81 \xF0\x9F\x98\x82 \xF0\x9F\x98\x83 \xF0\x9F\x98\x84</font>";
  textParameters.horizontalAlignment = "center";
  textParameters.verticalAlignment = "center";
  textParameters.circularAlignment = "center";
  textParameters.fontFamily = "SamsungUI";
  textParameters.fontWeight = "";
  textParameters.fontWidth = "";
  textParameters.fontSlant = "";
  textParameters.layout = "circular";
  textParameters.textColor = Color::BLACK;
  textParameters.fontSize = 25.f;
  textParameters.textWidth = 360u;
  textParameters.textHeight = 360u;
  textParameters.radius = 180u;
  textParameters.beginAngle = 15.f;
  textParameters.incrementAngle = 360.f;
  textParameters.ellipsisEnabled = true;
  textParameters.markupEnabled = true;

  CircularTextData data =
  {
      "Emoji",
      "",
      textParameters,
      embeddedItems,
      true
  };

  if( !CircularRenderTest( data ) )
  {
    tet_result(TET_FAIL);
  }

  tet_result(TET_PASS);
  END_TEST;
}

int UtcDaliTextUpdateBufferFormatCheck(void)
{
  tet_infoline(" UtcDaliTextUpdateBufferFormatCheck");
  ToolkitTestApplication application;

  Devel::PixelBuffer srcBuffer = Devel::PixelBuffer::New( 10, 10, Pixel::RGBA8888 );
  Devel::PixelBuffer dstBuffer = Devel::PixelBuffer::New( 10, 10, Pixel::A8 );

  Dali::Toolkit::DevelText::UpdateBuffer(srcBuffer, dstBuffer, 0, 0, true);

  Devel::PixelBuffer compressedSrcBuffer = Devel::PixelBuffer::New( 10, 10, Pixel::COMPRESSED_R11_EAC );
  Devel::PixelBuffer compressedDstBuffer = Devel::PixelBuffer::New( 10, 10, Pixel::COMPRESSED_R11_EAC );
  Dali::Toolkit::DevelText::UpdateBuffer(compressedSrcBuffer, compressedDstBuffer, 0, 0, true);

  tet_result(TET_PASS);
  END_TEST;
}

int UtcDaliTextCircularTextColor(void)
{
  tet_infoline(" UtcDaliTextCircularTextColor");

  ToolkitTestApplication application;

  const std::string image1 = "<item 'width'=26 'height'=26 'url'='" + TEST_IMAGE_FILE_NAME1 + "'/>";
  const std::string image2 = "<item 'width'=26 'height'=26/>";

  Dali::Toolkit::DevelText::RendererParameters textParameters;
  textParameters.text = "hello " + image1 + " <color value='blue'>world</color> " + image2 + " this " + image1 + " is " + image2 + " a " + image1 + " demo " + image2 + " of " + image1 + " circular " + image2 + " text " + image1 + " width " + image2 + " icons.";
  textParameters.horizontalAlignment = "center";
  textParameters.verticalAlignment = "center";
  textParameters.circularAlignment = "center";
  textParameters.fontFamily = "SamsungUI";
  textParameters.fontWeight = "";
  textParameters.fontWidth = "";
  textParameters.fontSlant = "";
  textParameters.layout = "circular";
  textParameters.textColor = Color::BLACK;
  textParameters.fontSize = 25.f;
  textParameters.textWidth = 360u;
  textParameters.textHeight = 360u;
  textParameters.radius = 180u;
  textParameters.beginAngle = 15.f;
  textParameters.incrementAngle = 360.f;
  textParameters.ellipsisEnabled = true;
  textParameters.markupEnabled = true;

  CircularTextData data =
  {
      "textColor",
      "",
      textParameters,
      embeddedItems,
      true
  };

  if( !CircularRenderTest( data ) )
  {
    tet_result(TET_FAIL);
  }

  tet_result(TET_PASS);
  END_TEST;
}

int UtcDaliTextCircularColorBlend(void)
{
  tet_infoline(" UtcDaliTextCircularColorBlend");

  ToolkitTestApplication application;

  const std::string image1 = "<item 'width'=26 'height'=26 'url'='" + TEST_IMAGE_FILE_NAME1 +  "' 'color-blending'=multiply/>";
  const std::string image2 = "<item 'width'=26 'height'=26/>";

  Dali::Toolkit::DevelText::RendererParameters textParameters;
  textParameters.text = "hello " + image1 + " world " + image2 + " this " + image1 + " is " + image2 + " a " + image1 + " demo " + image2 + " of " + image1 + " circular " + image2 + " text " + image1 + " width " + image2 + " icons.";
  textParameters.horizontalAlignment = "center";
  textParameters.verticalAlignment = "center";
  textParameters.circularAlignment = "center";
  textParameters.fontFamily = "SamsungUI";
  textParameters.fontWeight = "";
  textParameters.fontWidth = "";
  textParameters.fontSlant = "";
  textParameters.layout = "circular";
  textParameters.textColor = Color::BLACK;
  textParameters.fontSize = 25.f;
  textParameters.textWidth = 360u;
  textParameters.textHeight = 360u;
  textParameters.radius = 180u;
  textParameters.beginAngle = 15.f;
  textParameters.incrementAngle = 360.f;
  textParameters.ellipsisEnabled = true;
  textParameters.markupEnabled = true;

  CircularTextData data =
  {
      "colorBlend",
      "",
      textParameters,
      embeddedItems,
      true
  };

  if( !CircularRenderTest( data ) )
  {
    tet_result(TET_FAIL);
  }

  tet_result(TET_PASS);
  END_TEST;
}
