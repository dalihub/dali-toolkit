/*
 * Copyright (c) 2021 Samsung Electronics Co., Ltd.
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

#include <dali/devel-api/adaptor-framework/canvas-renderer.h>
#include <dali/devel-api/adaptor-framework/canvas-renderer-drawable.h>
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

class CanvasRenderer: public Dali::BaseObject
{
public:
  CanvasRenderer( const Vector2& size )
  : mPixelBuffer( Devel::PixelBuffer::New(size.width, size.height, Dali::Pixel::RGBA8888) ),
    mSize(size),
    mViewBox(size)
  {
  }

  ~CanvasRenderer()
  {
  }

  bool Commit()
  {
     return true;
  }

  bool IsCanvasChanged() const
  {
     return true;
  }

  bool Rasterize()
  {
     return true;
  }

  bool AddDrawable(Dali::CanvasRenderer::Drawable& drawable)
  {
    if (!drawable)
    {
      return false;
    }
    return true;
  }

  Devel::PixelBuffer GetPixelBuffer()
  {
    return mPixelBuffer;
  }


  bool SetSize(const Vector2& size)
  {
    mSize = size;
    // For negative test
    if ( size.width == -999 && size.height == -999 )
    {
      return false;
    }
    return true;
  }

  const Vector2& GetSize()
  {
    mSize = Vector2(200, 200);
    return mSize;
  }

  bool SetViewBox(const Vector2& viewBox)
  {
    mViewBox = viewBox;
    // For negative test
    if ( viewBox.width == -999 && viewBox.height == -999 )
    {
      return false;
    }
    return true;
  }

  const Vector2& GetViewBox()
  {
    return mViewBox;
  }

public:
   Devel::PixelBuffer mPixelBuffer;
   Vector2 mSize;
   Vector2 mViewBox;
};

inline CanvasRenderer& GetImplementation( Dali::CanvasRenderer& renderer )
{
  DALI_ASSERT_ALWAYS( renderer && "CanvasRenderer handle is empty." );
  BaseObject& handle = renderer.GetBaseObject();
  return static_cast< Internal::Adaptor::CanvasRenderer& >( handle );
}

inline const CanvasRenderer& GetImplementation( const Dali::CanvasRenderer& renderer )
{
  DALI_ASSERT_ALWAYS( renderer && "CanvasRenderer handle is empty." );
  const BaseObject& handle = renderer.GetBaseObject();
  return static_cast< const Internal::Adaptor::CanvasRenderer& >( handle );
}

} // namespace Adaptor

} // namespace Internal


/********************************************************************************/
/*********************************  PUBLIC CLASS  *******************************/
/********************************************************************************/

CanvasRenderer CanvasRenderer::New( const Vector2& size )
{
  Internal::Adaptor::CanvasRenderer* imageRenderer = new Internal::Adaptor::CanvasRenderer(size);

  return CanvasRenderer( imageRenderer );
}

CanvasRenderer::CanvasRenderer()
{
}

CanvasRenderer::~CanvasRenderer()
{
}

CanvasRenderer::CanvasRenderer( Internal::Adaptor::CanvasRenderer* internal )
: BaseHandle( internal )
{
}

bool CanvasRenderer::Commit()
{
  return Internal::Adaptor::GetImplementation(*this).Commit();
}

bool CanvasRenderer::IsCanvasChanged() const
{
  return Internal::Adaptor::GetImplementation(*this).IsCanvasChanged();
}

bool CanvasRenderer::Rasterize()
{
  return Internal::Adaptor::GetImplementation(*this).Rasterize();
}

Devel::PixelBuffer CanvasRenderer::GetPixelBuffer()
{
  return Internal::Adaptor::GetImplementation(*this).GetPixelBuffer();
}

bool CanvasRenderer::AddDrawable(Dali::CanvasRenderer::Drawable& drawable)
{
  return Internal::Adaptor::GetImplementation(*this).AddDrawable(drawable);
}

bool CanvasRenderer::SetSize(const Vector2& size)
{
  return Internal::Adaptor::GetImplementation(*this).SetSize(size);
}

const Vector2& CanvasRenderer::GetSize()
{
  return Internal::Adaptor::GetImplementation(*this).GetSize();
}

bool CanvasRenderer::SetViewBox(const Vector2& viewBox)
{
  return Internal::Adaptor::GetImplementation(*this).SetViewBox(viewBox);
}

const Vector2& CanvasRenderer::GetViewBox()
{
  return Internal::Adaptor::GetImplementation(*this).GetViewBox();
}

} // namespace Dali
