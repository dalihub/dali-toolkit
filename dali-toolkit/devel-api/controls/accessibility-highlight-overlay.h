// accessibility-highlight-overlay.h
#ifndef DALI_TOOLKIT_ACCESSIBILITY_HIGHLIGHT_OVERLAY_H
#define DALI_TOOLKIT_ACCESSIBILITY_HIGHLIGHT_OVERLAY_H

/*
 * Copyright (c) 2025 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/actors/actor.h>
#include <dali/public-api/math/rect.h>
#include <dali/public-api/object/weak-handle.h>

namespace Dali::Toolkit::DevelControl
{

enum class OverlayHighlightMode : uint32_t
{
  AUTO = 0,
  MANUAL
};

class AccessibilityHighlightOverlay
{
public:
  AccessibilityHighlightOverlay();

  /**
  * @brief Function to set a custom highlight overlay
  *
  * This function sets a custom highlight overlay at the specified position and size.
  *
  * @param position A Vector2 representing the position of the overlay
  * @param size A Vector2 representing the size of the overlay
  */
  void SetCustomHighlight(Vector2 position, Vector2 size);

  /**
 * @brief Function to reset the custom highlight overlay
 *
 * This function resets the custom highlight overlay.
 */
  void ResetCustomHighlight();

  /**
  * @brief Updates the currently active highlight overlay. If no overlay exists, it creates a new one.
  * @param activeHighlight The currently active highlight actor.
  */
  void UpdateOverlay(Dali::Actor& activeHighlight);

  /**
  * @brief Hides the currently displayed highlight overlay.
  */
  void HideOverlay();

  /**
  * @brief Sets the currently overlay mode
  * @param mode The currently OverlayHighlightMode to set.
  */
  void SetOverlayMode(OverlayHighlightMode mode);

  /**
  * @brief Gets the currently overlay mode
  */
  OverlayHighlightMode GetOverlayMode() const;

private:
  /**
  * @brief Finds the parent SceneView of the given actor and returns the SceneView along with a boolean indicating whether the parent contains a model.
  * @param current The current actor.
  * @return Returns the SceneView containing the current object. If the current object is not a child of any SceneView, returns null.
  */
  Dali::Actor FindParentSceneView(Dali::Actor current);

  /**
  * @brief Creates a highlight overlay for the given SceneView.
  * @param sceneView The SceneView for which the highlight overlay will be created.
  */
  void CreateOverlay(Dali::Actor& sceneView);

  /**
  * @brief Updates the position of the highlight based on the given SceneView and the previous highlight actor.
  * @param sceneView The SceneView for which the highlight position will be updated.
  * @param highlight The previously used highlight actor.
  */
  void UpdateOverlayPosition(Dali::Actor& sceneView, Dali::Actor& highlight);

  /**
  * @brief Returns the size and position of the screen overlay based on the given SceneView and the previous highlight actor.
  * @param sceneView The SceneView for which the screen overlay extents will be calculated.
  * @param highlight The previously used highlight actor.
  * @return A Rect<float> representing the extents of the screen overlay.
  */
  Rect<float> GetOverlayExtents(Dali::Actor& sceneView, Dali::Actor& highlight);

private:
  bool                           mHasOverlayActor;
  OverlayHighlightMode           mOverlayMode;
  Vector2                        mManualPosition;
  Vector2                        mManualSize;
  Dali::WeakHandle<Dali::Actor>  mOverlayActor;
};

} // namespace Dali::Toolkit::DevelControl

#endif // DALI_TOOLKIT_ACCESSIBILITY_HIGHLIGHT_OVERLAY_H
