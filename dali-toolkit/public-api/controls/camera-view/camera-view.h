#ifndef DALI_TOOLKIT_CAMERA_VIEW_H
#define DALI_TOOLKIT_CAMERA_VIEW_H

/*
 * Copyright (c) 2021 Samsung Electronics Co., Ltd.
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

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/controls/control.h>

namespace Dali
{
class Any;

namespace Toolkit
{
namespace Internal DALI_INTERNAL
{
class CameraView;
} // namespace DALI_INTERNAL

/**
 * @addtogroup dali_toolkit_controls_camera_view
 * @{
 */

/**
 * @brief CameraView is a control for camera display.
 *
 * For working CameraView, a camera plugin for a platform should be provided.
 */
class DALI_TOOLKIT_API CameraView : public Control
{
public:
  /**
   * @brief Camera display type
   */
  enum class DisplayType
  {
    WINDOW = 0, // Overlay type
    IMAGE       // Native Image type
  };

  /**
   * @brief Creates an initialized CameraView.
   * @param[in] handle Multimedia camera player handle
   * @param[in] type Where it is an overlay type or a native image type
   * @return A handle to a newly allocated Dali ImageView
   */
  static CameraView New(Any handle, DisplayType type = DisplayType::WINDOW);

  /**
   * @brief Creates an uninitialized CameraView.
   */
  CameraView();

  /**
   * @brief Destructor.
   *
   * This is non-virtual since derived Handel types must not contain data or virtual methods.
   */
  ~CameraView();

  /**
   * @brief Copy constructor.
   *
   * @param[in] CameraView CameraView to copy. The copied CameraView will point at the same implementation
   */
  CameraView(const CameraView& CameraView);

  /**
   * @brief Move constructor
   *
   * @param[in] rhs A reference to the moved handle
   */
  CameraView(CameraView&& rhs);

  /**
   * @brief Update camera view
   *
   * Multimedia camera operation is work outside the view.
   * So, This must be called when the view needs to be updated after camera operation.
   */
  void Update();

  /**
   * @brief Assignment operator.
   *
   * @param[in] CameraView The CameraView to assign from
   * @return The updated CameraView
   */
  CameraView& operator=(const CameraView& CameraView);

  /**
   * @brief Move assignment
   *
   * @param[in] rhs A reference to the moved handle
   * @return A reference to this
   */
  CameraView& operator=(CameraView&& rhs);

  /**
   * @brief Downcasts a handle to CameraView handle.
   *
   * If handle points to a CameraView, the downcast produces valid handle.
   * If not, the returned handle is left uninitialized.
   *
   * @param[in] handle Handle to an object
   * @return Handle to a CameraView or an uninitialized handle
   */
  static CameraView DownCast(BaseHandle handle);

public: // Not intended for application developers
  /// @cond internal
  /**
   * @brief Creates a handle using the Toolkit::Internal implementation.
   *
   * @param[in] implementation The CameraView implementation
   */
  DALI_INTERNAL CameraView(Internal::CameraView& implementation);

  /**
   * @brief Allows the creation of this CameraView from an Internal::CustomActor pointer.
   *
   * @param[in] internal A pointer to the internal CustomActor
   */
  DALI_INTERNAL CameraView(Dali::Internal::CustomActor* internal);
  /// @endcond
};

/**
 * @}
 */

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_CAMERA_VIEW_H
