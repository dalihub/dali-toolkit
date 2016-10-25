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

    public static readonly int PARENT_ORIGIN = NDalicPINVOKE.Actor_Property_PARENT_ORIGIN_get();
    public static readonly int PARENT_ORIGIN_X = NDalicPINVOKE.Actor_Property_PARENT_ORIGIN_X_get();
    public static readonly int PARENT_ORIGIN_Y = NDalicPINVOKE.Actor_Property_PARENT_ORIGIN_Y_get();
    public static readonly int PARENT_ORIGIN_Z = NDalicPINVOKE.Actor_Property_PARENT_ORIGIN_Z_get();
    public static readonly int ANCHOR_POINT = NDalicPINVOKE.Actor_Property_ANCHOR_POINT_get();
    public static readonly int ANCHOR_POINT_X = NDalicPINVOKE.Actor_Property_ANCHOR_POINT_X_get();
    public static readonly int ANCHOR_POINT_Y = NDalicPINVOKE.Actor_Property_ANCHOR_POINT_Y_get();
    public static readonly int ANCHOR_POINT_Z = NDalicPINVOKE.Actor_Property_ANCHOR_POINT_Z_get();
    public static readonly int SIZE = NDalicPINVOKE.Actor_Property_SIZE_get();
    public static readonly int SIZE_WIDTH = NDalicPINVOKE.Actor_Property_SIZE_WIDTH_get();
    public static readonly int SIZE_HEIGHT = NDalicPINVOKE.Actor_Property_SIZE_HEIGHT_get();
    public static readonly int SIZE_DEPTH = NDalicPINVOKE.Actor_Property_SIZE_DEPTH_get();
    public static readonly int POSITION = NDalicPINVOKE.Actor_Property_POSITION_get();
    public static readonly int POSITION_X = NDalicPINVOKE.Actor_Property_POSITION_X_get();
    public static readonly int POSITION_Y = NDalicPINVOKE.Actor_Property_POSITION_Y_get();
    public static readonly int POSITION_Z = NDalicPINVOKE.Actor_Property_POSITION_Z_get();
    public static readonly int WORLD_POSITION = NDalicPINVOKE.Actor_Property_WORLD_POSITION_get();
    public static readonly int WORLD_POSITION_X = NDalicPINVOKE.Actor_Property_WORLD_POSITION_X_get();
    public static readonly int WORLD_POSITION_Y = NDalicPINVOKE.Actor_Property_WORLD_POSITION_Y_get();
    public static readonly int WORLD_POSITION_Z = NDalicPINVOKE.Actor_Property_WORLD_POSITION_Z_get();
    public static readonly int ORIENTATION = NDalicPINVOKE.Actor_Property_ORIENTATION_get();
    public static readonly int WORLD_ORIENTATION = NDalicPINVOKE.Actor_Property_WORLD_ORIENTATION_get();
    public static readonly int SCALE = NDalicPINVOKE.Actor_Property_SCALE_get();
    public static readonly int SCALE_X = NDalicPINVOKE.Actor_Property_SCALE_X_get();
    public static readonly int SCALE_Y = NDalicPINVOKE.Actor_Property_SCALE_Y_get();
    public static readonly int SCALE_Z = NDalicPINVOKE.Actor_Property_SCALE_Z_get();
    public static readonly int WORLD_SCALE = NDalicPINVOKE.Actor_Property_WORLD_SCALE_get();
    public static readonly int VISIBLE = NDalicPINVOKE.Actor_Property_VISIBLE_get();
    public static readonly int COLOR = NDalicPINVOKE.Actor_Property_COLOR_get();
    public static readonly int COLOR_RED = NDalicPINVOKE.Actor_Property_COLOR_RED_get();
    public static readonly int COLOR_GREEN = NDalicPINVOKE.Actor_Property_COLOR_GREEN_get();
    public static readonly int COLOR_BLUE = NDalicPINVOKE.Actor_Property_COLOR_BLUE_get();
    public static readonly int COLOR_ALPHA = NDalicPINVOKE.Actor_Property_COLOR_ALPHA_get();
    public static readonly int WORLD_COLOR = NDalicPINVOKE.Actor_Property_WORLD_COLOR_get();
    public static readonly int WORLD_MATRIX = NDalicPINVOKE.Actor_Property_WORLD_MATRIX_get();
    public static readonly int NAME = NDalicPINVOKE.Actor_Property_NAME_get();
    public static readonly int SENSITIVE = NDalicPINVOKE.Actor_Property_SENSITIVE_get();
    public static readonly int LEAVE_REQUIRED = NDalicPINVOKE.Actor_Property_LEAVE_REQUIRED_get();
    public static readonly int INHERIT_ORIENTATION = NDalicPINVOKE.Actor_Property_INHERIT_ORIENTATION_get();
    public static readonly int INHERIT_SCALE = NDalicPINVOKE.Actor_Property_INHERIT_SCALE_get();
    public static readonly int COLOR_MODE = NDalicPINVOKE.Actor_Property_COLOR_MODE_get();
    public static readonly int POSITION_INHERITANCE = NDalicPINVOKE.Actor_Property_POSITION_INHERITANCE_get();
    public static readonly int DRAW_MODE = NDalicPINVOKE.Actor_Property_DRAW_MODE_get();
    public static readonly int SIZE_MODE_FACTOR = NDalicPINVOKE.Actor_Property_SIZE_MODE_FACTOR_get();
    public static readonly int WIDTH_RESIZE_POLICY = NDalicPINVOKE.Actor_Property_WIDTH_RESIZE_POLICY_get();
    public static readonly int HEIGHT_RESIZE_POLICY = NDalicPINVOKE.Actor_Property_HEIGHT_RESIZE_POLICY_get();
    public static readonly int SIZE_SCALE_POLICY = NDalicPINVOKE.Actor_Property_SIZE_SCALE_POLICY_get();
    public static readonly int WIDTH_FOR_HEIGHT = NDalicPINVOKE.Actor_Property_WIDTH_FOR_HEIGHT_get();
    public static readonly int HEIGHT_FOR_WIDTH = NDalicPINVOKE.Actor_Property_HEIGHT_FOR_WIDTH_get();
    public static readonly int PADDING = NDalicPINVOKE.Actor_Property_PADDING_get();
    public static readonly int MINIMUM_SIZE = NDalicPINVOKE.Actor_Property_MINIMUM_SIZE_get();
    public static readonly int MAXIMUM_SIZE = NDalicPINVOKE.Actor_Property_MAXIMUM_SIZE_get();
    public static readonly int INHERIT_POSITION = NDalicPINVOKE.Actor_Property_INHERIT_POSITION_get();
    public static readonly int CLIPPING_MODE = NDalicPINVOKE.Actor_Property_CLIPPING_MODE_get();
    public static readonly int BATCH_PARENT = NDalicPINVOKE.Actor_Property_BATCH_PARENT_get();


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
