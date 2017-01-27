%csmethodmodifiers Dali::PinchGesture::scale "private";
%csmethodmodifiers Dali::PinchGesture::speed "private";
%csmethodmodifiers Dali::PinchGesture::screenCenterPoint "private";
%csmethodmodifiers Dali::PinchGesture::localCenterPoint "private";

%typemap(cscode) Dali::PinchGesture %{
  public static PinchGesture GetPinchGestureFromPtr(global::System.IntPtr cPtr) {
    PinchGesture ret = new PinchGesture(cPtr, false);
    if (NDalicPINVOKE.SWIGPendingException.Pending) throw NDalicPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public float Scale
  {
    get
    {
      return scale;
    }
  }

  public float Speed
  {
    get
    {
      return speed;
    }
  }

  public Vector2 ScreenCenterPoint
  {
    get
    {
      return screenCenterPoint;
    }
  }

  public Vector2 LocalCenterPoint
  {
    get
    {
      return localCenterPoint;
    }
  }
%}