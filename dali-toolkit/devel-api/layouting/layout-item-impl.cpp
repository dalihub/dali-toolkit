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
 */

#include <dali/integration-api/debug.h>

#include <dali/public-api/animation/animation.h>
#include <dali/public-api/object/type-registry-helper.h>
#include <dali-toolkit/public-api/controls/control.h>
#include <dali-toolkit/devel-api/layouting/layout-item-impl.h>
#include <dali-toolkit/devel-api/layouting/layout-group-impl.h>
#include <dali-toolkit/internal/layouting/layout-item-data-impl.h>

namespace
{

#if defined(DEBUG_ENABLED)
Debug::Filter* gLayoutFilter = Debug::Filter::New( Debug::NoLogging, false, "LOG_LAYOUT" );
#endif

const char* WIDTH_SPECIFICATION_NAME( "widthSpecification" );
const char* HEIGHT_SPECIFICATION_NAME( "heightSpecification" );

const float DEFAULT_TRANSITION_DURATION( 0.5f );
}

namespace Dali
{
namespace Toolkit
{
namespace Internal
{

LayoutItem::LayoutItem()
: mImpl( new LayoutItem::Impl() ),
  mSlotDelegate( this )
{
}

LayoutItem::~LayoutItem()
{
  // An object with a unique_ptr to an opaque structure must define it's destructor in the translation unit
  // where the opaque structure is defined. It cannot use the default method in the header file.
}

LayoutItemPtr LayoutItem::New( Handle& owner )
{
  LayoutItemPtr layoutPtr = new LayoutItem();
  return layoutPtr;
}

void LayoutItem::Initialize( Handle& owner, const std::string& containerType )
{
  mImpl->mOwner = &(owner.GetBaseObject());
  RegisterChildProperties( containerType );
  OnInitialize(); // Ensure direct deriving class gets initialized
  RequestLayout();
}

Handle LayoutItem::GetOwner() const
{
  return Handle::DownCast(BaseHandle(mImpl->mOwner));
}

void LayoutItem::Unparent()
{
  // Enable directly derived types to first remove children
  OnUnparent();

  // Remove myself from parent
  LayoutParent* parent = GetParent();
  if( parent )
  {
    parent->Remove( *this );
  }

  // Remove parent reference
  SetParent(nullptr);

  // Last, clear owner
  mImpl->mOwner = NULL;
}

void LayoutItem::SetAnimateLayout( bool animateLayout )
{
  auto owner = GetOwner();
  auto actor = Actor::DownCast(owner);

  DALI_LOG_INFO( gLayoutFilter, Debug::Verbose, "LayoutItem::SetAnimateLayout animateLayout(%s) owner(%s)\n", (animateLayout)?"true":"false",
                 ( ( Actor::DownCast( owner) ) ? Actor::DownCast(owner).GetName().c_str() : "Invalid Actor" ) );

  mImpl->mAnimated = animateLayout;

  OnAnimationStateChanged( animateLayout );
}

bool LayoutItem::IsLayoutAnimated() const
{
  return mImpl->mAnimated;
}

void LayoutItem::RegisterChildProperties( const std::string& containerType )
{
  // Call on derived types
  auto typeInfo = TypeRegistry::Get().GetTypeInfo( containerType );
  if( typeInfo )
  {
    Property::IndexContainer indices;
    typeInfo.GetChildPropertyIndices( indices );

    if( std::find( indices.Begin(), indices.End(), Toolkit::LayoutItem::ChildProperty::WIDTH_SPECIFICATION ) ==
        indices.End() )
    {
      ChildPropertyRegistration( typeInfo.GetName(), WIDTH_SPECIFICATION_NAME,
                                 Toolkit::LayoutItem::ChildProperty::WIDTH_SPECIFICATION, Property::INTEGER );

      ChildPropertyRegistration( typeInfo.GetName(), HEIGHT_SPECIFICATION_NAME,
                                 Toolkit::LayoutItem::ChildProperty::HEIGHT_SPECIFICATION, Property::INTEGER );
    }

    OnRegisterChildProperties( containerType );
  }
}

void LayoutItem::OnRegisterChildProperties( const std::string& containerType )
{
}


void LayoutItem::Measure( MeasureSpec widthMeasureSpec, MeasureSpec heightMeasureSpec )
{
  DALI_LOG_TRACE_METHOD( gLayoutFilter );

  const bool forceLayout = mImpl->GetPrivateFlag( Impl::PRIVATE_FLAG_FORCE_LAYOUT );

  const bool specChanged =
    ( widthMeasureSpec  != mImpl->mOldWidthMeasureSpec ) ||
    ( heightMeasureSpec != mImpl->mOldHeightMeasureSpec );

  const bool isSpecExactly =
    ( widthMeasureSpec.GetMode() == MeasureSpec::Mode::EXACTLY ) &&
    ( heightMeasureSpec.GetMode() == MeasureSpec::Mode::EXACTLY );

  const bool matchesSpecSize =
    ( GetMeasuredWidth() == widthMeasureSpec.GetSize() ) &&
    ( GetMeasuredHeight() == heightMeasureSpec.GetSize() );

  const bool needsLayout = specChanged && ( !isSpecExactly || !matchesSpecSize );

  DALI_LOG_STREAM( gLayoutFilter, Debug::General, "LayoutItem::Measure("<<widthMeasureSpec<<", "<<heightMeasureSpec<<") Owner:"<<Actor::DownCast(GetOwner()).GetName() <<"  forceLayout="<<forceLayout<<", specChanged="<<specChanged<<", isSpecExactly="<<isSpecExactly<<", matchesSpecSize="<<matchesSpecSize<<", needsLayout="<<needsLayout <<std::endl <<(forceLayout||needsLayout?"  Remeasuring":"  NoChange"));

  if( forceLayout || needsLayout )
  {
    mImpl->ClearPrivateFlag( Impl::PRIVATE_FLAG_MEASURED_DIMENSION_SET );

    // measure ourselves, this should set the measured dimension flag back
#if defined(DEBUG_ENABLED)
    std::ostringstream o;
    o<<widthMeasureSpec<<","<<heightMeasureSpec;
    DALI_LOG_INFO( gLayoutFilter, Debug::General, "LayoutItem::Measure Calling %s OnMeasure( %s )\n", Actor::DownCast(GetOwner()).GetName().c_str(), o.str().c_str());
#endif
    OnMeasure( widthMeasureSpec, heightMeasureSpec );
    mImpl->ClearPrivateFlag( Impl::PRIVATE_FLAG_MEASURE_NEEDED_BEFORE_LAYOUT );

    // flag not set, setMeasuredDimension() was not invoked, we raise an exception to warn the developer
    DALI_ASSERT_ALWAYS( mImpl->GetPrivateFlag( Impl::PRIVATE_FLAG_MEASURED_DIMENSION_SET ) &&
                        "Layout's OnMeasure() Measured dimension flag not set" );
    mImpl->SetPrivateFlag( Impl::PRIVATE_FLAG_LAYOUT_REQUIRED );
  }

  mImpl->mOldWidthMeasureSpec = widthMeasureSpec;
  mImpl->mOldHeightMeasureSpec = heightMeasureSpec;
}

void LayoutItem::Layout( LayoutLength l, LayoutLength t, LayoutLength r, LayoutLength b )
{
  DALI_LOG_TRACE_METHOD( gLayoutFilter );

  if( mImpl->GetPrivateFlag( Impl::PRIVATE_FLAG_MEASURE_NEEDED_BEFORE_LAYOUT ) )
  {
    OnMeasure( mImpl->mOldWidthMeasureSpec, mImpl->mOldHeightMeasureSpec );
    mImpl->ClearPrivateFlag( Impl::PRIVATE_FLAG_MEASURE_NEEDED_BEFORE_LAYOUT );
  }

  bool changed = SetFrame( l, t, r, b );

  if( changed || mImpl->GetPrivateFlag( Impl::PRIVATE_FLAG_LAYOUT_REQUIRED ) )
  {
    OnLayout( changed, l, t, r, b );
    mImpl->ClearPrivateFlag( Impl::PRIVATE_FLAG_LAYOUT_REQUIRED );
  }

  mImpl->ClearPrivateFlag( Impl::PRIVATE_FLAG_FORCE_LAYOUT );
  mImpl->SetPrivateFlag( Impl::PRIVATE_FLAG_IS_LAID_OUT );
}

LayoutLength LayoutItem::GetMinimumWidth() const
{
  return mImpl->mMinimumSize.GetWidth();
}

LayoutLength LayoutItem::GetMinimumHeight() const
{
  return mImpl->mMinimumSize.GetHeight();
}

void LayoutItem::SetMinimumWidth( LayoutLength minimumWidth )
{
  mImpl->mMinimumSize.SetWidth( minimumWidth );
  RequestLayout();
}

void LayoutItem::SetMinimumHeight( LayoutLength minimumHeight )
{
  mImpl->mMinimumSize.SetHeight( minimumHeight );
  RequestLayout();
}

Extents LayoutItem::GetPadding() const
{
  Toolkit::Control control = Toolkit::Control::DownCast( mImpl->mOwner );
  if( control )
  {
    Extents padding = control.GetProperty<Extents>( Toolkit::Control::Property::PADDING );

    DALI_LOG_INFO( gLayoutFilter, Debug::Verbose, "LayoutItem::Padding for %s : (%d,%d,%d,%d) \n",
                   control.GetName().c_str(),
                   padding.start, padding.end, padding.top, padding.bottom
                 );
    return padding;
  }
  else
  {
    return Extents();
  }
}

Extents LayoutItem::GetMargin() const
{
  Toolkit::Control control = Toolkit::Control::DownCast( mImpl->mOwner );
  if ( control )
  {
    return control.GetProperty<Extents>( Toolkit::Control::Property::MARGIN );
  }
  else
  {
    return Extents();
  }
}

LayoutLength LayoutItem::GetDefaultSize( LayoutLength size, MeasureSpec measureSpec )
{
  LayoutLength result = size;
  auto specMode = measureSpec.GetMode();
  auto specSize = measureSpec.GetSize();

  switch (specMode)
  {
    case MeasureSpec::Mode::UNSPECIFIED:
    {
      result = size;
      break;
    }
    case MeasureSpec::Mode::AT_MOST:
    {
      LayoutLength tmp = specSize;
      if( size < tmp )
      {
        result = size;
      }
      else
      {
        result = specSize;
      }
      break;
    }
    case MeasureSpec::Mode::EXACTLY:
    {
      result = specSize;
      break;
    }
  }
  return result;
}

void LayoutItem::OnMeasure( MeasureSpec widthMeasureSpec, MeasureSpec heightMeasureSpec)
{
  DALI_LOG_INFO( gLayoutFilter, Debug::Verbose, "LayoutItem::OnMeasure\n");

  SetMeasuredDimensions( GetDefaultSize( GetSuggestedMinimumWidth(), widthMeasureSpec ),
                         GetDefaultSize( GetSuggestedMinimumHeight(), heightMeasureSpec ) );
}

void LayoutItem::OnLayout( bool changed, LayoutLength left, LayoutLength top, LayoutLength right, LayoutLength bottom )
{
}

void LayoutItem::SetParent( LayoutParent* parent )
{
  mImpl->mLayoutParent = parent;
  mImpl->SetPrivateFlag( Impl::PRIVATE_FLAG_FORCE_SET_FRAME );
}

LayoutParent* LayoutItem::GetParent()
{
  return mImpl->mLayoutParent;
}

void LayoutItem::RequestLayout()
{
  Toolkit::Control control = Toolkit::Control::DownCast( mImpl->mOwner );
  if ( control )
  {
    DALI_LOG_INFO( gLayoutFilter, Debug::Verbose, "LayoutItem::RequestLayout %s\n", control.GetName().c_str());
  }
  // @todo Enforce failure if called in Measure/Layout passes.
  mImpl->SetPrivateFlag( Impl::PRIVATE_FLAG_FORCE_LAYOUT );
  Toolkit::LayoutController layoutController = Toolkit::LayoutController::Get();
  layoutController.RequestLayout( Toolkit::LayoutItem(this) );
}

bool LayoutItem::IsLayoutRequested() const
{
  return mImpl->GetPrivateFlag( Impl::PRIVATE_FLAG_FORCE_LAYOUT );
}

void LayoutItem::SetLayoutRequested()
{
  mImpl->SetPrivateFlag( Impl::PRIVATE_FLAG_FORCE_LAYOUT );
}

void LayoutItem::SetMeasuredDimensions( MeasuredSize measuredWidth, MeasuredSize measuredHeight )
{
  DALI_LOG_STREAM( gLayoutFilter, Debug::Verbose, "LayoutItem::SetMeasuredDimensions width(" << measuredWidth.GetSize() << ") height(" << measuredHeight.GetSize() << ") \n" );

  mImpl->SetPrivateFlag( Impl::PRIVATE_FLAG_MEASURED_DIMENSION_SET );
  mImpl->mMeasuredWidth = measuredWidth;
  mImpl->mMeasuredHeight = measuredHeight;
}

LayoutLength LayoutItem::GetMeasuredWidth() const
{
  // Get the size portion of the measured width
  return  mImpl->mMeasuredWidth.GetSize();
}

LayoutLength LayoutItem::GetMeasuredHeight() const
{
  return  mImpl->mMeasuredHeight.GetSize();
}

MeasuredSize LayoutItem::GetMeasuredWidthAndState() const
{
  return mImpl->mMeasuredWidth;
}

MeasuredSize LayoutItem::GetMeasuredHeightAndState() const
{
  return mImpl->mMeasuredHeight;
}

LayoutLength LayoutItem::GetSuggestedMinimumWidth() const
{
  auto owner = GetOwner();
  auto actor = Actor::DownCast(owner);
  auto naturalSize = actor ? actor.GetNaturalSize() : Vector3::ZERO;

  return std::max( mImpl->mMinimumSize.GetWidth(), LayoutLength( naturalSize.width ) );
}

LayoutLength LayoutItem::GetSuggestedMinimumHeight() const
{
  auto owner = GetOwner();
  auto actor = Actor::DownCast(owner);
  auto naturalSize = actor ? actor.GetNaturalSize() : Vector3::ZERO;

  return std::max( mImpl->mMinimumSize.GetHeight(), LayoutLength( naturalSize.height ) );
}

MeasuredSize LayoutItem::ResolveSizeAndState( LayoutLength size, MeasureSpec measureSpec, MeasuredSize::State childMeasuredState )
{
  auto specMode = measureSpec.GetMode();
  LayoutLength specSize = measureSpec.GetSize();
  MeasuredSize result;

  switch( specMode )
  {
    case MeasureSpec::Mode::AT_MOST:
    {
      if (specSize < size)
      {
        result = MeasuredSize( specSize, MeasuredSize::MEASURED_SIZE_TOO_SMALL );
      }
      else
      {
        result.SetSize( size );
      }
      break;
    }

    case MeasureSpec::Mode::EXACTLY:
    {
      result.SetSize( specSize );
      break;
    }

    case MeasureSpec::Mode::UNSPECIFIED:
    default:
    {
      result.SetSize( size );
      break;
    }
  }

  result.SetState( childMeasuredState );
  return result;
}


bool LayoutItem::SetFrame( LayoutLength left, LayoutLength top, LayoutLength right, LayoutLength bottom )
{
  bool changed = false;

  DALI_LOG_STREAM( gLayoutFilter, Debug::Verbose, "LayoutItem::SetFrame enter(" << left << ", " << top << ", " << right << ", " << bottom << ")\n" );

  if( mImpl->mLeft != left || mImpl->mRight != right || mImpl->mTop != top || mImpl->mBottom != bottom || mImpl->GetPrivateFlag( Impl::PRIVATE_FLAG_FORCE_SET_FRAME ) )
  {
    changed = true;

    LayoutLength oldWidth = mImpl->mRight - mImpl->mLeft;
    LayoutLength oldHeight = mImpl->mBottom - mImpl->mTop;
    LayoutLength newWidth = right - left;
    LayoutLength newHeight = bottom - top;
    bool sizeChanged = (newWidth != oldWidth) || (newHeight != oldHeight);

    mImpl->mLeft = left;
    mImpl->mTop = top;
    mImpl->mRight = right;
    mImpl->mBottom = bottom;

    mImpl->ClearPrivateFlag( Impl::PRIVATE_FLAG_FORCE_SET_FRAME );


    // Reflect up to parent control
    auto owner = GetOwner();
    auto actor = Actor::DownCast(owner);
    if( actor )
    {
      DALI_LOG_STREAM( gLayoutFilter, Debug::Verbose, "LayoutItem::SetFrame owner(" << left << ", " << top << ", " << right << ", " << bottom << ")\n" );

      if( mImpl->mAnimated )
      {
        auto animation = Animation::New( 0.5f );
        animation.AnimateTo( Property( actor, Actor::Property::POSITION_X ), left.AsInteger() );
        animation.AnimateTo( Property( actor, Actor::Property::POSITION_Y ), top.AsInteger() );

        animation.AnimateTo( Property( actor, Actor::Property::SIZE_WIDTH ), newWidth.AsInteger() );
        animation.AnimateTo( Property( actor, Actor::Property::SIZE_HEIGHT ), newHeight.AsInteger() );

        animation.FinishedSignal().Connect( mSlotDelegate, &LayoutItem::OnLayoutAnimationFinished );
        animation.Play();
      }
      else
      {
        // @todo Collate into list of Property & Property::Value pairs.
        actor.SetX( left.AsInteger() );
        actor.SetY( top.AsInteger() );
        actor.SetProperty( Actor::Property::SIZE_WIDTH, newWidth.AsInteger() );
        actor.SetProperty( Actor::Property::SIZE_HEIGHT, newHeight.AsInteger() );
      }
    }

    if( sizeChanged )
    {
      SizeChange( LayoutSize( newWidth, newHeight ), LayoutSize( oldWidth, oldHeight ) );
    }
  }

  DALI_LOG_STREAM( gLayoutFilter, Debug::Verbose, "LayoutItem::SetFrame  exit(" << left << ", " << top << ", " << right << ", " << bottom << ")\n" );

  return changed;
}

void LayoutItem::OnLayoutAnimationFinished( Animation& animation )
{
  auto owner = GetOwner();
  auto actor = Actor::DownCast(owner);
  if( actor )
  {
    actor.SetSize( Vector3( mImpl->mRight.AsInteger() - mImpl->mLeft.AsInteger(), mImpl->mBottom.AsInteger() - mImpl->mTop.AsInteger(), 0.0f ) );
  }
}

void LayoutItem::SizeChange( LayoutSize newSize, LayoutSize oldSize)
{
  OnSizeChanged( newSize, oldSize );
}


void LayoutItem::OnSizeChanged( LayoutSize newSize, LayoutSize oldSize )
{
}

void LayoutItem::OnInitialize()
{
}


} // namespace Internal
} // namespace Toolkit
} // namespace Dali
