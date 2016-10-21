#ifndef __DALI_TOOLKIT_TRANSITION_DATA_H__
#define __DALI_TOOLKIT_TRANSITION_DATA_H__

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

// EXTERNAL INCLUDES
#include <dali/public-api/object/base-handle.h>
#include <dali/public-api/animation/alpha-function.h>
#include <dali/public-api/object/property-array.h>
#include <dali/public-api/object/property-map.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{
class TransitionData;
}

/**
 * @brief This object translates data from a property array of maps
 * into an array of animators.
 *
 * Each animator describes a named object and a named property of that object
 * to be animated. Internally, these are translated into object instances and
 * property indices to be animated.
 * @see Dali::Toolkit::Internal::Control::CreateTransition()
 *
 * The animators can each be retrieved as a Property::Map by using Count() and
 * GetAnimatorAt().
 *
 * In psuedo-JSON, the property array can be represented as follows:
 *
 * [
 *   {
 *     "target": "objectName",
 *     "property": "propertyKey",
 *     "initialValue": <value>,  # The property value can be one of several types
 *     "targetValue":  <value>,
 *     "animator": {
 *       "alphaFunction":<easing-function>",
 *       "timePeriod":{
 *         "duration": 1.0,
 *         "delay":    0.0
 *       }
 *     }
 *   },
 *   # more animators
 * ]
 *
 */
class DALI_IMPORT_API TransitionData : public BaseHandle
{
public:
  /**
   * Create an uninitialized handle
   *
   * @SINCE_1_2.12
   */
  TransitionData();

  /**
   * Destructor - non virtual
   *
   * @SINCE_1_2.12
   */
  ~TransitionData();

  /**
   * @brief Creates a TransitionData object
   *
   * @SINCE_1_2.12
   * @param[in] transition The transition data to store (a single animator)
   * @return A handle to an initialized data.
   */
  static TransitionData New( const Property::Map& transition );

  /**
   * @brief Creates a TransitionData object
   *
   * @SINCE_1_2.12
   * @param[in] transition The transition data to store (an array of maps of animators)
   * @return A handle to an initialized data.
   */
  static TransitionData New( const Property::Array& transition );

  /**
   * @brief Downcast to a TransitionData handle
   *
   * @SINCE_1_2.12
   * If handle is not a TransitionData, the returned handle is left uninitialized.
   * @param[in] handle Handle to an object
   * @return TransitionData handle or an uninitialized handle.
   */
  static TransitionData DownCast( BaseHandle handle );

  /**
   * @brief Copy constructor
   *
   * @SINCE_1_2.12
   * @param[in] handle Handle to an object
   */
  TransitionData( const TransitionData& handle );

  /**
   * @brief Assignment Operator
   *
   * @SINCE_1_2.12
   * @param[in] handle Handle to an object
   * @return A reference to this object.
   */
  TransitionData& operator=( const TransitionData& handle );

  /**
   * @brief returns the count of the individual property transitions
   * stored within this handle.
   *
   * @SINCE_1_2.12
   * @return The count of individual transitions
   */
  size_t Count() const;

  /**
   * @brief Return the animator at the given index as a property map.
   * @param[in] index The index of the animator ( Must be less than Count() )
   * @return A property map representing the animator
   */
  Property::Map GetAnimatorAt( size_t index );

public: // Not intended for application developers

  explicit DALI_INTERNAL TransitionData( Internal::TransitionData *impl );
};

} // namespace Toolkit
} // namespace Dali

#endif // __DALI_TOOLKIT_TRANSITION_DATA_H__
