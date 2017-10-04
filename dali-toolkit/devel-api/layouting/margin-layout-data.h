#ifndef DALI_TOOLKIT_LAYOUTING_MARGIN_LAYOUT_DATA_H
#define DALI_TOOLKIT_LAYOUTING_MARGIN_LAYOUT_DATA_H

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

#include <dali/public-api/common/dali-common.h>
#include <dali-toolkit/devel-api/layouting/child-layout-data.h>

namespace Dali
{
namespace Toolkit
{

namespace Internal DALI_INTERNAL
{
class MarginLayoutData;
}

class DALI_IMPORT_API MarginLayoutData : public ChildLayoutData
{
public:

  MarginLayoutData();
  static MarginLayoutData New( uint16_t width, uint16_t height, uint16_t begin, uint16_t end, uint16_t top, uint16_t bottom );
  static MarginLayoutData DownCast( BaseHandle handle );
  MarginLayoutData( const MarginLayoutData& other );
  MarginLayoutData& operator=( const MarginLayoutData& other );
  ~MarginLayoutData()=default;

public:
  explicit DALI_INTERNAL MarginLayoutData( Internal::MarginLayoutData* body );
};

} // namespace Toolkit
} // namespace Dali

#endif // DALI_TOOLKIT_LAYOUTING_MARGIN_LAYOUT_DATA_H
