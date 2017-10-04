#ifndef DALI_TOOLKIT_LAYOUTING_MEASURE_SPEC_H
#define DALI_TOOLKIT_LAYOUTING_MEASURE_SPEC_H

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

namespace Dali
{
namespace Toolkit
{

class DALI_IMPORT_API MeasureSpec
{
public:
  static const uint32_t MODE_SHIFT  = 30;
  static const uint32_t MODE_MASK   = (0x3 << MODE_SHIFT);
  static const uint32_t UNSPECIFIED = (0x0 << MODE_SHIFT);
  static const uint32_t EXACTLY     = (0x1 << MODE_SHIFT);
  static const uint32_t AT_MOST     = (0x2 << MODE_SHIFT);

  MeasureSpec( uint32_t measureSpec )
  : mMeasureSpec( measureSpec )
  {
  }

  ~MeasureSpec() = default;

  MeasureSpec& operator=( const MeasureSpec& rhs )
  {
    this->mMeasureSpec = rhs.mMeasureSpec;
    return *this;
  }

  MeasureSpec& operator=( uint32_t rhs )
  {
    this->mMeasureSpec = rhs;
    return *this;
  }

  bool operator==( MeasureSpec value )
  {
    return mMeasureSpec == value.mMeasureSpec;
  }

  bool operator!=( MeasureSpec value )
  {
    return mMeasureSpec != value.mMeasureSpec;
  }

  static uint32_t MakeMeasureSpec( MeasureSpec size, const uint32_t mode)
  {
    return ( size.mMeasureSpec & ~MeasureSpec::MODE_MASK ) | ( mode & MeasureSpec::MODE_MASK );
  }

  static uint32_t GetMode( MeasureSpec measureSpec )
  {
    return ( measureSpec.mMeasureSpec & MODE_MASK );
  }

  static uint32_t GetSize( MeasureSpec measureSpec )
  {
    return ( measureSpec.mMeasureSpec & ~MODE_MASK );
  }

  static uint32_t Adjust( MeasureSpec measureSpec, int32_t delta )
  {
    uint32_t mode = GetMode( measureSpec );
    uint32_t size = GetSize( measureSpec );

    if( mode == UNSPECIFIED )
    {
      return MakeMeasureSpec( size, UNSPECIFIED );
    }

    if( delta < 0 && measureSpec.mMeasureSpec < static_cast<uint32_t>(abs(delta)) )
    {
      size = 0;
    }
    else
    {
      size += delta;
    }
    return MakeMeasureSpec( size, mode );
  }

public:
  uint32_t mMeasureSpec;
};


} //namespace Toolkit
} //namespace Dali


#endif // DALI_TOOLKIT_LAYOUTING_MEASURE_SPEC_H
