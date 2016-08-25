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

// common name mappings
#if defined(SWIGCSHARP)
%rename(Assign)            operator=;
%rename(Add)               operator+;
%rename(AddAssign)         operator+=;
%rename(Subtract)          operator-;
%rename(SubtractAssign)    operator-=;
%rename(Multiply)          operator*;
%rename(MultiplyAssign)    operator*=;
%rename(Divide)            operator/;
%rename(DivideAssign)      operator/=;
%rename(Assign)            operator=;
%rename(EqualTo)           operator==;
%rename(NotEqualTo)        operator!=;
%rename(LessThan)          operator<;
%rename(GreaterThan)       operator>;
%rename(ValueOfIndex)      operator[];
#endif

%typemap(cscode) Dali::Vector2 %{
  public static Vector2 operator+(Vector2 arg1, Vector2 arg2) {
    return arg1.Add(arg2);
  }
  
  public static Vector2 operator-(Vector2 arg1, Vector2 arg2) {
    return arg1.Subtract(arg2);
  }

  public static Vector2 operator-(Vector2 arg1) {
    return arg1.Subtract();
  }

  public static Vector2 operator*(Vector2 arg1, Vector2 arg2) {
    return arg1.Multiply(arg2);
  }

  public static Vector2 operator*(Vector2 arg1, float arg2) {
    return arg1.Multiply(arg2);
  }

  public static Vector2 operator/(Vector2 arg1, Vector2 arg2) {
    return arg1.Divide(arg2);
  }

  public static Vector2 operator/(Vector2 arg1, float arg2) {
    return arg1.Divide(arg2);
  }

  public float this[uint index]
  {
    get
    {
      return ValueOfIndex(index);
    }
  }

  public static Vector2 GetVector2FromPtr(global::System.IntPtr cPtr) {
    Vector2 ret = new Vector2(cPtr, false);
    if (NDalicPINVOKE.SWIGPendingException.Pending) throw NDalicPINVOKE.SWIGPendingException.Retrieve();
       return ret;
  }

%}

%typemap(cscode) Dali::Vector3 %{
  public static Vector3 operator+(Vector3 arg1, Vector3 arg2) {
    return arg1.Add(arg2);
  }

  public static Vector3 operator-(Vector3 arg1, Vector3 arg2) {
    return arg1.Subtract(arg2);
  }

  public static Vector3 operator-(Vector3 arg1) {
    return arg1.Subtract();
  }

  public static Vector3 operator*(Vector3 arg1, Vector3 arg2) {
    return arg1.Multiply(arg2);
  }

  public static Vector3 operator*(Vector3 arg1, float arg2) {
    return arg1.Multiply(arg2);
  }

  public static Vector3 operator/(Vector3 arg1, Vector3 arg2) {
    return arg1.Divide(arg2);
  }

  public static Vector3 operator/(Vector3 arg1, float arg2) {
    return arg1.Divide(arg2);
  }

  public float this[uint index]
  {
    get
    {
      return ValueOfIndex(index);
    }
  }

  public static Vector3 GetVector3FromPtr(global::System.IntPtr cPtr) {
    Vector3 ret = new Vector3(cPtr, false);
    if (NDalicPINVOKE.SWIGPendingException.Pending) throw NDalicPINVOKE.SWIGPendingException.Retrieve();
       return ret;
  }

%}

%typemap(cscode) Dali::Vector4 %{
  public static Vector4 operator+(Vector4 arg1, Vector4 arg2) {
    return arg1.Add(arg2);
  }
  
  public static Vector4 operator-(Vector4 arg1, Vector4 arg2) {
    return arg1.Subtract(arg2);
  }

  public static Vector4 operator-(Vector4 arg1) {
    return arg1.Subtract();
  }

  public static Vector4 operator*(Vector4 arg1, Vector4 arg2) {
    return arg1.Multiply(arg2);
  }

  public static Vector4 operator*(Vector4 arg1, float arg2) {
    return arg1.Multiply(arg2);
  }

  public static Vector4 operator/(Vector4 arg1, Vector4 arg2) {
    return arg1.Divide(arg2);
  }

  public static Vector4 operator/(Vector4 arg1, float arg2) {
    return arg1.Divide(arg2);
  }

  public float this[uint index]
  {
    get
    {
      return ValueOfIndex(index);
    }
  }

  public static Vector4 GetVector4FromPtr(global::System.IntPtr cPtr) {
    Vector4 ret = new Vector4(cPtr, false);
    if (NDalicPINVOKE.SWIGPendingException.Pending) throw NDalicPINVOKE.SWIGPendingException.Retrieve();
       return ret;
  }

%}

%typemap(cscode) Dali::Matrix %{
  public static Vector4 operator*(Matrix arg1, Vector4 arg2) {
    return arg1.Multiply(arg2);
  }
%}

%typemap(cscode) Dali::Quaternion %{
  public static Quaternion operator+(Quaternion arg1, Quaternion arg2) {
    return arg1.Add(arg2);
  }
  
  public static Quaternion operator-(Quaternion arg1, Quaternion arg2) {
    return arg1.Subtract(arg2);
  }

  public static Quaternion operator-(Quaternion arg1) {
    return arg1.Subtract();
  }

  public static Quaternion operator*(Quaternion arg1, Quaternion arg2) {
    return arg1.Multiply(arg2);
  }

  public static Vector3 operator*(Quaternion arg1, Vector3 arg2) {
    return arg1.Multiply(arg2);
  }

  public static Quaternion operator*(Quaternion arg1, float arg2) {
    return arg1.Multiply(arg2);
  }

  public static Quaternion operator/(Quaternion arg1, Quaternion arg2) {
    return arg1.Divide(arg2);
  }

  public static Quaternion operator/(Quaternion arg1, float arg2) {
    return arg1.Divide(arg2);
  }
%}

%typemap(cscode) Dali::Property::Array %{
  public Property.Value this[uint index]
  {
    get
    {
      return ValueOfIndex(index);
    }
  }
%}

%typemap(cscode) Dali::Property::Map %{
  public Property.Value this[string key]
  {
    get
    {
      return ValueOfIndex(key);
    }
  }

  public Property.Value this[int key]
  {
    get
    {
      return ValueOfIndex(key);
    }
  }
%}

%typemap(cscode) Dali::Uint16Pair %{
  public static bool operator<(Uint16Pair arg1, Uint16Pair arg2) {
    return arg1.LessThan(arg2);
  }
  
  public static bool operator>(Uint16Pair arg1, Uint16Pair arg2) {
    return arg1.GreaterThan(arg2);
  }
 
%}

/**
 * Extend the C++ base handle to include  a IsHandleEmpty() function
 * This is because from C# we can't wrap the operator BooleanType() function
 */
%extend Dali::BaseHandle {
   bool IsHandleEmpty() const {
     if( *$self )
     {
       return true;
     }
     else
     {
       return false;
     }
    }
};

/**
 * Extend C# base handle to support true / false testing of base handle
 * so we can do
 *  if ( actor )
 *  {
 *    ...
 *  }
 */
%typemap(cscode) Dali::BaseHandle %{

 public static bool operator true(BaseHandle  handle)
 {
   if( handle!= null  )
   {
     return  handle.IsHandleEmpty();
   }
   else
   {
     return false;
   }
 }
 public static bool operator false(BaseHandle  handle)
 {
   return  handle.IsHandleEmpty();
 }
%}


