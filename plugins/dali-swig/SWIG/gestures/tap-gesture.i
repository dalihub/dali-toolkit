%csmethodmodifiers Dali::TapGesture::numberOfTaps "private";
%csmethodmodifiers Dali::TapGesture::numberOfTouches "private";
%csmethodmodifiers Dali::TapGesture::screenPoint "private";
%csmethodmodifiers Dali::TapGesture::localPoint "private";

%typemap(cscode) Dali::TapGesture %{
  public static TapGesture GetTapGestureFromPtr(global::System.IntPtr cPtr) {
    TapGesture ret = new TapGesture(cPtr, false);
    if (NDalicPINVOKE.SWIGPendingException.Pending) throw NDalicPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public uint NumberOfTaps
  {
    get
    {
      return numberOfTaps;
    }
  }

  public uint NumberOfTouches
  {
    get
    {
      return numberOfTouches;
    }
  }

  public Vector2 ScreenPoint
  {
    get
    {
      return screenPoint;
    }
  }

  public Vector2 LocalPoint
  {
    get
    {
      return localPoint;
    }
  }
%}