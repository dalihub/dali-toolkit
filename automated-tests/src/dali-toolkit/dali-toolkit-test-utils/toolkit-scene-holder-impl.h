#ifndef DALI_TOOLKIT_SCENE_HOLDER_IMPL_H
#define DALI_TOOLKIT_SCENE_HOLDER_IMPL_H

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

#include <dali/integration-api/scene.h>
#include <dali/public-api/actors/layer.h>
#include <dali/public-api/object/base-object.h>
#include "test-render-surface.h"

namespace Dali
{

namespace Internal
{

namespace Adaptor
{

class SceneHolder : public Dali::BaseObject
{
public:
  SceneHolder(const Dali::Rect<int>& positionSize);

  virtual ~SceneHolder();

  void Add(Dali::Actor actor);

  void Remove(Dali::Actor actor);

  Dali::Layer GetRootLayer() const;

  void SetBackgroundColor(Vector4 color);

  Vector4 GetBackgroundColor() const;

  void FeedTouchPoint(Dali::TouchPoint& point, int timeStamp);

  void FeedWheelEvent(Dali::WheelEvent& wheelEvent);

  void FeedKeyEvent(Dali::KeyEvent& keyEvent);

  Dali::Integration::SceneHolder::KeyEventSignalType& KeyEventSignal();

  Dali::Integration::SceneHolder::KeyEventGeneratedSignalType& KeyEventGeneratedSignal();

  Dali::Integration::SceneHolder::TouchEventSignalType& TouchedSignal();

  Dali::Integration::SceneHolder::WheelEventSignalType& WheelEventSignal();

  Dali::Integration::SceneHolder::WheelEventGeneratedSignalType& WheelEventGeneratedSignal();

  Integration::Scene GetScene();

  Dali::Integration::RenderSurfaceInterface& GetRenderSurface();

  Dali::RenderTaskList GetRenderTaskList();

protected:
  TestRenderSurface* mRenderSurface;
  Integration::Scene mScene;
};

} // namespace Adaptor

} // namespace Internal

inline Internal::Adaptor::SceneHolder& GetImplementation(Dali::Integration::SceneHolder& sceneHolder)
{
  DALI_ASSERT_ALWAYS(sceneHolder && "SceneHolder handle is empty");
  BaseObject& object = sceneHolder.GetBaseObject();
  return static_cast<Internal::Adaptor::SceneHolder&>(object);
}

inline const Internal::Adaptor::SceneHolder& GetImplementation(const Dali::Integration::SceneHolder& sceneHolder)
{
  DALI_ASSERT_ALWAYS(sceneHolder && "SceneHolder handle is empty");
  const BaseObject& object = sceneHolder.GetBaseObject();
  return static_cast<const Internal::Adaptor::SceneHolder&>(object);
}

} // namespace Dali

#endif // DALI_TOOLKIT_SCENE_HOLDER_IMPL_H
