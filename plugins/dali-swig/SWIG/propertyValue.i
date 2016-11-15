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

%define PropertyValue_CONSTRUCTOR_HELPER(NameSpace, ClassName)
%typemap(cscode) NameSpace::ClassName %{

/* APIs to help use the high level classes Size, Position and Color */

    public Value(Dali.CSharp.Size val): this(NDalicPINVOKE.new_Property_Value__SWIG_4(Vector2.getCPtr(new Vector2(val.W,val.H))), true) {
      if (NDalicPINVOKE.SWIGPendingException.Pending) throw NDalicPINVOKE.SWIGPendingException.Retrieve();
    }

    public Value(Dali.CSharp.Position val) : this(NDalicPINVOKE.new_Property_Value__SWIG_5(Vector3.getCPtr(new Vector3(val.X,val.Y,val.Z))), true) {
      if (NDalicPINVOKE.SWIGPendingException.Pending) throw NDalicPINVOKE.SWIGPendingException.Retrieve();
    }

    public Value(Dali.CSharp.Color val) : this(NDalicPINVOKE.new_Property_Value__SWIG_6(Vector4.getCPtr(new Vector4(val.R,val.G,val.B,val.A))), true) {
      if (NDalicPINVOKE.SWIGPendingException.Pending) throw NDalicPINVOKE.SWIGPendingException.Retrieve();
    }

    public bool Get(Dali.CSharp.Size val) {
      bool ret = NDalicPINVOKE.Property_Value_Get__SWIG_5(swigCPtr, Vector2.getCPtr(new Vector2(val.W, val.H)));
      if (NDalicPINVOKE.SWIGPendingException.Pending) throw NDalicPINVOKE.SWIGPendingException.Retrieve();
      return ret;
    }

    public bool Get(Dali.CSharp.Position val) {
      bool ret = NDalicPINVOKE.Property_Value_Get__SWIG_6(swigCPtr, Vector3.getCPtr(new Vector3(val.X,val.Y,val.Z)));
      if (NDalicPINVOKE.SWIGPendingException.Pending) throw NDalicPINVOKE.SWIGPendingException.Retrieve();
      return ret;
    }

    public bool Get(Dali.CSharp.Color val) {
      bool ret = NDalicPINVOKE.Property_Value_Get__SWIG_7(swigCPtr, Vector4.getCPtr(new Vector4(val.R,val.G,val.B,val.A)));
      if (NDalicPINVOKE.SWIGPendingException.Pending) throw NDalicPINVOKE.SWIGPendingException.Retrieve();
      return ret;
    }

/* APIs end */

%}
%enddef

namespace Dali
{
  PropertyValue_CONSTRUCTOR_HELPER(Dali::Property, Value);
}
