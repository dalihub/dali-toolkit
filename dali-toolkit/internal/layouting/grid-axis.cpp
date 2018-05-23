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
: mLocations()
{
}

GridAxis::~GridAxis(){}

void GridAxis::CalculateLocations( int numberOfColumns, unsigned int availableWidth, unsigned int availableHeight, unsigned int numberOfCells )
{
  mLocations.clear();

  auto numberOfRows = numberOfCells/numberOfColumns;
  int columnWidth = availableWidth / numberOfColumns;
  int rowHeight = availableHeight / numberOfRows;
  DALI_LOG_INFO( gLogFilter, Debug::Verbose, "ColumWidth[%d] RowHeight[%d]\n", columnWidth, rowHeight );

  auto y1 = 0;
  auto y2 = y1 + rowHeight;

  // Iterate rows
  for( auto i = 0u; i < numberOfRows; i++ )
  {
    auto x1 = 0;
    auto x2 = x1 + columnWidth;

    // Iterate columns
    for( auto j = 0; j < numberOfColumns; j++ )
    {
      DALI_LOG_INFO( gLogFilter, Debug::Verbose, "Cell[%d] rows[%u] column[%u] x[%d,%d] y[%d,%d]\n", i, j, x1, x2, y1, y2  );
      GridAxis::Cell cell( x1, x2, y1, y2 );
      mLocations.push_back( cell );
      x2 = x2 + columnWidth;
    }
    y1 = y2;
    y2 = y2 + rowHeight;
  }

#if defined(DEBUG_ENABLED)

  std::ostringstream oss;
  DALI_LOG_INFO( gLogFilter, Debug::Verbose, "Locations::CalculateLocations (%d)\n", numberOfCells );
  for( auto i = 0u; i < numberOfCells + numberOfColumns; i++ )
  {
    oss << "h:" << mLocations[i].xStart << "," << mLocations[i].xEnd << "," << mLocations[i].yTop << "," << mLocations[i].yBottom << std::endl;
  }
  DALI_LOG_INFO( gLogFilter, Debug::Verbose, oss.str().c_str() );
#endif
}

GridAxis::LocationVector GridAxis::GetLocations()
{
#if defined(DEBUG_ENABLED)
  std::ostringstream oss;
  auto itemCount = mLocations.size(); // mVerticalLocations mirrors this so same size.
  oss << "GetLocations:";
  for( auto i = 0u; i < itemCount; i++ )
  {
    oss << mLocations[i].xStart << "," << mLocations[i].xEnd << "," << mLocations[i].yTop << "," << mLocations[i].yBottom << std::endl;
  }
  oss << std::endl;
  DALI_LOG_INFO( gLogFilter, Debug::Verbose, oss.str().c_str() );
#endif

  return mLocations;
}

} // namespace Internal

} // namespace Toolkit
} // namespace Dali