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

%define TableView_PROPERTIES_HELPER(NameSpace, ClassName)
%typemap(cscode) NameSpace::ClassName %{

/* Properties earlier added by Ruby Script */

  public int Rows
  {
    get
    {
      int temp = 0;
      GetProperty( TableView.Property.ROWS).Get( ref temp );
      return temp;
    }
    set
    {
      SetProperty( TableView.Property.ROWS, new Dali.Property.Value( value ) );
    }
  }
  public int Columns
  {
    get
    {
      int temp = 0;
      GetProperty( TableView.Property.COLUMNS).Get( ref temp );
      return temp;
    }
    set
    {
      SetProperty( TableView.Property.COLUMNS, new Dali.Property.Value( value ) );
    }
  }
  public Dali.CSharp.Size CellPadding
  {
    get
    {
      Vector2 temp = new Vector2(0.0f,0.0f);
      GetProperty( TableView.Property.CELL_PADDING).Get(  temp );
      Dali.CSharp.Size ret = new Dali.CSharp.Size(temp.x, temp.y);
      return ret;
    }
    set
    {
      SetProperty( TableView.Property.CELL_PADDING, new Dali.Property.Value( value ) );
    }
  }
  public Dali.Property.Map LayoutRows
  {
    get
    {
      Dali.Property.Map temp = new Dali.Property.Map();
      GetProperty( TableView.Property.LAYOUT_ROWS).Get(  temp );
      return temp;
    }
    set
    {
      SetProperty( TableView.Property.LAYOUT_ROWS, new Dali.Property.Value( value ) );
    }
  }
  public Dali.Property.Map LayoutColumns
  {
    get
    {
      Dali.Property.Map temp = new Dali.Property.Map();
      GetProperty( TableView.Property.LAYOUT_COLUMNS).Get(  temp );
      return temp;
    }
    set
    {
      SetProperty( TableView.Property.LAYOUT_COLUMNS, new Dali.Property.Value( value ) );
    }
  }

/* Properties ends */
%}
%enddef

namespace Dali
{
  TableView_PROPERTIES_HELPER(Dali::Toolkit, TableView);
}
