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
 */

#include <dali/integration-api/adaptor-framework/adaptor.h>

#include <dali/devel-api/common/singleton-service.h>
#include <dali/public-api/adaptor-framework/ui-context.h>
#include <dali/public-api/object/base-object.h>
#include <dali/public-api/update/frame-callback-interface.h>
#include <dali/public-api/update/update-proxy.h>
#include <toolkit-adaptor-impl.h>

namespace Dali
{
namespace Internal
{
namespace Adaptor
{
extern Dali::Adaptor* gAdaptor;

class UiContext : public Dali::BaseObject
{
public:
  static Dali::UiContext Get()
  {
    Dali::UiContext uiContext;

    Dali::SingletonService service(Dali::SingletonService::Get());
    if(service)
    {
      Dali::BaseHandle handle = service.GetSingleton(typeid(Dali::UiContext));
      if(handle)
      {
        uiContext = Dali::UiContext(static_cast<UiContext*>(handle.GetObjectPtr()));
      }
      else
      {
        auto* impl = new UiContext();
        uiContext  = Dali::UiContext(impl);
        service.Register(typeid(uiContext), uiContext);
      }
    }
    return uiContext;
  }

  void AddFrameCallback(FrameCallbackInterface& frameCallback, Dali::Actor rootActor)
  {
    if(gAdaptor)
    {
      Adaptor::GetImpl(*gAdaptor).AddFrameCallback(frameCallback, rootActor);
    }
  }

  void RemoveFrameCallback(FrameCallbackInterface& frameCallback)
  {
    if(gAdaptor)
    {
      Adaptor::GetImpl(*gAdaptor).RemoveFrameCallback(frameCallback);
    }
  }

  Dali::UpdateProxy::NotifySyncPoint NotifyFrameCallback(FrameCallbackInterface& frameCallback)
  {
    if(gAdaptor)
    {
      return Adaptor::GetImpl(*gAdaptor).NotifyFrameCallback(frameCallback);
    }
    return Dali::UpdateProxy::INVALID_SYNC;
  }

private:
  UiContext()  = default;
  ~UiContext() = default;
};

} // namespace Adaptor
} // namespace Internal

// Override public UiContext methods so that calls from dali2-physics-3d.so
// (or any other DSO) are dispatched to our stub via the main executable's PLT.
// Without these overrides, DSOs would resolve the symbols to dali2-adaptor.so
// and crash because the stub's mAdaptor is not a real adaptor.

UiContext UiContext::Get()
{
  return Internal::Adaptor::UiContext::Get();
}

void UiContext::AddFrameCallback(FrameCallbackInterface& frameCallback, Actor rootActor)
{
  if(Internal::Adaptor::gAdaptor)
  {
    Internal::Adaptor::Adaptor::GetImpl(*Internal::Adaptor::gAdaptor).AddFrameCallback(frameCallback, rootActor);
  }
}

void UiContext::RemoveFrameCallback(FrameCallbackInterface& frameCallback)
{
  if(Internal::Adaptor::gAdaptor)
  {
    Internal::Adaptor::Adaptor::GetImpl(*Internal::Adaptor::gAdaptor).RemoveFrameCallback(frameCallback);
  }
}

UpdateProxy::NotifySyncPoint UiContext::NotifyFrameCallback(FrameCallbackInterface& frameCallback)
{
  if(Internal::Adaptor::gAdaptor)
  {
    return Internal::Adaptor::Adaptor::GetImpl(*Internal::Adaptor::gAdaptor).NotifyFrameCallback(frameCallback);
  }
  return UpdateProxy::INVALID_SYNC;
}

} // namespace Dali
