#ifndef DALI_TOOLKIT_INTERNAL_TRANSITION_DATA_H
#define DALI_TOOLKIT_INTERNAL_TRANSITION_DATA_H

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
 */

// EXTERNAL INCLUDES
#include <dali/public-api/object/base-object.h>
#include <dali/public-api/animation/alpha-function.h>
#include <dali/public-api/object/property-key.h>
#include <dali/devel-api/common/owner-container.h>

// INTERNAL INCLUDES
#include <dali-toolkit/devel-api/visual-factory/transition-data.h>

namespace Dali
{
namespace Toolkit
{
namespace Internal
{

class TransitionData;
typedef IntrusivePtr<TransitionData> TransitionDataPtr;

/**
 * TransitionData is an object that holds animator data.
 */
class TransitionData : public BaseObject
{
public:

  /**
   * @brief TransitionDataElement Describes one animator of an transition.
   */
  struct Animator
  {
    Animator()
    : propertyKey( Property::INVALID_INDEX ),
      alphaFunction( AlphaFunction::DEFAULT ),
      timePeriodDelay( 0.0f ),
      timePeriodDuration( 1.0f ),
      animate(false)
    {
    }

    std::string objectName;   ///< An identifier of the actor or visual
    Property::Key propertyKey; ///< A property key of the property owner
    Property::Value initialValue; ///< The value to set at the start of the transition
    Property::Value targetValue;   ///< The value to set or animate to
    Dali::AlphaFunction::BuiltinFunction alphaFunction;
    float timePeriodDelay;
    float timePeriodDuration;
    bool animate;
  };

  /**
   * @brief TransitionData holds the required data required to define an
   * transition to be performed on a property owner
   */
  typedef Dali::OwnerContainer< Animator* > AnimatorList;
  typedef AnimatorList::Iterator Iterator;

public:
  /**
   * @copydoc Dali::Transition::New()
   */
  static TransitionDataPtr New( const Property::Array& value );

  /**
   * @copydoc Dali::Transition::New()
   */
  static TransitionDataPtr New( const Property::Map& value );

  /**
   * @brief Iterator to the beginning of the data
   */
  Iterator Begin() const;

  /**
   * @brief Iterator to the end of the data (one past last element)
   */
  Iterator End() const;

  /**
   * @copydoc Dali::Transition::Count()
   */
  size_t Count() const;

  /**
   * @copydoc Dali::Transition::GetAnimatorAt()
   */
  Property::Map GetAnimatorAt( size_t index );

private: // Implementation
  /**
   * Ref counted object - Only allow construction via New().
   */
  TransitionData();

  /**
   * Second stage initialiazation
   */
  void Initialize( const Property::Map& value );

  /**
   * Second stage initialiazation
   */
  void Initialize( const Property::Array& value );

  /**
   * @brief Adds one Animator to the list to describe a transition.
   * @param[in] animator An animator data structure
   */
  void Add( Animator* animator );

  /**
   * Convert a Property map into Animator data
   */
  Animator* ConvertMap( const Property::Map& map );

protected:
  /**
   *  A ref counted object may only be deleted by calling Unreference
   */
  virtual ~TransitionData();

private: // Unimplemented methods
  TransitionData( const TransitionData& );
  TransitionData& operator=( const TransitionData& );

private: // Data members
  AnimatorList mAnimators; ///< A vector of individual property transitions from which to generate a Dali::Animation.
};

} // namespace Internal

// Helpers for public-api forwarding methods
inline Internal::TransitionData& GetImplementation( Dali::Toolkit::TransitionData& handle )
{
  DALI_ASSERT_ALWAYS(handle && "TransitionData handle is empty");
  BaseObject& object = handle.GetBaseObject();
  return static_cast<Internal::TransitionData&>(object);
}

inline const Internal::TransitionData& GetImplementation( const Dali::Toolkit::TransitionData& handle )
{
  DALI_ASSERT_ALWAYS(handle && "TransitionData handle is empty");
  const BaseObject& object = handle.GetBaseObject();
  return static_cast<const Internal::TransitionData&>(object);
}

} // namespace Toolkit
} // namespace Dali


#endif // DALI_TOOLKIT_INTERNAL_TRANSITION_DATA_H
