#ifndef DALI_TOOLKIT_LAYOUTING_MEASURED_SIZE_H
#define DALI_TOOLKIT_LAYOUTING_MEASURED_SIZE_H

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

#include <dali/public-api/common/dali-common.h>
#include <dali-toolkit/devel-api/layouting/layout-length.h>
#include <dali-toolkit/public-api/dali-toolkit-common.h>

namespace Dali
{
namespace Toolkit
{

/**
 * Class that encodes a measurement and a measure state, which is set if the measured size is too small.
 */
class DALI_TOOLKIT_API MeasuredSize
{
public:

  enum State
  {
    MEASURED_SIZE_OK, ///< The measured size is good
    MEASURED_SIZE_TOO_SMALL ///< The measured size is too small
  };

  MeasuredSize()
  : mMeasuredSize( 0u ),
    mState ( MeasuredSize::State::MEASURED_SIZE_OK )
  {
  }

  MeasuredSize( LayoutLength measuredSize )
  : mMeasuredSize( measuredSize ),
    mState ( MeasuredSize::State::MEASURED_SIZE_OK )
  {
  }

  MeasuredSize( LayoutLength measuredSize, MeasuredSize::State state )
  : mMeasuredSize( measuredSize ),
    mState( state )
  {
  }

  ~MeasuredSize() = default;

  MeasuredSize& operator=( const MeasuredSize& rhs )
  {
    if( this != &rhs )
    {
      this->mMeasuredSize = rhs.mMeasuredSize;
      this->mState = rhs.mState;
    }
    return *this;
  }

  MeasuredSize& operator=( LayoutLength::IntType rhs )
  {
    this->mMeasuredSize = rhs;
    this->mState = State::MEASURED_SIZE_OK;
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

  inline operator LayoutLength::IntType()
  {
    return mMeasuredSize.mValue;
  }

  inline void SetState( MeasuredSize::State state )
  {
    mState = state;
  }

  inline MeasuredSize::State GetState()
  {
    return mState;
  }

  inline void SetSize( LayoutLength size )
  {
    mMeasuredSize = size;
  }
  inline LayoutLength GetSize()
  {
    return mMeasuredSize;
  }

private:
  LayoutLength mMeasuredSize; ///< The measured size
  State mState; ///< The measured state
};

} //namespace Toolkit
} //namespace Dali


#endif // DALI_TOOLKIT_LAYOUTING_MEASURED_SIZE_H
