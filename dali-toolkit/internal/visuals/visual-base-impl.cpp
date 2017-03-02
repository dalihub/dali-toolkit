/*
 * Copyright (c) 2017 Samsung Electronics Co., Ltd.
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
#include <dali/devel-api/object/handle-devel.h>
#include <dali/integration-api/debug.h>

//INTERNAL HEARDER
#include <dali-toolkit/public-api/visuals/color-visual-properties.h>
#include <dali-toolkit/public-api/visuals/primitive-visual-properties.h>
#include <dali-toolkit/devel-api/visuals/visual-properties-devel.h>
#include <dali-toolkit/internal/visuals/visual-base-data-impl.h>
#include <dali-toolkit/internal/visuals/visual-string-constants.h>

namespace
{
#if defined(DEBUG_ENABLED)
Debug::Filter* gVisualBaseLogFilter = Debug::Filter::New( Debug::NoLogging, false, "LOG_VISUAL_BASE" );
#endif
}

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
  for( size_t i = 0; i < propertyMap.Count(); ++i )
  {
    const KeyValuePair& pair = propertyMap.GetKeyValue( i );
    const Property::Key& key = pair.first;
    const Property::Value& value = pair.second;

    Property::Key matchKey = key;
    if( matchKey.type == Property::Key::STRING )
    {
      if( matchKey == CUSTOM_SHADER )
      {
        matchKey = Property::Key( DevelVisual::Property::SHADER );
      }
      else if( matchKey == TRANSFORM )
      {
        matchKey = Property::Key( DevelVisual::Property::TRANSFORM );
      }
      else if( matchKey == PREMULTIPLIED_ALPHA )
      {
        matchKey = Property::Key( DevelVisual::Property::PREMULTIPLIED_ALPHA );
      }
      else if( matchKey == MIX_COLOR )
      {
        matchKey = Property::Key( DevelVisual::Property::MIX_COLOR );
      }
      else if( matchKey == OPACITY )
      {
        matchKey = Property::Key( DevelVisual::Property::OPACITY );
      }
    }

    switch( matchKey.indexKey )
    {
      case DevelVisual::Property::SHADER:
      {
        Property::Map shaderMap;
        if( value.Get( shaderMap ) )
        {
          SetCustomShader( shaderMap );
        }
        break;
      }

      case DevelVisual::Property::TRANSFORM:
      {
        Property::Map map;
        if( value.Get( map ) )
        {
          mImpl->mTransform.SetPropertyMap( map );
        }
        break;
      }

      case DevelVisual::Property::PREMULTIPLIED_ALPHA:
      {
        bool premultipliedAlpha = false;
        if( value.Get( premultipliedAlpha ) )
        {
          EnablePreMultipliedAlpha( premultipliedAlpha );
        }
        break;
      }

      case DevelVisual::Property::MIX_COLOR:
      {
        Vector4 mixColor;
        if( value.Get( mixColor ) )
        {
          if( value.GetType() == Property::VECTOR4 )
          {
            SetMixColor( mixColor );
          }
          else
          {
            Vector3 mixColor3(mixColor);
            SetMixColor( mixColor3 );
          }
        }
        break;
      }
      case DevelVisual::Property::OPACITY:
      {
        float opacity;
        if( value.Get( opacity ) )
        {
          mImpl->mMixColor.a = opacity;
          SetMixColor( mImpl->mMixColor );
        }
        break;
      }
    }
  }

  DoSetProperties( propertyMap );
}

void Visual::Base::SetTransformAndSize( const Property::Map& transform, Size controlSize )
{
  mImpl->mControlSize = controlSize;
  mImpl->mTransform.UpdatePropertyMap( transform );

#if defined(DEBUG_ENABLED)
  std::ostringstream oss;
  oss << transform;
  DALI_LOG_INFO( gVisualBaseLogFilter, Debug::General, "Visual::Base::SetTransformAndSize(%s) - [\e[1;32mtransform: %s  controlSize: (%3.1f, %3.1f)]\e[0m\n",
                 GetName().c_str(), oss.str().c_str(), controlSize.x, controlSize.y );
#endif

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

float Visual::Base::GetHeightForWidth( float width )
{
  float aspectCorrectedHeight = 0.f;
  Vector2 naturalSize;
  GetNaturalSize( naturalSize );
  if( naturalSize.width )
  {
    aspectCorrectedHeight = naturalSize.height * width / naturalSize.width;
  }
  return aspectCorrectedHeight;
}

float Visual::Base::GetWidthForHeight( float height )
{
  float aspectCorrectedWidth = 0.f;
  Vector2 naturalSize;
  GetNaturalSize( naturalSize );
  if( naturalSize.height > 0.0f )
  {
    aspectCorrectedWidth = naturalSize.width * height / naturalSize.height;
  }
  return aspectCorrectedWidth;
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
      RegisterMixColor();

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
    mImpl->mMixColorIndex = Property::INVALID_INDEX;
    mImpl->mOpacityIndex = Property::INVALID_INDEX;
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
  map.Insert( DevelVisual::Property::TRANSFORM, transform );

  bool premultipliedAlpha( IsPreMultipliedAlphaEnabled() );
  map.Insert( DevelVisual::Property::PREMULTIPLIED_ALPHA, premultipliedAlpha );

  // Note, Color and Primitive will also insert their own mix color into the map
  // which is ok, because they have a different key value range.
  map.Insert( DevelVisual::Property::MIX_COLOR, mImpl->mMixColor ); // vec4
  map.Insert( DevelVisual::Property::OPACITY, mImpl->mMixColor.a );
}

void Visual::Base::CreateInstancePropertyMap( Property::Map& map ) const
{
  DoCreateInstancePropertyMap( map );

  if( mImpl->mCustomShader )
  {
    mImpl->mCustomShader->CreatePropertyMap( map );
  }

  //map.Insert( DevelVisual::Property::DEPTH_INDEX, mImpl->mDepthIndex );
  //map.Insert( DevelVisual::Property::ENABLED, (bool) mImpl->mRenderer );
}


void Visual::Base::EnablePreMultipliedAlpha( bool preMultipled )
{
  if( preMultipled )
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

void Visual::Base::RegisterMixColor()
{
  // Only register if not already registered.
  // (Color and Primitive visuals will register their own and save to this index)
  if( mImpl->mMixColorIndex == Property::INVALID_INDEX )
  {
    mImpl->mMixColorIndex = DevelHandle::RegisterProperty(
      mImpl->mRenderer,
      Toolkit::DevelVisual::Property::MIX_COLOR,
      MIX_COLOR,
      Vector3(mImpl->mMixColor) );
  }

  if( mImpl->mMixColor.a < 1.f )
  {
    mImpl->mRenderer.SetProperty( Renderer::Property::BLEND_MODE, BlendMode::ON );
  }

  if( mImpl->mOpacityIndex == Property::INVALID_INDEX )
  {
    mImpl->mOpacityIndex = DevelHandle::RegisterProperty(
      mImpl->mRenderer,
      Toolkit::DevelVisual::Property::OPACITY,
      OPACITY,
      mImpl->mMixColor.a );
  }

  float preMultipliedAlpha = 0.0f;
  if( IsPreMultipliedAlphaEnabled() )
  {
    preMultipliedAlpha = 1.0f;
  }
  mImpl->mRenderer.RegisterProperty( "preMultipliedAlpha", preMultipliedAlpha );
}

void Visual::Base::SetMixColor( const Vector4& color )
{
  mImpl->mMixColor = color;

  if( mImpl->mRenderer )
  {
    mImpl->mRenderer.SetProperty( mImpl->mMixColorIndex, Vector3(color) );
    mImpl->mRenderer.SetProperty( mImpl->mOpacityIndex, color.a );
    if( color.a < 1.f )
    {
      mImpl->mRenderer.SetProperty( Renderer::Property::BLEND_MODE, BlendMode::ON );
    }
  }
}

void Visual::Base::SetMixColor( const Vector3& color )
{
  mImpl->mMixColor.r = color.r;
  mImpl->mMixColor.g = color.g;
  mImpl->mMixColor.b = color.b;

  if( mImpl->mRenderer )
  {
    mImpl->mRenderer.SetProperty( mImpl->mMixColorIndex, color );
  }
}

const Vector4& Visual::Base::GetMixColor() const
{
  return mImpl->mMixColor;
}

Renderer Visual::Base::GetRenderer()
{
  return mImpl->mRenderer;
}

Property::Index Visual::Base::GetPropertyIndex( Property::Key key )
{
  Property::Index index = DevelHandle::GetPropertyIndex( mImpl->mRenderer, key );

  if( index == Property::INVALID_INDEX )
  {
    // Is it a shader property?
    Shader shader = mImpl->mRenderer.GetShader();
    index = DevelHandle::GetPropertyIndex( shader, key );
    if( index != Property::INVALID_INDEX )
    {
      // Yes - we should register it in the Renderer so it can be set / animated
      // independently, as shaders are shared across multiple renderers.
      std::string keyName;
      Property::Index keyIndex( Property::INVALID_KEY );
      if( key.type == Property::Key::INDEX )
      {
        keyName = shader.GetPropertyName( index );
        keyIndex = key.indexKey;
      }
      else
      {
        keyName = key.stringKey;
        // Leave keyIndex as INVALID_KEY - it can still be registered against the string key.
      }
      Property::Value value = shader.GetProperty( index );
      index = DevelHandle::RegisterProperty( mImpl->mRenderer, keyIndex, keyName, value );
    }
  }
  return index;
}

void Visual::Base::SetupTransition(
  Dali::Animation& transition,
  Internal::TransitionData::Animator& animator,
  Property::Index index,
  Property::Value& initialValue,
  Property::Value& targetValue )
{
  if( index != Property::INVALID_INDEX )
  {
    if( mImpl->mRenderer )
    {
      if( animator.animate == false )
      {
        mImpl->mRenderer.SetProperty( index, targetValue );
      }
      else
      {
        if( animator.initialValue.GetType() != Property::NONE )
        {
          mImpl->mRenderer.SetProperty( index, initialValue );
        }

        if( ! transition )
        {
          transition = Dali::Animation::New( 0.1f );
        }

        transition.AnimateTo( Property( mImpl->mRenderer, index ),
                              targetValue,
                              animator.alphaFunction,
                              TimePeriod( animator.timePeriodDelay,
                                          animator.timePeriodDuration ) );
      }
    }
  }
}

void Visual::Base::AnimateProperty(
  Dali::Animation& transition,
  Internal::TransitionData::Animator& animator )
{
#if defined(DEBUG_ENABLED)
  {
    std::ostringstream oss;
    oss << "Visual::Base::AnimateProperty(Visual:" << mImpl->mName << " Property:" << animator.propertyKey << " Target: " << animator.targetValue << std::endl;
    DALI_LOG_INFO( gVisualBaseLogFilter, Debug::General, oss.str().c_str() );
  }
#endif

  Property::Map map;
  DoCreatePropertyMap( map );
  Property::Value* valuePtr = map.Find( Toolkit::DevelVisual::Property::TYPE );
  int visualType;
  valuePtr->Get(visualType);

  if( animator.propertyKey == Toolkit::DevelVisual::Property::MIX_COLOR ||
      animator.propertyKey == MIX_COLOR ||
      ( visualType == Toolkit::Visual::COLOR &&
        animator.propertyKey == ColorVisual::Property::MIX_COLOR ) ||
      ( visualType == Toolkit::Visual::PRIMITIVE &&
        animator.propertyKey == PrimitiveVisual::Property::MIX_COLOR ) )
  {
    AnimateMixColorProperty( transition, animator );
  }
  else if(animator.propertyKey == Toolkit::DevelVisual::Property::OPACITY ||
          animator.propertyKey == OPACITY )
  {
    AnimateOpacityProperty( transition, animator );
  }
  else if( mImpl->mRenderer )
  {
    AnimateRendererProperty(transition, animator);
  }
}

void Visual::Base::AnimateOpacityProperty(
  Dali::Animation& transition,
  Internal::TransitionData::Animator& animator )
{
  Property::Index index = mImpl->mOpacityIndex;

  bool isOpaque = mImpl->mMixColor.a >= 1.0f;

  if( index != Property::INVALID_INDEX )
  {
    float initialOpacity;
    if( animator.initialValue.Get( initialOpacity ) )
    {
      isOpaque = (initialOpacity >= 1.0f);
    }

    float targetOpacity;
    if( animator.targetValue.Get( targetOpacity ) )
    {
      mImpl->mMixColor.a = targetOpacity;
    }

    SetupTransition( transition, animator, index, animator.initialValue, animator.targetValue );
    SetupBlendMode( transition, isOpaque, animator.animate );
  }
}

void Visual::Base::AnimateRendererProperty(
  Dali::Animation& transition,
  Internal::TransitionData::Animator& animator )
{
  Property::Index index = GetPropertyIndex( animator.propertyKey );
  if( index != Property::INVALID_INDEX )
  {
    if( animator.targetValue.GetType() != Property::NONE )
    {
      // Try writing target value into transform property map
      // if it's not a valid key, then it won't alter mTransform
      Property::Map map;
      if( animator.propertyKey.type == Property::Key::INDEX )
      {
        map.Add( animator.propertyKey.indexKey, animator.targetValue );
      }
      else
      {
        map.Add( animator.propertyKey.stringKey, animator.targetValue );
      }

      mImpl->mTransform.UpdatePropertyMap( map );
    }

    SetupTransition( transition, animator, index, animator.initialValue, animator.targetValue );
  }
}

void Visual::Base::AnimateMixColorProperty(
  Dali::Animation& transition,
  Internal::TransitionData::Animator& animator )
{
  Property::Index index = mImpl->mMixColorIndex;
  bool animateOpacity = false;
  bool isOpaque = true;

  Property::Value initialOpacity;
  Property::Value targetOpacity;
  Property::Value initialMixColor;
  Property::Value targetMixColor;

  if( index != Property::INVALID_INDEX )
  {
    Vector4 initialColor;
    if( animator.initialValue.Get(initialColor) )
    {
      if( animator.initialValue.GetType() == Property::VECTOR4 )
      {
        // if there is an initial color specifying alpha, test it
        isOpaque = initialColor.a >= 1.0f;
        initialOpacity = initialColor.a;
      }
      initialMixColor = Vector3( initialColor );
    }

    // Set target value into data store
    if( animator.targetValue.GetType() != Property::NONE )
    {
      Vector4 mixColor;
      animator.targetValue.Get(mixColor);
      if( animator.targetValue.GetType() == Property::VECTOR4 )
      {
        mImpl->mMixColor.a = mixColor.a;
        targetOpacity = mixColor.a;
        animateOpacity = true;
      }

      mImpl->mMixColor.r = mixColor.r;
      mImpl->mMixColor.g = mixColor.g;
      mImpl->mMixColor.b = mixColor.b;
      targetMixColor = Vector3(mixColor);
    }

    SetupTransition( transition, animator, index, initialMixColor, targetMixColor );
    if( animateOpacity )
    {
      SetupTransition( transition, animator, mImpl->mOpacityIndex, initialOpacity, targetOpacity );
      SetupBlendMode( transition, isOpaque, animator.animate );
    }
  }
}

void Visual::Base::SetupBlendMode( Animation& transition, bool isInitialOpaque, bool animating )
{
  // Ensure the blend mode is turned on if we are animating opacity, and
  // turned off after the animation ends if the final value is opaque
  if( ! isInitialOpaque || mImpl->mMixColor.a < 1.0f )
  {
    mImpl->mRenderer.SetProperty( Renderer::Property::BLEND_MODE, BlendMode::ON );

    if( animating == true && mImpl->mMixColor.a >= 1.0f )
    {
      // When it becomes opaque, set the blend mode back to automatically
      if( ! mImpl->mBlendSlotDelegate )
      {
        mImpl->mBlendSlotDelegate = new SlotDelegate<Visual::Base>(this);
      }
      transition.FinishedSignal().Connect( *(mImpl->mBlendSlotDelegate),
                                           &Visual::Base::OnMixColorFinished );
    }
  }
}

void Visual::Base::OnMixColorFinished( Animation& animation )
{
  if( mImpl->mRenderer )
  {
    DALI_LOG_INFO( gVisualBaseLogFilter, Debug::General, "Visual::Base::OnMixColorFinished()\n");
    mImpl->mRenderer.SetProperty( Renderer::Property::BLEND_MODE,
                                  ( mImpl->mMixColor.a < 1.0 ) ? BlendMode::ON : BlendMode::AUTO );
  }
  delete mImpl->mBlendSlotDelegate;
  mImpl->mBlendSlotDelegate = NULL;
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
