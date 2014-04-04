//
// Copyright (c) 2014 Samsung Electronics Co., Ltd.
//
// Licensed under the Flora License, Version 1.0 (the License);
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://floralicense.org/license/
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an AS IS BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

// CLASS HEADER

#include "builder.h"

// EXTERNAL INCLUDES

// INTERNAL INCLUDES

#include <dali-toolkit/internal/builder/builder-impl.h>

namespace Dali
{

namespace Toolkit
{

Builder::Builder()
{
}

Builder::~Builder()
{
}

Builder Builder::New(void)
{
  return Builder(new Internal::Builder());
}

Builder::Builder(Internal::Builder *impl)
  : BaseHandle(impl)
{
}

void Builder::LoadFromString( const std::string &data, UIFormat rep )
{
  GetImpl(*this).LoadFromString( data );
}

void Builder::AddConstants( const PropertyValueMap& map )
{
  GetImpl(*this).AddConstants( map );
}

Animation Builder::CreateAnimation( const std::string& animationName )
{
  return GetImpl(*this).CreateAnimation( animationName );
}

Animation Builder::CreateAnimation( const std::string& animationName, const PropertyValueMap& map )
{
  return GetImpl(*this).CreateAnimation( animationName, map );
}

BaseHandle Builder::CreateFromStyle( const std::string& styleName )
{
  return GetImpl(*this).CreateFromStyle( styleName );
}

BaseHandle Builder::CreateFromStyle( const std::string& styleName, const PropertyValueMap& map )
{
  return GetImpl(*this).CreateFromStyle( styleName, map );
}

void Builder::ApplyStyle( const std::string& styleName, Handle& handle )
{
  GetImpl(*this).ApplyStyle( styleName, handle );
}

void Builder::AddActors( Actor toActor )
{
  GetImpl(*this).AddActors( toActor );
}

void Builder::AddActors( const std::string &sectionName, Actor toActor )
{
  GetImpl(*this).AddActors( sectionName, toActor );
}

Font Builder::GetFont( const std::string &name ) const
{
  return GetImpl(*this).GetFont( name );
}

TextStyle Builder::GetTextStyle( const std::string &name ) const
{
  return GetImpl(*this).GetTextStyle( name );
}

Image Builder::GetImage( const std::string &name ) const
{
  return GetImpl(*this).GetImage( name );
}

Actor Builder::GetActor( const std::string &name ) const
{
  return GetImpl(*this).GetActor( name );
}

Animation Builder::GetAnimation( const std::string &name ) const
{
  return GetImpl(*this).GetAnimation( name );
}

void Builder::CreateRenderTask( const std::string &name )
{
  GetImpl(*this).CreateRenderTask( name );
}

ShaderEffect Builder::GetShaderEffect( const std::string &name )
{
  return GetImpl(*this).GetShaderEffect( name );
}

FrameBufferImage Builder::GetFrameBufferImage( const std::string &name )
{
  return GetImpl(*this).GetFrameBufferImage( name );
}

ActorContainer Builder::GetTopLevelActors( void ) const
{
  return GetImpl(*this).GetTopLevelActors();
}

} // namespace Toolkit

} // namespace Dali

