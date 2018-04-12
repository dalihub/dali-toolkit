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
#include <dali/public-api/object/type-registry-helper.h>
#include <dali/devel-api/object/handle-devel.h>
#include <dali-toolkit/devel-api/layouting/layout-group-impl.h>
#include <dali-toolkit/internal/layouting/layout-group-data-impl.h>


namespace
{
const char* MARGIN_SPECIFICATION_NAME( "marginSpec" );
}

namespace Dali
{
namespace Toolkit
{
namespace Internal
{

LayoutGroup::LayoutGroup()
: mImpl( new Impl( *this ) )
{
}

LayoutGroup::~LayoutGroup()
{
}

LayoutGroup& LayoutGroup::operator=( const LayoutGroup& handle )
{
  if( &handle != this )
  {
    LayoutGroup::operator=( handle );
  }
  return *this;
}

Dali::Toolkit::LayoutGroup LayoutGroup::New( LayoutGroupPtr layoutGroup )
{
  Dali::Toolkit::LayoutGroup handle( *layoutGroup );

  return handle;
}

void LayoutGroup::DoRegisterChildProperties( const std::type_info& containerType )
{
  // Chain up to parent class
  LayoutBase::DoRegisterChildProperties( containerType );

  auto typeInfo = TypeRegistry::Get().GetTypeInfo( containerType );
  if( typeInfo )
  {
    Property::IndexContainer indices;
    typeInfo.GetChildPropertyIndices( indices );

    if( std::find( indices.Begin(), indices.End(), Toolkit::LayoutGroup::ChildProperty::MARGIN_SPECIFICATION ) ==
        indices.End() )
    {

      ChildPropertyRegistration( typeInfo.GetName(), MARGIN_SPECIFICATION_NAME, Toolkit::LayoutGroup::ChildProperty::MARGIN_SPECIFICATION, Property::EXTENTS );
    }
  }
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

  // If there are no LayoutBase child properties, add them
  if( ! DevelHandle::DoesCustomPropertyExist( owner, Toolkit::LayoutBase::ChildProperty::WIDTH_SPECIFICATION ) )
  {
    GenerateDefaultChildProperties( owner );
  }

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
      mImpl->mChildren.erase(iter);
      break;
    }
  }
  RequestLayout();
}

LayoutBasePtr LayoutGroup::GetChild( Toolkit::LayoutGroup::LayoutId childId )
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

unsigned int LayoutGroup::GetChildCount()
{
  return mImpl->mChildren.size();
}

LayoutParent* LayoutGroup::GetParent()
{
  return LayoutBase::GetParent();
}

void LayoutGroup::OnSetChildProperties( Handle& handle, Property::Index index, Property::Value value )
{
  if ( ( index >= CHILD_PROPERTY_REGISTRATION_START_INDEX ) && ( index <= CHILD_PROPERTY_REGISTRATION_MAX_INDEX ) )
  {
    // If any child properties are set, must perform relayout
    RequestLayout();
  }
}

void LayoutGroup::GenerateDefaultChildProperties( Handle child )
{
  child.SetProperty( Toolkit::LayoutBase::ChildProperty::WIDTH_SPECIFICATION,
                     Toolkit::ChildLayoutData::WRAP_CONTENT );
  child.SetProperty( Toolkit::LayoutBase::ChildProperty::HEIGHT_SPECIFICATION,
                     Toolkit::ChildLayoutData::WRAP_CONTENT );
  child.SetProperty( Toolkit::LayoutGroup::ChildProperty::MARGIN_SPECIFICATION, Extents() );
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
  auto childOwner = child->GetOwner();
  auto desiredWidth = childOwner.GetProperty<int>( Toolkit::LayoutBase::ChildProperty::WIDTH_SPECIFICATION );
  auto desiredHeight = childOwner.GetProperty<int>( Toolkit::LayoutBase::ChildProperty::HEIGHT_SPECIFICATION );

  auto padding = GetPadding();

  const MeasureSpec childWidthMeasureSpec = GetChildMeasureSpec( parentWidthMeasureSpec,
                                                                 padding.start + padding.end,
                                                                 desiredWidth);
  const MeasureSpec childHeightMeasureSpec = GetChildMeasureSpec( parentHeightMeasureSpec,
                                                                  padding.top + padding.bottom,
                                                                  desiredHeight);

  child->Measure( childWidthMeasureSpec, childHeightMeasureSpec );
}

void LayoutGroup::MeasureChildWithMargins( LayoutBasePtr child,
                                           MeasureSpec parentWidthMeasureSpec, uint16_t widthUsed,
                                           MeasureSpec parentHeightMeasureSpec, uint16_t heightUsed)
{
  auto childOwner = child->GetOwner();
  auto desiredWidth = childOwner.GetProperty<int>( Toolkit::LayoutBase::ChildProperty::WIDTH_SPECIFICATION );
  auto desiredHeight = childOwner.GetProperty<int>( Toolkit::LayoutBase::ChildProperty::HEIGHT_SPECIFICATION );
  auto desiredMargin = childOwner.GetProperty<Extents>( Toolkit::LayoutGroup::ChildProperty::MARGIN_SPECIFICATION );

  MeasureSpec childWidthMeasureSpec = GetChildMeasureSpec( parentWidthMeasureSpec,
                                                           mImpl->mPadding.start + mImpl->mPadding.end +
                                                           desiredMargin.start + desiredMargin.end +
                                                           widthUsed, desiredWidth );

  MeasureSpec childHeightMeasureSpec = GetChildMeasureSpec( parentHeightMeasureSpec,
                                                            mImpl->mPadding.top + mImpl->mPadding.bottom +
                                                            desiredMargin.top + desiredMargin.end +
                                                            heightUsed, desiredHeight );

  child->Measure( childWidthMeasureSpec, childHeightMeasureSpec );
}


MeasureSpec LayoutGroup::GetChildMeasureSpec(
  MeasureSpec measureSpec,
  int padding,
  uint16_t childDimension )
{
  int specMode = measureSpec.GetMode();
  int specSize = measureSpec.GetSize();

  int size = std::max( 0, specSize - padding );

  int resultSize = 0;
  int resultMode = 0;

  switch( specMode )
  {
    // Parent has imposed an exact size on us
    case MeasureSpec::EXACTLY:
    {

      if (childDimension == Toolkit::ChildLayoutData::MATCH_PARENT)
      {
        // Child wants to be our size. So be it.
        resultSize = size;
        resultMode = MeasureSpec::EXACTLY;
      }
      else if (childDimension == Toolkit::ChildLayoutData::WRAP_CONTENT)
      {
        // Child wants to determine its own size. It can't be
        // bigger than us.
        resultSize = size;
        resultMode = MeasureSpec::AT_MOST;
      }
      else
      {
        resultSize = childDimension;
        resultMode = MeasureSpec::EXACTLY;
      }

      break;
    }

      // Parent has imposed a maximum size on us
    case MeasureSpec::AT_MOST:
    {
      if (childDimension == Toolkit::ChildLayoutData::MATCH_PARENT)
      {
        // Child wants to be our size, but our size is not fixed.
        // Constrain child to not be bigger than us.
        resultSize = size;
        resultMode = MeasureSpec::AT_MOST;
      }
      else if (childDimension == Toolkit::ChildLayoutData::WRAP_CONTENT)
      {
        // Child wants to determine its own size. It can't be
        // bigger than us.
        resultSize = size;
        resultMode = MeasureSpec::AT_MOST;
      }
      else
      {
        // Child wants a specific size... so be it
        resultSize = childDimension;
        resultMode = MeasureSpec::EXACTLY;
      }

      break;
    }

      // Parent asked to see how big we want to be
    case MeasureSpec::UNSPECIFIED:
    {
      if (childDimension == Toolkit::ChildLayoutData::MATCH_PARENT)
      {
        // Child wants to be our size... find out how big it should be
        resultSize = LayoutBase::Impl::sUseZeroUnspecifiedMeasureSpec ? 0 : size;
        resultMode = MeasureSpec::UNSPECIFIED;
      }
      else if (childDimension == Toolkit::ChildLayoutData::WRAP_CONTENT)
      {
        // Child wants to determine its own size.... find out how big
        // it should be
        resultSize = LayoutBase::Impl::sUseZeroUnspecifiedMeasureSpec ? 0 : size;
        resultMode = MeasureSpec::UNSPECIFIED;
      }
      else
      {
        // Child wants a specific size... let him have it
        resultSize = childDimension;
        resultMode = MeasureSpec::EXACTLY;
      }
      break;
    }
  }

  //noinspection ResourceType
  return MeasureSpec::MakeMeasureSpec( resultSize, resultMode );
}

bool LayoutGroup::IsLayoutRequested()
{
  return mImpl->GetPrivateFlag( Impl::PFLAG_FORCE_LAYOUT );
}

Extents LayoutGroup::GetPadding()
{
  return mImpl->mPadding;
}

} // namespace Internal
} // namespace Toolkit
} // namespace Dali
