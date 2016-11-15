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

%define PROGRESSBAR_PROPERTIES_HELPER(NameSpace, ClassName)
%typemap(cscode) NameSpace::ClassName %{

/* Properties earlier added by Ruby Script */

  public float ProgressValue
  {
    get
    {
      float temp = 0.0f;
      GetProperty( ProgressBar.Property.PROGRESS_VALUE).Get( ref temp );
      return temp;
    }
    set
    {
      SetProperty( ProgressBar.Property.PROGRESS_VALUE, new Dali.Property.Value( value ) );
    }
  }
  public Dali.Property.Map TrackVisual
  {
    get
    {
      Dali.Property.Map temp = new Dali.Property.Map();
      GetProperty( ProgressBar.Property.TRACK_VISUAL).Get(  temp );
      return temp;
    }
    set
    {
      SetProperty( ProgressBar.Property.TRACK_VISUAL, new Dali.Property.Value( value ) );
    }
  }
  public Dali.Property.Map ProgressVisual
  {
    get
    {
      Dali.Property.Map temp = new Dali.Property.Map();
      GetProperty( ProgressBar.Property.PROGRESS_VISUAL).Get(  temp );
      return temp;
    }
    set
    {
      SetProperty( ProgressBar.Property.PROGRESS_VISUAL, new Dali.Property.Value( value ) );
    }
  }

/* Properties ends */

%}
%enddef

namespace Dali
{
  PROGRESSBAR_PROPERTIES_HELPER(Dali::Toolkit, ProgressBar);
}
