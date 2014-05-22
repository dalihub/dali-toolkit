/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
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

#include "toolkit-clipboard-event-notifier.h"

#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/object/base-object.h>
#include <dali/public-api/signals/dali-signal-v2.h>

namespace Dali
{

namespace
{
ToolkitClipboardEventNotifier* gToolkitClipboardEventNotifier(NULL);
} // unnamed namespace

namespace Internal
{

namespace Adaptor
{

/**
 * Stub for the ClipboardEventNotifier
 */
class ClipboardEventNotifier : public BaseObject
{
public: // Creation & Destruction

  static Dali::ClipboardEventNotifier Get();

  ClipboardEventNotifier();
  ClipboardEventNotifier(ToolkitClipboardEventNotifier *clipboardEventNotifier);
  ~ClipboardEventNotifier();

public: // Signals

  Dali::ClipboardEventNotifier::ClipboardEventSignalV2& SignalContentSelected()
  {
    return mClipboardSignal;
  }

private:

  ToolkitClipboardEventNotifier* mToolkitClipboardEventNotifier;
  Dali::ClipboardEventNotifier::ClipboardEventSignalV2 mClipboardSignal;

};

Dali::ClipboardEventNotifier ClipboardEventNotifier::Get()
{
  return gToolkitClipboardEventNotifier->GetClipboardEventNotifier();
}

ClipboardEventNotifier::ClipboardEventNotifier()
: mToolkitClipboardEventNotifier(NULL)
{
}

ClipboardEventNotifier::ClipboardEventNotifier(ToolkitClipboardEventNotifier *clipboardEventNotifier)
: mToolkitClipboardEventNotifier(clipboardEventNotifier)
{
}

ClipboardEventNotifier::~ClipboardEventNotifier()
{
}

} // namespace Adaptor

} // namespace Internal

////////////////////////////////////////////////////////////////////////////////////////////////////

ToolkitClipboardEventNotifier::ToolkitClipboardEventNotifier()
: mClipboardEventNotifierStub(new Internal::Adaptor::ClipboardEventNotifier(this)),
  mClipboardEventNotifier( mClipboardEventNotifierStub )
{
  gToolkitClipboardEventNotifier = this;
}

ToolkitClipboardEventNotifier::~ToolkitClipboardEventNotifier()
{
  gToolkitClipboardEventNotifier = NULL;
}

ClipboardEventNotifier ToolkitClipboardEventNotifier::GetClipboardEventNotifier()
{
  return mClipboardEventNotifier;
}

} // namespace Dali
