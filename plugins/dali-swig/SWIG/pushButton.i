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

%define PUSHBUTTON_PROPERTIES_HELPER(NameSpace, ClassName)
%typemap(cscode) NameSpace::ClassName %{

/* Properties earlier added by Ruby Script */

  public string UnselectedIcon
  {
    get
    {
      string temp;
      GetProperty( PushButton.Property.UNSELECTED_ICON).Get( out temp );
      return temp;
    }
    set
    {
      SetProperty( PushButton.Property.UNSELECTED_ICON, new Dali.Property.Value( value ) );
    }
  }
  public string SelectedIcon
  {
    get
    {
      string temp;
      GetProperty( PushButton.Property.SELECTED_ICON).Get( out temp );
      return temp;
    }
    set
    {
      SetProperty( PushButton.Property.SELECTED_ICON, new Dali.Property.Value( value ) );
    }
  }
  public string IconAlignment
  {
    get
    {
      string temp;
      GetProperty( PushButton.Property.ICON_ALIGNMENT).Get( out temp );
      return temp;
    }
    set
    {
      SetProperty( PushButton.Property.ICON_ALIGNMENT, new Dali.Property.Value( value ) );
    }
  }
  public string LabelPadding
  {
    get
    {
      string temp;
      GetProperty( PushButton.Property.LABEL_PADDING).Get( out temp );
      return temp;
    }
    set
    {
      SetProperty( PushButton.Property.LABEL_PADDING, new Dali.Property.Value( value ) );
    }
  }
  public string IconPadding
  {
    get
    {
      string temp;
      GetProperty( PushButton.Property.ICON_PADDING).Get( out temp );
      return temp;
    }
    set
    {
      SetProperty( PushButton.Property.ICON_PADDING, new Dali.Property.Value( value ) );
    }
  }


/* Properties ends */

%}
%enddef

namespace Dali
{
  PUSHBUTTON_PROPERTIES_HELPER(Dali::Toolkit, PushButton);
}
