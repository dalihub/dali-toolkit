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

// CLASS HEADER
#include <dali-toolkit/devel-api/layouting/layout-group-impl.h>

// EXTERNAL INCLUDES
#include <dali/public-api/object/type-registry-helper.h>
#include <dali/devel-api/actors/actor-devel.h>
#include <dali/devel-api/object/handle-devel.h>
#include <dali/integration-api/debug.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/layouting/layout-group-data-impl.h>
#include <dali-toolkit/public-api/controls/control-impl.h>
#include <dali-toolkit/internal/controls/control/control-data-impl.h>

#if defined(DEBUG_ENABLED)
    Debug::Filter* gLogFilter = Debug::Filter::New( Debug::Concise, false, "LOG_LAYOUT" );
#endif

namespace
{
  Dali::Vector3 GetNaturalSizeStrippedOfPadding( Dali::Toolkit::Control& control )
  {
    Dali::Extents padding = control.GetProperty<Dali::Extents>( Dali::Toolkit::Control::Property::PADDING );
    Dali::Vector3 desiredSize = control.GetNaturalSize();
    desiredSize.width -= padding.start + padding.end;
    desiredSize.height -= padding.top + padding.bottom;
    return desiredSize;
  }
}

namespace Dali
{
namespace Toolkit
{
namespace Internal
{

LayoutGroup::LayoutGroup()
: mImpl( new LayoutGroup::Impl() ),
  mSlotDelegate(this)
{
}

LayoutGroup::~LayoutGroup()
{
}

Toolkit::LayoutGroup::LayoutId LayoutGroup::Add( LayoutBase& child )
{
  LayoutParent* oldParent = child.GetParent();
  if( oldParent )
  {
    LayoutGroupPtr parentGroup( dynamic_cast< LayoutGroup* >( oldParent ) );
    if( parentGroup )
    {
      parentGroup->Remove( child );
    }
  }

  Impl::ChildLayout childLayout;
  childLayout.layoutId = mImpl->mNextLayoutId++;
  childLayout.child = &child;
  mImpl->mChildren.emplace_back( childLayout );

  auto owner = child.GetOwner();

  // If the owner does not have any LayoutBase child properties, add them
  if( ! DevelHandle::DoesCustomPropertyExist( owner, Toolkit::LayoutBase::ChildProperty::WIDTH_SPECIFICATION ) )
  {
    // Set default properties for LayoutGroup and LayoutBase.
    // Deriving classes can override OnChildAdd() to add their own default properties
    GenerateDefaultChildPropertyValues( owner );
  }

  // Inform deriving classes that this child has been added
  OnChildAdd( *childLayout.child.Get() );

  // Now listen to future changes to the child properties.
  DevelHandle::PropertySetSignal(owner).Connect( this, &LayoutGroup::OnSetChildProperties );

  RequestLayout();

  return childLayout.layoutId;
}

void LayoutGroup::Remove( Toolkit::LayoutGroup::LayoutId childId )
{
  for( auto iter = mImpl->mChildren.begin() ; iter != mImpl->mChildren.end() ; ++iter )
  {
    if( iter->layoutId == childId )
    {
      OnChildRemove( *iter->child.Get() );
      mImpl->mChildren.erase(iter);
      break;
    }
  }
  RequestLayout();
}

void LayoutGroup::Remove( LayoutBase& child )
{
  for( auto iter = mImpl->mChildren.begin() ; iter != mImpl->mChildren.end() ; ++iter )
  {
    if( iter->child.Get() == &child )
    {
      OnChildRemove( *iter->child.Get() );
      mImpl->mChildren.erase(iter);
      break;
    }
  }
  RequestLayout();
}

void LayoutGroup::RemoveAll()
{
  for( auto iter = mImpl->mChildren.begin() ; iter != mImpl->mChildren.end() ; ++iter )
  {
    OnChildRemove( *iter->child.Get() );
    iter = mImpl->mChildren.erase(iter);
  }
}

unsigned int LayoutGroup::GetChildCount() const
{
  return mImpl->mChildren.size();
}

LayoutBasePtr LayoutGroup::GetChildAt( unsigned int index ) const
{
  DALI_ASSERT_ALWAYS( index < mImpl->mChildren.size() );
  return mImpl->mChildren[ index ].child;
}

LayoutBasePtr LayoutGroup::GetChild( Toolkit::LayoutGroup::LayoutId childId ) const
{
  for( auto&& childLayout : mImpl->mChildren )
  {
    if( childLayout.layoutId == childId )
    {
      return childLayout.child;
    }
  }
  return NULL;
}

Toolkit::LayoutGroup::LayoutId LayoutGroup::GetChildId( LayoutBase& child ) const
{
  for( auto&& childLayout : mImpl->mChildren )
  {
    if( childLayout.child.Get() == &child )
    {
      return childLayout.layoutId;
    }
  }
  return Toolkit::LayoutGroup::UNKNOWN_ID;
}

void LayoutGroup::OnChildAdd( LayoutBase& child )
{
}

void LayoutGroup::OnChildRemove( LayoutBase& child )
{
}

void LayoutGroup::DoInitialize()
{
}

void LayoutGroup::DoRegisterChildProperties( const std::string& containerType )
{
}

void LayoutGroup::OnSetChildProperties( Handle& handle, Property::Index index, Property::Value value )
{
  DALI_LOG_INFO( gLogFilter, Debug::Verbose, "LayoutGroup::OnSetChildProperties");

  if ( ( ( index >= CHILD_PROPERTY_REGISTRATION_START_INDEX ) &&
         ( index <= CHILD_PROPERTY_REGISTRATION_MAX_INDEX ) )
       ||
       ( index == Toolkit::Control::Property::MARGIN || index == Toolkit::Control::Property::PADDING ) )
  {
    // If any child properties are set, must perform relayout
    RequestLayout();
  }
}

void LayoutGroup::GenerateDefaultChildPropertyValues( Handle child )
{
  child.SetProperty( Toolkit::LayoutBase::ChildProperty::WIDTH_SPECIFICATION,
                     Toolkit::ChildLayoutData::WRAP_CONTENT );
  child.SetProperty( Toolkit::LayoutBase::ChildProperty::HEIGHT_SPECIFICATION,
                     Toolkit::ChildLayoutData::WRAP_CONTENT );
}

void LayoutGroup::MeasureChildren( MeasureSpec widthMeasureSpec, MeasureSpec heightMeasureSpec)
{
  for( auto&& child : mImpl->mChildren )
  {
    //if( (child.mViewFlags & Impl::VISIBILITY_MASK) != Impl::GONE ) // Use owner visibility/enabled/ready
    {
      MeasureChild( child.child, widthMeasureSpec, heightMeasureSpec );
    }
  }
}

void LayoutGroup::MeasureChild( LayoutBasePtr child,
                                MeasureSpec parentWidthMeasureSpec,
                                MeasureSpec parentHeightMeasureSpec )
{
  DALI_LOG_INFO( gLogFilter, Debug::Verbose, "LayoutGroup::MeasureChild\n" );

  auto childOwner = child->GetOwner();
  auto desiredWidth = childOwner.GetProperty<int>( Toolkit::LayoutBase::ChildProperty::WIDTH_SPECIFICATION );
  auto desiredHeight = childOwner.GetProperty<int>( Toolkit::LayoutBase::ChildProperty::HEIGHT_SPECIFICATION );

  auto padding = child->GetPadding();

  const MeasureSpec childWidthMeasureSpec = GetChildMeasureSpec( parentWidthMeasureSpec,
                                                                 padding.start + padding.end,
                                                                 desiredWidth);
  const MeasureSpec childHeightMeasureSpec = GetChildMeasureSpec( parentHeightMeasureSpec,
                                                                  padding.top + padding.bottom,
                                                                  desiredHeight);

  child->Measure( childWidthMeasureSpec, childHeightMeasureSpec );
}

void LayoutGroup::MeasureChildWithMargins( LayoutBasePtr child,
                                           MeasureSpec parentWidthMeasureSpec, LayoutLength widthUsed,
                                           MeasureSpec parentHeightMeasureSpec, LayoutLength heightUsed)
{
  auto childOwner = child->GetOwner();
  auto desiredWidth = childOwner.GetProperty<int>( Toolkit::LayoutBase::ChildProperty::WIDTH_SPECIFICATION );
  auto desiredHeight = childOwner.GetProperty<int>( Toolkit::LayoutBase::ChildProperty::HEIGHT_SPECIFICATION );
  auto padding = child->GetPadding();


  DALI_LOG_INFO( gLogFilter, Debug::Verbose, "LayoutGroup::MeasureChildWithMargins desiredWidth(%d)\n",  desiredWidth );

  MeasureSpec childWidthMeasureSpec = GetChildMeasureSpec( parentWidthMeasureSpec,
                                                           padding.start + padding.end +
                                                           widthUsed, desiredWidth );

  DALI_LOG_INFO( gLogFilter, Debug::Verbose, "LayoutGroup::MeasureChildWithMargins desiredHeight(%d)\n",  desiredHeight );

  MeasureSpec childHeightMeasureSpec = GetChildMeasureSpec( parentHeightMeasureSpec,
                                                            padding.top + padding.bottom +
                                                            heightUsed, desiredHeight );

  child->Measure( childWidthMeasureSpec, childHeightMeasureSpec );
}


MeasureSpec LayoutGroup::GetChildMeasureSpec(
  MeasureSpec  measureSpec,
  LayoutLength padding,
  LayoutLength childDimension )
{
  auto specMode = measureSpec.GetMode();
  LayoutLength specSize = measureSpec.GetSize();

  auto size = std::max( LayoutLength(0), specSize );

  MeasureSpec::IntType resultSize = 0;
  MeasureSpec::Mode resultMode = MeasureSpec::Mode::UNSPECIFIED;

  switch( specMode )
  {
    // Parent has imposed an exact size on us
    case MeasureSpec::Mode::EXACTLY:
    {
     DALI_LOG_INFO( gLogFilter, Debug::Verbose, "LayoutGroup::GetChildMeasureSpec MeasureSpec::Mode::EXACTLY\n");
      if (childDimension == Toolkit::ChildLayoutData::MATCH_PARENT)
      {
        DALI_LOG_INFO( gLogFilter, Debug::Verbose, "LayoutGroup::GetChildMeasureSpec childDimension MATCH_PARENT\n");

        // Child wants to be our size. So be it.
        resultSize = size;
        resultMode = MeasureSpec::Mode::EXACTLY;
      }
      else if (childDimension == Toolkit::ChildLayoutData::WRAP_CONTENT)
      {
        DALI_LOG_INFO( gLogFilter, Debug::Verbose, "LayoutGroup::GetChildMeasureSpec childDimension WRAP_CONTENT\n");

        // Child wants to determine its own size. It can't be
        // bigger than us.
        resultSize = size;
        resultMode = MeasureSpec::Mode::AT_MOST;
      }
      else
      {
        DALI_LOG_INFO( gLogFilter, Debug::Verbose, "LayoutGroup::GetChildMeasureSpec childDimension UNSPECIFIED\n");
        resultSize = childDimension + padding;
        resultMode = MeasureSpec::Mode::EXACTLY;
      }

      break;
    }

      // Parent has imposed a maximum size on us
    case MeasureSpec::Mode::AT_MOST:
    {
      DALI_LOG_INFO( gLogFilter, Debug::Verbose, "LayoutGroup::GetChildMeasureSpec MeasureSpec::Mode::AT_MOST\n");
      if (childDimension == Toolkit::ChildLayoutData::MATCH_PARENT)
      {
        // Child wants to be our size, but our size is not fixed.
        // Constrain child to not be bigger than us.
        resultSize = size;
        resultMode = MeasureSpec::Mode::AT_MOST;
      }
      else if (childDimension == Toolkit::ChildLayoutData::WRAP_CONTENT)
      {
        // Child wants to determine its own size. It can't be
        // bigger than us.
        resultSize = size;
        resultMode = MeasureSpec::Mode::AT_MOST;
      }
      else
      {
        // Child wants a specific size... so be it
        resultSize = childDimension + padding;
        resultMode = MeasureSpec::Mode::EXACTLY;
      }

      break;
    }

      // Parent asked to see how big we want to be
    case MeasureSpec::Mode::UNSPECIFIED:
    {
      DALI_LOG_INFO( gLogFilter, Debug::Verbose, "LayoutGroup::GetChildMeasureSpec MeasureSpec::Mode::UNSPECIFIED\n");

      if (childDimension == Toolkit::ChildLayoutData::MATCH_PARENT)
      {
        // Child wants to be our size... find out how big it should be
        resultSize = LayoutBase::Impl::sUseZeroUnspecifiedMeasureSpec ? LayoutLength(0) : size;
        resultMode = MeasureSpec::Mode::UNSPECIFIED;
      }
      else if (childDimension == Toolkit::ChildLayoutData::WRAP_CONTENT)
      {
        // Child wants to determine its own size.... find out how big
        // it should be
        resultSize = LayoutBase::Impl::sUseZeroUnspecifiedMeasureSpec ? LayoutLength(0) : size;
        resultMode = MeasureSpec::Mode::UNSPECIFIED;
      }
      else
      {
        // Child wants a specific size... let him have it
        resultSize = childDimension + padding;
        resultMode = MeasureSpec::Mode::EXACTLY;
      }
      break;
    }
  }

  DALI_LOG_INFO( gLogFilter, Debug::Verbose, "LayoutGroup::GetChildMeasureSpec resultSize(%u)\n", resultSize );


  //noinspection ResourceType
  return MeasureSpec( resultSize, resultMode );
}


void LayoutGroup::OnInitialize()
{
  auto control = Toolkit::Control::DownCast( GetOwner() );

  if( control )
  {
    // Take ownership of existing children
    for( unsigned int childIndex = 0 ; childIndex < control.GetChildCount(); ++childIndex )
    {
      ChildAddedToOwner( control.GetChildAt( childIndex ) );
    }

    DevelActor::ChildAddedSignal( control ).Connect( mSlotDelegate, &LayoutGroup::ChildAddedToOwner );
    DevelActor::ChildRemovedSignal( control ).Connect( mSlotDelegate, &LayoutGroup::ChildRemovedFromOwner );
    DevelHandle::PropertySetSignal( control ).Connect( mSlotDelegate, &LayoutGroup::OnOwnerPropertySet );
  }
}

void LayoutGroup::OnRegisterChildProperties( const std::string& containerType )
{
  DoRegisterChildProperties( containerType );
}

void LayoutGroup::OnUnparent()
{
  RemoveAll();
}

void LayoutGroup::ChildAddedToOwner( Actor child )
{
  LayoutBasePtr childLayout;
  Toolkit::Control control = Toolkit::Control::DownCast( child );
  DALI_LOG_INFO( gLogFilter, Debug::Verbose, "LayoutGroup::ChildAddedToOwner(%s)\n", control.GetName().c_str() );

  if( control ) // Can only support adding Controls, not Actors to layout
  {
    Internal::Control& childControlImpl = GetImplementation( control );
    Internal::Control::Impl& childControlDataImpl = Internal::Control::Impl::Get( childControlImpl );
    childLayout = childControlDataImpl.GetLayout();

    if( ! childLayout )
    {
      // If the child doesn't already have a layout, then create a LayoutBase for it.
      childLayout = LayoutBase::New( control );
      childLayout->SetAnimateLayout( IsLayoutAnimated() ); // @todo this essentially forces animation inheritance. Bad?

      auto desiredSize = GetNaturalSizeStrippedOfPadding( control );
      DALI_LOG_INFO( gLogFilter, Debug::Verbose, "LayoutGroup::ChildAddedToOwner desiredSize(%f,%f)\n", desiredSize.width, desiredSize.height );

      childControlDataImpl.SetLayout( *childLayout.Get() );

      // Default layout data for this object
      child.SetProperty( Toolkit::LayoutBase::ChildProperty::WIDTH_SPECIFICATION, LayoutLength::IntType( desiredSize.width ) );
      child.SetProperty( Toolkit::LayoutBase::ChildProperty::HEIGHT_SPECIFICATION, LayoutLength::IntType( desiredSize.height ) );
    }

    Add( *childLayout.Get() );
  }
}

void LayoutGroup::ChildRemovedFromOwner( Actor child )
{
  Toolkit::Control control = Toolkit::Control::DownCast( child );
  if( control )
  {
    Internal::Control& childControlImpl = GetImplementation( control );
    Internal::Control::Impl& childControlDataImpl = Internal::Control::Impl::Get( childControlImpl );
    auto childLayout = childControlDataImpl.GetLayout();
    if( childLayout )
    {
      Remove( *childLayout.Get() );
    }
  }
}

void LayoutGroup::OnOwnerPropertySet( Handle& handle, Property::Index index, Property::Value value )
{
  DALI_LOG_INFO( gLogFilter, Debug::Concise, "LayoutGroup::OnOwnerPropertySet\n");
  auto actor = Actor::DownCast( handle );
  if( actor &&
      (
        index == Actor::Property::LAYOUT_DIRECTION  ||
        index == Toolkit::Control::Property::PADDING  ||
        index == Toolkit::Control::Property::MARGIN
      )
    )
  {
    RequestLayout();
  }
}

} // namespace Internal
} // namespace Toolkit
} // namespace Dali
