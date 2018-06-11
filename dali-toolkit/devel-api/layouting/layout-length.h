#ifndef DALI_TOOLKIT_DEVEL_LAYOUT_LENGTH_H
#define DALI_TOOLKIT_DEVEL_LAYOUT_LENGTH_H

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
#include <cstdint>
#include <iostream>

namespace Dali
{
namespace Toolkit
{

/**
 * @brief A type that represents a layout length.
 *
 * Currently, this implies pixels, but could be extended to handle device dependant sizes, etc.
 */
class LayoutLength
{
public:
  using IntType = int;

  LayoutLength( IntType value )
  : mValue( value )
  {
  }

  LayoutLength( const LayoutLength& layoutLength )
  : mValue( layoutLength.mValue )
  {
  }

  LayoutLength& operator=(const LayoutLength& rhs)
  {
    if( this != &rhs )
    {
      mValue = rhs.mValue;
    }
    return *this;
  }

  bool operator==( const LayoutLength& rhs )
  {
    return mValue == rhs.mValue;
  }

  bool operator==( LayoutLength::IntType rhs )
  {
    return mValue == rhs;
  }

  bool operator!=( const LayoutLength& rhs )
  {
    return !operator==(rhs);
  }

  bool operator<( const LayoutLength& rhs )
  {
    return mValue < rhs.mValue;
  }

  bool operator<( const LayoutLength rhs ) const
  {
    return mValue < rhs.mValue;
  }

  bool operator<=( const LayoutLength& rhs )
  {
    return mValue <= rhs.mValue;
  }
  bool operator<=( LayoutLength rhs )
  {
    return mValue <= rhs.mValue;
  }
  bool operator>( const LayoutLength& rhs )
  {
    return mValue > rhs.mValue;
  }
  bool operator>( LayoutLength rhs )
  {
    return mValue > rhs.mValue;
  }
  bool operator>=( const LayoutLength& rhs )
  {
    return mValue >= rhs.mValue;
  }
  bool operator>=( LayoutLength rhs )
  {
    return mValue >= rhs.mValue;
  }

  LayoutLength operator+( const LayoutLength& rhs )
  {
    return mValue + rhs.mValue;
  }

  LayoutLength operator+( LayoutLength::IntType rhs )
  {
    return mValue + rhs;
  }

  LayoutLength operator-( const LayoutLength& rhs )
  {
    return mValue - rhs.mValue;
  }
  LayoutLength operator-( LayoutLength::IntType rhs )
  {
    return mValue - rhs;
  }

  LayoutLength& operator+=( const LayoutLength& rhs )
  {
    mValue += rhs.mValue;
    return *this;
  }
  LayoutLength& operator+=( LayoutLength::IntType rhs )
  {
    mValue += rhs;
    return *this;
  }

  LayoutLength& operator-=( const LayoutLength& rhs )
  {
    mValue -= rhs.mValue;
    return *this;
  }

  LayoutLength& operator-=( LayoutLength::IntType rhs )
  {
    mValue -= rhs;
    return *this;
  }

  LayoutLength operator/( const LayoutLength& rhs )
  {
    return mValue / rhs.mValue;
  }
  LayoutLength operator/(  LayoutLength::IntType rhs )
  {
    return mValue / rhs;
  }

  LayoutLength operator*( const LayoutLength& rhs )
  {
    return mValue * rhs.mValue;
  }
  LayoutLength operator*( LayoutLength::IntType rhs )
  {
    return mValue * rhs;
  }
  LayoutLength operator*( float rhs )
  {
    return LayoutLength(LayoutLength::IntType(float(mValue) * rhs));
  }

  operator float()
  {
    return float( mValue );
  }

  IntType mValue;
};

/**
 * @brief Prints a LayoutLength
 *
 * @param[in] o The output stream operator
 * @param[in] layoutLength the layout length to print
 * @return The output stream operator
 */
inline std::ostream& operator<<( std::ostream& o, const LayoutLength& layoutLength )
{
  return o<<layoutLength.mValue;
}

} // namespace Toolkit

} // namespace Dali

#endif //DALI_TOOLKIT_DEVEL_LAYOUT_LENGTH_H
