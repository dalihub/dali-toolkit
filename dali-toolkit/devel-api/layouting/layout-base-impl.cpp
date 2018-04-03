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
#include <dali/public-api/object/type-registry-helper.h>
#include <dali-toolkit/public-api/controls/control.h>
#include <dali-toolkit/devel-api/layouting/layout-base-impl.h>
#include <dali-toolkit/internal/layouting/layout-base-data-impl.h>

#if defined(DEBUG_ENABLED)
    Debug::Filter* gLayoutFilter = Debug::Filter::New( Debug::Verbose, false, "LOG_LAYOUT" );
#endif

namespace
{
const char* WIDTH_SPECIFICATION_NAME( "widthSpecification" );
const char* HEIGHT_SPECIFICATION_NAME( "heightSpecification" );
}

namespace Dali
{
namespace Toolkit
{
namespace Internal
{

LayoutBase::LayoutBase()
: mImpl( new Impl(*this) )
{
}

LayoutBasePtr LayoutBase::New( Handle& owner )
{
  LayoutBasePtr layoutPtr = new LayoutBase();
  layoutPtr->Initialize( owner );
  return layoutPtr;
}

void LayoutBase::Initialize( Handle& owner )
{
  mImpl->mOwner = &(owner.GetBaseObject());
}

Handle LayoutBase::GetOwner() const
{
  return Handle::DownCast(BaseHandle(mImpl->mOwner));
}

void LayoutBase::RegisterChildProperties( const std::type_info& containerType )
{
  // Call on derived types
  auto typeInfo = TypeRegistry::Get().GetTypeInfo( containerType );
  if( typeInfo )
  {
    DoRegisterChildProperties( containerType );
  }
}

void LayoutBase::DoRegisterChildProperties( const std::type_info& containerType )
{
  auto typeInfo = TypeRegistry::Get().GetTypeInfo( containerType );
  if( typeInfo )
  {
    Property::IndexContainer indices;
    typeInfo.GetChildPropertyIndices( indices );

    if( std::find( indices.Begin(), indices.End(), Toolkit::LayoutBase::ChildProperty::WIDTH_SPECIFICATION ) ==
        indices.End() )
    {
      ChildPropertyRegistration( typeInfo.GetName(), WIDTH_SPECIFICATION_NAME,
                                 Toolkit::LayoutBase::ChildProperty::WIDTH_SPECIFICATION, Property::INTEGER );

      ChildPropertyRegistration( typeInfo.GetName(), HEIGHT_SPECIFICATION_NAME,
                                 Toolkit::LayoutBase::ChildProperty::HEIGHT_SPECIFICATION, Property::INTEGER );
    }
  }
}

void LayoutBase::Measure( MeasureSpec widthMeasureSpec, MeasureSpec heightMeasureSpec )
{
  const bool forceLayout = mImpl->GetPrivateFlag( Impl::PFLAG_FORCE_LAYOUT );

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

  if( forceLayout || needsLayout)
  {
    mImpl->ClearPrivateFlag( Impl::PFLAG_MEASURED_DIMENSION_SET );

    //resolveRtlPropertiesIfNeeded();

    int cacheIndex = -1;  // = forceLayout ? -1 : mMeasureCache.indexOfKey(key);
    if( cacheIndex < 0 ) //|| sIgnoreMeasureCache )
    {
      // measure ourselves, this should set the measured dimension flag back
      OnMeasure( widthMeasureSpec, heightMeasureSpec );
      mImpl->ClearPrivateFlag( Impl::PFLAG_MEASURE_NEEDED_BEFORE_LAYOUT );
    }
    else
    {
      mImpl->SetPrivateFlag( Impl::PFLAG_MEASURE_NEEDED_BEFORE_LAYOUT );
    }

    // flag not set, setMeasuredDimension() was not invoked, we raise an exception to warn the developer
    DALI_ASSERT_ALWAYS( mImpl->GetPrivateFlag( Impl::PFLAG_MEASURED_DIMENSION_SET ) &&
                        "Layout's OnMeasure() did not set the measured dimension by calling setMeasuredDimension()" );
    mImpl->SetPrivateFlag( Impl::PFLAG_LAYOUT_REQUIRED );
  }

  mImpl->mOldWidthMeasureSpec = widthMeasureSpec;
  mImpl->mOldHeightMeasureSpec = heightMeasureSpec;

  //mMeasureCache.put(key, ((long) mMeasuredWidth) << 32 | (long) mMeasuredHeight & 0xffffffffL); // suppress sign extension
}

void LayoutBase::Layout( LayoutLength l, LayoutLength t, LayoutLength r, LayoutLength b, bool animate )
{
  if( mImpl->GetPrivateFlag( Impl::PFLAG_MEASURE_NEEDED_BEFORE_LAYOUT ) )
  {
    OnMeasure( mImpl->mOldWidthMeasureSpec, mImpl->mOldHeightMeasureSpec );
    mImpl->ClearPrivateFlag( Impl::PFLAG_MEASURE_NEEDED_BEFORE_LAYOUT );
  }

  // keep old margins for observers
  //int oldL = mImpl->mLeft;
  //int oldT = mImpl->mTop;
  //int oldB = mImpl->mBottom;
  //int oldR = mImpl->mRight;

  bool changed = SetFrame( l, t, r, b );

  if( changed || mImpl->GetPrivateFlag( Impl::PFLAG_LAYOUT_REQUIRED ) )
  {
    OnLayout( changed, l, t, r, b, animate );
    mImpl->ClearPrivateFlag( Impl::PFLAG_LAYOUT_REQUIRED );

    // @todo Inform observers
  }
  mImpl->ClearPrivateFlag( Impl::PFLAG_FORCE_LAYOUT );
  mImpl->SetPrivateFlag( Impl::PFLAG_IS_LAID_OUT );
}

LayoutLength LayoutBase::GetMinimumWidth()
{
  return mImpl->mMinimumSize.GetWidth();
}

LayoutLength LayoutBase::GetMinimumHeight()
{
  return mImpl->mMinimumSize.GetHeight();
}

void LayoutBase::SetMinimumWidth( LayoutLength minimumWidth )
{
  mImpl->mMinimumSize.SetWidth( minimumWidth );
  RequestLayout();
}

void LayoutBase::SetMinimumHeight( LayoutLength minimumHeight )
{
  mImpl->mMinimumSize.SetHeight( minimumHeight );
  RequestLayout();
}

LayoutLength LayoutBase::GetDefaultSize( LayoutLength size, MeasureSpec measureSpec )
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
    case MeasureSpec::Mode::EXACTLY:
    {
      result = specSize;
      break;
    }
  }
  return result;
}

void LayoutBase::OnMeasure( MeasureSpec widthMeasureSpec, MeasureSpec heightMeasureSpec)
{
  SetMeasuredDimensions( GetDefaultSize( GetSuggestedMinimumWidth(), widthMeasureSpec ),
                         GetDefaultSize( GetSuggestedMinimumHeight(), heightMeasureSpec ) );
}

void LayoutBase::OnLayout( bool changed, LayoutLength left, LayoutLength top, LayoutLength right, LayoutLength bottom, bool animate )
{
}


LayoutParent* LayoutBase::GetParent()
{
  return mImpl->mLayoutParent;
}

void LayoutBase::RequestLayout()
{
  // @todo Enforce failure if called in Measure/Layout passes.
  mImpl->SetPrivateFlag( Impl::PFLAG_FORCE_LAYOUT );
  mImpl->mLayoutController.RequestLayout( Toolkit::LayoutBase(this) );
}

void LayoutBase::SetMeasuredDimensions( MeasuredSize measuredWidth, MeasuredSize measuredHeight )
{
  mImpl->SetPrivateFlag( Impl::PFLAG_MEASURED_DIMENSION_SET );
  mImpl->mMeasuredWidth = measuredWidth;
  mImpl->mMeasuredHeight = measuredHeight;
}

LayoutLength LayoutBase::GetMeasuredWidth()
{
  // Get the size portion of the measured width
  return  mImpl->mMeasuredWidth.GetSize();
}

LayoutLength LayoutBase::GetMeasuredHeight()
{
  return  mImpl->mMeasuredHeight.GetSize();
}

MeasuredSize LayoutBase::GetMeasuredWidthAndState()
{
  return mImpl->mMeasuredWidth;
}

MeasuredSize LayoutBase::GetMeasuredHeightAndState()
{
  return mImpl->mMeasuredHeight;
}

LayoutLength LayoutBase::GetSuggestedMinimumWidth()
{
  auto owner = GetOwner();
  auto actor = Actor::DownCast(owner);
  auto naturalSize = actor ? actor.GetNaturalSize() : Vector3::ZERO;

  return std::max( mImpl->mMinimumSize.GetWidth(), LayoutLength::IntType( naturalSize.width ) );
}

LayoutLength LayoutBase::GetSuggestedMinimumHeight()
{
  auto owner = GetOwner();
  auto actor = Actor::DownCast(owner);
  auto naturalSize = actor ? actor.GetNaturalSize() : Vector3::ZERO;

  return std::max( mImpl->mMinimumSize.GetHeight(), LayoutLength::IntType(naturalSize.height) );
}

MeasuredSize LayoutBase::ResolveSizeAndState( LayoutLength size, MeasureSpec measureSpec, MeasuredSize::State childMeasuredState )
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


bool LayoutBase::SetFrame( LayoutLength left, LayoutLength top, LayoutLength right, LayoutLength bottom )
{
  bool changed = false;

  DALI_LOG_INFO( gLayoutFilter, Debug::Verbose, "LayoutBase::SetFrame(%d, %d, %d, %d)\n", left.mValue, top.mValue, right.mValue, bottom.mValue );

  if( mImpl->mLeft != left || mImpl->mRight != right || mImpl->mTop != top || mImpl->mBottom != bottom )
  {
    changed = true;

    auto oldWidth = mImpl->mRight - mImpl->mLeft;
    auto oldHeight = mImpl->mBottom - mImpl->mTop;
    auto newWidth = right - left;
    auto newHeight = bottom - top;
    bool sizeChanged = (newWidth != oldWidth) || (newHeight != oldHeight);

    mImpl->mLeft = left;
    mImpl->mTop = top;
    mImpl->mRight = right;
    mImpl->mBottom = bottom;

    mImpl->SetPrivateFlag( Impl::PFLAG_HAS_BOUNDS );


    // Reflect up to parent control
    auto owner = GetOwner();
    auto actor = Actor::DownCast(owner);
    if( actor )
    {
      actor.SetPosition( Vector3( float(left.mValue), float(top.mValue), 0.0f ) );
      actor.SetSize( Vector3( right-left, bottom-top, 0.0f ) );
    }

    if( sizeChanged )
    {
      SizeChange( LayoutSize( newWidth, newHeight ), LayoutSize( oldWidth, oldHeight ) );
    }

    //mImpl->mBackgroundSizeChanged = true;
    //mImpl->mDefaultFocusHighlightSizeChanged = true;
  }
  return changed;
}

void LayoutBase::SizeChange( LayoutSize newSize, LayoutSize oldSize)
{
  OnSizeChanged( newSize, oldSize );
}


void LayoutBase::OnSizeChanged( LayoutSize newSize, LayoutSize oldSize )
{
}


} // namespace Internal
} // namespace Toolkit
} // namespace Dali
