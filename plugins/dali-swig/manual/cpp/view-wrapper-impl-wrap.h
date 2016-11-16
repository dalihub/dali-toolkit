#ifndef CSHARP_VIEW_WRAPPER_IMPL_H
#define CSHARP_VIEW_WRAPPER_IMPL_H

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

#include "common.h"

class SwigDirector_ViewWrapperImpl : public Dali::Toolkit::Internal::ControlWrapper
{
public:

    SwigDirector_ViewWrapperImpl(Dali::Toolkit::Internal::ControlWrapper::CustomControlBehaviour behaviourFlags);
    virtual ~SwigDirector_ViewWrapperImpl();
    virtual void OnStageConnection(int depth);
    virtual void OnStageConnectionSwigPublic(int depth)
    {
      Dali::Toolkit::Internal::Control::OnStageConnection(depth);
    }
    virtual void OnStageDisconnection();
    virtual void OnStageDisconnectionSwigPublic()
    {
      Dali::Toolkit::Internal::Control::OnStageDisconnection();
    }
    virtual void OnChildAdd(Dali::Actor &child);
    virtual void OnChildAddSwigPublic(Dali::Actor &child)
    {
      Dali::Toolkit::Internal::Control::OnChildAdd(child);
    }
    virtual void OnChildRemove(Dali::Actor &child);
    virtual void OnChildRemoveSwigPublic(Dali::Actor &child)
    {
      Dali::Toolkit::Internal::Control::OnChildRemove(child);
    }
    virtual void OnPropertySet(Dali::Property::Index index, Dali::Property::Value propertyValue);
    virtual void OnSizeSet(Dali::Vector3 const &targetSize);
    virtual void OnSizeSetSwigPublic(Dali::Vector3 const &targetSize)
    {
      Dali::Toolkit::Internal::Control::OnSizeSet(targetSize);
    }
    virtual void OnSizeAnimation(Dali::Animation &animation, Dali::Vector3 const &targetSize);
    virtual void OnSizeAnimationSwigPublic(Dali::Animation &animation, Dali::Vector3 const &targetSize)
    {
      Dali::Toolkit::Internal::Control::OnSizeAnimation(animation,targetSize);
    }
    virtual bool OnTouchEvent(Dali::TouchEvent const &event);
    virtual bool OnTouchEventSwigPublic(Dali::TouchEvent const &event)
    {
      return Dali::Toolkit::Internal::Control::OnTouchEvent(event);
    }
    virtual bool OnHoverEvent(Dali::HoverEvent const &event);
    virtual bool OnHoverEventSwigPublic(Dali::HoverEvent const &event)
    {
      return Dali::Toolkit::Internal::Control::OnHoverEvent(event);
    }
    virtual bool OnKeyEvent(Dali::KeyEvent const &event);
    virtual bool OnKeyEventSwigPublic(Dali::KeyEvent const &event)
    {
      return Dali::Toolkit::Internal::Control::OnKeyEvent(event);
    }
    virtual bool OnWheelEvent(Dali::WheelEvent const &event);
    virtual bool OnWheelEventSwigPublic(Dali::WheelEvent const &event)
    {
      return Dali::Toolkit::Internal::Control::OnWheelEvent(event);
    }
    virtual void OnRelayout(Dali::Vector2 const &size, Dali::RelayoutContainer &container);
    virtual void OnRelayoutSwigPublic(Dali::Vector2 const &size, Dali::RelayoutContainer &container)
    {
      Dali::Toolkit::Internal::Control::OnRelayout(size,container);
    }
    virtual void OnSetResizePolicy(Dali::ResizePolicy::Type policy, Dali::Dimension::Type dimension);
    virtual void OnSetResizePolicySwigPublic(Dali::ResizePolicy::Type policy, Dali::Dimension::Type dimension)
    {
      Dali::Toolkit::Internal::Control::OnSetResizePolicy(policy,dimension);
    }
    virtual Dali::Vector3 GetNaturalSize();
    virtual Dali::Vector3 GetNaturalSizeSwigPublic()
    {
      return Dali::Toolkit::Internal::Control::GetNaturalSize();
    }
    virtual float CalculateChildSize(Dali::Actor const &child, Dali::Dimension::Type dimension);
    virtual float CalculateChildSizeSwigPublic(Dali::Actor const &child, Dali::Dimension::Type dimension)
    {
      return Dali::Toolkit::Internal::Control::CalculateChildSize(child,dimension);
    }
    virtual float GetHeightForWidth(float width);
    virtual float GetHeightForWidthSwigPublic(float width)
    {
      return Dali::Toolkit::Internal::Control::GetHeightForWidth(width);
    }
    virtual float GetWidthForHeight(float height);
    virtual float GetWidthForHeightSwigPublic(float height)
    {
      return Dali::Toolkit::Internal::Control::GetWidthForHeight(height);
    }
    virtual bool RelayoutDependentOnChildren(Dali::Dimension::Type dimension = Dali::Dimension::ALL_DIMENSIONS);
    virtual bool RelayoutDependentOnChildrenSwigPublic(Dali::Dimension::Type dimension = Dali::Dimension::ALL_DIMENSIONS)
    {
      return Dali::Toolkit::Internal::Control::RelayoutDependentOnChildren(dimension);
    }
    virtual void OnCalculateRelayoutSize(Dali::Dimension::Type dimension);
    virtual void OnCalculateRelayoutSizeSwigPublic(Dali::Dimension::Type dimension)
    {
      Dali::Toolkit::Internal::Control::OnCalculateRelayoutSize(dimension);
    }
    virtual void OnLayoutNegotiated(float size, Dali::Dimension::Type dimension);
    virtual void OnLayoutNegotiatedSwigPublic(float size, Dali::Dimension::Type dimension)
    {
      Dali::Toolkit::Internal::Control::OnLayoutNegotiated(size,dimension);
    }
    virtual void OnInitialize();
    virtual void OnControlChildAdd(Dali::Actor &child);
    virtual void OnControlChildRemove(Dali::Actor &child);
    virtual void OnStyleChange(Dali::Toolkit::StyleManager styleManager, Dali::StyleChange::Type change);
    virtual bool OnAccessibilityActivated();
    virtual bool OnAccessibilityPan(Dali::PanGesture gesture);
    virtual bool OnAccessibilityTouch(Dali::TouchEvent const &touchEvent);
    virtual bool OnAccessibilityValueChange(bool isIncrease);
    virtual bool OnAccessibilityZoom();
    virtual void OnKeyInputFocusGained();
    virtual void OnKeyInputFocusLost();
    virtual Dali::Actor GetNextKeyboardFocusableActor(Dali::Actor currentFocusedActor, Dali::Toolkit::Control::KeyboardFocus::Direction direction, bool loopEnabled);
    virtual void OnKeyboardFocusChangeCommitted(Dali::Actor commitedFocusableActor);
    virtual bool OnKeyboardEnter();
    virtual void OnPinch(Dali::PinchGesture const &pinch);
    virtual void OnPan(Dali::PanGesture const &pan);
    virtual void OnTap(Dali::TapGesture const &tap);
    virtual void OnLongPress(Dali::LongPressGesture const &longPress);
    virtual void SignalConnected(Dali::SlotObserver *slotObserver, Dali::CallbackBase *callback);
    virtual void SignalDisconnected(Dali::SlotObserver *slotObserver, Dali::CallbackBase *callback);
    virtual Dali::Toolkit::Internal::Control::Extension *GetControlExtension();

    typedef void (SWIGSTDCALL* SWIG_Callback0_t)(int);
    typedef void (SWIGSTDCALL* SWIG_Callback1_t)();
    typedef void (SWIGSTDCALL* SWIG_Callback2_t)(void *);
    typedef void (SWIGSTDCALL* SWIG_Callback3_t)(void *);
    typedef void (SWIGSTDCALL* SWIG_Callback4_t)(int, void *);
    typedef void (SWIGSTDCALL* SWIG_Callback5_t)(void *);
    typedef void (SWIGSTDCALL* SWIG_Callback6_t)(void *, void *);
    typedef unsigned int (SWIGSTDCALL* SWIG_Callback7_t)(void *);
    typedef unsigned int (SWIGSTDCALL* SWIG_Callback8_t)(void *);
    typedef unsigned int (SWIGSTDCALL* SWIG_Callback9_t)(void *);
    typedef unsigned int (SWIGSTDCALL* SWIG_Callback10_t)(void *);
    typedef void (SWIGSTDCALL* SWIG_Callback11_t)(void *, void *);
    typedef void (SWIGSTDCALL* SWIG_Callback12_t)(int, int);
    typedef void * (SWIGSTDCALL* SWIG_Callback13_t)();
    typedef float (SWIGSTDCALL* SWIG_Callback14_t)(void *, int);
    typedef float (SWIGSTDCALL* SWIG_Callback15_t)(float);
    typedef float (SWIGSTDCALL* SWIG_Callback16_t)(float);
    typedef unsigned int (SWIGSTDCALL* SWIG_Callback17_t)(int);
    typedef unsigned int (SWIGSTDCALL* SWIG_Callback18_t)();
    typedef void (SWIGSTDCALL* SWIG_Callback19_t)(int);
    typedef void (SWIGSTDCALL* SWIG_Callback20_t)(float, int);
    typedef void (SWIGSTDCALL* SWIG_Callback21_t)();
    typedef void (SWIGSTDCALL* SWIG_Callback22_t)(void *);
    typedef void (SWIGSTDCALL* SWIG_Callback23_t)(void *);
    typedef void (SWIGSTDCALL* SWIG_Callback24_t)(void *, int);
    typedef unsigned int (SWIGSTDCALL* SWIG_Callback25_t)();
    typedef unsigned int (SWIGSTDCALL* SWIG_Callback26_t)(void *);
    typedef unsigned int (SWIGSTDCALL* SWIG_Callback27_t)(void *);
    typedef unsigned int (SWIGSTDCALL* SWIG_Callback28_t)(unsigned int);
    typedef unsigned int (SWIGSTDCALL* SWIG_Callback29_t)();
    typedef void (SWIGSTDCALL* SWIG_Callback30_t)();
    typedef void (SWIGSTDCALL* SWIG_Callback31_t)();
    typedef void * (SWIGSTDCALL* SWIG_Callback32_t)(void *, int, unsigned int);
    typedef void (SWIGSTDCALL* SWIG_Callback33_t)(void *);
    typedef unsigned int (SWIGSTDCALL* SWIG_Callback34_t)();
    typedef void (SWIGSTDCALL* SWIG_Callback35_t)(void *);
    typedef void (SWIGSTDCALL* SWIG_Callback36_t)(void *);
    typedef void (SWIGSTDCALL* SWIG_Callback37_t)(void *);
    typedef void (SWIGSTDCALL* SWIG_Callback38_t)(void *);
    typedef void (SWIGSTDCALL* SWIG_Callback39_t)(void *, void *);
    typedef void (SWIGSTDCALL* SWIG_Callback40_t)(void *, void *);
    void swig_connect_director(SWIG_Callback0_t callbackOnStageConnection, SWIG_Callback1_t callbackOnStageDisconnection, SWIG_Callback2_t callbackOnChildAdd, SWIG_Callback3_t callbackOnChildRemove, SWIG_Callback4_t callbackOnPropertySet, SWIG_Callback5_t callbackOnSizeSet, SWIG_Callback6_t callbackOnSizeAnimation, SWIG_Callback7_t callbackOnTouchEvent, SWIG_Callback8_t callbackOnHoverEvent, SWIG_Callback9_t callbackOnKeyEvent, SWIG_Callback10_t callbackOnWheelEvent, SWIG_Callback11_t callbackOnRelayout, SWIG_Callback12_t callbackOnSetResizePolicy, SWIG_Callback13_t callbackGetNaturalSize, SWIG_Callback14_t callbackCalculateChildSize, SWIG_Callback15_t callbackGetHeightForWidth, SWIG_Callback16_t callbackGetWidthForHeight, SWIG_Callback17_t callbackRelayoutDependentOnChildren__SWIG_0, SWIG_Callback18_t callbackRelayoutDependentOnChildren__SWIG_1, SWIG_Callback19_t callbackOnCalculateRelayoutSize, SWIG_Callback20_t callbackOnLayoutNegotiated, SWIG_Callback21_t callbackOnInitialize, SWIG_Callback22_t callbackOnControlChildAdd, SWIG_Callback23_t callbackOnControlChildRemove, SWIG_Callback24_t callbackOnStyleChange, SWIG_Callback25_t callbackOnAccessibilityActivated, SWIG_Callback26_t callbackOnAccessibilityPan, SWIG_Callback27_t callbackOnAccessibilityTouch, SWIG_Callback28_t callbackOnAccessibilityValueChange, SWIG_Callback29_t callbackOnAccessibilityZoom, SWIG_Callback30_t callbackOnKeyInputFocusGained, SWIG_Callback31_t callbackOnKeyInputFocusLost, SWIG_Callback32_t callbackGetNextKeyboardFocusableActor, SWIG_Callback33_t callbackOnKeyboardFocusChangeCommitted, SWIG_Callback34_t callbackOnKeyboardEnter, SWIG_Callback35_t callbackOnPinch, SWIG_Callback36_t callbackOnPan, SWIG_Callback37_t callbackOnTap, SWIG_Callback38_t callbackOnLongPress, SWIG_Callback39_t callbackSignalConnected, SWIG_Callback40_t callbackSignalDisconnected);

private:
    SWIG_Callback0_t swig_callbackOnStageConnection;
    SWIG_Callback1_t swig_callbackOnStageDisconnection;
    SWIG_Callback2_t swig_callbackOnChildAdd;
    SWIG_Callback3_t swig_callbackOnChildRemove;
    SWIG_Callback4_t swig_callbackOnPropertySet;
    SWIG_Callback5_t swig_callbackOnSizeSet;
    SWIG_Callback6_t swig_callbackOnSizeAnimation;
    SWIG_Callback7_t swig_callbackOnTouchEvent;
    SWIG_Callback8_t swig_callbackOnHoverEvent;
    SWIG_Callback9_t swig_callbackOnKeyEvent;
    SWIG_Callback10_t swig_callbackOnWheelEvent;
    SWIG_Callback11_t swig_callbackOnRelayout;
    SWIG_Callback12_t swig_callbackOnSetResizePolicy;
    SWIG_Callback13_t swig_callbackGetNaturalSize;
    SWIG_Callback14_t swig_callbackCalculateChildSize;
    SWIG_Callback15_t swig_callbackGetHeightForWidth;
    SWIG_Callback16_t swig_callbackGetWidthForHeight;
    SWIG_Callback17_t swig_callbackRelayoutDependentOnChildren__SWIG_0;
    SWIG_Callback18_t swig_callbackRelayoutDependentOnChildren__SWIG_1;
    SWIG_Callback19_t swig_callbackOnCalculateRelayoutSize;
    SWIG_Callback20_t swig_callbackOnLayoutNegotiated;
    SWIG_Callback21_t swig_callbackOnInitialize;
    SWIG_Callback22_t swig_callbackOnControlChildAdd;
    SWIG_Callback23_t swig_callbackOnControlChildRemove;
    SWIG_Callback24_t swig_callbackOnStyleChange;
    SWIG_Callback25_t swig_callbackOnAccessibilityActivated;
    SWIG_Callback26_t swig_callbackOnAccessibilityPan;
    SWIG_Callback27_t swig_callbackOnAccessibilityTouch;
    SWIG_Callback28_t swig_callbackOnAccessibilityValueChange;
    SWIG_Callback29_t swig_callbackOnAccessibilityZoom;
    SWIG_Callback30_t swig_callbackOnKeyInputFocusGained;
    SWIG_Callback31_t swig_callbackOnKeyInputFocusLost;
    SWIG_Callback32_t swig_callbackGetNextKeyboardFocusableActor;
    SWIG_Callback33_t swig_callbackOnKeyboardFocusChangeCommitted;
    SWIG_Callback34_t swig_callbackOnKeyboardEnter;
    SWIG_Callback35_t swig_callbackOnPinch;
    SWIG_Callback36_t swig_callbackOnPan;
    SWIG_Callback37_t swig_callbackOnTap;
    SWIG_Callback38_t swig_callbackOnLongPress;
    SWIG_Callback39_t swig_callbackSignalConnected;
    SWIG_Callback40_t swig_callbackSignalDisconnected;
    void swig_init_callbacks();
};

#endif /* CSHARP_VIEW_WRAPPER_IMPL_H */
