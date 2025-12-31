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
#include <dali-toolkit/internal/visuals/svg/svg-visual.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/visuals/image/image-visual-shader-factory.h>
#include <dali-toolkit/internal/visuals/image/image-visual-shader-feature-builder.h>
#include <dali-toolkit/internal/visuals/svg/svg-loader.h>
#include <dali-toolkit/internal/visuals/visual-base-data-impl.h>
#include <dali-toolkit/internal/visuals/visual-string-constants.h>
#include <dali-toolkit/public-api/visuals/image-visual-properties.h>

// EXTERNAL INCLUDES
#include <dali/devel-api/common/stage.h>
#include <dali/devel-api/scripting/enum-helper.h>
#include <dali/devel-api/scripting/scripting.h>
#include <dali/integration-api/adaptor-framework/adaptor.h>
#include <dali/integration-api/debug.h>
#include <dali/public-api/rendering/decorated-visual-renderer.h>

namespace Dali
{
namespace Toolkit
{
namespace Internal
{
namespace
{
constexpr Dali::Vector4 FULL_TEXTURE_RECT(0.f, 0.f, 1.f, 1.f);

constexpr float ALPHA_VALUE_PREMULTIPLIED(1.0f);

// load policies
DALI_ENUM_TO_STRING_TABLE_BEGIN(LOAD_POLICY)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Dali::Toolkit::ImageVisual::LoadPolicy, IMMEDIATE)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Dali::Toolkit::ImageVisual::LoadPolicy, ATTACHED)
DALI_ENUM_TO_STRING_TABLE_END(LOAD_POLICY)

// release policies
DALI_ENUM_TO_STRING_TABLE_BEGIN(RELEASE_POLICY)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Dali::Toolkit::ImageVisual::ReleasePolicy, DETACHED)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Dali::Toolkit::ImageVisual::ReleasePolicy, DESTROYED)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Dali::Toolkit::ImageVisual::ReleasePolicy, NEVER)
DALI_ENUM_TO_STRING_TABLE_END(RELEASE_POLICY)

struct NameIndexMatch
{
  const char* const name;
  Property::Index   index;
};

const NameIndexMatch NAME_INDEX_MATCH_TABLE[] =
  {
    {IMAGE_DESIRED_WIDTH, Toolkit::ImageVisual::Property::DESIRED_WIDTH},
    {IMAGE_DESIRED_HEIGHT, Toolkit::ImageVisual::Property::DESIRED_HEIGHT},
    {SYNCHRONOUS_LOADING, Toolkit::ImageVisual::Property::SYNCHRONOUS_LOADING},
    {LOAD_POLICY_NAME, Toolkit::ImageVisual::Property::LOAD_POLICY},
    {RELEASE_POLICY_NAME, Toolkit::ImageVisual::Property::RELEASE_POLICY},
};
const int NAME_INDEX_MATCH_TABLE_SIZE = sizeof(NAME_INDEX_MATCH_TABLE) / sizeof(NAME_INDEX_MATCH_TABLE[0]);

} // namespace

SvgVisualPtr SvgVisual::New(VisualFactoryCache& factoryCache, ImageVisualShaderFactory& shaderFactory, const VisualUrl& imageUrl, const Property::Map& properties)
{
  SvgVisualPtr svgVisual(new SvgVisual(factoryCache, shaderFactory, imageUrl, ImageDimensions{}));
  svgVisual->SetProperties(properties);
  svgVisual->Initialize();
  return svgVisual;
}

SvgVisualPtr SvgVisual::New(VisualFactoryCache& factoryCache, ImageVisualShaderFactory& shaderFactory, const VisualUrl& imageUrl, ImageDimensions size)
{
  SvgVisualPtr svgVisual(new SvgVisual(factoryCache, shaderFactory, imageUrl, size));
  svgVisual->Initialize();
  return svgVisual;
}

SvgVisual::SvgVisual(VisualFactoryCache& factoryCache, ImageVisualShaderFactory& shaderFactory, const VisualUrl& imageUrl, ImageDimensions size)
: Visual::Base(factoryCache, Visual::FittingMode::DONT_CARE, Toolkit::Visual::SVG),
  mImageVisualShaderFactory(shaderFactory),
  mSvgLoader(factoryCache.GetSvgLoader()),
  mSvgLoadId(SvgLoader::INVALID_SVG_LOAD_ID),
  mSvgRasterizeId(SvgLoader::INVALID_SVG_RASTERIZE_ID),
  mImageUrl(imageUrl),
  mDefaultWidth(0),
  mDefaultHeight(0),
  mPlacementActor(),
  mDesiredSize(size),
  mLoadPolicy(Toolkit::ImageVisual::LoadPolicy::ATTACHED),
  mReleasePolicy(Toolkit::ImageVisual::ReleasePolicy::DETACHED),
  mLoadCompleted(false),
  mRasterizeCompleted(false),
  mLoadFailed(false),
  mRasterizeForcibly(true)
{
  // the rasterized image is with pre-multiplied alpha format
  mImpl->mFlags |= Impl::IS_PREMULTIPLIED_ALPHA;
}

SvgVisual::~SvgVisual()
{
  if(DALI_LIKELY(Dali::Adaptor::IsAvailable()))
  {
    if(mReleasePolicy != Toolkit::ImageVisual::ReleasePolicy::NEVER && mSvgLoadId != SvgLoader::INVALID_SVG_LOAD_ID)
    {
      mSvgLoader.RequestLoadRemove(mSvgLoadId, this);
      mSvgLoadId = SvgLoader::INVALID_SVG_LOAD_ID;
    }
    if(mReleasePolicy != Toolkit::ImageVisual::ReleasePolicy::NEVER && mSvgRasterizeId != SvgLoader::INVALID_SVG_RASTERIZE_ID)
    {
      // We don't need to remove task synchronously.
      mSvgLoader.RequestRasterizeRemove(mSvgRasterizeId, this, false);
      mSvgRasterizeId = SvgLoader::INVALID_SVG_RASTERIZE_ID;
    }

    if(mImageUrl.IsBufferResource())
    {
      TextureManager& textureManager = mFactoryCache.GetTextureManager();
      textureManager.RemoveEncodedImageBuffer(mImageUrl);
    }
  }
}

void SvgVisual::OnInitialize()
{
  Shader   shader   = GenerateShader();
  Geometry geometry = mFactoryCache.GetGeometry(VisualFactoryCache::QUAD_GEOMETRY);
  mImpl->mRenderer  = DecoratedVisualRenderer::New(geometry, shader);

  if(IsUsingCustomShader())
  {
    mImpl->mRenderer.RegisterVisualTransformUniform();
  }

  if(mSvgLoadId == SvgLoader::INVALID_SVG_LOAD_ID)
  {
    const Vector2 dpi     = Stage::GetCurrent().GetDpi();
    const float   meanDpi = (dpi.height + dpi.width) * 0.5f;

    const bool synchronousLoading = IsSynchronousLoadingRequired() && (mImageUrl.IsLocalResource() || mImageUrl.IsBufferResource());

    // It will call SvgVisual::LoadComplete() synchronously if it required, or we already loaded same svg before.
    mSvgLoadId = mSvgLoader.Load(mImageUrl, meanDpi, this, synchronousLoading);
  }
}

void SvgVisual::DoSetProperties(const Property::Map& propertyMap)
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
      for(int i = 0; i < NAME_INDEX_MATCH_TABLE_SIZE; ++i)
      {
        if(keyValue.first == NAME_INDEX_MATCH_TABLE[i].name)
        {
          DoSetProperty(NAME_INDEX_MATCH_TABLE[i].index, keyValue.second);
          break;
        }
      }
    }
  }

  // Load image immediately if LOAD_POLICY requires it
  if(mLoadPolicy == Toolkit::ImageVisual::LoadPolicy::IMMEDIATE)
  {
    const Vector2 dpi     = Stage::GetCurrent().GetDpi();
    const float   meanDpi = (dpi.height + dpi.width) * 0.5f;

    const bool synchronousLoading = IsSynchronousLoadingRequired() && (mImageUrl.IsLocalResource() || mImageUrl.IsBufferResource());

    // It will call SvgVisual::LoadComplete() synchronously if it required, or we already loaded same svg before.
    mSvgLoadId = mSvgLoader.Load(mImageUrl, meanDpi, this, synchronousLoading);

    AddRasterizationTask(mDesiredSize);
  }
}

void SvgVisual::DoSetProperty(Property::Index index, const Property::Value& value)
{
  switch(index)
  {
    case Toolkit::ImageVisual::Property::SYNCHRONOUS_LOADING:
    {
      bool sync = false;
      if(value.Get(sync))
      {
        if(sync)
        {
          mImpl->mFlags |= Impl::IS_SYNCHRONOUS_RESOURCE_LOADING;
        }
        else
        {
          mImpl->mFlags &= ~Impl::IS_SYNCHRONOUS_RESOURCE_LOADING;
        }
      }
      else
      {
        DALI_LOG_ERROR("ImageVisual: synchronousLoading property has incorrect type\n");
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
    case Toolkit::ImageVisual::Property::RELEASE_POLICY:
    {
      int releasePolicy = static_cast<int>(mReleasePolicy);
      if(DALI_LIKELY(Scripting::GetEnumerationProperty(value, RELEASE_POLICY_TABLE, RELEASE_POLICY_TABLE_COUNT, releasePolicy)))
      {
        mReleasePolicy = Toolkit::ImageVisual::ReleasePolicy::Type(releasePolicy);
      }
      break;
    }
    case Toolkit::ImageVisual::Property::LOAD_POLICY:
    {
      int loadPolicy = static_cast<int>(mLoadPolicy);
      if(DALI_LIKELY(Scripting::GetEnumerationProperty(value, LOAD_POLICY_TABLE, LOAD_POLICY_TABLE_COUNT, loadPolicy)))
      {
        mLoadPolicy = Toolkit::ImageVisual::LoadPolicy::Type(loadPolicy);
      }
      break;
    }
  }
}

void SvgVisual::DoSetOnScene(Actor& actor)
{
  // Register transform properties
  mImpl->SetTransformUniforms(mImpl->mRenderer, Direction::LEFT_TO_RIGHT);

  // Defer the rasterisation task until we get given a size (by Size Negotiation algorithm)

  // Hold the weak handle of the placement actor and delay the adding of renderer until the svg rasterization is finished.
  mPlacementActor = actor;

  if(mLoadFailed)
  {
    Vector2 imageSize = Vector2::ZERO;
    imageSize         = actor.GetProperty(Actor::Property::SIZE).Get<Vector2>();
    mFactoryCache.UpdateBrokenImageRenderer(mImpl->mRenderer, imageSize);
    actor.AddRenderer(mImpl->mRenderer);

    ResourceReady(Toolkit::Visual::ResourceStatus::FAILED);
  }
  else
  {
    if(mImpl->mEventObserver)
    {
      // SVG visual needs it's size set before it can be rasterized hence request relayout once on stage
      mImpl->mEventObserver->RelayoutRequest(*this);
    }

    if(mDesiredSize.GetWidth() > 0 && mDesiredSize.GetHeight() > 0)
    {
      // Use desired size. Need to request rasterize forcibly.
      AddRasterizationTask(mDesiredSize);

      if(mRasterizeCompleted)
      {
        // The case when we got cached rasterized result. Since "IsOnScene()" still false,
        // RasterizeComplete will not send resource ready signal. Need to emit this time.
        EmitResourceReady(Toolkit::Visual::ResourceStatus::READY);
      }
      else if(DALI_UNLIKELY(mLoadFailed))
      {
        // If rasterize failed.
        EmitResourceReady(Toolkit::Visual::ResourceStatus::FAILED);
      }
    }
  }
}

void SvgVisual::DoSetOffScene(Actor& actor)
{
  // Remove rasterizing task
  if(mReleasePolicy == Toolkit::ImageVisual::ReleasePolicy::DETACHED && mSvgRasterizeId != SvgLoader::INVALID_SVG_RASTERIZE_ID)
  {
    // We don't need to remove task synchronously.
    mSvgLoader.RequestRasterizeRemove(mSvgRasterizeId, this, false);
    mSvgRasterizeId = SvgLoader::INVALID_SVG_RASTERIZE_ID;
  }

  // When adding the actor back to stage the SVG rasterization should be forced again. (To emit ResourceReady signal at SceneOn).
  mRasterizeForcibly = true;

  actor.RemoveRenderer(mImpl->mRenderer);
  mPlacementActor.Reset();
}

void SvgVisual::GetNaturalSize(Vector2& naturalSize)
{
  if(mDesiredSize.GetWidth() > 0 && mDesiredSize.GetHeight() > 0)
  {
    naturalSize.x = mDesiredSize.GetWidth();
    naturalSize.y = mDesiredSize.GetHeight();
  }
  else if(mLoadFailed && mImpl->mRenderer)
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
    naturalSize.x = mDefaultWidth;
    naturalSize.y = mDefaultHeight;
  }
}

void SvgVisual::DoCreatePropertyMap(Property::Map& map) const
{
  map.Clear();
  map.Insert(Toolkit::Visual::Property::TYPE, Toolkit::Visual::SVG);
  if(mImageUrl.IsValid())
  {
    map.Insert(Toolkit::ImageVisual::Property::URL, mImageUrl.GetUrl());
  }

  map.Insert(Toolkit::ImageVisual::Property::SYNCHRONOUS_LOADING, IsSynchronousLoadingRequired());
  map.Insert(Toolkit::ImageVisual::Property::DESIRED_WIDTH, mDesiredSize.GetWidth());
  map.Insert(Toolkit::ImageVisual::Property::DESIRED_HEIGHT, mDesiredSize.GetHeight());
  map.Insert(Toolkit::ImageVisual::Property::LOAD_POLICY, mLoadPolicy);
  map.Insert(Toolkit::ImageVisual::Property::RELEASE_POLICY, mReleasePolicy);
}

void SvgVisual::DoCreateInstancePropertyMap(Property::Map& map) const
{
  // Do nothing
}

void SvgVisual::EnablePreMultipliedAlpha(bool preMultiplied)
{
  // Make always enable pre multiplied alpha whether preMultiplied value is false.
  if(!preMultiplied)
  {
    DALI_LOG_WARNING("Note : SvgVisual cannot disable PreMultipliedAlpha\n");
  }
}

void SvgVisual::EmitResourceReady(Toolkit::Visual::ResourceStatus resourceStatus)
{
  SvgVisualPtr self = this; // Keep reference until this API finished

  // Rasterized pixels are uploaded to texture. If weak handle is holding a placement actor, it is the time to add the renderer to actor.
  Actor actor = mPlacementActor.GetHandle();
  if(actor)
  {
    if(mImpl->mRenderer)
    {
      if(resourceStatus == Toolkit::Visual::ResourceStatus::FAILED)
      {
        Vector2 imageSize = Vector2::ZERO;
        imageSize         = actor.GetProperty(Actor::Property::SIZE).Get<Vector2>();
        mFactoryCache.UpdateBrokenImageRenderer(mImpl->mRenderer, imageSize);
      }
      actor.AddRenderer(mImpl->mRenderer);
    }
    // reset the weak handle so that the renderer only get added to actor once
    mPlacementActor.Reset();
  }

  // Svg loaded and ready to display
  ResourceReady(resourceStatus);
}

void SvgVisual::AddRasterizationTask(const Dali::ImageDimensions& size)
{
  if(!mRasterizeForcibly && size == mLastRequiredSize)
  {
    // No size change. Skip rasterization.
    return;
  }

  // Reset the flag
  mRasterizeForcibly = false;

  mLastRequiredSize = size;

  // Remove previous task
  if(mSvgRasterizeId != SvgLoader::INVALID_SVG_RASTERIZE_ID)
  {
    mSvgLoader.RequestRasterizeRemove(mSvgRasterizeId, this, false);
    mSvgRasterizeId = SvgLoader::INVALID_SVG_RASTERIZE_ID;
  }

  const bool synchronousRasterize = IsSynchronousLoadingRequired() && (mImageUrl.IsLocalResource() || mImageUrl.IsBufferResource());

  mRasterizeCompleted = false;
  mSvgRasterizeId     = mSvgLoader.Rasterize(mSvgLoadId, size.GetWidth(), size.GetHeight(), this, synchronousRasterize);
}

/// Called when SvgLoader::Load is completed.
void SvgVisual::LoadComplete(int32_t loadId, Dali::VectorImageRenderer vectorImageRenderer)
{
  // mSvgLoadId might not be updated if svg file is cached. Update now.
  mSvgLoadId = loadId;

  mLoadCompleted = true;

  if(DALI_LIKELY(vectorImageRenderer))
  {
    vectorImageRenderer.GetDefaultSize(mDefaultWidth, mDefaultHeight);
    if(mImpl->mEventObserver && mImpl->mFittingMode != DevelVisual::FittingMode::DONT_CARE)
    {
      // Need teo call ApplyFittingMode once again, after load completed.
      mImpl->mEventObserver->RelayoutRequest(*this);
    }

    // Very rarely, rasterize completed inovked before load completed invoke.
    // In this case, we should send resource ready here.
    if(DALI_UNLIKELY(mRasterizeCompleted && IsOnScene()))
    {
      EmitResourceReady(Toolkit::Visual::ResourceStatus::READY);
    }
  }
  else if(!mLoadFailed)
  {
    mLoadFailed = true;

    // Remove rasterizing task if we requested before.
    if(mSvgRasterizeId != SvgLoader::INVALID_SVG_RASTERIZE_ID)
    {
      mSvgLoader.RequestRasterizeRemove(mSvgRasterizeId, this, true);
      mSvgRasterizeId = SvgLoader::INVALID_SVG_RASTERIZE_ID;
    }

    if(IsOnScene())
    {
      EmitResourceReady(Toolkit::Visual::ResourceStatus::FAILED);
    }
  }
}

/// Called when SvgLoader::Rasterize is completed.
void SvgVisual::RasterizeComplete(int32_t rasterizeId, Dali::TextureSet textureSet)
{
  // rasterize id might not be updated if rasterize is cached.
  mSvgRasterizeId = rasterizeId;

  mRasterizeCompleted = true;

  if(DALI_LIKELY(textureSet))
  {
    bool updateShader = false;

    if(DALI_LIKELY(mImpl->mRenderer))
    {
      TextureSet currentTextureSet = mImpl->mRenderer.GetTextures();

      if(textureSet != currentTextureSet)
      {
        mImpl->mRenderer.SetTextures(textureSet);
      }

      if(DALI_UNLIKELY(updateShader))
      {
        UpdateShader();
      }
    }

    if(IsOnScene() && DALI_LIKELY(mLoadCompleted))
    {
      EmitResourceReady(Toolkit::Visual::ResourceStatus::READY);
    }
  }
  else if(!mLoadFailed)
  {
    mLoadFailed = true;

    if(IsOnScene())
    {
      EmitResourceReady(Toolkit::Visual::ResourceStatus::FAILED);
    }
  }
}

void SvgVisual::OnSetTransform()
{
  if(mImpl->mRenderer && mImpl->mTransformMapChanged)
  {
    mImpl->SetTransformUniforms(mImpl->mRenderer, Direction::LEFT_TO_RIGHT);
  }

  if(IsOnScene() && !mLoadFailed)
  {
    Dali::ImageDimensions size;
    if(mDesiredSize.GetWidth() > 0 && mDesiredSize.GetHeight() > 0)
    {
      // Use desired size
      size = mDesiredSize;
    }
    else
    {
      // Use visual size
      Vector2 visualSize = mImpl->GetTransformVisualSize(mImpl->mControlSize);

      // roundf and change as integer scale.
      size = Dali::ImageDimensions(static_cast<uint32_t>(roundf(visualSize.x)), static_cast<uint32_t>(roundf(visualSize.y)));
    }

    AddRasterizationTask(size);
  }
}

void SvgVisual::UpdateShader()
{
  if(mImpl->mRenderer)
  {
    Shader shader = GenerateShader();
    mImpl->mRenderer.SetShader(shader);
  }
}

Shader SvgVisual::GenerateShader() const
{
  Shader shader;
  if(!IsUsingCustomShader())
  {
    shader = mImageVisualShaderFactory.GetShader(
      mFactoryCache,
      ImageVisualShaderFeature::FeatureBuilder()
        .EnableRoundedCorner(IsRoundedCornerRequired(), IsSquircleCornerRequired())
        .EnableBorderline(IsBorderlineRequired()));
  }
  else
  {
    shader = Shader::New(mImpl->GetCustomShaderAt(0)->mVertexShader.empty() ? mImageVisualShaderFactory.GetVertexShaderSource().data() : mImpl->GetCustomShaderAt(0)->mVertexShader,
                         mImpl->GetCustomShaderAt(0)->mFragmentShader.empty() ? mImageVisualShaderFactory.GetFragmentShaderSource().data() : mImpl->GetCustomShaderAt(0)->mFragmentShader,
                         mImpl->GetCustomShaderAt(0)->mHints);

    shader.RegisterProperty(PIXEL_AREA_UNIFORM_NAME, FULL_TEXTURE_RECT);

    // Most of image visual shader user (like svg, animated vector image visual) use pre-multiplied alpha.
    // If the visual dont want to using pre-multiplied alpha, it should be set as 0.0f as renderer side.
    shader.RegisterProperty(PREMULTIPLIED_ALPHA, ALPHA_VALUE_PREMULTIPLIED);

    if(mImpl->mRenderer)
    {
      mImpl->mRenderer.RegisterVisualTransformUniform();
    }
  }
  return shader;
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
