/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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
#include <dali-toolkit/public-api/controls/scrollable/item-view/item-layout.h>

// EXTERNAL INCLUDES
#include <dali/public-api/animation/animation.h>
#include <dali/public-api/animation/constraint.h>
#include <dali/public-api/animation/time-period.h>

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/controls/scrollable/item-view/item-view.h>

namespace
{

// Lerps between current and target using the progress
template< typename Type >
void Lerp( Type& current, const Type& target, float progress )
{
  current += ((target - current) * progress);
}

// Functors which wrap constraint functions with stored item IDs
struct WrappedQuaternionConstraint
{
  WrappedQuaternionConstraint( Dali::Toolkit::ItemLayout::QuaternionFunction wrapMe, unsigned int itemId )
  :mWrapMe(wrapMe),
   mItemId(itemId)
  {
  }

  void operator()( Dali::Quaternion& current, const Dali::PropertyInputContainer& inputs )
  {
    float offsetLayoutPosition = inputs[0]->GetFloat() + static_cast<float>(mItemId);
    float weight = inputs[3]->GetFloat();

    current = Dali::Quaternion::Slerp( current, mWrapMe( current, offsetLayoutPosition, inputs[1]->GetFloat(), inputs[2]->GetVector3() ), weight );
  }

  Dali::Toolkit::ItemLayout::QuaternionFunction mWrapMe;
  unsigned int mItemId;
};

struct WrappedVector3Constraint
{
  WrappedVector3Constraint( Dali::Toolkit::ItemLayout::Vector3Function wrapMe, unsigned int itemId )
  : mWrapMe(wrapMe),
    mItemId(itemId)
  {
  }

  void operator()( Dali::Vector3& current, const Dali::PropertyInputContainer& inputs )
  {
    float offsetLayoutPosition = inputs[0]->GetFloat() + static_cast<float>(mItemId);
    float weight = inputs[3]->GetFloat();

    Lerp( current, mWrapMe( current, offsetLayoutPosition, inputs[1]->GetFloat(), inputs[2]->GetVector3() ), weight );
  }

  Dali::Toolkit::ItemLayout::Vector3Function mWrapMe;
  unsigned int mItemId;
};

struct WrappedVector4Constraint
{
  WrappedVector4Constraint( Dali::Toolkit::ItemLayout::Vector4Function wrapMe, unsigned int itemId )
  : mWrapMe(wrapMe),
    mItemId(itemId)
  {
  }

  void operator()( Dali::Vector4& current, const Dali::PropertyInputContainer& inputs )
  {
    float offsetLayoutPosition = inputs[0]->GetFloat() + static_cast<float>(mItemId);
    float weight = inputs[3]->GetFloat();

    Lerp( current, mWrapMe( current, offsetLayoutPosition, inputs[1]->GetFloat(), inputs[2]->GetVector3() ), weight );
  }

  Dali::Toolkit::ItemLayout::Vector4Function mWrapMe;
  unsigned int mItemId;
};

struct WrappedBoolConstraint
{
  WrappedBoolConstraint( Dali::Toolkit::ItemLayout::BoolFunction wrapMe, unsigned int itemId )
  : mWrapMe(wrapMe),
    mItemId(itemId)
  {
  }

  void operator()( bool& current, const Dali::PropertyInputContainer& inputs )
  {
    float weight = inputs[3]->GetFloat();

    if ( weight >= 1.0f )
    {
      float offsetLayoutPosition = inputs[0]->GetFloat() + static_cast<float>(mItemId);
      current = mWrapMe( current, offsetLayoutPosition, inputs[1]->GetFloat(), inputs[2]->GetVector3() );
    }
  }

  Dali::Toolkit::ItemLayout::BoolFunction mWrapMe;
  unsigned int mItemId;
};

}  //Unnamed namespace

namespace Dali
{

namespace Toolkit
{

ItemLayout::ItemLayout()
: mOrientation( ControlOrientation::Up ),
  mAlphaFunction( AlphaFunction::LINEAR ),
  mWeightObject()
{
}

ItemLayout::~ItemLayout()
{
}

void ItemLayout::SetOrientation(ControlOrientation::Type orientation)
{
  mOrientation = orientation;
}

ControlOrientation::Type ItemLayout::GetOrientation() const
{
  return mOrientation;
}

float ItemLayout::GetClosestOnScreenLayoutPosition(int itemID, float currentLayoutPosition, const Vector3& layoutSize)
{
  Vector3 itemPosition = GetItemPosition( itemID, currentLayoutPosition, layoutSize );
  Vector3 itemSize;
  GetItemSize(itemID, layoutSize, itemSize);
  Vector3 onScreenArea = (layoutSize - itemSize) * 0.5f;
  if (itemPosition.x < -onScreenArea.x
      || itemPosition.x > onScreenArea.x
      || itemPosition.y < -onScreenArea.y
      || itemPosition.y > onScreenArea.y)
  {
    // item not within viewable area
    // safest thing to do here since we have no idea how the implementation will work is to return the scroll to position
    return GetItemScrollToPosition(itemID);
  }
  return currentLayoutPosition;
}

int ItemLayout::GetNextFocusItemID(int itemID, int maxItems, Dali::Toolkit::Control::KeyboardFocusNavigationDirection direction, bool loopEnabled)
{
  switch( direction )
  {
    case Control::Left:
    case Control::Up:
    {
      itemID--;
      if( itemID < 0 )
      {
        itemID = loopEnabled ? maxItems - 1 : 0;
      }
      break;
    }
    case Control::Right:
    case Control::Down:
    {
      itemID++;
      if( itemID >= maxItems )
      {
        itemID = loopEnabled ? 0 : maxItems - 1;
      }
      break;
    }
  }
  return itemID;
}

float ItemLayout::GetFlickSpeedFactor() const
{
  // By default, the speed factor while dragging and swiping is the same.
  return GetScrollSpeedFactor();
}

void ItemLayout::ApplyConstraints( Actor& actor, const int itemId, const float durationSeconds, Handle scrollPositionObject, const Actor& itemViewActor )
{
  // This just implements the default behaviour of constraint application.
  // Custom layouts can override this function to apply their custom constraints.
  Dali::Toolkit::ItemView itemView = Dali::Toolkit::ItemView::DownCast( itemViewActor );
  if(itemView && scrollPositionObject)
  {
    Property::Index scrollSpeedProperty = itemView.GetPropertyIndex("item-view-scroll-speed");
    Property::Index scrollPositionProperty = scrollPositionObject.GetPropertyIndex("scroll-position");

    // We want to animate the layout in so use a weight object to do this
    if ( !mWeightObject )
    {
      mWeightObject = WeightObject::New();
    }

    ItemLayout::Vector3Function positionConstraint;
    if (GetPositionConstraint(itemId, positionConstraint))
    {
      WrappedVector3Constraint wrapped(positionConstraint, itemId);
      Constraint constraint = Constraint::New<Vector3>( actor, Actor::Property::POSITION, wrapped );
      constraint.AddSource( Source( scrollPositionObject, scrollPositionProperty ) );
      constraint.AddSource( ParentSource( scrollSpeedProperty ) );
      constraint.AddSource( ParentSource( Actor::Property::SIZE ) );
      constraint.AddSource( Source( mWeightObject, WeightObject::WEIGHT ) );
      constraint.Apply();
    }

    ItemLayout::QuaternionFunction rotationConstraint;
    if (GetRotationConstraint(itemId, rotationConstraint))
    {
      WrappedQuaternionConstraint wrapped(rotationConstraint, itemId);

      Constraint constraint = Constraint::New<Quaternion>( actor, Actor::Property::ORIENTATION, wrapped );
      constraint.AddSource( Source( scrollPositionObject, scrollPositionProperty ) );
      constraint.AddSource( ParentSource( scrollSpeedProperty ) );
      constraint.AddSource( ParentSource( Actor::Property::SIZE ) );
      constraint.AddSource( Source( mWeightObject, WeightObject::WEIGHT ) );
      constraint.Apply();
    }

    ItemLayout::Vector3Function scaleConstraint;
    if (GetScaleConstraint(itemId, scaleConstraint))
    {
      WrappedVector3Constraint wrapped(scaleConstraint, itemId);

      Constraint constraint = Constraint::New<Vector3>( actor, Actor::Property::SCALE, wrapped );
      constraint.AddSource( Source( scrollPositionObject, scrollPositionProperty ) );
      constraint.AddSource( ParentSource( scrollSpeedProperty ) );
      constraint.AddSource( ParentSource( Actor::Property::SIZE ) );
      constraint.AddSource( Source( mWeightObject, WeightObject::WEIGHT ) );
      constraint.Apply();
    }

    ItemLayout::Vector4Function colorConstraint;
    if (GetColorConstraint(itemId, colorConstraint))
    {
      WrappedVector4Constraint wrapped(colorConstraint, itemId);

      Constraint constraint = Constraint::New<Vector4>( actor, Actor::Property::COLOR, wrapped );
      constraint.AddSource( Source( scrollPositionObject, scrollPositionProperty ) );
      constraint.AddSource( ParentSource( scrollSpeedProperty ) );
      constraint.AddSource( ParentSource( Actor::Property::SIZE ) );
      constraint.AddSource( Source( mWeightObject, WeightObject::WEIGHT ) );
      constraint.SetRemoveAction(Dali::Constraint::Discard);
      constraint.Apply();
    }

    ItemLayout::BoolFunction visibilityConstraint;
    if (GetVisibilityConstraint(itemId, visibilityConstraint))
    {
      WrappedBoolConstraint wrapped(visibilityConstraint, itemId);

      Constraint constraint = Constraint::New<bool>( actor, Actor::Property::VISIBLE, wrapped );
      constraint.AddSource( Source( scrollPositionObject, scrollPositionProperty ) );
      constraint.AddSource( ParentSource( scrollSpeedProperty ) );
      constraint.AddSource( ParentSource( Actor::Property::SIZE ) );
      constraint.AddSource( Source( mWeightObject, WeightObject::WEIGHT ) );

      // Release visibility constraints the same time as the color constraint
      constraint.SetRemoveAction(Dali::Constraint::Discard);

      constraint.Apply();
    }

    KeyFrames keyFrames = KeyFrames::New();
    keyFrames.Add( 0.0f, 0.0f );
    keyFrames.Add( 1.0f, 1.0f );

    Animation applyAnimation = Dali::Animation::New( durationSeconds );
    applyAnimation.AnimateBetween( Property( mWeightObject, WeightObject::WEIGHT ), keyFrames, mAlphaFunction, TimePeriod(durationSeconds) );
    applyAnimation.Play();
  }
}

Vector3 ItemLayout::GetItemPosition(int itemID, float currentLayoutPosition, const Vector3& layoutSize) const
{
  Vector3 itemPosition = Vector3::ZERO;

  ItemLayout::Vector3Function positionConstraint;
  if (GetPositionConstraint(itemID, positionConstraint))
  {
    itemPosition = positionConstraint(Vector3::ZERO, currentLayoutPosition + itemID, 0.0f, layoutSize);
  }

  return itemPosition;
}

void ItemLayout::SetAlphaFunction(AlphaFunction func)
{
  mAlphaFunction = func;
}

AlphaFunction ItemLayout::GetAlphaFunction() const
{
  return mAlphaFunction;
}


} // namespace Toolkit

} // namespace Dali
