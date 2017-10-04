#ifndef DALI_TOOLKIT_LAYOUTING_MEASURED_SIZE_H
#define DALI_TOOLKIT_LAYOUTING_MEASURED_SIZE_H

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

/**
 * Class that encodes a measurement into a 16 bit value, using the top
 * bit to signify if the measured size is too small.
 */
class DALI_IMPORT_API MeasuredSize
{
public:
  /**
   * Mask for masking out the states of the measured size
   */
  static const uint16_t MEASURED_SIZE_MASK = 0x7FFF;

  /**
   * Mask for masking out the states of the measured size
   */
  static const uint16_t MEASURED_STATE_MASK = 0x8000;

  /**
   * Bit indicating if the measured size is too small
   */
  static const uint16_t MEASURED_SIZE_TOO_SMALL = 0x8000;


  MeasuredSize( uint16_t measuredSize )
  : mMeasuredSize( measuredSize )
  {
  }

  MeasuredSize( uint16_t measuredSize, uint16_t measuredState )
  : mMeasuredSize( measuredSize | measuredState )
  {
  }

  ~MeasuredSize() = default;

  MeasuredSize& operator=( const MeasuredSize& rhs )
  {
    this->mMeasuredSize = rhs.mMeasuredSize;
    return *this;
  }

  MeasuredSize& operator=( uint16_t rhs )
  {
    this->mMeasuredSize = rhs;
    return *this;
  }

  inline bool operator==( MeasuredSize value )
  {
    return mMeasuredSize == value.mMeasuredSize;
  }

  inline bool operator!=( MeasuredSize value )
  {
    return mMeasuredSize != value.mMeasuredSize;
  }

  inline operator uint16_t()
  {
    return mMeasuredSize;
  }

  inline uint16_t GetState()
  {
    return ( mMeasuredSize & MEASURED_STATE_MASK );
  }

  inline uint16_t GetSize()
  {
    return ( mMeasuredSize & MEASURED_SIZE_MASK );
  }

private:
  uint16_t mMeasuredSize;
};


} //namespace Toolkit
} //namespace Dali


#endif // DALI_TOOLKIT_LAYOUTING_MEASURED_SIZE_H
