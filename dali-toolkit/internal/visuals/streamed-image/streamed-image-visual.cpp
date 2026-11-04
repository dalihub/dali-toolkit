/*
 * Copyright (c) 2026 Samsung Electronics Co., Ltd.
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
#include <dali-toolkit/internal/visuals/streamed-image/streamed-image-visual.h>

// EXTERNAL INCLUDES
#include <dali/devel-api/rendering/texture-devel.h>
#include <dali/integration-api/rendering/decorated-visual-renderer.h>
#include <dali/integration-api/string-utils.h>
#include <dali/public-api/rendering/sampler.h>
#include <dali/public-api/rendering/texture-set.h>
#include <utility>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/graphics/builtin-shader-extern-gen.h>
#include <dali-toolkit/internal/visuals/image/image-visual-shader-feature-builder.h>
#include <dali-toolkit/internal/visuals/visual-base-data-impl.h>
#include <dali-toolkit/internal/visuals/visual-factory-cache.h>
#include <dali-toolkit/internal/visuals/visual-string-constants.h>

namespace DALI_NAMESPACE
{
namespace Toolkit
{
namespace Internal
{
namespace
{
constexpr auto SWAP_UV_DEFINE = "#define IS_REQUIRED_SWAP_UV\n";
} // namespace

StreamedImageVisualPtr StreamedImageVisual::New(VisualFactoryCache& factoryCache, const Frame& frame)
{
  DALI_ASSERT_ALWAYS(frame.texture && "StreamedImageVisual requires a frame texture");
  StreamedImageVisualPtr visual(new StreamedImageVisual(factoryCache, frame));
  visual->Initialize();
  return visual;
}

StreamedImageVisual::StreamedImageVisual(VisualFactoryCache& factoryCache, const Frame& frame)
: Visual::Base(factoryCache, Visual::FittingMode::FIT_KEEP_ASPECT_RATIO, static_cast<Toolkit::Visual::Type>(Toolkit::DevelVisual::STREAMED_IMAGE), Visual::DECORATION_CORNER_RADIUS | Visual::DECORATION_BORDERLINE),
  mFrame(frame)
{
  UpdateCoordinates();
}

StreamedImageVisual::~StreamedImageVisual() = default;

void StreamedImageVisual::SetFrame(const Frame& frame)
{
  DALI_ASSERT_ALWAYS(frame.texture && "StreamedImageVisual requires a frame texture");
  const bool textureChanged     = mFrame.texture != frame.texture;
  const bool nativenessInvolved = textureChanged && (DevelTexture::IsNative(mFrame.texture) || DevelTexture::IsNative(frame.texture));
  const bool previousSwapAxes   = mSwapAxes;

  mFrame = frame;
  UpdateCoordinates();

  if(mImpl->mRenderer)
  {
    if(mPixelAreaIndex != Property::INVALID_INDEX)
    {
      mImpl->mRenderer.SetProperty(mPixelAreaIndex, mPixelArea);
    }
    if(textureChanged)
    {
      mImpl->mRenderer.GetTextures().SetTexture(0u, frame.texture);
    }
    if(previousSwapAxes != mSwapAxes || nativenessInvolved)
    {
      ApplyShaderIfChanged();
    }
  }
}

const StreamedImageVisual::Frame& StreamedImageVisual::GetFrame() const
{
  return mFrame;
}

void StreamedImageVisual::GetNaturalSize(Vector2& naturalSize)
{
  const Vector2 croppedSize(mFrame.contentSize.width * mFrame.crop.width, mFrame.contentSize.height * mFrame.crop.height);
  naturalSize = mSwapAxes ? Vector2(croppedSize.height, croppedSize.width) : croppedSize;
}

void StreamedImageVisual::OnInitialize()
{
  Geometry geometry = mFactoryCache.GetGeometry(VisualFactoryCache::QUAD_GEOMETRY);
  BuildShaderSources(mVertexShader, mFragmentShader);
  Shader shader    = CreateShader(mVertexShader, mFragmentShader);
  mImpl->mRenderer = DecoratedVisualRenderer::New(geometry, shader);

  TextureSet textures = TextureSet::New();
  textures.SetTexture(0u, mFrame.texture);
  Sampler sampler = Sampler::New();
  sampler.SetFilterMode(FilterMode::LINEAR, FilterMode::LINEAR);
  sampler.SetWrapMode(WrapMode::CLAMP_TO_EDGE, WrapMode::CLAMP_TO_EDGE);
  textures.SetSampler(0u, sampler);
  mImpl->mRenderer.SetTextures(textures);

  mPixelAreaIndex = mImpl->mRenderer.RegisterProperty(PIXEL_AREA_UNIFORM_NAME, mPixelArea);
  EnablePreMultipliedAlpha(false);
  mImpl->mRenderer.RegisterProperty(PREMULTIPLIED_ALPHA, 0.0f);
  mImpl->SetTransformUniforms(mImpl->mRenderer, Direction::LEFT_TO_RIGHT);
}

void StreamedImageVisual::DoCreatePropertyMap(Property::Map& map) const
{
  map.Clear();
  map.Insert(Toolkit::Visual::Property::TYPE, Toolkit::DevelVisual::STREAMED_IMAGE);
}

void StreamedImageVisual::DoCreateInstancePropertyMap(Property::Map& /*map*/) const
{
}

void StreamedImageVisual::DoSetProperties(const Property::Map& /*propertyMap*/)
{
  // The frame cannot be expressed as properties; generic transform, color and
  // decoration properties are owned by Visual::Base.
}

void StreamedImageVisual::OnSetTransform()
{
  if(mImpl->mRenderer && mImpl->mTransformMapChanged)
  {
    mImpl->SetTransformUniforms(mImpl->mRenderer, Direction::LEFT_TO_RIGHT);
  }
}

void StreamedImageVisual::DoSetOnScene(Actor& actor)
{
  actor.AddRenderer(mImpl->mRenderer);
  ResourceReady(Toolkit::Visual::ResourceStatus::READY);
}

void StreamedImageVisual::DoSetOffScene(Actor& actor)
{
  actor.RemoveRenderer(mImpl->mRenderer);
}

void StreamedImageVisual::UpdateShader()
{
  if(mImpl->mRenderer)
  {
    BuildShaderSources(mVertexShader, mFragmentShader);
    Shader shader = CreateShader(mVertexShader, mFragmentShader);
    mImpl->mRenderer.SetShader(shader);
  }
}

Dali::Shader StreamedImageVisual::GenerateShader() const
{
  std::string vertexShader;
  std::string fragmentShader;
  BuildShaderSources(vertexShader, fragmentShader);
  return CreateShader(vertexShader, fragmentShader);
}

void StreamedImageVisual::BuildShaderSources(std::string& vertexShader, std::string& fragmentShader) const
{
  ImageVisualShaderFeature::FeatureBuilder features;
  features.EnableRoundedCorner(IsRoundedCornerRequired(), IsSquircleCornerRequired())
    .EnableBorderline(IsBorderlineRequired())
    .ApplyDefaultTextureWrapMode(true);

  std::string vertexPrefix;
  std::string fragmentPrefix;
  features.GetVertexShaderPrefixList(vertexPrefix);
  features.GetFragmentShaderPrefixList(fragmentPrefix);
  if(mSwapAxes)
  {
    vertexPrefix += SWAP_UV_DEFINE;
  }

  vertexShader   = std::string(vertexPrefix + SHADER_IMAGE_VISUAL_SHADER_VERT.data());
  fragmentShader = std::string(fragmentPrefix + SHADER_IMAGE_VISUAL_SHADER_FRAG.data());
  DevelTexture::ApplyNativeFragmentShader(mFrame.texture, fragmentShader, 1);
}

Dali::Shader StreamedImageVisual::CreateShader(const std::string& vertexShader, const std::string& fragmentShader) const
{
  Shader shader = Shader::New(Dali::Integration::ToDaliStringView(vertexShader), Dali::Integration::ToDaliStringView(fragmentShader));
  shader.RegisterProperty(PIXEL_AREA_UNIFORM_NAME, Vector4(0.0f, 0.0f, 1.0f, 1.0f));
  shader.RegisterProperty(PREMULTIPLIED_ALPHA, 1.0f);
  return shader;
}

void StreamedImageVisual::ApplyShaderIfChanged()
{
  std::string vertexShader;
  std::string fragmentShader;
  BuildShaderSources(vertexShader, fragmentShader);
  if(vertexShader != mVertexShader || fragmentShader != mFragmentShader)
  {
    mVertexShader   = std::move(vertexShader);
    mFragmentShader = std::move(fragmentShader);
    Shader shader   = CreateShader(mVertexShader, mFragmentShader);
    mImpl->mRenderer.SetShader(shader);
  }
}

Vector2 StreamedImageVisual::MapToSourceCoordinates(Vector2 point) const
{
  if(mFrame.horizontalFlip)
  {
    point.x = 1.0f - point.x;
  }
  if(mFrame.verticalFlip)
  {
    point.y = 1.0f - point.y;
  }
  switch(mFrame.rotation)
  {
    case Rotation::DEGREE_90:
    {
      point = Vector2(point.y, 1.0f - point.x);
      break;
    }
    case Rotation::DEGREE_180:
    {
      point = Vector2(1.0f - point.x, 1.0f - point.y);
      break;
    }
    case Rotation::DEGREE_270:
    {
      point = Vector2(1.0f - point.y, point.x);
      break;
    }
    case Rotation::DEGREE_0:
    {
      break;
    }
  }
  const auto& crop = mFrame.crop;
  return Vector2(crop.x + point.x * crop.width, crop.y + point.y * crop.height);
}

void StreamedImageVisual::UpdateCoordinates()
{
  mSwapAxes            = mFrame.rotation == Rotation::DEGREE_90 || mFrame.rotation == Rotation::DEGREE_270;
  const Vector2 origin = MapToSourceCoordinates(Vector2::ZERO);
  const Vector2 xAxis  = MapToSourceCoordinates(Vector2(1.0f, 0.0f)) - origin;
  const Vector2 yAxis  = MapToSourceCoordinates(Vector2(0.0f, 1.0f)) - origin;
  mPixelArea           = Vector4(origin.x, origin.y, mSwapAxes ? yAxis.x : xAxis.x, mSwapAxes ? xAxis.y : yAxis.y);
}

} // namespace Internal
} // namespace Toolkit
} // namespace DALI_NAMESPACE
