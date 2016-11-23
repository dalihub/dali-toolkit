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
#include <dali-toolkit/devel-api/visual-factory/devel-visual-properties.h>
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

void Visual::Base::SetProperties( const Property::Map& propertyMap )
{
  Property::Value* customShaderValue = propertyMap.Find( VisualProperty::SHADER, CUSTOM_SHADER );
  if( customShaderValue )
  {
    Property::Map shaderMap;
    if( customShaderValue->Get( shaderMap ) )
    {
      SetCustomShader( shaderMap );
    }
  }

  Property::Value* transform = propertyMap.Find( Toolkit::Visual::DevelProperty::TRANSFORM, TRANSFORM );
  if( transform )
  {
    Property::Map map;
    if( transform->Get( map ) )
    {
      mImpl->mTransform.SetPropertyMap( map );
    }
  }

  DoSetProperties( propertyMap );
}

void Visual::Base::SetTransformAndSize( const Property::Map& transform, Size controlSize )
{
  mImpl->mControlSize = controlSize;
  mImpl->mTransform.SetPropertyMap( transform );
  OnSetTransform();
}

void Visual::Base::SetName( const std::string& name )
{
  mImpl->mName = name;
}

const std::string& Visual::Base::GetName()
{
  return mImpl->mName;
}

float Visual::Base::GetHeightForWidth( float width ) const
{
  return 0.f;
}

void Visual::Base::GetNaturalSize( Vector2& naturalSize )
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
  if( !IsOnStage() )
  {
    // To display the actor correctly, renderer should not be added to actor until all required resources are ready.
    // Thus the calling of actor.AddRenderer() should happen inside derived class as base class does not know the exact timing.
    DoSetOnStage( actor );

    if( mImpl->mRenderer )
    {
      mImpl->mRenderer.SetProperty( Renderer::Property::BLEND_PRE_MULTIPLIED_ALPHA, IsPreMultipliedAlphaEnabled());
      mImpl->mRenderer.SetProperty( Renderer::Property::DEPTH_INDEX, mImpl->mDepthIndex );
      mImpl->mFlags |= Impl::IS_ON_STAGE; // Only sets the flag if renderer exists
    }
  }
}

void Visual::Base::SetOffStage( Actor& actor )
{
  if( IsOnStage() )
  {
    DoSetOffStage( actor );

    mImpl->mFlags &= ~Impl::IS_ON_STAGE;
  }
}

void Visual::Base::CreatePropertyMap( Property::Map& map ) const
{
  DoCreatePropertyMap( map );

  if( mImpl->mCustomShader )
  {
    mImpl->mCustomShader->CreatePropertyMap( map );
  }

  Property::Map transform;
  mImpl->mTransform.GetPropertyMap( transform );
  map.Insert( Toolkit::Visual::DevelProperty::TRANSFORM, transform );
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

bool Visual::Base::IsOnStage() const
{
  return mImpl->mFlags & Impl::IS_ON_STAGE;
}

bool Visual::Base::IsFromCache() const
{
  return mImpl->mFlags & Impl::IS_FROM_CACHE;
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
