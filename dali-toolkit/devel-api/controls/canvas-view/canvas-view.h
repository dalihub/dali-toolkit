#ifndef DALI_TOOLKIT_CANVAS_VIEW_H
#define DALI_TOOLKIT_CANVAS_VIEW_H

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

// EXTERNAL INCLUDES
#include <dali/devel-api/adaptor-framework/canvas-renderer-drawable.h>
#include <dali/devel-api/adaptor-framework/canvas-renderer-shape.h>

namespace Dali
{
namespace Toolkit
{
namespace Internal DALI_INTERNAL
{
class CanvasView;
}
/**
 * @addtogroup dali_toolkit_controls_canvas_view
 * @{
 */

/**
 * @brief CanvasView is a class for displaying an vector primitives.
 *
 * @code
 *    auto myCanvasView = CanvasView::New( viewBox ) ); //The viewBox is the size of viewbox of CanvasView.
 *
 *    //Create shape and set properties.
 *    auto shape = Dali::CanvasRenderer::Shape::New();
 *    shape.AddRect( 0, 0, 10, 10, 0, 0 );
 *    shape.SetFillColor( Vector4( 1.0, 1.0, 1.0, 1.0 ) );
 *    myCanvasView.AddDrawable( shape );
 * @endcode
 *
 *
 */
class DALI_TOOLKIT_API CanvasView : public Control
{
public:
  /**
   * @brief Creates an uninitialized CanvasView.
   */
  CanvasView();

  /**
   * @brief Creates an initialized CanvasView
   *
   * @param [in] viewBox The width and height.
   * @return A handle to a newly allocated CanvasView
   */
  static CanvasView New(const Vector2& viewBox);

  /**
   * @brief Destructor.
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   */
  ~CanvasView();

  /**
   * @brief Copy constructor.
   *
   * @param[in] canvasView CanvasView to copy. The copied CanvasView will point at the same implementation
   */
  CanvasView(const CanvasView& canvasView);

  /**
   * @brief Move constructor
   *
   * @param[in] rhs A reference to the moved handle
   */
  CanvasView(CanvasView&& rhs);

  /**
   * @brief Assignment operator.
   *
   * @param[in] canvasView The CanvasView to assign from
   * @return The updated CanvasView
   */
  CanvasView& operator=(const CanvasView& canvasView);

  /**
   * @brief Move assignment
   *
   * @param[in] rhs A reference to the moved handle
   * @return A reference to this
   */
  CanvasView& operator=(CanvasView&& rhs);

  /**
   * @brief Downcasts a handle to CanvasView handle.
   *
   * If handle points to a CanvasView, the downcast produces valid handle.
   * If not, the returned handle is left uninitialized.
   *
   * @param[in] handle Handle to an object
   * @return Handle to a CanvasView or an uninitialized handle
   */
  static CanvasView DownCast(BaseHandle handle);

  /**
   * @brief Add drawable object to the CanvasView.
   * This method is similar to registration. The added shape is drawn on the inner canvas.
   */
  void AddDrawable(Dali::CanvasRenderer::Drawable& drawable);

public: // Not intended for application developers
  /// @cond internal
  /**
   * @brief Creates a handle using the Toolkit::Internal implementation.
   *
   * @param[in] implementation The CanvasView implementation
   */
  DALI_INTERNAL CanvasView(Internal::CanvasView& implementation);

  /**
   * @brief Allows the creation of this CanvasView from an Internal::CustomActor pointer.
   *
   * @param[in] internal A pointer to the internal CustomActor
   */
  DALI_INTERNAL CanvasView(Dali::Internal::CustomActor* internal);
  /// @endcond
};

/**
 * @}
 */
} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_CANVAS_VIEW_H
