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

%define FLEXCONTAINER_PROPERTIES_HELPER(NameSpace, ClassName)
%typemap(cscode) NameSpace::ClassName %{

/* Properties earlier added by Ruby Script */

  public int ContentDirection
  {
    get
    {
      int temp = 0;
      GetProperty( FlexContainer.Property.CONTENT_DIRECTION).Get( ref temp );
      return temp;
    }
    set
    {
      SetProperty( FlexContainer.Property.CONTENT_DIRECTION, new Dali.Property.Value( value ) );
    }
  }
  public int FlexDirection
  {
    get
    {
      int temp = 0;
      GetProperty( FlexContainer.Property.FLEX_DIRECTION).Get( ref temp );
      return temp;
    }
    set
    {
      SetProperty( FlexContainer.Property.FLEX_DIRECTION, new Dali.Property.Value( value ) );
    }
  }
  public int FlexWrap
  {
    get
    {
      int temp = 0;
      GetProperty( FlexContainer.Property.FLEX_WRAP).Get( ref temp );
      return temp;
    }
    set
    {
      SetProperty( FlexContainer.Property.FLEX_WRAP, new Dali.Property.Value( value ) );
    }
  }
  public int JustifyContent
  {
    get
    {
      int temp = 0;
      GetProperty( FlexContainer.Property.JUSTIFY_CONTENT).Get( ref temp );
      return temp;
    }
    set
    {
      SetProperty( FlexContainer.Property.JUSTIFY_CONTENT, new Dali.Property.Value( value ) );
    }
  }
  public int AlignItems
  {
    get
    {
      int temp = 0;
      GetProperty( FlexContainer.Property.ALIGN_ITEMS).Get( ref temp );
      return temp;
    }
    set
    {
      SetProperty( FlexContainer.Property.ALIGN_ITEMS, new Dali.Property.Value( value ) );
    }
  }
  public int AlignContent
  {
    get
    {
      int temp = 0;
      GetProperty( FlexContainer.Property.ALIGN_CONTENT).Get( ref temp );
      return temp;
    }
    set
    {
      SetProperty( FlexContainer.Property.ALIGN_CONTENT, new Dali.Property.Value( value ) );
    }
  }

/* Properties ends */

%}
%enddef

namespace Dali
{
  FLEXCONTAINER_PROPERTIES_HELPER(Dali::Toolkit, FlexContainer);
}


