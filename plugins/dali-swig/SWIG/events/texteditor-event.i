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

%define TEXTEDITOR_EVENTHANDLER_TYPEMAP_EVENTARG(NameSpace, ClassName)
%typemap(csimports) NameSpace::ClassName %{
using System;
using System.Runtime.InteropServices;

%}

%enddef

%define TEXTEDITOR_EVENTHANDLER_TYPEMAP_HELPER(NameSpace, ClassName)
%typemap(cscode) NameSpace::ClassName %{

/**
  * @brief Event arguments that passed via TextChanged signal
  *
  */
public class TextChangedEventArgs : EventArgs
{
   private TextEditor _textEditor;
   /**
     * @brief TextEditor - is the texteditor control which has the text contents changed.
     *
     */
   public TextEditor TextEditor
   {
      get
      {
         return _textEditor;
      }
      set
      {
         _textEditor = value;
      }
   }
}

  [UnmanagedFunctionPointer(CallingConvention.StdCall)]
  private delegate void TextChangedCallbackDelegate(IntPtr textEditor);
  private DaliEventHandler<object,TextChangedEventArgs> _textEditorTextChangedEventHandler;
  private TextChangedCallbackDelegate _textEditorTextChangedCallbackDelegate;

  /**
    * @brief Event for TextChanged signal which can be used to subscribe/unsubscribe the event handler
    * (in the type of TextChangedEventHandler-DaliEventHandler<object,TextChangedEventArgs>) 
    * provided by the user. TextChanged signal is emitted when the text changes.
    */
  public event DaliEventHandler<object,TextChangedEventArgs> TextChanged
  {
     add
     {
        lock(this)
        {
           // Restricted to only one listener
           if (_textEditorTextChangedEventHandler == null)
           {
              _textEditorTextChangedEventHandler += value;

              _textEditorTextChangedCallbackDelegate = new TextChangedCallbackDelegate(OnTextChanged);
              this.TextChangedSignal().Connect(_textEditorTextChangedCallbackDelegate);
           }
        }
     }

     remove
     {
        lock(this)
        {
           if (_textEditorTextChangedEventHandler != null)
           {
              this.TextChangedSignal().Disconnect(_textEditorTextChangedCallbackDelegate);
           }

           _textEditorTextChangedEventHandler -= value;
        }
     }
  }

 private void OnTextChanged(IntPtr textEditor)
  {
   TextChangedEventArgs e = new TextChangedEventArgs();

   // Populate all members of "e" (TextChangedEventArgs) with real data
   e.TextEditor = Dali.TextEditor.GetTextEditorFromPtr(textEditor);

   if (_textEditorTextChangedEventHandler != null)
   {
      //here we send all data to user event handlers
      _textEditorTextChangedEventHandler(this, e);
   }

  }

 public static ClassName Get ## ClassName ## FromPtr(global::System.IntPtr cPtr) {
    ClassName ret = new ClassName(cPtr, false);
   if (NDalicPINVOKE.SWIGPendingException.Pending) throw NDalicPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

%}

%enddef

%define DALI_TEXTEDITOR_EVENTHANDLER_PARAM( NameSpace, ClassName)

  TEXTEDITOR_EVENTHANDLER_TYPEMAP_EVENTARG( NameSpace, ClassName);
  TEXTEDITOR_EVENTHANDLER_TYPEMAP_HELPER( NameSpace, ClassName);

%enddef

namespace Dali
{
  DALI_TEXTEDITOR_EVENTHANDLER_PARAM( Dali::Toolkit, TextEditor);
}
