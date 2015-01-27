#ifndef __DALI_TOOLKIT_CLIPBOARD_EVENT_NOTIFIER_H__
#define __DALI_TOOLKIT_CLIPBOARD_EVENT_NOTIFIER_H__

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

// EXTERNAL INCLUDES
#include <string>

// PUBLIC INCLUDES
#define __DALI_CLIPBOARD_EVENT_NOTIFIER_H__
#include <dali/public-api/object/base-handle.h>
#include <dali/public-api/signals/dali-signal-v2.h>


namespace Dali
{

namespace Internal
{
namespace Adaptor
{
class ClipboardEventNotifier;
}
}

class ClipboardEventNotifier : public BaseHandle
{
public:
  typedef SignalV2< void ( ClipboardEventNotifier& ) > ClipboardEventSignalV2;

  ClipboardEventNotifier();
  static ClipboardEventNotifier Get();
  ~ClipboardEventNotifier();

  const std::string& GetContent() const;
  void SetContent( const std::string& content );
  void ClearContent();

  void EmitContentSelectedSignal();
  ClipboardEventSignalV2& ContentSelectedSignal();

  ClipboardEventNotifier( Internal::Adaptor::ClipboardEventNotifier* notifier );
};

} // namespace Dali

#endif // __DALI_TOOLKIT_TOOLKIT_CLIPBOARD_EVENT_NOTIFIER_H__
