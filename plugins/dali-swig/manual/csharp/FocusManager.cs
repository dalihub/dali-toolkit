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

namespace Dali {

using System;
using System.Runtime.InteropServices;

public class FocusManager : BaseHandle {
  private global::System.Runtime.InteropServices.HandleRef swigCPtr;

  internal FocusManager(global::System.IntPtr cPtr, bool cMemoryOwn) : base(NDalicManualPINVOKE.KeyboardFocusManager_SWIGUpcast(cPtr), cMemoryOwn) {
    swigCPtr = new global::System.Runtime.InteropServices.HandleRef(this, cPtr);
  }

  internal static global::System.Runtime.InteropServices.HandleRef getCPtr(FocusManager obj) {
    return (obj == null) ? new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero) : obj.swigCPtr;
  }

  ~FocusManager() {
    DisposeQueue.Instance.Add(this);
  }

  public override void Dispose() {
    if (!Stage.IsInstalled()) {
      DisposeQueue.Instance.Add(this);
      return;
    }

    lock(this) {
      if (swigCPtr.Handle != global::System.IntPtr.Zero) {
        if (swigCMemOwn) {
          swigCMemOwn = false;
          NDalicManualPINVOKE.delete_KeyboardFocusManager(swigCPtr);
        }
        swigCPtr = new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero);
      }
      global::System.GC.SuppressFinalize(this);
      base.Dispose();
    }
  }


/**
  * @brief Event arguments that passed via FocusChanged signal
  *
  */
public class FocusChangedEventArgs : EventArgs
{
   private Actor _actorCurrent;
   private Actor _actorNext;

   /**
     * @brief Actor - is the original focused Actor
     *
     */
   public Actor ActorCurrent
   {
      get
      {
         return _actorCurrent;
      }
      set
      {
         _actorCurrent = value;
      }
   }

   /**
     * @brief Actor - is the current focused Actor
     *
     */
   public Actor ActorNext
   {
      get
      {
         return _actorNext;
      }
      set
      {
         _actorNext = value;
      }
   }
}

/**
  * @brief Event arguments that passed via FocusGroupChanged signal
  *
  */
public class FocusGroupChangedEventArgs : EventArgs
{
   private Actor _currentFocusedActor;
   private bool _forwardDirection;

   /**
     * @brief Actor - is the current focused Actor
     *
     */
   public Actor CurrentFocusedActor
   {
      get
      {
         return _currentFocusedActor;
      }
      set
      {
         _currentFocusedActor = value;
      }
   }

   /**
     * @brief ForwardDirection - is the direction (forward or backward) in which to move the focus next
     *
     */
   public bool ForwardDirection
   {
      get
      {
         return _forwardDirection;
      }
      set
      {
         _forwardDirection = value;
      }
   }
}

/**
  * @brief Event arguments that passed via FocusedActorEnterKey signal
  *
  */
public class FocusedActorEnterKeyEventArgs : EventArgs
{
   private Actor _actor;

   /**
     * @brief Actor - is the current focused Actor which has the enter key pressed on it.
     *
     */
   public Actor Actor
   {
      get
      {
         return _actor;
      }
      set
      {
         _actor = value;
      }
   }
}

/**
  * @brief Event arguments that passed via PreFocusChange signal
  *
  */
public class PreFocusChangeEventArgs : EventArgs
{
   private Actor _current;
   private Actor _proposed;
   private View.KeyboardFocus.Direction _direction;

   /**
    * @brief Actor - is the current focused Actor.
    *
    */
   public Actor Current
   {
      get
      {
         return _current;
      }
      set
      {
          _current = value;
      }
   }

    /**
    * @brief Actor - is the proposed focused Actor.
    *
    */
    public Actor Proposed
    {
        get
        {
            return _proposed;
        }
        set
        {
            _proposed = value;
        }
    }

    /**
    * @brief Direction - is the direction of Focus change.
    *
    */
    public View.KeyboardFocus.Direction Direction
    {
        get
        {
            return _direction;
        }
        set
        {
            _direction = value;
        }
    }
}

  [UnmanagedFunctionPointer(CallingConvention.StdCall)]
  public delegate void FocusChangedEventHandler(object source, FocusChangedEventArgs e);

  [UnmanagedFunctionPointer(CallingConvention.StdCall)]
  public delegate void FocusGroupChangedEventHandler(object source, FocusGroupChangedEventArgs e);

  [UnmanagedFunctionPointer(CallingConvention.StdCall)]
  public delegate void FocusedActorEnterKeyEventHandler(object source, FocusedActorEnterKeyEventArgs e);

  [UnmanagedFunctionPointer(CallingConvention.StdCall)]
  public delegate Actor PreFocusChangeEventHandler(object source, PreFocusChangeEventArgs e);

  [UnmanagedFunctionPointer(CallingConvention.StdCall)]
  public delegate IntPtr PreFocusChangeEventCallbackDelegate(IntPtr current, IntPtr proposed, View.KeyboardFocus.Direction direction);
  private PreFocusChangeEventHandler _keyboardFocusManagerPreFocusChangeEventHandler;
  private PreFocusChangeEventCallbackDelegate _keyboardFocusManagerPreFocusChangeEventCallbackDelegate;

  [UnmanagedFunctionPointer(CallingConvention.StdCall)]
  private delegate void FocusChangedEventCallbackDelegate(IntPtr actorCurrent, IntPtr actorNext);
  private FocusChangedEventHandler _keyboardFocusManagerFocusChangedEventHandler;
  private FocusChangedEventCallbackDelegate _keyboardFocusManagerFocusChangedEventCallbackDelegate;

  [UnmanagedFunctionPointer(CallingConvention.StdCall)]
  private delegate void FocusGroupChangedEventCallbackDelegate(IntPtr currentFocusedActor, bool forwardDirection);
  private FocusGroupChangedEventHandler _keyboardFocusManagerFocusGroupChangedEventHandler;
  private FocusGroupChangedEventCallbackDelegate _keyboardFocusManagerFocusGroupChangedEventCallbackDelegate;

  [UnmanagedFunctionPointer(CallingConvention.StdCall)]
  private delegate void FocusedActorEnterKeyEventCallbackDelegate(IntPtr actor);
  private FocusedActorEnterKeyEventHandler _keyboardFocusManagerFocusedActorEnterKeyEventHandler;
  private FocusedActorEnterKeyEventCallbackDelegate _keyboardFocusManagerFocusedActorEnterKeyEventCallbackDelegate;

  public event PreFocusChangeEventHandler PreFocusChange
  {
     add
     {
        lock(this)
        {
           // Restricted to only one listener
           if (_keyboardFocusManagerPreFocusChangeEventHandler == null)
           {
               _keyboardFocusManagerPreFocusChangeEventHandler += value;

               _keyboardFocusManagerPreFocusChangeEventCallbackDelegate = new PreFocusChangeEventCallbackDelegate(OnPreFocusChange);
              this.PreFocusChangeSignal().Connect(_keyboardFocusManagerPreFocusChangeEventCallbackDelegate);
           }
        }
     }

     remove
     {
        lock(this)
        {
           if (_keyboardFocusManagerPreFocusChangeEventHandler != null)
           {
              this.PreFocusChangeSignal().Disconnect(_keyboardFocusManagerPreFocusChangeEventCallbackDelegate);
           }

           _keyboardFocusManagerPreFocusChangeEventHandler -= value;
        }
     }
  }

  // Callback for KeyboardFocusManager PreFocusChangeSignal
  private IntPtr OnPreFocusChange(IntPtr current, IntPtr proposed, View.KeyboardFocus.Direction direction)
  {
      Actor actor = null;
      PreFocusChangeEventArgs e = new PreFocusChangeEventArgs();

      // Populate all members of "e" (PreFocusChangeEventArgs) with real data
      e.Current = Actor.GetActorFromPtr(current);
      e.Proposed = Actor.GetActorFromPtr(proposed);
      e.Direction = direction;

      if (_keyboardFocusManagerPreFocusChangeEventHandler != null)
      {
        //here we send all data to user event handlers
          actor = _keyboardFocusManagerPreFocusChangeEventHandler(this, e);
      }

      return actor.GetPtrfromActor();
  }

  /**
    * @brief Event for FocusChanged signal which can be used to subscribe/unsubscribe the event handler
    * (in the type of FocusChangedEventHandler) provided by the user.
    * FocusChanged signal is emitted after the current focused actor has been changed.
    */
  public event FocusChangedEventHandler FocusChanged
  {
     add
     {
        lock(this)
        {
           // Restricted to only one listener
           if (_keyboardFocusManagerFocusChangedEventHandler == null)
           {
              _keyboardFocusManagerFocusChangedEventHandler += value;

              _keyboardFocusManagerFocusChangedEventCallbackDelegate = new FocusChangedEventCallbackDelegate(OnFocusChanged);
              this.FocusChangedSignal().Connect(_keyboardFocusManagerFocusChangedEventCallbackDelegate);
           }
        }
     }

     remove
     {
        lock(this)
        {
           if (_keyboardFocusManagerFocusChangedEventHandler != null)
           {
              this.FocusChangedSignal().Disconnect(_keyboardFocusManagerFocusChangedEventCallbackDelegate);
           }

           _keyboardFocusManagerFocusChangedEventHandler -= value;
        }
     }
  }

  // Callback for KeyboardFocusManager FocusChangedSignal
  private void OnFocusChanged(IntPtr actorCurrent, IntPtr actorNext)
  {
     FocusChangedEventArgs e = new FocusChangedEventArgs();

     // Populate all members of "e" (FocusChangedEventArgs) with real data
     e.ActorCurrent = Actor.GetActorFromPtr(actorCurrent);
     e.ActorNext = Actor.GetActorFromPtr(actorNext);

     if (_keyboardFocusManagerFocusChangedEventHandler != null)
     {
        //here we send all data to user event handlers
        _keyboardFocusManagerFocusChangedEventHandler(this, e);
     }
  }

  /**
    * @brief Event for FocusGroupChanged signal which can be used to subscribe/unsubscribe the event handler
    * (in the type of FocusGroupChangedEventHandler) provided by the user.
    * FocusGroupChanged signal is emitted when the focus group has been changed.
    */
  public event FocusGroupChangedEventHandler FocusGroupChanged
  {
     add
     {
        lock(this)
        {
           // Restricted to only one listener
           if (_keyboardFocusManagerFocusGroupChangedEventHandler == null)
           {
              _keyboardFocusManagerFocusGroupChangedEventHandler += value;

              _keyboardFocusManagerFocusGroupChangedEventCallbackDelegate = new FocusGroupChangedEventCallbackDelegate(OnFocusGroupChanged);
              this.FocusGroupChangedSignal().Connect(_keyboardFocusManagerFocusGroupChangedEventCallbackDelegate);
           }
        }
     }

     remove
     {
        lock(this)
        {
           if (_keyboardFocusManagerFocusGroupChangedEventHandler != null)
           {
              this.FocusGroupChangedSignal().Disconnect(_keyboardFocusManagerFocusGroupChangedEventCallbackDelegate);
           }

           _keyboardFocusManagerFocusGroupChangedEventHandler -= value;
        }
     }
  }

  // Callback for KeyboardFocusManager FocusGroupChangedSignal
  private void OnFocusGroupChanged(IntPtr currentFocusedActor, bool forwardDirection)
  {
     FocusGroupChangedEventArgs e = new FocusGroupChangedEventArgs();

     // Populate all members of "e" (FocusGroupChangedEventArgs) with real data
     e.CurrentFocusedActor = Actor.GetActorFromPtr(currentFocusedActor);
     e.ForwardDirection = forwardDirection;

     if (_keyboardFocusManagerFocusGroupChangedEventHandler != null)
     {
        //here we send all data to user event handlers
        _keyboardFocusManagerFocusGroupChangedEventHandler(this, e);
     }
  }

  /**
    * @brief Event for FocusedActorEnterKeyPressed signal which can be used to subscribe/unsubscribe the event handler
    * (in the type of FocusedActorEnterKeyEventHandler) provided by the user.
    * FocusedActorEnterKeyPressed signal is emitted when the current focused actor has the enter key pressed on it.
    */
  public event FocusedActorEnterKeyEventHandler FocusedActorEnterKeyPressed
  {
     add
     {
        lock(this)
        {
           // Restricted to only one listener
           if (_keyboardFocusManagerFocusedActorEnterKeyEventHandler == null)
           {
              _keyboardFocusManagerFocusedActorEnterKeyEventHandler += value;

              _keyboardFocusManagerFocusedActorEnterKeyEventCallbackDelegate = new FocusedActorEnterKeyEventCallbackDelegate(OnFocusedActorEnterKey);
              this.FocusedActorEnterKeySignal().Connect(_keyboardFocusManagerFocusedActorEnterKeyEventCallbackDelegate);
           }
        }
     }

     remove
     {
        lock(this)
        {
           if (_keyboardFocusManagerFocusedActorEnterKeyEventHandler != null)
           {
              this.FocusedActorEnterKeySignal().Disconnect(_keyboardFocusManagerFocusedActorEnterKeyEventCallbackDelegate);
           }

           _keyboardFocusManagerFocusedActorEnterKeyEventHandler -= value;
        }
     }
  }

  // Callback for KeyboardFocusManager FocusedActorEnterKeySignal
  private void OnFocusedActorEnterKey(IntPtr actor)
  {
     FocusedActorEnterKeyEventArgs e = new FocusedActorEnterKeyEventArgs();

     // Populate all members of "e" (FocusedActorEnterKeyEventArgs) with real data
     e.Actor = Actor.GetActorFromPtr(actor);

     if (_keyboardFocusManagerFocusedActorEnterKeyEventHandler != null)
     {
        //here we send all data to user event handlers
        _keyboardFocusManagerFocusedActorEnterKeyEventHandler(this, e);
     }
  }

  public FocusManager() : this(NDalicManualPINVOKE.new_KeyboardFocusManager(), true) {
    if (NDalicPINVOKE.SWIGPendingException.Pending) throw NDalicPINVOKE.SWIGPendingException.Retrieve();
  }

  public static FocusManager Get() {
    FocusManager ret = new FocusManager(NDalicManualPINVOKE.KeyboardFocusManager_Get(), true);
    if (NDalicPINVOKE.SWIGPendingException.Pending) throw NDalicPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public bool SetCurrentFocusActor(Actor actor) {
    bool ret = NDalicManualPINVOKE.KeyboardFocusManager_SetCurrentFocusActor(swigCPtr, Actor.getCPtr(actor));
    if (NDalicPINVOKE.SWIGPendingException.Pending) throw NDalicPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public Actor GetCurrentFocusActor() {
    Actor ret = new Actor(NDalicManualPINVOKE.KeyboardFocusManager_GetCurrentFocusActor(swigCPtr), true);
    if (NDalicPINVOKE.SWIGPendingException.Pending) throw NDalicPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public bool MoveFocus(View.KeyboardFocus.Direction direction) {
    bool ret = NDalicManualPINVOKE.KeyboardFocusManager_MoveFocus(swigCPtr, (int)direction);
    if (NDalicPINVOKE.SWIGPendingException.Pending) throw NDalicPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public void ClearFocus() {
    NDalicManualPINVOKE.KeyboardFocusManager_ClearFocus(swigCPtr);
    if (NDalicPINVOKE.SWIGPendingException.Pending) throw NDalicPINVOKE.SWIGPendingException.Retrieve();
  }

  public void SetFocusGroupLoop(bool enabled) {
    NDalicManualPINVOKE.KeyboardFocusManager_SetFocusGroupLoop(swigCPtr, enabled);
    if (NDalicPINVOKE.SWIGPendingException.Pending) throw NDalicPINVOKE.SWIGPendingException.Retrieve();
  }

  public bool GetFocusGroupLoop() {
    bool ret = NDalicManualPINVOKE.KeyboardFocusManager_GetFocusGroupLoop(swigCPtr);
    if (NDalicPINVOKE.SWIGPendingException.Pending) throw NDalicPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public void SetAsFocusGroup(Actor actor, bool isFocusGroup) {
    NDalicManualPINVOKE.KeyboardFocusManager_SetAsFocusGroup(swigCPtr, Actor.getCPtr(actor), isFocusGroup);
    if (NDalicPINVOKE.SWIGPendingException.Pending) throw NDalicPINVOKE.SWIGPendingException.Retrieve();
  }

  public bool IsFocusGroup(Actor actor) {
    bool ret = NDalicManualPINVOKE.KeyboardFocusManager_IsFocusGroup(swigCPtr, Actor.getCPtr(actor));
    if (NDalicPINVOKE.SWIGPendingException.Pending) throw NDalicPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public Actor GetFocusGroup(Actor actor) {
    Actor ret = new Actor(NDalicManualPINVOKE.KeyboardFocusManager_GetFocusGroup(swigCPtr, Actor.getCPtr(actor)), true);
    if (NDalicPINVOKE.SWIGPendingException.Pending) throw NDalicPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public void SetFocusIndicatorActor(Actor indicator) {
    NDalicManualPINVOKE.KeyboardFocusManager_SetFocusIndicatorActor(swigCPtr, Actor.getCPtr(indicator));
    if (NDalicPINVOKE.SWIGPendingException.Pending) throw NDalicPINVOKE.SWIGPendingException.Retrieve();
  }

  public Actor GetFocusIndicatorActor() {
    Actor ret = new Actor(NDalicManualPINVOKE.KeyboardFocusManager_GetFocusIndicatorActor(swigCPtr), true);
    if (NDalicPINVOKE.SWIGPendingException.Pending) throw NDalicPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public KeyboardPreFocusChangeSignal PreFocusChangeSignal() {
    KeyboardPreFocusChangeSignal ret = new KeyboardPreFocusChangeSignal(NDalicManualPINVOKE.KeyboardFocusManager_PreFocusChangeSignal(swigCPtr), false);
    if (NDalicPINVOKE.SWIGPendingException.Pending) throw NDalicPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public FocusChangedSignal FocusChangedSignal() {
    FocusChangedSignal ret = new FocusChangedSignal(NDalicManualPINVOKE.KeyboardFocusManager_FocusChangedSignal(swigCPtr), false);
    if (NDalicPINVOKE.SWIGPendingException.Pending) throw NDalicPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public FocusGroupChangedSignal FocusGroupChangedSignal() {
    FocusGroupChangedSignal ret = new FocusGroupChangedSignal(NDalicManualPINVOKE.KeyboardFocusManager_FocusGroupChangedSignal(swigCPtr), false);
    if (NDalicPINVOKE.SWIGPendingException.Pending) throw NDalicPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public ActorSignal FocusedActorEnterKeySignal() {
    ActorSignal ret = new ActorSignal(NDalicManualPINVOKE.KeyboardFocusManager_FocusedActorEnterKeySignal(swigCPtr), false);
    if (NDalicPINVOKE.SWIGPendingException.Pending) throw NDalicPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  private static readonly FocusManager instance = FocusManager.Get();

  public static FocusManager Instance
  {
      get
      {
          return instance;
      }
  }


}

}
