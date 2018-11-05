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

// EXTERNAL INCLUDES
#include <sstream>
#include <dali-toolkit/public-api/dali-toolkit-common.h>
#include <dali/public-api/common/dali-common.h>

// INTERNAL INCLUDES
#include <dali-toolkit/devel-api/layouting/layout-length.h>
#include <dali-toolkit/public-api/dali-toolkit-common.h>

namespace Dali
{
namespace Toolkit
{

/**
 * A MeasureSpec is used during the Measure pass by a LayoutGroup to inform it's children
 * how to be measured. For instance, it may measure a child with an exact width and an unspecified
 * height in order to determine height for width.
 */
class DALI_TOOLKIT_API MeasureSpec
{
public:

  enum class Mode
  {
    UNSPECIFIED, ///< This is used by a parent to determine the desired dimension of a child layout.
    EXACTLY, /** This is used by a parent to impose an exact size on the child. The child must use
                 this size, and guarantee that all of its descendants will fit within this size */
    AT_MOST /** This is used by the parent to impose a maximum size on the child. The child must guarantee
             * that it and all of it's descendants will fit within this size. */
  };

  MeasureSpec( LayoutLength measureSpec, MeasureSpec::Mode mode )
  : mSize( measureSpec ),
    mMode( mode )
  {
  }

  MeasureSpec( LayoutLength measureSpec )
  : mSize( measureSpec ),
    mMode( Mode::UNSPECIFIED )
  {
  }

  ~MeasureSpec() = default;

  MeasureSpec& operator=( const MeasureSpec& rhs )
  {
    if( this != &rhs )
    {
      this->mSize = rhs.mSize;
      this->mMode = rhs.mMode;
    }
    return *this;
  }

  bool operator==( MeasureSpec value )
  {
    return mSize == value.mSize;
  }

  bool operator!=( MeasureSpec value )
  {
    return mSize != value.mSize;
  }

  /**
   * @brief Set the mode of the measure spec.
   *
   * @param mode The mode to set
   */
  void SetMode( MeasureSpec::Mode mode )
  {
    mMode = mode;
  }

  /**
   * @brief Get the mode of the measure spec.
   *
   * @return The mode of the measure spec
   */
  MeasureSpec::Mode GetMode() const
  {
    return mMode;
  }

  /**
   * @brief Set the size of the measure spec
   *
   * @param size the size to set
   */
  void SetSize( LayoutLength size )
  {
    mSize = size;
  }

  /**
   * @brief Get the size of the measure spec
   *
   * @return the size of the measure spec
   */
  LayoutLength GetSize() const
  {
    return mSize;
  }

  /**
   * @brief Adjust the measure size by the given delta.
   *
   * Used only for EXACT and AT_MOST modes.
   * @param[in] measureSpec the measure spec to adjust
   * @param[in] delta A positive or negative value to adjust the measure spec by.
   *
   * @note if the adjusted size is negative, it is zeroed.
   * @return A new measure spec with the adjusted values.
   */
  static MeasureSpec Adjust( MeasureSpec measureSpec, int delta )
  {
    auto mode = measureSpec.GetMode();
    auto size = measureSpec.GetSize();

    if( mode == MeasureSpec::Mode::UNSPECIFIED )
    {
      return MeasureSpec( size, MeasureSpec::Mode::UNSPECIFIED );
    }

    if( delta < 0 && measureSpec.mSize < abs(delta) )
    {
      size = 0;
    }
    else
    {
      size += delta;
    }
    return MeasureSpec( size, mode );
  }

private:

  LayoutLength  mSize; ///< The specified size
  Mode     mMode; ///< The measure mode

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
