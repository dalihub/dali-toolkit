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

// CLASS HEADER
#include <dali-toolkit/internal/controls/text-controls/text-anchor-impl.h>

// EXTERNAL INCLUDES
#include <dali/devel-api/actors/actor-devel.h>
#include <dali/devel-api/object/property-helper-devel.h>
#include <dali/integration-api/debug.h>
#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/object/type-registry-helper.h>

// INTERNAL INCLUDES

// DEVEL INCLUDES
#include <dali-toolkit/devel-api/controls/control-devel.h>

using namespace Dali::Toolkit::Text;

namespace Dali
{
namespace Toolkit
{
namespace Internal
{
namespace
{
#if defined(DEBUG_ENABLED)
Debug::Filter* gLogFilter = Debug::Filter::New(Debug::NoLogging, true, "LOG_TEXT_CONTROLS");
#endif

// Type registration
BaseHandle Create()
{
  return Toolkit::TextAnchor::New();
}

// clang-format off
// Setup properties, signals and actions using the type-registry.
DALI_TYPE_REGISTRATION_BEGIN(Toolkit::TextAnchor, Toolkit::Control, Create);

DALI_PROPERTY_REGISTRATION(Toolkit, TextAnchor, "startCharacterIndex", INTEGER, START_CHARACTER_INDEX)
DALI_PROPERTY_REGISTRATION(Toolkit, TextAnchor, "endCharacterIndex",   INTEGER, END_CHARACTER_INDEX  )
DALI_PROPERTY_REGISTRATION(Toolkit, TextAnchor, "uri",                 STRING,  URI        )

DALI_TYPE_REGISTRATION_END()
// clang-format on

} // namespace

Toolkit::TextAnchor TextAnchor::New()
{
  // Create the implementation, temporarily owned by this handle on stack
  IntrusivePtr<TextAnchor> impl = new TextAnchor();

  // Pass ownership to CustomActor handle
  Toolkit::TextAnchor handle(*impl);

  // Second-phase init of the implementation
  // This can only be done after the CustomActor connection has been made...
  impl->Initialize();

  return handle;
}

Property::Value TextAnchor::GetProperty(BaseObject* object, Property::Index index)
{
  Property::Value value;

  Toolkit::TextAnchor anchor = Toolkit::TextAnchor::DownCast(Dali::BaseHandle(object));

  if(anchor)
  {
    TextAnchor& impl(GetImpl(anchor));

    switch(index)
    {
      case Toolkit::TextAnchor::Property::START_CHARACTER_INDEX:
      {
        value = impl.mStartCharacterIndex;
        break;
      }
      case Toolkit::TextAnchor::Property::END_CHARACTER_INDEX:
      {
        value = impl.mEndCharacterIndex;
        break;
      }
      case Toolkit::TextAnchor::Property::URI:
      {
        value = impl.mUri;
        break;
      }
    }
  }

  return value;
}

void TextAnchor::SetProperty(BaseObject* object, Property::Index index, const Property::Value& value)
{
  Toolkit::TextAnchor anchor = Toolkit::TextAnchor::DownCast(Dali::BaseHandle(object));

  if(anchor)
  {
    TextAnchor& impl(GetImpl(anchor));
    switch(index)
    {
      case Toolkit::TextAnchor::Property::START_CHARACTER_INDEX:
      {
        value.Get(impl.mStartCharacterIndex);
        break;
      }

      case Toolkit::TextAnchor::Property::END_CHARACTER_INDEX:
      {
        value.Get(impl.mEndCharacterIndex);
        break;
      }

      case Toolkit::TextAnchor::Property::URI:
      {
        value.Get(impl.mUri);
        break;
      }
    }
  }
}

void TextAnchor::OnInitialize()
{
  Actor self = Self();

  // Enable highlightability
  self.SetProperty(Toolkit::DevelControl::Property::ACCESSIBILITY_HIGHLIGHTABLE, true);

  DevelControl::SetAccessibilityConstructor(self, [](Dali::Actor actor) {
    return std::unique_ptr<Dali::Accessibility::Accessible>(
      new AccessibleImpl(actor, Dali::Accessibility::Role::LINK));
  });
}

TextAnchor::TextAnchor()
: Control(ControlBehaviour(CONTROL_BEHAVIOUR_DEFAULT))
{
}

TextAnchor::~TextAnchor()
{
}

int32_t TextAnchor::AccessibleImpl::GetEndIndex() const
{
  auto self = Toolkit::TextAnchor::DownCast(Self());
  return self.GetProperty(Toolkit::TextAnchor::Property::END_CHARACTER_INDEX).Get<int>();
}

int32_t TextAnchor::AccessibleImpl::GetStartIndex() const
{
  auto self = Toolkit::TextAnchor::DownCast(Self());
  return self.GetProperty(Toolkit::TextAnchor::Property::START_CHARACTER_INDEX).Get<int>();
}

int32_t TextAnchor::AccessibleImpl::GetAnchorCount() const
{
  return 1;
}

Dali::Accessibility::Accessible* TextAnchor::AccessibleImpl::GetAnchorAccessible(int32_t anchorIndex) const
{
  return Control::Impl::GetAccessibilityObject(Self());
}

std::string TextAnchor::AccessibleImpl::GetAnchorUri(int32_t anchorIndex) const
{
  auto self = Toolkit::TextAnchor::DownCast(Self());
  return self.GetProperty(Toolkit::TextAnchor::Property::URI).Get<std::string>();
}

bool TextAnchor::AccessibleImpl::IsValid() const
{
  return !GetAnchorUri(0).empty();
}

bool TextAnchor::OnAccessibilityActivated()
{
  auto uri = Self().GetProperty(Toolkit::TextAnchor::Property::URI).Get<std::string>();
  if(!uri.empty())
  {
    Dali::Actor                                  current                             = Self();
    Dali::Toolkit::Text::AnchorControlInterface* parentImplementationAnchorInterface = nullptr;
    while(!current.GetProperty<bool>(Actor::Property::IS_ROOT) && !parentImplementationAnchorInterface)
    {
      Dali::Actor            parentAsActor        = current.GetParent();
      Dali::CustomActor      parentAsCustomActor  = Dali::CustomActor::DownCast(parentAsActor);
      Dali::CustomActorImpl& parentImplementation = parentAsCustomActor.GetImplementation();
      parentImplementationAnchorInterface         = dynamic_cast<Dali::Toolkit::Text::AnchorControlInterface*>(&parentImplementation);
      current                                     = parentAsActor;
    }
    if(parentImplementationAnchorInterface)
    {
      parentImplementationAnchorInterface->AnchorClicked(uri);
      return true;
    }
    else
    {
      DALI_LOG_ERROR("TextAnchor::OnAccessibilityActivate cannot find ancestor actor implementing Dali::Toolkit::Text::AnchorControlInterface.\n");
    }
  }
  return false;
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
