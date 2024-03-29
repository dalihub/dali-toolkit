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

// CLASS HEADER
#include "alignment-impl.h"

// EXTERNAL INCLUDES
#include <dali/public-api/object/property-input.h>
#include <dali/public-api/object/type-registry-helper.h>
#include <dali/public-api/object/type-registry.h>
#include <dali/public-api/size-negotiation/relayout-container.h>

// INTERNAL INCLUDES
#include <dali-toolkit/devel-api/controls/control-devel.h>

namespace Dali
{
namespace Toolkit
{
namespace Internal
{
namespace
{
//Type Registration
BaseHandle Create()
{
  return Toolkit::Alignment::New();
}

DALI_TYPE_REGISTRATION_BEGIN(Toolkit::Alignment, Toolkit::Control, Create)
DALI_TYPE_REGISTRATION_END()

/**
 * @param padding The padding value
 * @param horizontalAlignment The horizontal alignment.
 * @param verticalAlignment The vertical alignment.
 * @param currentSize of the object
 * @param parentSize
 */
inline Vector3 GetPosition(const Toolkit::Alignment::Padding& padding, Toolkit::Alignment::Type horizontalAlignment, Toolkit::Alignment::Type verticalAlignment, const Vector2& currentSize, const Vector2& parentSize)
{
  Vector3 position(0.f, 0.f, 0.f);

  switch(horizontalAlignment)
  {
    case Dali::Toolkit::Alignment::HORIZONTAL_LEFT:
    {
      position.x += padding.left;
      break;
    }
    case Dali::Toolkit::Alignment::HORIZONTAL_RIGHT:
    {
      position.x -= padding.right;
      break;
    }
    case Dali::Toolkit::Alignment::HORIZONTAL_CENTER: // FALLTHROUGH
    default:                                          // use center as default
    {
      if(currentSize.width + padding.left + padding.right >= parentSize.width)
      {
        position.x += 0.5f * (padding.left - padding.right);
      }
      break;
    }
  }

  switch(verticalAlignment)
  {
    case Dali::Toolkit::Alignment::VERTICAL_TOP:
    {
      position.y += padding.top;
      break;
    }
    case Dali::Toolkit::Alignment::VERTICAL_BOTTOM:
    {
      position.y -= padding.bottom;
      break;
    }
    case Dali::Toolkit::Alignment::VERTICAL_CENTER: // FALLTHROUGH
    default:                                        // use center as default
    {
      if(currentSize.height + padding.top + padding.bottom >= parentSize.height)
      {
        position.y += 0.5f * (padding.top - padding.bottom);
      }
      break;
    }
  }

  return position;
}

} // namespace

Toolkit::Alignment Alignment::New(Toolkit::Alignment::Type horizontal, Toolkit::Alignment::Type vertical)
{
  // Create the implementation, temporarily owned on stack
  IntrusivePtr<Alignment> internalAlignment = new Alignment(horizontal, vertical);

  // Pass ownership to Toolkit::Alignment
  Toolkit::Alignment alignment(*internalAlignment);

  // Second-phase init of the implementation
  // This can only be done after the CustomActor connection has been made...
  internalAlignment->Initialize();

  return alignment;
}

void Alignment::SetAlignmentType(Toolkit::Alignment::Type type)
{
  // Horizontal Alignment
  if(type & Toolkit::Alignment::HORIZONTAL_RIGHT)
  {
    mHorizontal = Toolkit::Alignment::HORIZONTAL_RIGHT;
  }
  if(type & Toolkit::Alignment::HORIZONTAL_LEFT)
  {
    mHorizontal = Toolkit::Alignment::HORIZONTAL_LEFT;
  }
  if(type & Toolkit::Alignment::HORIZONTAL_CENTER)
  {
    mHorizontal = Toolkit::Alignment::HORIZONTAL_CENTER;
  }

  // Vertical Alignment
  if(type & Toolkit::Alignment::VERTICAL_BOTTOM)
  {
    mVertical = Toolkit::Alignment::VERTICAL_BOTTOM;
  }
  if(type & Toolkit::Alignment::VERTICAL_TOP)
  {
    mVertical = Toolkit::Alignment::VERTICAL_TOP;
  }
  if(type & Toolkit::Alignment::VERTICAL_CENTER)
  {
    mVertical = Toolkit::Alignment::VERTICAL_CENTER;
  }

  RelayoutRequest();
}

Toolkit::Alignment::Type Alignment::GetAlignmentType() const
{
  return Toolkit::Alignment::Type(mHorizontal | mVertical);
}

void Alignment::SetScaling(Toolkit::Alignment::Scaling scaling)
{
  mScaling = scaling;

  RelayoutRequest();
}

Toolkit::Alignment::Scaling Alignment::GetScaling() const
{
  return mScaling;
}

void Alignment::SetPadding(const Toolkit::Alignment::Padding& padding)
{
  DALI_ASSERT_ALWAYS((padding.left >= 0.f) && (padding.top >= 0.f) && (padding.right >= 0.f) && (padding.bottom >= 0.f));

  mPadding = padding;

  RelayoutRequest();
}

const Toolkit::Alignment::Padding& Alignment::GetPadding() const
{
  return mPadding;
}

void Alignment::OnInitialize()
{
  Self().SetProperty(DevelControl::Property::ACCESSIBILITY_ROLE, Dali::Accessibility::Role::FILLER);
}

void Alignment::OnRelayout(const Vector2& size, RelayoutContainer& container)
{
  // lay out the actors
  Vector3 anchorPointAndParentOrigin = Vector3::ZERO;
  anchorPointAndParentOrigin.z       = 0.5f;
  // anchorPoint.x is initialized to 0.0, which is HORIZONTAL_LEFT
  if(Toolkit::Alignment::HORIZONTAL_CENTER == mHorizontal)
  {
    anchorPointAndParentOrigin.x = 0.5f;
  }
  else if(Toolkit::Alignment::HORIZONTAL_RIGHT == mHorizontal)
  {
    anchorPointAndParentOrigin.x = 1.0f;
  }
  // anchorPoint.y is initialized to 0.0, which is VERTICAL_TOP
  if(Toolkit::Alignment::VERTICAL_CENTER == mVertical)
  {
    anchorPointAndParentOrigin.y = 0.5f;
  }
  else if(Toolkit::Alignment::VERTICAL_BOTTOM == mVertical)
  {
    anchorPointAndParentOrigin.y = 1.0f;
  }

  for(unsigned int i = 0, childCount = Self().GetChildCount(); i < childCount; ++i)
  {
    Actor child = Self().GetChildAt(i);

    child.SetProperty(Actor::Property::ANCHOR_POINT, anchorPointAndParentOrigin);
    child.SetProperty(Actor::Property::PARENT_ORIGIN, anchorPointAndParentOrigin);

    Vector2 currentChildSize(child.GetTargetSize().GetVectorXY());
    if(currentChildSize == Vector2::ZERO)
    {
      currentChildSize = child.GetNaturalSize();
    }

    bool    renegotiate = true;
    Vector2 newChildSize;
    newChildSize.width  = size.width - (mPadding.left + mPadding.right);
    newChildSize.height = size.height - (mPadding.top + mPadding.bottom);

    // prevent ridiculous sizes if parent is really small or if we don't have a proper size for the actor
    if((newChildSize.width > Math::MACHINE_EPSILON_1000) && (newChildSize.height > Math::MACHINE_EPSILON_1000) &&
       (currentChildSize.width > Math::MACHINE_EPSILON_1000) && (currentChildSize.height > Math::MACHINE_EPSILON_1000))
    {
      // no point trying to squeeze actors into too small size
      switch(mScaling)
      {
        case Toolkit::Alignment::SCALE_NONE:
        {
          // Nothing to do
          renegotiate = false;
          break;
        }
        case Toolkit::Alignment::SCALE_TO_FILL:
        {
          // Nothing to do, newChildSize is already full size minus padding
          break;
        }
        case Toolkit::Alignment::SCALE_TO_FIT_KEEP_ASPECT:
        {
          newChildSize = currentChildSize * std::min((newChildSize.width / currentChildSize.width), (newChildSize.height / currentChildSize.height));
          break;
        }
        case Toolkit::Alignment::SCALE_TO_FILL_KEEP_ASPECT:
        {
          newChildSize = currentChildSize * std::max((newChildSize.width / currentChildSize.width), (newChildSize.height / currentChildSize.height));
          break;
        }
        case Toolkit::Alignment::SHRINK_TO_FIT:
        {
          newChildSize = Vector2(std::min(newChildSize.width, currentChildSize.width), std::min(newChildSize.height, currentChildSize.height));
          break;
        }
        case Toolkit::Alignment::SHRINK_TO_FIT_KEEP_ASPECT:
        {
          // check source size vs target size to see if we need to shrink
          float widthScale  = (newChildSize.width < currentChildSize.width) ? (newChildSize.width / currentChildSize.width) : 1.f;
          float heightScale = (newChildSize.height < currentChildSize.height) ? (newChildSize.height / currentChildSize.height) : 1.0f;
          // use smaller of the scales
          float scale = std::min(widthScale, heightScale);
          // check if we need to scale
          if(scale < 1.0f)
          {
            // scale natural size to fit inside
            newChildSize *= scale;
          }
          break;
        }
      }
    }

    child.SetProperty(Actor::Property::POSITION, GetPosition(mPadding, mHorizontal, mVertical, newChildSize, currentChildSize));

    if(renegotiate)
    {
      container.Add(child, newChildSize);
    }
  }
}

Alignment::Alignment(Toolkit::Alignment::Type horizontal, Toolkit::Alignment::Type vertical)
: Control(ControlBehaviour(CONTROL_BEHAVIOUR_DEFAULT)),
  mHorizontal(horizontal),
  mVertical(vertical),
  mScaling(Toolkit::Alignment::SCALE_NONE),
  mPadding(0.f, 0.f, 0.f, 0.f)
{
}

Alignment::~Alignment()
{
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
