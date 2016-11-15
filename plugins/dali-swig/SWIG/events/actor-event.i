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

%define ACTOR_EVENTHANDLER_TYPEMAP_EVENTARG(NameSpace, ClassName)
%typemap(csimports) NameSpace::ClassName %{
    using System;
    using System.Runtime.InteropServices;
%}

%enddef

%define ACTOR_EVENTHANDLER_TYPEMAP_HELPER(NameSpace, ClassName)
  %typemap(cscode) NameSpace::ClassName %{

    public static ClassName Get ## ClassName ## FromPtr(global::System.IntPtr cPtr) {
      ClassName ret = new ClassName(cPtr, false);
      if (NDalicPINVOKE.SWIGPendingException.Pending) throw NDalicPINVOKE.SWIGPendingException.Retrieve();
      return ret;
    }

    public IntPtr GetPtrfrom ## ClassName ()
    {
       return (IntPtr)swigCPtr;
    }

    public Actor Parent
    {
      get
      {
        Actor parent = GetParent();
        return parent;
      }
    }

   public bool Visibility
   {
      get
      {
        bool visibility = IsVisible();
        return visibility;
      }
    }

   public float Opacity
   {
      set
      {
        SetOpacity(value);
      }
      get
      {
        float ret = GetCurrentOpacity();
        return ret;
      }
   }

    public bool StateFocusEnable
    {
      set
      {
        SetKeyboardFocusable(value);
      }
      get
      {
        bool focusable = IsKeyboardFocusable();
        return focusable;
      }
    }

    public void Show()
    {
      SetVisible(true);
    }

    public void Hide()
    {
      SetVisible(false);
    }

/* Properties earlier added by Ruby Script */

  public Dali.CSharp.Position ParentOrigin
  {
    get
    {
      Vector3 temp = new Vector3(0.0f,0.0f,0.0f);
      GetProperty( Actor.Property.PARENT_ORIGIN).Get(  temp );
      Dali.CSharp.Position ret = new Dali.CSharp.Position(temp.x, temp.y, temp.z);
      return ret;
    }
    set
    {
      SetProperty( Actor.Property.PARENT_ORIGIN, new Dali.Property.Value( value ) );
    }
  }
  public float ParentOriginX
  {
    get
    {
      float temp = 0.0f;
      GetProperty( Actor.Property.PARENT_ORIGIN_X).Get( ref temp );
      return temp;
    }
    set
    {
      SetProperty( Actor.Property.PARENT_ORIGIN_X, new Dali.Property.Value( value ) );
    }
  }
  public float ParentOriginY
  {
    get
    {
      float temp = 0.0f;
      GetProperty( Actor.Property.PARENT_ORIGIN_Y).Get( ref temp );
      return temp;
    }
    set
    {
      SetProperty( Actor.Property.PARENT_ORIGIN_Y, new Dali.Property.Value( value ) );
    }
  }
  public float ParentOriginZ
  {
    get
    {
      float temp = 0.0f;
      GetProperty( Actor.Property.PARENT_ORIGIN_Z).Get( ref temp );
      return temp;
    }
    set
    {
      SetProperty( Actor.Property.PARENT_ORIGIN_Z, new Dali.Property.Value( value ) );
    }
  }
  public Dali.CSharp.Position AnchorPoint
  {
    get
    {
      Vector3 temp = new Vector3(0.0f,0.0f,0.0f);
      GetProperty( Actor.Property.ANCHOR_POINT).Get(  temp );
      Dali.CSharp.Position ret = new Dali.CSharp.Position(temp.x, temp.y, temp.z);
      return ret;
    }
    set
    {
      SetProperty( Actor.Property.ANCHOR_POINT, new Dali.Property.Value( value ) );
    }
  }
  public float AnchorPointX
  {
    get
    {
      float temp = 0.0f;
      GetProperty( Actor.Property.ANCHOR_POINT_X).Get( ref temp );
      return temp;
    }
    set
    {
      SetProperty( Actor.Property.ANCHOR_POINT_X, new Dali.Property.Value( value ) );
    }
  }
  public float AnchorPointY
  {
    get
    {
      float temp = 0.0f;
      GetProperty( Actor.Property.ANCHOR_POINT_Y).Get( ref temp );
      return temp;
    }
    set
    {
      SetProperty( Actor.Property.ANCHOR_POINT_Y, new Dali.Property.Value( value ) );
    }
  }
  public float AnchorPointZ
  {
    get
    {
      float temp = 0.0f;
      GetProperty( Actor.Property.ANCHOR_POINT_Z).Get( ref temp );
      return temp;
    }
    set
    {
      SetProperty( Actor.Property.ANCHOR_POINT_Z, new Dali.Property.Value( value ) );
    }
  }
  public Dali.CSharp.Position Size
  {
    get
    {
      Vector3 temp = new Vector3(0.0f,0.0f,0.0f);
      GetProperty( Actor.Property.SIZE).Get(  temp );
      Dali.CSharp.Position ret = new Dali.CSharp.Position(temp.x, temp.y, temp.z);
      return ret;
    }
    set
    {
      SetProperty( Actor.Property.SIZE, new Dali.Property.Value( value ) );
    }
  }
  public float SizeWidth
  {
    get
    {
      float temp = 0.0f;
      GetProperty( Actor.Property.SIZE_WIDTH).Get( ref temp );
      return temp;
    }
    set
    {
      SetProperty( Actor.Property.SIZE_WIDTH, new Dali.Property.Value( value ) );
    }
  }
  public float SizeHeight
  {
    get
    {
      float temp = 0.0f;
      GetProperty( Actor.Property.SIZE_HEIGHT).Get( ref temp );
      return temp;
    }
    set
    {
      SetProperty( Actor.Property.SIZE_HEIGHT, new Dali.Property.Value( value ) );
    }
  }
  public float SizeDepth
  {
    get
    {
      float temp = 0.0f;
      GetProperty( Actor.Property.SIZE_DEPTH).Get( ref temp );
      return temp;
    }
    set
    {
      SetProperty( Actor.Property.SIZE_DEPTH, new Dali.Property.Value( value ) );
    }
  }
  public Dali.CSharp.Position Position
  {
    get
    {
      Vector3 temp = new Vector3(0.0f,0.0f,0.0f);
      GetProperty( Actor.Property.POSITION).Get(  temp );
      Dali.CSharp.Position ret = new Dali.CSharp.Position(temp.x, temp.y, temp.z);
      return ret;
    }
    set
    {
      SetProperty( Actor.Property.POSITION, new Dali.Property.Value( value ) );
    }
  }
  public float PositionX
  {
    get
    {
      float temp = 0.0f;
      GetProperty( Actor.Property.POSITION_X).Get( ref temp );
      return temp;
    }
    set
    {
      SetProperty( Actor.Property.POSITION_X, new Dali.Property.Value( value ) );
    }
  }
  public float PositionY
  {
    get
    {
      float temp = 0.0f;
      GetProperty( Actor.Property.POSITION_Y).Get( ref temp );
      return temp;
    }
    set
    {
      SetProperty( Actor.Property.POSITION_Y, new Dali.Property.Value( value ) );
    }
  }
  public float PositionZ
  {
    get
    {
      float temp = 0.0f;
      GetProperty( Actor.Property.POSITION_Z).Get( ref temp );
      return temp;
    }
    set
    {
      SetProperty( Actor.Property.POSITION_Z, new Dali.Property.Value( value ) );
    }
  }
  public Dali.CSharp.Position WorldPosition
  {
    get
    {
      Vector3 temp = new Vector3(0.0f,0.0f,0.0f);
      GetProperty( Actor.Property.WORLD_POSITION).Get(  temp );
      Dali.CSharp.Position ret = new Dali.CSharp.Position(temp.x, temp.y, temp.z);
      return ret;
    }
}  public float WorldPositionX
  {
    get
    {
      float temp = 0.0f;
      GetProperty( Actor.Property.WORLD_POSITION_X).Get( ref temp );
      return temp;
    }
}  public float WorldPositionY
  {
    get
    {
      float temp = 0.0f;
      GetProperty( Actor.Property.WORLD_POSITION_Y).Get( ref temp );
      return temp;
    }
}  public float WorldPositionZ
  {
    get
    {
      float temp = 0.0f;
      GetProperty( Actor.Property.WORLD_POSITION_Z).Get( ref temp );
      return temp;
    }
}  public Quaternion Orientation
  {
    get
    {
      Quaternion temp = new Quaternion();
      GetProperty( Actor.Property.ORIENTATION).Get(  temp );
      return temp;
    }
    set
    {
      SetProperty( Actor.Property.ORIENTATION, new Dali.Property.Value( value ) );
    }
  }
  public Quaternion WorldOrientation
  {
    get
    {
      Quaternion temp = new Quaternion();
      GetProperty( Actor.Property.WORLD_ORIENTATION).Get(  temp );
      return temp;
    }
}  public Dali.CSharp.Position Scale
  {
    get
    {
      Vector3 temp = new Vector3(0.0f,0.0f,0.0f);
      GetProperty( Actor.Property.SCALE).Get(  temp );
      Dali.CSharp.Position ret = new Dali.CSharp.Position(temp.x, temp.y, temp.z);
      return ret;
    }
    set
    {
      SetProperty( Actor.Property.SCALE, new Dali.Property.Value( value ) );
    }
  }
  public float ScaleX
  {
    get
    {
      float temp = 0.0f;
      GetProperty( Actor.Property.SCALE_X).Get( ref temp );
      return temp;
    }
    set
    {
      SetProperty( Actor.Property.SCALE_X, new Dali.Property.Value( value ) );
    }
  }
  public float ScaleY
  {
    get
    {
      float temp = 0.0f;
      GetProperty( Actor.Property.SCALE_Y).Get( ref temp );
      return temp;
    }
    set
    {
      SetProperty( Actor.Property.SCALE_Y, new Dali.Property.Value( value ) );
    }
  }
  public float ScaleZ
  {
    get
    {
      float temp = 0.0f;
      GetProperty( Actor.Property.SCALE_Z).Get( ref temp );
      return temp;
    }
    set
    {
      SetProperty( Actor.Property.SCALE_Z, new Dali.Property.Value( value ) );
    }
  }
  public Dali.CSharp.Position WorldScale
  {
    get
    {
      Vector3 temp = new Vector3(0.0f,0.0f,0.0f);
      GetProperty( Actor.Property.WORLD_SCALE).Get(  temp );
      Dali.CSharp.Position ret = new Dali.CSharp.Position(temp.x, temp.y, temp.z);
      return ret;
    }
}  public bool Visible
  {
    get
    {
      bool temp = false;
      GetProperty( Actor.Property.VISIBLE).Get( ref temp );
      return temp;
    }
    set
    {
      SetProperty( Actor.Property.VISIBLE, new Dali.Property.Value( value ) );
    }
  }
  public Dali.CSharp.Color Color
  {
    get
    {
      Vector4 temp = new Vector4(0.0f,0.0f,0.0f,0.0f);
      GetProperty( Actor.Property.COLOR).Get(  temp );
      Dali.CSharp.Color ret = new Dali.CSharp.Color(temp.x, temp.y, temp.z, temp.w);
      return ret;
    }
    set
    {
      SetProperty( Actor.Property.COLOR, new Dali.Property.Value( value ) );
    }
  }
  public float ColorRed
  {
    get
    {
      float temp = 0.0f;
      GetProperty( Actor.Property.COLOR_RED).Get( ref temp );
      return temp;
    }
    set
    {
      SetProperty( Actor.Property.COLOR_RED, new Dali.Property.Value( value ) );
    }
  }
  public float ColorGreen
  {
    get
    {
      float temp = 0.0f;
      GetProperty( Actor.Property.COLOR_GREEN).Get( ref temp );
      return temp;
    }
    set
    {
      SetProperty( Actor.Property.COLOR_GREEN, new Dali.Property.Value( value ) );
    }
  }
  public float ColorBlue
  {
    get
    {
      float temp = 0.0f;
      GetProperty( Actor.Property.COLOR_BLUE).Get( ref temp );
      return temp;
    }
    set
    {
      SetProperty( Actor.Property.COLOR_BLUE, new Dali.Property.Value( value ) );
    }
  }
  public float ColorAlpha
  {
    get
    {
      float temp = 0.0f;
      GetProperty( Actor.Property.COLOR_ALPHA).Get( ref temp );
      return temp;
    }
    set
    {
      SetProperty( Actor.Property.COLOR_ALPHA, new Dali.Property.Value( value ) );
    }
  }
  public Dali.CSharp.Color WorldColor
  {
    get
    {
      Vector4 temp = new Vector4(0.0f,0.0f,0.0f,0.0f);
      GetProperty( Actor.Property.WORLD_COLOR).Get(  temp );
      Dali.CSharp.Color ret = new Dali.CSharp.Color(temp.r, temp.g, temp.b, temp.a);
      return ret;
    }
}  public Matrix WorldMatrix
  {
    get
    {
      Matrix temp = new Matrix();
      GetProperty( Actor.Property.WORLD_MATRIX).Get(  temp );
      return temp;
    }
}  public string Name
  {
    get
    {
      string temp;
      GetProperty( Actor.Property.NAME).Get( out temp );
      return temp;
    }
    set
    {
      SetProperty( Actor.Property.NAME, new Dali.Property.Value( value ) );
    }
  }
  public bool Sensitive
  {
    get
    {
      bool temp = false;
      GetProperty( Actor.Property.SENSITIVE).Get( ref temp );
      return temp;
    }
    set
    {
      SetProperty( Actor.Property.SENSITIVE, new Dali.Property.Value( value ) );
    }
  }
  public bool LeaveRequired
  {
    get
    {
      bool temp = false;
      GetProperty( Actor.Property.LEAVE_REQUIRED).Get( ref temp );
      return temp;
    }
    set
    {
      SetProperty( Actor.Property.LEAVE_REQUIRED, new Dali.Property.Value( value ) );
    }
  }
  public bool InheritOrientation
  {
    get
    {
      bool temp = false;
      GetProperty( Actor.Property.INHERIT_ORIENTATION).Get( ref temp );
      return temp;
    }
    set
    {
      SetProperty( Actor.Property.INHERIT_ORIENTATION, new Dali.Property.Value( value ) );
    }
  }
  public bool InheritScale
  {
    get
    {
      bool temp = false;
      GetProperty( Actor.Property.INHERIT_SCALE).Get( ref temp );
      return temp;
    }
    set
    {
      SetProperty( Actor.Property.INHERIT_SCALE, new Dali.Property.Value( value ) );
    }
  }
  public string ColorMode
  {
    get
    {
      string temp;
      GetProperty( Actor.Property.COLOR_MODE).Get( out temp );
      return temp;
    }
    set
    {
      SetProperty( Actor.Property.COLOR_MODE, new Dali.Property.Value( value ) );
    }
  }
  public string PositionInheritance
  {
    get
    {
      string temp;
      GetProperty( Actor.Property.POSITION_INHERITANCE).Get( out temp );
      return temp;
    }
    set
    {
      SetProperty( Actor.Property.POSITION_INHERITANCE, new Dali.Property.Value( value ) );
    }
  }
  public string DrawMode
  {
    get
    {
      string temp;
      GetProperty( Actor.Property.DRAW_MODE).Get( out temp );
      return temp;
    }
    set
    {
      SetProperty( Actor.Property.DRAW_MODE, new Dali.Property.Value( value ) );
    }
  }
  public Dali.CSharp.Position SizeModeFactor
  {
    get
    {
      Vector3 temp = new Vector3(0.0f,0.0f,0.0f);
      GetProperty( Actor.Property.SIZE_MODE_FACTOR).Get(  temp );
      Dali.CSharp.Position ret = new Dali.CSharp.Position(temp.x, temp.y, temp.z);
      return ret;
    }
    set
    {
      SetProperty( Actor.Property.SIZE_MODE_FACTOR, new Dali.Property.Value( value ) );
    }
  }
  public string WidthResizePolicy
  {
    get
    {
      string temp;
      GetProperty( Actor.Property.WIDTH_RESIZE_POLICY).Get( out temp );
      return temp;
    }
    set
    {
      SetProperty( Actor.Property.WIDTH_RESIZE_POLICY, new Dali.Property.Value( value ) );
    }
  }
  public string HeightResizePolicy
  {
    get
    {
      string temp;
      GetProperty( Actor.Property.HEIGHT_RESIZE_POLICY).Get( out temp );
      return temp;
    }
    set
    {
      SetProperty( Actor.Property.HEIGHT_RESIZE_POLICY, new Dali.Property.Value( value ) );
    }
  }
  public string SizeScalePolicy
  {
    get
    {
      string temp;
      GetProperty( Actor.Property.SIZE_SCALE_POLICY).Get( out temp );
      return temp;
    }
    set
    {
      SetProperty( Actor.Property.SIZE_SCALE_POLICY, new Dali.Property.Value( value ) );
    }
  }
  public bool WidthForHeight
  {
    get
    {
      bool temp = false;
      GetProperty( Actor.Property.WIDTH_FOR_HEIGHT).Get( ref temp );
      return temp;
    }
    set
    {
      SetProperty( Actor.Property.WIDTH_FOR_HEIGHT, new Dali.Property.Value( value ) );
    }
  }
  public bool HeightForWidth
  {
    get
    {
      bool temp = false;
      GetProperty( Actor.Property.HEIGHT_FOR_WIDTH).Get( ref temp );
      return temp;
    }
    set
    {
      SetProperty( Actor.Property.HEIGHT_FOR_WIDTH, new Dali.Property.Value( value ) );
    }
  }
  public Vector4 Padding
  {
    get
    {
      Vector4 temp = new Vector4(0.0f,0.0f,0.0f,0.0f);
      GetProperty( Actor.Property.PADDING).Get(  temp );
      return temp;
    }
    set
    {
      SetProperty( Actor.Property.PADDING, new Dali.Property.Value( value ) );
    }
  }
  public Dali.CSharp.Size MinimumSize
  {
    get
    {
      Vector2 temp = new Vector2(0.0f,0.0f);
      GetProperty( Actor.Property.MINIMUM_SIZE).Get(  temp );
      Dali.CSharp.Size ret = new Dali.CSharp.Size(temp.x, temp.y);
      return ret;
    }
    set
    {
      SetProperty( Actor.Property.MINIMUM_SIZE, new Dali.Property.Value( value ) );
    }
  }
  public Dali.CSharp.Size MaximumSize
  {
    get
    {
      Vector2 temp = new Vector2(0.0f,0.0f);
      GetProperty( Actor.Property.MAXIMUM_SIZE).Get(  temp );
      Dali.CSharp.Size ret = new Dali.CSharp.Size(temp.x, temp.y);
      return ret;
    }
    set
    {
      SetProperty( Actor.Property.MAXIMUM_SIZE, new Dali.Property.Value( value ) );
    }
  }
  public bool InheritPosition
  {
    get
    {
      bool temp = false;
      GetProperty( Actor.Property.INHERIT_POSITION).Get( ref temp );
      return temp;
    }
    set
    {
      SetProperty( Actor.Property.INHERIT_POSITION, new Dali.Property.Value( value ) );
    }
  }
  public string ClippingMode
  {
    get
    {
      string temp;
      GetProperty( Actor.Property.CLIPPING_MODE).Get( out temp );
      return temp;
    }
    set
    {
      SetProperty( Actor.Property.CLIPPING_MODE, new Dali.Property.Value( value ) );
    }
  }
  public bool BatchParent
  {
    get
    {
      bool temp = false;
      GetProperty( Actor.Property.BATCH_PARENT).Get( ref temp );
      return temp;
    }
    set
    {
      SetProperty( Actor.Property.BATCH_PARENT, new Dali.Property.Value( value ) );
    }
  }

/* Properties ends */

%}

%enddef

%define DALI_ACTOR_EVENTHANDLER_PARAM( NameSpace, ClassName)

ACTOR_EVENTHANDLER_TYPEMAP_EVENTARG( NameSpace, ClassName);
ACTOR_EVENTHANDLER_TYPEMAP_HELPER( NameSpace, ClassName);

%enddef

%define DALI_CONST_VECTOR4_REF_TO_COLOR_CONVERSION
%typemap(cstype) (const Vector4& color) "Dali.CSharp.Color"
%typemap(csin,
         pre=" Vector4 temp$csinput = new Vector4($csinput.R, $csinput.G, $csinput.B, $csinput.A);",
         cshin="ref $csinput"
        ) (const Vector4& color) "$csclassname.getCPtr(temp$csinput)"

%enddef

%define DALI_VECTOR2_TO_SIZE_CONVERSION
%typemap(cstype) (const Vector2& position) "Dali.CSharp.Size"
%typemap(csin,
         pre=" Vector2 temp$csinput = new Vector2($csinput.W, $csinput.H);",
         cshin="ref $csinput"
        ) (const Vector2& position) "$csclassname.getCPtr(temp$csinput)"

%enddef

namespace Dali
{
  DALI_ACTOR_EVENTHANDLER_PARAM( Dali, Actor);
  DALI_CONST_VECTOR4_REF_TO_COLOR_CONVERSION;
  DALI_VECTOR2_TO_SIZE_CONVERSION;
}
