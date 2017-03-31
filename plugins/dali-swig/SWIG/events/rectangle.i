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
%csmethodmodifiers Dali::Rect<int>::x "private";
%csmethodmodifiers Dali::Rect<int>::y "private";
%csmethodmodifiers Dali::Rect<int>::left "private";
%csmethodmodifiers Dali::Rect<int>::right "private";
%csmethodmodifiers Dali::Rect<int>::width "private";
%csmethodmodifiers Dali::Rect<int>::bottom "private";
%csmethodmodifiers Dali::Rect<int>::height "private";
%csmethodmodifiers Dali::Rect<int>::top "private";

%define DALI_RECTANGLE_PROPERTY_PARAM(NameSpace,ClassName)
  %typemap(cscode) NameSpace::ClassName %{
    public Rectangle(float x, float y, float width, float height) : this( (int)x, (int)y, (int)width, (int)height )
    {
    }

    public static bool operator ==(Rectangle a, Rectangle b)
    {
        // If both are null, or both are same instance, return true.
        if (System.Object.ReferenceEquals(a, b))
        {
            return true;
        }

        // If one is null, but not both, return false.
        if (((object)a == null) || ((object)b == null))
        {
            return false;
        }

        // Return true if the fields match:
        return a.X == b.X && a.Y == b.Y && a.Width == b.Width && a.Height == b.Height;
    }

    public static bool operator !=(Rectangle a, Rectangle b)
    {
        return !(a == b);
    }

    ///< X position of the rectangle, values after the decimal point are ignored, float type provided for convenience.
    public float X
    {
        set
        {
            x = (int)( value );
        }
        get
        {
            return x;
        }
    }

    ///< Y position of the rectangle, values after the decimal point are ignored, float type provided for convenience.
    public float Y
    {
        set
        {
            y = (int)( value );
        }
        get
        {
            return y;
        }
    }

    ///< Width of the rectangle, values after the decimal point are ignored, float type provided for convenience.
    public float Width
    {
        set
        {
            width = (int)( value );
        }
        get
        {
            return width;
        }
    }

    ///< Height of the rectangle, values after the decimal point are ignored, float type provided for convenience.
    public float Height
    {
        set
        {
            height = (int)( value );
        }
        get
        {
            return height;
        }
    }
%}

%enddef

namespace Dali
{
  DALI_RECTANGLE_PROPERTY_PARAM( Dali, Rect<int>);
}
