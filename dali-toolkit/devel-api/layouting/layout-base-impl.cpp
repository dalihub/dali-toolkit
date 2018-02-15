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
#include <dali-toolkit/public-api/controls/control.h>
#include <dali-toolkit/devel-api/layouting/layout-base-impl.h>
#include <dali-toolkit/internal/layouting/layout-base-data-impl.h>

#if defined(DEBUG_ENABLED)
    Debug::Filter* gLayoutFilter = Debug::Filter::New( Debug::Concise, false, "LOG_LAYOUT" );
#endif


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

LayoutBasePtr LayoutBase::New( IntrusivePtr<RefObject> handle )
{
  LayoutBasePtr layoutPtr = new LayoutBase();
  layoutPtr->Initialize( handle );
  return layoutPtr;
}

void LayoutBase::Initialize( IntrusivePtr<RefObject> handle )
{
  mImpl->mOwner = handle.Get();
}

IntrusivePtr<RefObject> LayoutBase::GetOwner() const
{
  return IntrusivePtr<RefObject>(mImpl->mOwner);
}

void LayoutBase::SetLayoutData( ChildLayoutDataPtr childLayoutData )
{
  mImpl->mLayoutData = childLayoutData;
  auto layoutClone = childLayoutData->Clone();
  OnSetLayoutData( layoutClone );
  RequestLayout();
}

ChildLayoutDataPtr LayoutBase::GetLayoutData()
{
  return mImpl->mLayoutData;
}

void LayoutBase::Measure( MeasureSpec widthMeasureSpec, MeasureSpec heightMeasureSpec )
{
  const bool forceLayout = mImpl->GetPrivateFlag( Impl::PFLAG_FORCE_LAYOUT );

  const bool specChanged =
    ( widthMeasureSpec  != mImpl->mOldWidthMeasureSpec ) ||
    ( heightMeasureSpec != mImpl->mOldHeightMeasureSpec );

  const bool isSpecExactly =
    ( widthMeasureSpec.GetMode() == MeasureSpec::EXACTLY ) &&
    ( heightMeasureSpec.GetMode() == MeasureSpec::EXACTLY );

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
      //long value = mMeasureCache.valueAt(cacheIndex);
      // Casting a long to int drops the high 32 bits, no mask needed
      //setMeasuredDimensionRaw((int) (value >> 32), (int) value);
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

void LayoutBase::Layout( int l, int t, int r, int b, bool animate )
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

uint16_t LayoutBase::GetMinimumWidth()
{
  return mImpl->mMinimumSize.GetWidth();
}

uint16_t LayoutBase::GetMinimumHeight()
{
  return mImpl->mMinimumSize.GetHeight();
}

void LayoutBase::SetMinimumWidth( uint16_t minimumWidth )
{
  mImpl->mMinimumSize.SetWidth( minimumWidth );
  RequestLayout();
}

void LayoutBase::SetMinimumHeight( uint16_t minimumHeight )
{
  mImpl->mMinimumSize.SetHeight( minimumHeight );
  RequestLayout();
}

uint16_t LayoutBase::GetDefaultSize( uint16_t size, MeasureSpec measureSpec )
{
  uint16_t result = size;
  uint32_t specMode = measureSpec.GetMode();
  uint32_t specSize = measureSpec.GetSize();

  switch (specMode)
  {
    case MeasureSpec::UNSPECIFIED:
    {
      result = size;
      break;
    }
    case MeasureSpec::AT_MOST:
    case MeasureSpec::EXACTLY:
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

void LayoutBase::OnLayout( bool changed, int left, int top, int right, int bottom, bool animate )
{
}

void LayoutBase::OnSetLayoutData( ChildLayoutDataPtr childLayoutData )
{
  // Base function does nothing
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
  mImpl->mMeasuredSize = Uint16Pair( measuredWidth, measuredHeight );
}

uint16_t LayoutBase::GetMeasuredWidth()
{
  // Get the size portion of the measured width
  return MeasuredSize( mImpl->mMeasuredSize.GetWidth() ).GetSize();
}

uint16_t LayoutBase::GetMeasuredHeight()
{
  return MeasuredSize( mImpl->mMeasuredSize.GetHeight() ).GetSize();
}

MeasuredSize LayoutBase::GetMeasuredWidthAndState()
{
  return MeasuredSize( mImpl->mMeasuredSize.GetWidth() );
}

MeasuredSize LayoutBase::GetMeasuredHeightAndState()
{
  return MeasuredSize( mImpl->mMeasuredSize.GetHeight() );
}

uint16_t LayoutBase::GetSuggestedMinimumWidth()
{
  auto customActor = dynamic_cast<CustomActorImpl*>( mImpl->mOwner );
  auto actor = (customActor != nullptr) ? customActor->Self() : nullptr;
  auto naturalSize = (actor != nullptr) ? actor.GetNaturalSize() : Vector3::ZERO;

  return std::max( mImpl->mMinimumSize.GetWidth(), uint16_t( naturalSize.width ) );
}

uint16_t LayoutBase::GetSuggestedMinimumHeight()
{
  auto customActor = dynamic_cast<CustomActorImpl*>( mImpl->mOwner );
  auto actor = (customActor != nullptr) ? customActor->Self() : nullptr;
  auto naturalSize = (actor != nullptr) ? actor.GetNaturalSize() : Vector3::ZERO;

  return std::max( mImpl->mMinimumSize.GetHeight(), uint16_t(naturalSize.height) );
}

MeasuredSize LayoutBase::ResolveSizeAndState( uint16_t size, MeasureSpec measureSpec, uint16_t childMeasuredState )
{
  uint32_t specMode = measureSpec.GetMode();
  uint32_t specSize = measureSpec.GetSize();
  MeasuredSize result;

  switch( specMode )
  {
    case MeasureSpec::AT_MOST:
    {
      if (specSize < size)
      {
        result = MeasuredSize( specSize, MeasuredSize::MEASURED_SIZE_TOO_SMALL );
      }
      else
      {
        result = size;
      }
      break;
    }

    case MeasureSpec::EXACTLY:
    {
      result = specSize;
      break;
    }

    case MeasureSpec::UNSPECIFIED:
    default:
    {
      result = size;
      break;
    }
  }

  result.SetState( childMeasuredState );
  return result;
}


bool LayoutBase::SetFrame( int left, int top, int right, int bottom )
{
  bool changed = false;

  DALI_LOG_INFO( gLayoutFilter, Debug::Verbose, "LayoutBase::SetFrame(%d, %d, %d, %d)", left, top, right, bottom );

  if( mImpl->mLeft != left || mImpl->mRight != right || mImpl->mTop != top || mImpl->mBottom != bottom )
  {
    changed = true;

    int oldWidth = mImpl->mRight - mImpl->mLeft;
    int oldHeight = mImpl->mBottom - mImpl->mTop;
    int newWidth = right - left;
    int newHeight = bottom - top;
    bool sizeChanged = (newWidth != oldWidth) || (newHeight != oldHeight);

    mImpl->mLeft = left;
    mImpl->mTop = top;
    mImpl->mRight = right;
    mImpl->mBottom = bottom;

    mImpl->SetPrivateFlag( Impl::PFLAG_HAS_BOUNDS );


    // Reflect up to parent control
    auto customActor = dynamic_cast<CustomActorImpl*>( mImpl->mOwner );
    auto actor = (customActor != nullptr) ? customActor->Self() : nullptr;

    if( actor )
    {
      actor.SetPosition( Vector3( left, top, 0.0f ) );
      actor.SetSize( Vector3( right-left, bottom-top, 0.0f ) );
    }

    if( sizeChanged )
    {
      SizeChange( Uint16Pair( newWidth, newHeight ), Uint16Pair( oldWidth, oldHeight ) );
    }

    //mImpl->mBackgroundSizeChanged = true;
    //mImpl->mDefaultFocusHighlightSizeChanged = true;
  }
  return changed;
}

void LayoutBase::SizeChange( Uint16Pair newSize, Uint16Pair oldSize)
{
  OnSizeChanged( newSize, oldSize );
}


void LayoutBase::OnSizeChanged( Uint16Pair newSize, Uint16Pair oldSize )
{
}


} // namespace Internal
} // namespace Toolkit
} // namespace Dali
