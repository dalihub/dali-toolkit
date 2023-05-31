/*
 * Copyright (c) 2023 Samsung Electronics Co., Ltd.
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
#include <toolkit-test-application.h>

// INTERNAL INCLUDES
#include <dali-test-suite-utils.h>
#include <dali/devel-api/adaptor-framework/accessibility-bridge.h>
#include <dali/devel-api/atspi-interfaces/accessible.h>
#include <dali/devel-api/text-abstraction/font-client.h>
#include <dali/integration-api/adaptor-framework/adaptor.h>
#include <toolkit-adaptor-impl.h>
#include <toolkit-lifecycle-controller.h>

namespace Dali
{
using AdaptorImpl = Dali::Internal::Adaptor::Adaptor;

ToolkitTestApplication::ToolkitTestApplication(size_t surfaceWidth, size_t surfaceHeight, float horizontalDpi, float verticalDpi)
: TestApplication(surfaceWidth, surfaceHeight, horizontalDpi, verticalDpi, false /* Do not Initialize Core */),
  mMainWindow(new Dali::Window),
  mAdaptor(&AdaptorImpl::New()) // Need to create Adaptor first as many singletons in dali-adaptor need it
{
  // Create Core next
  CreateCore();

  // Override Scene creation in TestApplication by creating a window.
  // The window will create a Scene & surface and set up the scene's surface appropriately.
  *mMainWindow = Window::New(PositionSize(0, 0, surfaceWidth, surfaceHeight), "");
  mScene       = AdaptorImpl::GetScene(*mMainWindow);
  mScene.SetDpi(Vector2(horizontalDpi, verticalDpi));

  // Create render target for the scene
  Graphics::RenderTargetCreateInfo rtInfo{};
  rtInfo.SetExtent({mSurfaceWidth, mSurfaceHeight});
  mScene.SetSurfaceRenderTarget(rtInfo);

  mScenes.push_back(mScene);

  // Core needs to be initialized next before we start the adaptor
  InitializeCore();
  Accessibility::Accessible::SetObjectRegistry(mCore->GetObjectRegistry());

  // This will also emit the window created signals
  AdaptorImpl::GetImpl(*mAdaptor).Start(*mMainWindow);
  AdaptorImpl::GetImpl(*mAdaptor).SetApplication(*this);

  Dali::LifecycleController lifecycleController = Dali::LifecycleController::Get();
  lifecycleController.InitSignal().Emit();

  // set the DPI value for font rendering
  TextAbstraction::FontClient fontClient = Dali::TextAbstraction::FontClient::Get();
  if(fontClient)
  {
    fontClient.SetDpi(mDpi.x, mDpi.y);
  }
}

ToolkitTestApplication::~ToolkitTestApplication()
{
  // Need to delete core before we delete the adaptor.
  delete mCore;
  mCore = NULL;
}

void ToolkitTestApplication::RunIdles()
{
  AdaptorImpl::GetImpl(*mAdaptor.get()).RunIdles();
}

} // namespace Dali
