/*
 * Copyright (c) 2024 Samsung Electronics Co., Ltd.
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
#include <dali-toolkit/internal/visuals/image/image-atlas-manager.h>
#include <dali-toolkit/internal/visuals/image/image-visual-shader-factory.h>
#include <dali-toolkit/internal/visuals/image/image-visual-shader-feature-builder.h>
#include <dali-toolkit/internal/visuals/svg/svg-loader.h>
#include <dali-toolkit/internal/visuals/visual-base-data-impl.h>
#include <dali-toolkit/internal/visuals/visual-string-constants.h>
#include <dali-toolkit/public-api/visuals/image-visual-properties.h>

// EXTERNAL INCLUDES
#include <dali/devel-api/common/stage.h>
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
const int CUSTOM_PROPERTY_COUNT(1); // atlas

constexpr Dali::Vector4 FULL_TEXTURE_RECT(0.f, 0.f, 1.f, 1.f);

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
  mAtlasRect(FULL_TEXTURE_RECT),
  mAtlasRectIndex(Property::INVALID_INDEX),
  mImageUrl(imageUrl),
  mDefaultWidth(0),
  mDefaultHeight(0),
  mPlacementActor(),
  mRasterizedSize(-Vector2::ONE), ///< Let we don't use zero since visual size could be zero after trasnform
  mDesiredSize(size),
  mLoadFailed(false),
  mAttemptAtlasing(false)
{
  // the rasterized image is with pre-multiplied alpha format
  mImpl->mFlags |= Impl::IS_PREMULTIPLIED_ALPHA;
}

SvgVisual::~SvgVisual()
{
  if(Stage::IsInstalled())
  {
    if(mSvgLoadId != SvgLoader::INVALID_SVG_LOAD_ID)
    {
      mSvgLoader.RequestLoadRemove(mSvgLoadId, this);
      mSvgLoadId = SvgLoader::INVALID_SVG_LOAD_ID;
    }
    if(mSvgRasterizeId != SvgLoader::INVALID_SVG_LOAD_ID)
    {
      // We don't need to remove task synchronously.
      mSvgLoader.RequestRasterizeRemove(mSvgRasterizeId, this, false);
      mSvgRasterizeId = SvgLoader::INVALID_SVG_RASTERIZE_ID;
    }

    if(mImageUrl.IsBufferResource())
    {
      TextureManager& textureManager = mFactoryCache.GetTextureManager();
      textureManager.RemoveEncodedImageBuffer(mImageUrl.GetUrl());
    }
  }
}

void SvgVisual::OnInitialize()
{
  Shader   shader   = GenerateShader();
  Geometry geometry = mFactoryCache.GetGeometry(VisualFactoryCache::QUAD_GEOMETRY);
  mImpl->mRenderer  = DecoratedVisualRenderer::New(geometry, shader);
  mImpl->mRenderer.ReserveCustomProperties(CUSTOM_PROPERTY_COUNT);

  Vector2 dpi     = Stage::GetCurrent().GetDpi();
  float   meanDpi = (dpi.height + dpi.width) * 0.5f;

  const bool synchronousLoading = IsSynchronousLoadingRequired() && (mImageUrl.IsLocalResource() || mImageUrl.IsBufferResource());

  // It will call SvgVisual::LoadComplete() synchronously if it required, or we already loaded same svg before.
  mSvgLoadId = mSvgLoader.Load(mImageUrl, meanDpi, this, synchronousLoading);
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
    else if(keyValue.first == IMAGE_ATLASING)
    {
      DoSetProperty(Toolkit::ImageVisual::Property::ATLASING, keyValue.second);
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
  }
}

void SvgVisual::DoSetProperty(Property::Index index, const Property::Value& value)
{
  switch(index)
  {
    case Toolkit::ImageVisual::Property::ATLASING:
    {
      bool atlasing = false;
      if(value.Get(atlasing))
      {
        mAttemptAtlasing = atlasing;
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
  }
}

void SvgVisual::DoSetOnScene(Actor& actor)
{
  TextureSet textureSet = TextureSet::New();
  mImpl->mRenderer.SetTextures(textureSet);

  // Register transform properties
  mImpl->mTransform.SetUniforms(mImpl->mRenderer, Direction::LEFT_TO_RIGHT);

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
  }
}

void SvgVisual::DoSetOffScene(Actor& actor)
{
  // Remove rasterizing task
  if(mSvgRasterizeId != SvgLoader::INVALID_SVG_LOAD_ID)
  {
    // We don't need to remove task synchronously.
    mSvgLoader.RequestRasterizeRemove(mSvgRasterizeId, this, false);
    mSvgRasterizeId = SvgLoader::INVALID_SVG_RASTERIZE_ID;
  }

  actor.RemoveRenderer(mImpl->mRenderer);
  mPlacementActor.Reset();

  // Reset the visual size so that when adding the actor back to stage the SVG rasterization is forced
  mRasterizedSize = -Vector2::ONE; ///< Let we don't use zero since visual size could be zero after trasnform
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
    map.Insert(Toolkit::ImageVisual::Property::ATLASING, mAttemptAtlasing);
  }
  map.Insert(Toolkit::ImageVisual::Property::SYNCHRONOUS_LOADING, IsSynchronousLoadingRequired());
  map.Insert(Toolkit::ImageVisual::Property::DESIRED_WIDTH, mDesiredSize.GetWidth());
  map.Insert(Toolkit::ImageVisual::Property::DESIRED_HEIGHT, mDesiredSize.GetHeight());
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

bool SvgVisual::AttemptAtlasing() const
{
  return (!mImpl->mCustomShader && (mImageUrl.IsLocalResource() || mImageUrl.IsBufferResource()) && mAttemptAtlasing);
}

void SvgVisual::AddRasterizationTask(const Vector2& size)
{
  if(mImpl->mRenderer)
  {
    // Remove previous task
    if(mSvgRasterizeId != SvgLoader::INVALID_SVG_LOAD_ID)
    {
      mSvgLoader.RequestRasterizeRemove(mSvgRasterizeId, this, true);
      mSvgRasterizeId = SvgLoader::INVALID_SVG_RASTERIZE_ID;
    }

    uint32_t width  = static_cast<uint32_t>(roundf(size.width));
    uint32_t height = static_cast<uint32_t>(roundf(size.height));

    const bool synchronousRasterize = IsSynchronousLoadingRequired() && (mImageUrl.IsLocalResource() || mImageUrl.IsBufferResource());
    const bool attemtAtlasing       = AttemptAtlasing();

    mSvgRasterizeId = mSvgLoader.Rasterize(mSvgLoadId, width, height, attemtAtlasing, this, synchronousRasterize);
  }
}

/// Called when SvgLoader::Load is completed.
void SvgVisual::LoadComplete(int32_t loadId, Dali::VectorImageRenderer vectorImageRenderer)
{
  // mSvgLoadId might not be updated if svg file is cached. Update now.
  mSvgLoadId = loadId;

  if(DALI_LIKELY(vectorImageRenderer))
  {
    vectorImageRenderer.GetDefaultSize(mDefaultWidth, mDefaultHeight);
  }
  else if(!mLoadFailed)
  {
    SvgVisualPtr self = this; // Keep reference until this API finished

    mLoadFailed = true;

    // Remove rasterizing task if we requested before.
    if(mSvgRasterizeId != SvgLoader::INVALID_SVG_LOAD_ID)
    {
      mSvgLoader.RequestRasterizeRemove(mSvgRasterizeId, this, true);
      mSvgRasterizeId = SvgLoader::INVALID_SVG_RASTERIZE_ID;
    }

    if(IsOnScene())
    {
      Actor actor = mPlacementActor.GetHandle();
      if(actor && mImpl->mRenderer)
      {
        Vector2 imageSize = Vector2::ZERO;
        imageSize         = actor.GetProperty(Actor::Property::SIZE).Get<Vector2>();
        mFactoryCache.UpdateBrokenImageRenderer(mImpl->mRenderer, imageSize);
        actor.AddRenderer(mImpl->mRenderer);
        // reset the weak handle so that the renderer only get added to actor once
        mPlacementActor.Reset();
      }

      // Emit failed signal only if this visual is scene-on
      ResourceReady(Toolkit::Visual::ResourceStatus::FAILED);
    }
  }
}

/// Called when SvgLoader::Rasterize is completed.
void SvgVisual::RasterizeComplete(int32_t rasterizeId, Dali::TextureSet textureSet, Vector4 atlasRect)
{
  // rasterize id might not be updated if rasterize is cached.
  mSvgRasterizeId = rasterizeId;

  if(DALI_LIKELY(textureSet))
  {
    bool updateShader = false;

    if(AttemptAtlasing() && atlasRect != FULL_TEXTURE_RECT)
    {
      mAtlasRect = atlasRect;
      if(DALI_UNLIKELY(!(mImpl->mFlags & Impl::IS_ATLASING_APPLIED)))
      {
        updateShader = true;
      }
      mImpl->mFlags |= Impl::IS_ATLASING_APPLIED;
    }
    else
    {
      mAtlasRect = FULL_TEXTURE_RECT;
      if(DALI_UNLIKELY(mImpl->mFlags & Impl::IS_ATLASING_APPLIED))
      {
        updateShader = true;
      }
      mImpl->mFlags &= ~Impl::IS_ATLASING_APPLIED;
    }

    if(DALI_LIKELY(mImpl->mRenderer))
    {
      TextureSet currentTextureSet = mImpl->mRenderer.GetTextures();

      if(mAtlasRectIndex == Property::INVALID_INDEX)
      {
        if(DALI_UNLIKELY(mAtlasRect != FULL_TEXTURE_RECT))
        {
          // Register atlas rect property only if it's not full texture rect.
          mAtlasRectIndex = mImpl->mRenderer.RegisterUniqueProperty(mAtlasRectIndex, ATLAS_RECT_UNIFORM_NAME, mAtlasRect);
        }
      }
      else
      {
        mImpl->mRenderer.SetProperty(mAtlasRectIndex, mAtlasRect);
      }

      if(textureSet != currentTextureSet)
      {
        mImpl->mRenderer.SetTextures(textureSet);
      }

      if(DALI_UNLIKELY(updateShader))
      {
        UpdateShader();
      }
    }

    if(IsOnScene())
    {
      SvgVisualPtr self = this; // Keep reference until this API finished

      // Rasterized pixels are uploaded to texture. If weak handle is holding a placement actor, it is the time to add the renderer to actor.
      Actor actor = mPlacementActor.GetHandle();
      if(actor)
      {
        actor.AddRenderer(mImpl->mRenderer);
        // reset the weak handle so that the renderer only get added to actor once
        mPlacementActor.Reset();
      }

      // Svg loaded and ready to display
      ResourceReady(Toolkit::Visual::ResourceStatus::READY);
    }
  }
  else if(!mLoadFailed)
  {
    SvgVisualPtr self = this; // Keep reference until this API finished

    mLoadFailed = true;

    if(IsOnScene())
    {
      Actor actor = mPlacementActor.GetHandle();
      if(actor && mImpl->mRenderer)
      {
        Vector2 imageSize = Vector2::ZERO;
        imageSize         = actor.GetProperty(Actor::Property::SIZE).Get<Vector2>();
        mFactoryCache.UpdateBrokenImageRenderer(mImpl->mRenderer, imageSize);
        actor.AddRenderer(mImpl->mRenderer);

        // reset the weak handle so that the renderer only get added to actor once
        mPlacementActor.Reset();
      }

      // Emit failed signal only if this visual is scene-on
      ResourceReady(Toolkit::Visual::ResourceStatus::FAILED);
    }
  }
}

void SvgVisual::OnSetTransform()
{
  if(mImpl->mRenderer)
  {
    mImpl->mTransform.SetUniforms(mImpl->mRenderer, Direction::LEFT_TO_RIGHT);
  }

  if(IsOnScene() && !mLoadFailed)
  {
    Vector2 size;
    if(mDesiredSize.GetWidth() > 0 && mDesiredSize.GetHeight() > 0)
    {
      // Use desired size
      size = Vector2(mDesiredSize.GetWidth(), mDesiredSize.GetHeight());
    }
    else
    {
      // Use visual size
      size = mImpl->mTransform.GetVisualSize(mImpl->mControlSize);
    }

    // roundf and change as integer scale.
    size.width  = static_cast<uint32_t>(roundf(size.width));
    size.height = static_cast<uint32_t>(roundf(size.height));

    if(size != mRasterizedSize)
    {
      mRasterizedSize = size;
      AddRasterizationTask(size);
    }
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
  if(!mImpl->mCustomShader)
  {
    shader = mImageVisualShaderFactory.GetShader(
      mFactoryCache,
      ImageVisualShaderFeatureBuilder()
        .EnableTextureAtlas(mImpl->mFlags & Visual::Base::Impl::IS_ATLASING_APPLIED)
        .EnableRoundedCorner(IsRoundedCornerRequired())
        .EnableBorderline(IsBorderlineRequired()));
  }
  else
  {
    shader = Shader::New(mImpl->mCustomShader->mVertexShader.empty() ? mImageVisualShaderFactory.GetVertexShaderSource().data() : mImpl->mCustomShader->mVertexShader,
                         mImpl->mCustomShader->mFragmentShader.empty() ? mImageVisualShaderFactory.GetFragmentShaderSource().data() : mImpl->mCustomShader->mFragmentShader,
                         mImpl->mCustomShader->mHints);

    shader.RegisterProperty(PIXEL_AREA_UNIFORM_NAME, FULL_TEXTURE_RECT);
  }
  return shader;
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
