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
  public static Rotation operator+(Rotation arg1, Rotation arg2) {
    return arg1.Add(arg2);
  }

  public static Rotation operator-(Rotation arg1, Rotation arg2) {
    return arg1.Subtract(arg2);
  }

  public static Rotation operator-(Rotation arg1) {
    return arg1.Subtract();
  }

  public static Rotation operator*(Rotation arg1, Rotation arg2) {
    return arg1.Multiply(arg2);
  }

  public static Vector3 operator*(Rotation arg1, Vector3 arg2) {
    return arg1.Multiply(arg2);
  }

  public static Rotation operator*(Rotation arg1, float arg2) {
    return arg1.Multiply(arg2);
  }

  public static Rotation operator/(Rotation arg1, Rotation arg2) {
    return arg1.Divide(arg2);
  }

  public static Rotation operator/(Rotation arg1, float arg2) {
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
   bool HasBody() const {

     // C++ code. DALi uses Handle <-> Body design pattern.
     // This function checks the Handle to see if it has a body attached ( possible to have empty handles).
     // Handles in DALi can be converted into a boolean type
     // to check if the handle has a valid body attached to it.
     // Internally checking *$self will  checks IntrusivePtr<Dali::RefObject> mObjectHandle in BaseHandle;
     if( *$self )
     {
       return true;
     }
     else
     {
       return false;
     }
    }

     // Check if two handles point to the same body / ref-object.
     bool IsEqual( const BaseHandle& rhs ) const {

     // C++ code. Check if two handles reference the same implemtion
     if( *$self == rhs)
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

  // Returns the bool value true to indicate that an operand is true and returns false otherwise.
  public static bool operator true(BaseHandle handle)
  {
    // if the C# object is null, return false
    if( BaseHandle.ReferenceEquals( handle, null ) )
    {
      return false;
    }
    // returns true if the handle has a body, false otherwise
    return handle.HasBody();
  }

  // Returns the bool false  to indicate that an operand is false and returns true otherwise.
  public static bool operator false(BaseHandle  handle)
  {
    // if the C# object is null, return true
    if( BaseHandle.ReferenceEquals( handle, null ) )
    {
      return true;
    }
    return !handle.HasBody();
  }

  // Explicit conversion from Handle to bool.
  public static explicit operator bool(BaseHandle handle)
  {
     // if the C# object is null, return false
    if( BaseHandle.ReferenceEquals( handle, null ) )
    {
      return false;
    }
    // returns true if the handle has a body, false otherwise
    return handle.HasBody();
  }

  // Equality operator
  public static bool operator == (BaseHandle x, BaseHandle y)
  {
    // if the C# objects are the same return true
    if(  BaseHandle.ReferenceEquals( x, y ) )
    {
      return true;
    }
    if ( !BaseHandle.ReferenceEquals( x, null ) && !BaseHandle.ReferenceEquals( y, null ) )
    {
      // drop into native code to see if both handles point to the same body
      return x.IsEqual( y) ;
    }
    return false;

  }

  // Inequality operator. Returns Null if either operand is Null
  public static bool operator !=(BaseHandle x, BaseHandle y)
  {
    return !(x==y);
  }

  // Logical AND operator for &&
  // It's possible when doing a && this function (opBitwiseAnd) is never called due
  // to short circuiting. E.g.
  // If you perform x && y What actually is called is
  // BaseHandle.op_False( x ) ? BaseHandle.op_True( x ) : BaseHandle.opTrue( BaseHandle.opBitwiseAnd(x,y) )
  //
  public static BaseHandle operator &(BaseHandle x, BaseHandle y)
  {
    if( x == y )
    {
      return x;
    }
    return null;
  }

  // Logical OR operator for ||
  // It's possible when doing a || this function (opBitwiseOr) is never called due
  // to short circuiting. E.g.
  // If you perform x || y What actually is called is
  // BaseHandle.op_True( x ) ? BaseHandle.op_True( x ) : BaseHandle.opTrue( BaseHandle.opBitwiseOr(x,y) )
  public static BaseHandle operator |(BaseHandle x, BaseHandle y)
  {
    if ( !BaseHandle.ReferenceEquals( x, null ) || !BaseHandle.ReferenceEquals( y, null ) )
    {
       if( x.HasBody() )
       {
         return x;
       }
       if( y.HasBody() )
       {
         return y;
       }
       return null;
    }
    return null;
  }

  // Logical ! operator
  public static bool operator !(BaseHandle x)
  {
    // if the C# object is null, return true
    if( BaseHandle.ReferenceEquals( x, null ) )
    {
      return true;
    }
    if( x.HasBody() )
    {
      return false;
    }
    return true;
  }

%}