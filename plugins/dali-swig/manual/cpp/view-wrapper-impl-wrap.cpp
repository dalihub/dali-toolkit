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

// CLASS HEADER
#include "view-wrapper-impl-wrap.h"

// INTERNAL INCLUDES
#include <dali-toolkit/devel-api/visual-factory/visual-base.h>

#ifdef __cplusplus
extern "C" {
#endif

SwigDirector_ViewWrapperImpl::SwigDirector_ViewWrapperImpl(Dali::Toolkit::Internal::ControlWrapper::CustomControlBehaviour behaviourFlags) : Dali::Toolkit::Internal::ControlWrapper(behaviourFlags) {
  swig_init_callbacks();
}

SwigDirector_ViewWrapperImpl::~SwigDirector_ViewWrapperImpl() {

}


void SwigDirector_ViewWrapperImpl::OnStageConnection(int depth) {
    Dali::Toolkit::Internal::Control::OnStageConnection(depth);
    swig_callbackOnStageConnection(depth);
}

void SwigDirector_ViewWrapperImpl::OnStageDisconnection() {
    swig_callbackOnStageDisconnection();
    Dali::Toolkit::Internal::Control::OnStageDisconnection();
}

void SwigDirector_ViewWrapperImpl::OnChildAdd(Dali::Actor &child) {
    Dali::Toolkit::Internal::Control::OnChildAdd( child );
    void * jchild = (Dali::Actor *) &child;
    swig_callbackOnChildAdd(jchild);
}

void SwigDirector_ViewWrapperImpl::OnChildRemove(Dali::Actor &child) {
    void * jchild = (Dali::Actor *) &child;
    swig_callbackOnChildRemove(jchild);
    Dali::Toolkit::Internal::Control::OnChildRemove( child );
}

void SwigDirector_ViewWrapperImpl::OnPropertySet(Dali::Property::Index index, Dali::Property::Value propertyValue) {
  int jindex  ;
  void * jpropertyValue  ;

  if (!swig_callbackOnPropertySet) {
    Dali::Toolkit::Internal::ControlWrapper::OnPropertySet(index,propertyValue);
    return;
  } else {
    jindex = index;
    jpropertyValue = (void *)new Dali::Property::Value((const Dali::Property::Value &)propertyValue);
    swig_callbackOnPropertySet(jindex, jpropertyValue);
  }
}

void SwigDirector_ViewWrapperImpl::OnSizeSet(Dali::Vector3 const &targetSize) {
  Dali::Toolkit::Internal::Control::OnSizeSet(targetSize);
  swig_callbackOnSizeSet((Dali::Vector3 *) &targetSize);
}

void SwigDirector_ViewWrapperImpl::OnSizeAnimation(Dali::Animation &animation, Dali::Vector3 const &targetSize) {
  Dali::Toolkit::Internal::Control::OnSizeAnimation(animation,targetSize);
  swig_callbackOnSizeAnimation(&animation, (Dali::Vector3 *) &targetSize);
}

bool SwigDirector_ViewWrapperImpl::OnTouchEvent(Dali::TouchEvent const &event) {
  bool c_result = SwigValueInit< bool >() ;
  unsigned int jresult = 0 ;
  void * jarg0 = 0 ;

  if (!swig_callbackOnTouchEvent) {
    return Dali::Toolkit::Internal::Control::OnTouchEvent(event);
  } else {
    jarg0 = (Dali::TouchEvent *) &event;
    jresult = (unsigned int) swig_callbackOnTouchEvent(jarg0);
    c_result = jresult ? true : false;
  }
  return c_result;
}

bool SwigDirector_ViewWrapperImpl::OnHoverEvent(Dali::HoverEvent const &event) {
  bool c_result = SwigValueInit< bool >() ;
  unsigned int jresult = 0 ;
  void * jarg0 = 0 ;

  if (!swig_callbackOnHoverEvent) {
    return Dali::Toolkit::Internal::Control::OnHoverEvent(event);
  } else {
    jarg0 = (Dali::HoverEvent *) &event;
    jresult = (unsigned int) swig_callbackOnHoverEvent(jarg0);
    c_result = jresult ? true : false;
  }
  return c_result;
}

bool SwigDirector_ViewWrapperImpl::OnKeyEvent(Dali::KeyEvent const &event) {
  bool c_result = SwigValueInit< bool >() ;
  unsigned int jresult = 0 ;
  void * jarg0 = 0 ;

  if (!swig_callbackOnKeyEvent) {
    return Dali::Toolkit::Internal::Control::OnKeyEvent(event);
  } else {
    jarg0 = (Dali::KeyEvent *) &event;
    jresult = (unsigned int) swig_callbackOnKeyEvent(jarg0);
    c_result = jresult ? true : false;
  }
  return c_result;
}

bool SwigDirector_ViewWrapperImpl::OnWheelEvent(Dali::WheelEvent const &event) {
  bool c_result = SwigValueInit< bool >() ;
  unsigned int jresult = 0 ;
  void * jarg0 = 0 ;

  if (!swig_callbackOnWheelEvent) {
    return Dali::Toolkit::Internal::Control::OnWheelEvent(event);
  } else {
    jarg0 = (Dali::WheelEvent *) &event;
    jresult = (unsigned int) swig_callbackOnWheelEvent(jarg0);
    c_result = jresult ? true : false;
  }
  return c_result;
}

void SwigDirector_ViewWrapperImpl::OnRelayout(Dali::Vector2 const &size, Dali::RelayoutContainer &container) {
  void * jsize = 0 ;
  void * jcontainer = 0 ;

  if (!swig_callbackOnRelayout) {
    Dali::Toolkit::Internal::Control::OnRelayout(size,container);
    return;
  } else {
    jsize = (Dali::Vector2 *) &size;
    jcontainer = (Dali::RelayoutContainer *) &container;
    swig_callbackOnRelayout(jsize, jcontainer);
  }
}

void SwigDirector_ViewWrapperImpl::OnSetResizePolicy(Dali::ResizePolicy::Type policy, Dali::Dimension::Type dimension) {
  int jpolicy  ;
  int jdimension  ;

  if (!swig_callbackOnSetResizePolicy) {
    Dali::Toolkit::Internal::Control::OnSetResizePolicy(policy,dimension);
    return;
  } else {
    jpolicy = (int)policy;
    jdimension = (int)dimension;
    swig_callbackOnSetResizePolicy(jpolicy, jdimension);
  }
}

Dali::Vector3 SwigDirector_ViewWrapperImpl::GetNaturalSize() {
  Dali::Vector3 c_result ;
  void * jresult = 0 ;

  if (!swig_callbackGetNaturalSize) {
    return Dali::Toolkit::Internal::Control::GetNaturalSize();
  } else {
    jresult = (void *) swig_callbackGetNaturalSize();
    if (!jresult) {
      SWIG_CSharpSetPendingExceptionArgument(SWIG_CSharpArgumentNullException, "Unexpected null return for type Dali::Vector3", 0);
      return c_result;
    }
    c_result = *(Dali::Vector3 *)jresult;
  }
  return c_result;
}

float SwigDirector_ViewWrapperImpl::CalculateChildSize(Dali::Actor const &child, Dali::Dimension::Type dimension) {
  float c_result = SwigValueInit< float >() ;
  float jresult = 0 ;
  void * jchild = 0 ;
  int jdimension  ;

  if (!swig_callbackCalculateChildSize) {
    return Dali::Toolkit::Internal::Control::CalculateChildSize(child,dimension);
  } else {
    jchild = (Dali::Actor *) &child;
    jdimension = (int)dimension;
    jresult = (float) swig_callbackCalculateChildSize(jchild, jdimension);
    c_result = (float)jresult;
  }
  return c_result;
}

float SwigDirector_ViewWrapperImpl::GetHeightForWidth(float width) {
  float c_result = SwigValueInit< float >() ;
  float jresult = 0 ;
  float jwidth  ;

  if (!swig_callbackGetHeightForWidth) {
    return Dali::Toolkit::Internal::Control::GetHeightForWidth(width);
  } else {
    jwidth = width;
    jresult = (float) swig_callbackGetHeightForWidth(jwidth);
    c_result = (float)jresult;
  }
  return c_result;
}

float SwigDirector_ViewWrapperImpl::GetWidthForHeight(float height) {
  float c_result = SwigValueInit< float >() ;
  float jresult = 0 ;
  float jheight  ;

  if (!swig_callbackGetWidthForHeight) {
    return Dali::Toolkit::Internal::Control::GetWidthForHeight(height);
  } else {
    jheight = height;
    jresult = (float) swig_callbackGetWidthForHeight(jheight);
    c_result = (float)jresult;
  }
  return c_result;
}

bool SwigDirector_ViewWrapperImpl::RelayoutDependentOnChildren(Dali::Dimension::Type dimension) {
  bool c_result = SwigValueInit< bool >() ;
  unsigned int jresult = 0 ;
  int jdimension  ;

  if (!swig_callbackRelayoutDependentOnChildren__SWIG_0) {
    return Dali::Toolkit::Internal::Control::RelayoutDependentOnChildren(dimension);
  } else {
    jdimension = (int)dimension;
    jresult = (unsigned int) swig_callbackRelayoutDependentOnChildren__SWIG_0(jdimension);
    c_result = jresult ? true : false;
  }
  return c_result;
}

void SwigDirector_ViewWrapperImpl::OnCalculateRelayoutSize(Dali::Dimension::Type dimension) {
  int jdimension  ;

  if (!swig_callbackOnCalculateRelayoutSize) {
    Dali::Toolkit::Internal::Control::OnCalculateRelayoutSize(dimension);
    return;
  } else {
    jdimension = (int)dimension;
    swig_callbackOnCalculateRelayoutSize(jdimension);
  }
}

void SwigDirector_ViewWrapperImpl::OnLayoutNegotiated(float size, Dali::Dimension::Type dimension) {
  float jsize  ;
  int jdimension  ;

  if (!swig_callbackOnLayoutNegotiated) {
    Dali::Toolkit::Internal::Control::OnLayoutNegotiated(size,dimension);
    return;
  } else {
    jsize = size;
    jdimension = (int)dimension;
    swig_callbackOnLayoutNegotiated(jsize, jdimension);
  }
}

void SwigDirector_ViewWrapperImpl::OnInitialize() {
  if (!swig_callbackOnInitialize) {
    Dali::Toolkit::Internal::Control::OnInitialize();
    return;
  } else {
    swig_callbackOnInitialize();
  }
}

void SwigDirector_ViewWrapperImpl::OnControlChildAdd(Dali::Actor &child) {
  Dali::Toolkit::Internal::Control::OnControlChildAdd(child);
  swig_callbackOnControlChildAdd(&child);
}

void SwigDirector_ViewWrapperImpl::OnControlChildRemove(Dali::Actor &child) {
  swig_callbackOnControlChildRemove(&child);
  Dali::Toolkit::Internal::Control::OnControlChildRemove(child);
}

void SwigDirector_ViewWrapperImpl::OnStyleChange(Dali::Toolkit::StyleManager styleManager, Dali::StyleChange::Type change) {
  void * jstyleManager  ;
  int jchange  ;

  if (!swig_callbackOnStyleChange) {
    Dali::Toolkit::Internal::Control::OnStyleChange(styleManager,change);
    return;
  } else {
    jstyleManager = (void *)new Dali::Toolkit::StyleManager((const Dali::Toolkit::StyleManager &)styleManager);
    jchange = (int)change;
    swig_callbackOnStyleChange(jstyleManager, jchange);
  }
}

bool SwigDirector_ViewWrapperImpl::OnAccessibilityActivated() {
  bool c_result = SwigValueInit< bool >() ;
  unsigned int jresult = 0 ;

  if (!swig_callbackOnAccessibilityActivated) {
    return Dali::Toolkit::Internal::Control::OnAccessibilityActivated();
  } else {
    jresult = (unsigned int) swig_callbackOnAccessibilityActivated();
    c_result = jresult ? true : false;
  }
  return c_result;
}

bool SwigDirector_ViewWrapperImpl::OnAccessibilityPan(Dali::PanGesture gesture) {
  bool c_result = SwigValueInit< bool >() ;
  unsigned int jresult = 0 ;
  void * jgesture  ;

  if (!swig_callbackOnAccessibilityPan) {
    return Dali::Toolkit::Internal::Control::OnAccessibilityPan(gesture);
  } else {
    jgesture = (void *)new Dali::PanGesture((const Dali::PanGesture &)gesture);
    jresult = (unsigned int) swig_callbackOnAccessibilityPan(jgesture);
    c_result = jresult ? true : false;
  }
  return c_result;
}

bool SwigDirector_ViewWrapperImpl::OnAccessibilityTouch(Dali::TouchEvent const &touchEvent) {
  bool c_result = SwigValueInit< bool >() ;
  unsigned int jresult = 0 ;
  void * jtouchEvent = 0 ;

  if (!swig_callbackOnAccessibilityTouch) {
    return Dali::Toolkit::Internal::Control::OnAccessibilityTouch(touchEvent);
  } else {
    jtouchEvent = (Dali::TouchEvent *) &touchEvent;
    jresult = (unsigned int) swig_callbackOnAccessibilityTouch(jtouchEvent);
    c_result = jresult ? true : false;
  }
  return c_result;
}

bool SwigDirector_ViewWrapperImpl::OnAccessibilityValueChange(bool isIncrease) {
  bool c_result = SwigValueInit< bool >() ;
  unsigned int jresult = 0 ;
  unsigned int jisIncrease  ;

  if (!swig_callbackOnAccessibilityValueChange) {
    return Dali::Toolkit::Internal::Control::OnAccessibilityValueChange(isIncrease);
  } else {
    jisIncrease = isIncrease;
    jresult = (unsigned int) swig_callbackOnAccessibilityValueChange(jisIncrease);
    c_result = jresult ? true : false;
  }
  return c_result;
}

bool SwigDirector_ViewWrapperImpl::OnAccessibilityZoom() {
  bool c_result = SwigValueInit< bool >() ;
  unsigned int jresult = 0 ;

  if (!swig_callbackOnAccessibilityZoom) {
    return Dali::Toolkit::Internal::Control::OnAccessibilityZoom();
  } else {
    jresult = (unsigned int) swig_callbackOnAccessibilityZoom();
    c_result = jresult ? true : false;
  }
  return c_result;
}

void SwigDirector_ViewWrapperImpl::OnKeyInputFocusGained() {
  if (!swig_callbackOnKeyInputFocusGained) {
    Dali::Toolkit::Internal::Control::OnKeyInputFocusGained();
    return;
  } else {
    swig_callbackOnKeyInputFocusGained();
  }
}

void SwigDirector_ViewWrapperImpl::OnKeyInputFocusLost() {
  if (!swig_callbackOnKeyInputFocusLost) {
    Dali::Toolkit::Internal::Control::OnKeyInputFocusLost();
    return;
  } else {
    swig_callbackOnKeyInputFocusLost();
  }
}

Dali::Actor SwigDirector_ViewWrapperImpl::GetNextKeyboardFocusableActor(Dali::Actor currentFocusedActor, Dali::Toolkit::Control::KeyboardFocus::Direction direction, bool loopEnabled) {
  Dali::Actor c_result ;
  void * jresult = 0 ;
  void * jcurrentFocusedActor  ;
  int jdirection  ;
  unsigned int jloopEnabled  ;

  if (!swig_callbackGetNextKeyboardFocusableActor) {
    return Dali::Toolkit::Internal::Control::GetNextKeyboardFocusableActor(currentFocusedActor,direction,loopEnabled);
  } else {
    jcurrentFocusedActor = (void *)new Dali::Actor((const Dali::Actor &)currentFocusedActor);
    jdirection = (int)direction;
    jloopEnabled = loopEnabled;
    jresult = (void *) swig_callbackGetNextKeyboardFocusableActor(jcurrentFocusedActor, jdirection, jloopEnabled);
    if (!jresult) {
      SWIG_CSharpSetPendingExceptionArgument(SWIG_CSharpArgumentNullException, "Unexpected null return for type Dali::Actor", 0);
      return c_result;
    }
    c_result = *(Dali::Actor *)jresult;
  }
  return c_result;
}

void SwigDirector_ViewWrapperImpl::OnKeyboardFocusChangeCommitted(Dali::Actor commitedFocusableActor) {
  void * jcommitedFocusableActor  ;

  if (!swig_callbackOnKeyboardFocusChangeCommitted) {
    Dali::Toolkit::Internal::Control::OnKeyboardFocusChangeCommitted(commitedFocusableActor);
    return;
  } else {
    jcommitedFocusableActor = (void *)new Dali::Actor((const Dali::Actor &)commitedFocusableActor);
    swig_callbackOnKeyboardFocusChangeCommitted(jcommitedFocusableActor);
  }
}

bool SwigDirector_ViewWrapperImpl::OnKeyboardEnter() {
  bool c_result = SwigValueInit< bool >() ;
  unsigned int jresult = 0 ;

  if (!swig_callbackOnKeyboardEnter) {
    return Dali::Toolkit::Internal::Control::OnKeyboardEnter();
  } else {
    jresult = (unsigned int) swig_callbackOnKeyboardEnter();
    c_result = jresult ? true : false;
  }
  return c_result;
}

void SwigDirector_ViewWrapperImpl::OnPinch(Dali::PinchGesture const &pinch) {
  void * jpinch = 0 ;

  if (!swig_callbackOnPinch) {
    Dali::Toolkit::Internal::Control::OnPinch(pinch);
    return;
  } else {
    jpinch = (Dali::PinchGesture *) &pinch;
    swig_callbackOnPinch(jpinch);
  }
}

void SwigDirector_ViewWrapperImpl::OnPan(Dali::PanGesture const &pan) {
  void * jpan = 0 ;

  if (!swig_callbackOnPan) {
    Dali::Toolkit::Internal::Control::OnPan(pan);
    return;
  } else {
    jpan = (Dali::PanGesture *) &pan;
    swig_callbackOnPan(jpan);
  }
}

void SwigDirector_ViewWrapperImpl::OnTap(Dali::TapGesture const &tap) {
  void * jtap = 0 ;

  if (!swig_callbackOnTap) {
    Dali::Toolkit::Internal::Control::OnTap(tap);
    return;
  } else {
    jtap = (Dali::TapGesture *) &tap;
    swig_callbackOnTap(jtap);
  }
}

void SwigDirector_ViewWrapperImpl::OnLongPress(Dali::LongPressGesture const &longPress) {
  void * jlongPress = 0 ;

  if (!swig_callbackOnLongPress) {
    Dali::Toolkit::Internal::Control::OnLongPress(longPress);
    return;
  } else {
    jlongPress = (Dali::LongPressGesture *) &longPress;
    swig_callbackOnLongPress(jlongPress);
  }
}

void SwigDirector_ViewWrapperImpl::SignalConnected(Dali::SlotObserver *slotObserver, Dali::CallbackBase *callback) {
  void * jslotObserver = 0 ;
  void * jcallback = 0 ;

  if (!swig_callbackSignalConnected) {
    Dali::Toolkit::Internal::Control::SignalConnected(slotObserver,callback);
    return;
  } else {
    jslotObserver = (void *) slotObserver;
    jcallback = (void *) callback;
    swig_callbackSignalConnected(jslotObserver, jcallback);
  }
}

void SwigDirector_ViewWrapperImpl::SignalDisconnected(Dali::SlotObserver *slotObserver, Dali::CallbackBase *callback) {
  void * jslotObserver = 0 ;
  void * jcallback = 0 ;

  if (!swig_callbackSignalDisconnected) {
    Dali::Toolkit::Internal::Control::SignalDisconnected(slotObserver,callback);
    return;
  } else {
    jslotObserver = (void *) slotObserver;
    jcallback = (void *) callback;
    swig_callbackSignalDisconnected(jslotObserver, jcallback);
  }
}

Dali::Toolkit::Internal::Control::Extension *SwigDirector_ViewWrapperImpl::GetControlExtension() {
  return Dali::Toolkit::Internal::Control::GetControlExtension();
}

void SwigDirector_ViewWrapperImpl::swig_connect_director(SWIG_Callback0_t callbackOnStageConnection, SWIG_Callback1_t callbackOnStageDisconnection, SWIG_Callback2_t callbackOnChildAdd, SWIG_Callback3_t callbackOnChildRemove, SWIG_Callback4_t callbackOnPropertySet, SWIG_Callback5_t callbackOnSizeSet, SWIG_Callback6_t callbackOnSizeAnimation, SWIG_Callback7_t callbackOnTouchEvent, SWIG_Callback8_t callbackOnHoverEvent, SWIG_Callback9_t callbackOnKeyEvent, SWIG_Callback10_t callbackOnWheelEvent, SWIG_Callback11_t callbackOnRelayout, SWIG_Callback12_t callbackOnSetResizePolicy, SWIG_Callback13_t callbackGetNaturalSize, SWIG_Callback14_t callbackCalculateChildSize, SWIG_Callback15_t callbackGetHeightForWidth, SWIG_Callback16_t callbackGetWidthForHeight, SWIG_Callback17_t callbackRelayoutDependentOnChildren__SWIG_0, SWIG_Callback18_t callbackRelayoutDependentOnChildren__SWIG_1, SWIG_Callback19_t callbackOnCalculateRelayoutSize, SWIG_Callback20_t callbackOnLayoutNegotiated, SWIG_Callback21_t callbackOnInitialize, SWIG_Callback22_t callbackOnControlChildAdd, SWIG_Callback23_t callbackOnControlChildRemove, SWIG_Callback24_t callbackOnStyleChange, SWIG_Callback25_t callbackOnAccessibilityActivated, SWIG_Callback26_t callbackOnAccessibilityPan, SWIG_Callback27_t callbackOnAccessibilityTouch, SWIG_Callback28_t callbackOnAccessibilityValueChange, SWIG_Callback29_t callbackOnAccessibilityZoom, SWIG_Callback30_t callbackOnKeyInputFocusGained, SWIG_Callback31_t callbackOnKeyInputFocusLost, SWIG_Callback32_t callbackGetNextKeyboardFocusableActor, SWIG_Callback33_t callbackOnKeyboardFocusChangeCommitted, SWIG_Callback34_t callbackOnKeyboardEnter, SWIG_Callback35_t callbackOnPinch, SWIG_Callback36_t callbackOnPan, SWIG_Callback37_t callbackOnTap, SWIG_Callback38_t callbackOnLongPress, SWIG_Callback39_t callbackSignalConnected, SWIG_Callback40_t callbackSignalDisconnected) {
  swig_callbackOnStageConnection = callbackOnStageConnection;
  swig_callbackOnStageDisconnection = callbackOnStageDisconnection;
  swig_callbackOnChildAdd = callbackOnChildAdd;
  swig_callbackOnChildRemove = callbackOnChildRemove;
  swig_callbackOnPropertySet = callbackOnPropertySet;
  swig_callbackOnSizeSet = callbackOnSizeSet;
  swig_callbackOnSizeAnimation = callbackOnSizeAnimation;
  swig_callbackOnTouchEvent = callbackOnTouchEvent;
  swig_callbackOnHoverEvent = callbackOnHoverEvent;
  swig_callbackOnKeyEvent = callbackOnKeyEvent;
  swig_callbackOnWheelEvent = callbackOnWheelEvent;
  swig_callbackOnRelayout = callbackOnRelayout;
  swig_callbackOnSetResizePolicy = callbackOnSetResizePolicy;
  swig_callbackGetNaturalSize = callbackGetNaturalSize;
  swig_callbackCalculateChildSize = callbackCalculateChildSize;
  swig_callbackGetHeightForWidth = callbackGetHeightForWidth;
  swig_callbackGetWidthForHeight = callbackGetWidthForHeight;
  swig_callbackRelayoutDependentOnChildren__SWIG_0 = callbackRelayoutDependentOnChildren__SWIG_0;
  swig_callbackRelayoutDependentOnChildren__SWIG_1 = callbackRelayoutDependentOnChildren__SWIG_1;
  swig_callbackOnCalculateRelayoutSize = callbackOnCalculateRelayoutSize;
  swig_callbackOnLayoutNegotiated = callbackOnLayoutNegotiated;
  swig_callbackOnInitialize = callbackOnInitialize;
  swig_callbackOnControlChildAdd = callbackOnControlChildAdd;
  swig_callbackOnControlChildRemove = callbackOnControlChildRemove;
  swig_callbackOnStyleChange = callbackOnStyleChange;
  swig_callbackOnAccessibilityActivated = callbackOnAccessibilityActivated;
  swig_callbackOnAccessibilityPan = callbackOnAccessibilityPan;
  swig_callbackOnAccessibilityTouch = callbackOnAccessibilityTouch;
  swig_callbackOnAccessibilityValueChange = callbackOnAccessibilityValueChange;
  swig_callbackOnAccessibilityZoom = callbackOnAccessibilityZoom;
  swig_callbackOnKeyInputFocusGained = callbackOnKeyInputFocusGained;
  swig_callbackOnKeyInputFocusLost = callbackOnKeyInputFocusLost;
  swig_callbackGetNextKeyboardFocusableActor = callbackGetNextKeyboardFocusableActor;
  swig_callbackOnKeyboardFocusChangeCommitted = callbackOnKeyboardFocusChangeCommitted;
  swig_callbackOnKeyboardEnter = callbackOnKeyboardEnter;
  swig_callbackOnPinch = callbackOnPinch;
  swig_callbackOnPan = callbackOnPan;
  swig_callbackOnTap = callbackOnTap;
  swig_callbackOnLongPress = callbackOnLongPress;
  swig_callbackSignalConnected = callbackSignalConnected;
  swig_callbackSignalDisconnected = callbackSignalDisconnected;
}

void SwigDirector_ViewWrapperImpl::swig_init_callbacks() {
  swig_callbackOnStageConnection = 0;
  swig_callbackOnStageDisconnection = 0;
  swig_callbackOnChildAdd = 0;
  swig_callbackOnChildRemove = 0;
  swig_callbackOnPropertySet = 0;
  swig_callbackOnSizeSet = 0;
  swig_callbackOnSizeAnimation = 0;
  swig_callbackOnTouchEvent = 0;
  swig_callbackOnHoverEvent = 0;
  swig_callbackOnKeyEvent = 0;
  swig_callbackOnWheelEvent = 0;
  swig_callbackOnRelayout = 0;
  swig_callbackOnSetResizePolicy = 0;
  swig_callbackGetNaturalSize = 0;
  swig_callbackCalculateChildSize = 0;
  swig_callbackGetHeightForWidth = 0;
  swig_callbackGetWidthForHeight = 0;
  swig_callbackRelayoutDependentOnChildren__SWIG_0 = 0;
  swig_callbackRelayoutDependentOnChildren__SWIG_1 = 0;
  swig_callbackOnCalculateRelayoutSize = 0;
  swig_callbackOnLayoutNegotiated = 0;
  swig_callbackOnInitialize = 0;
  swig_callbackOnControlChildAdd = 0;
  swig_callbackOnControlChildRemove = 0;
  swig_callbackOnStyleChange = 0;
  swig_callbackOnAccessibilityActivated = 0;
  swig_callbackOnAccessibilityPan = 0;
  swig_callbackOnAccessibilityTouch = 0;
  swig_callbackOnAccessibilityValueChange = 0;
  swig_callbackOnAccessibilityZoom = 0;
  swig_callbackOnKeyInputFocusGained = 0;
  swig_callbackOnKeyInputFocusLost = 0;
  swig_callbackGetNextKeyboardFocusableActor = 0;
  swig_callbackOnKeyboardFocusChangeCommitted = 0;
  swig_callbackOnKeyboardEnter = 0;
  swig_callbackOnPinch = 0;
  swig_callbackOnPan = 0;
  swig_callbackOnTap = 0;
  swig_callbackOnLongPress = 0;
  swig_callbackSignalConnected = 0;
  swig_callbackSignalDisconnected = 0;
}

SWIGEXPORT int SWIGSTDCALL CSharp_Dali_ViewWrapperImpl_CONTROL_BEHAVIOUR_FLAG_COUNT_get() {
  int jresult ;
  int result;

  result = (int)Dali::Toolkit::Internal::ControlWrapper::CONTROL_BEHAVIOUR_FLAG_COUNT;
  jresult = result;
  return jresult;
}


SWIGEXPORT void * SWIGSTDCALL CSharp_Dali_new_ViewWrapperImpl(int jarg1) {
  void * jresult ;
  Dali::Toolkit::Internal::ControlWrapper::CustomControlBehaviour arg1 ;
  Dali::Toolkit::Internal::ControlWrapper *result = 0 ;

  arg1 = (Dali::Toolkit::Internal::ControlWrapper::CustomControlBehaviour)jarg1;
  {
    try {
      result = (Dali::Toolkit::Internal::ControlWrapper *)new SwigDirector_ViewWrapperImpl(arg1);
    } catch (std::out_of_range& e) {
      {
        SWIG_CSharpException(SWIG_IndexError, const_cast<char*>(e.what())); return 0;
      };
    } catch (std::exception& e) {
      {
        SWIG_CSharpException(SWIG_RuntimeError, const_cast<char*>(e.what())); return 0;
      };
    } catch (...) {
      {
        SWIG_CSharpException(SWIG_UnknownError, "unknown error"); return 0;
      };
    }
  }
  jresult = (void *)result;
  return jresult;
}


SWIGEXPORT void * SWIGSTDCALL CSharp_Dali_ViewWrapperImpl_New(char * jarg1, void * jarg2) {
  void * jresult ;
  std::string arg1 ;
  Dali::Toolkit::Internal::ControlWrapper *arg2 = (Dali::Toolkit::Internal::ControlWrapper *) 0 ;
  Dali::Toolkit::ControlWrapper result;

  if (!jarg1) {
    SWIG_CSharpSetPendingExceptionArgument(SWIG_CSharpArgumentNullException, "null string", 0);
    return 0;
  }
  (&arg1)->assign(jarg1);
  arg2 = (Dali::Toolkit::Internal::ControlWrapper *)jarg2;
  {
    try {
      result = Dali::Toolkit::Internal::ControlWrapper::New(arg1,arg2);
    } catch (std::out_of_range& e) {
      {
        SWIG_CSharpException(SWIG_IndexError, const_cast<char*>(e.what())); return 0;
      };
    } catch (std::exception& e) {
      {
        SWIG_CSharpException(SWIG_RuntimeError, const_cast<char*>(e.what())); return 0;
      };
    } catch (...) {
      {
        SWIG_CSharpException(SWIG_UnknownError, "unknown error"); return 0;
      };
    }
  }
  jresult = new Dali::Toolkit::ControlWrapper((const Dali::Toolkit::ControlWrapper &)result);
  return jresult;
}


SWIGEXPORT void SWIGSTDCALL CSharp_Dali_delete_ViewWrapperImpl(void * jarg1) {
  Dali::Toolkit::Internal::ControlWrapper *arg1 = (Dali::Toolkit::Internal::ControlWrapper *) 0 ;

  arg1 = (Dali::Toolkit::Internal::ControlWrapper *)jarg1;
  {
    try {
      if (arg1)
      {
        arg1->Unreference();
      }
    } catch (std::out_of_range& e) {
      {
        SWIG_CSharpException(SWIG_IndexError, const_cast<char*>(e.what())); return ;
      };
    } catch (std::exception& e) {
      {
        SWIG_CSharpException(SWIG_RuntimeError, const_cast<char*>(e.what())); return ;
      };
    } catch (...) {
      {
        SWIG_CSharpException(SWIG_UnknownError, "unknown error"); return ;
      };
    }
  }
}


SWIGEXPORT void SWIGSTDCALL CSharp_Dali_ViewWrapperImpl_director_connect(void *objarg, SwigDirector_ViewWrapperImpl::SWIG_Callback0_t callback0, SwigDirector_ViewWrapperImpl::SWIG_Callback1_t callback1, SwigDirector_ViewWrapperImpl::SWIG_Callback2_t callback2, SwigDirector_ViewWrapperImpl::SWIG_Callback3_t callback3, SwigDirector_ViewWrapperImpl::SWIG_Callback4_t callback4, SwigDirector_ViewWrapperImpl::SWIG_Callback5_t callback5, SwigDirector_ViewWrapperImpl::SWIG_Callback6_t callback6, SwigDirector_ViewWrapperImpl::SWIG_Callback7_t callback7, SwigDirector_ViewWrapperImpl::SWIG_Callback8_t callback8, SwigDirector_ViewWrapperImpl::SWIG_Callback9_t callback9, SwigDirector_ViewWrapperImpl::SWIG_Callback10_t callback10, SwigDirector_ViewWrapperImpl::SWIG_Callback11_t callback11, SwigDirector_ViewWrapperImpl::SWIG_Callback12_t callback12, SwigDirector_ViewWrapperImpl::SWIG_Callback13_t callback13, SwigDirector_ViewWrapperImpl::SWIG_Callback14_t callback14, SwigDirector_ViewWrapperImpl::SWIG_Callback15_t callback15, SwigDirector_ViewWrapperImpl::SWIG_Callback16_t callback16, SwigDirector_ViewWrapperImpl::SWIG_Callback17_t callback17, SwigDirector_ViewWrapperImpl::SWIG_Callback18_t callback18, SwigDirector_ViewWrapperImpl::SWIG_Callback19_t callback19, SwigDirector_ViewWrapperImpl::SWIG_Callback20_t callback20, SwigDirector_ViewWrapperImpl::SWIG_Callback21_t callback21, SwigDirector_ViewWrapperImpl::SWIG_Callback22_t callback22, SwigDirector_ViewWrapperImpl::SWIG_Callback23_t callback23, SwigDirector_ViewWrapperImpl::SWIG_Callback24_t callback24, SwigDirector_ViewWrapperImpl::SWIG_Callback25_t callback25, SwigDirector_ViewWrapperImpl::SWIG_Callback26_t callback26, SwigDirector_ViewWrapperImpl::SWIG_Callback27_t callback27, SwigDirector_ViewWrapperImpl::SWIG_Callback28_t callback28, SwigDirector_ViewWrapperImpl::SWIG_Callback29_t callback29, SwigDirector_ViewWrapperImpl::SWIG_Callback30_t callback30, SwigDirector_ViewWrapperImpl::SWIG_Callback31_t callback31, SwigDirector_ViewWrapperImpl::SWIG_Callback32_t callback32, SwigDirector_ViewWrapperImpl::SWIG_Callback33_t callback33, SwigDirector_ViewWrapperImpl::SWIG_Callback34_t callback34, SwigDirector_ViewWrapperImpl::SWIG_Callback35_t callback35, SwigDirector_ViewWrapperImpl::SWIG_Callback36_t callback36, SwigDirector_ViewWrapperImpl::SWIG_Callback37_t callback37, SwigDirector_ViewWrapperImpl::SWIG_Callback38_t callback38, SwigDirector_ViewWrapperImpl::SWIG_Callback39_t callback39, SwigDirector_ViewWrapperImpl::SWIG_Callback40_t callback40) {
  Dali::Toolkit::Internal::ControlWrapper *obj = (Dali::Toolkit::Internal::ControlWrapper *)objarg;
  SwigDirector_ViewWrapperImpl *director = dynamic_cast<SwigDirector_ViewWrapperImpl *>(obj);
  if (director) {
    director->swig_connect_director(callback0, callback1, callback2, callback3, callback4, callback5, callback6, callback7, callback8, callback9, callback10, callback11, callback12, callback13, callback14, callback15, callback16, callback17, callback18, callback19, callback20, callback21, callback22, callback23, callback24, callback25, callback26, callback27, callback28, callback29, callback30, callback31, callback32, callback33, callback34, callback35, callback36, callback37, callback38, callback39, callback40);
  }
}


SWIGEXPORT void * SWIGSTDCALL CSharp_Dali_GetControlWrapperImpl__SWIG_0(void * jarg1) {
  void * jresult ;
  Dali::Toolkit::ControlWrapper *arg1 = 0 ;
  Dali::Toolkit::Internal::ControlWrapper *result = 0 ;

  arg1 = (Dali::Toolkit::ControlWrapper *)jarg1;
  if (!arg1) {
    SWIG_CSharpSetPendingExceptionArgument(SWIG_CSharpArgumentNullException, "Dali::Toolkit::ControlWrapper & type is null", 0);
    return 0;
  }
  {
    try {
      result = (Dali::Toolkit::Internal::ControlWrapper *) &Dali::Toolkit::GetControlWrapperImpl(*arg1);
    } catch (std::out_of_range& e) {
      {
        SWIG_CSharpException(SWIG_IndexError, const_cast<char*>(e.what())); return 0;
      };
    } catch (std::exception& e) {
      {
        SWIG_CSharpException(SWIG_RuntimeError, const_cast<char*>(e.what())); return 0;
      };
    } catch (...) {
      {
        SWIG_CSharpException(SWIG_UnknownError, "unknown error"); return 0;
      };
    }
  }
  jresult = (void *)result;
  return jresult;
}


SWIGEXPORT void * SWIGSTDCALL CSharp_Dali_ViewWrapper_New(char * jarg1, void * jarg2) {
  void * jresult ;
  std::string arg1 ;
  Dali::Toolkit::Internal::ControlWrapper *arg2 = 0 ;
  Dali::Toolkit::ControlWrapper result;

  if (!jarg1) {
    SWIG_CSharpSetPendingExceptionArgument(SWIG_CSharpArgumentNullException, "null string", 0);
    return 0;
  }
  (&arg1)->assign(jarg1);
  arg2 = (Dali::Toolkit::Internal::ControlWrapper *)jarg2;
  if (!arg2) {
    SWIG_CSharpSetPendingExceptionArgument(SWIG_CSharpArgumentNullException, "Dali::Toolkit::Internal::ControlWrapper & type is null", 0);
    return 0;
  }
  {
    try {
      result = Dali::Toolkit::ControlWrapper::New(arg1,*arg2);
    } catch (std::out_of_range& e) {
      {
        SWIG_CSharpException(SWIG_IndexError, const_cast<char*>(e.what())); return 0;
      };
    } catch (std::exception& e) {
      {
        SWIG_CSharpException(SWIG_RuntimeError, const_cast<char*>(e.what())); return 0;
      };
    } catch (...) {
      {
        SWIG_CSharpException(SWIG_UnknownError, "unknown error"); return 0;
      };
    }
  }
  jresult = new Dali::Toolkit::ControlWrapper((const Dali::Toolkit::ControlWrapper &)result);
  return jresult;
}


SWIGEXPORT void * SWIGSTDCALL CSharp_Dali_new_ViewWrapper__SWIG_0() {
  void * jresult ;
  Dali::Toolkit::ControlWrapper *result = 0 ;

  {
    try {
      result = (Dali::Toolkit::ControlWrapper *)new Dali::Toolkit::ControlWrapper();
    } catch (std::out_of_range& e) {
      {
        SWIG_CSharpException(SWIG_IndexError, const_cast<char*>(e.what())); return 0;
      };
    } catch (std::exception& e) {
      {
        SWIG_CSharpException(SWIG_RuntimeError, const_cast<char*>(e.what())); return 0;
      };
    } catch (...) {
      {
        SWIG_CSharpException(SWIG_UnknownError, "unknown error"); return 0;
      };
    }
  }
  jresult = (void *)result;
  return jresult;
}


SWIGEXPORT void SWIGSTDCALL CSharp_Dali_delete_ViewWrapper(void * jarg1) {
  Dali::Toolkit::ControlWrapper *arg1 = (Dali::Toolkit::ControlWrapper *) 0 ;

  arg1 = (Dali::Toolkit::ControlWrapper *)jarg1;
  {
    try {
      delete arg1;
    } catch (std::out_of_range& e) {
      {
        SWIG_CSharpException(SWIG_IndexError, const_cast<char*>(e.what())); return ;
      };
    } catch (std::exception& e) {
      {
        SWIG_CSharpException(SWIG_RuntimeError, const_cast<char*>(e.what())); return ;
      };
    } catch (...) {
      {
        SWIG_CSharpException(SWIG_UnknownError, "unknown error"); return ;
      };
    }
  }
}


SWIGEXPORT void * SWIGSTDCALL CSharp_Dali_new_ViewWrapper__SWIG_1(void * jarg1) {
  void * jresult ;
  Dali::Toolkit::ControlWrapper *arg1 = 0 ;
  Dali::Toolkit::ControlWrapper *result = 0 ;

  arg1 = (Dali::Toolkit::ControlWrapper *)jarg1;
  if (!arg1) {
    SWIG_CSharpSetPendingExceptionArgument(SWIG_CSharpArgumentNullException, "Dali::Toolkit::ControlWrapper const & type is null", 0);
    return 0;
  }
  {
    try {
      result = (Dali::Toolkit::ControlWrapper *)new Dali::Toolkit::ControlWrapper((Dali::Toolkit::ControlWrapper const &)*arg1);
    } catch (std::out_of_range& e) {
      {
        SWIG_CSharpException(SWIG_IndexError, const_cast<char*>(e.what())); return 0;
      };
    } catch (std::exception& e) {
      {
        SWIG_CSharpException(SWIG_RuntimeError, const_cast<char*>(e.what())); return 0;
      };
    } catch (...) {
      {
        SWIG_CSharpException(SWIG_UnknownError, "unknown error"); return 0;
      };
    }
  }
  jresult = (void *)result;
  return jresult;
}


SWIGEXPORT void * SWIGSTDCALL CSharp_Dali_ViewWrapper_Assign(void * jarg1, void * jarg2) {
  void * jresult ;
  Dali::Toolkit::ControlWrapper *arg1 = (Dali::Toolkit::ControlWrapper *) 0 ;
  Dali::Toolkit::ControlWrapper *arg2 = 0 ;
  Dali::Toolkit::ControlWrapper *result = 0 ;

  arg1 = (Dali::Toolkit::ControlWrapper *)jarg1;
  arg2 = (Dali::Toolkit::ControlWrapper *)jarg2;
  if (!arg2) {
    SWIG_CSharpSetPendingExceptionArgument(SWIG_CSharpArgumentNullException, "Dali::Toolkit::ControlWrapper const & type is null", 0);
    return 0;
  }
  {
    try {
      result = (Dali::Toolkit::ControlWrapper *) &(arg1)->operator =((Dali::Toolkit::ControlWrapper const &)*arg2);
    } catch (std::out_of_range& e) {
      {
        SWIG_CSharpException(SWIG_IndexError, const_cast<char*>(e.what())); return 0;
      };
    } catch (std::exception& e) {
      {
        SWIG_CSharpException(SWIG_RuntimeError, const_cast<char*>(e.what())); return 0;
      };
    } catch (...) {
      {
        SWIG_CSharpException(SWIG_UnknownError, "unknown error"); return 0;
      };
    }
  }
  jresult = (void *)result;
  return jresult;
}


SWIGEXPORT void * SWIGSTDCALL CSharp_Dali_ViewWrapper_DownCast(void * jarg1) {
  void * jresult ;
  Dali::BaseHandle arg1 ;
  Dali::BaseHandle *argp1 ;
  Dali::Toolkit::ControlWrapper result;

  argp1 = (Dali::BaseHandle *)jarg1;
  if (!argp1) {
    SWIG_CSharpSetPendingExceptionArgument(SWIG_CSharpArgumentNullException, "Attempt to dereference null Dali::BaseHandle", 0);
    return 0;
  }
  arg1 = *argp1;
  {
    try {
      result = Dali::Toolkit::ControlWrapper::DownCast(arg1);
    } catch (std::out_of_range& e) {
      {
        SWIG_CSharpException(SWIG_IndexError, const_cast<char*>(e.what())); return 0;
      };
    } catch (std::exception& e) {
      {
        SWIG_CSharpException(SWIG_RuntimeError, const_cast<char*>(e.what())); return 0;
      };
    } catch (...) {
      {
        SWIG_CSharpException(SWIG_UnknownError, "unknown error"); return 0;
      };
    }
  }
  jresult = new Dali::Toolkit::ControlWrapper((const Dali::Toolkit::ControlWrapper &)result);
  return jresult;
}

SWIGEXPORT Dali::Toolkit::Internal::Control * SWIGSTDCALL CSharp_Dali_ViewWrapperImpl_SWIGUpcast(Dali::Toolkit::Internal::ControlWrapper *jarg1) {
    return (Dali::Toolkit::Internal::Control *)jarg1;
}

SWIGEXPORT Dali::Toolkit::Control * SWIGSTDCALL CSharp_Dali_ViewWrapper_SWIGUpcast(Dali::Toolkit::ControlWrapper *jarg1) {
    return (Dali::Toolkit::Control *)jarg1;
}

SWIGEXPORT void SWIGSTDCALL CSharp_ViewWrapperImpl_RelayoutRequest(void * jarg1) {
  Dali::Toolkit::Internal::ControlWrapper *arg1 = (Dali::Toolkit::Internal::ControlWrapper *) 0 ;

  arg1 = (Dali::Toolkit::Internal::ControlWrapper *)jarg1;
  {
    try {
      (arg1)->RelayoutRequest();
    } catch (std::out_of_range& e) {
      {
        SWIG_CSharpException(SWIG_IndexError, const_cast<char*>(e.what())); return ;
      };
    } catch (std::exception& e) {
      {
        SWIG_CSharpException(SWIG_RuntimeError, const_cast<char*>(e.what())); return ;
      };
    } catch (...) {
      {
        SWIG_CSharpException(SWIG_UnknownError, "unknown error"); return ;
      };
    }
  }
}


SWIGEXPORT float SWIGSTDCALL CSharp_ViewWrapperImpl_GetHeightForWidthBase(void * jarg1, float jarg2) {
  float jresult ;
  Dali::Toolkit::Internal::ControlWrapper *arg1 = (Dali::Toolkit::Internal::ControlWrapper *) 0 ;
  float arg2 ;
  float result;

  arg1 = (Dali::Toolkit::Internal::ControlWrapper *)jarg1;
  arg2 = (float)jarg2;
  {
    try {
      result = (float)(arg1)->GetHeightForWidthBase(arg2);
    } catch (std::out_of_range& e) {
      {
        SWIG_CSharpException(SWIG_IndexError, const_cast<char*>(e.what())); return 0;
      };
    } catch (std::exception& e) {
      {
        SWIG_CSharpException(SWIG_RuntimeError, const_cast<char*>(e.what())); return 0;
      };
    } catch (...) {
      {
        SWIG_CSharpException(SWIG_UnknownError, "unknown error"); return 0;
      };
    }
  }
  jresult = result;
  return jresult;
}


SWIGEXPORT float SWIGSTDCALL CSharp_ViewWrapperImpl_GetWidthForHeightBase(void * jarg1, float jarg2) {
  float jresult ;
  Dali::Toolkit::Internal::ControlWrapper *arg1 = (Dali::Toolkit::Internal::ControlWrapper *) 0 ;
  float arg2 ;
  float result;

  arg1 = (Dali::Toolkit::Internal::ControlWrapper *)jarg1;
  arg2 = (float)jarg2;
  {
    try {
      result = (float)(arg1)->GetWidthForHeightBase(arg2);
    } catch (std::out_of_range& e) {
      {
        SWIG_CSharpException(SWIG_IndexError, const_cast<char*>(e.what())); return 0;
      };
    } catch (std::exception& e) {
      {
        SWIG_CSharpException(SWIG_RuntimeError, const_cast<char*>(e.what())); return 0;
      };
    } catch (...) {
      {
        SWIG_CSharpException(SWIG_UnknownError, "unknown error"); return 0;
      };
    }
  }
  jresult = result;
  return jresult;
}


SWIGEXPORT float SWIGSTDCALL CSharp_ViewWrapperImpl_CalculateChildSizeBase(void * jarg1, void * jarg2, int jarg3) {
  float jresult ;
  Dali::Toolkit::Internal::ControlWrapper *arg1 = (Dali::Toolkit::Internal::ControlWrapper *) 0 ;
  Dali::Actor *arg2 = 0 ;
  Dali::Dimension::Type arg3 ;
  float result;

  arg1 = (Dali::Toolkit::Internal::ControlWrapper *)jarg1;
  arg2 = (Dali::Actor *)jarg2;
  if (!arg2) {
    SWIG_CSharpSetPendingExceptionArgument(SWIG_CSharpArgumentNullException, "Dali::Actor const & type is null", 0);
    return 0;
  }
  arg3 = (Dali::Dimension::Type)jarg3;
  {
    try {
      result = (float)(arg1)->CalculateChildSizeBase((Dali::Actor const &)*arg2,arg3);
    } catch (std::out_of_range& e) {
      {
        SWIG_CSharpException(SWIG_IndexError, const_cast<char*>(e.what())); return 0;
      };
    } catch (std::exception& e) {
      {
        SWIG_CSharpException(SWIG_RuntimeError, const_cast<char*>(e.what())); return 0;
      };
    } catch (...) {
      {
        SWIG_CSharpException(SWIG_UnknownError, "unknown error"); return 0;
      };
    }
  }
  jresult = result;
  return jresult;
}


SWIGEXPORT unsigned int SWIGSTDCALL CSharp_ViewWrapperImpl_RelayoutDependentOnChildrenBase__SWIG_0(void * jarg1, int jarg2) {
  unsigned int jresult ;
  Dali::Toolkit::Internal::ControlWrapper *arg1 = (Dali::Toolkit::Internal::ControlWrapper *) 0 ;
  Dali::Dimension::Type arg2 ;
  bool result;

  arg1 = (Dali::Toolkit::Internal::ControlWrapper *)jarg1;
  arg2 = (Dali::Dimension::Type)jarg2;
  {
    try {
      result = (bool)(arg1)->RelayoutDependentOnChildrenBase(arg2);
    } catch (std::out_of_range& e) {
      {
        SWIG_CSharpException(SWIG_IndexError, const_cast<char*>(e.what())); return 0;
      };
    } catch (std::exception& e) {
      {
        SWIG_CSharpException(SWIG_RuntimeError, const_cast<char*>(e.what())); return 0;
      };
    } catch (...) {
      {
        SWIG_CSharpException(SWIG_UnknownError, "unknown error"); return 0;
      };
    }
  }
  jresult = result;
  return jresult;
}


SWIGEXPORT unsigned int SWIGSTDCALL CSharp_ViewWrapperImpl_RelayoutDependentOnChildrenBase__SWIG_1(void * jarg1) {
  unsigned int jresult ;
  Dali::Toolkit::Internal::ControlWrapper *arg1 = (Dali::Toolkit::Internal::ControlWrapper *) 0 ;
  bool result;

  arg1 = (Dali::Toolkit::Internal::ControlWrapper *)jarg1;
  {
    try {
      result = (bool)(arg1)->RelayoutDependentOnChildrenBase();
    } catch (std::out_of_range& e) {
      {
        SWIG_CSharpException(SWIG_IndexError, const_cast<char*>(e.what())); return 0;
      };
    } catch (std::exception& e) {
      {
        SWIG_CSharpException(SWIG_RuntimeError, const_cast<char*>(e.what())); return 0;
      };
    } catch (...) {
      {
        SWIG_CSharpException(SWIG_UnknownError, "unknown error"); return 0;
      };
    }
  }
  jresult = result;
  return jresult;
}

SWIGEXPORT void SWIGSTDCALL CSharp_ViewWrapperImpl_RegisterVisual__SWIG_0(void * jarg1, int jarg2, void * jarg3) {
  Dali::Toolkit::Internal::ControlWrapper *arg1 = (Dali::Toolkit::Internal::ControlWrapper *) 0 ;
  Dali::Property::Index arg2 ;
  Dali::Toolkit::Visual::Base *arg3 = 0 ;

  arg1 = (Dali::Toolkit::Internal::ControlWrapper *)jarg1;
  arg2 = (Dali::Property::Index)jarg2;
  arg3 = (Dali::Toolkit::Visual::Base *)jarg3;
  if (!arg3) {
    SWIG_CSharpSetPendingExceptionArgument(SWIG_CSharpArgumentNullException, "Dali::Toolkit::Visual::Base & type is null", 0);
    return ;
  }
  {
    try {
      (arg1)->RegisterVisual(arg2,*arg3);
    } catch (std::out_of_range& e) {
      {
        SWIG_CSharpException(SWIG_IndexError, const_cast<char*>(e.what())); return ;
      };
    } catch (std::exception& e) {
      {
        SWIG_CSharpException(SWIG_RuntimeError, const_cast<char*>(e.what())); return ;
      };
    } catch (...) {
      {
        SWIG_CSharpException(SWIG_UnknownError, "unknown error"); return ;
      };
    }
  }
}


SWIGEXPORT void SWIGSTDCALL CSharp_ViewWrapperImpl_RegisterVisual__SWIG_1(void * jarg1, int jarg2, void * jarg3, unsigned int jarg4) {
  Dali::Toolkit::Internal::ControlWrapper *arg1 = (Dali::Toolkit::Internal::ControlWrapper *) 0 ;
  Dali::Property::Index arg2 ;
  Dali::Toolkit::Visual::Base *arg3 = 0 ;
  bool arg4 ;

  arg1 = (Dali::Toolkit::Internal::ControlWrapper *)jarg1;
  arg2 = (Dali::Property::Index)jarg2;
  arg3 = (Dali::Toolkit::Visual::Base *)jarg3;
  if (!arg3) {
    SWIG_CSharpSetPendingExceptionArgument(SWIG_CSharpArgumentNullException, "Dali::Toolkit::Visual::Base & type is null", 0);
    return ;
  }
  arg4 = jarg4 ? true : false;
  {
    try {
      (arg1)->RegisterVisual(arg2,*arg3,arg4);
    } catch (std::out_of_range& e) {
      {
        SWIG_CSharpException(SWIG_IndexError, const_cast<char*>(e.what())); return ;
      };
    } catch (std::exception& e) {
      {
        SWIG_CSharpException(SWIG_RuntimeError, const_cast<char*>(e.what())); return ;
      };
    } catch (...) {
      {
        SWIG_CSharpException(SWIG_UnknownError, "unknown error"); return ;
      };
    }
  }
}


SWIGEXPORT void SWIGSTDCALL CSharp_ViewWrapperImpl_UnregisterVisual(void * jarg1, int jarg2) {
  Dali::Toolkit::Internal::ControlWrapper *arg1 = (Dali::Toolkit::Internal::ControlWrapper *) 0 ;
  Dali::Property::Index arg2 ;

  arg1 = (Dali::Toolkit::Internal::ControlWrapper *)jarg1;
  arg2 = (Dali::Property::Index)jarg2;
  {
    try {
      (arg1)->UnregisterVisual(arg2);
    } catch (std::out_of_range& e) {
      {
        SWIG_CSharpException(SWIG_IndexError, const_cast<char*>(e.what())); return ;
      };
    } catch (std::exception& e) {
      {
        SWIG_CSharpException(SWIG_RuntimeError, const_cast<char*>(e.what())); return ;
      };
    } catch (...) {
      {
        SWIG_CSharpException(SWIG_UnknownError, "unknown error"); return ;
      };
    }
  }
}


SWIGEXPORT void * SWIGSTDCALL CSharp_ViewWrapperImpl_GetVisual(void * jarg1, int jarg2) {
  void * jresult ;
  Dali::Toolkit::Internal::ControlWrapper *arg1 = (Dali::Toolkit::Internal::ControlWrapper *) 0 ;
  Dali::Property::Index arg2 ;
  Dali::Toolkit::Visual::Base result;

  arg1 = (Dali::Toolkit::Internal::ControlWrapper *)jarg1;
  arg2 = (Dali::Property::Index)jarg2;
  {
    try {
      result = ((Dali::Toolkit::Internal::ControlWrapper const *)arg1)->GetVisual(arg2);
    } catch (std::out_of_range& e) {
      {
        SWIG_CSharpException(SWIG_IndexError, const_cast<char*>(e.what())); return 0;
      };
    } catch (std::exception& e) {
      {
        SWIG_CSharpException(SWIG_RuntimeError, const_cast<char*>(e.what())); return 0;
      };
    } catch (...) {
      {
        SWIG_CSharpException(SWIG_UnknownError, "unknown error"); return 0;
      };
    }
  }
  jresult = new Dali::Toolkit::Visual::Base((const Dali::Toolkit::Visual::Base &)result);
  return jresult;
}


SWIGEXPORT void SWIGSTDCALL CSharp_ViewWrapperImpl_EnableVisual(void * jarg1, int jarg2, unsigned int jarg3) {
  Dali::Toolkit::Internal::ControlWrapper *arg1 = (Dali::Toolkit::Internal::ControlWrapper *) 0 ;
  Dali::Property::Index arg2 ;
  bool arg3 ;

  arg1 = (Dali::Toolkit::Internal::ControlWrapper *)jarg1;
  arg2 = (Dali::Property::Index)jarg2;
  arg3 = jarg3 ? true : false;
  {
    try {
      (arg1)->EnableVisual(arg2,arg3);
    } catch (std::out_of_range& e) {
      {
        SWIG_CSharpException(SWIG_IndexError, const_cast<char*>(e.what())); return ;
      };
    } catch (std::exception& e) {
      {
        SWIG_CSharpException(SWIG_RuntimeError, const_cast<char*>(e.what())); return ;
      };
    } catch (...) {
      {
        SWIG_CSharpException(SWIG_UnknownError, "unknown error"); return ;
      };
    }
  }
}


SWIGEXPORT unsigned int SWIGSTDCALL CSharp_ViewWrapperImpl_IsVisualEnabled(void * jarg1, int jarg2) {
  unsigned int jresult ;
  Dali::Toolkit::Internal::ControlWrapper *arg1 = (Dali::Toolkit::Internal::ControlWrapper *) 0 ;
  Dali::Property::Index arg2 ;
  bool result;

  arg1 = (Dali::Toolkit::Internal::ControlWrapper *)jarg1;
  arg2 = (Dali::Property::Index)jarg2;
  {
    try {
      result = (bool)((Dali::Toolkit::Internal::ControlWrapper const *)arg1)->IsVisualEnabled(arg2);
    } catch (std::out_of_range& e) {
      {
        SWIG_CSharpException(SWIG_IndexError, const_cast<char*>(e.what())); return 0;
      };
    } catch (std::exception& e) {
      {
        SWIG_CSharpException(SWIG_RuntimeError, const_cast<char*>(e.what())); return 0;
      };
    } catch (...) {
      {
        SWIG_CSharpException(SWIG_UnknownError, "unknown error"); return 0;
      };
    }
  }
  jresult = result;
  return jresult;
}

SWIGEXPORT void * SWIGSTDCALL CSharp_ViewWrapperImpl_CreateTransition(void * jarg1, void * jarg2) {
  void * jresult ;
  Dali::Toolkit::Internal::ControlWrapper *arg1 = (Dali::Toolkit::Internal::ControlWrapper *) 0 ;
  Dali::Toolkit::TransitionData *arg2 = 0 ;
  Dali::Animation result;

  arg1 = (Dali::Toolkit::Internal::ControlWrapper *)jarg1;
  arg2 = (Dali::Toolkit::TransitionData *)jarg2;
  if (!arg2) {
    SWIG_CSharpSetPendingExceptionArgument(SWIG_CSharpArgumentNullException, "Dali::Toolkit::TransitionData const & type is null", 0);
    return 0;
  }
  {
    try {
      result = (arg1)->CreateTransition((Dali::Toolkit::TransitionData const &)*arg2);
    } catch (std::out_of_range& e) {
      {
        SWIG_CSharpException(SWIG_IndexError, const_cast<char*>(e.what())); return 0;
      };
    } catch (std::exception& e) {
      {
        SWIG_CSharpException(SWIG_RuntimeError, const_cast<char*>(e.what())); return 0;
      };
    } catch (...) {
      {
        SWIG_CSharpException(SWIG_UnknownError, "unknown error"); return 0;
      };
    }
  }
  jresult = new Dali::Animation((const Dali::Animation &)result);
  return jresult;
}


SWIGEXPORT void SWIGSTDCALL CSharp_ViewWrapperImpl_EmitKeyInputFocusSignal(void * jarg1, unsigned int jarg2) {
  Dali::Toolkit::Internal::ControlWrapper *arg1 = (Dali::Toolkit::Internal::ControlWrapper *) 0 ;
  bool arg2 ;

  arg1 = (Dali::Toolkit::Internal::ControlWrapper *)jarg1;
  arg2 = jarg2 ? true : false;
  {
    try {
      (arg1)->EmitKeyInputFocusSignal(arg2);
    } catch (std::out_of_range& e) {
      {
        SWIG_CSharpException(SWIG_IndexError, const_cast<char*>(e.what())); return ;
      };
    } catch (std::exception& e) {
      {
        SWIG_CSharpException(SWIG_RuntimeError, const_cast<char*>(e.what())); return ;
      };
    } catch (...) {
      {
        SWIG_CSharpException(SWIG_UnknownError, "unknown error"); return ;
      };
    }
  }
}

SWIGEXPORT void SWIGSTDCALL CSharp_Dali_ViewWrapperImpl_ApplyThemeStyle(void * jarg1) {
  Dali::Toolkit::Internal::ControlWrapper *arg1 = (Dali::Toolkit::Internal::ControlWrapper *) 0 ;

  arg1 = (Dali::Toolkit::Internal::ControlWrapper *)jarg1;
  {
    try {
      (arg1)->ApplyThemeStyle();
    } catch (std::out_of_range& e) {
      {
        SWIG_CSharpException(SWIG_IndexError, const_cast<char*>(e.what())); return ;
      };
    } catch (std::exception& e) {
      {
        SWIG_CSharpException(SWIG_RuntimeError, const_cast<char*>(e.what())); return ;
      };
    } catch (...) {
      {
        SWIG_CSharpException(SWIG_UnknownError, "unknown error"); return ;
      };
    }
  }
}


#ifdef __cplusplus
}
#endif
