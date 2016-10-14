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

%define TEXTFIELD_EVENTHANDLER_TYPEMAP_EVENTARG(NameSpace, ClassName)
%typemap(csimports) NameSpace::ClassName %{
using System;
using System.Runtime.InteropServices;

%}

%enddef

%define TEXTFIELD_EVENTHANDLER_TYPEMAP_HELPER(NameSpace, ClassName)
%typemap(cscode) NameSpace::ClassName %{

public class TextChangedEventArgs : EventArgs
{
   private TextField _textField;

   public TextField TextField
   {
      get
      {
         return _textField;
      }
      set
      {
         _textField = value;
      }
   }
}

public class MaxLengthReachedEventArgs : EventArgs
{
   private TextField _textField;

   public TextField TextField
   {
      get
      {
         return _textField;
      }
      set
      {
         _textField = value;
      }
   }
}


  [UnmanagedFunctionPointer(CallingConvention.StdCall)]
  private delegate void TextChangedCallbackDelegate(IntPtr textField);
  private DaliEventHandler<object,TextChangedEventArgs> _textFieldTextChangedEventHandler;
  private TextChangedCallbackDelegate _textFieldTextChangedCallbackDelegate;
  
  [UnmanagedFunctionPointer(CallingConvention.StdCall)]
  private delegate void MaxLengthReachedCallbackDelegate(IntPtr textField);
  private DaliEventHandler<object,MaxLengthReachedEventArgs> _textFieldMaxLengthReachedEventHandler;
  private MaxLengthReachedCallbackDelegate _textFieldMaxLengthReachedCallbackDelegate;

  public event DaliEventHandler<object,TextChangedEventArgs> TextChanged
  {
     add
     {
        lock(this)
        {
           // Restricted to only one listener
           if (_textFieldTextChangedEventHandler == null)
           {
              _textFieldTextChangedEventHandler += value;

              _textFieldTextChangedCallbackDelegate = new TextChangedCallbackDelegate(OnTextChanged);
              this.TextChangedSignal().Connect(_textFieldTextChangedCallbackDelegate);
           }
        }
     }

     remove
     {
        lock(this)
        {
           if (_textFieldTextChangedEventHandler != null)
           {
              this.TextChangedSignal().Disconnect(_textFieldTextChangedCallbackDelegate);
           }

           _textFieldTextChangedEventHandler -= value;
        }
     }
  }

 private void OnTextChanged(IntPtr textField)
  {
   TextChangedEventArgs e = new TextChangedEventArgs();

   // Populate all members of "e" (TextChangedEventArgs) with real data
   e.TextField = Dali.TextField.GetTextFieldFromPtr(textField);

   if (_textFieldTextChangedEventHandler != null)
   {
      //here we send all data to user event handlers
      _textFieldTextChangedEventHandler(this, e);
   }

  }

  public event DaliEventHandler<object,MaxLengthReachedEventArgs> MaxLengthReached
  {
     add
     {
        lock(this)
        {
           // Restricted to only one listener
           if (_textFieldMaxLengthReachedEventHandler == null)
           {
              _textFieldMaxLengthReachedEventHandler += value;

              _textFieldMaxLengthReachedCallbackDelegate = new MaxLengthReachedCallbackDelegate(OnMaxLengthReached);
              this.MaxLengthReachedSignal().Connect(_textFieldMaxLengthReachedCallbackDelegate);
           }
        }
     }

     remove
     {
        lock(this)
        {
           if (_textFieldMaxLengthReachedEventHandler != null)
           {
              this.MaxLengthReachedSignal().Disconnect(_textFieldMaxLengthReachedCallbackDelegate);
           }

           _textFieldMaxLengthReachedEventHandler -= value;
        }
     }
  }

 private void OnMaxLengthReached(IntPtr textField)
  {
   MaxLengthReachedEventArgs e = new MaxLengthReachedEventArgs();

   // Populate all members of "e" (MaxLengthReachedEventArgs) with real data
   e.TextField = Dali.TextField.GetTextFieldFromPtr(textField);

   if (_textFieldMaxLengthReachedEventHandler != null)
   {
      //here we send all data to user event handlers
      _textFieldMaxLengthReachedEventHandler(this, e);
   }

  }

 public static ClassName Get ## ClassName ## FromPtr(global::System.IntPtr cPtr) {
   ClassName ret = new ClassName(cPtr, false);
   if (NDalicPINVOKE.SWIGPendingException.Pending) throw NDalicPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

%}

%enddef

%define DALI_TEXTFIELD_EVENTHANDLER_PARAM( NameSpace, ClassName)

  TEXTFIELD_EVENTHANDLER_TYPEMAP_EVENTARG( NameSpace, ClassName);
  TEXTFIELD_EVENTHANDLER_TYPEMAP_HELPER( NameSpace, ClassName);

%enddef

namespace Dali
{
  DALI_TEXTFIELD_EVENTHANDLER_PARAM( Dali::Toolkit, TextField);
}
