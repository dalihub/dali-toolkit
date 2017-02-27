%csmethodmodifiers Dali::PanGesture::velocity "private";
%csmethodmodifiers Dali::PanGesture::displacement "private";
%csmethodmodifiers Dali::PanGesture::position "private";
%csmethodmodifiers Dali::PanGesture::screenVelocity "private";
%csmethodmodifiers Dali::PanGesture::screenDisplacement "private";
%csmethodmodifiers Dali::PanGesture::screenPosition "private";
%csmethodmodifiers Dali::PanGesture::numberOfTouches "private";

%typemap(cscode) Dali::PanGesture %{
  public static PanGesture GetPanGestureFromPtr(global::System.IntPtr cPtr) {
    PanGesture ret = new PanGesture(cPtr, false);
    if (NDalicPINVOKE.SWIGPendingException.Pending) throw NDalicPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public Vector2 Velocity
  {
    get
    {
      return velocity;
    }
  }

  public Vector2 Displacement
  {
    get
    {
      return displacement;
    }
  }

  public Vector2 Position
  {
    get
    {
      return position;
    }
  }

  public Vector2 ScreenVelocity
  {
    get
    {
      return screenVelocity;
    }
  }

  public Vector2 ScreenDisplacement
  {
    get
    {
      return screenDisplacement;
    }
  }

  public Vector2 ScreenPosition
  {
    get
    {
      return screenPosition;
    }
  }

  public uint NumberOfTouches
  {
    get
    {
      return numberOfTouches;
    }
  }
%}