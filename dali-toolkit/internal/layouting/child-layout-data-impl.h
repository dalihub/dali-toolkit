#ifndef DALI_TOOLKIT_INTERNAL_LAYOUTING_CHILD_LAYOUT_DATA_H
#define DALI_TOOLKIT_INTERNAL_LAYOUTING_CHILD_LAYOUT_DATA_H

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
#include <dali/public-api/math/uint-16-pair.h>
#include <dali/public-api/object/base-object.h>

#include <dali-toolkit/devel-api/layouting/child-layout-data.h>

namespace Dali
{
namespace Toolkit
{
namespace Internal
{

class ChildLayoutData;
using ChildLayoutDataPtr = IntrusivePtr<ChildLayoutData>;

class ChildLayoutData : public BaseObject
{
public:
  static ChildLayoutDataPtr New( uint16_t width, uint16_t height );
  virtual ChildLayoutDataPtr Clone();

  ChildLayoutData( const ChildLayoutData& other ) = delete;
  ChildLayoutData& operator=( const ChildLayoutData& other ) = delete;

  uint16_t GetWidth();
  uint16_t GetHeight();
  void SetWidth( uint16_t width );
  void SetHeight( uint16_t height );

protected:
  ChildLayoutData( uint16_t width, uint16_t height );
  virtual ~ChildLayoutData();

private:
  Uint16Pair mSize;
};

} // namespace Internal

inline Internal::ChildLayoutData& GetImplementation( Dali::Toolkit::ChildLayoutData& handle )
{
  DALI_ASSERT_ALWAYS( handle && "ChildLayoutData handle is empty" );
  BaseObject& object = handle.GetBaseObject();
  return static_cast<Internal::ChildLayoutData&>( object );
}

inline const Internal::ChildLayoutData& GetImplementation( const Dali::Toolkit::ChildLayoutData& handle )
{
  DALI_ASSERT_ALWAYS( handle && "ChildLayoutData handle is empty" );
  const BaseObject& object = handle.GetBaseObject();
  return static_cast<const Internal::ChildLayoutData&>( object );
}

} // namespace Toolkit
} // namespace Dali

#endif // DALI_TOOLKIT_INTERNAL_LAYOUTING_CHILD_LAYOUT_DATA_H
