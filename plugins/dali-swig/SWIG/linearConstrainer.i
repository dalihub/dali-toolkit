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

%define LINEARCONSTRAINER_PROPERTIES_HELPER(NameSpace, ClassName)
%typemap(cscode) NameSpace::ClassName %{

/* Properties earlier added by Ruby Script */

  public Dali.Property.Array Value
  {
    get
    {
      Dali.Property.Array temp = new Dali.Property.Array();
      GetProperty( LinearConstrainer.Property.VALUE).Get(  temp );
      return temp;
    }
    set
    {
      SetProperty( LinearConstrainer.Property.VALUE, new Dali.Property.Value( value ) );
    }
  }
  public Dali.Property.Array Progress
  {
    get
    {
      Dali.Property.Array temp = new Dali.Property.Array();
      GetProperty( LinearConstrainer.Property.PROGRESS).Get(  temp );
      return temp;
    }
    set
    {
      SetProperty( LinearConstrainer.Property.PROGRESS, new Dali.Property.Value( value ) );
    }
  }


/* Properties ends */

%}
%enddef

namespace Dali
{
  LINEARCONSTRAINER_PROPERTIES_HELPER(Dali, LinearConstrainer);
}
