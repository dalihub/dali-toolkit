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
#include <dali/public-api/object/type-registry.h>
#include <dali/public-api/object/type-registry-helper.h>
#include <dali/public-api/size-negotiation/relayout-container.h>

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

DALI_TYPE_REGISTRATION_BEGIN( Toolkit::Alignment, Toolkit::Control, Create )
DALI_TYPE_REGISTRATION_END()

struct ScaleToFillConstraint
{
  /**
   * @param padding to be added.
   */
  ScaleToFillConstraint( const Toolkit::Alignment::Padding& padding )
  : mPadding( padding )
  {}

  /**
   * Called by render thread
   */
  Vector3 operator()( const Vector3& currentSize,
                      const PropertyInput& parentSizeProperty )
  {
    const Vector3& parentSize( parentSizeProperty.GetVector3() );
    return GetSize( currentSize, parentSize );
  }

  inline Vector3 GetSize( const Vector3& currentSize, const Vector3& parentSize )
  {
    const float parentSizeWidth = parentSize.width - ( mPadding.left + mPadding.right );
    const float parentSizeHeight = parentSize.height - ( mPadding.top + mPadding.bottom );

    // prevent ridiculous sizes if parent is really small or if we don't have a proper size for the actor
    if( ( parentSizeWidth < Math::MACHINE_EPSILON_1000 ) || ( parentSizeHeight < Math::MACHINE_EPSILON_1000 ) )
    {
      // no point trying to squeeze actors into this small size
      return Vector3::ZERO;
    }
    return  Vector3( parentSizeWidth, parentSizeHeight, parentSize.depth );
  }

  const Toolkit::Alignment::Padding mPadding;
};

struct ScaleToFitKeepAspectConstraint
{
  /**
   * @param padding to be added.
   */
  ScaleToFitKeepAspectConstraint( const Toolkit::Alignment::Padding& padding )
  : mPadding( padding ),
    mSizeStored( false ),
    mOriginalSize()
  {}

  /**
   * Called by render thread
   */
  Vector3 operator()( const Vector3& currentSize,
                      const PropertyInput& parentSizeProperty )
  {
    const Vector3& parentSize( parentSizeProperty.GetVector3() );
    return GetSize( currentSize, parentSize );
  }

  inline Vector3 GetSize( const Vector3& currentSize, const Vector3& parentSize )
  {
    if( ( !mSizeStored ) && ( Vector3::ZERO != currentSize ) )
    {
      mOriginalSize = currentSize;
      mSizeStored = true;
    }

    const float parentSizeWidth = parentSize.width - ( mPadding.left + mPadding.right );
    const float parentSizeHeight = parentSize.height - ( mPadding.top + mPadding.bottom );

    // prevent ridiculous sizes if parent is really small or if we don't have a proper size for the actor
    if( ( parentSizeWidth < Math::MACHINE_EPSILON_1000 ) || ( parentSizeHeight < Math::MACHINE_EPSILON_1000 )||
        ( mOriginalSize.width < Math::MACHINE_EPSILON_1000 ) || ( mOriginalSize.height < Math::MACHINE_EPSILON_1000 ) )
    {
      // no point trying to squeeze actors into this small size
      return Vector3::ZERO;
    }

    return mOriginalSize * std::min( ( parentSizeWidth / mOriginalSize.width ), ( parentSizeHeight / mOriginalSize.height ) );
  }

  const Toolkit::Alignment::Padding mPadding;
        bool                        mSizeStored;
        Vector3                     mOriginalSize;
};

struct ScaleToFillKeepAspectConstraint
{
  /**
   * @param padding to be added.
   */
  ScaleToFillKeepAspectConstraint( const Toolkit::Alignment::Padding& padding )
  : mPadding( padding ),
    mSizeStored( false ),
    mOriginalSize()
  { }

  /**
   * Called by render thread
   */
  Vector3 operator()( const Vector3& currentSize,
                      const PropertyInput& parentSizeProperty )
  {
    const Vector3& parentSize( parentSizeProperty.GetVector3() );
    return GetSize( currentSize, parentSize );
  }

  Vector3 GetSize( const Vector3& currentSize, const Vector3& parentSize )
  {
    if( ( !mSizeStored ) && ( Vector3::ZERO != currentSize ) )
    {
      mOriginalSize = currentSize;
      mSizeStored = true;
    }

    const float parentSizeWidth = parentSize.width - ( mPadding.left + mPadding.right );
    const float parentSizeHeight = parentSize.height - ( mPadding.top + mPadding.bottom );

    // prevent ridiculous sizes if parent is really small or if we don't have a proper size for the actor
    if( ( parentSizeWidth < Math::MACHINE_EPSILON_1000 ) || ( parentSizeHeight < Math::MACHINE_EPSILON_1000 )||
        ( mOriginalSize.width < Math::MACHINE_EPSILON_1000 ) || ( mOriginalSize.height < Math::MACHINE_EPSILON_1000 ) )
    {
      // no point trying to squeeze actors into this small size
      return Vector3::ZERO;
    }

    return mOriginalSize * std::max( ( parentSizeWidth / mOriginalSize.width ), ( parentSizeHeight / mOriginalSize.height ) );
  }

  const Toolkit::Alignment::Padding mPadding;
        bool                        mSizeStored;
        Vector3                     mOriginalSize;
};

struct ShrinkToFitConstraint
{
  /**
   * @param padding to be added.
   */
  ShrinkToFitConstraint( const Toolkit::Alignment::Padding& padding )
  : mPadding( padding ),
    mSizeStored( false ),
    mOriginalSize()
  {}

  /**
   * Called by render thread
   */
  Vector3 operator()( const Vector3& currentSize,
                      const PropertyInput& parentSizeProperty )
  {
    const Vector3& parentSize( parentSizeProperty.GetVector3() );
    return GetSize( currentSize, parentSize );
  }

  Vector3 GetSize( const Vector3& currentSize, const Vector3& parentSize )
  {
    if( ( !mSizeStored ) && ( Vector3::ZERO != currentSize ) )
    {
      mOriginalSize = currentSize;
      mSizeStored = true;
    }

    const float parentSizeWidth = parentSize.width - ( mPadding.left + mPadding.right );
    const float parentSizeHeight = parentSize.height - ( mPadding.top + mPadding.bottom );

    // prevent ridiculous sizes if parent is really small or if we don't have a proper size for the actor
    if( ( parentSizeWidth < Math::MACHINE_EPSILON_1000 ) || ( parentSizeHeight < Math::MACHINE_EPSILON_1000 )||
        ( mOriginalSize.width < Math::MACHINE_EPSILON_1000 ) || ( mOriginalSize.height < Math::MACHINE_EPSILON_1000 ) )
    {
      // no point trying to squeeze actors into this small size
      return Vector3::ZERO;
    }

    return Vector3( std::min( parentSizeWidth, mOriginalSize.width ), std::min( parentSizeHeight, mOriginalSize.height ), std::min( parentSize.depth, mOriginalSize.depth ) );
  }

  const Toolkit::Alignment::Padding mPadding;
        bool                        mSizeStored;
        Vector3                     mOriginalSize;
};

/**
 * Constraint that uses naturalSize if it fits inside parent and parent size if not. It also adds some padding pixels
 */
struct ShrinkToFitKeepAspectConstraint
{
  /**
   * @param padding to be added.
   */
  ShrinkToFitKeepAspectConstraint( const Toolkit::Alignment::Padding& padding )
  : mPadding( padding ),
    mSizeStored( false ),
    mOriginalSize()
  {}

  /**
   * Called by render thread
   */
  Vector3 operator()( const Vector3& currentSize,
                      const PropertyInput& parentSizeProperty )
  {
    const Vector3& parentSize( parentSizeProperty.GetVector3() );
    return GetSize( currentSize, parentSize );
  }

  inline Vector3 GetSize( const Vector3& currentSize, const Vector3& parentSize )
  {
    if( ( !mSizeStored ) && ( Vector3::ZERO != currentSize ) )
    {
      mOriginalSize = currentSize;
      mSizeStored = true;
    }

    const float parentSizeWidth = parentSize.width - ( mPadding.left + mPadding.right );
    const float parentSizeHeight = parentSize.height - ( mPadding.top + mPadding.bottom );

    // prevent ridiculous sizes if parent is really small or if we don't have a proper size for the actor
    if( ( parentSizeWidth < Math::MACHINE_EPSILON_1000 ) || ( parentSizeHeight < Math::MACHINE_EPSILON_1000 )||
        ( mOriginalSize.width < Math::MACHINE_EPSILON_1000 ) || ( mOriginalSize.height < Math::MACHINE_EPSILON_1000 ) )
    {
      // no point trying to squeeze actors into this small size
      return Vector3::ZERO;
    }

    return Vector3( ShrinkInside( Vector2( parentSizeWidth, parentSizeHeight ), Vector2( mOriginalSize ) ) );
  }

  const Toolkit::Alignment::Padding mPadding;
        bool                        mSizeStored;
        Vector3                     mOriginalSize;
};

/**
 * Constraint that modifies the contained actor taking into account the padding value.
 */
struct PositionConstraint
{
  /**
   * @param padding The padding value
   * @param horizontalAlignment The horizontal alignment.
   * @param verticalAlignment The vertical alignment.
   */
  PositionConstraint( const Toolkit::Alignment::Padding& padding, Toolkit::Alignment::Type horizontalAlignment, Toolkit::Alignment::Type verticalAlignment )
  : mPadding( padding ),
    mHorizontalAlignment( horizontalAlignment ),
    mVerticalAlignment( verticalAlignment )
  {}

  /**
   * Called by render thread.
   */
  Vector3 operator()( const Vector3& currentPosition,
                      const PropertyInput& currentSizeProperty,
                      const PropertyInput& parentSizeProperty )
  {
    const Vector3& currentSize( currentSizeProperty.GetVector3() );
    const Vector3& parentSize( parentSizeProperty.GetVector3() );

    return GetPosition( currentSize, parentSize );
  }

  inline Vector3 GetPosition( const Vector3& currentSize, const Vector3& parentSize )
  {
    Vector3 position( 0.f, 0.f, 0.f );

    switch( mHorizontalAlignment )
    {
    case Dali::Toolkit::Alignment::HorizontalLeft:
    {
      position.x += mPadding.left;
      break;
    }
    case Dali::Toolkit::Alignment::HorizontalCenter:
    {
      if( currentSize.width + mPadding.left + mPadding.right >= parentSize.width )
      {
        position.x += 0.5f * ( mPadding.left - mPadding.right );
      }
      break;
    }
    case Dali::Toolkit::Alignment::HorizontalRight:
    {
      position.x -= mPadding.right;
      break;
    }
    default:
    {
      DALI_ASSERT_ALWAYS( !"Wrong horizontal alignment value" );
      break;
    }
    }

    switch( mVerticalAlignment )
    {
    case Dali::Toolkit::Alignment::VerticalTop:
    {
      position.y += mPadding.top;
      break;
    }
    case Dali::Toolkit::Alignment::VerticalCenter:
    {
      if( currentSize.height + mPadding.top + mPadding.bottom >= parentSize.height )
      {
        position.y += 0.5f * ( mPadding.top - mPadding.bottom );
      }
      break;
    }
    case Dali::Toolkit::Alignment::VerticalBottom:
    {
      position.y -= mPadding.bottom;
      break;
    }
    default:
    {
      DALI_ASSERT_ALWAYS( !"Wrong vertical alignment value" );
      break;
    }
    }

    return position;
  }

  const Toolkit::Alignment::Padding mPadding;
  const Toolkit::Alignment::Type mHorizontalAlignment;
  const Toolkit::Alignment::Type mVerticalAlignment;
};
} // namespace

Toolkit::Alignment Alignment::New( Toolkit::Alignment::Type horizontal, Toolkit::Alignment::Type vertical )
{
  // Create the implementation, temporarily owned on stack
  IntrusivePtr< Alignment > internalAlignment = new Alignment( horizontal, vertical );

  // Pass ownership to Toolkit::View
  Toolkit::Alignment alignment( *internalAlignment );

  // Second-phase init of the implementation
  // This can only be done after the CustomActor connection has been made...
  internalAlignment->Initialize();

  return alignment;
}

void Alignment::SetAlignmentType( Toolkit::Alignment::Type type )
{
  // Horizontal Alignment
  if( type & Toolkit::Alignment::HorizontalRight )
  {
    mHorizontal = Toolkit::Alignment::HorizontalRight;
  }
  if( type & Toolkit::Alignment::HorizontalLeft )
  {
    mHorizontal = Toolkit::Alignment::HorizontalLeft;
  }
  if( type & Toolkit::Alignment::HorizontalCenter )
  {
    mHorizontal = Toolkit::Alignment::HorizontalCenter;
  }

  // Vertical Alignment
  if( type & Toolkit::Alignment::VerticalBottom )
  {
    mVertical = Toolkit::Alignment::VerticalBottom;
  }
  if( type & Toolkit::Alignment::VerticalTop )
  {
    mVertical = Toolkit::Alignment::VerticalTop;
  }
  if( type & Toolkit::Alignment::VerticalCenter )
  {
    mVertical = Toolkit::Alignment::VerticalCenter;
  }

  RelayoutRequest();
}

Toolkit::Alignment::Type Alignment::GetAlignmentType() const
{
  return Toolkit::Alignment::Type( mHorizontal | mVertical );
}

void Alignment::SetScaling( Toolkit::Alignment::Scaling scaling )
{
  mScaling = scaling;

  RelayoutRequest();
}

Toolkit::Alignment::Scaling Alignment::GetScaling() const
{
  return mScaling;
}

void Alignment::SetPadding( const Toolkit::Alignment::Padding& padding )
{
  DALI_ASSERT_ALWAYS( ( padding.left >= 0.f ) && ( padding.top >= 0.f ) && ( padding.right >= 0.f ) && ( padding.bottom >= 0.f ) );

  mPadding = padding;

  RelayoutRequest();
}

const Toolkit::Alignment::Padding& Alignment::GetPadding() const
{
  return mPadding;
}

void Alignment::OnRelayout( const Vector2& size, RelayoutContainer& container )
{
  // lay out the actors
  Vector3 anchorPointAndParentOrigin  = Vector3::ZERO;
  anchorPointAndParentOrigin.z = 0.5f;
  // anchorPoint.x is initialized to 0.0, which is HorizontalLeft
  if( Toolkit::Alignment::HorizontalCenter == mHorizontal )
  {
    anchorPointAndParentOrigin.x = 0.5f;
  }
  else if( Toolkit::Alignment::HorizontalRight == mHorizontal )
  {
    anchorPointAndParentOrigin.x = 1.0f;
  }
  // anchorPoint.y is initialized to 0.0, which is VerticalTop
  if( Toolkit::Alignment::VerticalCenter == mVertical )
  {
    anchorPointAndParentOrigin.y = 0.5f;
  }
  else if( Toolkit::Alignment::VerticalBottom == mVertical )
  {
    anchorPointAndParentOrigin.y = 1.0f;
  }

  for( unsigned int i = 0, childCount = Self().GetChildCount(); i < childCount; ++i )
  {
    Actor child = Self().GetChildAt(i);

    child.SetAnchorPoint( anchorPointAndParentOrigin );
    child.SetParentOrigin( anchorPointAndParentOrigin );

    Vector3 currentChildSize( child.GetTargetSize() );
    if( currentChildSize == Vector3::ZERO )
    {
      currentChildSize = child.GetNaturalSize();
    }

    bool renegotiate = true;
    Vector3 newChildSize;

    switch( mScaling )
    {
      case Toolkit::Alignment::ScaleNone:
      {
        // Nothing to do but needed just to not to jump to the default.
        newChildSize = currentChildSize;
        renegotiate = false;
        break;
      }
      case Toolkit::Alignment::ScaleToFill:
      {
        ScaleToFillConstraint constraint( mPadding );
        newChildSize = constraint.GetSize( currentChildSize, Vector3(size) ) ;
        break;
      }
      case Toolkit::Alignment::ScaleToFitKeepAspect:
      {
        ScaleToFitKeepAspectConstraint constraint( mPadding );
        newChildSize = constraint.GetSize( currentChildSize, Vector3(size) ) ;
        break;
      }
      case Toolkit::Alignment::ScaleToFillKeepAspect:
      {
        ScaleToFillKeepAspectConstraint constraint( mPadding );
        newChildSize = constraint.GetSize( currentChildSize, Vector3(size) );
        break;
      }
      case Toolkit::Alignment::ShrinkToFit:
      {
        ShrinkToFitConstraint constraint( mPadding );
        newChildSize = constraint.GetSize( currentChildSize, Vector3(size) );
        break;
      }
      case Toolkit::Alignment::ShrinkToFitKeepAspect:
      {
        ShrinkToFitKeepAspectConstraint constraint( mPadding );
        newChildSize = constraint.GetSize( currentChildSize, Vector3(size) );
        break;
      }
      default:
      {
        DALI_ASSERT_ALWAYS( !"Invalid Alignment::mGeometryScaling value" );
        break;
      }
    }

    PositionConstraint positionConstraint(mPadding, mHorizontal, mVertical);
    child.SetPosition( positionConstraint.GetPosition(newChildSize, currentChildSize) );

    if( renegotiate )
    {
      container.Add( child, Vector2(newChildSize) );
    }
  }
}

Alignment::Alignment( Toolkit::Alignment::Type horizontal, Toolkit::Alignment::Type vertical )
: Control( CONTROL_BEHAVIOUR_NONE ),
  mHorizontal( horizontal ),
  mVertical( vertical ),
  mScaling( Toolkit::Alignment::ScaleNone ),
  mPadding( 0.f, 0.f, 0.f, 0.f )
{
}

Alignment::~Alignment()
{
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
