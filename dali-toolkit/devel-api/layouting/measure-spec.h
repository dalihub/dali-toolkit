#ifndef DALI_TOOLKIT_LAYOUTING_MEASURE_SPEC_H
#define DALI_TOOLKIT_LAYOUTING_MEASURE_SPEC_H

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

#include <sstream>

namespace Dali
{
namespace Toolkit
{

class DALI_IMPORT_API MeasureSpec
{
public:
  using IntType = LayoutLength::IntType;

  enum class Mode
  {
    UNSPECIFIED,
    EXACTLY,
    AT_MOST
  };

  MeasureSpec( LayoutLength measureSpec, MeasureSpec::Mode mode )
  : mMeasureSpec( measureSpec.mValue ),
    mMode( mode )
  {
  }

  MeasureSpec( IntType measureSpec )
  : mMeasureSpec( measureSpec ),
    mMode( Mode::UNSPECIFIED )
  {
  }

  ~MeasureSpec() = default;

  MeasureSpec& operator=( const MeasureSpec& rhs )
  {
    this->mMeasureSpec = rhs.mMeasureSpec;
    this->mMode = rhs.mMode;
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

  MeasureSpec::Mode GetMode() const
  {
    return mMode;
  }

  IntType GetSize() const
  {
    return mMeasureSpec;
  }

  static MeasureSpec Adjust( MeasureSpec measureSpec, int delta )
  {
    auto mode = measureSpec.GetMode();
    auto size = measureSpec.GetSize();

    if( mode == MeasureSpec::Mode::UNSPECIFIED )
    {
      return MeasureSpec( size, MeasureSpec::Mode::UNSPECIFIED );
    }

    if( delta < 0 && measureSpec.mMeasureSpec < static_cast<IntType>(abs(delta)) )
    {
      size = 0;
    }
    else
    {
      size += delta;
    }
    return MeasureSpec( size, mode );
  }

public:
  IntType  mMeasureSpec;
  Mode     mMode;
};

inline std::ostream& operator<< (std::ostream& o, const MeasureSpec& measureSpec )
{
  return o << ( (measureSpec.GetMode() == MeasureSpec::Mode::UNSPECIFIED ? "Unspecified"
                 : (measureSpec.GetMode() == MeasureSpec::Mode::EXACTLY ? "Exactly":"At most" ) ) )
           << " " << measureSpec.GetSize();
}

} //namespace Toolkit
} //namespace Dali


#endif // DALI_TOOLKIT_LAYOUTING_MEASURE_SPEC_H
