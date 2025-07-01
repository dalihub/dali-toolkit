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
#include "gaussian-blur-algorithm.h"

// EXTERNAL INCLUDES
#include <dali/integration-api/shader-integ.h>
#include <cstdint>
#include <random>
#include <vector>

namespace
{
static constexpr float MINIMUM_DOWNSCALE_FACTOR = Dali::Math::MACHINE_EPSILON_1000;
static constexpr float MAXIMUM_DOWNSCALE_FACTOR = 1.0f;

static constexpr uint32_t MAXIMUM_BLUR_RADIUS       = 500u; ///< Maximum pixel radius for blur effect. (GL_MAX_FRAGMENT_UNIFORM_COMPONENTS(Usually 1024) - 19 (vertex shader used)) / 3 float
static constexpr uint32_t MAXIMUM_NUMBER_OF_SAMPLES = (MAXIMUM_BLUR_RADIUS >> 1);

static constexpr float   MAXIMUM_BELL_CURVE_WIDTH            = 171.352f; ///< bell curve width for MAXIMUM_BLUR_RADIUS case
static constexpr int32_t MAXIMUM_BELL_CURVE_LOOP_TRIAL_COUNT = 50;

/**
* @brief Calculates gaussian weight
* @param[in] localOffset Input variable of gaussian distribution
* @param[in] sigma Standard deviation of gaussian distribution, the width of the "bell"
* @note Expected value of this gaussian distribution is 0.
*/
inline static float CalculateGaussianWeight(float localOffset, float sigma)
{
  return (1.0f / (sigma * sqrt(2.0f * Dali::Math::PI))) * exp(-0.5f * (localOffset / sigma * localOffset / sigma));
}

/**
* @brief Calculates gaussian bell curve width from given radius.
* Appropriate bell curve width lets gaussian bell curve reside in a valid range(with non-tail, distinguishable values).
* @param[in] blurRadius Given radius value of gaussian bell curve.
* @return Bell curve width of gaussian bell curve.
*/
float CalculateBellCurveWidth(uint32_t blurRadius)
{
  const float epsilon     = 1e-2f / (blurRadius * 2);
  const float localOffset = (blurRadius * 2) - 1;

  float lowerBoundBellCurveWidth = Dali::Math::MACHINE_EPSILON_10000;
  float upperBoundBellCurveWidth = MAXIMUM_BELL_CURVE_WIDTH;

  float bellCurveWidth = -1.0f;

  int trialCount = 0;
  while(trialCount++ < MAXIMUM_BELL_CURVE_LOOP_TRIAL_COUNT && upperBoundBellCurveWidth - lowerBoundBellCurveWidth > Dali::Math::MACHINE_EPSILON_10000)
  {
    bellCurveWidth = (lowerBoundBellCurveWidth + upperBoundBellCurveWidth) * 0.5f;
    if(CalculateGaussianWeight(localOffset, bellCurveWidth) < epsilon)
    {
      lowerBoundBellCurveWidth = bellCurveWidth;
    }
    else
    {
      upperBoundBellCurveWidth = bellCurveWidth;
    }
  }
  return bellCurveWidth;
}

/**
* @brief Fills in gaussian kernel vectors of a specific size. Uses 4*numSamples+1 sized gaussian bell curve for sampling.
* @param[in] numSamples Number of samples. Note that we sample half size of given blur radius, and the curve is symmetric.
* @param[out] weights Samples from a gaussian bell curve.
* @param[out] offsets Adjacent pixel offsets to apply gaussian weights.
*/
void CalculateGaussianConstants(uint32_t numSamples, std::vector<float>& weights, std::vector<float>& offsets)
{
  const float bellCurveWidth = CalculateBellCurveWidth(numSamples);

  const uint32_t kernelSize     = numSamples * 4 - 1;
  const uint32_t halfKernelSize = kernelSize / 2 + 1; // Gaussian curve is symmetric

  // Generate half size kernel
  std::vector<float> halfSideKernel(halfKernelSize);

  halfSideKernel[0]  = CalculateGaussianWeight(0.0f, bellCurveWidth);
  float totalWeights = halfSideKernel[0];
  for(unsigned int i = 1; i < halfKernelSize; i++)
  {
    float w           = CalculateGaussianWeight(i, bellCurveWidth);
    halfSideKernel[i] = w;
    totalWeights += w * 2.0f;
  }
  for(unsigned int i = 0; i < halfKernelSize; i++)
  {
    halfSideKernel[i] /= totalWeights;
  }
  halfSideKernel[0] *= 0.5f;

  weights.clear();
  weights.resize(numSamples);
  offsets.clear();
  offsets.resize(numSamples);

  // Compress kernel to half size
  for(unsigned int i = 0; i < numSamples; i++)
  {
    weights[i] = halfSideKernel[2 * i] + halfSideKernel[2 * i + 1];
    offsets[i] = 2.0f * i + halfSideKernel[2 * i + 1] / weights[i];
  }
}

/**
 * @brief Retrieves precalculated UniformBlock of gaussian kernel. If none, creates new UniformBlock.
 * @param[in] numSamples Number of samples.
 * @return UniformBlock for SHADER_BLUR_EFFECT_FRAG
 */
inline static Dali::UniformBlock& GetCachedUniformBlock(const uint32_t numSamples)
{
  static Dali::UniformBlock gPredefinedUniformBlock[MAXIMUM_NUMBER_OF_SAMPLES + 1u];
  DALI_ASSERT_DEBUG(numSamples <= MAXIMUM_NUMBER_OF_SAMPLES && "numSamples too big!");
  return gPredefinedUniformBlock[numSamples];
}

/**
 * @brief Retrieves precalculated fragment shader of gaussian kernel. If none, creates new shader.
 * @param[in] numSamples Number of samples.
 * @return Fragment shader of gaussian blur.
 */
inline static Dali::Shader& GetCachedShader(const uint32_t numSamples)
{
  static Dali::Shader gPredefinedShader[MAXIMUM_NUMBER_OF_SAMPLES + 1u];
  DALI_ASSERT_DEBUG(numSamples <= MAXIMUM_NUMBER_OF_SAMPLES && "numSamples too big!");
  return gPredefinedShader[numSamples];
}

} // namespace

namespace Dali
{
namespace Toolkit
{
namespace Internal
{
Dali::Renderer GaussianBlurAlgorithm::CreateRenderer(const uint32_t blurRadius)
{
  Dali::Renderer renderer = Dali::Toolkit::Internal::CreateRenderer("", "");
  renderer.SetShader(GetGaussianBlurShader(blurRadius));
  renderer.SetProperty(Renderer::Property::BLEND_PRE_MULTIPLIED_ALPHA, true); // Always use premultiplied alpha
  return renderer;
}

Dali::Shader& GaussianBlurAlgorithm::GetGaussianBlurShader(const uint32_t blurRadius)
{
  uint32_t numSamples = blurRadius >> 1;

  auto& cachedShader = GetCachedShader(numSamples);
  if(!cachedShader)
  {
    auto& cachedUniformBlock = GetCachedUniformBlock(numSamples);
    if(!cachedUniformBlock)
    {
      std::vector<float> weights;
      std::vector<float> offsets;
      CalculateGaussianConstants(numSamples, weights, offsets);

      Dali::UniformBlock sharedUBO = Dali::UniformBlock::New("GaussianBlurSampleBlock");

      for(uint32_t i = 0; i < numSamples; i++)
      {
        {
          std::stringstream oss;
          oss << "uSampleOffsets[" << i << "]";
          sharedUBO.RegisterProperty(oss.str(), offsets[i]);
        }
        {
          std::stringstream oss;
          oss << "uSampleWeights[" << i << "]";
          sharedUBO.RegisterProperty(oss.str(), weights[i]);
        }
      }
      cachedUniformBlock = sharedUBO;
    }

    std::ostringstream shaderNameBuilder;
    shaderNameBuilder << "GaussianBlurShader_" << numSamples;

    std::ostringstream fragmentStringStream;
    fragmentStringStream << "#define NUM_SAMPLES " << numSamples << "\n";
    fragmentStringStream << SHADER_BLUR_EFFECT_FRAG;
    std::string fragmentSource(fragmentStringStream.str());

    cachedShader = Dali::Integration::ShaderNewWithUniformBlock(BASIC_VERTEX_SOURCE, fragmentSource.c_str(), Dali::Shader::Hint::FILE_CACHE_SUPPORT, shaderNameBuilder.str(), {cachedUniformBlock});
  }
  return cachedShader;
}

uint32_t GaussianBlurAlgorithm::GetDownscaledBlurRadius(float& downscaleFactor, uint32_t& blurRadius)
{
  downscaleFactor = Dali::Clamp(downscaleFactor, MINIMUM_DOWNSCALE_FACTOR, MAXIMUM_DOWNSCALE_FACTOR);

  if(DALI_UNLIKELY(blurRadius > MAXIMUM_BLUR_RADIUS))
  {
    const uint32_t fixedBlurRadius      = MAXIMUM_BLUR_RADIUS;
    const float    fixedDownScaleFactor = Dali::Clamp(
      downscaleFactor * static_cast<float>(fixedBlurRadius) / static_cast<float>(blurRadius),
      MINIMUM_DOWNSCALE_FACTOR,
      MAXIMUM_DOWNSCALE_FACTOR);

    DALI_LOG_ERROR("Blur radius is out of bound: %u. Use %u and make downscale factor %f to %f.\n",
                   blurRadius,
                   fixedBlurRadius,
                   downscaleFactor,
                   fixedDownScaleFactor);

    downscaleFactor = fixedDownScaleFactor;
    blurRadius      = fixedBlurRadius;
  }
  return static_cast<uint32_t>(blurRadius * downscaleFactor);
}

} // namespace Internal
} // namespace Toolkit
} // namespace Dali
