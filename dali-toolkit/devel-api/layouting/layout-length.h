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

// EXTERNAL INCLUDES
#include <cstdint>
#include <cmath>
#include <iostream>

namespace Dali
{

namespace Toolkit
{

/**
 * @brief A type that represents a layout length.
 *
 * Currently, this implies pixels, but could be extended to handle device dependent sizes, etc.
 */
class LayoutLength
{
public:

  /**
   * @brief default constructor, initialises to 0
   */
  LayoutLength()
  : mValue( 0 )
  {
  }

  /**
   * @brief constructor
   *
   * @param value the value to initialise with
   */
  LayoutLength( int value )
  : mValue( value )
  {
  }

  /**
   * @brief constructor from float
   *
   * @param value the value to initialise with
   */
  LayoutLength( float value )
  : mValue( value )
  {
  }

  /**
   * @brief constructor
   *
   * @param layoutLength the value to initialise with
   */
  LayoutLength( const LayoutLength& layoutLength )
  : mValue( layoutLength.mValue )
  {
  }

  /**
   * @brief assignment operator
   *
   * @param rhs LayoutLength to assign
   * @return reference to itself
   */
  LayoutLength& operator=( const LayoutLength& rhs )
  {
    if( this != &rhs )
    {
      mValue = rhs.mValue;
    }
    return *this;
  }

  /**
   * @brief assignment operator from int
   *
   * @param value the value to assign
   * @return reference to itself
   */
  LayoutLength& operator=( int value )
  {
    mValue = value;
    return *this;
  }

  /**
   * @brief assignment operator from float
   *
   * @param value the value to assign
   * @return reference to itself
   */
  LayoutLength& operator=( float value )
  {
    mValue = value;
    return *this;
  }

  /**
   * @brief plus equals operator
   *
   * @param rhs to add to this
   * @return reference to itself
   */
  LayoutLength& operator+=( LayoutLength rhs )
  {
    mValue += rhs.mValue;
    return *this;
  }

  /**
   * @brief minus equals operator
   *
   * @param rhs to subtract from this
   * @return reference to itself
   */
  LayoutLength& operator-=( LayoutLength rhs )
  {
    mValue -= rhs.mValue;
    return *this;
  }

  /**
   * @brief return value as decimal value (full raw value)
   *
   * @return the LayoutLength as full decimal value
   */
  float AsDecimal() const
  {
    return mValue;
  }

  /**
   * @brief return value as rounded integer value (whole number)
   *
   * @return the LayoutLength rounded to next integer value
   */
  float AsInteger() const
  {
    return round( mValue );
  }

  /**
   * @brief return value as truncated integral value (whole number)
   *
   * @return the LayoutLength rounded to next integral value
   */
  float AsTruncated() const
  {
    return trunc( mValue );
  }

private:

  float mValue;

};

/**
 * @brief equal to operator
 *
 * @param lhs value to compare
 * @param rhs value to compare against
 * @note Using value instead of reference to allow conversions from int and float
 * @return true if identical, false otherwise
 */
inline bool operator==( LayoutLength lhs, LayoutLength rhs )
{
  return lhs.AsDecimal() == rhs.AsDecimal();
}

/**
 * @brief not equal to operator
 *
 * @param lhs value to compare
 * @param rhs value to compare against
 * @note Using value instead of reference to allow conversions from int and float
 * @return true if value is not identical, false otherwise
 */
inline bool operator!=( LayoutLength lhs, LayoutLength rhs )
{
  return !operator==( lhs, rhs );
}

/**
 * @brief less than operator
 *
 * @param lhs value to compare
 * @param rhs value to compare against
 * @note Using value instead of reference to allow conversions from int and float
 * @return true if lhs value is less, false otherwise
 */
inline bool operator<( LayoutLength lhs, LayoutLength rhs )
{
  return lhs.AsDecimal() < rhs.AsDecimal();
}

/**
 * @brief greater than operator
 *
 * @param lhs value to compare
 * @param rhs value to compare against
 * @note Using value instead of reference to allow conversions from int and float
 * @return true if lhs value is greater, false otherwise
 */
inline bool operator>( LayoutLength lhs, LayoutLength rhs )
{
  return rhs < lhs;
}

/**
 * @brief less than or equal to operator
 *
 * @param lhs value to compare
 * @param rhs value to compare against
 * @note Using value instead of reference to allow conversions from int and float
 * @return true if lhs value is less than or equal to, false otherwise
 */
inline bool operator<=( LayoutLength lhs, LayoutLength rhs )
{
  return !(lhs > rhs);
}

/**
 * @brief greater than or equal to operator
 *
 * @param lhs value to compare
 * @param rhs value to compare against
 * @note Using value instead of reference to allow conversions from int and float
 * @return true if lhs value is greater than or equal to, false otherwise
 */
inline bool operator>=( LayoutLength lhs, LayoutLength rhs )
{
  return !(lhs < rhs);
}

/**
 * @brief add two LayoutLengths
 *
 * @param lhs The LayoutLength to add
 * @param rhs The LayoutLength to add
 * @note Using value instead of reference to allow conversions from int and float
 * @return the resulting LayoutLength
 */
inline LayoutLength operator+( LayoutLength lhs, LayoutLength rhs )
{
  return lhs.AsDecimal() + rhs.AsDecimal();
}

/**
 * @brief subtract two LayoutLengths
 *
 * @param lhs The LayoutLength to subtract from
 * @param rhs The LayoutLength to subtract
 * @note Using value instead of reference to allow conversions from int and float
 * @return the resulting LayoutLength
 */
inline LayoutLength operator-( LayoutLength lhs, LayoutLength rhs )
{
  return lhs.AsDecimal() - rhs.AsDecimal();
}

/**
 * @brief multiply two LayoutLengths
 *
 * @param lhs The LayoutLength to multiply
 * @param rhs The LayoutLength to multiply
 * @note Using value instead of reference to allow conversions from int and float
 * @return the resulting LayoutLength
 */
inline LayoutLength operator*( LayoutLength lhs, LayoutLength rhs )
{
  return lhs.AsDecimal() * rhs.AsDecimal();
}

/**
 * @brief divide LayoutLength by a LayoutLength
 *
 * @param lhs The LayoutLength to divide
 * @param rhs The LayoutLength to divide by
 * @note Using value instead of reference to allow conversions from int and float
 * @return the resulting value as float
 */
inline LayoutLength operator/( LayoutLength lhs, LayoutLength rhs )
{
  return lhs.AsDecimal() / rhs.AsDecimal();
}

/**
 * @brief Prints a LayoutLength
 *
 * @param[in] o The output stream operator
 * @param[in] layoutLength the layout length to print
 * @return The output stream operator
 */
inline std::ostream& operator<<( std::ostream& o, const LayoutLength& layoutLength )
{
  return o << layoutLength.AsDecimal();
}

} // namespace Toolkit

} // namespace Dali

#endif //DALI_TOOLKIT_DEVEL_LAYOUT_LENGTH_H
