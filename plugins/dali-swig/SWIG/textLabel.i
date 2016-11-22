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

%define TextLabel_PROPERTIES_HELPER(NameSpace, ClassName)
%typemap(cscode) NameSpace::ClassName %{

/* Properties earlier added by Ruby Script */

  public int RenderingBackend
  {
    get
    {
      int temp = 0;
      GetProperty( TextLabel.Property.RENDERING_BACKEND).Get( ref temp );
      return temp;
    }
    set
    {
      SetProperty( TextLabel.Property.RENDERING_BACKEND, new Dali.Property.Value( value ) );
    }
  }
  public string Text
  {
    get
    {
      string temp;
      GetProperty( TextLabel.Property.TEXT).Get( out temp );
      return temp;
    }
    set
    {
      SetProperty( TextLabel.Property.TEXT, new Dali.Property.Value( value ) );
    }
  }
  public string FontFamily
  {
    get
    {
      string temp;
      GetProperty( TextLabel.Property.FONT_FAMILY).Get( out temp );
      return temp;
    }
    set
    {
      SetProperty( TextLabel.Property.FONT_FAMILY, new Dali.Property.Value( value ) );
    }
  }
  public Dali.Property.Map FontStyle
  {
    get
    {
      Dali.Property.Map temp = new Dali.Property.Map();
      GetProperty( TextLabel.Property.FONT_STYLE).Get(  temp );
      return temp;
    }
    set
    {
      SetProperty( TextLabel.Property.FONT_STYLE, new Dali.Property.Value( value ) );
    }
  }
  public float PointSize
  {
    get
    {
      float temp = 0.0f;
      GetProperty( TextLabel.Property.POINT_SIZE).Get( ref temp );
      return temp;
    }
    set
    {
      SetProperty( TextLabel.Property.POINT_SIZE, new Dali.Property.Value( value ) );
    }
  }
  public bool MultiLine
  {
    get
    {
      bool temp = false;
      GetProperty( TextLabel.Property.MULTI_LINE).Get( ref temp );
      return temp;
    }
    set
    {
      SetProperty( TextLabel.Property.MULTI_LINE, new Dali.Property.Value( value ) );
    }
  }
  public string HorizontalAlignment
  {
    get
    {
      string temp;
      GetProperty( TextLabel.Property.HORIZONTAL_ALIGNMENT).Get( out temp );
      return temp;
    }
    set
    {
      SetProperty( TextLabel.Property.HORIZONTAL_ALIGNMENT, new Dali.Property.Value( value ) );
    }
  }
  public string VerticalAlignment
  {
    get
    {
      string temp;
      GetProperty( TextLabel.Property.VERTICAL_ALIGNMENT).Get( out temp );
      return temp;
    }
    set
    {
      SetProperty( TextLabel.Property.VERTICAL_ALIGNMENT, new Dali.Property.Value( value ) );
    }
  }
  public Dali.CSharp.Color TextColor
  {
    get
    {
      Vector4 temp = new Vector4(0.0f,0.0f,0.0f,0.0f);
      GetProperty( TextLabel.Property.TEXT_COLOR).Get(  temp );
      Dali.CSharp.Color ret = new Dali.CSharp.Color(temp.r, temp.g, temp.b, temp.a);
      return ret;
    }
    set
    {
      SetProperty( TextLabel.Property.TEXT_COLOR, new Dali.Property.Value( value ) );
    }
  }
  public Dali.CSharp.Size ShadowOffset
  {
    get
    {
      Vector2 temp = new Vector2(0.0f,0.0f);
      GetProperty( TextLabel.Property.SHADOW_OFFSET).Get(  temp );
      Dali.CSharp.Size ret = new Dali.CSharp.Size(temp.x, temp.y);
      return ret;
    }
    set
    {
      SetProperty( TextLabel.Property.SHADOW_OFFSET, new Dali.Property.Value( value ) );
    }
  }
  public Dali.CSharp.Color ShadowColor
  {
    get
    {
      Vector4 temp = new Vector4(0.0f,0.0f,0.0f,0.0f);
      GetProperty( TextLabel.Property.SHADOW_COLOR).Get(  temp );
      Dali.CSharp.Color ret = new Dali.CSharp.Color(temp.r, temp.g, temp.b, temp.a);
      return ret;
    }
    set
    {
      SetProperty( TextLabel.Property.SHADOW_COLOR, new Dali.Property.Value( value ) );
    }
  }
  public bool UnderlineEnabled
  {
    get
    {
      bool temp = false;
      GetProperty( TextLabel.Property.UNDERLINE_ENABLED).Get( ref temp );
      return temp;
    }
    set
    {
      SetProperty( TextLabel.Property.UNDERLINE_ENABLED, new Dali.Property.Value( value ) );
    }
  }
  public Dali.CSharp.Color UnderlineColor
  {
    get
    {
      Vector4 temp = new Vector4(0.0f,0.0f,0.0f,0.0f);
      GetProperty( TextLabel.Property.UNDERLINE_COLOR).Get(  temp );
      Dali.CSharp.Color ret = new Dali.CSharp.Color(temp.r, temp.g, temp.b, temp.a);
      return ret;
    }
    set
    {
      SetProperty( TextLabel.Property.UNDERLINE_COLOR, new Dali.Property.Value( value ) );
    }
  }
  public float UnderlineHeight
  {
    get
    {
      float temp = 0.0f;
      GetProperty( TextLabel.Property.UNDERLINE_HEIGHT).Get( ref temp );
      return temp;
    }
    set
    {
      SetProperty( TextLabel.Property.UNDERLINE_HEIGHT, new Dali.Property.Value( value ) );
    }
  }
  public bool EnableMarkup
  {
    get
    {
      bool temp = false;
      GetProperty( TextLabel.Property.ENABLE_MARKUP).Get( ref temp );
      return temp;
    }
    set
    {
      SetProperty( TextLabel.Property.ENABLE_MARKUP, new Dali.Property.Value( value ) );
    }
  }
  public bool EnableAutoScroll
  {
    get
    {
      bool temp = false;
      GetProperty( TextLabel.Property.ENABLE_AUTO_SCROLL).Get( ref temp );
      return temp;
    }
    set
    {
      SetProperty( TextLabel.Property.ENABLE_AUTO_SCROLL, new Dali.Property.Value( value ) );
    }
  }
  public int AutoScrollSpeed
  {
    get
    {
      int temp = 0;
      GetProperty( TextLabel.Property.AUTO_SCROLL_SPEED).Get( ref temp );
      return temp;
    }
    set
    {
      SetProperty( TextLabel.Property.AUTO_SCROLL_SPEED, new Dali.Property.Value( value ) );
    }
  }
  public int AutoScrollLoopCount
  {
    get
    {
      int temp = 0;
      GetProperty( TextLabel.Property.AUTO_SCROLL_LOOP_COUNT).Get( ref temp );
      return temp;
    }
    set
    {
      SetProperty( TextLabel.Property.AUTO_SCROLL_LOOP_COUNT, new Dali.Property.Value( value ) );
    }
  }
  public float AutoScrollGap
  {
    get
    {
      float temp = 0.0f;
      GetProperty( TextLabel.Property.AUTO_SCROLL_GAP).Get( ref temp );
      return temp;
    }
    set
    {
      SetProperty( TextLabel.Property.AUTO_SCROLL_GAP, new Dali.Property.Value( value ) );
    }
  }
  public float LineSpacing
  {
    get
    {
      float temp = 0.0f;
      GetProperty( TextLabel.Property.LINE_SPACING).Get( ref temp );
      return temp;
    }
    set
    {
      SetProperty( TextLabel.Property.LINE_SPACING, new Dali.Property.Value( value ) );
    }
  }
  public Dali.Property.Map Underline
  {
    get
    {
      Dali.Property.Map temp = new Dali.Property.Map();
      GetProperty( TextLabel.Property.UNDERLINE).Get(  temp );
      return temp;
    }
    set
    {
      SetProperty( TextLabel.Property.UNDERLINE, new Dali.Property.Value( value ) );
    }
  }
  public Dali.Property.Map Shadow
  {
    get
    {
      Dali.Property.Map temp = new Dali.Property.Map();
      GetProperty( TextLabel.Property.SHADOW).Get(  temp );
      return temp;
    }
    set
    {
      SetProperty( TextLabel.Property.SHADOW, new Dali.Property.Value( value ) );
    }
  }
  public Dali.Property.Map Emboss
  {
    get
    {
      Dali.Property.Map temp = new Dali.Property.Map();
      GetProperty( TextLabel.Property.EMBOSS).Get(  temp );
      return temp;
    }
    set
    {
      SetProperty( TextLabel.Property.EMBOSS, new Dali.Property.Value( value ) );
    }
  }
  public Dali.Property.Map Outline
  {
    get
    {
      Dali.Property.Map temp = new Dali.Property.Map();
      GetProperty( TextLabel.Property.OUTLINE).Get(  temp );
      return temp;
    }
    set
    {
      SetProperty( TextLabel.Property.OUTLINE, new Dali.Property.Value( value ) );
    }
  }

/* Properties ends */
%}
%enddef

namespace Dali
{
  TextLabel_PROPERTIES_HELPER(Dali::Toolkit, TextLabel);
}
