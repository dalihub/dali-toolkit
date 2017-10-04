/*
 * Copyright (c) 2017 Samsung Electronics Co., Ltd.
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
#include <dali-toolkit/internal/layouting/margin-layout-data-impl.h>

//EXTERNAL HEADERS
//INTERNAL HEADERS

namespace Dali
{
namespace Toolkit
{
namespace Internal
{

MarginLayoutDataPtr MarginLayoutData::New( uint16_t width, uint16_t height, uint16_t begin, uint16_t end, uint16_t top, uint16_t bottom )
{
  MarginLayoutDataPtr object( new MarginLayoutData( width, height, begin, end, top, bottom ) );
  return object;
}

MarginLayoutData::MarginLayoutData( uint16_t width, uint16_t height, uint16_t begin, uint16_t end, uint16_t top, uint16_t bottom )
: ChildLayoutData(width, height),
  mBeginMargin(begin),
  mEndMargin(end),
  mTopMargin(top),
  mBottomMargin(bottom)
{
}

MarginLayoutData::~MarginLayoutData()
{
}

} // namespace Internal
} // namespace Toolkit
} // namespace Dali
