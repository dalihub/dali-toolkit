/*
 * Copyright (c) 2022 Samsung Electronics Co., Ltd.
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
#include "svg-visual.h"

// INTERNAL INCLUDES
#include <dali-toolkit/internal/visuals/image-atlas-manager.h>
#include <dali-toolkit/internal/visuals/image-visual-shader-factory.h>
#include <dali-toolkit/internal/visuals/svg/svg-rasterize-thread.h>
#include <dali-toolkit/internal/visuals/visual-base-data-impl.h>
#include <dali-toolkit/internal/visuals/visual-string-constants.h>
#include <dali-toolkit/public-api/visuals/image-visual-properties.h>

// EXTERNAL INCLUDES
#include <dali/devel-api/common/stage.h>
#include <dali/integration-api/debug.h>

namespace Dali
{
namespace Toolkit
{
namespace Internal
{
namespace
{
const int CUSTOM_PROPERTY_COUNT(6); // atlas + corner/border

// property name
const Dali::Vector4 FULL_TEXTURE_RECT(0.f, 0.f, 1.f, 1.f);

} // namespace

SvgVisualPtr SvgVisual::New(VisualFactoryCache& factoryCache, ImageVisualShaderFactory& shaderFactory, const VisualUrl& imageUrl, const Property::Map& properties)
{
  SvgVisualPtr svgVisual(new SvgVisual(factoryCache, shaderFactory, imageUrl));
  svgVisual->SetProperties(properties);
  svgVisual->Initialize();
  return svgVisual;
}

SvgVisualPtr SvgVisual::New(VisualFactoryCache& factoryCache, ImageVisualShaderFactory& shaderFactory, const VisualUrl& imageUrl)
{
  SvgVisualPtr svgVisual(new SvgVisual(factoryCache, shaderFactory, imageUrl));
  svgVisual->Initialize();
  return svgVisual;
}

SvgVisual::SvgVisual(VisualFactoryCache& factoryCache, ImageVisualShaderFactory& shaderFactory, const VisualUrl& imageUrl)
: Visual::Base(factoryCache, Visual::FittingMode::FILL, Toolkit::Visual::SVG),
  mImageVisualShaderFactory(shaderFactory),
  mAtlasRect(FULL_TEXTURE_RECT),
  mImageUrl(imageUrl),
  mVectorRenderer(VectorImageRenderer::New()),
  mDefaultWidth(0),
  mDefaultHeight(0),
  mPlacementActor(),
  mRasterizedSize(Vector2::ZERO),
  mLoadFailed(false),
  mAttemptAtlasing(false)
{
  // the rasterized image is with pre-multiplied alpha format
  mImpl->mFlags |= Impl::IS_PREMULTIPLIED_ALPHA;
}

SvgVisual::~SvgVisual()
{
}

void SvgVisual::OnInitialize()
{
  Shader   shader   = GenerateShader();
  Geometry geometry = mFactoryCache.GetGeometry(VisualFactoryCache::QUAD_GEOMETRY);
  mImpl->mRenderer  = VisualRenderer::New(geometry, shader);
  mImpl->mRenderer.ReserveCustomProperties(CUSTOM_PROPERTY_COUNT);
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
  }
}

void SvgVisual::DoSetProperty(Property::Index index, const Property::Value& value)
{
  switch(index)
  {
    case Toolkit::ImageVisual::Property::ATLASING:
    {
      value.Get(mAttemptAtlasing);
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
  mFactoryCache.GetSVGRasterizationThread()->RemoveTask(this);

  actor.RemoveRenderer(mImpl->mRenderer);
  mPlacementActor.Reset();

  // Reset the visual size to zero so that when adding the actor back to stage the SVG rasterization is forced
  mRasterizedSize = Vector2::ZERO;
}

void SvgVisual::GetNaturalSize(Vector2& naturalSize)
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

void SvgVisual::AddRasterizationTask(const Vector2& size)
{
  if(mImpl->mRenderer)
  {
    unsigned int width  = static_cast<unsigned int>(size.width);
    unsigned int height = static_cast<unsigned int>(size.height);

    Vector2 dpi     = Stage::GetCurrent().GetDpi();
    float   meanDpi = (dpi.height + dpi.width) * 0.5f;

    RasterizingTaskPtr newTask = new RasterizingTask(this, mVectorRenderer, mImageUrl, meanDpi, width, height);

    if(IsSynchronousLoadingRequired() && mImageUrl.IsLocalResource())
    {
      newTask->Load();
      newTask->Rasterize();
      ApplyRasterizedImage(newTask->GetPixelData(), newTask->IsLoaded());
    }
    else
    {
      mFactoryCache.GetSVGRasterizationThread()->AddTask(newTask);
    }
  }
}

void SvgVisual::ApplyRasterizedImage(PixelData rasterizedPixelData, bool isLoaded)
{
  if(isLoaded && rasterizedPixelData && IsOnScene())
  {
    if(mDefaultWidth == 0 || mDefaultHeight == 0)
    {
      mVectorRenderer.GetDefaultSize(mDefaultWidth, mDefaultHeight);
    }

    mRasterizedSize.x = static_cast<float>(rasterizedPixelData.GetWidth());
    mRasterizedSize.y = static_cast<float>(rasterizedPixelData.GetHeight());

    TextureSet currentTextureSet = mImpl->mRenderer.GetTextures();
    if(mImpl->mFlags & Impl::IS_ATLASING_APPLIED)
    {
      mFactoryCache.GetAtlasManager()->Remove(currentTextureSet, mAtlasRect);
    }

    TextureSet textureSet;

    if(mAttemptAtlasing && !mImpl->mCustomShader)
    {
      Vector4 atlasRect;
      textureSet = mFactoryCache.GetAtlasManager()->Add(atlasRect, rasterizedPixelData);
      if(textureSet) // atlasing
      {
        if(textureSet != currentTextureSet)
        {
          mImpl->mRenderer.SetTextures(textureSet);
        }
        mImpl->mRenderer.RegisterProperty(ATLAS_RECT_UNIFORM_NAME, atlasRect);
        mAtlasRect = atlasRect;
        mImpl->mFlags |= Impl::IS_ATLASING_APPLIED;
      }
    }

    if(!textureSet) // no atlasing - mAttemptAtlasing is false or adding to atlas is failed
    {
      Texture texture = Texture::New(Dali::TextureType::TEXTURE_2D, Pixel::RGBA8888, rasterizedPixelData.GetWidth(), rasterizedPixelData.GetHeight());
      texture.Upload(rasterizedPixelData);
      mImpl->mFlags &= ~Impl::IS_ATLASING_APPLIED;

      if(mAtlasRect == FULL_TEXTURE_RECT)
      {
        textureSet = currentTextureSet;
      }
      else
      {
        textureSet = TextureSet::New();
        mImpl->mRenderer.SetTextures(textureSet);

        mImpl->mRenderer.RegisterProperty(ATLAS_RECT_UNIFORM_NAME, FULL_TEXTURE_RECT);
        mAtlasRect = FULL_TEXTURE_RECT;
      }

      if(textureSet)
      {
        textureSet.SetTexture(0, texture);
      }
    }

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
  else if(!isLoaded || !rasterizedPixelData)
  {
    mLoadFailed = true;

    Actor actor = mPlacementActor.GetHandle();
    if(actor)
    {
      Vector2 imageSize = Vector2::ZERO;
      imageSize         = actor.GetProperty(Actor::Property::SIZE).Get<Vector2>();
      mFactoryCache.UpdateBrokenImageRenderer(mImpl->mRenderer, imageSize);
      actor.AddRenderer(mImpl->mRenderer);
    }

    ResourceReady(Toolkit::Visual::ResourceStatus::FAILED);
  }
}

void SvgVisual::OnSetTransform()
{
  Vector2 visualSize = mImpl->mTransform.GetVisualSize(mImpl->mControlSize);

  if(IsOnScene() && !mLoadFailed)
  {
    if(visualSize != mRasterizedSize || mDefaultWidth == 0 || mDefaultHeight == 0)
    {
      mRasterizedSize = visualSize;
      AddRasterizationTask(visualSize);
    }
  }

  if(mImpl->mRenderer)
  {
    mImpl->mTransform.SetUniforms(mImpl->mRenderer, Direction::LEFT_TO_RIGHT);
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
      ImageVisualShaderFeature::FeatureBuilder()
        .EnableTextureAtlas(mAttemptAtlasing)
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
