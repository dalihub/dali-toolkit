#ifndef DALI_TOOLKIT_INTERNAL_CANVAS_VIEW_H
#define DALI_TOOLKIT_INTERNAL_CANVAS_VIEW_H

/*
 * Copyright (c) 2024 Samsung Electronics Co., Ltd.
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
#include <dali/devel-api/adaptor-framework/canvas-renderer/canvas-renderer-drawable.h>
#include <dali/devel-api/adaptor-framework/canvas-renderer/canvas-renderer-shape.h>
#include <dali/devel-api/adaptor-framework/canvas-renderer/canvas-renderer.h>
#include <dali/devel-api/rendering/renderer-devel.h>
#include <dali/integration-api/processor-interface.h>

// INTERNAL INCLUDES
#include <dali-toolkit/devel-api/controls/canvas-view/canvas-view.h>
#include <dali-toolkit/internal/controls/canvas-view/canvas-view-rasterize-task.h>
#include <dali-toolkit/public-api/controls/control-impl.h>

namespace Dali
{
namespace Toolkit
{
namespace Internal
{
class CanvasView;
class CanvasViewRasterizeThread;

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

  /**
   * Called when a property of an object of this type is set.
   * @param[in] object The object whose property is set.
   * @param[in] propertyIndex The property index.
   * @param[in] value The new property value.
   */
  static void SetProperty(BaseObject* object, Property::Index propertyIndex, const Property::Value& value);

  /**
   * Called to retrieve a property of an object of this type.
   * @param[in] object The object whose property is to be retrieved.
   * @param[in] propertyIndex The property index.
   * @return The current value of the property.
   */
  static Property::Value GetProperty(BaseObject* object, Property::Index propertyIndex);

  /**
   * @copydoc Toolkit::Control::CanvasView::RemoveDrawable
   */
  bool RemoveDrawable(Dali::CanvasRenderer::Drawable& drawable);

  /**
   * @copydoc Toolkit::Control::CanvasView::RemoveAllDrawables
   */
  bool RemoveAllDrawables();

  /**
   * @copydoc Toolkit::Control::CanvasView::RequestRasterization
   */
  void RequestRasterization();

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

  /**
   * @brief This is the viewbox of the Canvas.
   * @param[in] viewBox The size of viewbox.
   * @return Returns True when it's successful. False otherwise.
   */
  bool SetViewBox(const Vector2& viewBox);

  /**
   * @brief This is the viewbox of the Canvas.
   * @return Returns The size of viewbox.
   */
  const Vector2& GetViewBox();

  /**
   * @brief Set to load the canvas synchronously.
   *
   * @param isSynchronous The synchronous
   */
  void SetSynchronous(const bool isSynchronous);

  /**
   * @brief Whether to load the canvas synchronously.
   *
   * @return Returns true if synchronously.
   */
  const bool IsSynchronous() const;

  /**
   * @brief Set to request rasterize canvas manually or automatically
   *
   * @param isRasterizationManually True if rasterize canvas manually.
   */
  void SetRasterizationRequestManually(const bool isRasterizationManually);

  /**
   * @brief Whether to request rasterize canvas manually or automatically
   *
   * @return Returns true if rasterize canvas manually.
   */
  const bool IsRasterizationRequestManually() const;

  /**
   * @bried Rasterize the canvas, and add it to the view.
   *
   * @param[in] forceProcess True if we need to rasterize forcibly without changeness check.
   */
  void AddRasterizationTask(bool forceProcess);

protected:
  /**
   * @copydoc Dali::Integration::Processor::Process()
   */
  void Process(bool postProcessor) override;

  /**
   * @copydoc Dali::Integration::Processor::GetProcessorName()
   */
  std::string_view GetProcessorName() const override
  {
    return "CanvasView";
  }

public:
  /**
   * @bried Apply the rasterized image to the canvas view
   *
   * @param[in] task CanvasRendererRasterizingTaskPtr
   */
  void ApplyRasterizedImage(CanvasRendererRasterizingTaskPtr task);

private:
  CanvasView(const CanvasView&) = delete;
  CanvasView& operator=(const CanvasView&) = delete;

private:
  CanvasRenderer                   mCanvasRenderer;
  Dali::Texture                    mTexture;
  TextureSet                       mTextureSet;
  Vector2                          mSize;
  CanvasRendererRasterizingTaskPtr mRasterizingTask;
  bool                             mIsSynchronous : 1;
  bool                             mManualRasterization : 1;
  bool                             mProcessorRegistered : 1;
  bool                             mLastCommitRasterized : 1;
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
