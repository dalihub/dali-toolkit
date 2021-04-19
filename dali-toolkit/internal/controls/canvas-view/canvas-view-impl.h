#ifndef DALI_TOOLKIT_INTERNAL_CANVAS_VIEW_H
#define DALI_TOOLKIT_INTERNAL_CANVAS_VIEW_H

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
#include <dali/devel-api/adaptor-framework/canvas-renderer-drawable.h>
#include <dali/devel-api/adaptor-framework/canvas-renderer-shape.h>
#include <dali/devel-api/adaptor-framework/canvas-renderer.h>
#include <dali/devel-api/rendering/renderer-devel.h>
#include <dali/integration-api/processor-interface.h>

// INTERNAL INCLUDES
#include <dali-toolkit/devel-api/controls/canvas-view/canvas-view.h>
#include <dali-toolkit/public-api/controls/control-impl.h>

namespace Dali
{
namespace Toolkit
{
class CanvasView;

namespace Internal
{
class CanvasView : public Control, public Integration::Processor
{
public:
  /**
   * Construct a new CanvasView.
   */
  CanvasView(const Vector2& viewBox);

  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~CanvasView();

public:
  /**
   * Create a new CanvasView.
   * @return A smart-pointer to the newly allocated CanvasView.
   */
  static Toolkit::CanvasView New(const Vector2& viewBox);

  /**
   * @copydoc Toolkit::Control::CanvasView::AddDrawable
   */
  bool AddDrawable(Dali::CanvasRenderer::Drawable& drawable);

private: // From Control
  /**
   * @copydoc Control::OnRelayout
   */
  void OnRelayout(const Vector2& size, RelayoutContainer& container) override;

  /**
   * @copydoc Toolkit::Control::OnSizeSet()
   */
  void OnSizeSet(const Vector3& targetSize) override;

  /**
   * @copydoc Toolkit::Control::OnInitialize
   */
  void OnInitialize() override;

protected: // Implementation of Processor
  /**
   * @copydoc Dali::Integration::Processor::Process()
   */
  void Process() override;

private:
  /**
   * @brief Draw drawables added to CanvasView on inner canvas.
   * Then make that buffer into a texture and add it to renderer.
   */
  void Commit();

private:
  CanvasView(const CanvasView&) = delete;
  CanvasView& operator=(const CanvasView&) = delete;

private:
  CanvasRenderer mCanvasRenderer;
  Dali::Texture  mTexture;
  bool           mChanged;
};

} // namespace Internal

// Helpers for public-api forwarding methods
inline Toolkit::Internal::CanvasView& GetImpl(Toolkit::CanvasView& obj)
{
  DALI_ASSERT_ALWAYS(obj);
  Dali::RefObject& handle = obj.GetImplementation();
  return static_cast<Toolkit::Internal::CanvasView&>(handle);
}

inline const Toolkit::Internal::CanvasView& GetImpl(const Toolkit::CanvasView& obj)
{
  DALI_ASSERT_ALWAYS(obj);
  const Dali::RefObject& handle = obj.GetImplementation();
  return static_cast<const Toolkit::Internal::CanvasView&>(handle);
}

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_INTERNAL_CANVAS_VIEW_H
