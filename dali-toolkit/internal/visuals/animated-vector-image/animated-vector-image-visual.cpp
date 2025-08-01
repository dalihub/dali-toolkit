/*
 * Copyright (c) 2025 Samsung Electronics Co., Ltd.
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
#include <dali-toolkit/internal/visuals/animated-vector-image/animated-vector-image-visual.h>

// EXTERNAL INCLUDES
#include <dali/devel-api/adaptor-framework/window-devel.h>
#include <dali/devel-api/common/stage.h>
#include <dali/devel-api/rendering/renderer-devel.h>
#include <dali/integration-api/adaptor-framework/adaptor.h>
#include <dali/integration-api/debug.h>
#include <dali/public-api/math/math-utils.h>
#include <dali/public-api/rendering/decorated-visual-renderer.h>

// INTERNAL INCLUDES
#include <dali-toolkit/devel-api/visuals/animated-vector-image-visual-signals-devel.h>
#include <dali-toolkit/devel-api/visuals/image-visual-properties-devel.h>
#include <dali-toolkit/devel-api/visuals/visual-actions-devel.h>
#include <dali-toolkit/internal/visuals/animated-vector-image/vector-animation-manager.h>
#include <dali-toolkit/internal/visuals/image/image-visual-shader-factory.h>
#include <dali-toolkit/internal/visuals/image/image-visual-shader-feature-builder.h>
#include <dali-toolkit/internal/visuals/visual-base-data-impl.h>
#include <dali-toolkit/internal/visuals/visual-factory-cache.h>
#include <dali-toolkit/internal/visuals/visual-string-constants.h>
#include <dali-toolkit/public-api/visuals/image-visual-properties.h>
#include <dali-toolkit/public-api/visuals/visual-properties.h>

namespace Dali
{
namespace Toolkit
{
namespace Internal
{
namespace
{
const int CUSTOM_PROPERTY_COUNT(1); // PixelArea

const Dali::Vector4 FULL_TEXTURE_RECT(0.f, 0.f, 1.f, 1.f);

constexpr float ALPHA_VALUE_PREMULTIPLIED(1.0f);

// stop behavior
DALI_ENUM_TO_STRING_TABLE_BEGIN(STOP_BEHAVIOR)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Dali::Toolkit::DevelImageVisual::StopBehavior, CURRENT_FRAME)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Dali::Toolkit::DevelImageVisual::StopBehavior, FIRST_FRAME)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Dali::Toolkit::DevelImageVisual::StopBehavior, LAST_FRAME)
DALI_ENUM_TO_STRING_TABLE_END(STOP_BEHAVIOR)

// looping mode
DALI_ENUM_TO_STRING_TABLE_BEGIN(LOOPING_MODE)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Dali::Toolkit::DevelImageVisual::LoopingMode, RESTART)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Dali::Toolkit::DevelImageVisual::LoopingMode, AUTO_REVERSE)
DALI_ENUM_TO_STRING_TABLE_END(LOOPING_MODE)

constexpr float MINIMUM_FRAME_SPEED_FACTOR(0.01f);
constexpr float MAXIMUM_FRAME_SPEED_FACTOR(100.0f);

#if defined(DEBUG_ENABLED)
Debug::Filter* gVectorAnimationLogFilter = Debug::Filter::New(Debug::NoLogging, false, "LOG_VECTOR_ANIMATION");
#endif

} // unnamed namespace

AnimatedVectorImageVisualPtr AnimatedVectorImageVisual::New(VisualFactoryCache& factoryCache, ImageVisualShaderFactory& shaderFactory, const VisualUrl& imageUrl, const Property::Map& properties)
{
  AnimatedVectorImageVisualPtr visual(new AnimatedVectorImageVisual(factoryCache, shaderFactory, imageUrl, ImageDimensions{}));
  visual->SetProperties(properties);
  visual->Initialize();
  return visual;
}

AnimatedVectorImageVisualPtr AnimatedVectorImageVisual::New(VisualFactoryCache& factoryCache, ImageVisualShaderFactory& shaderFactory, const VisualUrl& imageUrl, ImageDimensions size)
{
  AnimatedVectorImageVisualPtr visual(new AnimatedVectorImageVisual(factoryCache, shaderFactory, imageUrl, size));
  visual->Initialize();
  return visual;
}

AnimatedVectorImageVisual::AnimatedVectorImageVisual(VisualFactoryCache& factoryCache, ImageVisualShaderFactory& shaderFactory, const VisualUrl& imageUrl, ImageDimensions size)
: Visual::Base(factoryCache, Visual::FittingMode::DONT_CARE, static_cast<Toolkit::Visual::Type>(Toolkit::DevelVisual::ANIMATED_VECTOR_IMAGE)),
  mImageUrl(imageUrl),
  mAnimationData(),
  mVectorAnimationTask(new VectorAnimationTask(factoryCache)),
  mImageVisualShaderFactory(shaderFactory),
  mVisualSize(),
  mVisualScale(Vector2::ONE),
  mDesiredSize(size),
  mPlacementActor(),
  mPlayState(DevelImageVisual::PlayState::STOPPED),
  mEventCallback(nullptr),
  mFrameSpeedFactor(1.0f),
  mRenderScale(1.0f),
  mLastSentPlayStateId(0u),
  mLoadFailed(false),
  mRendererAdded(false),
  mRedrawInScalingDown(true),
  mRedrawInScalingUp(true),
  mEnableFrameCache(false),
  mUseNativeImage(false),
  mNotifyAfterRasterization(false)
{
  // the rasterized image is with pre-multiplied alpha format
  mImpl->mFlags |= Visual::Base::Impl::IS_PREMULTIPLIED_ALPHA;

  // By default, load a file synchronously
  mImpl->mFlags |= Visual::Base::Impl::IS_SYNCHRONOUS_RESOURCE_LOADING;
}

AnimatedVectorImageVisual::~AnimatedVectorImageVisual()
{
  if(Dali::Adaptor::IsAvailable())
  {
    if(mImageUrl.IsBufferResource())
    {
      TextureManager& textureManager = mFactoryCache.GetTextureManager();
      textureManager.RemoveEncodedImageBuffer(mImageUrl);
    }

    if(mEventCallback)
    {
      mFactoryCache.GetVectorAnimationManager().UnregisterEventCallback(mEventCallback);
      mEventCallback = nullptr;
    }

    // Finalize animation task and disconnect the signal in the main thread
    mVectorAnimationTask->ResourceReadySignal().Disconnect(this, &AnimatedVectorImageVisual::OnResourceReady);
    mVectorAnimationTask->Finalize();
  }
}

void AnimatedVectorImageVisual::GetNaturalSize(Vector2& naturalSize)
{
  if(mDesiredSize.GetWidth() > 0 && mDesiredSize.GetHeight() > 0)
  {
    naturalSize.x = mDesiredSize.GetWidth();
    naturalSize.y = mDesiredSize.GetHeight();
  }
  else if(mVisualSize != Vector2::ZERO)
  {
    naturalSize = mVisualSize;
  }
  else
  {
    if(mLoadFailed && mImpl->mRenderer)
    {
      // Load failed, use broken image size
      auto textureSet = mImpl->mRenderer.GetTextures();
      if(textureSet && textureSet.GetTextureCount())
      {
        auto texture = textureSet.GetTexture(0);
        if(texture)
        {
          naturalSize.x = texture.GetWidth();
          naturalSize.y = texture.GetHeight();
          return;
        }
      }
    }
    else
    {
      uint32_t width, height;
      mVectorAnimationTask->GetDefaultSize(width, height);
      naturalSize.x = width;
      naturalSize.y = height;
    }
  }

  DALI_LOG_INFO(gVectorAnimationLogFilter, Debug::Verbose, "AnimatedVectorImageVisual::GetNaturalSize: w = %f, h = %f [%p]\n", naturalSize.width, naturalSize.height, this);
}

void AnimatedVectorImageVisual::DoCreatePropertyMap(Property::Map& map) const
{
  map.Clear();
  map.Insert(Toolkit::Visual::Property::TYPE, Toolkit::DevelVisual::ANIMATED_VECTOR_IMAGE);
  if(mImageUrl.IsValid())
  {
    map.Insert(Toolkit::ImageVisual::Property::URL, mImageUrl.GetUrl());
  }
  map.Insert(Toolkit::DevelImageVisual::Property::LOOP_COUNT, mAnimationData.loopCount);

  uint32_t startFrame, endFrame;
  mVectorAnimationTask->GetPlayRange(startFrame, endFrame);

  Property::Array playRange;
  playRange.PushBack(static_cast<int32_t>(startFrame));
  playRange.PushBack(static_cast<int32_t>(endFrame));
  map.Insert(Toolkit::DevelImageVisual::Property::PLAY_RANGE, playRange);

  map.Insert(Toolkit::DevelImageVisual::Property::PLAY_STATE, static_cast<int32_t>(mPlayState));
  map.Insert(Toolkit::DevelImageVisual::Property::CURRENT_FRAME_NUMBER, static_cast<int32_t>(mVectorAnimationTask->GetCurrentFrameNumber()));
  map.Insert(Toolkit::DevelImageVisual::Property::TOTAL_FRAME_NUMBER, static_cast<int32_t>(mVectorAnimationTask->GetTotalFrameNumber()));

  map.Insert(Toolkit::DevelImageVisual::Property::STOP_BEHAVIOR, mAnimationData.stopBehavior);
  map.Insert(Toolkit::DevelImageVisual::Property::LOOPING_MODE, mAnimationData.loopingMode);
  map.Insert(Toolkit::DevelImageVisual::Property::REDRAW_IN_SCALING_DOWN, mRedrawInScalingDown);
  map.Insert(Toolkit::DevelImageVisual::Property::REDRAW_IN_SCALING_UP, mRedrawInScalingUp);

  Property::Map layerInfo;
  mVectorAnimationTask->GetLayerInfo(layerInfo);
  map.Insert(Toolkit::DevelImageVisual::Property::CONTENT_INFO, layerInfo);

  Property::Map markerInfo;
  mVectorAnimationTask->GetMarkerInfo(markerInfo);
  map.Insert(Toolkit::DevelImageVisual::Property::MARKER_INFO, markerInfo);

  map.Insert(Toolkit::ImageVisual::Property::SYNCHRONOUS_LOADING, IsSynchronousLoadingRequired());
  map.Insert(Toolkit::ImageVisual::Property::DESIRED_WIDTH, mDesiredSize.GetWidth());
  map.Insert(Toolkit::ImageVisual::Property::DESIRED_HEIGHT, mDesiredSize.GetHeight());
  map.Insert(Toolkit::DevelImageVisual::Property::ENABLE_FRAME_CACHE, mEnableFrameCache);
  map.Insert(Toolkit::DevelImageVisual::Property::NOTIFY_AFTER_RASTERIZATION, mNotifyAfterRasterization);
  map.Insert(Toolkit::DevelImageVisual::Property::FRAME_SPEED_FACTOR, mFrameSpeedFactor);
  map.Insert(Toolkit::DevelImageVisual::Property::RENDER_SCALE, mRenderScale);
}

void AnimatedVectorImageVisual::DoCreateInstancePropertyMap(Property::Map& map) const
{
  // Do nothing
}

void AnimatedVectorImageVisual::EnablePreMultipliedAlpha(bool preMultiplied)
{
  // Make always enable pre multiplied alpha whether preMultiplied value is false.
  if(!preMultiplied)
  {
    DALI_LOG_WARNING("Note : AnimatedVectorVisual cannot disable PreMultipliedAlpha\n");
  }
}

void AnimatedVectorImageVisual::DoSetProperties(const Property::Map& propertyMap)
{
  // url already passed in from constructor
  for(Property::Map::SizeType iter = 0; iter < propertyMap.Count(); ++iter)
  {
    KeyValuePair keyValue = propertyMap.GetKeyValue(iter);
    if(keyValue.first.type == Property::Key::INDEX)
    {
      DoSetProperty(keyValue.first.indexKey, keyValue.second);
    }
    else
    {
      if(keyValue.first == LOOP_COUNT_NAME)
      {
        DoSetProperty(Toolkit::DevelImageVisual::Property::LOOP_COUNT, keyValue.second);
      }
      else if(keyValue.first == PLAY_RANGE_NAME)
      {
        DoSetProperty(Toolkit::DevelImageVisual::Property::PLAY_RANGE, keyValue.second);
      }
      else if(keyValue.first == STOP_BEHAVIOR_NAME)
      {
        DoSetProperty(Toolkit::DevelImageVisual::Property::STOP_BEHAVIOR, keyValue.second);
      }
      else if(keyValue.first == LOOPING_MODE_NAME)
      {
        DoSetProperty(Toolkit::DevelImageVisual::Property::LOOPING_MODE, keyValue.second);
      }
      else if(keyValue.first == REDRAW_IN_SCALING_DOWN_NAME)
      {
        DoSetProperty(Toolkit::DevelImageVisual::Property::REDRAW_IN_SCALING_DOWN, keyValue.second);
      }
      else if(keyValue.first == REDRAW_IN_SCALING_UP_NAME)
      {
        DoSetProperty(Toolkit::DevelImageVisual::Property::REDRAW_IN_SCALING_UP, keyValue.second);
      }
      else if(keyValue.first == SYNCHRONOUS_LOADING)
      {
        DoSetProperty(Toolkit::ImageVisual::Property::SYNCHRONOUS_LOADING, keyValue.second);
      }
      else if(keyValue.first == IMAGE_DESIRED_WIDTH)
      {
        DoSetProperty(Toolkit::ImageVisual::Property::DESIRED_WIDTH, keyValue.second);
      }
      else if(keyValue.first == IMAGE_DESIRED_HEIGHT)
      {
        DoSetProperty(Toolkit::ImageVisual::Property::DESIRED_HEIGHT, keyValue.second);
      }
      else if(keyValue.first == ENABLE_FRAME_CACHE)
      {
        DoSetProperty(Toolkit::DevelImageVisual::Property::ENABLE_FRAME_CACHE, keyValue.second);
      }
      else if(keyValue.first == NOTIFY_AFTER_RASTERIZATION)
      {
        DoSetProperty(Toolkit::DevelImageVisual::Property::NOTIFY_AFTER_RASTERIZATION, keyValue.second);
      }
      else if(keyValue.first == FRAME_SPEED_FACTOR)
      {
        DoSetProperty(Toolkit::DevelImageVisual::Property::FRAME_SPEED_FACTOR, keyValue.second);
      }
      else if(keyValue.first == RENDER_SCALE_NAME)
      {
        DoSetProperty(Toolkit::DevelImageVisual::Property::RENDER_SCALE, keyValue.second);
      }
    }
  }

  TriggerVectorRasterization();
}

void AnimatedVectorImageVisual::DoSetProperty(Property::Index index, const Property::Value& value)
{
  switch(index)
  {
    case Toolkit::DevelImageVisual::Property::LOOP_COUNT:
    {
      int32_t loopCount;
      if(value.Get(loopCount))
      {
        mAnimationData.loopCount = loopCount;
        mAnimationData.resendFlag |= VectorAnimationTask::RESEND_LOOP_COUNT;
      }
      break;
    }
    case Toolkit::DevelImageVisual::Property::PLAY_RANGE:
    {
      const Property::Array* array = value.GetArray();
      if(array)
      {
        mAnimationData.playRange = *array;
        mAnimationData.resendFlag |= VectorAnimationTask::RESEND_PLAY_RANGE;
      }
      else if(value.GetType() == Property::STRING)
      {
        std::string markerName;
        if(value.Get(markerName))
        {
          Property::Array array;
          array.Add(markerName);
          mAnimationData.playRange = std::move(array);
          mAnimationData.resendFlag |= VectorAnimationTask::RESEND_PLAY_RANGE;
        }
      }
      break;
    }
    case Toolkit::DevelImageVisual::Property::STOP_BEHAVIOR:
    {
      int32_t stopBehavior = mAnimationData.stopBehavior;
      if(Scripting::GetEnumerationProperty(value, STOP_BEHAVIOR_TABLE, STOP_BEHAVIOR_TABLE_COUNT, stopBehavior))
      {
        mAnimationData.stopBehavior = DevelImageVisual::StopBehavior::Type(stopBehavior);
        mAnimationData.resendFlag |= VectorAnimationTask::RESEND_STOP_BEHAVIOR;
      }
      break;
    }
    case Toolkit::DevelImageVisual::Property::LOOPING_MODE:
    {
      int32_t loopingMode = mAnimationData.loopingMode;
      if(Scripting::GetEnumerationProperty(value, LOOPING_MODE_TABLE, LOOPING_MODE_TABLE_COUNT, loopingMode))
      {
        mAnimationData.loopingMode = DevelImageVisual::LoopingMode::Type(loopingMode);
        mAnimationData.resendFlag |= VectorAnimationTask::RESEND_LOOPING_MODE;
      }
      break;
    }
    case Toolkit::DevelImageVisual::Property::REDRAW_IN_SCALING_DOWN:
    {
      bool redraw;
      if(value.Get(redraw))
      {
        mRedrawInScalingDown = redraw;
      }
      break;
    }
    case Toolkit::DevelImageVisual::Property::REDRAW_IN_SCALING_UP:
    {
      bool redraw;
      if(value.Get(redraw))
      {
        mRedrawInScalingUp = redraw;
      }
      break;
    }
    case Toolkit::ImageVisual::Property::SYNCHRONOUS_LOADING:
    {
      bool sync = false;
      if(value.Get(sync))
      {
        if(sync)
        {
          mImpl->mFlags |= Visual::Base::Impl::IS_SYNCHRONOUS_RESOURCE_LOADING;
        }
        else
        {
          mImpl->mFlags &= ~Visual::Base::Impl::IS_SYNCHRONOUS_RESOURCE_LOADING;
        }
      }
      break;
    }
    case Toolkit::ImageVisual::Property::DESIRED_WIDTH:
    {
      int32_t desiredWidth = 0;
      if(value.Get(desiredWidth))
      {
        mDesiredSize.SetWidth(desiredWidth);
      }
      break;
    }

    case Toolkit::ImageVisual::Property::DESIRED_HEIGHT:
    {
      int32_t desiredHeight = 0;
      if(value.Get(desiredHeight))
      {
        mDesiredSize.SetHeight(desiredHeight);
      }
      break;
    }

    case Toolkit::DevelImageVisual::Property::ENABLE_FRAME_CACHE:
    {
      bool enableFrameCache = false;
      if(value.Get(enableFrameCache))
      {
        mEnableFrameCache = enableFrameCache;
        if(mVectorAnimationTask)
        {
          mVectorAnimationTask->KeepRasterizedBuffer(mEnableFrameCache);
        }
      }
      break;
    }

    case Toolkit::DevelImageVisual::Property::NOTIFY_AFTER_RASTERIZATION:
    {
      bool notifyAfterRasterization = false;
      if(value.Get(notifyAfterRasterization))
      {
        if(mNotifyAfterRasterization != notifyAfterRasterization)
        {
          mNotifyAfterRasterization = notifyAfterRasterization;

          mAnimationData.notifyAfterRasterization = mNotifyAfterRasterization;
          mAnimationData.resendFlag |= VectorAnimationTask::RESEND_NOTIFY_AFTER_RASTERIZATION;
        }
      }
      break;
    }

    case Toolkit::DevelImageVisual::Property::FRAME_SPEED_FACTOR:
    {
      float frameSpeedFactor = 1.0f;
      if(value.Get(frameSpeedFactor))
      {
        // TODO : Could we remove this limitation?
        Dali::ClampInPlace(frameSpeedFactor, MINIMUM_FRAME_SPEED_FACTOR, MAXIMUM_FRAME_SPEED_FACTOR);

        if(!Dali::Equals(mFrameSpeedFactor, frameSpeedFactor))
        {
          mFrameSpeedFactor = frameSpeedFactor;

          mAnimationData.frameSpeedFactor = mFrameSpeedFactor;
          mAnimationData.resendFlag |= VectorAnimationTask::RESEND_FRAME_SPEED_FACTOR;
        }
      }
      break;
    }

    case Toolkit::DevelImageVisual::Property::RENDER_SCALE:
    {
      float renderScale = 1.0f;
      if(value.Get(renderScale))
      {
        mRenderScale = renderScale;
        SetVectorImageSize();
      }
      break;
    }
  }
}

void AnimatedVectorImageVisual::OnInitialize(void)
{
  mVectorAnimationTask->ResourceReadySignal().Connect(this, &AnimatedVectorImageVisual::OnResourceReady);
  mVectorAnimationTask->SetAnimationFinishedCallback(MakeCallback(this, &AnimatedVectorImageVisual::OnAnimationFinished));

  EncodedImageBuffer encodedImageBuffer;

  if(mImageUrl.IsBufferResource())
  {
    // Increase reference count of External Resources :
    // EncodedImageBuffer.
    // Reference count will be decreased at destructor of the visual.
    TextureManager& textureManager = mFactoryCache.GetTextureManager();
    textureManager.UseExternalResource(mImageUrl);

    encodedImageBuffer = textureManager.GetEncodedImageBuffer(mImageUrl);
  }

  mVectorAnimationTask->KeepRasterizedBuffer(mEnableFrameCache);
  mVectorAnimationTask->RequestLoad(mImageUrl, encodedImageBuffer, IsSynchronousLoadingRequired());

  Shader shader = GenerateShader();

  Geometry geometry = mFactoryCache.GetGeometry(VisualFactoryCache::QUAD_GEOMETRY);

  mImpl->mRenderer = DecoratedVisualRenderer::New(geometry, shader);
  mImpl->mRenderer.ReserveCustomProperties(CUSTOM_PROPERTY_COUNT);

  TextureSet textureSet = TextureSet::New();
  mImpl->mRenderer.SetTextures(textureSet);

  // Register transform properties
  mImpl->SetTransformUniforms(mImpl->mRenderer, Direction::LEFT_TO_RIGHT);
  if(mImpl->mCustomShader)
  {
    mImpl->mRenderer.RegisterVisualTransformUniform();
  }

  mVectorAnimationTask->SetRenderer(mImpl->mRenderer);
}

void AnimatedVectorImageVisual::DoSetOnScene(Actor& actor)
{
  // Defer the rasterisation task until we get given a size (by Size Negotiation algorithm)

  // Hold the weak handle of the placement actor and delay the adding of renderer until the rasterization is finished.
  mPlacementActor = actor;

  if(mLoadFailed)
  {
    Vector2 imageSize = actor.GetProperty(Actor::Property::SIZE).Get<Vector2>();
    mFactoryCache.UpdateBrokenImageRenderer(mImpl->mRenderer, imageSize, false);
    actor.AddRenderer(mImpl->mRenderer);
    mRendererAdded = true;
    ResourceReady(Toolkit::Visual::ResourceStatus::FAILED);
  }
  else
  {
    // Add property notification for scaling & size
    mScaleNotification = actor.AddPropertyNotification(Actor::Property::WORLD_SCALE, StepCondition(0.1f, 1.0f));
    mScaleNotification.NotifySignal().Connect(this, &AnimatedVectorImageVisual::OnScaleNotification);

    mSizeNotification = actor.AddPropertyNotification(Actor::Property::SIZE, StepCondition(3.0f));
    mSizeNotification.NotifySignal().Connect(this, &AnimatedVectorImageVisual::OnSizeNotification);

    actor.InheritedVisibilityChangedSignal().Connect(this, &AnimatedVectorImageVisual::OnControlInheritedVisibilityChanged);

    if(mImpl->mEventObserver)
    {
      // The visual needs it's size set before it can be rasterized hence request relayout once on stage
      mImpl->mEventObserver->RelayoutRequest(*this);
    }

    mAnimationData.resendFlag |= VectorAnimationTask::RESEND_NEED_RESOURCE_READY;
    TriggerVectorRasterization();
  }

  DALI_LOG_INFO(gVectorAnimationLogFilter, Debug::Verbose, "AnimatedVectorImageVisual::DoSetOnScene [%p]\n", this);
}

void AnimatedVectorImageVisual::DoSetOffScene(Actor& actor)
{
  StopAnimation();
  TriggerVectorRasterization();

  if(mImpl->mRenderer)
  {
    actor.RemoveRenderer(mImpl->mRenderer);
    mRendererAdded = false;
  }

  // Remove property notification
  actor.RemovePropertyNotification(mScaleNotification);
  actor.RemovePropertyNotification(mSizeNotification);

  actor.InheritedVisibilityChangedSignal().Disconnect(this, &AnimatedVectorImageVisual::OnControlInheritedVisibilityChanged);

  mPlacementActor.Reset();

  // Reset the visual size to zero so that when adding the actor back to stage the rasterization is forced
  mVisualSize           = Vector2::ZERO;
  mVisualScale          = Vector2::ONE;
  mAnimationData.width  = 0;
  mAnimationData.height = 0;

  DALI_LOG_INFO(gVectorAnimationLogFilter, Debug::Verbose, "AnimatedVectorImageVisual::DoSetOffScene [%p]\n", this);
}

void AnimatedVectorImageVisual::OnSetTransform()
{
  if(mImpl->mRenderer && mImpl->mTransformMapChanged)
  {
    mImpl->SetTransformUniforms(mImpl->mRenderer, Direction::LEFT_TO_RIGHT);
  }

  if(IsOnScene())
  {
    Vector2 visualSize = mImpl->GetTransformVisualSize(mImpl->mControlSize);

    if(visualSize != mVisualSize)
    {
      DALI_LOG_INFO(gVectorAnimationLogFilter, Debug::Verbose, "AnimatedVectorImageVisual::OnSetTransform: width = %f, height = %f [%p]\n", visualSize.width, visualSize.height, this);

      mVisualSize = visualSize;

      SetVectorImageSize();

      if(mPlayState == DevelImageVisual::PlayState::PLAYING && mAnimationData.playState != DevelImageVisual::PlayState::PLAYING)
      {
        mAnimationData.playState = DevelImageVisual::PlayState::PLAYING;
        mAnimationData.resendFlag |= VectorAnimationTask::RESEND_PLAY_STATE;
      }

      TriggerVectorRasterization();
    }
  }
}

void AnimatedVectorImageVisual::UpdateShader()
{
  if(mImpl->mRenderer)
  {
    Shader shader = GenerateShader();
    mImpl->mRenderer.SetShader(shader);
  }
}

void AnimatedVectorImageVisual::OnDoAction(const Property::Index actionId, const Property::Value& attributes)
{
  // Check if action is valid for this visual type and perform action if possible
  switch(actionId)
  {
    case DevelAnimatedVectorImageVisual::Action::PLAY:
    {
      if(IsOnScene() && mVisualSize != Vector2::ZERO)
      {
        // Always resend Playing state. If task is already playing, it will be ignored at Rasterize time.
        mAnimationData.playState = DevelImageVisual::PlayState::PLAYING;
        mAnimationData.resendFlag |= VectorAnimationTask::RESEND_PLAY_STATE;
      }
      DALI_LOG_DEBUG_INFO("[%p] Play request (url:%s)\n", this, mImageUrl.GetEllipsedUrl().c_str());
      mPlayState = DevelImageVisual::PlayState::PLAYING;
      break;
    }
    case DevelAnimatedVectorImageVisual::Action::PAUSE:
    {
      if(mAnimationData.playState == DevelImageVisual::PlayState::PLAYING)
      {
        mAnimationData.playState = DevelImageVisual::PlayState::PAUSED;
        mAnimationData.resendFlag |= VectorAnimationTask::RESEND_PLAY_STATE;
      }
      DALI_LOG_DEBUG_INFO("[%p] Pause request (url:%s)\n", this, mImageUrl.GetEllipsedUrl().c_str());
      mPlayState = DevelImageVisual::PlayState::PAUSED;
      break;
    }
    case DevelAnimatedVectorImageVisual::Action::STOP:
    {
      if(mAnimationData.playState != DevelImageVisual::PlayState::STOPPED)
      {
        mAnimationData.playState = DevelImageVisual::PlayState::STOPPED;
        mAnimationData.resendFlag |= VectorAnimationTask::RESEND_PLAY_STATE;
      }
      DALI_LOG_DEBUG_INFO("[%p] Stop request (url:%s)\n", this, mImageUrl.GetEllipsedUrl().c_str());
      mPlayState = DevelImageVisual::PlayState::STOPPED;
      break;
    }
    case DevelAnimatedVectorImageVisual::Action::JUMP_TO:
    {
      int32_t frameNumber;
      if(attributes.Get(frameNumber))
      {
        mAnimationData.currentFrame = frameNumber;
        mAnimationData.resendFlag |= VectorAnimationTask::RESEND_CURRENT_FRAME;
        DALI_LOG_DEBUG_INFO("[%p] Jump to (%d) request (url:%s)\n", this, frameNumber, mImageUrl.GetEllipsedUrl().c_str());
      }
      break;
    }
    case DevelAnimatedVectorImageVisual::Action::FLUSH:
    {
      if(DALI_LIKELY(Dali::Adaptor::IsAvailable()))
      {
        DALI_LOG_DEBUG_INFO("[%p] Flush request (url:%s)\n", this, mImageUrl.GetEllipsedUrl().c_str());
        SendAnimationData();
      }
      break;
    }
  }

  TriggerVectorRasterization();
}

void AnimatedVectorImageVisual::OnDoActionExtension(const Property::Index actionId, const Dali::Any& attributes)
{
  switch(actionId)
  {
    case DevelAnimatedVectorImageVisual::Action::SET_DYNAMIC_PROPERTY:
    {
      DevelAnimatedVectorImageVisual::DynamicPropertyInfo info = AnyCast<DevelAnimatedVectorImageVisual::DynamicPropertyInfo>(attributes);
      mAnimationData.dynamicProperties.push_back(info);
      mAnimationData.resendFlag |= VectorAnimationTask::RESEND_DYNAMIC_PROPERTY;
      break;
    }
  }

  TriggerVectorRasterization();
}

void AnimatedVectorImageVisual::OnResourceReady(VectorAnimationTask::ResourceStatus status)
{
  DALI_LOG_DEBUG_INFO("[%p] OnResourceReady (status:%d) (url:%s)\n", this, static_cast<int>(status), mImageUrl.GetEllipsedUrl().c_str());

  AnimatedVectorImageVisualPtr self = this; // Keep reference until this API finished

  if(status == VectorAnimationTask::ResourceStatus::LOADED)
  {
    if(mImpl->mEventObserver)
    {
      mImpl->mEventObserver->RelayoutRequest(*this);
    }
  }
  else
  {
    mLoadFailed = status == VectorAnimationTask::ResourceStatus::FAILED ? true : false;
    if(status == VectorAnimationTask::ResourceStatus::READY)
    {
      // Texture was ready. Change the shader if we need.
      bool useNativeImage = false;
      if(mImpl->mRenderer)
      {
        auto textureSet = mImpl->mRenderer.GetTextures();
        if(textureSet && textureSet.GetTextureCount() > 0)
        {
          auto texture = textureSet.GetTexture(0u);
          if(texture)
          {
            useNativeImage = DevelTexture::IsNative(texture);

            if(mUseNativeImage != useNativeImage)
            {
              mUseNativeImage = useNativeImage;
              UpdateShader();
            }
          }
        }
      }
    }

    // If weak handle is holding a placement actor, it is the time to add the renderer to actor.
    Actor actor = mPlacementActor.GetHandle();
    if(actor && !mRendererAdded)
    {
      if(!mLoadFailed)
      {
        actor.AddRenderer(mImpl->mRenderer);
        ResourceReady(Toolkit::Visual::ResourceStatus::READY);
      }
      else
      {
        Vector2 imageSize = actor.GetProperty(Actor::Property::SIZE).Get<Vector2>();
        mFactoryCache.UpdateBrokenImageRenderer(mImpl->mRenderer, imageSize, false);
        actor.AddRenderer(mImpl->mRenderer);
        ResourceReady(Toolkit::Visual::ResourceStatus::FAILED);
      }

      mRendererAdded = true;
    }
  }

  DALI_LOG_INFO(gVectorAnimationLogFilter, Debug::Verbose, "status = %d [%p]\n", status, this);
}

void AnimatedVectorImageVisual::OnAnimationFinished(uint32_t playStateId)
{
  DALI_LOG_DEBUG_INFO("[%p] OnAnimationFinished (id : %u vs %u) (url:%s)\n", this, mLastSentPlayStateId, playStateId, mImageUrl.GetEllipsedUrl().c_str());

  // Only send event when animation is finished by the last Play/Pause/Stop request.
  if(mLastSentPlayStateId != playStateId)
  {
    return;
  }

  AnimatedVectorImageVisualPtr self = this; // Keep reference until this API finished

  DALI_LOG_INFO(gVectorAnimationLogFilter, Debug::Verbose, "AnimatedVectorImageVisual::OnAnimationFinished: action state = %d [%p]\n", mPlayState, this);

  if(mPlayState != DevelImageVisual::PlayState::STOPPED)
  {
    mPlayState = DevelImageVisual::PlayState::STOPPED;

    mAnimationData.playState = DevelImageVisual::PlayState::STOPPED;

    if(mImpl->mEventObserver)
    {
      mImpl->mEventObserver->NotifyVisualEvent(*this, DevelAnimatedVectorImageVisual::Signal::ANIMATION_FINISHED);
    }
  }

  if(!mNotifyAfterRasterization && mImpl->mRenderer)
  {
    mImpl->mRenderer.SetProperty(DevelRenderer::Property::RENDERING_BEHAVIOR, DevelRenderer::Rendering::IF_REQUIRED);
  }
}

void AnimatedVectorImageVisual::SendAnimationData()
{
  if(mAnimationData.resendFlag)
  {
    if(mAnimationData.resendFlag & VectorAnimationTask::RESEND_PLAY_STATE)
    {
      // Keep last sent playId. It will be used when we try to emit AnimationFinished signal.
      // The OnAnimationFinished signal what before Play/Pause/Stop action send could be come after action sent.
      // To ensure the OnAnimationFinished signal comes belong to what we sent, we need to keep last sent playId.
      mAnimationData.playStateId = ++mLastSentPlayStateId;
      DALI_LOG_DEBUG_INFO("[%p] Resend play state request (id:%u) (state:%d,%d) (url:%s)\n", this, mLastSentPlayStateId, static_cast<int>(mPlayState), static_cast<int>(mAnimationData.playState), mImageUrl.GetEllipsedUrl().c_str());
    }
    mVectorAnimationTask->SetAnimationData(mAnimationData);

    if(mAnimationData.resendFlag & VectorAnimationTask::RESEND_DYNAMIC_PROPERTY)
    {
      // Remove applied dynamic properties
      mAnimationData.dynamicProperties.clear();
    }

    if(mImpl->mRenderer &&
       ((mAnimationData.resendFlag & VectorAnimationTask::RESEND_PLAY_STATE) ||
        (mAnimationData.resendFlag & VectorAnimationTask::RESEND_NOTIFY_AFTER_RASTERIZATION)))
    {
      if(!mNotifyAfterRasterization && mPlayState == DevelImageVisual::PlayState::PLAYING)
      {
        // Make rendering behaviour if we don't notify after rasterization, but animation playing.
        mImpl->mRenderer.SetProperty(DevelRenderer::Property::RENDERING_BEHAVIOR, DevelRenderer::Rendering::CONTINUOUSLY);
      }
      else
      {
        // Otherwise, notify will be sended after rasterization. Make behaviour as required.
        mImpl->mRenderer.SetProperty(DevelRenderer::Property::RENDERING_BEHAVIOR, DevelRenderer::Rendering::IF_REQUIRED);
      }
    }

    mAnimationData.resendFlag = 0;
  }
}

void AnimatedVectorImageVisual::SetVectorImageSize()
{
  uint32_t width, height;
  if(mDesiredSize.GetWidth() > 0 && mDesiredSize.GetHeight() > 0)
  {
    width  = mDesiredSize.GetWidth();
    height = mDesiredSize.GetHeight();
  }
  else
  {
    width  = static_cast<uint32_t>(std::roundf(mVisualSize.width * mVisualScale.width * std::fabs(mRenderScale)));
    height = static_cast<uint32_t>(std::roundf(mVisualSize.height * mVisualScale.height * std::fabs(mRenderScale)));
  }

  if(mAnimationData.width != width || mAnimationData.height != height)
  {
    mAnimationData.width  = width;
    mAnimationData.height = height;
    mAnimationData.resendFlag |= VectorAnimationTask::RESEND_SIZE;
  }
}

void AnimatedVectorImageVisual::StopAnimation()
{
  if(mAnimationData.playState != DevelImageVisual::PlayState::STOPPED)
  {
    mAnimationData.playState = DevelImageVisual::PlayState::STOPPED;
    mAnimationData.resendFlag |= VectorAnimationTask::RESEND_PLAY_STATE;

    mPlayState = DevelImageVisual::PlayState::STOPPED;
  }
}

void AnimatedVectorImageVisual::TriggerVectorRasterization()
{
  if(!mEventCallback && Dali::Adaptor::IsAvailable())
  {
    mEventCallback               = MakeCallback(this, &AnimatedVectorImageVisual::OnProcessEvents);
    auto& vectorAnimationManager = mFactoryCache.GetVectorAnimationManager();
    vectorAnimationManager.RegisterEventCallback(mEventCallback);
    Stage::GetCurrent().KeepRendering(0.0f); // Trigger event processing
  }
}

void AnimatedVectorImageVisual::OnScaleNotification(PropertyNotification& source)
{
  Actor actor = mPlacementActor.GetHandle();
  if(actor)
  {
    Vector3 scale = actor.GetProperty<Vector3>(Actor::Property::WORLD_SCALE);

    if((!Dali::Equals(mVisualScale.width, scale.width) || !Dali::Equals(mVisualScale.height, scale.height)))
    {
      bool redrawInScalingDown = mRedrawInScalingDown && (scale.width <= 1.0f || scale.height <= 1.0f);
      bool redrawInScalingUp   = mRedrawInScalingUp && (scale.width >= 1.0f || scale.height >= 1.0f);

      if(redrawInScalingDown || redrawInScalingUp)
      {
        mVisualScale.width  = scale.width;
        mVisualScale.height = scale.height;

        DALI_LOG_INFO(gVectorAnimationLogFilter, Debug::Verbose, "AnimatedVectorImageVisual::OnScaleNotification: scale = %f, %f [%p]\n", mVisualScale.width, mVisualScale.height, this);

        SetVectorImageSize();
        SendAnimationData();

        Stage::GetCurrent().KeepRendering(0.0f); // Trigger event processing
      }
    }
  }
}

void AnimatedVectorImageVisual::OnSizeNotification(PropertyNotification& source)
{
  Actor actor = mPlacementActor.GetHandle();
  if(actor)
  {
    Vector3 size = actor.GetCurrentProperty<Vector3>(Actor::Property::SIZE);

    if(!Dali::Equals(mVisualSize.width, size.width) || !Dali::Equals(mVisualSize.height, size.height))
    {
      mVisualSize.width  = size.width;
      mVisualSize.height = size.height;

      DALI_LOG_INFO(gVectorAnimationLogFilter, Debug::Verbose, "AnimatedVectorImageVisual::OnSizeNotification: size = %f, %f [%p]\n", mVisualSize.width, mVisualSize.height, this);

      SetVectorImageSize();
      SendAnimationData();

      Stage::GetCurrent().KeepRendering(0.0f); // Trigger event processing
    }
  }
}

void AnimatedVectorImageVisual::OnControlInheritedVisibilityChanged(Actor actor, bool visible)
{
  if(!visible)
  {
    StopAnimation();
    TriggerVectorRasterization();

    DALI_LOG_INFO(gVectorAnimationLogFilter, Debug::Verbose, "AnimatedVectorImageVisual::OnControlInheritedVisibilityChanged: invisibile. Pause animation [%p]\n", this);
  }
}

void AnimatedVectorImageVisual::OnProcessEvents()
{
  SendAnimationData();

  mEventCallback = nullptr; // The callback will be deleted in the VectorAnimationManager
}

Shader AnimatedVectorImageVisual::GenerateShader() const
{
  Shader shader;
  if(mImpl->mCustomShader)
  {
    shader = Shader::New(mImpl->mCustomShader->mVertexShader.empty() ? mImageVisualShaderFactory.GetVertexShaderSource().data() : mImpl->mCustomShader->mVertexShader,
                         mImpl->mCustomShader->mFragmentShader.empty() ? mImageVisualShaderFactory.GetFragmentShaderSource().data() : mImpl->mCustomShader->mFragmentShader,
                         mImpl->mCustomShader->mHints);

    shader.RegisterProperty(PIXEL_AREA_UNIFORM_NAME, FULL_TEXTURE_RECT);

    // Most of image visual shader user (like svg, animated vector image visual) use pre-multiplied alpha.
    // If the visual dont want to using pre-multiplied alpha, it should be set as 0.0f as renderer side.
    shader.RegisterProperty(PREMULTIPLIED_ALPHA, ALPHA_VALUE_PREMULTIPLIED);

    if(mImpl->mRenderer)
    {
      mImpl->mRenderer.RegisterVisualTransformUniform();
    }
  }
  else
  {
    shader = mImageVisualShaderFactory.GetShader(
      mFactoryCache,
      ImageVisualShaderFeature::FeatureBuilder()
        .EnableRoundedCorner(IsRoundedCornerRequired(), IsSquircleCornerRequired())
        .EnableBorderline(IsBorderlineRequired())
        .SetTextureForFragmentShaderCheck(mUseNativeImage ? mImpl->mRenderer.GetTextures().GetTexture(0) : Dali::Texture()));
  }
  return shader;
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
