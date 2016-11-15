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

%define LAYER_PROPERTIES_HELPER(NameSpace, ClassName)
%typemap(cscode) NameSpace::ClassName %{

/* Properties earlier added by Ruby Script */

  public bool ClippingEnable
  {
    get
    {
      bool temp = false;
      GetProperty( Layer.Property.CLIPPING_ENABLE).Get( ref temp );
      return temp;
    }
    set
    {
      SetProperty( Layer.Property.CLIPPING_ENABLE, new Dali.Property.Value( value ) );
    }
  }
  public RectInteger ClippingBox
  {
    get
    {
      RectInteger temp = new RectInteger(0,0,0,0);
      GetProperty( Layer.Property.CLIPPING_BOX).Get(  temp );
      return temp;
    }
    set
    {
      SetProperty( Layer.Property.CLIPPING_BOX, new Dali.Property.Value( value ) );
    }
  }
  public string Behavior
  {
    get
    {
      string temp;
      GetProperty( Layer.Property.BEHAVIOR).Get( out temp );
      return temp;
    }
    set
    {
      SetProperty( Layer.Property.BEHAVIOR, new Dali.Property.Value( value ) );
    }
  }

/* Properties ends */

%}
%enddef

namespace Dali
{
  LAYER_PROPERTIES_HELPER(Dali, Layer);

}
