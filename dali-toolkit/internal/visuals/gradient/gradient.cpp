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

#include "gradient.h"

#include <algorithm>    // std::sort
#include <dali/public-api/math/vector4.h>

namespace
{
// The maximum width of the lookup texture ( it is a 1-dimension texture with the height as 1 )
const unsigned int MAXIMUM_TEXTURE_RESOLUTION(128u);
}

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

Gradient::Gradient()
: mGradientUnits( OBJECT_BOUNDING_BOX ),
  mSpreadMethod( PAD )
{}

Gradient::~Gradient()
{}

void Gradient::AddStop( float offset, const Vector4& color )
{
  // the offset is clamped to the range [0.0, 1.0]
  mGradientStops.PushBack( GradientStop( Clamp( offset, 0.f, 1.f ), color) );
}

const Vector<Gradient::GradientStop>& Gradient::GetStops()
{
  return mGradientStops;
}

void Gradient::SetGradientUnits( GradientUnits gradientUnits )
{
  mGradientUnits = gradientUnits;
}

Gradient::GradientUnits Gradient::GetGradientUnits() const
{
  return mGradientUnits;
}

void Gradient::SetSpreadMethod( SpreadMethod spread )
{
  mSpreadMethod = spread;
}

Gradient::SpreadMethod Gradient::GetSpreadMethod() const
{
  return mSpreadMethod;
}

const Matrix3& Gradient::GetAlignmentTransform() const
{
  return mAlignmentTransform;
}

/**
 * Following the SVG gradient.
 *
 * Not only the spread method decides the texture wrap mode:
 *    PAD-->GL_CLAMP_TO_EDGE; REPEAT-->GL_REPEAT; REFLECT-->GL_MIRROR_REPEAT
 *
 *  If the stops have not covered the whole zero to one range,
 *  the REPEAT spread behaves different from the two others in the lookup texture generation.
 */
Dali::Texture Gradient::GenerateLookupTexture()
{
  std::sort( mGradientStops.Begin(), mGradientStops.End() );

  unsigned int numStops = mGradientStops.Count();

  /**
   * If the stops have not covered the whole zero to one range,
   * for PAD and REFLECT, use the color of the first stop to fill the range  [0.0, first stop offset)
   *                  and use the color of the last stop to fill the range (last stop offset, 1.0]
   * for REPEAT, mix the two color of the first and last stop to fill the remainder
   */
  bool tempFirstStop = false;
  if( mGradientStops[0].mOffset > 0.f )
  {
    tempFirstStop = true;
    Vector4 firstStopColor( mGradientStops[0].mStopColor ); // If spread method is PAD or REFLECT
    if( mSpreadMethod == REPEAT )
    {
      firstStopColor = ( mGradientStops[0].mStopColor * (1.f-mGradientStops[numStops-1].mOffset)
                       + mGradientStops[numStops-1].mStopColor  * mGradientStops[0].mOffset )
                   / ( mGradientStops[0].mOffset+1.f-mGradientStops[numStops-1].mOffset);
    }

    mGradientStops.Insert( mGradientStops.Begin(), GradientStop(0.f, firstStopColor) );
    numStops++;
  }

  bool tempLastStop = false;
  if( mGradientStops[numStops-1].mOffset < 1.f )
  {
    tempLastStop = true;
    Vector4 lastStopColor( mGradientStops[numStops-1].mStopColor ); // If spread method is PAD or REFLECT
    if( mSpreadMethod == REPEAT )
    {
      lastStopColor = mGradientStops[0].mStopColor;
    }
    mGradientStops.PushBack( GradientStop(1.f, lastStopColor) );
    numStops++;
  }

  /**
   * Generate the pixels with the color transit from one stop to next.
   */
  unsigned int resolution = EstimateTextureResolution();

  unsigned int bufferSize = resolution * 4u;
  unsigned char* pixels = new unsigned char[ bufferSize ];
  PixelData pixelData = PixelData::New( pixels, bufferSize, resolution, 1u, Pixel::RGBA8888, PixelData::DELETE_ARRAY );

  int segmentStart = 0;
  int segmentEnd = 0;
  int k = 0;
  float length = static_cast<float>(resolution);
  for( unsigned int i=0; i<numStops-1u; i++ )
  {
    segmentEnd = floorf(mGradientStops[i+1].mOffset * length + 0.5f);
    if( segmentEnd == segmentStart )
    {
      continue;
    }
    float segmentWidth = static_cast<float>(segmentEnd-segmentStart);

    for( int j = segmentStart; j<segmentEnd; j++ )
    {
      float ratio = static_cast<float>(j-segmentStart)/segmentWidth;
      Vector4 currentColor = mGradientStops[i].mStopColor * (1.f-ratio) + mGradientStops[i+1].mStopColor * ratio;
      pixels[k*4] = static_cast<unsigned char>( 255.f * Clamp( currentColor.r, 0.f, 1.f ) );
      pixels[k*4+1] = static_cast<unsigned char>( 255.f * Clamp( currentColor.g, 0.f, 1.f ) );
      pixels[k*4+2] = static_cast<unsigned char>( 255.f * Clamp( currentColor.b, 0.f, 1.f ) );
      pixels[k*4+3] = static_cast<unsigned char>( 255.f * Clamp( currentColor.a, 0.f, 1.f ) );
      k++;
    }
    segmentStart = segmentEnd;
  }

  Texture texture = Texture::New( TextureType::TEXTURE_2D, Pixel::RGBA8888, resolution, 1u );
  texture.Upload( pixelData );

  // remove the stops added temporarily for generating the pixels, as the spread method might get changed later
  if( tempLastStop )
  {
    mGradientStops.Erase( mGradientStops.Begin()+numStops-1 );
  }
  if( tempFirstStop )
  {
    mGradientStops.Erase( mGradientStops.Begin());
  }

  return texture;
}

unsigned int Gradient::EstimateTextureResolution()
{
  float minInterval = 1.0;
  for( unsigned int i=0, numStops = mGradientStops.Count(); i<numStops-1u; i++ )
  {
    float interval = mGradientStops[i+1].mOffset - mGradientStops[i].mOffset;
    minInterval = interval > minInterval ? minInterval:interval;
  }
  // Use at least three pixels for each segment between two stops
  unsigned int resolution = static_cast<int>(3.f/(minInterval+Math::MACHINE_EPSILON_100)+0.5f);
  // Clamp the resolution to handle the overlapping stops
  if( resolution > MAXIMUM_TEXTURE_RESOLUTION )
  {
    return MAXIMUM_TEXTURE_RESOLUTION;
  }

  return resolution;
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
