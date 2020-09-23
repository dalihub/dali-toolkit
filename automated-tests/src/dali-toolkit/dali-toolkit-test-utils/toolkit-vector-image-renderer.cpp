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
#include <cstring>
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
  : mWidth(0),
    mHeight(0),
    mRasterizeSuccess(true)
  {
  }

  ~VectorImageRenderer()
  {
  }

  bool Load(const Vector<uint8_t>& data, float dpi)
  {
    if(strncmp(reinterpret_cast<char*>(data.Begin()), "invalid", 7) == 0)
    {
      return false;
    }
    else if(strncmp(reinterpret_cast<char*>(data.Begin()), "Fail to rasterize", strlen("Fail to rasterize")) == 0)
    {
      mRasterizeSuccess = false;
    }
    return true;
  }

  bool Rasterize(Dali::Devel::PixelBuffer& buffer, float scale)
  {
     return mRasterizeSuccess;
  }

  void GetDefaultSize(uint32_t& width, uint32_t& height) const
  {
    width = 100;
    height = 100;
  }

public:

  uint32_t mWidth;
  uint32_t mHeight;
  bool     mRasterizeSuccess;
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

bool VectorImageRenderer::Load(const Vector<uint8_t>& data, float dpi)
{
  return Internal::Adaptor::GetImplementation(*this).Load(data, dpi);
}

bool VectorImageRenderer::Rasterize(Dali::Devel::PixelBuffer& buffer, float scale)
{
  return Internal::Adaptor::GetImplementation(*this).Rasterize(buffer, scale);
}

void VectorImageRenderer::GetDefaultSize( uint32_t& width, uint32_t& height ) const
{
  Internal::Adaptor::GetImplementation( *this ).GetDefaultSize( width, height );
}

} // namespace Dali
