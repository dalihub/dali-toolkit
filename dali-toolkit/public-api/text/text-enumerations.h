#ifndef DALI_TOOLKIT_TEXT_ENUMERATIONS_H
#define DALI_TOOLKIT_TEXT_ENUMERATIONS_H

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
 *
 */

namespace Dali
{

namespace Toolkit
{

/**
 * @addtogroup dali_toolkit_controls
 * @{
 */

namespace Text
{

/**
 * @brief The available Horizontal alignments for text
 * @SINCE_1_2.60
 */
namespace HorizontalAlignment
{
  /**
   * @brief Enumerations for Horizontal alignment
   *
   * @SINCE_1_2.60
   */
  enum Type
  {
    BEGIN,   ///< @SINCE_1_2.60
    CENTER,  ///< @SINCE_1_2.60
    END      ///< @SINCE_1_2.60
  };
}

/**
 * @brief The available Vertical alignments for text
 * @SINCE_1_2.60
 */
namespace VerticalAlignment
{
  /**
   * @brief Enumerations for Vertical alignment
   *
   * @SINCE_1_2.60
   */
  enum Type
  {
    TOP,     ///< @SINCE_1_2.60
    CENTER,  ///< @SINCE_1_2.60
    BOTTOM   ///< @SINCE_1_2.60
  };
}

} // Text

/**
 * @}
 */

} // Toolkit

} // Dali

#endif //DALI_TOOLKIT_TEXT_ENUMERATIONS_H
