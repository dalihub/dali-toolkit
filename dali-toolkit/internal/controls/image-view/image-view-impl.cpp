/*
 * Copyright (c) 2023 Samsung Electronics Co., Ltd.
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
#include "image-view-impl.h"

// EXTERNAL INCLUDES
#include <dali/devel-api/scripting/scripting.h>
#include <dali/public-api/math/math-utils.h>
#include <dali/public-api/object/type-registry-helper.h>
#include <dali/public-api/object/type-registry.h>

// INTERNAL INCLUDES
#include <dali-toolkit/devel-api/controls/control-devel.h>
#include <dali-toolkit/devel-api/visual-factory/visual-factory.h>
#include <dali-toolkit/devel-api/visuals/visual-actions-devel.h>
#include <dali-toolkit/internal/controls/control/control-data-impl.h>
#include <dali-toolkit/internal/visuals/visual-base-data-impl.h>
#include <dali-toolkit/internal/visuals/visual-base-impl.h>
#include <dali-toolkit/internal/visuals/visual-string-constants.h>
#include <dali-toolkit/public-api/controls/image-view/image-view.h>
#include <dali-toolkit/public-api/visuals/visual-properties.h>

namespace Dali
{
namespace Toolkit
{
namespace Internal
{
namespace
{
const Vector4 FULL_TEXTURE_RECT(0.f, 0.f, 1.f, 1.f);

constexpr float FULL_OPACITY = 1.0f;
constexpr float LOW_OPACITY  = 0.2f;
constexpr float TRANSITION_EFFECT_SPEED = 0.3f;

BaseHandle Create()
{
  return Toolkit::ImageView::New();
}

// Setup properties, signals and actions using the type-registry.
DALI_TYPE_REGISTRATION_BEGIN(Toolkit::ImageView, Toolkit::Control, Create);
DALI_PROPERTY_REGISTRATION(Toolkit, ImageView, "image", MAP, IMAGE)
DALI_PROPERTY_REGISTRATION(Toolkit, ImageView, "preMultipliedAlpha", BOOLEAN, PRE_MULTIPLIED_ALPHA)
DALI_PROPERTY_REGISTRATION(Toolkit, ImageView, "placeholderImage", STRING, PLACEHOLDER_IMAGE)
DALI_PROPERTY_REGISTRATION(Toolkit, ImageView, "enableTransitionEffect", BOOLEAN, ENABLE_TRANSITION_EFFECT)
DALI_ANIMATABLE_PROPERTY_REGISTRATION_WITH_DEFAULT(Toolkit, ImageView, "pixelArea", Vector4(0.f, 0.f, 1.f, 1.f), PIXEL_AREA)
DALI_TYPE_REGISTRATION_END()

} // anonymous namespace

using namespace Dali;

ImageView::ImageView(ControlBehaviour additionalBehaviour)
: Control(ControlBehaviour(CONTROL_BEHAVIOUR_DEFAULT | additionalBehaviour)),
  mImageSize(),
  mTransitionTargetAlpha(FULL_OPACITY),
  mImageVisualPaddingSetByTransform(false),
  mImageViewPixelAreaSetByFittingMode(false),
  mTransitionEffect(false),
  mNeedLazyFittingMode(false)
{
}

ImageView::~ImageView()
{
}

Toolkit::ImageView ImageView::New(ControlBehaviour additionalBehaviour)
{
  ImageView* impl = new ImageView(additionalBehaviour);

  Toolkit::ImageView handle = Toolkit::ImageView(*impl);

  // Second-phase init of the implementation
  // This can only be done after the CustomActor connection has been made...
  impl->Initialize();

  return handle;
}

/////////////////////////////////////////////////////////////

void ImageView::OnInitialize()
{
  // ImageView can relayout in the OnImageReady, alternative to a signal would be to have a upcall from the Control to ImageView
  Dali::Toolkit::Control handle(GetOwner());
  handle.ResourceReadySignal().Connect(this, &ImageView::OnResourceReady);

  Self().SetProperty(DevelControl::Property::ACCESSIBILITY_ROLE, Dali::Accessibility::Role::IMAGE);
}

void ImageView::SetImage(const Property::Map& map)
{
  if(mTransitionEffect && mVisual)
  {
    // Clear previous transition effect if it is playing
    if(mPreviousVisual)
    {
      if(mTransitionAnimation)
      {
        if(mTransitionAnimation.GetState() == Animation::PLAYING)
        {
          mTransitionAnimation.Stop();
          ClearTransitionAnimation();
        }
      }
    }

    // Enable transition effect for previous visual.
    // This previous visual will be deleted when transition effect is done.
    Internal::Control::Impl& controlDataImpl = Internal::Control::Impl::Get(*this);
    controlDataImpl.EnableReadyTransitionOverriden(mVisual, true);
    mPreviousVisual = mVisual;
  }

  // Comparing a property map is too expensive so just creating a new visual
  mPropertyMap = map;
  mUrl.clear();

  // keep alpha for transition effect
  if(mTransitionEffect)
  {
    float            alpha      = FULL_OPACITY;
    Property::Value* alphaValue = map.Find(Toolkit::Visual::Property::OPACITY);
    if(alphaValue && alphaValue->Get(alpha))
    {
      mTransitionTargetAlpha = alpha;
    }
  }

  if(!mVisual)
  {
    ShowPlaceholderImage();
  }

  Toolkit::Visual::Base visual = Toolkit::VisualFactory::Get().CreateVisual(mPropertyMap);
  if(visual)
  {
    // Don't set mVisual until it is ready and shown. Getters will still use current visual.
    if(!mVisual)
    {
      mVisual = visual;
    }

    if(!mShaderMap.Empty())
    {
      Internal::Visual::Base& visualImpl = Toolkit::GetImplementation(visual);
      visualImpl.SetCustomShader(mShaderMap);
    }

    DevelControl::RegisterVisual(*this, Toolkit::ImageView::Property::IMAGE, visual);
  }
  else
  {
    // Unregister the exsiting visual
    DevelControl::UnregisterVisual(*this, Toolkit::ImageView::Property::IMAGE);

    // Trigger a size negotiation request that may be needed when unregistering a visual.
    RelayoutRequest();
  }

  // Signal that a Relayout may be needed
}

void ImageView::SetImage(const std::string& url, ImageDimensions size)
{
  if(mTransitionEffect && mVisual)
  {
    // Clear previous transition effect if it is playing
    if(mPreviousVisual)
    {
      if(mTransitionAnimation)
      {
        if(mTransitionAnimation.GetState() == Animation::PLAYING)
        {
          mTransitionAnimation.Stop();
          ClearTransitionAnimation();
        }
      }
    }

    // Enable transition effect for previous visual.
    // This previous visual will be deleted when transition effect is done.
    Internal::Control::Impl& controlDataImpl = Internal::Control::Impl::Get(*this);
    controlDataImpl.EnableReadyTransitionOverriden(mVisual, true);
    mPreviousVisual = mVisual;
  }

  // Don't bother comparing if we had a visual previously, just drop old visual and create new one
  mUrl       = url;
  mImageSize = size;
  mPropertyMap.Clear();

  if(!mVisual)
  {
    ShowPlaceholderImage();
  }

  // Don't set mVisual until it is ready and shown. Getters will still use current visual.
  Toolkit::Visual::Base visual = Toolkit::VisualFactory::Get().CreateVisual(url, size);
  if(visual)
  {
    if(!mVisual)
    {
      mVisual = visual;
    }

    if(!mShaderMap.Empty())
    {
      Internal::Visual::Base& visualImpl = Toolkit::GetImplementation(visual);
      visualImpl.SetCustomShader(mShaderMap);
    }

    DevelControl::RegisterVisual(*this, Toolkit::ImageView::Property::IMAGE, visual);
  }
  else
  {
    // Unregister the exsiting visual
    DevelControl::UnregisterVisual(*this, Toolkit::ImageView::Property::IMAGE);

    // Trigger a size negotiation request that may be needed when unregistering a visual.
    RelayoutRequest();
  }

  // Signal that a Relayout may be needed
}

void ImageView::ClearImageVisual()
{
  // Clear cached properties
  mPropertyMap.Clear();
  mUrl.clear();
  mVisual.Reset();

  // Unregister the exsiting visual
  DevelControl::UnregisterVisual(*this, Toolkit::ImageView::Property::IMAGE);

  // Trigger a size negotiation request that may be needed when unregistering a visual.
  RelayoutRequest();
}

void ImageView::EnablePreMultipliedAlpha(bool preMultipled)
{
  if(mVisual)
  {
    Toolkit::GetImplementation(mVisual).EnablePreMultipliedAlpha(preMultipled);
  }
}

bool ImageView::IsPreMultipliedAlphaEnabled() const
{
  if(mVisual)
  {
    return Toolkit::GetImplementation(mVisual).IsPreMultipliedAlphaEnabled();
  }
  return false;
}

void ImageView::SetDepthIndex(int depthIndex)
{
  if(mVisual)
  {
    mVisual.SetDepthIndex(depthIndex);
  }
}

void ImageView::SetPlaceholderUrl(const std::string& url)
{
  mPlaceholderUrl = url;
  if(!url.empty())
  {
    mPlaceholderVisual.Reset();
    CreatePlaceholderImage();
  }
  else
  {
    // Clear current placeholder image
    Toolkit::Visual::Base visual = DevelControl::GetVisual(*this, Toolkit::ImageView::Property::PLACEHOLDER_IMAGE);
    if(visual)
    {
      DevelControl::UnregisterVisual(*this, Toolkit::ImageView::Property::PLACEHOLDER_IMAGE);
    }

    mPlaceholderVisual.Reset();
    mPlaceholderUrl = url;
  }
}

std::string ImageView::GetPlaceholderUrl() const
{
  return mPlaceholderUrl;
}

void ImageView::EnableTransitionEffect(bool effectEnable)
{
  mTransitionEffect = effectEnable;
}

bool ImageView::IsTransitionEffectEnabled() const
{
  return mTransitionEffect;
}

Vector3 ImageView::GetNaturalSize()
{
  if(mVisual)
  {
    Vector2 rendererNaturalSize;
    mVisual.GetNaturalSize(rendererNaturalSize);

    Extents padding;
    padding = Self().GetProperty<Extents>(Toolkit::Control::Property::PADDING);

    rendererNaturalSize.width += (padding.start + padding.end);
    rendererNaturalSize.height += (padding.top + padding.bottom);
    return Vector3(rendererNaturalSize);
  }

  // if no visual then use Control's natural size
  return Control::GetNaturalSize();
}

float ImageView::GetHeightForWidth(float width)
{
  Extents padding;
  padding = Self().GetProperty<Extents>(Toolkit::Control::Property::PADDING);

  if(mVisual)
  {
    return mVisual.GetHeightForWidth(width) + padding.top + padding.bottom;
  }
  else
  {
    return Control::GetHeightForWidth(width) + padding.top + padding.bottom;
  }
}

float ImageView::GetWidthForHeight(float height)
{
  Extents padding;
  padding = Self().GetProperty<Extents>(Toolkit::Control::Property::PADDING);

  if(mVisual)
  {
    return mVisual.GetWidthForHeight(height) + padding.start + padding.end;
  }
  else
  {
    return Control::GetWidthForHeight(height) + padding.start + padding.end;
  }
}

void ImageView::OnRelayout(const Vector2& size, RelayoutContainer& container)
{
  Control::OnRelayout(size, container);
  if(mVisual)
  {
    // If Resource is not ready, fittingMode is not working well.
    // in this case, imageview set the flag for working applyFittingMode again when the resource is ready
    if(!IsResourceReady())
    {
      mNeedLazyFittingMode = true;
    }

    // Apply FittingMode using actor's size
    ApplyFittingMode(size);

    // mVisual is not updated util the resource is ready in the case of visual replacement.
    // in this case, the Property Map must be initialized so that the previous value is not reused.
    // after mVisual is updated, the correct value will be reset.
    Toolkit::Visual::Base visual = DevelControl::GetVisual(*this, Toolkit::ImageView::Property::IMAGE);
    if(visual && visual != mVisual)
    {
      visual.SetTransformAndSize(Property::Map(), size);
    }
  }
}

void ImageView::OnCreateTransitions(std::vector<std::pair<Dali::Property::Index, Dali::Property::Map>>& sourceProperties,
                                    std::vector<std::pair<Dali::Property::Index, Dali::Property::Map>>& destinationProperties,
                                    Dali::Toolkit::Control                                              source,
                                    Dali::Toolkit::Control                                              destination)
{
  // Retrieves image properties to be transitioned.
  Dali::Property::Map imageSourcePropertyMap, imageDestinationPropertyMap;
  MakeVisualTransition(imageSourcePropertyMap, imageDestinationPropertyMap, source, destination, Toolkit::ImageView::Property::IMAGE);
  if(imageSourcePropertyMap.Count() > 0)
  {
    sourceProperties.push_back(std::pair<Dali::Property::Index, Dali::Property::Map>(Toolkit::ImageView::Property::IMAGE, imageSourcePropertyMap));
    destinationProperties.push_back(std::pair<Dali::Property::Index, Dali::Property::Map>(Toolkit::ImageView::Property::IMAGE, imageDestinationPropertyMap));
  }
}

void ImageView::OnUpdateVisualProperties(const std::vector<std::pair<Dali::Property::Index, Dali::Property::Map>>& properties)
{
  Toolkit::Visual::Base visual = DevelControl::GetVisual(*this, Toolkit::ImageView::Property::IMAGE);
  if(visual)
  {
    Dali::Toolkit::Control handle(GetOwner());

    for(auto&& data : properties)
    {
      if(data.first == Toolkit::ImageView::Property::IMAGE)
      {
        DevelControl::DoAction(handle, Toolkit::ImageView::Property::IMAGE, DevelVisual::Action::UPDATE_PROPERTY, data.second);
        break;
      }
    }
  }
}

void ImageView::OnResourceReady(Toolkit::Control control)
{
  // In case of placeholder, we need to skip this call.
  // TODO: In case of placeholder, it needs to be modified not to call OnResourceReady()
  if(control.GetVisualResourceStatus(Toolkit::ImageView::Property::IMAGE) != Toolkit::Visual::ResourceStatus::READY)
  {
    return;
  }

  // Do transition effect if need.
  if(mTransitionEffect)
  {
    // TODO: Consider about placeholder image is loaded failed
    Toolkit::Visual::Base placeholderVisual = DevelControl::GetVisual(*this, Toolkit::ImageView::Property::PLACEHOLDER_IMAGE);
    if(!placeholderVisual || control.GetVisualResourceStatus(Toolkit::ImageView::Property::PLACEHOLDER_IMAGE) == Toolkit::Visual::ResourceStatus::READY)
    {
      // when placeholder is disabled or ready placeholder and image, we need to transition effect
      TransitionImageWithEffect();
    }
    else
    {
      ClearTransitionAnimation();
    }
  }
  else
  {
    // we don't need placeholder anymore because visual is replaced. so hide placeholder.
    HidePlaceholderImage();
  }

  // Visual ready so update visual attached to this ImageView, following call to RelayoutRequest will use this visual.
  mVisual = DevelControl::GetVisual(*this, Toolkit::ImageView::Property::IMAGE);

  // Applying FittingMode again if it is not working well on OnRelayout().
  if(mNeedLazyFittingMode)
  {
    const Vector2& size = Self().GetProperty(Dali::Actor::Property::SIZE).Get<Vector2>();
    ApplyFittingMode(size);
    mNeedLazyFittingMode = false;
  }

  // Signal that a Relayout may be needed
}

void ImageView::SetTransformMapForFittingMode(Vector2 finalSize, Vector2 naturalSize, Vector2 finalOffset, Visual::FittingMode fittingMode, Property::Map& transformMap)
{
  switch(fittingMode)
  {
    case Visual::FittingMode::FIT_KEEP_ASPECT_RATIO:
    {
      auto availableVisualSize = finalSize;

      // scale to fit the padded area
      finalSize = naturalSize * std::min((!Dali::EqualsZero(naturalSize.width) ? (availableVisualSize.width / naturalSize.width) : 0),
                                         (!Dali::EqualsZero(naturalSize.height) ? (availableVisualSize.height / naturalSize.height) : 0));

      // calculate final offset within the padded area
      finalOffset += (availableVisualSize - finalSize) * .5f;

      // populate the transform map
      transformMap.Add(Toolkit::Visual::Transform::Property::OFFSET, finalOffset)
        .Add(Toolkit::Visual::Transform::Property::SIZE, finalSize);
      break;
    }
    case Visual::FittingMode::OVER_FIT_KEEP_ASPECT_RATIO:
    {
      mImageViewPixelAreaSetByFittingMode = true;
      auto availableVisualSize            = finalSize;
      finalSize                           = naturalSize * std::max((!Dali::EqualsZero(naturalSize.width) ? (availableVisualSize.width / naturalSize.width) : 0.0f),
                                         (!Dali::EqualsZero(naturalSize.height) ? (availableVisualSize.height / naturalSize.height) : 0.0f));

      auto originalOffset = finalOffset;
      finalOffset += (availableVisualSize - finalSize) * .5f;

      float   x           = abs((availableVisualSize.width - finalSize.width) / finalSize.width) * .5f;
      float   y           = abs((availableVisualSize.height - finalSize.height) / finalSize.height) * .5f;
      float   widthRatio  = 1.f - abs((availableVisualSize.width - finalSize.width) / finalSize.width);
      float   heightRatio = 1.f - abs((availableVisualSize.height - finalSize.height) / finalSize.height);
      Vector4 pixelArea   = Vector4(x, y, widthRatio, heightRatio);
      Self().SetProperty(Toolkit::ImageView::Property::PIXEL_AREA, pixelArea);

      // populate the transform map
      transformMap.Add(Toolkit::Visual::Transform::Property::OFFSET, originalOffset)
        .Add(Toolkit::Visual::Transform::Property::SIZE, availableVisualSize);
      break;
    }
    case Visual::FittingMode::CENTER:
    {
      auto availableVisualSize = finalSize;
      if(availableVisualSize.width > naturalSize.width && availableVisualSize.height > naturalSize.height)
      {
        finalSize = naturalSize;
      }
      else
      {
        finalSize = naturalSize * std::min((!Dali::EqualsZero(naturalSize.width) ? (availableVisualSize.width / naturalSize.width) : 0.0f),
                                           (!Dali::EqualsZero(naturalSize.height) ? (availableVisualSize.height / naturalSize.height) : 0.0f));
      }

      finalOffset += (availableVisualSize - finalSize) * .5f;

      // populate the transform map
      transformMap.Add(Toolkit::Visual::Transform::Property::OFFSET, finalOffset)
        .Add(Toolkit::Visual::Transform::Property::SIZE, finalSize);
      break;
    }
    case Visual::FittingMode::FILL:
    {
      transformMap.Add(Toolkit::Visual::Transform::Property::OFFSET, finalOffset)
        .Add(Toolkit::Visual::Transform::Property::SIZE, finalSize);
      break;
    }
    case Visual::FittingMode::FIT_WIDTH:
    case Visual::FittingMode::FIT_HEIGHT:
    {
      // This FittingMode already converted
      break;
    }
  }
}

void ImageView::ApplyFittingMode(const Vector2& size)
{
  Property::Map transformMap = Property::Map();

  Extents padding = Self().GetProperty<Extents>(Toolkit::Control::Property::PADDING);

  bool zeroPadding = (padding == Extents());

  Dali::LayoutDirection::Type layoutDirection = static_cast<Dali::LayoutDirection::Type>(
  Self().GetProperty(Dali::Actor::Property::LAYOUT_DIRECTION).Get<int>());
  if(Dali::LayoutDirection::RIGHT_TO_LEFT == layoutDirection)
  {
    std::swap(padding.start, padding.end);
  }

  // remove padding from the size to know how much is left for the visual
  Vector2 finalSize   = size - Vector2(padding.start + padding.end, padding.top + padding.bottom);
  Vector2 finalOffset = Vector2(padding.start, padding.top);

  Visual::FittingMode fittingMode = Toolkit::GetImplementation(mVisual).GetFittingMode();

  // Reset PIXEL_AREA after using OVER_FIT_KEEP_ASPECT_RATIO
  if(mImageViewPixelAreaSetByFittingMode)
  {
    Self().SetProperty(Toolkit::ImageView::Property::PIXEL_AREA, FULL_TEXTURE_RECT);
    mImageViewPixelAreaSetByFittingMode = false;
  }

  if((!zeroPadding) || // If padding is not zero
     (fittingMode != Visual::FittingMode::FILL))
  {
    mImageVisualPaddingSetByTransform = true;

    Vector2 naturalSize;
    // NaturalSize will not be used for FILL fitting mode, which is default.
    // Skip GetNaturalSize
    if(fittingMode != Visual::FittingMode::FILL)
    {
      mVisual.GetNaturalSize(naturalSize);
    }

    // If FittingMode use FIT_WIDTH or FIT_HEIGTH, it need to change proper fittingMode
    if(fittingMode == Visual::FittingMode::FIT_WIDTH)
    {
      fittingMode = (finalSize.height / naturalSize.height) < (finalSize.width / naturalSize.width) ? Visual::FittingMode::OVER_FIT_KEEP_ASPECT_RATIO : Visual::FittingMode::FIT_KEEP_ASPECT_RATIO;
    }
    else if(fittingMode == Visual::FittingMode::FIT_HEIGHT)
    {
      fittingMode = (finalSize.height / naturalSize.height) < (finalSize.width / naturalSize.width) ? Visual::FittingMode::FIT_KEEP_ASPECT_RATIO : Visual::FittingMode::OVER_FIT_KEEP_ASPECT_RATIO;
    }

    SetTransformMapForFittingMode(finalSize, naturalSize, finalOffset, fittingMode, transformMap);

    // Set extra value for applying transformMap
    transformMap.Add(Toolkit::Visual::Transform::Property::OFFSET_POLICY,
                     Vector2(Toolkit::Visual::Transform::Policy::ABSOLUTE, Toolkit::Visual::Transform::Policy::ABSOLUTE))
      .Add(Toolkit::Visual::Transform::Property::ORIGIN, Toolkit::Align::TOP_BEGIN)
      .Add(Toolkit::Visual::Transform::Property::ANCHOR_POINT, Toolkit::Align::TOP_BEGIN)
      .Add(Toolkit::Visual::Transform::Property::SIZE_POLICY,
           Vector2(Toolkit::Visual::Transform::Policy::ABSOLUTE, Toolkit::Visual::Transform::Policy::ABSOLUTE));
  }
  else if(mImageVisualPaddingSetByTransform && zeroPadding) // Reset offset to zero only if padding applied previously
  {
    mImageVisualPaddingSetByTransform = false;
    // Reset the transform map
    transformMap.Add(Toolkit::Visual::Transform::Property::OFFSET, Vector2::ZERO)
      .Add(Toolkit::Visual::Transform::Property::OFFSET_POLICY,
           Vector2(Toolkit::Visual::Transform::Policy::RELATIVE, Toolkit::Visual::Transform::Policy::RELATIVE))
      .Add(Toolkit::Visual::Transform::Property::SIZE, Vector2::ONE)
      .Add(Toolkit::Visual::Transform::Property::SIZE_POLICY,
           Vector2(Toolkit::Visual::Transform::Policy::RELATIVE, Toolkit::Visual::Transform::Policy::RELATIVE));
  }

  mVisual.SetTransformAndSize(transformMap, size);
}

void ImageView::CreatePlaceholderImage()
{
  Property::Map propertyMap;
  propertyMap.Insert(Toolkit::Visual::Property::TYPE, Toolkit::Visual::IMAGE);
  propertyMap.Insert(Toolkit::ImageVisual::Property::URL, mPlaceholderUrl);
  //propertyMap.Insert(Toolkit::ImageVisual::Property::LOAD_POLICY, Toolkit::ImageVisual::LoadPolicy::IMMEDIATE); // TODO: need to enable this property
  propertyMap.Insert(Toolkit::ImageVisual::Property::RELEASE_POLICY, Toolkit::ImageVisual::ReleasePolicy::DESTROYED);
  propertyMap.Insert(Toolkit::DevelImageVisual::Property::ENABLE_BROKEN_IMAGE, false);
  mPlaceholderVisual = Toolkit::VisualFactory::Get().CreateVisual(propertyMap);
  if(mPlaceholderVisual)
  {
    mPlaceholderVisual.SetName("placeholder");
  }
  else
  {
    DevelControl::UnregisterVisual(*this, Toolkit::ImageView::Property::PLACEHOLDER_IMAGE);
    mPlaceholderVisual.Reset();
  }
}

void ImageView::ShowPlaceholderImage()
{
  if(mPlaceholderVisual)
  {
    DevelControl::RegisterVisual(*this, Toolkit::ImageView::Property::PLACEHOLDER_IMAGE, mPlaceholderVisual, false);
    Actor self = Self();
    Toolkit::GetImplementation(mPlaceholderVisual).SetOnScene(self);
  }
}

void ImageView::HidePlaceholderImage()
{
  if(mPlaceholderVisual)
  {
    DevelControl::UnregisterVisual(*this, Toolkit::ImageView::Property::PLACEHOLDER_IMAGE);

    // Hide placeholder
    Actor self = Self();
    Toolkit::GetImplementation(mPlaceholderVisual).SetOffScene(self);
  }
}

void ImageView::TransitionImageWithEffect()
{
  Toolkit::ImageView handle = Toolkit::ImageView(GetOwner());

  if(handle)
  {
    mTransitionAnimation = Animation::New(TRANSITION_EFFECT_SPEED);
    mTransitionAnimation.SetEndAction(Animation::EndAction::DISCARD);
    float destinationAlpha = (mTransitionTargetAlpha > LOW_OPACITY) ? mTransitionTargetAlpha : LOW_OPACITY;

    // Transition current image
    Toolkit::Visual::Base imageVisual = DevelControl::GetVisual(*this, Toolkit::ImageView::Property::IMAGE);
    if(imageVisual)
    {
      Dali::KeyFrames fadeinKeyFrames = Dali::KeyFrames::New();
      fadeinKeyFrames.Add(0.0f, LOW_OPACITY);
      fadeinKeyFrames.Add(1.0f, destinationAlpha);
      mTransitionAnimation.AnimateBetween(DevelControl::GetVisualProperty(handle, Toolkit::ImageView::Property::IMAGE, Toolkit::Visual::Property::OPACITY), fadeinKeyFrames,  AlphaFunction::EASE_IN_OUT);
    }

    // Play transition animation
    mTransitionAnimation.FinishedSignal().Connect(this, &ImageView::OnTransitionAnimationFinishedCallback);
    mTransitionAnimation.Play();
  }
}

void ImageView::ClearTransitionAnimation()
{
  // Hide placeholder
  HidePlaceholderImage();

  // Clear PreviousVisual
  if(mPreviousVisual)
  {
    Actor                    self            = Self();
    Internal::Control::Impl& controlDataImpl = Internal::Control::Impl::Get(*this);
    controlDataImpl.EnableReadyTransitionOverriden(mVisual, false);
    Toolkit::GetImplementation(mPreviousVisual).SetOffScene(self);
    mPreviousVisual.Reset();
  }

  if(mTransitionAnimation)
  {
    mTransitionAnimation.FinishedSignal().Disconnect(this, &ImageView::OnTransitionAnimationFinishedCallback);
    mTransitionAnimation.Clear();
  }
}

///////////////////////////////////////////////////////////
//
// Properties
//

void ImageView::SetProperty(BaseObject* object, Property::Index index, const Property::Value& value)
{
  Toolkit::ImageView imageView = Toolkit::ImageView::DownCast(Dali::BaseHandle(object));

  if(imageView)
  {
    ImageView& impl = GetImpl(imageView);
    switch(index)
    {
      case Toolkit::ImageView::Property::IMAGE:
      {
        std::string          imageUrl;
        const Property::Map* map;
        if(value.Get(imageUrl))
        {
          impl.SetImage(imageUrl, ImageDimensions());
        }
        // if its not a string then get a Property::Map from the property if possible.
        else
        {
          map = value.GetMap();
          if(DALI_LIKELY(map))
          {
            // the property map is emtpy map. Unregister visual.
            if(DALI_UNLIKELY(map->Count() == 0u))
            {
              impl.ClearImageVisual();
            }
            else
            {
              Property::Value* shaderValue = map->Find(Toolkit::Visual::Property::SHADER, CUSTOM_SHADER);
              // set image only if property map contains image information other than custom shader
              if(map->Count() > 1u || !shaderValue)
              {
                impl.SetImage(*map);
              }
              // the property map contains only the custom shader
              else if((map->Count() == 1u) && (shaderValue))
              {
                Property::Map* shaderMap = shaderValue->GetMap();
                if(shaderMap)
                {
                  impl.mShaderMap = *shaderMap;

                  if(!impl.mUrl.empty())
                  {
                    impl.SetImage(impl.mUrl, impl.mImageSize);
                  }
                  else if(!impl.mPropertyMap.Empty())
                  {
                    impl.SetImage(impl.mPropertyMap);
                  }
                }
              }
            }
          }
          else
          {
            // invalid property value comes. Unregister visual.
            impl.ClearImageVisual();
          }
        }
        break;
      }

      case Toolkit::ImageView::Property::PRE_MULTIPLIED_ALPHA:
      {
        bool isPre;
        if(value.Get(isPre))
        {
          impl.EnablePreMultipliedAlpha(isPre);
        }
        break;
      }

      case Toolkit::ImageView::Property::PLACEHOLDER_IMAGE:
      {
        std::string placeholderUrl;
        if(value.Get(placeholderUrl))
        {
          impl.SetPlaceholderUrl(placeholderUrl);
        }
        break;
      }

      case Toolkit::ImageView::Property::ENABLE_TRANSITION_EFFECT:
      {
        bool transitionEffect;
        if(value.Get(transitionEffect))
        {
          impl.EnableTransitionEffect(transitionEffect);
        }
        break;
      }
    }
  }
}

Property::Value ImageView::GetProperty(BaseObject* object, Property::Index propertyIndex)
{
  Property::Value value;

  Toolkit::ImageView imageview = Toolkit::ImageView::DownCast(Dali::BaseHandle(object));

  if(imageview)
  {
    ImageView& impl = GetImpl(imageview);
    switch(propertyIndex)
    {
      case Toolkit::ImageView::Property::IMAGE:
      {
        if(!impl.mUrl.empty())
        {
          value = impl.mUrl;
        }
        else
        {
          Property::Map         map;
          Toolkit::Visual::Base visual = DevelControl::GetVisual(impl, Toolkit::ImageView::Property::IMAGE);
          if(visual)
          {
            visual.CreatePropertyMap(map);
          }
          value = map;
        }
        break;
      }

      case Toolkit::ImageView::Property::PRE_MULTIPLIED_ALPHA:
      {
        value = impl.IsPreMultipliedAlphaEnabled();
        break;
      }

      case Toolkit::ImageView::Property::PLACEHOLDER_IMAGE:
      {
        value = impl.GetPlaceholderUrl();
        break;
      }

      case Toolkit::ImageView::Property::ENABLE_TRANSITION_EFFECT:
      {
        value = impl.IsTransitionEffectEnabled();
        break;
      }
    }
  }

  return value;
}

void ImageView::OnTransitionAnimationFinishedCallback(Animation& animation)
{
  ClearTransitionAnimation();
}

} // namespace Internal
} // namespace Toolkit
} // namespace Dali
