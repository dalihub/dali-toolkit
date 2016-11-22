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

%define PATHCONSTRAINER_PROPERTIES_HELPER(NameSpace, ClassName)
%typemap(cscode) NameSpace::ClassName %{

/* Properties earlier added by Ruby Script */

  public Dali.CSharp.Position Forward
  {
    get
    {
      Vector3 temp = new Vector3(0.0f,0.0f,0.0f);
      GetProperty( PathConstrainer.Property.FORWARD).Get(  temp );
      Dali.CSharp.Position ret = new Dali.CSharp.Position(temp.x, temp.y, temp.z);
      return ret;
    }
    set
    {
      SetProperty( PathConstrainer.Property.FORWARD, new Dali.Property.Value( value ) );
    }
  }
  public Dali.Property.Array Points
  {
    get
    {
      Dali.Property.Array temp = new Dali.Property.Array();
      GetProperty( PathConstrainer.Property.POINTS).Get(  temp );
      return temp;
    }
    set
    {
      SetProperty( PathConstrainer.Property.POINTS, new Dali.Property.Value( value ) );
    }
  }
  public Dali.Property.Array ControlPoints
  {
    get
    {
      Dali.Property.Array temp = new Dali.Property.Array();
      GetProperty( PathConstrainer.Property.CONTROL_POINTS).Get(  temp );
      return temp;
    }
    set
    {
      SetProperty( PathConstrainer.Property.CONTROL_POINTS, new Dali.Property.Value( value ) );
    }
  }

/* Properties ends */

%}
%enddef

namespace Dali
{
  PATHCONSTRAINER_PROPERTIES_HELPER(Dali, PathConstrainer);
}
