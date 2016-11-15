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

%define SHADER_PROPERTIES_HELPER(NameSpace, ClassName)
%typemap(cscode) NameSpace::ClassName %{

/* Properties earlier added by Ruby Script */

  public Dali.Property.Map Program
  {
    get
    {
      Dali.Property.Map temp = new Dali.Property.Map();
      GetProperty( Shader.Property.PROGRAM).Get(  temp );
      return temp;
    }
    set
    {
      SetProperty( Shader.Property.PROGRAM, new Dali.Property.Value( value ) );
    }
  }

/* Properties ends */
%}
%enddef

namespace Dali
{
  SHADER_PROPERTIES_HELPER(Dali, Shader);
}
