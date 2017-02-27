%csmethodmodifiers Dali::LongPressGesture::numberOfTouches "private";
%csmethodmodifiers Dali::LongPressGesture::screenPoint "private";
%csmethodmodifiers Dali::LongPressGesture::localPoint "private";

%typemap(cscode) Dali::LongPressGesture %{
  public static LongPressGesture GetLongPressGestureFromPtr(global::System.IntPtr cPtr) {
    LongPressGesture ret = new LongPressGesture(cPtr, false);
    if (NDalicPINVOKE.SWIGPendingException.Pending) throw NDalicPINVOKE.SWIGPendingException.Retrieve();
    return ret;
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