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
#include <dali-toolkit/internal/layouting/size-negotiation-mapper.h>

// EXTERNAL HEADER
#include <dali/integration-api/debug.h>

// INTERNAL HEADER
#include <dali-toolkit/devel-api/controls/control-devel.h>
#include <dali-toolkit/devel-api/layouting/child-layout-data.h>
#include <dali-toolkit/devel-api/layouting/layout-item-impl.h>

namespace
{
#if defined(DEBUG_ENABLED)
Debug::Filter* gLogFilter = Debug::Filter::New( Debug::NoLogging, false, "LOG_LAYOUT" );
#endif

/**
 * Set the width specification with the given layout params
 */
void SetWidthLayoutParams( Dali::Toolkit::Control control, int layoutParams )
{
  DALI_LOG_INFO( gLogFilter, Debug::General, "SizeNegotiationMapper::SetLayoutParamsUsingResizePolicy Setting WIDTH LayoutParam(%d) for %s\n",
                                             layoutParams,
                                             control.GetName().c_str() );
  control.SetProperty( Dali::Toolkit::LayoutItem::ChildProperty::WIDTH_SPECIFICATION, layoutParams );
}

/**
 * Set the height specification with the given layout params
 */
void SetHeightLayoutParams( Dali::Toolkit::Control control, int layoutParams )
{
  DALI_LOG_INFO( gLogFilter, Debug::General, "SizeNegotiationMapper::SetLayoutParamsUsingResizePolicy Setting HEIGHT LayoutParam(%d) for %s\n",
                                             layoutParams,
                                             control.GetName().c_str() );
  control.SetProperty( Dali::Toolkit::LayoutItem::ChildProperty::HEIGHT_SPECIFICATION, layoutParams );
}

/**
 * Call SetResizePolicyRequired true on the layout belonging to the provided control
 */
void SetResizePolicyRequiredFlag( Dali::Toolkit::Internal::LayoutItemPtr layout )
{
  layout->SetResizePolicyRequired( true );
}

} // namspace

namespace Dali
{
namespace Toolkit
{
namespace Internal
{

void SizeNegotiationMapper::SetLayoutParametersUsingResizePolicy( Toolkit::Control control, Toolkit::Internal::LayoutItemPtr layout, Dimension::Type dimension )
{
  // Get control's current width specification
  int matchedLayoutParamWidth = control.GetProperty<int>( Toolkit::LayoutItem::ChildProperty::WIDTH_SPECIFICATION );
  int matchedLayoutParamHeight = control.GetProperty<int>( Toolkit::LayoutItem::ChildProperty::HEIGHT_SPECIFICATION );

  // Get control's Resize Policy that should be mapped to a width specification
  const ResizePolicy::Type resizePolicy = control.GetResizePolicy( dimension );

  DALI_LOG_INFO( gLogFilter, Debug::Verbose, "SizeNegotiationMapper::SetLayoutParamsUsingResizePolicy ResizePolicy for control:%s resizePolicy %d for dimension %d\n",
                                              control?control.GetName().c_str():"Invalid",
                                              resizePolicy,
                                              dimension );

  bool matchFound( false );

  // Use the control's ResizePolicy to determine the new specification
  switch( resizePolicy )
  {
    case ResizePolicy::USE_ASSIGNED_SIZE :
    {
      DALI_LOG_INFO( gLogFilter, Debug::Verbose, "SizeNegotiationMapper::SetLayoutParamsUsingResizePolicy USE_ASSIGNED_SIZE\n");
      // Set by legacy controls on their children. Will not be exposed to a Layout.
      break;
    }

    case ResizePolicy::FIXED :
    {
      DALI_LOG_INFO( gLogFilter, Debug::Verbose, "SizeNegotiationMapper::SetLayoutParamsUsingResizePolicy FIXED\n");
      Vector3 controlSize = control.GetTargetSize();
      matchedLayoutParamWidth = controlSize.width;
      matchedLayoutParamHeight = controlSize.height;
      matchFound = true;
      break;
    };
    case ResizePolicy::USE_NATURAL_SIZE :
    {
      DALI_LOG_INFO( gLogFilter, Debug::Verbose, "SizeNegotiationMapper::SetLayoutParamsUsingResizePolicy USE_NATURAL_SIZE\n");
      // Default ResizePolicy for controls.
      // LayoutGroups are containers, containers will not have a natural size.
      break;
    };
    case ResizePolicy::FILL_TO_PARENT :
    {
      DALI_LOG_INFO( gLogFilter, Debug::Verbose, "SizeNegotiationMapper::SetLayoutParamsUsingResizePolicy FILL_TO_PARENT\n");
      matchedLayoutParamWidth = ChildLayoutData::MATCH_PARENT;
      matchedLayoutParamHeight = ChildLayoutData::MATCH_PARENT;
      matchFound = true;
      break;
    };
    case ResizePolicy::SIZE_RELATIVE_TO_PARENT :
    {
      SetResizePolicyRequiredFlag( layout ); // Defer setting the exact size until the parent size is known.
      break;
    };
    case ResizePolicy::SIZE_FIXED_OFFSET_FROM_PARENT :
    {
      SetResizePolicyRequiredFlag( layout ); // Defer setting the exact size until the parent size is known.
      break;
    };
    case ResizePolicy::FIT_TO_CHILDREN :
    {
      DALI_LOG_INFO( gLogFilter, Debug::Verbose, "SizeNegotiationMapper::SetLayoutParamsUsingResizePolicy FIT_TO_CHILDREN\n");
      matchedLayoutParamWidth = ChildLayoutData::WRAP_CONTENT;
      matchedLayoutParamHeight = ChildLayoutData::WRAP_CONTENT;
      matchFound = true;
      break;
    };
    case ResizePolicy::DIMENSION_DEPENDENCY :
    {
      // Not supported
      break;
    };
  }

  // Use the resize policy for the required dimensions only.
  // Possible that only one dimension was set hence either the matchedLayoutParamWidth or
  // matchedLayoutParamHeight should not be used.
  if( matchFound )
  {
    if( dimension & Dimension::WIDTH )
    {
      SetWidthLayoutParams( control, matchedLayoutParamWidth );
    }

    if( dimension & Dimension::HEIGHT )
    {
      SetHeightLayoutParams( control, matchedLayoutParamHeight );
    }
  }
};

void SizeNegotiationMapper::GetSizeofChildForParentDependentResizePolicy( Toolkit::Control control, const MeasureSpec parentWidthSpecification, const MeasureSpec parentHeightSpecification, LayoutLength& childWidth, LayoutLength& childHeight)
{
  DALI_LOG_INFO( gLogFilter, Debug::General, "SizeNegotiationMapper::SetLayoutParamsForParentDependantResizePolicy ResizePolicy required for %s\n", control.GetName().c_str() );
  ResizePolicy::Type widthResizePolicy = control.GetResizePolicy( Dimension::WIDTH );
  ResizePolicy::Type heightResizePolicy = control.GetResizePolicy( Dimension::HEIGHT );
  Vector3 sizeModeFactor = control.GetSizeModeFactor();

  DALI_LOG_INFO( gLogFilter, Debug::Verbose, "SizeNegotiationMapper::MeasureChild SetLayoutParamsForParentDependantResizePolicy for control:%s resizePolicy w:%d h:%d  modeFactor(%f,%f,%f)\n",
                                              control?control.GetName().c_str():"Invalid",
                                              widthResizePolicy, heightResizePolicy,
                                              sizeModeFactor.x,
                                              sizeModeFactor.y,
                                              sizeModeFactor.z );

  if( widthResizePolicy == ResizePolicy::SIZE_RELATIVE_TO_PARENT )
  {
    DALI_LOG_INFO( gLogFilter, Debug::Verbose, "SizeNegotiationMapper::SetLayoutParamsForParentDependantResizePolicy width SIZE_RELATIVE_TO_PARENT\n");
    childWidth = parentWidthSpecification.GetSize().AsDecimal() * sizeModeFactor.x;
  }
  else if( widthResizePolicy == ResizePolicy::SIZE_FIXED_OFFSET_FROM_PARENT )
  {
    DALI_LOG_INFO( gLogFilter, Debug::Verbose, "SizeNegotiationMapper::SetLayoutParamsForParentDependantResizePolicy width SIZE_FIXED_OFFSET_FROM_PARENT\n");
    childWidth = parentWidthSpecification.GetSize().AsDecimal() + sizeModeFactor.x;
  }

  if( heightResizePolicy == ResizePolicy::SIZE_RELATIVE_TO_PARENT )
  {
    DALI_LOG_INFO( gLogFilter, Debug::Verbose, "SizeNegotiationMapper::SetLayoutParamsForParentDependantResizePolicy height  SIZE_RELATIVE_TO_PARENT\n");
    childHeight = parentHeightSpecification.GetSize().AsDecimal() * sizeModeFactor.y;
  }
  else if( heightResizePolicy == ResizePolicy::SIZE_FIXED_OFFSET_FROM_PARENT )
  {
    DALI_LOG_INFO( gLogFilter, Debug::Verbose, "SizeNegotiationMapper::SetLayoutParamsForParentDependantResizePolicy height  SIZE_FIXED_OFFSET_FROM_PARENT\n");
    childHeight = parentHeightSpecification.GetSize().AsDecimal() + sizeModeFactor.y;
  }

  // DIMENSION_DEPENDENCY not supported

  DALI_LOG_INFO( gLogFilter, Debug::General, "SizeNegotiationMapper::SetLayoutParamsForParentDependantResizePolicy child size(%f,%f)\n",  childWidth.AsInteger(), childHeight.AsInteger() );
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali