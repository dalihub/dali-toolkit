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

/* Properties earlier added by Ruby Script */

  public int RenderingBackend
  {
    get
    {
      int temp = 0;
      GetProperty( TextField.Property.RENDERING_BACKEND).Get( ref temp );
      return temp;
    }
    set
    {
      SetProperty( TextField.Property.RENDERING_BACKEND, new Dali.Property.Value( value ) );
    }
  }
  public string Text
  {
    get
    {
      string temp;
      GetProperty( TextField.Property.TEXT).Get( out temp );
      return temp;
    }
    set
    {
      SetProperty( TextField.Property.TEXT, new Dali.Property.Value( value ) );
    }
  }
  public string PlaceholderText
  {
    get
    {
      string temp;
      GetProperty( TextField.Property.PLACEHOLDER_TEXT).Get( out temp );
      return temp;
    }
    set
    {
      SetProperty( TextField.Property.PLACEHOLDER_TEXT, new Dali.Property.Value( value ) );
    }
  }
  public string PlaceholderTextFocused
  {
    get
    {
      string temp;
      GetProperty( TextField.Property.PLACEHOLDER_TEXT_FOCUSED).Get( out temp );
      return temp;
    }
    set
    {
      SetProperty( TextField.Property.PLACEHOLDER_TEXT_FOCUSED, new Dali.Property.Value( value ) );
    }
  }
  public string FontFamily
  {
    get
    {
      string temp;
      GetProperty( TextField.Property.FONT_FAMILY).Get( out temp );
      return temp;
    }
    set
    {
      SetProperty( TextField.Property.FONT_FAMILY, new Dali.Property.Value( value ) );
    }
  }
  public Dali.Property.Map FontStyle
  {
    get
    {
      Dali.Property.Map temp = new Dali.Property.Map();
      GetProperty( TextField.Property.FONT_STYLE).Get(  temp );
      return temp;
    }
    set
    {
      SetProperty( TextField.Property.FONT_STYLE, new Dali.Property.Value( value ) );
    }
  }
  public float PointSize
  {
    get
    {
      float temp = 0.0f;
      GetProperty( TextField.Property.POINT_SIZE).Get( ref temp );
      return temp;
    }
    set
    {
      SetProperty( TextField.Property.POINT_SIZE, new Dali.Property.Value( value ) );
    }
  }
  public int MaxLength
  {
    get
    {
      int temp = 0;
      GetProperty( TextField.Property.MAX_LENGTH).Get( ref temp );
      return temp;
    }
    set
    {
      SetProperty( TextField.Property.MAX_LENGTH, new Dali.Property.Value( value ) );
    }
  }
  public int ExceedPolicy
  {
    get
    {
      int temp = 0;
      GetProperty( TextField.Property.EXCEED_POLICY).Get( ref temp );
      return temp;
    }
    set
    {
      SetProperty( TextField.Property.EXCEED_POLICY, new Dali.Property.Value( value ) );
    }
  }
  public string HorizontalAlignment
  {
    get
    {
      string temp;
      GetProperty( TextField.Property.HORIZONTAL_ALIGNMENT).Get( out temp );
      return temp;
    }
    set
    {
      SetProperty( TextField.Property.HORIZONTAL_ALIGNMENT, new Dali.Property.Value( value ) );
    }
  }
  public string VerticalAlignment
  {
    get
    {
      string temp;
      GetProperty( TextField.Property.VERTICAL_ALIGNMENT).Get( out temp );
      return temp;
    }
    set
    {
      SetProperty( TextField.Property.VERTICAL_ALIGNMENT, new Dali.Property.Value( value ) );
    }
  }
  public Dali.CSharp.Color TextColor
  {
    get
    {
      Vector4 temp = new Vector4(0.0f,0.0f,0.0f,0.0f);
      GetProperty( TextField.Property.TEXT_COLOR).Get(  temp );
      Dali.CSharp.Color ret = new Dali.CSharp.Color(temp.r, temp.g, temp.b, temp.a);
      return ret;
    }
    set
    {
      SetProperty( TextField.Property.TEXT_COLOR, new Dali.Property.Value( value ) );
    }
  }
  public Dali.CSharp.Color PlaceholderTextColor
  {
    get
    {
      Vector4 temp = new Vector4(0.0f,0.0f,0.0f,0.0f);
      GetProperty( TextField.Property.PLACEHOLDER_TEXT_COLOR).Get(  temp );
      Dali.CSharp.Color ret = new Dali.CSharp.Color(temp.r, temp.g, temp.b, temp.a);
      return ret;
    }
    set
    {
      SetProperty( TextField.Property.PLACEHOLDER_TEXT_COLOR, new Dali.Property.Value( value ) );
    }
  }
  public Dali.CSharp.Size ShadowOffset
  {
    get
    {
      Vector2 temp = new Vector2(0.0f,0.0f);
      GetProperty( TextField.Property.SHADOW_OFFSET).Get(  temp );
      Dali.CSharp.Size ret = new Dali.CSharp.Size(temp.x, temp.y);
      return ret;
    }
    set
    {
      SetProperty( TextField.Property.SHADOW_OFFSET, new Dali.Property.Value( value ) );
    }
  }
  public Dali.CSharp.Color ShadowColor
  {
    get
    {
      Vector4 temp = new Vector4(0.0f,0.0f,0.0f,0.0f);
      GetProperty( TextField.Property.SHADOW_COLOR).Get(  temp );
      Dali.CSharp.Color ret = new Dali.CSharp.Color(temp.r, temp.g, temp.b, temp.a);
      return ret;
    }
    set
    {
      SetProperty( TextField.Property.SHADOW_COLOR, new Dali.Property.Value( value ) );
    }
  }
  public Dali.CSharp.Color PrimaryCursorColor
  {
    get
    {
      Vector4 temp = new Vector4(0.0f,0.0f,0.0f,0.0f);
      GetProperty( TextField.Property.PRIMARY_CURSOR_COLOR).Get(  temp );
      Dali.CSharp.Color ret = new Dali.CSharp.Color(temp.r, temp.g, temp.b, temp.a);
      return ret;
    }
    set
    {
      SetProperty( TextField.Property.PRIMARY_CURSOR_COLOR, new Dali.Property.Value( value ) );
    }
  }
  public Dali.CSharp.Color SecondaryCursorColor
  {
    get
    {
      Vector4 temp = new Vector4(0.0f,0.0f,0.0f,0.0f);
      GetProperty( TextField.Property.SECONDARY_CURSOR_COLOR).Get(  temp );
      Dali.CSharp.Color ret = new Dali.CSharp.Color(temp.r, temp.g, temp.b, temp.a);
      return ret;
    }
    set
    {
      SetProperty( TextField.Property.SECONDARY_CURSOR_COLOR, new Dali.Property.Value( value ) );
    }
  }
  public bool EnableCursorBlink
  {
    get
    {
      bool temp = false;
      GetProperty( TextField.Property.ENABLE_CURSOR_BLINK).Get( ref temp );
      return temp;
    }
    set
    {
      SetProperty( TextField.Property.ENABLE_CURSOR_BLINK, new Dali.Property.Value( value ) );
    }
  }
  public float CursorBlinkInterval
  {
    get
    {
      float temp = 0.0f;
      GetProperty( TextField.Property.CURSOR_BLINK_INTERVAL).Get( ref temp );
      return temp;
    }
    set
    {
      SetProperty( TextField.Property.CURSOR_BLINK_INTERVAL, new Dali.Property.Value( value ) );
    }
  }
  public float CursorBlinkDuration
  {
    get
    {
      float temp = 0.0f;
      GetProperty( TextField.Property.CURSOR_BLINK_DURATION).Get( ref temp );
      return temp;
    }
    set
    {
      SetProperty( TextField.Property.CURSOR_BLINK_DURATION, new Dali.Property.Value( value ) );
    }
  }
  public int CursorWidth
  {
    get
    {
      int temp = 0;
      GetProperty( TextField.Property.CURSOR_WIDTH).Get( ref temp );
      return temp;
    }
    set
    {
      SetProperty( TextField.Property.CURSOR_WIDTH, new Dali.Property.Value( value ) );
    }
  }
  public string GrabHandleImage
  {
    get
    {
      string temp;
      GetProperty( TextField.Property.GRAB_HANDLE_IMAGE).Get( out temp );
      return temp;
    }
    set
    {
      SetProperty( TextField.Property.GRAB_HANDLE_IMAGE, new Dali.Property.Value( value ) );
    }
  }
  public string GrabHandlePressedImage
  {
    get
    {
      string temp;
      GetProperty( TextField.Property.GRAB_HANDLE_PRESSED_IMAGE).Get( out temp );
      return temp;
    }
    set
    {
      SetProperty( TextField.Property.GRAB_HANDLE_PRESSED_IMAGE, new Dali.Property.Value( value ) );
    }
  }
  public float ScrollThreshold
  {
    get
    {
      float temp = 0.0f;
      GetProperty( TextField.Property.SCROLL_THRESHOLD).Get( ref temp );
      return temp;
    }
    set
    {
      SetProperty( TextField.Property.SCROLL_THRESHOLD, new Dali.Property.Value( value ) );
    }
  }
  public float ScrollSpeed
  {
    get
    {
      float temp = 0.0f;
      GetProperty( TextField.Property.SCROLL_SPEED).Get( ref temp );
      return temp;
    }
    set
    {
      SetProperty( TextField.Property.SCROLL_SPEED, new Dali.Property.Value( value ) );
    }
  }
  public Dali.Property.Map SelectionHandleImageLeft
  {
    get
    {
      Dali.Property.Map temp = new Dali.Property.Map();
      GetProperty( TextField.Property.SELECTION_HANDLE_IMAGE_LEFT).Get(  temp );
      return temp;
    }
    set
    {
      SetProperty( TextField.Property.SELECTION_HANDLE_IMAGE_LEFT, new Dali.Property.Value( value ) );
    }
  }
  public Dali.Property.Map SelectionHandleImageRight
  {
    get
    {
      Dali.Property.Map temp = new Dali.Property.Map();
      GetProperty( TextField.Property.SELECTION_HANDLE_IMAGE_RIGHT).Get(  temp );
      return temp;
    }
    set
    {
      SetProperty( TextField.Property.SELECTION_HANDLE_IMAGE_RIGHT, new Dali.Property.Value( value ) );
    }
  }
  public Dali.Property.Map SelectionHandlePressedImageLeft
  {
    get
    {
      Dali.Property.Map temp = new Dali.Property.Map();
      GetProperty( TextField.Property.SELECTION_HANDLE_PRESSED_IMAGE_LEFT).Get(  temp );
      return temp;
    }
    set
    {
      SetProperty( TextField.Property.SELECTION_HANDLE_PRESSED_IMAGE_LEFT, new Dali.Property.Value( value ) );
    }
  }
  public Dali.Property.Map SelectionHandlePressedImageRight
  {
    get
    {
      Dali.Property.Map temp = new Dali.Property.Map();
      GetProperty( TextField.Property.SELECTION_HANDLE_PRESSED_IMAGE_RIGHT).Get(  temp );
      return temp;
    }
    set
    {
      SetProperty( TextField.Property.SELECTION_HANDLE_PRESSED_IMAGE_RIGHT, new Dali.Property.Value( value ) );
    }
  }
  public Dali.Property.Map SelectionHandleMarkerImageLeft
  {
    get
    {
      Dali.Property.Map temp = new Dali.Property.Map();
      GetProperty( TextField.Property.SELECTION_HANDLE_MARKER_IMAGE_LEFT).Get(  temp );
      return temp;
    }
    set
    {
      SetProperty( TextField.Property.SELECTION_HANDLE_MARKER_IMAGE_LEFT, new Dali.Property.Value( value ) );
    }
  }
  public Dali.Property.Map SelectionHandleMarkerImageRight
  {
    get
    {
      Dali.Property.Map temp = new Dali.Property.Map();
      GetProperty( TextField.Property.SELECTION_HANDLE_MARKER_IMAGE_RIGHT).Get(  temp );
      return temp;
    }
    set
    {
      SetProperty( TextField.Property.SELECTION_HANDLE_MARKER_IMAGE_RIGHT, new Dali.Property.Value( value ) );
    }
  }
  public Dali.CSharp.Color SelectionHighlightColor
  {
    get
    {
      Vector4 temp = new Vector4(0.0f,0.0f,0.0f,0.0f);
      GetProperty( TextField.Property.SELECTION_HIGHLIGHT_COLOR).Get(  temp );
      Dali.CSharp.Color ret = new Dali.CSharp.Color(temp.r, temp.g, temp.b, temp.a);
      return ret;
    }
    set
    {
      SetProperty( TextField.Property.SELECTION_HIGHLIGHT_COLOR, new Dali.Property.Value( value ) );
    }
  }
  public RectInteger DecorationBoundingBox
  {
    get
    {
      RectInteger temp = new RectInteger(0,0,0,0);
      GetProperty( TextField.Property.DECORATION_BOUNDING_BOX).Get(  temp );
      return temp;
    }
    set
    {
      SetProperty( TextField.Property.DECORATION_BOUNDING_BOX, new Dali.Property.Value( value ) );
    }
  }
  public Dali.Property.Map InputMethodSettings
  {
    get
    {
      Dali.Property.Map temp = new Dali.Property.Map();
      GetProperty( TextField.Property.INPUT_METHOD_SETTINGS).Get(  temp );
      return temp;
    }
    set
    {
      SetProperty( TextField.Property.INPUT_METHOD_SETTINGS, new Dali.Property.Value( value ) );
    }
  }
  public Dali.CSharp.Color InputColor
  {
    get
    {
      Vector4 temp = new Vector4(0.0f,0.0f,0.0f,0.0f);
      GetProperty( TextField.Property.INPUT_COLOR).Get(  temp );
      Dali.CSharp.Color ret = new Dali.CSharp.Color(temp.r, temp.g, temp.b, temp.a);
      return ret;
    }
    set
    {
      SetProperty( TextField.Property.INPUT_COLOR, new Dali.Property.Value( value ) );
    }
  }
  public bool EnableMarkup
  {
    get
    {
      bool temp = false;
      GetProperty( TextField.Property.ENABLE_MARKUP).Get( ref temp );
      return temp;
    }
    set
    {
      SetProperty( TextField.Property.ENABLE_MARKUP, new Dali.Property.Value( value ) );
    }
  }
  public string InputFontFamily
  {
    get
    {
      string temp;
      GetProperty( TextField.Property.INPUT_FONT_FAMILY).Get( out temp );
      return temp;
    }
    set
    {
      SetProperty( TextField.Property.INPUT_FONT_FAMILY, new Dali.Property.Value( value ) );
    }
  }
  public Dali.Property.Map InputFontStyle
  {
    get
    {
      Dali.Property.Map temp = new Dali.Property.Map();
      GetProperty( TextField.Property.INPUT_FONT_STYLE).Get(  temp );
      return temp;
    }
    set
    {
      SetProperty( TextField.Property.INPUT_FONT_STYLE, new Dali.Property.Value( value ) );
    }
  }
  public float InputPointSize
  {
    get
    {
      float temp = 0.0f;
      GetProperty( TextField.Property.INPUT_POINT_SIZE).Get( ref temp );
      return temp;
    }
    set
    {
      SetProperty( TextField.Property.INPUT_POINT_SIZE, new Dali.Property.Value( value ) );
    }
  }
  public Dali.Property.Map Underline
  {
    get
    {
      Dali.Property.Map temp = new Dali.Property.Map();
      GetProperty( TextField.Property.UNDERLINE).Get(  temp );
      return temp;
    }
    set
    {
      SetProperty( TextField.Property.UNDERLINE, new Dali.Property.Value( value ) );
    }
  }
  public Dali.Property.Map InputUnderline
  {
    get
    {
      Dali.Property.Map temp = new Dali.Property.Map();
      GetProperty( TextField.Property.INPUT_UNDERLINE).Get(  temp );
      return temp;
    }
    set
    {
      SetProperty( TextField.Property.INPUT_UNDERLINE, new Dali.Property.Value( value ) );
    }
  }
  public Dali.Property.Map Shadow
  {
    get
    {
      Dali.Property.Map temp = new Dali.Property.Map();
      GetProperty( TextField.Property.SHADOW).Get(  temp );
      return temp;
    }
    set
    {
      SetProperty( TextField.Property.SHADOW, new Dali.Property.Value( value ) );
    }
  }
  public Dali.Property.Map InputShadow
  {
    get
    {
      Dali.Property.Map temp = new Dali.Property.Map();
      GetProperty( TextField.Property.INPUT_SHADOW).Get(  temp );
      return temp;
    }
    set
    {
      SetProperty( TextField.Property.INPUT_SHADOW, new Dali.Property.Value( value ) );
    }
  }
  public Dali.Property.Map Emboss
  {
    get
    {
      Dali.Property.Map temp = new Dali.Property.Map();
      GetProperty( TextField.Property.EMBOSS).Get(  temp );
      return temp;
    }
    set
    {
      SetProperty( TextField.Property.EMBOSS, new Dali.Property.Value( value ) );
    }
  }
  public Dali.Property.Map InputEmboss
  {
    get
    {
      Dali.Property.Map temp = new Dali.Property.Map();
      GetProperty( TextField.Property.INPUT_EMBOSS).Get(  temp );
      return temp;
    }
    set
    {
      SetProperty( TextField.Property.INPUT_EMBOSS, new Dali.Property.Value( value ) );
    }
  }
  public Dali.Property.Map Outline
  {
    get
    {
      Dali.Property.Map temp = new Dali.Property.Map();
      GetProperty( TextField.Property.OUTLINE).Get(  temp );
      return temp;
    }
    set
    {
      SetProperty( TextField.Property.OUTLINE, new Dali.Property.Value( value ) );
    }
  }
  public Dali.Property.Map InputOutline
  {
    get
    {
      Dali.Property.Map temp = new Dali.Property.Map();
      GetProperty( TextField.Property.INPUT_OUTLINE).Get(  temp );
      return temp;
    }
    set
    {
      SetProperty( TextField.Property.INPUT_OUTLINE, new Dali.Property.Value( value ) );
    }
  }

/* Properties ends */

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
