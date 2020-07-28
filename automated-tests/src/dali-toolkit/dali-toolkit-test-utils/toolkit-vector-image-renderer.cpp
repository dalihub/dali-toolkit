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

#include <dali/devel-api/adaptor-framework/vector-image-renderer.h>
#include <dali/public-api/object/base-object.h>
#include <dali/public-api/rendering/renderer.h>
#include <toolkit-application.h>
#include <toolkit-event-thread-callback.h>
#include <memory>
#include <sys/stat.h>

namespace Dali
{

namespace Internal
{

namespace Adaptor
{

class VectorImageRenderer: public Dali::BaseObject
{
public:

  VectorImageRenderer()
  : mUrl(),
    mRenderer(),
    mWidth( 0 ),
    mHeight( 0 )
  {
  }

  ~VectorImageRenderer()
  {
  }

  bool Render( float scale )
  {
     return true;
  }

  void SetBuffer( Dali::Devel::PixelBuffer &buffer )
  {
  }

  bool Load( const std::string& url )
  {
    struct stat sb;
    if ( stat(url.c_str(), &sb) == 0 )
    {
      return true;
    }

    return false;
  }

  bool Load( const char *data, uint32_t size )
  {
    return true;
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
};

inline VectorImageRenderer& GetImplementation( Dali::VectorImageRenderer& renderer )
{
  DALI_ASSERT_ALWAYS( renderer && "VectorImageRenderer handle is empty." );
  BaseObject& handle = renderer.GetBaseObject();
  return static_cast< Internal::Adaptor::VectorImageRenderer& >( handle );
}

inline const VectorImageRenderer& GetImplementation( const Dali::VectorImageRenderer& renderer )
{
  DALI_ASSERT_ALWAYS( renderer && "VectorImageRenderer handle is empty." );
  const BaseObject& handle = renderer.GetBaseObject();
  return static_cast< const Internal::Adaptor::VectorImageRenderer& >( handle );
}

} // namespace Adaptor

} // namespace Internal


/********************************************************************************/
/*********************************  PUBLIC CLASS  *******************************/
/********************************************************************************/

VectorImageRenderer VectorImageRenderer::New()
{
  Internal::Adaptor::VectorImageRenderer* imageRenderer = new Internal::Adaptor::VectorImageRenderer();

  return VectorImageRenderer( imageRenderer );
}

VectorImageRenderer::VectorImageRenderer()
{
}

VectorImageRenderer::~VectorImageRenderer()
{
}

VectorImageRenderer::VectorImageRenderer( Internal::Adaptor::VectorImageRenderer* internal )
: BaseHandle( internal )
{
}

void VectorImageRenderer::SetBuffer( Dali::Devel::PixelBuffer &buffer )
{
  Internal::Adaptor::GetImplementation( *this ).SetBuffer( buffer );
}

bool VectorImageRenderer::Render( float scale )
{
  return Internal::Adaptor::GetImplementation( *this ).Render( scale );
}

bool VectorImageRenderer::Load( const std::string& url )
{
  return Internal::Adaptor::GetImplementation( *this ).Load( url );
}

bool VectorImageRenderer::Load( const char *data, uint32_t size )
{
  return Internal::Adaptor::GetImplementation( *this ).Load( data, size );
}

void VectorImageRenderer::GetDefaultSize( uint32_t& width, uint32_t& height ) const
{
  Internal::Adaptor::GetImplementation( *this ).GetDefaultSize( width, height );
}

} // namespace Dali
