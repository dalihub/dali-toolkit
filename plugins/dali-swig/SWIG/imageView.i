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

%define IMAGEVIEW_PROPERTIES_HELPER(NameSpace, ClassName)
%typemap(cscode) NameSpace::ClassName %{

/* Properties earlier added by Ruby Script */

  public string ResourceUrl
  {
    get
    {
      string temp;
      GetProperty( ImageView.Property.RESOURCE_URL).Get( out temp );
      return temp;
    }
    set
    {
      SetProperty( ImageView.Property.RESOURCE_URL, new Dali.Property.Value( value ) );
    }
  }
  public Dali.Property.Map ImageMap
  {
    get
    {
      Dali.Property.Map temp = new Dali.Property.Map();
      GetProperty( ImageView.Property.IMAGE).Get(  temp );
      return temp;
    }
    set
    {
      SetProperty( ImageView.Property.IMAGE, new Dali.Property.Value( value ) );
    }
  }
  public bool PreMultipliedAlpha
  {
    get
    {
      bool temp = false;
      GetProperty( ImageView.Property.PRE_MULTIPLIED_ALPHA).Get( ref temp );
      return temp;
    }
    set
    {
      SetProperty( ImageView.Property.PRE_MULTIPLIED_ALPHA, new Dali.Property.Value( value ) );
    }
  }
  public Vector4 PixelArea
  {
    get
    {
      Vector4 temp = new Vector4(0.0f,0.0f,0.0f,0.0f);
      GetProperty( ImageView.Property.PIXEL_AREA).Get(  temp );
      return temp;
    }
    set
    {
      SetProperty( ImageView.Property.PIXEL_AREA, new Dali.Property.Value( value ) );
    }
  }
/* Properties ends */

%}
%enddef

namespace Dali
{
  IMAGEVIEW_PROPERTIES_HELPER(Dali::Toolkit, ImageView);
}

