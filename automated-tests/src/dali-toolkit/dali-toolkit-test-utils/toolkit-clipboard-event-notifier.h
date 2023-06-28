#ifndef DALI_TOOLKIT_CLIPBOARD_EVENT_NOTIFIER_H
#define DALI_TOOLKIT_CLIPBOARD_EVENT_NOTIFIER_H

/*
 * Copyright (c) 2019 Samsung Electronics Co., Ltd.
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
#define DALI_TEXT_CLIPBOARD_EVENT_NOTIFIER_H
#include <dali/public-api/object/base-handle.h>
#include <dali/public-api/signals/dali-signal.h>


namespace Dali
{

namespace Internal
{
namespace Adaptor
{
class TextClipboardEventNotifier;
}
}

class TextClipboardEventNotifier : public BaseHandle
{
public:
  typedef Signal< void ( TextClipboardEventNotifier& ) > TextClipboardEventSignalType;

  TextClipboardEventNotifier();
  static TextClipboardEventNotifier Get();
  ~TextClipboardEventNotifier();

  const std::string& GetContent() const;
  void SetContent( const std::string& content );
  void ClearContent();

  void EmitContentSelectedSignal();
  TextClipboardEventSignalType& ContentSelectedSignal();

  TextClipboardEventNotifier( Internal::Adaptor::TextClipboardEventNotifier* notifier );
};

} // namespace Dali

#endif // DALI_TOOLKIT_TOOLKIT_CLIPBOARD_EVENT_NOTIFIER_H
