#ifndef DALI_TOOLKIT_INTERNAL_CAMERA_VIEW_H
#define DALI_TOOLKIT_INTERNAL_CAMERA_VIEW_H

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

// EXTERNAL INCLUDES
#include <dali/devel-api/adaptor-framework/camera-player.h>
#include <dali/public-api/object/property-notification.h>
#include <dali/public-api/rendering/renderer.h>
#include <dali/public-api/rendering/texture.h>

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/controls/camera-view/camera-view.h>
#include <dali-toolkit/public-api/controls/control-impl.h>

namespace Dali
{
namespace Toolkit
{
class CameraView;

namespace Internal
{
class CameraView : public Control
{
protected:
  /**
   * @brief Constructor.
   * @param[in] type Where it is an overlay type or a native image type
   */
  CameraView(Dali::Toolkit::CameraView::DisplayType type);

  /**
   * @brief Destructor
   */
  virtual ~CameraView();

public:
  /**
   * @brief Creates an initialized CameraView.
   * @param[in] handle Multimedia camera player handle
   * @param[in] type Where it is an overlay type or a native image type
   */
  static Toolkit::CameraView New(Any handle, Dali::Toolkit::CameraView::DisplayType type);

  /**
   * @brief Update camera player.
   */
  void Update();

private: // From Control
  /**
   * @copydoc Toolkit::Control::OnSceneConnection()
   */
  void OnSceneConnection(int depth) override;

  /**
   * @copydoc Toolkit::Control::OnSceneDisconnection()
   */
  void OnSceneDisconnection() override;

private:
  /**
   * @brief Construct a new CameraView.
   */
  CameraView(const CameraView& cameraView);

  // Undefined assignment operator.
  CameraView& operator=(const CameraView& cameraView);

  /**
   * @brief Updates camera display area for window rendering target
   * @param[in] source PropertyNotification
   */
  void UpdateDisplayArea(Dali::PropertyNotification& source);

  /**
   * @brief SetWindowSurfaceTarget for underlay camera preview.
   */
  void SetWindowSurfaceTarget();

  /**
   * @brief SetNativeImageTarget for native image camera preview.
   */
  void SetNativeImageTarget();

  /**
   * @brief CreateShader for native image target
   * @param[in] nativeImageSourcePtr to apply custom fragment prefix
   * @return Returns the shader for NativeImage.
   */
  Dali::Shader CreateShader(Dali::NativeImageSourcePtr nativeImageSourcePtr);

private:
  Dali::CameraPlayer mCameraPlayer;
  Dali::Texture      mNativeTexture;

  Dali::DisplayArea mDisplayArea;
  Dali::Renderer    mOverlayRenderer;
  Dali::Renderer    mTextureRenderer;

  Dali::PropertyNotification mPositionUpdateNotification;
  Dali::PropertyNotification mSizeUpdateNotification;
  Dali::PropertyNotification mScaleUpdateNotification;

  Dali::Toolkit::CameraView::DisplayType mDisplayType;
};

} // namespace Internal

inline Toolkit::Internal::CameraView& GetImpl(Toolkit::CameraView& handle)
{
  DALI_ASSERT_ALWAYS(handle);
  Dali::RefObject& impl = handle.GetImplementation();
  return static_cast<Toolkit::Internal::CameraView&>(impl);
}

inline const Toolkit::Internal::CameraView& GetImpl(const Toolkit::CameraView& handle)
{
  DALI_ASSERT_ALWAYS(handle);
  const Dali::RefObject& impl = handle.GetImplementation();
  return static_cast<const Toolkit::Internal::CameraView&>(impl);
}

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_INTERNAL_CAMERA_VIEW_H