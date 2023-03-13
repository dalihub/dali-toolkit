#ifndef DALI_TOOLKIT_INTERNAL_IMAGE_VIEW_H
#define DALI_TOOLKIT_INTERNAL_IMAGE_VIEW_H

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

// EXTERNAL INCLUDES
#include <dali/public-api/object/property-map.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/visuals/image/image-visual.h>
#include <dali-toolkit/public-api/controls/control-impl.h>
#include <dali-toolkit/public-api/controls/image-view/image-view.h>

namespace Dali
{
namespace Toolkit
{
class ImageView;

namespace Internal
{
class ImageView : public Control
{
protected:
  /**
   * Construct a new ImageView.
   *
   * @param[in] additionalBehaviour additional behaviour flags for this ImageView
   */
  ImageView(ControlBehaviour additionalBehaviour);

  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~ImageView();

public:
  /**
   * Create a new ImageView.
   *
   * @param[in] additionalBehaviour custom behavior flags for this ImageView. Default is CONTROL_BEHAVIOUR_DEFAULT
   * @return A smart-pointer to the newly allocated ImageView.
   */
  static Toolkit::ImageView New(ControlBehaviour additionalBehaviour = ControlBehaviour::CONTROL_BEHAVIOUR_DEFAULT);

  /**
   * @brief Sets this ImageView from an Dali::Property::Map
   *
   * If the handle is empty, ImageView will display nothing
   * @param[in] map The Dali::Property::Map to use for to display.
   */
  void SetImage(const Dali::Property::Map& map);

  /**
   * @copydoc Dali::Toolkit::SetImage
   */
  void SetImage(const std::string& imageUrl, ImageDimensions size);

  /**
   * @brief Unregister ImageView IMAGE visual
   *
   * ImageView will display nothing
   */
  void ClearImageVisual();

  /**
   * @brief Set whether the Pre-multiplied Alpha Blending is required
   *
   * @param[in] preMultipled whether alpha is pre-multiplied.
   */
  void EnablePreMultipliedAlpha(bool preMultipled);

  /**
   * @brief Query whether alpha is pre-multiplied.
   *
   * @return True if alpha is pre-multiplied, false otherwise.
   */
  bool IsPreMultipliedAlphaEnabled() const;

  // Properties
  /**
   * Called when a property of an object of this type is set.
   * @param[in] object The object whose property is set.
   * @param[in] index The property index.
   * @param[in] value The new property value.
   */
  static void SetProperty(BaseObject* object, Property::Index index, const Property::Value& value);

  /**
   * Called to retrieve a property of an object of this type.
   * @param[in] object The object whose property is to be retrieved.
   * @param[in] index The property index.
   * @return The current value of the property.
   */
  static Property::Value GetProperty(BaseObject* object, Property::Index propertyIndex);

  /**
   * @brief Set the depth index of this image renderer
   *
   * Renderer with higher depth indices are rendered in front of other visuals with smaller values
   *
   * @param[in] depthIndex The depth index of this renderer
   */
  void SetDepthIndex(int depthIndex);

private: // From Control
  /**
   * @copydoc Toolkit::Control::OnInitialize
   */
  void OnInitialize();

  /**
   * @copydoc Toolkit::Control::GetNaturalSize
   */
  Vector3 GetNaturalSize() override;

  /**
   * @copydoc Toolkit::Control::GetHeightForWidth()
   */
  float GetHeightForWidth(float width) override;

  /**
   * @copydoc Toolkit::Control::GetWidthForHeight()
   */
  float GetWidthForHeight(float height) override;

  /**
   * @copydoc Toolkit::Control::OnRelayout()
   */
  void OnRelayout(const Vector2& size, RelayoutContainer& container) override;

  /**
   * @copydoc Toolkit::Control::OnCreateTransitions()
   */
  virtual void OnCreateTransitions(std::vector<std::pair<Dali::Property::Index, Dali::Property::Map>>& sourceProperties,
                                   std::vector<std::pair<Dali::Property::Index, Dali::Property::Map>>& destinationProperties,
                                   Dali::Toolkit::Control                                              source,
                                   Dali::Toolkit::Control                                              destination) override;

  /**
   * @copydoc Toolkit::Control::OnUpdateVisualProperties()
   */
  virtual void OnUpdateVisualProperties(const std::vector<std::pair<Dali::Property::Index, Dali::Property::Map>>& properties) override;

private:
  /**
   * @brief Callback for ResourceReadySignal
   * param[in] control signal prototype
   */
  void OnResourceReady(Toolkit::Control control);

  /**
   * @brief Set TransformMap for fittingMode
   * param[in] finalSize The size for fittingMode
   * param[in] textureSize The size of texture
   * param[in] offset The offset for fittingMode
   * param[in] fittingMode The mode for fitting image
   * param[in] transformMap  The map for fitting image
   */
  void SetTransformMapForFittingMode(Vector2 finalSize, Vector2 textureSize, Vector2 offset, Visual::FittingMode fittingMode, Property::Map& transformMap);

  /**
   * @brief Apply fittingMode
   * param[in] finalSize The size for fittingMode
   * param[in] offset The offset for fittingMode
   * param[in] zeroPadding whether padding is zero
   * param[in] transformMap  The map for fitting image
   */
  void ApplyFittingMode(Vector2 finalSize, Vector2 offset, bool zeroPadding, Property::Map& transformMap);

private:
  // Undefined
  ImageView(const ImageView&);
  ImageView& operator=(const ImageView&);

private:
  Toolkit::Visual::Base mVisual;

  std::string     mUrl;         ///< the url for the image if the image came from a URL, empty otherwise
  Property::Map   mPropertyMap; ///< the Property::Map if the image came from a Property::Map, empty otherwise
  Property::Map   mShaderMap;   ///< the Property::Map if the custom shader is set, empty otherwise
  ImageDimensions mImageSize;   ///< the image size

  bool mImageVisualPaddingSetByTransform : 1;   //< Flag to indicate Padding was set using a transform.
  bool mImageViewPixelAreaSetByFittingMode : 1; //< Flag to indicate pixel area was set by fitting Mode
};

} // namespace Internal

// Helpers for public-api forwarding methods
inline Toolkit::Internal::ImageView& GetImpl(Toolkit::ImageView& obj)
{
  DALI_ASSERT_ALWAYS(obj);
  Dali::RefObject& handle = obj.GetImplementation();
  return static_cast<Toolkit::Internal::ImageView&>(handle);
}

inline const Toolkit::Internal::ImageView& GetImpl(const Toolkit::ImageView& obj)
{
  DALI_ASSERT_ALWAYS(obj);
  const Dali::RefObject& handle = obj.GetImplementation();
  return static_cast<const Toolkit::Internal::ImageView&>(handle);
}

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_INTERNAL_IMAGE_VIEW_H
