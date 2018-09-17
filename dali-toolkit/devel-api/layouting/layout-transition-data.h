#ifndef DALI_TOOLKIT_LAYOUTING_LAYOUT_TRANSITION_DATA_H
#define DALI_TOOLKIT_LAYOUTING_LAYOUT_TRANSITION_DATA_H
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

#include <memory>
#include <cstdint>
#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/actors/actor-enumerations.h>
#include <dali/public-api/animation/animation.h>
#include <dali/public-api/object/base-handle.h>
#include <dali/public-api/object/property-index-ranges.h>
#include <dali/public-api/object/property-map.h>
#include <dali-toolkit/public-api/dali-toolkit-common.h>

namespace Dali
{
namespace Toolkit
{

namespace Internal
{
class LayoutTransitionData;
}

class DALI_TOOLKIT_API LayoutTransitionData final : public BaseHandle
{
public:

  /**
   * @brief Enumeration for property animator keys belonging to the LayoutTransitionData class.
   */
  struct AnimatorKey
  {
    enum Type
    {
      PROPERTY,           ///< A property to animate
      INITIAL_VALUE,      ///< Initial value of an animated property
      TARGET_VALUE,       ///< Target value of an animated property
      ANIMATOR,           ///< Animator for an animated property
      TYPE,               ///< Type of an animator
      NAME,               ///< Name of an animator
      TIME_PERIOD,        ///< Time period of an property animation
      DURATION,           ///< Duration of an property animation
      DELAY,              ///< Delay of an property animation
      ALPHA_FUNCTION,     ///< Alpha function of a property animation
    };
  };

  enum LayoutTransitionType
  {
    ON_CHILD_ADD,
    ON_CHILD_REMOVE,
    ON_OWNER_SET
  };

  typedef Signal< void (LayoutTransitionData::LayoutTransitionType, LayoutTransitionData&) > LayoutTransitionSignalType; ///< Transition finished signal

  /**
   * Create an uninitialized handle
   *
   * @SINCE_1_2.12
   */
  LayoutTransitionData();

  /**
   * Destructor - non virtual
   *
   * @SINCE_1_2.12
   */
  ~LayoutTransitionData() = default;

  /**
   * Create an initialized LayoutTransitionData
   */
  static LayoutTransitionData New();

  /**
   * @brief Downcasts a handle to LayoutTransitionData handle.
   *
   * If handle points to an LayoutTransitionData object, the downcast produces valid handle.
   * If not, the returned handle is left uninitialized.
   *
   * @param[in] handle Handle to an object
   * @return Handle to an LayoutTransitionData object or an uninitialized handle
   */
  static LayoutTransitionData DownCast( BaseHandle handle );

  /**
   * @brief Add a property animator for an actor
   *
   * @param[in] actor An owner of the animated property
   * @param[in] map The animated property map including animator map
   */
  void AddPropertyAnimator( Actor actor, Property::Map map );

  /**
   * @brief Add a property animator for an actor
   *
   * @param[in] actor An owner of the animated property
   * @param[in] map The properties map
   * @param[in] keyFrames key frames
   * @param[in] interpolation An interpolation
   */
  void AddPropertyAnimator( Actor actor, Property::Map map, KeyFrames keyFrames, Animation::Interpolation interpolation );

  /**
   * @brief Add a property animator for an actor
   *
   * @param[in] actor An owner of the animated property
   * @param[in] map A properties map
   * @param[in] path An animation path
   * @param[in] forward A forward vector for the path
   */
  void AddPropertyAnimator( Actor actor, Property::Map map, Path path, Vector3 forward );

  /**
    * @brief Connects to this signal to be notified when a transition animation have finished.
    *
    * @return A signal object to connect with
    */
  LayoutTransitionSignalType& FinishedSignal();

public:
  /// @cond internal
  /**
   * @brief This constructor is used by LayoutTransitionData::New() method.
   *
   * @param[in] layoutTransitonData A pointer to a newly allocated transition data resource
   */
  DALI_INTERNAL explicit LayoutTransitionData( Internal::LayoutTransitionData* layoutTransitonData );
  /// @endcond
};

}//namespace Toolkit
}//namespace Dali

#endif // DALI_TOOLKIT_LAYOUTING_LAYOUT_TRANSITION_DATA_H
