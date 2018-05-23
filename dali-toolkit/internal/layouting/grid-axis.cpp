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

//CLASS HEADER
#include <dali-toolkit/internal/layouting/grid-axis.h>

// EXTERNAL HEADERS
#include <dali/integration-api/debug.h>

// INTERNAL HEADERS
#include <dali-toolkit/devel-api/layouting/layout-item.h>

namespace
{

#if defined(DEBUG_ENABLED)
static Debug::Filter* gLogFilter = Debug::Filter::New( Debug::Concise, false, "LOG_LAYOUT" );
#endif

}

namespace Dali
{
namespace Toolkit
{
namespace Internal
{

GridAxisPtr GridAxis::New()
{
  GridAxisPtr gridAxis( new GridAxis() );
  return gridAxis;
}

GridAxis::GridAxis()
: mHorizontalLocations(),
  mVerticalLocations()
{
}

GridAxis::~GridAxis(){}

void GridAxis::CalculateLocations( int numberOfColumns, unsigned int availableWidth, unsigned int availableHeight, unsigned int numberOfCells )
{
  mHorizontalLocations.Clear();
  mVerticalLocations.Clear();

  int columnWidth = availableWidth / numberOfColumns;
  int rowHeight = availableHeight / numberOfColumns;

  for( auto i = 0u; i < numberOfCells + numberOfColumns; i++ )
  {
    unsigned int remainderOnDivision = i % ( numberOfColumns + 1 );
    int xPos = remainderOnDivision * columnWidth;
    DALI_LOG_INFO( gLogFilter, Debug::Verbose, "PushBack[%d] remainderOnDivision H [%d] numberOfColumns[%u]\n", xPos, remainderOnDivision, numberOfColumns );
    mHorizontalLocations.PushBack( xPos );
    int yPos = ( 1 + ( i / ( numberOfColumns + 1 ) ) ) * rowHeight;
    DALI_LOG_INFO( gLogFilter, Debug::Verbose, "PushBack[%d] remainderOnDivision V [%d] numberOfColumns[%u]\n", yPos, remainderOnDivision, numberOfColumns );
    mVerticalLocations.PushBack( yPos );
  }

#if defined(DEBUG_ENABLED)

  std::ostringstream oss;
  DALI_LOG_INFO( gLogFilter, Debug::Verbose, "Locations::CalculateLocations (%d)\n", numberOfCells );
  for( auto i = 0u; i < numberOfCells + numberOfColumns; i++ )
  {
    oss << "h:" << mHorizontalLocations[i] << " v:" << mVerticalLocations[i] << std::endl;
  }
  DALI_LOG_INFO( gLogFilter, Debug::Verbose, oss.str().c_str() );
#endif
}

Dali::Vector<int> GridAxis::GetLocations( unsigned int axis )
{
#if defined(DEBUG_ENABLED)
  std::ostringstream oss;
  auto itemCount = mHorizontalLocations.Count(); // mVerticalLocations mirrors this so same size.
  oss << "GetLocations:";
  for( auto i = 0u; i < itemCount; i++ )
  {
    if( HORIZONTAL == axis )
    {
      oss << mHorizontalLocations[i] << ",";
    }
    else
    {
      oss << mVerticalLocations[i] << ",";
    }
  }
  oss << std::endl;
  DALI_LOG_INFO( gLogFilter, Debug::Verbose, oss.str().c_str() );
#endif

  if( HORIZONTAL == axis )
  {
    return mHorizontalLocations;
  }
  else
  {
    return mVerticalLocations;
  }
}

} // namespace Internal

} // namespace Toolkit
} // namespace Dali