#ifndef DALI_TOOLKIT_MEDIA_VIEW_IMPL_H
#define DALI_TOOLKIT_MEDIA_VIEW_IMPL_H

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
#include <dali/integration-api/adaptor-framework/media-view-backend.h>
// INTERNAL INCLUDES
#include <dali-toolkit/devel-api/visual-factory/visual-base.h>
#include <dali-toolkit/public-api/controls/control-impl.h>
#include <dali-toolkit/public-api/controls/media-view/media-view.h>
namespace DALI_NAMESPACE
{
namespace Toolkit
{
namespace Internal
{
/**
 * @brief Registers a streamed image visual for the shared frame backend and manages its lifetime.
 */
class MediaView : public ControlImpl
{
public:
  MediaView();
  ~MediaView() override;
  static Toolkit::MediaView            New();
  Dali::Integration::MediaViewBackend& Backend()
  {
    return *mBackend;
  }

  const Dali::Integration::MediaViewBackend& Backend() const
  {
    return *mBackend;
  }

private:
  /**
   * @brief Updates the visual from an event-thread presentation notification.
   */
  void OnPresentationChanged();

  /**
   * @brief Unregisters the content visual.
   */
  void ClearVisual();

  /**
   * @copydoc ControlImpl::GetNaturalSize
   */
  Vector3 GetNaturalSize() override;

  /**
   * @copydoc ControlImpl::GetHeightForWidth
   */
  float GetHeightForWidth(float width) override;

  /**
   * @copydoc ControlImpl::GetWidthForHeight
   */
  float GetWidthForHeight(float height) override;

  void OnInitialize() override;
  void OnSceneConnection(int depth) override;
  void OnSceneDisconnection() override;
  MediaView(const MediaView&)                                                      = delete;
  MediaView&                                           operator=(const MediaView&) = delete;
  Toolkit::Visual::Base                                mVisual;
  Dali::UniquePtr<Dali::Integration::MediaViewBackend> mBackend;

  /**
   * @brief Stores the property index allocated for this view's content visual.
   */
  Dali::Property::Index mContentVisualIndex{Dali::Property::INVALID_INDEX};
};
} // namespace Internal
} // namespace Toolkit
} // namespace DALI_NAMESPACE
#endif // DALI_TOOLKIT_MEDIA_VIEW_IMPL_H
