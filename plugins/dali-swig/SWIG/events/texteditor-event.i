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

/* Properties earlier added by Ruby Script */

  public int RenderingBackend
  {
    get
    {
      int temp = 0;
      GetProperty( TextEditor.Property.RENDERING_BACKEND).Get( ref temp );
      return temp;
    }
    set
    {
      SetProperty( TextEditor.Property.RENDERING_BACKEND, new Dali.Property.Value( value ) );
    }
  }
  public string Text
  {
    get
    {
      string temp;
      GetProperty( TextEditor.Property.TEXT).Get( out temp );
      return temp;
    }
    set
    {
      SetProperty( TextEditor.Property.TEXT, new Dali.Property.Value( value ) );
    }
  }
  public Dali.CSharp.Color TextColor
  {
    get
    {
      Vector4 temp = new Vector4(0.0f,0.0f,0.0f,0.0f);
      GetProperty( TextEditor.Property.TEXT_COLOR).Get(  temp );
      Dali.CSharp.Color ret = new Dali.CSharp.Color(temp.r, temp.g, temp.b, temp.a);
      return ret;
    }
    set
    {
      SetProperty( TextEditor.Property.TEXT_COLOR, new Dali.Property.Value( value ) );
    }
  }
  public string FontFamily
  {
    get
    {
      string temp;
      GetProperty( TextEditor.Property.FONT_FAMILY).Get( out temp );
      return temp;
    }
    set
    {
      SetProperty( TextEditor.Property.FONT_FAMILY, new Dali.Property.Value( value ) );
    }
  }
  public Dali.Property.Map FontStyle
  {
    get
    {
      Dali.Property.Map temp = new Dali.Property.Map();
      GetProperty( TextEditor.Property.FONT_STYLE).Get(  temp );
      return temp;
    }
    set
    {
      SetProperty( TextEditor.Property.FONT_STYLE, new Dali.Property.Value( value ) );
    }
  }
  public float PointSize
  {
    get
    {
      float temp = 0.0f;
      GetProperty( TextEditor.Property.POINT_SIZE).Get( ref temp );
      return temp;
    }
    set
    {
      SetProperty( TextEditor.Property.POINT_SIZE, new Dali.Property.Value( value ) );
    }
  }
  public string HorizontalAlignment
  {
    get
    {
      string temp;
      GetProperty( TextEditor.Property.HORIZONTAL_ALIGNMENT).Get( out temp );
      return temp;
    }
    set
    {
      SetProperty( TextEditor.Property.HORIZONTAL_ALIGNMENT, new Dali.Property.Value( value ) );
    }
  }
  public float ScrollThreshold
  {
    get
    {
      float temp = 0.0f;
      GetProperty( TextEditor.Property.SCROLL_THRESHOLD).Get( ref temp );
      return temp;
    }
    set
    {
      SetProperty( TextEditor.Property.SCROLL_THRESHOLD, new Dali.Property.Value( value ) );
    }
  }
  public float ScrollSpeed
  {
    get
    {
      float temp = 0.0f;
      GetProperty( TextEditor.Property.SCROLL_SPEED).Get( ref temp );
      return temp;
    }
    set
    {
      SetProperty( TextEditor.Property.SCROLL_SPEED, new Dali.Property.Value( value ) );
    }
  }
  public Dali.CSharp.Color PrimaryCursorColor
  {
    get
    {
      Vector4 temp = new Vector4(0.0f,0.0f,0.0f,0.0f);
      GetProperty( TextEditor.Property.PRIMARY_CURSOR_COLOR).Get(  temp );
      Dali.CSharp.Color ret = new Dali.CSharp.Color(temp.r, temp.g, temp.b, temp.a);
      return ret;
    }
    set
    {
      SetProperty( TextEditor.Property.PRIMARY_CURSOR_COLOR, new Dali.Property.Value( value ) );
    }
  }
  public Dali.CSharp.Color SecondaryCursorColor
  {
    get
    {
      Vector4 temp = new Vector4(0.0f,0.0f,0.0f,0.0f);
      GetProperty( TextEditor.Property.SECONDARY_CURSOR_COLOR).Get(  temp );
      Dali.CSharp.Color ret = new Dali.CSharp.Color(temp.r, temp.g, temp.b, temp.a);
      return ret;
    }
    set
    {
      SetProperty( TextEditor.Property.SECONDARY_CURSOR_COLOR, new Dali.Property.Value( value ) );
    }
  }
  public bool EnableCursorBlink
  {
    get
    {
      bool temp = false;
      GetProperty( TextEditor.Property.ENABLE_CURSOR_BLINK).Get( ref temp );
      return temp;
    }
    set
    {
      SetProperty( TextEditor.Property.ENABLE_CURSOR_BLINK, new Dali.Property.Value( value ) );
    }
  }
  public float CursorBlinkInterval
  {
    get
    {
      float temp = 0.0f;
      GetProperty( TextEditor.Property.CURSOR_BLINK_INTERVAL).Get( ref temp );
      return temp;
    }
    set
    {
      SetProperty( TextEditor.Property.CURSOR_BLINK_INTERVAL, new Dali.Property.Value( value ) );
    }
  }
  public float CursorBlinkDuration
  {
    get
    {
      float temp = 0.0f;
      GetProperty( TextEditor.Property.CURSOR_BLINK_DURATION).Get( ref temp );
      return temp;
    }
    set
    {
      SetProperty( TextEditor.Property.CURSOR_BLINK_DURATION, new Dali.Property.Value( value ) );
    }
  }
  public int CursorWidth
  {
    get
    {
      int temp = 0;
      GetProperty( TextEditor.Property.CURSOR_WIDTH).Get( ref temp );
      return temp;
    }
    set
    {
      SetProperty( TextEditor.Property.CURSOR_WIDTH, new Dali.Property.Value( value ) );
    }
  }
  public string GrabHandleImage
  {
    get
    {
      string temp;
      GetProperty( TextEditor.Property.GRAB_HANDLE_IMAGE).Get( out temp );
      return temp;
    }
    set
    {
      SetProperty( TextEditor.Property.GRAB_HANDLE_IMAGE, new Dali.Property.Value( value ) );
    }
  }
  public string GrabHandlePressedImage
  {
    get
    {
      string temp;
      GetProperty( TextEditor.Property.GRAB_HANDLE_PRESSED_IMAGE).Get( out temp );
      return temp;
    }
    set
    {
      SetProperty( TextEditor.Property.GRAB_HANDLE_PRESSED_IMAGE, new Dali.Property.Value( value ) );
    }
  }
  public Dali.Property.Map SelectionHandleImageLeft
  {
    get
    {
      Dali.Property.Map temp = new Dali.Property.Map();
      GetProperty( TextEditor.Property.SELECTION_HANDLE_IMAGE_LEFT).Get(  temp );
      return temp;
    }
    set
    {
      SetProperty( TextEditor.Property.SELECTION_HANDLE_IMAGE_LEFT, new Dali.Property.Value( value ) );
    }
  }
  public Dali.Property.Map SelectionHandleImageRight
  {
    get
    {
      Dali.Property.Map temp = new Dali.Property.Map();
      GetProperty( TextEditor.Property.SELECTION_HANDLE_IMAGE_RIGHT).Get(  temp );
      return temp;
    }
    set
    {
      SetProperty( TextEditor.Property.SELECTION_HANDLE_IMAGE_RIGHT, new Dali.Property.Value( value ) );
    }
  }
  public Dali.Property.Map SelectionHandlePressedImageLeft
  {
    get
    {
      Dali.Property.Map temp = new Dali.Property.Map();
      GetProperty( TextEditor.Property.SELECTION_HANDLE_PRESSED_IMAGE_LEFT).Get(  temp );
      return temp;
    }
    set
    {
      SetProperty( TextEditor.Property.SELECTION_HANDLE_PRESSED_IMAGE_LEFT, new Dali.Property.Value( value ) );
    }
  }
  public Dali.Property.Map SelectionHandlePressedImageRight
  {
    get
    {
      Dali.Property.Map temp = new Dali.Property.Map();
      GetProperty( TextEditor.Property.SELECTION_HANDLE_PRESSED_IMAGE_RIGHT).Get(  temp );
      return temp;
    }
    set
    {
      SetProperty( TextEditor.Property.SELECTION_HANDLE_PRESSED_IMAGE_RIGHT, new Dali.Property.Value( value ) );
    }
  }
  public Dali.Property.Map SelectionHandleMarkerImageLeft
  {
    get
    {
      Dali.Property.Map temp = new Dali.Property.Map();
      GetProperty( TextEditor.Property.SELECTION_HANDLE_MARKER_IMAGE_LEFT).Get(  temp );
      return temp;
    }
    set
    {
      SetProperty( TextEditor.Property.SELECTION_HANDLE_MARKER_IMAGE_LEFT, new Dali.Property.Value( value ) );
    }
  }
  public Dali.Property.Map SelectionHandleMarkerImageRight
  {
    get
    {
      Dali.Property.Map temp = new Dali.Property.Map();
      GetProperty( TextEditor.Property.SELECTION_HANDLE_MARKER_IMAGE_RIGHT).Get(  temp );
      return temp;
    }
    set
    {
      SetProperty( TextEditor.Property.SELECTION_HANDLE_MARKER_IMAGE_RIGHT, new Dali.Property.Value( value ) );
    }
  }
  public Dali.CSharp.Color SelectionHighlightColor
  {
    get
    {
      Vector4 temp = new Vector4(0.0f,0.0f,0.0f,0.0f);
      GetProperty( TextEditor.Property.SELECTION_HIGHLIGHT_COLOR).Get(  temp );
      Dali.CSharp.Color ret = new Dali.CSharp.Color(temp.r, temp.g, temp.b, temp.a);
      return ret;
    }
    set
    {
      SetProperty( TextEditor.Property.SELECTION_HIGHLIGHT_COLOR, new Dali.Property.Value( value ) );
    }
  }
  public RectInteger DecorationBoundingBox
  {
    get
    {
      RectInteger temp = new RectInteger(0,0,0,0);
      GetProperty( TextEditor.Property.DECORATION_BOUNDING_BOX).Get(  temp );
      return temp;
    }
    set
    {
      SetProperty( TextEditor.Property.DECORATION_BOUNDING_BOX, new Dali.Property.Value( value ) );
    }
  }
  public bool EnableMarkup
  {
    get
    {
      bool temp = false;
      GetProperty( TextEditor.Property.ENABLE_MARKUP).Get( ref temp );
      return temp;
    }
    set
    {
      SetProperty( TextEditor.Property.ENABLE_MARKUP, new Dali.Property.Value( value ) );
    }
  }
  public Dali.CSharp.Color InputColor
  {
    get
    {
      Vector4 temp = new Vector4(0.0f,0.0f,0.0f,0.0f);
      GetProperty( TextEditor.Property.INPUT_COLOR).Get(  temp );
      Dali.CSharp.Color ret = new Dali.CSharp.Color(temp.r, temp.g, temp.b, temp.a);
      return ret;
    }
    set
    {
      SetProperty( TextEditor.Property.INPUT_COLOR, new Dali.Property.Value( value ) );
    }
  }
  public string InputFontFamily
  {
    get
    {
      string temp;
      GetProperty( TextEditor.Property.INPUT_FONT_FAMILY).Get( out temp );
      return temp;
    }
    set
    {
      SetProperty( TextEditor.Property.INPUT_FONT_FAMILY, new Dali.Property.Value( value ) );
    }
  }
  public Dali.Property.Map InputFontStyle
  {
    get
    {
      Dali.Property.Map temp = new Dali.Property.Map();
      GetProperty( TextEditor.Property.INPUT_FONT_STYLE).Get(  temp );
      return temp;
    }
    set
    {
      SetProperty( TextEditor.Property.INPUT_FONT_STYLE, new Dali.Property.Value( value ) );
    }
  }
  public float InputPointSize
  {
    get
    {
      float temp = 0.0f;
      GetProperty( TextEditor.Property.INPUT_POINT_SIZE).Get( ref temp );
      return temp;
    }
    set
    {
      SetProperty( TextEditor.Property.INPUT_POINT_SIZE, new Dali.Property.Value( value ) );
    }
  }
  public float LineSpacing
  {
    get
    {
      float temp = 0.0f;
      GetProperty( TextEditor.Property.LINE_SPACING).Get( ref temp );
      return temp;
    }
    set
    {
      SetProperty( TextEditor.Property.LINE_SPACING, new Dali.Property.Value( value ) );
    }
  }
  public float InputLineSpacing
  {
    get
    {
      float temp = 0.0f;
      GetProperty( TextEditor.Property.INPUT_LINE_SPACING).Get( ref temp );
      return temp;
    }
    set
    {
      SetProperty( TextEditor.Property.INPUT_LINE_SPACING, new Dali.Property.Value( value ) );
    }
  }
  public Dali.Property.Map Underline
  {
    get
    {
      Dali.Property.Map temp = new Dali.Property.Map();
      GetProperty( TextEditor.Property.UNDERLINE).Get(  temp );
      return temp;
    }
    set
    {
      SetProperty( TextEditor.Property.UNDERLINE, new Dali.Property.Value( value ) );
    }
  }
  public Dali.Property.Map InputUnderline
  {
    get
    {
      Dali.Property.Map temp = new Dali.Property.Map();
      GetProperty( TextEditor.Property.INPUT_UNDERLINE).Get(  temp );
      return temp;
    }
    set
    {
      SetProperty( TextEditor.Property.INPUT_UNDERLINE, new Dali.Property.Value( value ) );
    }
  }
  public Dali.Property.Map Shadow
  {
    get
    {
      Dali.Property.Map temp = new Dali.Property.Map();
      GetProperty( TextEditor.Property.SHADOW).Get(  temp );
      return temp;
    }
    set
    {
      SetProperty( TextEditor.Property.SHADOW, new Dali.Property.Value( value ) );
    }
  }
  public Dali.Property.Map InputShadow
  {
    get
    {
      Dali.Property.Map temp = new Dali.Property.Map();
      GetProperty( TextEditor.Property.INPUT_SHADOW).Get(  temp );
      return temp;
    }
    set
    {
      SetProperty( TextEditor.Property.INPUT_SHADOW, new Dali.Property.Value( value ) );
    }
  }
  public Dali.Property.Map Emboss
  {
    get
    {
      Dali.Property.Map temp = new Dali.Property.Map();
      GetProperty( TextEditor.Property.EMBOSS).Get(  temp );
      return temp;
    }
    set
    {
      SetProperty( TextEditor.Property.EMBOSS, new Dali.Property.Value( value ) );
    }
  }
  public Dali.Property.Map InputEmboss
  {
    get
    {
      Dali.Property.Map temp = new Dali.Property.Map();
      GetProperty( TextEditor.Property.INPUT_EMBOSS).Get(  temp );
      return temp;
    }
    set
    {
      SetProperty( TextEditor.Property.INPUT_EMBOSS, new Dali.Property.Value( value ) );
    }
  }
  public Dali.Property.Map Outline
  {
    get
    {
      Dali.Property.Map temp = new Dali.Property.Map();
      GetProperty( TextEditor.Property.OUTLINE).Get(  temp );
      return temp;
    }
    set
    {
      SetProperty( TextEditor.Property.OUTLINE, new Dali.Property.Value( value ) );
    }
  }
  public Dali.Property.Map InputOutline
  {
    get
    {
      Dali.Property.Map temp = new Dali.Property.Map();
      GetProperty( TextEditor.Property.INPUT_OUTLINE).Get(  temp );
      return temp;
    }
    set
    {
      SetProperty( TextEditor.Property.INPUT_OUTLINE, new Dali.Property.Value( value ) );
    }
  }

/* Properties ends */

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
