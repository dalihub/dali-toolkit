/*
 * Copyright (c) 2019 Samsung Electronics Co., Ltd.
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

#include <dali/devel-api/adaptor-framework/vector-animation-renderer.h>
#include <dali/public-api/object/base-object.h>
#include <toolkit-application.h>

namespace Dali
{

namespace Internal
{

namespace Adaptor
{

class VectorAnimationRenderer: public Dali::BaseObject
{
public:

  VectorAnimationRenderer( const std::string& url )
  : mUrl( url ),
    mRenderer(),
    mWidth( 0 ),
    mHeight( 0 ),
    mPreviousFrame( 0 )
  {
  }

  void SetRenderer( Dali::Renderer renderer )
  {
    mRenderer = renderer;

    if( mWidth != 0 && mHeight != 0 )
    {
      Dali::TextureSet textureSet = mRenderer.GetTextures();
      Dali::Texture texture = Dali::Texture::New( TextureType::TEXTURE_2D, Pixel::RGBA8888, mWidth, mHeight );
      textureSet.SetTexture( 0, texture );
    }
  }

  void SetSize( uint32_t width, uint32_t height )
  {
    mWidth = width;
    mHeight = height;

    if( mRenderer )
    {
      Dali::TextureSet textureSet = mRenderer.GetTextures();
      Dali::Texture texture = Dali::Texture::New( TextureType::TEXTURE_2D, Pixel::RGBA8888, mWidth, mHeight );
      textureSet.SetTexture( 0, texture );
    }
  }

  void StopRender()
  {
  }

  bool Render( uint32_t frameNumber )
  {
    if( frameNumber == 1 && mPreviousFrame != frameNumber )
    {
      mPreviousFrame = frameNumber;
      // For test corverage
      return false;
    }
    mPreviousFrame = frameNumber;
    return true;
  }

  uint32_t GetTotalFrameNumber() const
  {
    return 5;
  }

  float GetFrameRate() const
  {
    return 60.0f;
  }

  void GetDefaultSize( uint32_t& width, uint32_t& height ) const
  {
    width = 100;
    height = 100;
  }

public:

  std::string mUrl;
  Dali::Renderer mRenderer;
  uint32_t mWidth;
  uint32_t mHeight;
  uint32_t mPreviousFrame;
};

inline VectorAnimationRenderer& GetImplementation( Dali::VectorAnimationRenderer& renderer )
{
  DALI_ASSERT_ALWAYS( renderer && "VectorAnimationRenderer handle is empty." );
  BaseObject& handle = renderer.GetBaseObject();
  return static_cast< Internal::Adaptor::VectorAnimationRenderer& >( handle );
}

inline const VectorAnimationRenderer& GetImplementation( const Dali::VectorAnimationRenderer& renderer )
{
  DALI_ASSERT_ALWAYS( renderer && "VectorAnimationRenderer handle is empty." );
  const BaseObject& handle = renderer.GetBaseObject();
  return static_cast< const Internal::Adaptor::VectorAnimationRenderer& >( handle );
}

} // namespace Adaptor

} // namespace Internal


/********************************************************************************/
/*********************************  PUBLIC CLASS  *******************************/
/********************************************************************************/

VectorAnimationRenderer VectorAnimationRenderer::New( const std::string& url )
{
  Internal::Adaptor::VectorAnimationRenderer* animationRenderer = new Internal::Adaptor::VectorAnimationRenderer( url );

  return VectorAnimationRenderer( animationRenderer );
}

VectorAnimationRenderer::VectorAnimationRenderer()
{
}

VectorAnimationRenderer::~VectorAnimationRenderer()
{
}

VectorAnimationRenderer::VectorAnimationRenderer( Internal::Adaptor::VectorAnimationRenderer* internal )
: BaseHandle( internal )
{
}

VectorAnimationRenderer::VectorAnimationRenderer( const VectorAnimationRenderer& handle )
: BaseHandle( handle )
{
}

VectorAnimationRenderer& VectorAnimationRenderer::operator=( const VectorAnimationRenderer& rhs )
{
  BaseHandle::operator=( rhs );
  return *this;
}

void VectorAnimationRenderer::SetRenderer( Renderer renderer )
{
  Internal::Adaptor::GetImplementation( *this ).SetRenderer( renderer );
}

void VectorAnimationRenderer::SetSize( uint32_t width, uint32_t height )
{
  Internal::Adaptor::GetImplementation( *this ).SetSize( width, height );
}

void VectorAnimationRenderer::StopRender()
{
  Internal::Adaptor::GetImplementation( *this ).StopRender();
}

bool VectorAnimationRenderer::Render( uint32_t frameNumber )
{
  return Internal::Adaptor::GetImplementation( *this ).Render( frameNumber );
}

uint32_t VectorAnimationRenderer::GetTotalFrameNumber() const
{
  return Internal::Adaptor::GetImplementation( *this ).GetTotalFrameNumber();
}

float VectorAnimationRenderer::GetFrameRate() const
{
  return Internal::Adaptor::GetImplementation( *this ).GetFrameRate();
}

void VectorAnimationRenderer::GetDefaultSize( uint32_t& width, uint32_t& height ) const
{
  Internal::Adaptor::GetImplementation( *this ).GetDefaultSize( width, height );
}

} // namespace Dali;

