/*
 * Copyright (c) 2017 Samsung Electronics Co., Ltd.
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

%typemap(csimports) Dali::Window %{
    using System;
    using System.Runtime.InteropServices;
%}


%typemap(cscode) Dali::Window %{

    public void SetAcceptFocus( bool accept )
    {
      NDalicPINVOKE.SetAcceptFocus(swigCPtr, accept);
      if (NDalicPINVOKE.SWIGPendingException.Pending) throw NDalicPINVOKE.SWIGPendingException.Retrieve();
    }

    public bool IsFocusAcceptable()
    {
      return NDalicPINVOKE.IsFocusAcceptable(swigCPtr);
      if (NDalicPINVOKE.SWIGPendingException.Pending) throw NDalicPINVOKE.SWIGPendingException.Retrieve();
    }

    public void Show()
    {
      NDalicPINVOKE.Show(swigCPtr);
      if (NDalicPINVOKE.SWIGPendingException.Pending) throw NDalicPINVOKE.SWIGPendingException.Retrieve();
    }

    public void Hide()
    {
      NDalicPINVOKE.Hide(swigCPtr);
      if (NDalicPINVOKE.SWIGPendingException.Pending) throw NDalicPINVOKE.SWIGPendingException.Retrieve();
    }

    public void IsVisible()
    {
      NDalicPINVOKE.IsVisible(swigCPtr);
      if (NDalicPINVOKE.SWIGPendingException.Pending) throw NDalicPINVOKE.SWIGPendingException.Retrieve();
    }

    public class WindowFocusChangedEventArgs : EventArgs
    {
        public bool FocusGained
        {
            get;
            set;
        }
    }

    private WindowFocusChangedEventCallbackType _windowFocusChangedEventCallback;
    [UnmanagedFunctionPointer(CallingConvention.StdCall)]
    private delegate void WindowFocusChangedEventCallbackType(bool focusGained);
    private event EventHandler<WindowFocusChangedEventArgs> _windowFocusChangedEventHandler;

    public event EventHandler<WindowFocusChangedEventArgs> WindowFocusChanged
    {
        add
        {
            if (_windowFocusChangedEventHandler == null)
            {
                _windowFocusChangedEventCallback = OnWindowFocusedChanged;
                WindowFocusChangedSignal().Connect(_windowFocusChangedEventCallback);
            }

            _windowFocusChangedEventHandler += value;
        }
        remove
        {
            _windowFocusChangedEventHandler -= value;

            if (_windowFocusChangedEventHandler == null && _windowFocusChangedEventCallback != null)
            {
                WindowFocusChangedSignal().Disconnect(_windowFocusChangedEventCallback);
            }
        }
    }

    private void OnWindowFocusedChanged(bool focusGained)
    {
        WindowFocusChangedEventArgs e = new WindowFocusChangedEventArgs();

        e.FocusGained = focusGained;

        if (_windowFocusChangedEventHandler != null)
        {
            _windowFocusChangedEventHandler(this, e);
        }
    }

    public WindowFocusSignalType WindowFocusChangedSignal() {
      WindowFocusSignalType ret = new WindowFocusSignalType(NDalicPINVOKE.FocusChangedSignal(swigCPtr), false);
      if (NDalicPINVOKE.SWIGPendingException.Pending) throw NDalicPINVOKE.SWIGPendingException.Retrieve();
      return ret;
    }

%}

%ignore Dali::Window::IndicatorVisibilityChangedSignal();
