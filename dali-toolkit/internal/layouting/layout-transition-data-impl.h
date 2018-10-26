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
  LayoutTransition( LayoutItem& layoutItem, int layoutTransitionType )
  : layoutItem( &layoutItem )
  , layoutTransitionType( layoutTransitionType )
  {
  }

  LayoutTransition()
  : layoutTransitionType( -1 )
  {
  }

  bool operator==( const LayoutTransition& rhs )
  {
    return ( ( layoutItem.Get() == rhs.layoutItem.Get() ) && layoutTransitionType == rhs.layoutTransitionType );
  }

  LayoutItemPtr layoutItem;
  int layoutTransitionType;
};

const float DEFAULT_TRANSITION_DURATION( 0.5f );

struct LayoutDataAnimator
{
   enum class AnimatorType
   {
     ANIMATE_TO,
     ANIMATE_BY,
     ANIMATE_BETWEEN,
     ANIMATE_PATH
   };

   LayoutDataAnimator()
   : animatorType( AnimatorType::ANIMATE_TO ),
     alphaFunction( AlphaFunction::LINEAR ),
     timePeriod( 0.0f, DEFAULT_TRANSITION_DURATION ),
     interpolation( Animation::Linear )
   {
   }

   std::string name;
   AnimatorType animatorType;
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
  LayoutPositionData( Handle handle, float left, float top, float right, float bottom, bool animated ) :
      handle( handle ), left( left ), top( top ), right( right ), bottom( bottom ), animated( animated )
  {
  };

  BaseHandle handle;
  float left;
  float top;
  float right;
  float bottom;
  bool animated;
};

using LayoutPositionDataArray = std::vector< LayoutPositionData >;

struct LayoutDataElement
{
  LayoutDataElement()
  : propertyIndex( Property::INVALID_KEY ), animatorIndex( -1 ), positionDataIndex(-1 )
  {
  };

  LayoutDataElement( Actor actor, Property::Index propertyIndex, Property::Value value )
  : handle( actor ),
    propertyIndex( propertyIndex ),
    targetValue( value ),
    animatorIndex( -1 ),
    positionDataIndex( -1 )
  {
  };

  BaseHandle handle;
  Property::Index propertyIndex;
  Property::Value initialValue;
  Property::Value targetValue;
  int animatorIndex;
  int positionDataIndex;
};

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

    BaseHandle handle;

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

  void AddPropertyAnimator( Actor actor, Property::Map map );
  void AddPropertyAnimator( Actor actor, Property::Map map, KeyFrames keyFrames, Animation::Interpolation interpolation );
  void AddPropertyAnimator( Actor actor, Property::Map map, Path path, Vector3 forward );

  void ConvertToLayoutDataElements( Actor, LayoutData& layoutData );

  /**
   * @copydoc Dali::Animation::FinishedSignal()
   */
  Dali::Toolkit::LayoutTransitionData::LayoutTransitionSignalType& FinishedSignal();

  void EmitSignalFinish( int layoutTransitionType );

private:
  bool ConvertToLayoutAnimator( const Property::Map& animatorData, const PropertyAnimator& propertyAnimator, LayoutDataAnimator& layoutAnimator );
  bool ConvertToLayoutDataElement( const PropertyAnimator& propertyAnimator, LayoutDataElement& layoutDataElement, LayoutData& layoutData );

  PropertyAnimatorArray mPropertyAnimators;

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
  LayoutData( LayoutTransition& layoutTransition, LayoutPositionDataArray& layoutPositionDataArray, LayoutDataArray& layoutDataArray,
      LayoutAnimatorArray& layoutAnimatorArray, PropertyAnimatorArray& childrenPropertyAnimators )
  : speculativeLayout( false ),
    layoutTransition( layoutTransition ),
    layoutPositionDataArray( layoutPositionDataArray ),
    layoutDataArray( layoutDataArray),
    layoutAnimatorArray( layoutAnimatorArray ),
    childrenPropertyAnimators( childrenPropertyAnimators )
  {
  };

  bool speculativeLayout;
  LayoutTransition& layoutTransition;
  LayoutPositionDataArray& layoutPositionDataArray;
  LayoutDataArray& layoutDataArray;
  LayoutAnimatorArray& layoutAnimatorArray;
  PropertyAnimatorArray& childrenPropertyAnimators;
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
