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

%define CAMERAACTOR_PROPERTIES_HELPER(NameSpace, ClassName)
%typemap(cscode) NameSpace::ClassName %{

/* Properties earlier added by Ruby Script */

  public string Type
  {
    get
    {
      string temp;
      GetProperty( CameraActor.Property.TYPE).Get( out temp );
      return temp;
    }
    set
    {
      SetProperty( CameraActor.Property.TYPE, new Dali.Property.Value( value ) );
    }
  }
  public string ProjectionMode
  {
    get
    {
      string temp;
      GetProperty( CameraActor.Property.PROJECTION_MODE).Get( out temp );
      return temp;
    }
    set
    {
      SetProperty( CameraActor.Property.PROJECTION_MODE, new Dali.Property.Value( value ) );
    }
  }
  public float FieldOfView
  {
    get
    {
      float temp = 0.0f;
      GetProperty( CameraActor.Property.FIELD_OF_VIEW).Get( ref temp );
      return temp;
    }
    set
    {
      SetProperty( CameraActor.Property.FIELD_OF_VIEW, new Dali.Property.Value( value ) );
    }
  }
  public float AspectRatio
  {
    get
    {
      float temp = 0.0f;
      GetProperty( CameraActor.Property.ASPECT_RATIO).Get( ref temp );
      return temp;
    }
    set
    {
      SetProperty( CameraActor.Property.ASPECT_RATIO, new Dali.Property.Value( value ) );
    }
  }
  public float NearPlaneDistance
  {
    get
    {
      float temp = 0.0f;
      GetProperty( CameraActor.Property.NEAR_PLANE_DISTANCE).Get( ref temp );
      return temp;
    }
    set
    {
      SetProperty( CameraActor.Property.NEAR_PLANE_DISTANCE, new Dali.Property.Value( value ) );
    }
  }
  public float FarPlaneDistance
  {
    get
    {
      float temp = 0.0f;
      GetProperty( CameraActor.Property.FAR_PLANE_DISTANCE).Get( ref temp );
      return temp;
    }
    set
    {
      SetProperty( CameraActor.Property.FAR_PLANE_DISTANCE, new Dali.Property.Value( value ) );
    }
  }
  public float LeftPlaneDistance
  {
    get
    {
      float temp = 0.0f;
      GetProperty( CameraActor.Property.LEFT_PLANE_DISTANCE).Get( ref temp );
      return temp;
    }
    set
    {
      SetProperty( CameraActor.Property.LEFT_PLANE_DISTANCE, new Dali.Property.Value( value ) );
    }
  }
  public float RightPlaneDistance
  {
    get
    {
      float temp = 0.0f;
      GetProperty( CameraActor.Property.RIGHT_PLANE_DISTANCE).Get( ref temp );
      return temp;
    }
    set
    {
      SetProperty( CameraActor.Property.RIGHT_PLANE_DISTANCE, new Dali.Property.Value( value ) );
    }
  }
  public float TopPlaneDistance
  {
    get
    {
      float temp = 0.0f;
      GetProperty( CameraActor.Property.TOP_PLANE_DISTANCE).Get( ref temp );
      return temp;
    }
    set
    {
      SetProperty( CameraActor.Property.TOP_PLANE_DISTANCE, new Dali.Property.Value( value ) );
    }
  }
  public float BottomPlaneDistance
  {
    get
    {
      float temp = 0.0f;
      GetProperty( CameraActor.Property.BOTTOM_PLANE_DISTANCE).Get( ref temp );
      return temp;
    }
    set
    {
      SetProperty( CameraActor.Property.BOTTOM_PLANE_DISTANCE, new Dali.Property.Value( value ) );
    }
  }
  public Dali.CSharp.Position TargetPosition
  {
    get
    {
      Vector3 temp = new Vector3(0.0f,0.0f,0.0f);
      GetProperty( CameraActor.Property.TARGET_POSITION).Get(  temp );
      Dali.CSharp.Position ret = new Dali.CSharp.Position(temp.x, temp.y, temp.z);
      return ret;
    }
    set
    {
      SetProperty( CameraActor.Property.TARGET_POSITION, new Dali.Property.Value( value ) );
    }
  }
  public Matrix ProjectionMatrix
  {
    get
    {
      Matrix temp = new Matrix();
      GetProperty( CameraActor.Property.PROJECTION_MATRIX).Get(  temp );
      return temp;
    }
  }
  public Matrix ViewMatrix
  {
    get
    {
      Matrix temp = new Matrix();
      GetProperty( CameraActor.Property.VIEW_MATRIX).Get(  temp );
      return temp;
    }
  }
  public bool InvertYAxis
  {
    get
    {
      bool temp = false;
      GetProperty( CameraActor.Property.INVERT_Y_AXIS).Get( ref temp );
      return temp;
    }
    set
    {
      SetProperty( CameraActor.Property.INVERT_Y_AXIS, new Dali.Property.Value( value ) );
    }
  }

/* Properties ends */

/* Constructor and APIs with high level class Size */

  public CameraActor (Dali.CSharp.Size size) : this (NDalicPINVOKE.CameraActor_New__SWIG_1(Vector2.getCPtr(new Vector2(size.W,size.H))), true) {
      if (NDalicPINVOKE.SWIGPendingException.Pending) throw NDalicPINVOKE.SWIGPendingException.Retrieve();
  }

  public void SetPerspectiveProjection(Dali.CSharp.Size size) {
    NDalicPINVOKE.CameraActor_SetPerspectiveProjection(swigCPtr, Vector2.getCPtr(new Vector2(size.W,size.H)));
    if (NDalicPINVOKE.SWIGPendingException.Pending) throw NDalicPINVOKE.SWIGPendingException.Retrieve();
  }

  public void SetOrthographicProjection(Dali.CSharp.Size size) {
    NDalicPINVOKE.CameraActor_SetOrthographicProjection__SWIG_0(swigCPtr, Vector2.getCPtr(new Vector2(size.W,size.H)));
    if (NDalicPINVOKE.SWIGPendingException.Pending) throw NDalicPINVOKE.SWIGPendingException.Retrieve();
  }

/* constructor and APIs end */


%}
%enddef

namespace Dali
{
  CAMERAACTOR_PROPERTIES_HELPER( Dali, CameraActor);

}
