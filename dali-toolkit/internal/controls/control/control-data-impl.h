#ifndef DALI_TOOLKIT_CONTROL_DATA_IMPL_H
#define DALI_TOOLKIT_CONTROL_DATA_IMPL_H

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
#include <dali/public-api/object/type-registry.h>
#include <dali-toolkit/devel-api/controls/control-devel.h>
#include <string>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/visuals/visual-resource-observer.h>
#include <dali-toolkit/public-api/controls/control-impl.h>
#include <dali/devel-api/common/owner-container.h>
#include <dali-toolkit/devel-api/visual-factory/visual-base.h>
#include <dali-toolkit/internal/controls/tooltip/tooltip.h>
#include <dali-toolkit/internal/builder/style.h>
#include <dali-toolkit/internal/builder/dictionary.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

/**
  * Struct used to store Visual within the control, index is a unique key for each visual.
  */
 struct RegisteredVisual
 {
   Property::Index index;
   Toolkit::Visual::Base visual;
   bool enabled;

   RegisteredVisual( Property::Index aIndex, Toolkit::Visual::Base &aVisual, bool aEnabled)
   : index(aIndex), visual(aVisual), enabled(aEnabled)
   {
   }
 };

typedef Dali::OwnerContainer< RegisteredVisual* > RegisteredVisualContainer;


/**
 * Holds the Implementation for the internal control class
 */
class Control::Impl : public ConnectionTracker, public Visual::ResourceObserver
{

public:

  static Control::Impl& Get( Internal::Control& internalControl );

  static const Control::Impl& Get( const Internal::Control& internalControl );


  Impl( Control& controlImpl );

  ~Impl();

  void PinchDetected(Actor actor, const PinchGesture& pinch);

  void PanDetected(Actor actor, const PanGesture& pan);

  void TapDetected(Actor actor, const TapGesture& tap);

  void LongPressDetected(Actor actor, const LongPressGesture& longPress);

  void ResourceReady( Visual::Base& object);

  void StopObservingVisual( Toolkit::Visual::Base& visual );

  void StartObservingVisual( Toolkit::Visual::Base& visual);

  static void SetProperty( BaseObject* object, Property::Index index, const Property::Value& value );

  static Property::Value GetProperty( BaseObject* object, Property::Index index );

  void SetState( DevelControl::State newState, bool withTransitions=true );

  void SetSubState( const std::string& subStateName, bool withTransitions=true );

  void ReplaceStateVisualsAndProperties( const StylePtr oldState, const StylePtr newState, const std::string& subState );

  void RemoveVisual( RegisteredVisualContainer& visuals, const std::string& visualName );

  void RemoveVisuals( RegisteredVisualContainer& visuals, DictionaryKeys& removeVisuals );

  void  CopyInstancedProperties( RegisteredVisualContainer& visuals, Dictionary<Property::Map>& instancedProperties );

  void RecreateChangedVisuals( Dictionary<Property::Map>& stateVisualsToChange, Dictionary<Property::Map>& instancedProperties );

  Toolkit::Visual::Base GetVisualByName( RegisteredVisualContainer& visuals, const std::string& visualName );

  bool IsResourceReady() const;

  Control& mControlImpl;
  DevelControl::State mState;
  std::string mSubStateName;

  int mLeftFocusableActorId;       ///< Actor ID of Left focusable control.
  int mRightFocusableActorId;      ///< Actor ID of Right focusable control.
  int mUpFocusableActorId;         ///< Actor ID of Up focusable control.
  int mDownFocusableActorId;       ///< Actor ID of Down focusable control.


  RegisteredVisualContainer mVisuals;     ///< Stores visuals needed by the control, non trivial type so std::vector used.
  std::string mStyleName;
  Vector4 mBackgroundColor;               ///< The color of the background visual
  Vector3* mStartingPinchScale;           ///< The scale when a pinch gesture starts, TODO: consider removing this
  Toolkit::Control::KeyEventSignalType mKeyEventSignal;
  Toolkit::Control::KeyInputFocusSignalType mKeyInputFocusGainedSignal;
  Toolkit::Control::KeyInputFocusSignalType mKeyInputFocusLostSignal;

  Toolkit::DevelControl::ResourceReadySignalType mResourceReadySignal;

  // Gesture Detection
  PinchGestureDetector mPinchGestureDetector;
  PanGestureDetector mPanGestureDetector;
  TapGestureDetector mTapGestureDetector;
  LongPressGestureDetector mLongPressGestureDetector;

  // Tooltip
  TooltipPtr mTooltip;

  ControlBehaviour mFlags : CONTROL_BEHAVIOUR_FLAG_COUNT;    ///< Flags passed in from constructor.
  bool mIsKeyboardNavigationSupported :1;  ///< Stores whether keyboard navigation is supported by the control.
  bool mIsKeyboardFocusGroup :1;           ///< Stores whether the control is a focus group.

  // Properties - these need to be members of Internal::Control::Impl as they access private methods/data of Internal::Control and Internal::Control::Impl.
  static const PropertyRegistration PROPERTY_1;
  static const PropertyRegistration PROPERTY_2;
  static const PropertyRegistration PROPERTY_3;
  static const PropertyRegistration PROPERTY_4;
  static const PropertyRegistration PROPERTY_5;
  static const PropertyRegistration PROPERTY_6;
  static const PropertyRegistration PROPERTY_7;
  static const PropertyRegistration PROPERTY_8;
  static const PropertyRegistration PROPERTY_9;
  static const PropertyRegistration PROPERTY_10;
  static const PropertyRegistration PROPERTY_11;
  static const PropertyRegistration PROPERTY_12;

};


} // namespace Internal

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_CONTROL_DATA_IMPL_H
