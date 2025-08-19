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

// CLASS HEADER
#include "accessibility-highlight-overlay.h"

// EXTERNAL INCLUDES
#include <dali/public-api/actors/actor.h>
#include <dali/public-api/math/rect.h>
#include <dali/public-api/math/vector2.h>
#include <dali/public-api/common/constants.h>
#include <dali/public-api/object/property-map.h>
#include <dali/devel-api/actors/actor-devel.h>

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/controls/control.h>
#include <dali-toolkit/public-api/controls/control-impl.h>
#include <dali-toolkit/public-api/controls/image-view/image-view.h>
#include <dali-toolkit/devel-api/asset-manager/asset-manager.h>

#include <dali-toolkit/devel-api/controls/control-accessible.h>
#include <dali-toolkit/internal/controls/control/control-data-impl.h>

#include <dali/devel-api/atspi-interfaces/accessible.h>

namespace Dali::Toolkit::DevelControl
{
namespace
{
constexpr const char* FOCUS_IMAGE = "/keyboard_focus.9.png";
constexpr const char* OVERLAY_NAME = "HighlightOverlay";
constexpr const char* HIGHLIGHT_NAME = "HighlightIndicator";

#ifdef DEBUG_ENABLED
Debug::Filter* gLogFilter = Debug::Filter::New(Debug::NoLogging, false, "LOG_CONTROL_ACCESSIBLE");
#endif

Dali::Actor CreateOverlayActor()
{
  auto actor = Control::New(Toolkit::Control::ControlBehaviour::DISABLE_STYLE_CHANGE_SIGNALS);
  actor.SetResizePolicy(ResizePolicy::FIXED, Dimension::ALL_DIMENSIONS);

  actor.SetProperty(Actor::Property::NAME, OVERLAY_NAME);
  actor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  actor.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
  actor.SetProperty(Actor::Property::POSITION, Vector3(0.0f, 0.0f, 0.0f));

  actor.SetProperty(Toolkit::DevelControl::Property::ACCESSIBILITY_HIGHLIGHTABLE, false);
  return actor;
}

Dali::Actor CreateOverlayHighlightActor()
{
  std::string focusBorderImagePath(AssetManager::GetDaliImagePath());
  focusBorderImagePath += FOCUS_IMAGE;

  auto actor = Toolkit::ImageView::New(focusBorderImagePath);
  actor.SetProperty(Actor::Property::NAME, HIGHLIGHT_NAME);
  actor.SetResizePolicy(ResizePolicy::FIXED, Dimension::ALL_DIMENSIONS);
  actor.SetProperty(Toolkit::DevelControl::Property::ACCESSIBILITY_HIGHLIGHTABLE, false);
  return actor;
}
} // unnamed namespace

AccessibilityHighlightOverlay::AccessibilityHighlightOverlay()
: mHasOverlayActor(false),
  mOverlayMode(OverlayHighlightMode::AUTO),
  mManualPosition(0.0f, 0.0f),
  mManualSize(0.0f, 0.0f)
{
}

void AccessibilityHighlightOverlay::SetCustomHighlight(Vector2 position, Vector2 size)
{
  mManualPosition = position;
  mManualSize     = size;
  mOverlayMode    = OverlayHighlightMode::MANUAL;
}

void AccessibilityHighlightOverlay::ResetCustomHighlight()
{
  mManualPosition = Vector2(0, 0);
  mManualSize     = Vector2(0, 0);
  mOverlayMode = OverlayHighlightMode::AUTO;
}

void AccessibilityHighlightOverlay::SetOverlayMode(OverlayHighlightMode mode)
{
  mOverlayMode = mode;
}

OverlayHighlightMode AccessibilityHighlightOverlay::GetOverlayMode() const
{
  return mOverlayMode;
}

void AccessibilityHighlightOverlay::UpdateOverlay(Dali::Actor& activeHighlight)
{
  auto sceneView = FindParentSceneView(activeHighlight);
  if(!sceneView)
  {
    activeHighlight.SetProperty(Actor::Property::VISIBLE, true);
    DALI_LOG_INFO(gLogFilter, Debug::Verbose,"SceneView not found\n");
    return;
  }

  if(!mOverlayActor.GetHandle())
  {
    CreateOverlay(sceneView);
  }

  if(mOverlayActor.GetHandle())
  {
    UpdateOverlayPosition(sceneView, activeHighlight);
  }
}

void AccessibilityHighlightOverlay::HideOverlay()
{
  if(mHasOverlayActor)
  {
    auto overlayActor = mOverlayActor.GetHandle();
    if(overlayActor)
    {
      auto highlightActor = overlayActor.FindChildByName(HIGHLIGHT_NAME);
      if(highlightActor)
      {
        highlightActor.SetProperty(Actor::Property::VISIBLE, false);
      }
    }
  }
}

Dali::Actor AccessibilityHighlightOverlay::FindParentSceneView(Actor highlight)
{
  auto current = highlight.GetParent();

  while(current)
  {
    if(ControlAccessible::IsScene3D(current))
    {
      break;
    }
    current = current.GetParent();
  }
  return current;
}

void AccessibilityHighlightOverlay::CreateOverlay(Dali::Actor& sceneView)
{
  auto overlayLayer = sceneView.GetParent().FindChildByName(OVERLAY_NAME);
  if(!overlayLayer)
  {
    overlayLayer = CreateOverlayActor();

    Vector2 sceneSize;
    sceneView.GetProperty(Actor::Property::SIZE).Get(sceneSize);
    overlayLayer.SetProperty(Actor::Property::SIZE, sceneSize);

    auto highlightActor = CreateOverlayHighlightActor();
    overlayLayer.Add(highlightActor);
    sceneView.GetParent().Add(overlayLayer);
  }
  mOverlayActor = Dali::WeakHandle<Dali::Actor>(overlayLayer);
  mHasOverlayActor = true;
}

void AccessibilityHighlightOverlay::UpdateOverlayPosition(Dali::Actor& sceneView, Actor& highlight)
{
  auto highlightOverlayActor = mOverlayActor.GetHandle();
  auto highlightActor = highlightOverlayActor.FindChildByName(HIGHLIGHT_NAME);
  if(!highlightActor)
  {
    highlightActor = CreateOverlayHighlightActor();
    highlightOverlayActor.Add(highlightActor);
  }

  Vector2 sceneSize, scenePosition;
  sceneView.GetProperty(Actor::Property::POSITION).Get(scenePosition);
  sceneView.GetProperty(Actor::Property::SIZE).Get(sceneSize);

  highlight.SetProperty(Actor::Property::VISIBLE, false);
  highlightActor.SetProperty(Actor::Property::VISIBLE, true);
  highlightOverlayActor.SetProperty(Actor::Property::SIZE, sceneSize);
  highlightOverlayActor.SetProperty(Actor::Property::POSITION, scenePosition);

  if(mOverlayMode == OverlayHighlightMode::AUTO)
  {
    auto actualExtent = GetOverlayExtents(sceneView, highlight);

    if(actualExtent.x < 0.0f || actualExtent.y < 0.0f || actualExtent.width < 0.0f || actualExtent.height < 0.0f)
    {
      DALI_LOG_INFO(gLogFilter, Debug::Verbose, "Invalid extents returned\n");
      return;
    }
    highlightActor.SetProperty(Actor::Property::POSITION, Vector2(actualExtent.x, actualExtent.y));
    highlightActor.SetProperty(Actor::Property::SIZE, Vector2(actualExtent.width, actualExtent.height));
  }
  else
  {
    highlightActor.SetProperty(Actor::Property::POSITION, mManualPosition);
    highlightActor.SetProperty(Actor::Property::SIZE, mManualSize);
  }
}

Rect<float> AccessibilityHighlightOverlay::GetOverlayExtents(Dali::Actor& sceneView, Actor& highlight)
{
  auto model = highlight.GetParent();
  if(!model)
  {
    DALI_LOG_INFO(gLogFilter, Debug::Verbose, "model is null\n");
    return Rect<float>(-1.0f, -1.0f, -1.0f, -1.0f);
  }

  auto actualSceneExtent =  DevelActor::CalculateScreenExtents(sceneView);
  auto actualExtent =  DevelActor::CalculateScreenExtents(model);
  auto x = actualExtent.x - actualSceneExtent.x + (actualExtent.width / 2);
  auto y = actualExtent.y - actualSceneExtent.y + (actualExtent.height / 2);

  return Rect<float>(x, y, actualExtent.width, actualExtent.height);
}

} // namespace Dali::Toolkit::DevelControl
