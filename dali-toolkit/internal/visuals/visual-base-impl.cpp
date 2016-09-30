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

// CLASS HEADER
#include "visual-base-impl.h"

// EXTERNAL HEADER
#include <dali/public-api/common/dali-common.h>
#include <dali/integration-api/debug.h>

//INTERNAL HEARDER
#include <dali-toolkit/public-api/visuals/visual-properties.h>
#include <dali-toolkit/internal/visuals/visual-base-data-impl.h>
#include <dali-toolkit/internal/visuals/visual-string-constants.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

Visual::Base::Base( VisualFactoryCache& factoryCache )
: mImpl( new Impl() ),
  mFactoryCache( factoryCache )
{
}

Visual::Base::~Base()
{
  delete mImpl;
}

void Visual::Base::SetCustomShader( const Property::Map& shaderMap )
{
  if( mImpl->mCustomShader )
  {
    mImpl->mCustomShader->SetPropertyMap( shaderMap );
  }
  else
  {
    mImpl->mCustomShader = new Impl::CustomShader( shaderMap );
  }
}

void Visual::Base::Initialize( Actor& actor, const Property::Map& propertyMap )
{
  Property::Value* customShaderValue = propertyMap.Find( Toolkit::Visual::Property::SHADER, CUSTOM_SHADER );
  if( customShaderValue )
  {
    Property::Map shaderMap;
    if( customShaderValue->Get( shaderMap ) )
    {
      SetCustomShader( shaderMap );
    }
  }

  DoInitialize( actor, propertyMap );
}

void Visual::Base::SetName( const std::string& name )
{
  mImpl->mName = name;
}

const std::string& Visual::Base::GetName()
{
  return mImpl->mName;
}

void Visual::Base::SetSize( const Vector2& size )
{
  mImpl->mSize = size;
}

const Vector2& Visual::Base::GetSize() const
{
  return mImpl->mSize;
}

void Visual::Base::GetNaturalSize( Vector2& naturalSize ) const
{
  naturalSize = Vector2::ZERO;
}

void Visual::Base::SetDepthIndex( float index )
{
  mImpl->mDepthIndex = index;
  if( mImpl->mRenderer )
  {
    mImpl->mRenderer.SetProperty( Renderer::Property::DEPTH_INDEX, mImpl->mDepthIndex );
  }
}

float Visual::Base::GetDepthIndex() const
{
  return mImpl->mDepthIndex;
}

void Visual::Base::SetOnStage( Actor& actor )
{
  // To display the actor correctly, renderer should not be added to actor until all required resources are ready.
  // Thus the calling of actor.AddRenderer() should happen inside derived class as base class does not know the exact timing.
  DoSetOnStage( actor );

  mImpl->mRenderer.SetProperty(Renderer::Property::BLEND_PRE_MULTIPLIED_ALPHA, IsPreMultipliedAlphaEnabled());
  mImpl->mRenderer.SetProperty( Renderer::Property::DEPTH_INDEX, mImpl->mDepthIndex );
  mImpl->mFlags |= Impl::IS_ON_STAGE;
}

void Visual::Base::SetOffStage( Actor& actor )
{
  if( GetIsOnStage() )
  {
    DoSetOffStage( actor );

    mImpl->mFlags &= ~Impl::IS_ON_STAGE;
  }
}

void Visual::Base::EnablePreMultipliedAlpha( bool preMultipled )
{
  if(preMultipled)
  {
    mImpl->mFlags |= Impl::IS_PREMULTIPLIED_ALPHA;
  }
  else
  {
    mImpl->mFlags &= ~Impl::IS_PREMULTIPLIED_ALPHA;
  }

  if( mImpl->mRenderer )
  {
    mImpl->mRenderer.SetProperty(Renderer::Property::BLEND_PRE_MULTIPLIED_ALPHA, preMultipled);
  }
}

bool Visual::Base::IsPreMultipliedAlphaEnabled() const
{
  return mImpl->mFlags & Impl::IS_PREMULTIPLIED_ALPHA;
}

void Visual::Base::DoSetOffStage( Actor& actor )
{
  actor.RemoveRenderer( mImpl->mRenderer );
  mImpl->mRenderer.Reset();
}

void Visual::Base::CreatePropertyMap( Property::Map& map ) const
{
  DoCreatePropertyMap( map );

  if( mImpl->mCustomShader )
  {
    mImpl->mCustomShader->CreatePropertyMap( map );
  }
}

bool Visual::Base::GetIsOnStage() const
{
  return mImpl->mFlags & Impl::IS_ON_STAGE;
}

bool Visual::Base::GetIsFromCache() const
{
  return mImpl->mFlags & Impl::IS_FROM_CACHE;
}

void Visual::Base::SetProperty( Dali::Property::Index index, const Dali::Property::Value& propertyValue )
{
  DALI_ASSERT_ALWAYS( ( index > Property::INVALID_INDEX ) &&
                      ( index > VISUAL_PROPERTY_BASE_START_INDEX ) && // Change the type of visual is not allowed.
                      "Property index is out of bounds" );

  if( index < VISUAL_PROPERTY_START_INDEX )
  {
    // TODO set the properties of the visual base.
  }
  else
  {
    DoSetProperty( index, propertyValue );
  }
}

Dali::Property::Value Visual::Base::GetProperty( Dali::Property::Index index )
{
  DALI_ASSERT_ALWAYS( ( index > Property::INVALID_INDEX ) &&
                      ( index >= VISUAL_PROPERTY_BASE_START_INDEX ) &&
                      "Property index is out of bounds" );

  Dali::Property::Value value;

  if( index < VISUAL_PROPERTY_START_INDEX )
  {
    // TODO retrieve the properties of the visual base.
  }
  else
  {
    value = DoGetProperty( index );
  }

  return value;
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
