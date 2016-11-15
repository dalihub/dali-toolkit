#ifndef __DALI_TOOLKIT_ADAPTOR_IMPL_H__
#define __DALI_TOOLKIT_ADAPTOR_IMPL_H__

/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
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

#include <dali/devel-api/adaptor-framework/render-surface.h>

namespace Dali
{
class EglInterface;
class DisplayConnection;
class ThreadSynchronizationInterface;

namespace Integration
{

class GlAbstraction;

} // namespace Integration

class TestRenderSurface : public RenderSurface
{
public:
  virtual PositionSize GetPositionSize() const { PositionSize size; return size; }

  virtual void InitializeEgl( EglInterface& egl ) {}

  virtual void CreateEglSurface( EglInterface& egl ) {}

  virtual void DestroyEglSurface( EglInterface& egl ) {}

  virtual bool ReplaceEGLSurface( EglInterface& egl ) { return false; }

  virtual void MoveResize( Dali::PositionSize positionSize ) {}

  virtual void SetViewMode( ViewMode viewMode ) {}

  virtual void StartRender() {}

  virtual bool PreRender( EglInterface& egl, Integration::GlAbstraction& glAbstraction ) { return false; }

  virtual void PostRender( EglInterface& egl, Integration::GlAbstraction& glAbstraction, DisplayConnection* displayConnection, bool replacingSurface ) {}

  virtual void StopRender() {}

  virtual void ReleaseLock() {}

  virtual void SetThreadSynchronization( ThreadSynchronizationInterface& threadSynchronization ) {}

  virtual RenderSurface::Type GetSurfaceType() { return RenderSurface::ECORE_RENDER_SURFACE; }
};

namespace Internal
{
namespace Adaptor
{

class Adaptor: public BaseObject
{
public:
  static Dali::Adaptor& Get();
  Adaptor();
  ~Adaptor();

public:
  static Dali::RenderSurface& GetSurface();
  static Dali::Adaptor::AdaptorSignalType& AdaptorSignal();
  static bool mAvailable;
  static Vector<CallbackBase*> mCallbacks;
};

} // namespace Adaptor
} // namespace Internal
} // namespace Dali

#endif // __DALI_TOOLKIT_ADAPTOR_IMPL_H__
