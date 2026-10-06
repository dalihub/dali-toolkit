#ifndef DALI_TOOLKIT_INTERNAL_STREAMED_IMAGE_VISUAL_H
#define DALI_TOOLKIT_INTERNAL_STREAMED_IMAGE_VISUAL_H

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

// EXTERNAL INCLUDES
#include <dali/public-api/common/intrusive-ptr.h>
#include <dali/public-api/math/rect.h>
#include <dali/public-api/rendering/texture.h>
#include <cstdint>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/visuals/visual-base-impl.h>

namespace DALI_NAMESPACE
{
namespace Toolkit
{
namespace Internal
{
class StreamedImageVisual;
typedef IntrusivePtr<StreamedImageVisual> StreamedImageVisualPtr;

/**
 * @brief Renders frames pushed by an external producer.
 *
 * The visual never loads anything itself: the owner supplies every frame,
 * including its texture, and is responsible for the texture's lifetime.
 * It reports Toolkit::DevelVisual::STREAMED_IMAGE and supports the corner
 * radius and borderline decorations.
 */
class StreamedImageVisual : public Visual::Base
{
public:
  /**
   * @brief The clockwise rotation applied when presenting the cropped source.
   */
  enum class Rotation : uint8_t
  {
    DEGREE_0,
    DEGREE_90,
    DEGREE_180,
    DEGREE_270
  };

  /**
   * @brief One frame and how to present it.
   *
   * The source is cropped, rotated clockwise, then reflected on the display axes.
   */
  struct Frame
  {
    Dali::Texture texture;                      ///< The frame texture. Must be valid.
    Vector2       contentSize;                  ///< The frame's presented size in pixels, in unrotated source orientation, excluding any texture padding.
    Rect<float>   crop{0.0f, 0.0f, 1.0f, 1.0f}; ///< The displayed region, normalized to the texture, in unrotated source coordinates.
    Rotation      rotation{Rotation::DEGREE_0}; ///< The clockwise rotation of the presented content.
    bool          horizontalFlip{false};        ///< Whether to reflect the rotated content across its vertical axis.
    bool          verticalFlip{false};          ///< Whether to reflect the rotated content across its horizontal axis.
  };

  /**
   * @brief Creates a visual presenting a first frame.
   *
   * @param[in] factoryCache The shared visual factory cache
   * @param[in] frame The first frame; its texture must be valid
   * @return The initialized visual
   */
  static StreamedImageVisualPtr New(VisualFactoryCache& factoryCache, const Frame& frame);

  /**
   * @brief Presents a new frame on the event thread.
   *
   * The renderer keeps its shader unless the UV swap or the native fragment
   * adaptation actually changes with the new frame.
   *
   * @param[in] frame The frame to present; its texture must be valid
   */
  void SetFrame(const Frame& frame);

  /**
   * @brief Gets the frame being presented.
   *
   * @return The current frame
   */
  const Frame& GetFrame() const;

  /**
   * @copydoc Visual::Base::GetNaturalSize
   */
  void GetNaturalSize(Vector2& naturalSize) override;

protected:
  /**
   * @brief Initializes the presentation state before renderer creation.
   *
   * @param[in] factoryCache The shared visual factory cache
   * @param[in] frame The first frame
   */
  StreamedImageVisual(VisualFactoryCache& factoryCache, const Frame& frame);

  /**
   * @brief A reference counted object may only be deleted by calling Unreference().
   */
  ~StreamedImageVisual() override;

  /**
   * @copydoc Visual::Base::OnInitialize
   */
  void OnInitialize() override;

  /**
   * @copydoc Visual::Base::DoCreatePropertyMap
   */
  void DoCreatePropertyMap(Property::Map& map) const override;

  /**
   * @copydoc Visual::Base::DoCreateInstancePropertyMap
   */
  void DoCreateInstancePropertyMap(Property::Map& map) const override;

  /**
   * @copydoc Visual::Base::DoSetProperties
   */
  void DoSetProperties(const Property::Map& propertyMap) override;

  /**
   * @copydoc Visual::Base::OnSetTransform
   */
  void OnSetTransform() override;

  /**
   * @copydoc Visual::Base::DoSetOnScene
   */
  void DoSetOnScene(Actor& actor) override;

  /**
   * @copydoc Visual::Base::DoSetOffScene
   */
  void DoSetOffScene(Actor& actor) override;

  /**
   * @copydoc Visual::Base::UpdateShader
   */
  void UpdateShader() override;

  /**
   * @copydoc Visual::Base::GenerateShader
   */
  Dali::Shader GenerateShader() const override;

private:
  /**
   * @brief Maps one display coordinate back to the cropped source.
   *
   * @param[in] point A normalized display coordinate
   * @return The corresponding normalized source coordinate
   */
  Vector2 MapToSourceCoordinates(Vector2 point) const;

  /**
   * @brief Updates the signed pixel area and the UV swap state from the current frame.
   */
  void UpdateCoordinates();

  /**
   * @brief Builds the shader sources for the current frame and decorations.
   *
   * @param[out] vertexShader The vertex shader source
   * @param[out] fragmentShader The fragment shader source, after native fragment adaptation
   */
  void BuildShaderSources(std::string& vertexShader, std::string& fragmentShader) const;

  /**
   * @brief Creates a shader from already built sources.
   *
   * @param[in] vertexShader The vertex shader source
   * @param[in] fragmentShader The fragment shader source
   * @return The new shader
   */
  Dali::Shader CreateShader(const std::string& vertexShader, const std::string& fragmentShader) const;

  /**
   * @brief Rebuilds the shader sources and replaces the renderer's shader only when they differ.
   */
  void ApplyShaderIfChanged();

  StreamedImageVisual(const StreamedImageVisual&)            = delete;
  StreamedImageVisual& operator=(const StreamedImageVisual&) = delete;
  StreamedImageVisual(StreamedImageVisual&&)                 = delete;
  StreamedImageVisual& operator=(StreamedImageVisual&&)      = delete;

  Frame           mFrame;
  std::string     mVertexShader;   ///< The source the renderer's shader was built from.
  std::string     mFragmentShader; ///< The adapted fragment source the renderer's shader was built from.
  Vector4         mPixelArea{0.0f, 0.0f, 1.0f, 1.0f};
  Property::Index mPixelAreaIndex{Property::INVALID_INDEX};
  bool            mSwapAxes{false};
};

} // namespace Internal
} // namespace Toolkit
} // namespace DALI_NAMESPACE

#endif // DALI_TOOLKIT_INTERNAL_STREAMED_IMAGE_VISUAL_H
