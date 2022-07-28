#ifndef DALI_TOOLKIT_GL_VIEW_H
#define DALI_TOOLKIT_GL_VIEW_H
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

// EXTERNAL INCLUDES
#include <dali/public-api/rendering/texture.h>

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/controls/control.h>

namespace Dali
{
namespace Toolkit
{
namespace Internal DALI_INTERNAL
{
class GlViewImpl;
}

/**
 * @brief GlView is a class for rendering with GL
 *
 * GlView allows drawing with OpenGL.
 * GlView creates a GL context, a GL surface and a render thread.
 * The render thread invokes user's callbacks.
 *
 * @SINCE_2_0.45
 */
class DALI_TOOLKIT_API GlView : public Dali::Toolkit::Control
{
public:
  /**
   * @brief Implementation backend mode
   *
   * @SINCE_2_1.18
   */
  enum class BackendMode
  {
    /**
     * DIRECT_RENDERING mode executes GL code within DALi graphics
     * pipeline. When Renderer is about to be drawn, the callback
     * will be executed and the custom code "injected" into the pipeline.
     * This allows rendering directly to the surface rather than offscreen.
     *
     * * @SINCE_2_1.18
     */
    DIRECT_RENDERING = 0,

    /**
     * DIRECT_RENDERING_THREADED mode executes GL code on separate thread
     * and then blits the result within DALi graphics commands stream.
     * The mode is logically compatible with the EGL_IMAGE_OFFSCREEN_RENDERING.
     *
     * @SINCE_2_1.30
     */
    DIRECT_RENDERING_THREADED,

    /**
     * EGL_IMAGE_OFFSCREEN_RENDERING mode executes GL code in own thread
     * and renders to the offscreen NativeImage (EGL) buffer. This backend
     * will render in parallel but has higher memory footprint and may suffer
     * performance issues due to using EGL image.
     *
     * @SINCE_2_1.18
     */
    EGL_IMAGE_OFFSCREEN_RENDERING,

    /**
     * The default mode is set to EGL_IMAGE_OFFSCREEN_RENDERING for backwards
     * compatibility.
     *
     * @SINCE_2_1.18
     */
    DEFAULT = EGL_IMAGE_OFFSCREEN_RENDERING
  };

  /**
   * @brief Enumeration for rendering mode
   *
   * This Enumeration is used to choose the rendering mode.
   * It has two options.
   * One of them is continuous mode. It is rendered continuously.
   * The other is on demand mode. It is rendered by application.
   *
   * @SINCE_2_0.45
   */
  enum class RenderingMode
  {
    CONTINUOUS, ///< continuous mode
    ON_DEMAND   ///< on demand by application
  };

  /**
   * @brief Enumeration for Graphics API version
   *
   * This Enumeration is used to set a GLES version for EGL configuration.
   *
   * @SINCE_2_0.45
   */
  enum class GraphicsApiVersion
  {
    GLES_VERSION_2_0 = 0, ///< GLES version 2.0
    GLES_VERSION_3_0,     ///< GLES version 3.0
  };

  /**
   * @brief Enumeration for color buffer format
   *
   * This Enumeration is used to set a color buffer format of GlView
   *
   * @SINCE_2_0.45
   */
  enum class ColorFormat
  {
    RGB888,  ///< 8 red bits, 8 green bits, 8 blue bits
    RGBA8888 ///< 8 red bits, 8 green bits, 8 blue bits, alpha 8 bits
  };

  /**
   * @brief Creates a GlView control.
   *
   * @note This function always creates the GlView with NativeImage backend.
   *
   * @param[in] colorFormat the format of the color buffer.
   * @return A handle to a GlView control
   *
   * @SINCE_2_0.45
   */
  static GlView New(ColorFormat colorFormat);

  /**
   * @brief Creates a GlView control.
   *
   * The new GlView will be created with specified backend.
   * The colorFormat is ignored for DIRECT_RENDERING backend.
   *
   * @param[in] colorFormat the format of the color buffer.
   * @param[in] backendMode the backend used by the GlView
   * @return A handle to a GlView control
   *
   * @SINCE_2_1.18
   */
  static GlView New(BackendMode backendMode, ColorFormat colorFormat);

  /**
   * @brief Creates an uninitialized GlView.
   *
   * @SINCE_2_0.45
   */
  GlView();

  /**
   * @brief Destructor.
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   *
   * @SINCE_2_0.45
   */
  ~GlView();

  /**
   * @brief Copy constructor.
   *
   * @param[in] GlView GlView to copy. The copied GlView will point at the same implementation
   *
   * @SINCE_2_0.45
   */
  GlView(const GlView& GlView);

  /**
   * @brief Move constructor
   *
   * @param[in] rhs A reference to the moved handle
   *
   * @SINCE_2_0.45
   */
  GlView(GlView&& rhs);

  /**
   * @brief Assignment operator.
   *
   * @param[in] GlView The GlView to assign from
   * @return A reference to this
   *
   * @SINCE_2_0.45
   */
  GlView& operator=(const GlView& GlView);

  /**
   * @brief Move assignment
   *
   * @param[in] rhs A reference to the moved handle
   * @return A reference to this
   *
   * @SINCE_2_0.45
   */
  GlView& operator=(GlView&& rhs);

  /**
   * @brief Downcasts a handle to GlView handle.
   *
   * If handle points to a GlView, the downcast produces valid handle.
   * If not, the returned handle is left uninitialized.
   *
   * @param[in] handle Handle to an object
   * @return Handle to a GlView or an uninitialized handle
   *
   * @SINCE_2_0.45
   */
  static GlView DownCast(BaseHandle handle);

  /**
   * @brief Registers GL callback functions for GlView.
   *
   * @param[in] initCallback  the callback function to create GL resources.
   * @param[in] renderFrameCallback the callback function to render for the frame.
   * @param[in] terminateCallback the callback function to clean-up GL resources.
   *
   * A initCallback of the following type have to be used:
   * @code
   *   void intializeGL();
   * @endcode
   * This callback will be called before renderFrame callback is called once.
   *
   * A renderFrameCallback of the following type have to be used:
   * @code
   *   int renderFrameGL();
   * @endcode
   * If the return value of this callback is not 0, the eglSwapBuffers() will be called.
   *
   * A terminateCallback of the following type have to be used:
   * @code
   *   void terminateGL();
   * @endcode
   * This callback is called when GlView is deleted.
   *
   * @note Ownership of the callbacks is passed onto this class.
   * <b>You can't call Dali APIs in your callbacks because it is invoked in GlView's own render thread.</b>
   * And this must be called before adding GlView to the scene.
   *
   * @SINCE_2_0.45
   */
  void RegisterGlCallbacks(CallbackBase* initCallback, CallbackBase* renderFrameCallback, CallbackBase* terminateCallback);

  /**
   * @brief Sets the ResizeCallback of the GlView.
   * When GlView is resized, ResizeCallback would be invoked.
   * You can get the resized width and height of the GlView.
   *
   * @param[in] resizeCallback The ResizeCallback function
   *
   * A resizeCallback of the following type have to be used:
   * @code
   *   void resizeCallback(int width, int height);
   * @endcode
   *
   * @note Ownership of the callback is passed onto this class.
   * <b>You can't call Dali APIs in your callback because it is invoked in GlView's own render thread.</b>
   * And this must be called before adding GlView to the scene.
   *
   * @SINCE_2_0.45
   */
  void SetResizeCallback(CallbackBase* resizeCallback);

  /**
   * @brief Sets the rendering mode.
   *
   * @param[in] mode the rendering mode for GlView
   *
   * @note The default Rendering mode is CONTINUOUS.
   * If ON_DEMAND mode is set, it is rendered by RenderOnce()
   *
   * @SINCE_2_0.45
   */
  void SetRenderingMode(RenderingMode mode);

  /**
   * @brief Gets the rendering mode.
   *
   * @SINCE_2_0.45
   */
  [[nodiscard]] RenderingMode GetRenderingMode() const;

  /**
   * @brief Gets the backend mode.
   *
   * @SINCE_2_1.18
   */
  [[nodiscard]] BackendMode GetBackendMode() const;

  /**
   * @brief Sets egl configuration for GlView
   *
   * @param[in] depth the flag of depth buffer. If the value is true, 24bit depth buffer is enabled.
   * @param[in] stencil the flag of stencil. If the value is true, 8bit stencil buffer is enabled.
   * @param[in] msaa the expected sampling number per pixel.
   * @param[in] version the graphics API version
   * @return True if the config exists, false otherwise.
   *
   * @SINCE_2_0.45
   */
  bool SetGraphicsConfig(bool depth, bool stencil, int msaa, GraphicsApiVersion version);

  /**
   * @brief Renders once more even if GL render functions are not added to idler.
   * @note Will not work if the window is hidden or GL render functions are added to idler
   *
   * @SINCE_2_0.45
   */
  void RenderOnce();

  /**
   * @brief Binds DALi textures to the callback
   *
   * The textures that are bound to the callback will be passed upon
   * callback execution providing native handles (like GL name) so they
   * can be used alongside with custom GL code.
   *
   * Binding texture does not affect lifecycle and it's up to the client-side
   * to make sure the resource is alive when used inside the callback.
   *
   * @param[in] textures List of DALi textures to be bound to the callback
   *
   * @note It only supported only in the GlView::BackendMode::DIRECT_RENDERING.
   *
   * @SINCE_2_2.2
   */
  void BindTextureResources(std::vector<Dali::Texture> textures);

public: // Not intended for application developers
  /// @cond internal
  /**
   * @brief Creates a handle using the Toolkit::Internal implementation.
   * @param[in] implementation The GlView implementation
   *
   * @SINCE_2_0.45
   */
  DALI_INTERNAL GlView(Internal::GlViewImpl& implementation);

  /**
   * @brief Allows the creation of this GlView from an Internal::CustomActor pointer.
   * @param[in] internal A pointer to the internal CustomActor
   *
   * @SINCE_2_0.45
   */
  DALI_INTERNAL GlView(Dali::Internal::CustomActor* internal);
  /// @endcond
};

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_GL_VIEW_H
