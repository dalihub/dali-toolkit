#ifndef __DALI_TOOLKIT_DEVEL_API_ALIGN_ENUMS_H__
#define __DALI_TOOLKIT_DEVEL_API_ALIGN_ENUMS_H__

/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
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
 *
 */


namespace Dali
{

namespace Toolkit
{

namespace Direction
{
  enum Type
  {
    LEFT_TO_RIGHT = 0,
    RIGHT_TO_LEFT
  };
}

/**
 * @brief Enum used to describe anchor point and parent origin of visuals
 */
namespace Align
{
  enum Type
  {
    TOP_BEGIN      = 0,
    TOP_CENTER,
    TOP_END,
    CENTER_BEGIN,
    CENTER,
    CENTER_END,
    BOTTOM_BEGIN,
    BOTTOM_CENTER,
    BOTTOM_END
  };
}

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_DEVEL_API_ALIGN_ENUMS_H__
