/*
 * Copyright (c) 2018 Samsung Electronics Co., Ltd.
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

  VectorAnimationRenderer( const std::string& url, Dali::Renderer renderer, uint32_t width, uint32_t height )
  : mUrl( url ),
    mRenderer( renderer ),
    mWidth( width ),
    mHeight( height )
  {
  }

  void SetSize( uint32_t width, uint32_t height )
  {
    mWidth = width;
    mHeight = height;
  }

  bool StartRender()
  {
    return true;
  }

  void StopRender()
  {
  }

  void Render( uint32_t frameNumber )
  {
  }

  uint32_t GetTotalFrameNumber()
  {
    return 5;
  }

public:

  std::string mUrl;
  Dali::Renderer mRenderer;
  uint32_t mWidth;
  uint32_t mHeight;

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

VectorAnimationRenderer VectorAnimationRenderer::New( const std::string& url, Renderer renderer, uint32_t width, uint32_t height )
{
  Internal::Adaptor::VectorAnimationRenderer* animationRenderer = new Internal::Adaptor::VectorAnimationRenderer( url, renderer, width, height );

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

void VectorAnimationRenderer::SetSize( uint32_t width, uint32_t height )
{
  Internal::Adaptor::GetImplementation( *this ).SetSize( width, height );
}

bool VectorAnimationRenderer::StartRender()
{
  return Internal::Adaptor::GetImplementation( *this ).StartRender();
}

void VectorAnimationRenderer::StopRender()
{
  Internal::Adaptor::GetImplementation( *this ).StopRender();
}

void VectorAnimationRenderer::Render( uint32_t frameNumber )
{
  Internal::Adaptor::GetImplementation( *this ).Render( frameNumber );
}

uint32_t VectorAnimationRenderer::GetTotalFrameNumber()
{
  return Internal::Adaptor::GetImplementation( *this ).GetTotalFrameNumber();
}

} // namespace Dali;

