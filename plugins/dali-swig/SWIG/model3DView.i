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

%define MODEL3DVIEW_PROPERTIES_HELPER(NameSpace, ClassName)
%typemap(cscode) NameSpace::ClassName %{

/* Properties earlier added by Ruby Script */

  public string GeometryUrl
  {
    get
    {
      string temp;
      GetProperty( Model3dView.Property.GEOMETRY_URL).Get( out temp );
      return temp;
    }
    set
    {
      SetProperty( Model3dView.Property.GEOMETRY_URL, new Dali.Property.Value( value ) );
    }
  }
  public string MaterialUrl
  {
    get
    {
      string temp;
      GetProperty( Model3dView.Property.MATERIAL_URL).Get( out temp );
      return temp;
    }
    set
    {
      SetProperty( Model3dView.Property.MATERIAL_URL, new Dali.Property.Value( value ) );
    }
  }
  public string ImagesUrl
  {
    get
    {
      string temp;
      GetProperty( Model3dView.Property.IMAGES_URL).Get( out temp );
      return temp;
    }
    set
    {
      SetProperty( Model3dView.Property.IMAGES_URL, new Dali.Property.Value( value ) );
    }
  }
  public int IlluminationType
  {
    get
    {
      int temp = 0;
      GetProperty( Model3dView.Property.ILLUMINATION_TYPE).Get( ref temp );
      return temp;
    }
    set
    {
      SetProperty( Model3dView.Property.ILLUMINATION_TYPE, new Dali.Property.Value( value ) );
    }
  }
  public string Texture0Url
  {
    get
    {
      string temp;
      GetProperty( Model3dView.Property.TEXTURE0_URL).Get( out temp );
      return temp;
    }
    set
    {
      SetProperty( Model3dView.Property.TEXTURE0_URL, new Dali.Property.Value( value ) );
    }
  }
  public string Texture1Url
  {
    get
    {
      string temp;
      GetProperty( Model3dView.Property.TEXTURE1_URL).Get( out temp );
      return temp;
    }
    set
    {
      SetProperty( Model3dView.Property.TEXTURE1_URL, new Dali.Property.Value( value ) );
    }
  }
  public string Texture2Url
  {
    get
    {
      string temp;
      GetProperty( Model3dView.Property.TEXTURE2_URL).Get( out temp );
      return temp;
    }
    set
    {
      SetProperty( Model3dView.Property.TEXTURE2_URL, new Dali.Property.Value( value ) );
    }
  }
  public Dali.CSharp.Position LightPosition
  {
    get
    {
      Vector3 temp = new Vector3(0.0f,0.0f,0.0f);
      GetProperty( Model3dView.Property.LIGHT_POSITION).Get(  temp );
      Dali.CSharp.Position ret = new Dali.CSharp.Position(temp.x, temp.y, temp.z);
      return ret;
    }
    set
    {
      SetProperty( Model3dView.Property.LIGHT_POSITION, new Dali.Property.Value( value ) );
    }
  }

/* Properties ends */

%}
%enddef

namespace Dali
{
  MODEL3DVIEW_PROPERTIES_HELPER(Dali::Toolkit, Model3dView);
}
