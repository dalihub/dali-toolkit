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

namespace Internal
{

namespace Adaptor
{

class ClipboardEventNotifier : public Dali::BaseObject
{
public:

  typedef Dali::ClipboardEventNotifier::ClipboardEventSignalV2 ClipboardEventSignalV2;

  // Creation
  static Dali::ClipboardEventNotifier New();
  static Dali::ClipboardEventNotifier Get();

  // Public API
  const std::string& GetContent() const;
  void SetContent( const std::string& content );
  void ClearContent();
  void EmitContentSelectedSignal();

  // Signals
  ClipboardEventSignalV2& ContentSelectedSignal()
  {
    return mContentSelectedSignalV2;
  }

private:
  // Construction & Destruction
  ClipboardEventNotifier();
  virtual ~ClipboardEventNotifier();

  // Undefined
  ClipboardEventNotifier( const ClipboardEventNotifier& );
  ClipboardEventNotifier& operator=( ClipboardEventNotifier& );

private:

  std::string mContent;    ///< The current selected content.
  ClipboardEventSignalV2 mContentSelectedSignalV2;

  static Dali::ClipboardEventNotifier mToolkitClipboardEventNotifier;

public:

  // Helpers for public-api forwarding methods

  inline static Internal::Adaptor::ClipboardEventNotifier& GetImplementation(Dali::ClipboardEventNotifier& detector)
  {
    DALI_ASSERT_ALWAYS( detector && "ClipboardEventNotifier handle is empty" );

    BaseObject& handle = detector.GetBaseObject();

    return static_cast<Internal::Adaptor::ClipboardEventNotifier&>(handle);
  }

  inline static const Internal::Adaptor::ClipboardEventNotifier& GetImplementation(const Dali::ClipboardEventNotifier& detector)
  {
    DALI_ASSERT_ALWAYS( detector && "ClipboardEventNotifier handle is empty" );

    const BaseObject& handle = detector.GetBaseObject();

    return static_cast<const Internal::Adaptor::ClipboardEventNotifier&>(handle);
  }

};

Dali::ClipboardEventNotifier ClipboardEventNotifier::mToolkitClipboardEventNotifier;

Dali::ClipboardEventNotifier ClipboardEventNotifier::New()
{
  return Get();
}

Dali::ClipboardEventNotifier ClipboardEventNotifier::Get()
{
  if ( !mToolkitClipboardEventNotifier )
  {
    mToolkitClipboardEventNotifier = Dali::ClipboardEventNotifier( new ClipboardEventNotifier );
  }
  return mToolkitClipboardEventNotifier;
}

const std::string& ClipboardEventNotifier::GetContent() const
{
  return mContent;
}

void ClipboardEventNotifier::SetContent( const std::string& content )
{
  mContent = content;
}

void ClipboardEventNotifier::ClearContent()
{
  mContent.clear();
}

void ClipboardEventNotifier::EmitContentSelectedSignal()
{
  if ( !mContentSelectedSignalV2.Empty() )
  {
    Dali::ClipboardEventNotifier handle( this );
    mContentSelectedSignalV2.Emit( handle );
  }
}

ClipboardEventNotifier::ClipboardEventNotifier()
: mContent()
{
}

ClipboardEventNotifier::~ClipboardEventNotifier()
{
}

} // namespace Adaptor
} // namespace Internal

////////////////////////////////////////////////////////////////////////////////////////////////////

const char* const ClipboardEventNotifier::SIGNAL_CONTENT_SELECTED( "content-selected" );

ClipboardEventNotifier::ClipboardEventNotifier()
{
}

ClipboardEventNotifier ClipboardEventNotifier::Get()
{
  return Internal::Adaptor::ClipboardEventNotifier::Get();
}

ClipboardEventNotifier::~ClipboardEventNotifier()
{
}

const std::string& ClipboardEventNotifier::GetContent() const
{
  return Internal::Adaptor::ClipboardEventNotifier::GetImplementation(*this).GetContent();
}

void ClipboardEventNotifier::SetContent( const std::string& content )
{
  Internal::Adaptor::ClipboardEventNotifier::GetImplementation(*this).SetContent(content);
}

void ClipboardEventNotifier::ClearContent()
{
  Internal::Adaptor::ClipboardEventNotifier::GetImplementation(*this).ClearContent();
}

void ClipboardEventNotifier::EmitContentSelectedSignal()
{
  Internal::Adaptor::ClipboardEventNotifier::GetImplementation(*this).EmitContentSelectedSignal();
}

ClipboardEventNotifier::ClipboardEventSignalV2& ClipboardEventNotifier::ContentSelectedSignal()
{
  return Internal::Adaptor::ClipboardEventNotifier::GetImplementation(*this).ContentSelectedSignal();
}

ClipboardEventNotifier::ClipboardEventNotifier( Internal::Adaptor::ClipboardEventNotifier* notifier )
: BaseHandle( notifier )
{
}

} // namespace Dali
