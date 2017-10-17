#ifndef DALI_TOOLKIT_INTERNAL_LAYOUTING_MARGIN_LAYOUT_DATA_H
#define DALI_TOOLKIT_INTERNAL_LAYOUTING_MARGIN_LAYOUT_DATA_H

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

#include <dali/public-api/common/intrusive-ptr.h>
#include <dali-toolkit/internal/layouting/child-layout-data-impl.h>

#include <dali-toolkit/devel-api/layouting/margin-layout-data.h>

namespace Dali
{
namespace Toolkit
{
namespace Internal
{

class MarginLayoutData;
using MarginLayoutDataPtr = IntrusivePtr<MarginLayoutData>;

class MarginLayoutData : public ChildLayoutData
{
public:
  static MarginLayoutDataPtr New( uint16_t width, uint16_t height, uint16_t begin, uint16_t end, uint16_t top, uint16_t bottom );
  virtual ChildLayoutDataPtr Clone();

  MarginLayoutData( const MarginLayoutData& other ) = delete;
  MarginLayoutData& operator=( const MarginLayoutData& other ) = delete;

  /**
   * Get the margin data
   * @return the margin
   */
  const Extents& GetMargin();

protected:
  MarginLayoutData( uint16_t width, uint16_t height, uint16_t begin, uint16_t end, uint16_t top, uint16_t bottom );
  virtual ~MarginLayoutData();

private:
  Extents mMargin;
};

} // namespace Internal

inline Internal::MarginLayoutData& GetImplementation( Dali::Toolkit::MarginLayoutData& handle )
{
  DALI_ASSERT_ALWAYS( handle && "MarginLayoutData handle is empty" );
  BaseObject& object = handle.GetBaseObject();
  return static_cast<Internal::MarginLayoutData&>( object );
}

inline const Internal::MarginLayoutData& GetImplementation( const Dali::Toolkit::MarginLayoutData& handle )
{
  DALI_ASSERT_ALWAYS( handle && "MarginLayoutData handle is empty" );
  const BaseObject& object = handle.GetBaseObject();
  return static_cast<const Internal::MarginLayoutData&>( object );
}

} // namespace Toolkit
} // namespace Dali

#endif // DALI_TOOLKIT_INTERNAL_LAYOUTING_MARGIN_LAYOUT_DATA_H
