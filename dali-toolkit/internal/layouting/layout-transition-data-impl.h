#ifndef DALI_TOOLKIT_INTERNAL_LAYOUTING_LAYOUT_TRANSITION_DATA_IMPL_H
#define DALI_TOOLKIT_INTERNAL_LAYOUTING_LAYOUT_TRANSITION_DATA_IMPL_H
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

#include <dali/public-api/common/intrusive-ptr.h>
#include <dali/public-api/object/base-object.h>
#include <dali/public-api/object/property-map.h>
#include <dali/public-api/object/type-registry.h>
#include <dali/public-api/object/weak-handle.h>
#include <dali/public-api/actors/actor-enumerations.h>
#include <dali/public-api/animation/animation.h>

#include <dali-toolkit/devel-api/layouting/layout-item-impl.h>
#include <dali-toolkit/devel-api/layouting/layout-transition-data.h>

namespace Dali
{
namespace Toolkit
{
namespace Internal
{

struct LayoutTransition
{
  LayoutTransition( LayoutItem& layoutItem, int layoutTransitionType, Actor gainedChild, Actor lostChild )
  : layoutItem( &layoutItem )
  , layoutTransitionType( layoutTransitionType )
  , gainedChild( gainedChild )
  , lostChild( lostChild )
  {
  }

  LayoutTransition()
  : layoutTransitionType( -1 )
  {
  }

  bool operator==( const LayoutTransition& rhs )
  {
    return ( layoutItem.Get() == rhs.layoutItem.Get()
        && layoutTransitionType == rhs.layoutTransitionType
        && gainedChild == rhs.gainedChild
        && lostChild == rhs.lostChild );
  }

  LayoutItemPtr layoutItem;
  int layoutTransitionType;
  WeakHandle<Actor> gainedChild;
  WeakHandle<Actor> lostChild;
};

const float DEFAULT_TRANSITION_DURATION( 0.5f );

struct LayoutDataAnimator
{
   LayoutDataAnimator()
   : animatorType( Toolkit::LayoutTransitionData::Animator::ANIMATE_TO ),
     alphaFunction( AlphaFunction::LINEAR ),
     timePeriod( 0.0f, DEFAULT_TRANSITION_DURATION ),
     interpolation( Animation::Linear )
   {
   }

   std::string name;
   Toolkit::LayoutTransitionData::Animator::Type animatorType;
   AlphaFunction alphaFunction;
   TimePeriod timePeriod;

   KeyFrames keyFrames;
   Animation::Interpolation interpolation;

   Path path;
   Vector3 forward;
};

using LayoutAnimatorArray = std::vector< LayoutDataAnimator >;

struct LayoutPositionData
{
  LayoutPositionData( Actor handle, float left, float top, float right, float bottom, bool animated ) :
      handle( handle ), left( left ), top( top ), right( right ), bottom( bottom ), animated( animated ), updateWithCurrentSize(false)
  {
  };

  WeakHandle<Actor> handle;
  float left;
  float top;
  float right;
  float bottom;
  bool animated;
  bool updateWithCurrentSize;
};

using LayoutPositionDataArray = std::vector< LayoutPositionData >;

struct LayoutDataElement
{
  LayoutDataElement()
  : propertyIndex( Property::INVALID_KEY ),
    animatorIndex( -1 ),
    positionDataIndex(-1 ),
    condition( Dali::Toolkit::LayoutTransitionData::Condition::NONE ),
    updateMeasuredSize( false )
  {
  };

  bool AdjustMeasuredSize( float& width, float& height, Toolkit::LayoutTransitionData::Animator::Type animatorType );
  void UpdatePropertyIndex();
  void UpdateAnimatorIndex( const LayoutAnimatorArray& animators );
  void UpdatePositionDataIndex( LayoutData& layoutData );

  WeakHandle<Actor> handle;
  std::string propertyName;
  Property::Index propertyIndex;
  Property::Value initialValue;
  Property::Value targetValue;
  std::string animatorName;
  int animatorIndex;
  int positionDataIndex;
  int condition;
  bool updateMeasuredSize;
};

using LayoutDataArray = std::vector< LayoutDataElement >;

class LayoutTransitionData;
using LayoutTransitionDataPtr = IntrusivePtr<LayoutTransitionData>;

/**
 * LayoutTransitionData implementation class.
 */
class DALI_TOOLKIT_API LayoutTransitionData : public BaseObject
{
public:
  struct PropertyAnimator
  {
    PropertyAnimator();
    PropertyAnimator( Actor actor, Property::Map map );
    PropertyAnimator( Actor actor, Property::Map map, Path path, Vector3 forward );
    PropertyAnimator( Actor actor, Property::Map map, KeyFrames keyFrames, Animation::Interpolation interpolation );

    WeakHandle<Actor> handle;

    Property::Map map;

    KeyFrames keyFrames;
    Animation::Interpolation interpolation;

    Path path;
    Vector3 forward;
  };
  using PropertyAnimatorArray = std::vector< PropertyAnimator >;

  static LayoutTransitionDataPtr New();

  LayoutTransitionData( const LayoutTransitionData& ) = delete;
  LayoutTransitionData& operator=( const LayoutTransitionData& ) = delete;

  /**
   * @brief Add a property animator for an actor in the transition
   * @param[in] actor The actor
   * @param[in] map The map containing the transition animator keys
   *
   * This will parse the property animator map and add the layout data element to the array of layout data elements related to this transition
   */
  void AddPropertyAnimator( Actor actor, Property::Map map );

  /**
   * @brief Add a property animator for an actor in the transition
   * @param[in] actor The actor
   * @param[in] map The map containing the transition animator keys
   * @param[in] keyFrames The key frames used by the property animator
   * @param[in] interpolation The interpolation used by the property animator
   *
   * This will parse the property animator map and add the layout data element to the array of layout data elements related to this transition
   */
  void AddPropertyAnimator( Actor actor, Property::Map map, KeyFrames keyFrames, Animation::Interpolation interpolation );

  /**
   * @brief Add a property animator for an actor in the transition
   * @param[in] actor The actor
   * @param[in] map The map containing the transition animator keys
   * @param[in] path The path for the property animator
   * @param[in] forward The forward vector for the property animator
   *
   * This will parse the property animator map and add the layout data element to the array of layout data elements related to this transition
   */
  void AddPropertyAnimator( Actor actor, Property::Map map, Path path, Vector3 forward );

  /**
   * @brief Collect the transition layout data elements
   * @param[in] actor The actor the transition property animators are applied to
   * @param[in] layoutData The layout data containing layout data elements array for the layout update
   *
   * This will copy the transition layout data elements to the layout data elements array
   */
  void CollectLayoutDataElements( Actor, LayoutData& layoutData );

  /**
   * @brief Collect the transition children layout data elements
   * @param[in] actor The actor the transition property animators are applied to
   * @param[in] layoutData The layout data containing layout data elements array for the layout update
   *
   * This will copy the children transition layout data elements to the layout data elements array
   */
  static void CollectChildrenLayoutDataElements( Actor, LayoutData& layoutData );

  /**
   * @copydoc Dali::Toolkit::LayoutTransitionData::FinishedSignal()
   */
  Dali::Toolkit::LayoutTransitionData::LayoutTransitionSignalType& FinishedSignal();

  /**
   * @brief Emit the transition finish signal
   * @param[in] layoutTransitionType The transition type
   */
  void EmitSignalFinish( int layoutTransitionType );

  /**
   * @brief Check if one of the layout data elements has updateMeasuredSize flag set
   */
  bool HasUpdateMeasuredSize();

private:
  /**
   * @brief Convert the property animator data to the layout data animator
   * @param[in] animatorData The animator data map
   * @param[in] propertyAnimator The property animator
   * @param[in] layoutAnimator The layout animator
   *
   * This will parse the property animator map and add the layout data element animator to the layout animators array
   */
  bool ConvertToLayoutAnimator( const Property::Map& animatorData, const PropertyAnimator& propertyAnimator, LayoutDataAnimator& layoutAnimator );

  /**
   * @brief Convert the property animator to the layout data element
   * @param[in] propertyAnimator The property animator
   * @param[in] layoutDataElement The layout data element
   * @param[in] layoutDataElement The layout data
   *
   * This will parse the property animator map and add the layout data element to the layout data elements array
   */
  bool ConvertToLayoutDataElement( const PropertyAnimator& propertyAnimator, LayoutDataElement& layoutDataElement );

  void UpdateAnimatorsIndices();

  bool mUpdateMeasuredSize;
  LayoutAnimatorArray mLayoutAnimators;
  LayoutDataArray mLayoutDataElements;

  /**
   * Ref counted object - Only allow construction via New().
   */
  LayoutTransitionData();

protected:
  /**
   *  A ref counted object may only be deleted by calling Unreference
   */
  virtual ~LayoutTransitionData();

  Dali::Toolkit::LayoutTransitionData::LayoutTransitionSignalType mFinishedSignal;
};

using PropertyAnimatorArray = std::vector< LayoutTransitionData::PropertyAnimator >;
using LayoutTransitionDataArray = std::vector< LayoutTransitionDataPtr >;
using LayoutDataArray = std::vector< LayoutDataElement >;

struct LayoutData
{
  LayoutData( LayoutTransition& layoutTransition, LayoutPositionDataArray& layoutPositionDataArray, LayoutAnimatorArray& layoutAnimatorArray,
      LayoutDataArray& layoutDataArray, LayoutDataArray& childrenLayoutDataArray )
  : speculativeLayout( false ),
    updateMeasuredSize( false ),
    layoutTransition( layoutTransition ),
    layoutPositionDataArray( layoutPositionDataArray ),
    layoutAnimatorArray( layoutAnimatorArray ),
    layoutDataArray( layoutDataArray),
    childrenLayoutDataArray( childrenLayoutDataArray )
  {
  };

  bool speculativeLayout;
  bool updateMeasuredSize;
  LayoutTransition& layoutTransition;
  LayoutPositionDataArray& layoutPositionDataArray;
  LayoutAnimatorArray& layoutAnimatorArray;
  LayoutDataArray& layoutDataArray;
  LayoutDataArray& childrenLayoutDataArray;
};

} //namespace Internal

inline Internal::LayoutTransitionData& GetImplementation( Dali::Toolkit::LayoutTransitionData& handle )
{
  DALI_ASSERT_ALWAYS( handle && "LayoutTransitionData handle is empty" );
  BaseObject& object = handle.GetBaseObject();
  return static_cast< Internal::LayoutTransitionData& >( object );
}

inline const Internal::LayoutTransitionData& GetImplementation( const Dali::Toolkit::LayoutTransitionData& handle )
{
  DALI_ASSERT_ALWAYS( handle && "LayoutTransitionData handle is empty" );
  const BaseObject& object = handle.GetBaseObject();
  return static_cast< const Internal::LayoutTransitionData& >( object );
}

} //namespace Toolkit
} //namespace Dali

#endif // DALI_TOOLKIT_INTERNAL_LAYOUTING_LAYOUT_TRANSITION_DATA_IMPL_H
