//
// Copyright (c) 2014 Samsung Electronics Co., Ltd.
//
// Licensed under the Flora License, Version 1.0 (the License);
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://floralicense.org/license/
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an AS IS BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

// CLASS HEADER
#include <dali-toolkit/internal/controls/scroll-component/scroll-component-impl.h>
#include <dali-toolkit/internal/controls/scroll-component/scroll-bar-internal.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

namespace
{
BaseHandle Create()
{
  // empty handle as we cannot create ScrollComponent
  return Toolkit::ScrollComponent::New();
}

TypeRegistration mType( typeid(Toolkit::ScrollComponent), typeid(Toolkit::Control), Create );

}

ScrollComponent::ScrollComponent()
: ControlImpl(true/*requires touch*/)
{

}

ScrollComponent::~ScrollComponent()
{
}

Toolkit::ScrollComponent ScrollComponent::New(Toolkit::Scrollable& scrollable, Toolkit::Scrollable::ScrollComponentType type)
{
  Toolkit::ScrollComponent instance;

  switch(type)
  {
    case Toolkit::Scrollable::VerticalScrollBar:
    {
      instance = static_cast<Toolkit::ScrollComponent>(Toolkit::ScrollBarInternal::New(scrollable, true));
      break;
    }
    case Toolkit::Scrollable::HorizontalScrollBar:
    {
      instance = static_cast<Toolkit::ScrollComponent>(Toolkit::ScrollBarInternal::New(scrollable, false));
      break;
    }
    default:
    {
      DALI_ASSERT_ALWAYS(true && "Unrecognized component type");
    }
  }

  return instance;
}


} // namespace Internal

} // namespace Toolkit

} // namespace Dali
