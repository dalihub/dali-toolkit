#ifndef DALI_TOOLKIT_SCENE_HOLDER_IMPL_H
#define DALI_TOOLKIT_SCENE_HOLDER_IMPL_H

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

#include <dali/integration-api/adaptor-framework/render-surface-interface.h>

#include <dali/integration-api/scene.h>
#include <dali/public-api/actors/layer.h>
#include <dali/public-api/object/base-object.h>

namespace Dali
{

class TestRenderSurface : public Dali::RenderSurfaceInterface
{
public:

  TestRenderSurface( PositionSize positionSize ) : mPositionSize(positionSize) {};

  PositionSize GetPositionSize() const override { return mPositionSize; };

  virtual void GetDpi( unsigned int& dpiHorizontal, unsigned int& dpiVertical ) { dpiHorizontal = dpiVertical = 96; }

  void InitializeGraphics() override {};

  void CreateSurface() override {};

  void DestroySurface() override {};

  bool ReplaceGraphicsSurface() override { return false; };

  void MoveResize( Dali::PositionSize positionSize ) override { mPositionSize = positionSize; };

  void StartRender() override {};

  bool PreRender( bool resizingSurface, const std::vector<Rect<int>>& damagedRects, Rect<int>& clippingRect  ) override { return false; };

  void PostRender()
  {
  }

  //void PostRender( bool renderToFbo, bool replacingSurface, bool resizingSurface, const std::vector<Rect<int>>& damagedRects ) override {};

  void StopRender() override {};

  void ReleaseLock() override {};

  void SetThreadSynchronization( ThreadSynchronizationInterface& threadSynchronization ) override {};

  RenderSurfaceInterface::Type GetSurfaceType() override { return RenderSurfaceInterface::WINDOW_RENDER_SURFACE; };

  void MakeContextCurrent() override {};

  Integration::DepthBufferAvailable GetDepthBufferRequired() override { return Integration::DepthBufferAvailable::FALSE; };

  Integration::StencilBufferAvailable GetStencilBufferRequired() override { return Integration::StencilBufferAvailable::FALSE; };

  int GetSurfaceOrientation() const override {return 0;};

  int GetScreenOrientation() const override {return 0;};

  void SetBackgroundColor( Vector4 color ) {};

  Vector4 GetBackgroundColor() { return Color::WHITE; };

private:
  PositionSize mPositionSize;
};

namespace Internal
{

namespace Adaptor
{

class SceneHolder : public Dali::BaseObject
{
public:

  SceneHolder( const Dali::Rect<int>& positionSize );

  virtual ~SceneHolder();

  void Add( Dali::Actor actor );

  void Remove( Dali::Actor actor );

  Dali::Layer GetRootLayer() const;

  void SetBackgroundColor( Vector4 color );

  Vector4 GetBackgroundColor() const;

  void FeedTouchPoint( Dali::TouchPoint& point, int timeStamp );

  void FeedWheelEvent( Dali::WheelEvent& wheelEvent );

  void FeedKeyEvent( Dali::KeyEvent& keyEvent );

  Dali::Integration::SceneHolder::KeyEventSignalType& KeyEventSignal();

  Dali::Integration::SceneHolder::KeyEventGeneratedSignalType& KeyEventGeneratedSignal();

  Dali::Integration::SceneHolder::TouchEventSignalType& TouchedSignal();

  Dali::Integration::SceneHolder::WheelEventSignalType& WheelEventSignal();

  Dali::Integration::SceneHolder::WheelEventGeneratedSignalType& WheelEventGeneratedSignal();

  Integration::Scene GetScene();

  Dali::RenderSurfaceInterface& GetRenderSurface();

  Dali::RenderTaskList GetRenderTaskList();

protected:

  TestRenderSurface mRenderSurface;
  Integration::Scene mScene;
};

} // namespace Adaptor

} // namespace Internal

inline Internal::Adaptor::SceneHolder& GetImplementation( Dali::Integration::SceneHolder& sceneHolder )
{
  DALI_ASSERT_ALWAYS( sceneHolder && "SceneHolder handle is empty" );
  BaseObject& object = sceneHolder.GetBaseObject();
  return static_cast<Internal::Adaptor::SceneHolder&>( object );
}

inline const Internal::Adaptor::SceneHolder& GetImplementation( const Dali::Integration::SceneHolder& sceneHolder )
{
  DALI_ASSERT_ALWAYS( sceneHolder && "SceneHolder handle is empty" );
  const BaseObject& object = sceneHolder.GetBaseObject();
  return static_cast<const Internal::Adaptor::SceneHolder&>( object );
}

} // namespace Dali

#endif // DALI_TOOLKIT_SCENE_HOLDER_IMPL_H
