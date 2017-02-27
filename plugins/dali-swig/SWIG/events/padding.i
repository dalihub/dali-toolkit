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
%csmethodmodifiers Dali::Rect<float>::x "private";
%csmethodmodifiers Dali::Rect<float>::y "private";
%csmethodmodifiers Dali::Rect<float>::left "private";
%csmethodmodifiers Dali::Rect<float>::right "private";
%csmethodmodifiers Dali::Rect<float>::width "private";
%csmethodmodifiers Dali::Rect<float>::bottom "private";
%csmethodmodifiers Dali::Rect<float>::height "private";
%csmethodmodifiers Dali::Rect<float>::top "private";
%ignore Dali::Rect<float>::IsEmpty;
%ignore Dali::Rect<float>::Left;
%ignore Dali::Rect<float>::Right;
%ignore Dali::Rect<float>::Top;
%ignore Dali::Rect<float>::Bottom;
%ignore Dali::Rect<float>::Area;
%ignore Dali::Rect<float>::Intersects;
%ignore Dali::Rect<float>::Contains;

%define DALI_PADDINGTYPE_PROPERTY_PARAM(NameSpace,ClassName)
  %typemap(cscode) NameSpace::ClassName %{
    public static bool operator ==(PaddingType a, PaddingType b)
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
        return ( System.Math.Abs( a.Left - b.Left ) < NDalic.GetRangedEpsilon(a.Left, b.Left) )&&
               ( System.Math.Abs( a.Right - b.Right ) < NDalic.GetRangedEpsilon(a.Right, b.Right) )&&
               ( System.Math.Abs( a.Bottom - b.Bottom ) < NDalic.GetRangedEpsilon(a.Bottom, b.Bottom) )&&
               ( System.Math.Abs( a.Top - b.Top ) < NDalic.GetRangedEpsilon(a.Top, b.Top) );
    }

    public static bool operator !=(PaddingType a, PaddingType b)
    {
        return !(a == b);
    }

    ///< The Left value
    public float Left
    {
        set
        {
            left = value;
        }
        get
        {
            return left;
        }
    }

    ///< The Right value
    public float Right
    {
        set
        {
            right = value;
        }
        get
        {
            return right;
        }
    }

    ///< The Bottom value
    public float Bottom
    {
        set
        {
            bottom = value;
        }
        get
        {
            return bottom;
        }
    }

    ///< The Top value
    public float Top
    {
        set
        {
            top = value;
        }
        get
        {
            return top;
        }
    }

%}

%enddef

namespace Dali
{
  DALI_PADDINGTYPE_PROPERTY_PARAM( Dali, Rect<float>);
}
