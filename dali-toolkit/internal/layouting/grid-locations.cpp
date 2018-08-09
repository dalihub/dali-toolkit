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
#include <dali-toolkit/internal/layouting/grid-locations.h>

// EXTERNAL HEADERS
#include <dali/integration-api/debug.h>

// INTERNAL HEADERS
#include <dali-toolkit/devel-api/layouting/layout-item.h>

namespace
{

#if defined(DEBUG_ENABLED)
static Debug::Filter* gLogFilter = Debug::Filter::New( Debug::Concise, false, "LOG_AXIS" );
#endif

}

namespace Dali
{
namespace Toolkit
{
namespace Internal
{

GridLocationsPtr GridLocations::New()
{
  GridLocationsPtr gridAxis( new GridLocations() );
  return gridAxis;
}

GridLocations::GridLocations()
: mLocations()
{
}

GridLocations::~GridLocations(){}

void GridLocations::CalculateLocations( int numberOfColumns,
                                        unsigned int availableWidth,
                                        unsigned int availableHeight,
                                        unsigned int numberOfCells,
                                        unsigned int columnWidth,
                                        unsigned int rowHeight )
{
  mLocations.clear();

  // Calculate width and height of columns and rows.

  // Calculate numbers of rows, round down result as later check for remainder.
  unsigned int numberOfRows = numberOfCells/numberOfColumns;
  // If number of cells not cleanly dividable by colums, add another row to house remainder cells.
  numberOfRows += (numberOfCells%numberOfColumns)?1:0;

  unsigned int maxColumnWidth = availableWidth / numberOfColumns;

  if( columnWidth > 0 )
  {
    // Column width supplied so use this unless exceeds available width.
    columnWidth = std::min( columnWidth, maxColumnWidth );
  }
  else
  {
    // Column width not supplied so use calculated value
    columnWidth = maxColumnWidth;
  }

  unsigned int maxRowHeight = availableHeight / numberOfRows;

  if( rowHeight > 0 )
  {
    // Column height supplied so use this unless exceeds available height.
    rowHeight = std::min( rowHeight, maxRowHeight );
  }
  else
  {
    // Column height not supplied so use calculated value
    rowHeight = maxRowHeight;
  }

  DALI_LOG_INFO( gLogFilter, Debug::Verbose, "ColumWidth[%d] RowHeight[%d] NumberOfRows[%d] NumberOfColumns[%d]\n",
                                              columnWidth, rowHeight, numberOfRows, numberOfColumns );
  DALI_LOG_INFO( gLogFilter, Debug::Verbose, "Remainder[%d]\n", numberOfCells%numberOfColumns );

  int  y1 = 0;
  int  y2 = y1 + rowHeight;

  // Calculate start, end, top and bottom coordinate of each cell.

  // Iterate rows
  for( auto i = 0u; i < numberOfRows; i++ )
  {
    int x1 = 0;
    int x2 = x1 + columnWidth;

    // Iterate columns
    for( auto j = 0; j < numberOfColumns; j++ )
    {
      GridLocations::Cell cell( x1, x2, y1, y2 );
      mLocations.push_back( cell );
      // Calculate starting x and ending x position of each column
      x1 = x2;
      x2 = x2 + columnWidth;
    }

    // Calculate top y and bottom y position of each row.
    y1 = y2;
    y2 = y2 + rowHeight;
  }

#if defined(DEBUG_ENABLED)
  std::ostringstream oss;
  DALI_LOG_INFO( gLogFilter, Debug::Verbose, "GridLocations::CalculateLocations (%d)\n", numberOfCells );
  for( auto i = 0u; i < numberOfCells; i++ )
  {
    DALI_LOG_STREAM( gLogFilter, Debug::Verbose,"x1:"<<mLocations[i].xStart<<" x2:"<<mLocations[i].xEnd<<" y1:"<<mLocations[i].yTop<<" y2:"<<mLocations[i].yBottom);
  }
#endif
}

GridLocations::LocationVector GridLocations::GetLocations()
{
#if defined(DEBUG_ENABLED)
  std::ostringstream oss;
  auto itemCount = mLocations.size(); // mVerticalLocations mirrors this so same size.
  DALI_LOG_INFO( gLogFilter, Debug::Verbose, "GridLocations::GetLocations for %u cells\n", itemCount );
  for( auto i = 0u; i < itemCount; i++ )
  {
    DALI_LOG_STREAM( gLogFilter, Debug::Verbose,"x1:"<<mLocations[i].xStart<<" x2:"<<mLocations[i].xEnd<<" y1:"<<mLocations[i].yTop<<" y2:"<<mLocations[i].yBottom);
  }
#endif

  return mLocations;
}

} // namespace Internal

} // namespace Toolkit
} // namespace Dali