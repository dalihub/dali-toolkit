#ifndef DALI_TOOLKIT_MEDIA_VIEW_H
#define DALI_TOOLKIT_MEDIA_VIEW_H

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
#include <dali/public-api/math/rect.h>
#include <dali/public-api/object/any.h>
// INTERNAL INCLUDES
#include <dali-toolkit/public-api/controls/control.h>
namespace DALI_NAMESPACE
{
namespace Toolkit
{
namespace Internal DALI_INTERNAL
{
class MediaView;
}
/**
 * @brief Displays a borrowed native frame source without controlling its producer.
 * @note Use on the DALi event thread. One MediaView per source. Gapless is unsupported.
 * @SINCE_2_5.47
 * @addtogroup dali_toolkit_controls_media_view
 * @{
 */
class DALI_TOOLKIT_API MediaView : public Control
{
public:
  /**
   * @brief Constructs an empty handle.
   * @SINCE_2_5.47
   */
  MediaView();

  /**
   * @brief Creates a view without a source.
   * @SINCE_2_5.47
   * @return An initialized view
   */
  static MediaView New();

  /**
   * @brief Creates and connects a view.
   * @SINCE_2_5.47
   * @param[in] nativeSource A borrowed native frame source wrapped in Any
   * @return An initialized view, or an empty handle if connection fails
   */
  static MediaView New(Dali::Any nativeSource);

  /**
   * @brief Downcasts a compatible handle.
   * @SINCE_2_5.47
   * @param[in] handle A handle to an object
   * @return A MediaView handle, or an empty handle for an incompatible object
   */
  static MediaView DownCast(BaseHandle handle);

  /**
   * @brief Releases this handle.
   * @SINCE_2_5.47
   */
  ~MediaView();

  /**
   * @brief Copies a handle, sharing its view.
   * @SINCE_2_5.47
   */
  MediaView(const MediaView&);

  /**
   * @brief Moves a handle.
   * @SINCE_2_5.47
   */
  MediaView(MediaView&&) noexcept;

  /**
   * @brief Copies a handle.
   * @SINCE_2_5.47
   */
  MediaView& operator=(const MediaView&);

  /**
   * @brief Moves a handle.
   * @SINCE_2_5.47
   */
  MediaView& operator=(MediaView&&) noexcept;

  /**
   * @brief Connects an opaque native frame source; failure leaves this view disconnected.
   * @SINCE_2_5.47
   * @param[in] nativeSource A borrowed native frame source wrapped in Any
   * @return True if connected; false if unsupported, unavailable, or already used
   * @note The producer owns the source. ClearSource() before destroying the producer.
   */
  bool SetSource(Dali::Any nativeSource);

  /**
   * @brief Returns the connected native source, or an empty Any.
   * @SINCE_2_5.47
   * @return The borrowed native source, or an empty Any
   */
  Dali::Any GetSource() const;

  /**
   * @brief Disconnects and returns all retained producer packets.
   * @SINCE_2_5.47
   */
  void ClearSource();

  /**
   * @brief Sets a normalized crop in unrotated source coordinates; invalid values leave it unchanged.
   * @SINCE_2_5.47
   * @param[in] crop A finite rectangle within [0,1], with positive width and height
   * @return True for a valid crop; false if the previous crop is kept
   */
  bool SetSourceCrop(Dali::Rect<float> crop);

  /**
   * @brief Returns the source crop; the default is the full frame.
   * @SINCE_2_5.47
   * @return The crop rectangle; initially (0,0,1,1)
   */
  Dali::Rect<float> GetSourceCrop() const;

  /**
   * @brief Sets horizontal reflection in the rotated display coordinate system.
   * @SINCE_2_5.47
   * @param[in] enabled True to reflect the image; initially false
   */
  void SetHorizontalFlipEnabled(bool enabled);

  /**
   * @brief Returns whether horizontal reflection is enabled.
   * @SINCE_2_5.47
   * @return True if reflection is enabled
   */
  bool IsHorizontalFlipEnabled() const;

  /**
   * @brief Sets vertical reflection in the rotated display coordinate system.
   * @SINCE_2_5.47
   * @param[in] enabled True to reflect the image; initially false
   */
  void SetVerticalFlipEnabled(bool enabled);

  /**
   * @brief Returns whether vertical reflection is enabled.
   * @SINCE_2_5.47
   * @return True if reflection is enabled
   */
  bool IsVerticalFlipEnabled() const;

  /**
   * @brief Internal handle constructor.
   */
  explicit DALI_INTERNAL MediaView(Internal::MediaView& implementation);

  /**
   * @brief Internal actor constructor used by DownCast.
   */
  explicit DALI_INTERNAL MediaView(Dali::Internal::CustomActor* actor);
};
/**
 * @}
 */
} // namespace Toolkit
} // namespace DALI_NAMESPACE
#endif // DALI_TOOLKIT_MEDIA_VIEW_H
