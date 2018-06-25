#ifndef DALI_TOOLKIT_DEVEL_LAYOUT_SIZE_H
#define DALI_TOOLKIT_DEVEL_LAYOUT_SIZE_H

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

#include <dali-toolkit/devel-api/layouting/layout-length.h>

namespace Dali
{
namespace Toolkit
{

/**
 * This class represents a layout size (width and height)
 */
class LayoutSize
{
public:
  LayoutSize()
  : x(0u),
    y(0u)
  {
  }

  LayoutSize( LayoutLength::IntType anX, LayoutLength::IntType aY )
  : x( anX ),
    y( aY )
  {
  }

  LayoutSize( const LayoutSize& layoutSize )
  : x( layoutSize.x ),
    y( layoutSize.y )
  {
  }

  LayoutSize& operator=(const LayoutSize& rhs)
  {
    x = rhs.x;
    y = rhs.y;
    return *this;
  }

  LayoutLength::IntType GetWidth()
  {
    return width;
  }

  LayoutLength::IntType GetHeight()
  {
    return height;
  }

  void SetWidth(LayoutLength::IntType value)
  {
    width=value;
  }
  void SetHeight(LayoutLength::IntType value)
  {
    height=value;
  }
  void SetWidth(LayoutLength value)
  {
    width=value.mValue;
  }
  void SetHeight(LayoutLength value)
  {
    height=value.mValue;
  }

  bool operator==( const LayoutSize& rhs )
  {
    return x==rhs.x && y==rhs.y;
  }

  bool operator!=( const LayoutSize& rhs )
  {
    return !( *this==rhs );
  }

  union
  {
    LayoutLength::IntType x;
    LayoutLength::IntType width;
  };

  union
  {
    LayoutLength::IntType y;
    LayoutLength::IntType height;
  };
};

/**
 * @brief Prints a LayoutSize
 *
 * @param[in] o The output stream operator
 * @param[in] layoutSize the layout size to print
 * @return The output stream operator
 */
inline std::ostream& operator<< (std::ostream& o, const LayoutSize& layoutSize)
{
  return o << "[" << layoutSize.x << ", " << layoutSize.y << "]";
}

} // namespace Toolkit

} // namespace Dali

#endif //DALI_TOOLKIT_DEVEL_LAYOUT_SIZE_H
