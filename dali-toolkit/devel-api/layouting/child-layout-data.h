#ifndef DALI_TOOLKIT_LAYOUTING_CHILD_LAYOUT_DATA_H
#define DALI_TOOLKIT_LAYOUTING_CHILD_LAYOUT_DATA_H

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
#include <dali/public-api/object/base-handle.h>

namespace Dali
{
namespace Toolkit
{

namespace Internal DALI_INTERNAL
{
class ChildLayoutData;
}

class DALI_IMPORT_API ChildLayoutData : public BaseHandle
{
public:
  static const uint16_t MATCH_PARENT = uint16_t(-1);
  static const uint16_t WRAP_CONTENT = uint16_t(-2);

  ChildLayoutData();
  static ChildLayoutData New( uint16_t width, uint16_t height );
  static ChildLayoutData DownCast( BaseHandle handle );
  ChildLayoutData( const ChildLayoutData& other );
  ChildLayoutData& operator=( const ChildLayoutData& other );
  ~ChildLayoutData()=default;

public:
  uint16_t GetWidth();
  uint16_t GetHeight();
  void SetWidth( uint16_t width );
  void SetHeight( uint16_t height );

public:
  explicit DALI_INTERNAL ChildLayoutData( Internal::ChildLayoutData* body );
};

} // namespace Toolkit
} // namespace Dali

#endif // DALI_TOOLKIT_LAYOUTING_CHILD_LAYOUT_DATA_H
