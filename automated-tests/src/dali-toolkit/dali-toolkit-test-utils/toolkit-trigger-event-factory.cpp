
#include <dali/integration-api/adaptor-framework/trigger-event-factory.h>
#include <dali/integration-api/adaptor-framework/trigger-event-interface.h>
#include <dali/public-api/signals/callback.h>

namespace Dali
{

namespace Internal
{

namespace Toolkit
{

class TriggerEvent : public Dali::TriggerEventInterface
{
public:
  TriggerEvent(Dali::CallbackBase* callback, Dali::TriggerEventInterface::Options Options);
  ~TriggerEvent()
  {
  }

  void Trigger();

private:
  Dali::CallbackBase* mCallback;
};

TriggerEvent::TriggerEvent(Dali::CallbackBase* callback, Dali::TriggerEventInterface::Options Options)
: mCallback(callback)
{
}

void TriggerEvent::Trigger()
{
  Dali::CallbackBase::Execute(*mCallback);
}

} // namespace Toolkit

} // namespace Internal

} // namespace Dali

Dali::TriggerEventFactory::TriggerEventPtr Dali::TriggerEventFactory::CreateTriggerEvent(Dali::CallbackBase* callback, Dali::TriggerEventInterface::Options options)
{
  return TriggerEventFactory::TriggerEventPtr(new Dali::Internal::Toolkit::TriggerEvent(callback, options));
}

void Dali::TriggerEventFactory::DestroyTriggerEvent(Dali::TriggerEventInterface* triggerEventInterface)
{
  Dali::Internal::Toolkit::TriggerEvent* triggerEvent(static_cast<Dali::Internal::Toolkit::TriggerEvent*>(triggerEventInterface));
  delete triggerEvent;
}
