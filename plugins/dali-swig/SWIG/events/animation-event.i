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


    private EventCallbackDelegateType1<IntPtr> _animationFinishedEventCallbackDelegate;
    private event EventHandler _animationFinishedEventHandler;

    /**
      * @brief Event for Finished signal which can be used to subscribe/unsubscribe the event handler
      * Finished signal is emitted when an Animation's animations have finished.
      */
    public event EventHandler Finished
    {
      add
      {
        lock(this)
        {
          _animationFinishedEventHandler += value;
          _animationFinishedEventCallbackDelegate = OnFinished;
          this.FinishedSignal().Connect(_animationFinishedEventCallbackDelegate);
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
      if (_animationFinishedEventHandler != null)
      {
        //here we send all data to user event handlers
        _animationFinishedEventHandler(this, null);
      }
    }

  public static ClassName Get ## ClassName ## FromPtr(global::System.IntPtr cPtr) {
    ClassName ret = new ClassName(cPtr, false);
    if (NDalicPINVOKE.SWIGPendingException.Pending) throw NDalicPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  private float MilliSecondsToSeconds( int millisec )
  {
    return (float)millisec / 1000.0f;
  }

  private int SecondsToMilliSeconds( float sec )
  {
    return (int)( sec * 1000 );
  }

  public int Duration
  {
    set
    {
      SetDuration( MilliSecondsToSeconds( value ) );
    }
    get
    {
      return SecondsToMilliSeconds( GetDuration() );
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

  public Dali.Constants.Animation.State Status
  {
    get
    {
       return (Dali.Constants.Animation.State)GetState();
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

  public Dali.Constants.Animation.EndAction EndAction
  {
    set
    {
        switch(value)
        {
          case Dali.Constants.Animation.EndAction.Cancel :
            SetEndAction(Dali.Animation.DaliEndAction.Bake);
            break;
          case Dali.Constants.Animation.EndAction.Discard :
            SetEndAction(Dali.Animation.DaliEndAction.Discard);
            break;
          case Dali.Constants.Animation.EndAction.Stop :
            SetEndAction(Dali.Animation.DaliEndAction.BakeFinal);
            break;
          default :
            SetEndAction(Dali.Animation.DaliEndAction.Bake);
            break;
        }
    }
    get
    {
        Dali.Animation.DaliEndAction temp = GetEndAction();
        switch(temp)
        {
          case Dali.Animation.DaliEndAction.Bake :
            return Dali.Constants.Animation.EndAction.Cancel;
          case Dali.Animation.DaliEndAction.Discard :
            return Dali.Constants.Animation.EndAction.Discard;
          case Dali.Animation.DaliEndAction.BakeFinal :
            return Dali.Constants.Animation.EndAction.Stop;
          default :
            return Dali.Constants.Animation.EndAction.Cancel;
        }
    }
  }

  public void Stop(Dali.Constants.Animation.EndAction action) {
    switch( action )
    {
      case Dali.Constants.Animation.EndAction.Cancel :
        SetEndAction(Dali.Animation.DaliEndAction.Bake);
        break;
      case Dali.Constants.Animation.EndAction.Discard :
        SetEndAction(Dali.Animation.DaliEndAction.Discard);
        break;
      case Dali.Constants.Animation.EndAction.Stop :
        SetEndAction(Dali.Animation.DaliEndAction.BakeFinal);
        break;
      default :
        SetEndAction(Dali.Animation.DaliEndAction.Bake);
        break;
    }
    NDalicPINVOKE.Animation_Stop(swigCPtr);
    if (NDalicPINVOKE.SWIGPendingException.Pending) throw NDalicPINVOKE.SWIGPendingException.Retrieve();
  }

  public int StartTime { set; get; }
  public int EndTime { set; get; }
  public string TargetPoperty { set; get; }
  public object Destination { set; get; }
  public Dali.AlphaFunction AlphaFunction { set; get; }


  public void AnimateBy(Actor target)
  {
    string _str1 = TargetPoperty.Substring(0, 1);
    string _str2 = TargetPoperty.Substring(1);
    string _str = _str1.ToLower() + _str2;

    dynamic obj = (object)Destination;

    if( this.AlphaFunction != null )
    {
      if( this.StartTime == 0 && this.EndTime == 0 )
      {
        AnimateBy(new Property(target, _str), new Property.Value(obj), this.AlphaFunction );
      }
      else
      {
        Dali.TimePeriod time = new Dali.TimePeriod( MilliSecondsToSeconds( this.StartTime ), MilliSecondsToSeconds( this.EndTime ) );
        AnimateBy(new Property(target, _str), new Property.Value(obj), this.AlphaFunction, time );
      }
    }
    else
    {
      if( this.StartTime == 0 && this.EndTime == 0 )
      {
        AnimateBy(new Property(target, _str), new Property.Value(obj) );
      }
      else
      {
        Dali.TimePeriod time = new Dali.TimePeriod( MilliSecondsToSeconds( this.StartTime ), MilliSecondsToSeconds( this.EndTime ) );
        AnimateBy(new Property(target, _str), new Property.Value(obj), time );
      }
    }
  }

  public void AnimateBy(Actor target, string propertyIndex)
  {
    string _str1 = propertyIndex.Substring(0, 1);
    string _str2 = propertyIndex.Substring(1);
    string _str = _str1.ToLower() + _str2;

    dynamic obj = (object)Destination;

    if( this.AlphaFunction != null )
    {
      if( this.StartTime == 0 && this.EndTime == 0 )
      {
        AnimateBy(new Property(target, _str), new Property.Value(obj), this.AlphaFunction );
      }
      else
      {
        Dali.TimePeriod time = new Dali.TimePeriod( MilliSecondsToSeconds( this.StartTime ), MilliSecondsToSeconds( this.EndTime ) );
        AnimateBy(new Property(target, _str), new Property.Value(obj), this.AlphaFunction, time );
      }
    }
    else
    {
      if( this.StartTime == 0 && this.EndTime == 0 )
      {
        AnimateBy(new Property(target, _str), new Property.Value(obj) );
      }
      else
      {
        Dali.TimePeriod time = new Dali.TimePeriod( MilliSecondsToSeconds( this.StartTime ), MilliSecondsToSeconds( this.EndTime ) );
        AnimateBy(new Property(target, _str), new Property.Value(obj), time );
      }
    }
  }

  public void AnimateBy(Actor target, string propertyIndex, object relativeValue)
  {
    string _str1 = propertyIndex.Substring(0, 1);
    string _str2 = propertyIndex.Substring(1);
    string _str = _str1.ToLower() + _str2;

    dynamic obj = (object)relativeValue;

    if( this.AlphaFunction != null )
    {
      if( this.StartTime == 0 && this.EndTime == 0 )
      {
        AnimateBy(new Property(target, _str), new Property.Value(obj), this.AlphaFunction );
      }
      else
      {
        Dali.TimePeriod time = new Dali.TimePeriod( MilliSecondsToSeconds( this.StartTime ), MilliSecondsToSeconds( this.EndTime ) );
        AnimateBy(new Property(target, _str), new Property.Value(obj), this.AlphaFunction, time );
      }
    }
    else
    {
      if( this.StartTime == 0 && this.EndTime == 0 )
      {
        AnimateBy(new Property(target, _str), new Property.Value(obj) );
      }
      else
      {
        Dali.TimePeriod time = new Dali.TimePeriod( MilliSecondsToSeconds( this.StartTime ), MilliSecondsToSeconds( this.EndTime ) );
        AnimateBy(new Property(target, _str), new Property.Value(obj), time );
      }
    }
  }

  public void AnimateTo(Actor target)
  {
    string _str1 = TargetPoperty.Substring(0, 1);
    string _str2 = TargetPoperty.Substring(1);
    string _str = _str1.ToLower() + _str2;

    dynamic obj = (object)Destination;

    if( this.AlphaFunction != null )
    {
      if( this.StartTime == 0 && this.EndTime == 0 )
      {
        AnimateTo(new Property(target, _str), new Property.Value(obj), this.AlphaFunction );
      }
      else
      {
        Dali.TimePeriod time = new Dali.TimePeriod( MilliSecondsToSeconds( this.StartTime ), MilliSecondsToSeconds( this.EndTime ) );
        AnimateTo(new Property(target, _str), new Property.Value(obj), this.AlphaFunction, time );
      }
    }
    else
    {
      if( this.StartTime == 0 && this.EndTime == 0 )
      {
        AnimateTo(new Property(target, _str), new Property.Value(obj) );
      }
      else
      {
        Dali.TimePeriod time = new Dali.TimePeriod( MilliSecondsToSeconds( this.StartTime ), MilliSecondsToSeconds( this.EndTime ) );
        AnimateTo(new Property(target, _str), new Property.Value(obj), time );
      }
    }
  }

  public void AnimateTo(Actor target, string propertyIndex)
  {
    string _str1 = propertyIndex.Substring(0, 1);
    string _str2 = propertyIndex.Substring(1);
    string _str = _str1.ToLower() + _str2;

    dynamic obj = (object)Destination;

    if( this.AlphaFunction != null )
    {
      if( this.StartTime == 0 && this.EndTime == 0 )
      {
        AnimateTo(new Property(target, _str), new Property.Value(obj), this.AlphaFunction );
      }
      else
      {
        Dali.TimePeriod time = new Dali.TimePeriod( MilliSecondsToSeconds( this.StartTime ), MilliSecondsToSeconds( this.EndTime ) );
        AnimateTo(new Property(target, _str), new Property.Value(obj), this.AlphaFunction, time );
      }
    }
    else
    {
      if( this.StartTime == 0 && this.EndTime == 0 )
      {
        AnimateTo(new Property(target, _str), new Property.Value(obj) );
      }
      else
      {
        Dali.TimePeriod time = new Dali.TimePeriod( MilliSecondsToSeconds( this.StartTime ), MilliSecondsToSeconds( this.EndTime ) );
        AnimateTo(new Property(target, _str), new Property.Value(obj), time );
      }
    }
  }

  public void AnimateTo(Actor target, string propertyIndex, object destinationValue)
  {
    string _str1 = propertyIndex.Substring(0, 1);
    string _str2 = propertyIndex.Substring(1);
    string _str = _str1.ToLower() + _str2;

    dynamic obj = (object)destinationValue;

    if( this.AlphaFunction != null )
    {
      if( this.StartTime == 0 && this.EndTime == 0 )
      {
        AnimateTo(new Property(target, _str), new Property.Value(obj), this.AlphaFunction );
      }
      else
      {
        Dali.TimePeriod time = new Dali.TimePeriod( MilliSecondsToSeconds( this.StartTime ), MilliSecondsToSeconds( this.EndTime ) );
        AnimateTo(new Property(target, _str), new Property.Value(obj), this.AlphaFunction, time );
      }
    }
    else
    {
      if( this.StartTime == 0 && this.EndTime == 0 )
      {
        AnimateTo(new Property(target, _str), new Property.Value(obj) );
      }
      else
      {
        Dali.TimePeriod time = new Dali.TimePeriod( MilliSecondsToSeconds( this.StartTime ), MilliSecondsToSeconds( this.EndTime ) );
        AnimateTo(new Property(target, _str), new Property.Value(obj), time );
      }
    }
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

  public Animation () : this (NDalicPINVOKE.Animation_New( 0.0f ), true ) {
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
