/*
 * Copyright (c) 2018 Samsung Electronics Co., Ltd.
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

#include <dali-toolkit/public-api/dali-toolkit-common.h>
#include <dali/public-api/object/base-object.h>
#include <dali/public-api/signals/dali-signal.h>

namespace Dali
{

namespace Internal
{

namespace Adaptor
{

class TextClipboardEventNotifier : public Dali::BaseObject
{
public:

  typedef Dali::TextClipboardEventNotifier::TextClipboardEventSignalType TextClipboardEventSignalType;

  // Creation
  static Dali::TextClipboardEventNotifier New();
  static Dali::TextClipboardEventNotifier Get();

  // Public API
  const std::string& GetContent() const;
  void SetContent( const std::string& content );
  void ClearContent();
  void EmitContentSelectedSignal();

  // Signals
  TextClipboardEventSignalType& ContentSelectedSignal()
  {
    return mContentSelectedSignal;
  }

private:
  // Construction & Destruction
  TextClipboardEventNotifier();
  virtual ~TextClipboardEventNotifier();

  // Undefined
  TextClipboardEventNotifier( const TextClipboardEventNotifier& );
  TextClipboardEventNotifier& operator=( TextClipboardEventNotifier& );

private:

  std::string mContent;    ///< The current selected content.
  TextClipboardEventSignalType mContentSelectedSignal;

  static Dali::TextClipboardEventNotifier mToolkitClipboardEventNotifier;

public:

  // Helpers for public-api forwarding methods

  inline static Internal::Adaptor::TextClipboardEventNotifier& GetImplementation(Dali::TextClipboardEventNotifier& detector)
  {
    DALI_ASSERT_ALWAYS( detector && "TextClipboardEventNotifier handle is empty" );

    BaseObject& handle = detector.GetBaseObject();

    return static_cast<Internal::Adaptor::TextClipboardEventNotifier&>(handle);
  }

  inline static const Internal::Adaptor::TextClipboardEventNotifier& GetImplementation(const Dali::TextClipboardEventNotifier& detector)
  {
    DALI_ASSERT_ALWAYS( detector && "TextClipboardEventNotifier handle is empty" );

    const BaseObject& handle = detector.GetBaseObject();

    return static_cast<const Internal::Adaptor::TextClipboardEventNotifier&>(handle);
  }

};

Dali::TextClipboardEventNotifier TextClipboardEventNotifier::mToolkitClipboardEventNotifier;

Dali::TextClipboardEventNotifier TextClipboardEventNotifier::New()
{
  return Get();
}

Dali::TextClipboardEventNotifier TextClipboardEventNotifier::Get()
{
  if ( !mToolkitClipboardEventNotifier )
  {
    mToolkitClipboardEventNotifier = Dali::TextClipboardEventNotifier( new TextClipboardEventNotifier );
  }
  return mToolkitClipboardEventNotifier;
}

const std::string& TextClipboardEventNotifier::GetContent() const
{
  return mContent;
}

void TextClipboardEventNotifier::SetContent( const std::string& content )
{
  mContent = content;
}

void TextClipboardEventNotifier::ClearContent()
{
  mContent.clear();
}

void TextClipboardEventNotifier::EmitContentSelectedSignal()
{
  if ( !mContentSelectedSignal.Empty() )
  {
    Dali::TextClipboardEventNotifier handle( this );
    mContentSelectedSignal.Emit( handle );
  }
}

TextClipboardEventNotifier::TextClipboardEventNotifier()
: mContent()
{
}

TextClipboardEventNotifier::~TextClipboardEventNotifier()
{
}

} // namespace Adaptor
} // namespace Internal

////////////////////////////////////////////////////////////////////////////////////////////////////

TextClipboardEventNotifier::TextClipboardEventNotifier()
{
}

TextClipboardEventNotifier TextClipboardEventNotifier::Get()
{
  return Internal::Adaptor::TextClipboardEventNotifier::Get();
}

TextClipboardEventNotifier::~TextClipboardEventNotifier()
{
}

const std::string& TextClipboardEventNotifier::GetContent() const
{
  return Internal::Adaptor::TextClipboardEventNotifier::GetImplementation(*this).GetContent();
}

void TextClipboardEventNotifier::SetContent( const std::string& content )
{
  Internal::Adaptor::TextClipboardEventNotifier::GetImplementation(*this).SetContent(content);
}

void TextClipboardEventNotifier::ClearContent()
{
  Internal::Adaptor::TextClipboardEventNotifier::GetImplementation(*this).ClearContent();
}

void TextClipboardEventNotifier::EmitContentSelectedSignal()
{
  Internal::Adaptor::TextClipboardEventNotifier::GetImplementation(*this).EmitContentSelectedSignal();
}

TextClipboardEventNotifier::TextClipboardEventSignalType& TextClipboardEventNotifier::ContentSelectedSignal()
{
  return Internal::Adaptor::TextClipboardEventNotifier::GetImplementation(*this).ContentSelectedSignal();
}

TextClipboardEventNotifier::TextClipboardEventNotifier( Internal::Adaptor::TextClipboardEventNotifier* notifier )
: BaseHandle( notifier )
{
}

} // namespace Dali
