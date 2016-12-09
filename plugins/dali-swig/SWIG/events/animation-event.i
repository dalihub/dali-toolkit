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

%define Animation_EVENTHANDLER_TYPEMAP_EVENTARG(NameSpace, ClassName)
%typemap(csimports) NameSpace::ClassName %{
using System;
using System.Runtime.InteropServices;
%}

%enddef

%define Animation_EVENTHANDLER_TYPEMAP_HELPER(NameSpace, ClassName)
  %typemap(cscode) NameSpace::ClassName %{

    public static readonly int ParentOrigin = NDalicPINVOKE.Actor_Property_PARENT_ORIGIN_get();
    public static readonly int ParentOriginX = NDalicPINVOKE.Actor_Property_PARENT_ORIGIN_X_get();
    public static readonly int ParentOriginY = NDalicPINVOKE.Actor_Property_PARENT_ORIGIN_Y_get();
    public static readonly int ParentOriginZ = NDalicPINVOKE.Actor_Property_PARENT_ORIGIN_Z_get();
    public static readonly int AnchorPoint = NDalicPINVOKE.Actor_Property_ANCHOR_POINT_get();
    public static readonly int AnchorPointX = NDalicPINVOKE.Actor_Property_ANCHOR_POINT_X_get();
    public static readonly int AnchorPointY = NDalicPINVOKE.Actor_Property_ANCHOR_POINT_Y_get();
    public static readonly int AnchorPointZ = NDalicPINVOKE.Actor_Property_ANCHOR_POINT_Z_get();
    public static readonly int Size = NDalicPINVOKE.Actor_Property_SIZE_get();
    public static readonly int SizeWidth = NDalicPINVOKE.Actor_Property_SIZE_WIDTH_get();
    public static readonly int SizeHeight = NDalicPINVOKE.Actor_Property_SIZE_HEIGHT_get();
    public static readonly int SizeDepth = NDalicPINVOKE.Actor_Property_SIZE_DEPTH_get();
    public static readonly int Position = NDalicPINVOKE.Actor_Property_POSITION_get();
    public static readonly int PositionX = NDalicPINVOKE.Actor_Property_POSITION_X_get();
    public static readonly int PositionY = NDalicPINVOKE.Actor_Property_POSITION_Y_get();
    public static readonly int PositionZ = NDalicPINVOKE.Actor_Property_POSITION_Z_get();
    public static readonly int WorldPosition = NDalicPINVOKE.Actor_Property_WORLD_POSITION_get();
    public static readonly int WorldPositionX = NDalicPINVOKE.Actor_Property_WORLD_POSITION_X_get();
    public static readonly int WorldPositionY = NDalicPINVOKE.Actor_Property_WORLD_POSITION_Y_get();
    public static readonly int WorldPositionZ = NDalicPINVOKE.Actor_Property_WORLD_POSITION_Z_get();
    public static readonly int Orientation = NDalicPINVOKE.Actor_Property_ORIENTATION_get();
    public static readonly int WorldOrientation = NDalicPINVOKE.Actor_Property_WORLD_ORIENTATION_get();
    public static readonly int Scale = NDalicPINVOKE.Actor_Property_SCALE_get();
    public static readonly int ScaleX = NDalicPINVOKE.Actor_Property_SCALE_X_get();
    public static readonly int ScaleY = NDalicPINVOKE.Actor_Property_SCALE_Y_get();
    public static readonly int ScaleZ = NDalicPINVOKE.Actor_Property_SCALE_Z_get();
    public static readonly int WorldScale = NDalicPINVOKE.Actor_Property_WORLD_SCALE_get();
    public static readonly int Visibile = NDalicPINVOKE.Actor_Property_VISIBLE_get();
    public static readonly int Color = NDalicPINVOKE.Actor_Property_COLOR_get();
    public static readonly int ColorRed = NDalicPINVOKE.Actor_Property_COLOR_RED_get();
    public static readonly int ColorGreeb = NDalicPINVOKE.Actor_Property_COLOR_GREEN_get();
    public static readonly int ColorBlue = NDalicPINVOKE.Actor_Property_COLOR_BLUE_get();
    public static readonly int ColorAlpha = NDalicPINVOKE.Actor_Property_COLOR_ALPHA_get();
    public static readonly int WorldColor = NDalicPINVOKE.Actor_Property_WORLD_COLOR_get();
    public static readonly int WorldMatrix = NDalicPINVOKE.Actor_Property_WORLD_MATRIX_get();
    public static readonly int Name = NDalicPINVOKE.Actor_Property_NAME_get();
    public static readonly int Sensitive = NDalicPINVOKE.Actor_Property_SENSITIVE_get();
    public static readonly int LeaveRequired = NDalicPINVOKE.Actor_Property_LEAVE_REQUIRED_get();
    public static readonly int InheritOrientation = NDalicPINVOKE.Actor_Property_INHERIT_ORIENTATION_get();
    public static readonly int InheritScale = NDalicPINVOKE.Actor_Property_INHERIT_SCALE_get();
    public static readonly int ColorMode = NDalicPINVOKE.Actor_Property_COLOR_MODE_get();
    public static readonly int PositionInheritance = NDalicPINVOKE.Actor_Property_POSITION_INHERITANCE_get();
    public static readonly int DrawMode = NDalicPINVOKE.Actor_Property_DRAW_MODE_get();
    public static readonly int SizeModeFactor = NDalicPINVOKE.Actor_Property_SIZE_MODE_FACTOR_get();
    public static readonly int WidthResizePolicy = NDalicPINVOKE.Actor_Property_WIDTH_RESIZE_POLICY_get();
    public static readonly int HeightResizePolicy = NDalicPINVOKE.Actor_Property_HEIGHT_RESIZE_POLICY_get();
    public static readonly int SizeScalePolicy = NDalicPINVOKE.Actor_Property_SIZE_SCALE_POLICY_get();
    public static readonly int WidthForHeight = NDalicPINVOKE.Actor_Property_WIDTH_FOR_HEIGHT_get();
    public static readonly int HeightForWidth = NDalicPINVOKE.Actor_Property_HEIGHT_FOR_WIDTH_get();
    public static readonly int Padding = NDalicPINVOKE.Actor_Property_PADDING_get();
    public static readonly int MinimumSize = NDalicPINVOKE.Actor_Property_MINIMUM_SIZE_get();
    public static readonly int MaximumSize = NDalicPINVOKE.Actor_Property_MAXIMUM_SIZE_get();
    public static readonly int InheritPosition = NDalicPINVOKE.Actor_Property_INHERIT_POSITION_get();
    public static readonly int ClippingMode = NDalicPINVOKE.Actor_Property_CLIPPING_MODE_get();

    /**
      * @brief Event arguments that passed via Finished signal
      *
      */
    public class FinishedEventArgs : EventArgs
    {
      private Animation _animation;

      /**
        * @brief Animation - is the Animation which has finished with the animation.
        *
        */
      public Animation Animation
      {
        get
        {
          return _animation;
        }
        set
        {
          _animation = value;
        }
      }
    }

    [UnmanagedFunctionPointer(CallingConvention.StdCall)]
    private delegate void FinishedEventCallbackDelegate(IntPtr Animation);
    private DaliEventHandler<object,FinishedEventArgs> _animationFinishedEventHandler;
    private FinishedEventCallbackDelegate _animationFinishedEventCallbackDelegate;

    /**
      * @brief Event for Finished signal which can be used to subscribe/unsubscribe the event handler
      * (in the type of FinishedEventHandler - DaliEventHandler<object,FinishedEventArgs>) provided by the user.
      * Finished signal is emitted when an Animation's animations have finished.
      */
    public event DaliEventHandler<object,FinishedEventArgs> Finished
    {
      add
      {
        lock(this)
        {
          // Restricted to only one listener
          if (_animationFinishedEventHandler == null)
          {
            _animationFinishedEventHandler += value;

            _animationFinishedEventCallbackDelegate = new FinishedEventCallbackDelegate(OnFinished);
            this.FinishedSignal().Connect(_animationFinishedEventCallbackDelegate);
          }
        }
      }

      remove
      {
        lock(this)
        {
          if (_animationFinishedEventHandler != null)
          {
            this.FinishedSignal().Disconnect(_animationFinishedEventCallbackDelegate);
          }

          _animationFinishedEventHandler -= value;
        }
      }
    }

    // Callback for Animation FinishedSignal
    private void OnFinished(IntPtr data)
    {
      FinishedEventArgs e = new FinishedEventArgs();

      // Populate all members of "e" (FinishedEventArgs) with real data
      e.Animation = Animation.GetAnimationFromPtr(data);

      if (_animationFinishedEventHandler != null)
      {
        //here we send all data to user event handlers
        _animationFinishedEventHandler(this, e);
      }
    }


  public static ClassName Get ## ClassName ## FromPtr(global::System.IntPtr cPtr) {
    ClassName ret = new ClassName(cPtr, false);
    if (NDalicPINVOKE.SWIGPendingException.Pending) throw NDalicPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }


  public float Duration
  {
    set
    {
       SetDuration(value);
    }
    get
    {
       float ret = GetDuration();
       return ret;
    }
  }

  public int DurationMilliSecs
  {
    set
    {
      SetDuration(value/1000);
    }
    get
    {
      int ret = (int) GetDuration() * 1000;
      return ret;
    }
  }

  public AlphaFunction DefaultAlphaFunction
  {
    set
    {
       SetDefaultAlphaFunction(value);
    }
    get
    {
       AlphaFunction ret = GetDefaultAlphaFunction();
       return ret;
    }
  }

  public Animation.State Status
  {
    get
    {
       Animation.State ret = GetState();
       return ret;
    }
  }

  public int LoopCount
  {
    set
    {
      SetLoopCount(value);
    }
    get
    {
      int ret = GetLoopCount();
      return ret;
    }
  }

  public bool Looping
  {
    set
    {
      SetLooping(value);
    }
    get
    {
      bool ret = IsLooping();
      return ret;
    }
  }

  public void AnimateBy<T,U>(Actor target, T propertyIndex, U relativeValue)
  {
	dynamic var = (object)(propertyIndex);
	dynamic obj = (object)(relativeValue);
	AnimateBy(new Property(target, var), new Property.Value(obj));
  }

  public void AnimateBy<T,U>(Actor target, T propertyIndex, U relativeValue, AlphaFunction alpha)
  {
	dynamic var = (object)(propertyIndex);
	dynamic obj = (object)(relativeValue);
	AnimateBy(new Property(target, var), new Property.Value(obj),  alpha);
  }

  public void AnimateBy<T,U>(Actor target, T propertyIndex, U relativeValue, TimePeriod period)
  {
	dynamic var = (object)(propertyIndex);
	dynamic obj = (object)(relativeValue);
	AnimateBy(new Property(target, var), new Property.Value(obj), period);
  }

  public void AnimateBy<T,U>(Actor target, T propertyIndex, U relativeValue, AlphaFunction alpha, TimePeriod period)
  {
	dynamic var = (object)(propertyIndex);
	dynamic obj = (object)(relativeValue);
	AnimateBy(new Property(target, var), new Property.Value(obj), alpha, period);
  }

  public void AnimateTo<T,U>(Actor target, T propertyIndex, U destinationValue)
  {
	dynamic var = (object)(propertyIndex);
	dynamic obj = (object)(destinationValue);
	AnimateTo(new Property(target, var), new Property.Value(obj));
  }

  public void AnimateTo<T,U>(Actor target, T propertyIndex, U destinationValue, AlphaFunction alpha)
  {
	dynamic var = (object)(propertyIndex);
	dynamic obj = (object)(destinationValue);
	AnimateTo(new Property(target, var), new Property.Value(obj), alpha);
  }

  public void AnimateTo<T,U>(Actor target, T propertyIndex, U destinationValue, TimePeriod period)
  {
	dynamic var = (object)(propertyIndex);
	dynamic obj = (object)(destinationValue);
	AnimateTo(new Property(target, var), new Property.Value(obj), period);
  }

  public void AnimateTo<T,U>(Actor target, T propertyIndex, U destinationValue, AlphaFunction alpha, TimePeriod period)
  {
	dynamic var = (object)(propertyIndex);
	dynamic obj = (object)(destinationValue);
	AnimateTo(new Property(target, var), new Property.Value(obj), alpha, period);
  }

  public void AnimateBetween<U>(Actor target, U propertyIndex, KeyFrames keyFrames)
  {
	dynamic var = (object)(propertyIndex);
	AnimateBetween(new Property(target, var), keyFrames);
  }

  public void AnimateBetween<U>(Actor target, U propertyIndex, KeyFrames keyFrames, Animation.Interpolation interpolation)
  {
	dynamic var = (object)(propertyIndex);
	AnimateBetween(new Property(target, var), keyFrames, interpolation);
  }

  public void AnimateBetween<U>(Actor target, U propertyIndex, KeyFrames keyFrames, AlphaFunction alpha)
  {
	dynamic var = (object)(propertyIndex);
	AnimateBetween(new Property(target, var), keyFrames, alpha);
  }

  public void AnimateBetween<U>(Actor target, U propertyIndex, KeyFrames keyFrames, AlphaFunction alpha, Animation.Interpolation interpolation)
  {
	dynamic var = (object)(propertyIndex);
	AnimateBetween(new Property(target, var), keyFrames, alpha, interpolation);
  }

  public void AnimateBetween<U>(Actor target, U propertyIndex, KeyFrames keyFrames, TimePeriod period)
  {
	dynamic var = (object)(propertyIndex);
	AnimateBetween(new Property(target, var), keyFrames, period);
  }

  public void AnimateBetween<U>(Actor target, U propertyIndex, KeyFrames keyFrames, TimePeriod period, Animation.Interpolation interpolation)
  {
	dynamic var = (object)(propertyIndex);
	AnimateBetween(new Property(target, var), keyFrames,  period, interpolation);
  }

  public void AnimateBetween<U>(Actor target, U propertyIndex, KeyFrames keyFrames, AlphaFunction alpha, TimePeriod period)
  {
	dynamic var = (object)(propertyIndex);
	AnimateBetween(new Property(target, var), keyFrames, alpha, period);
  }

  public void AnimateBetween<U>(Actor target, U propertyIndex, KeyFrames keyFrames, AlphaFunction alpha, TimePeriod period, Animation.Interpolation interpolation)
  {
	dynamic var = (object)(propertyIndex);
	AnimateBetween(new Property(target, var), keyFrames, alpha, period, interpolation);
  }

  public void Stop(Animation.EndAction action) {
    SetEndAction(action);
    NDalicPINVOKE.Animation_Stop(swigCPtr);
    if (NDalicPINVOKE.SWIGPendingException.Pending) throw NDalicPINVOKE.SWIGPendingException.Retrieve();
  }

  /**
   * @brief Create an initialized Animation.
   *
   * The animation will not loop.
   * The default end action is "Bake".
   * The default Alpha function is linear.
   * @since 1.0.0
   * @param [in] durationmSeconds The duration in milli seconds (int).
   * @return A handle to a newly allocated Dali resource.
   * @pre DurationmSeconds must be greater than zero.
   */
  public Animation (int durationmSeconds) : this (NDalicPINVOKE.Animation_New((float)durationmSeconds/1000.0f), true) {
    if (NDalicPINVOKE.SWIGPendingException.Pending) throw NDalicPINVOKE.SWIGPendingException.Retrieve();
  }
%}

%enddef


%define DALI_animation_EVENTHANDLER_PARAM( NameSpace, ClassName)
  Animation_EVENTHANDLER_TYPEMAP_EVENTARG( NameSpace, ClassName);
  Animation_EVENTHANDLER_TYPEMAP_HELPER( NameSpace, ClassName);
  %enddef

  namespace Dali
{
  DALI_animation_EVENTHANDLER_PARAM( Dali, Animation);
}
