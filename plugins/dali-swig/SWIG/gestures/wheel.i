%rename(Wheel) Dali::WheelEvent;
%rename(WheelType) Dali::WheelEvent::Type;

%csmethodmodifiers Dali::WheelEvent::type "private";
%csmethodmodifiers Dali::WheelEvent::direction "private";
%csmethodmodifiers Dali::WheelEvent::modifiers "private";
%csmethodmodifiers Dali::WheelEvent::point "private";
%csmethodmodifiers Dali::WheelEvent::z "private";
%csmethodmodifiers Dali::WheelEvent::timeStamp "private";

%typemap(cscode) Dali::WheelEvent %{
  public static Wheel GetWheelFromPtr(global::System.IntPtr cPtr) {
    Wheel ret = new Wheel(cPtr, false);
    if (NDalicPINVOKE.SWIGPendingException.Pending) throw NDalicPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public Wheel.WheelType Type
  {
    get
    {
      return type;
    }
  }

  public int Direction
  {
    get
    {
      return direction;
    }
  }

  public uint Modifiers
  {
    get
    {
      return modifiers;
    }
  }

  public Vector2 Point
  {
    get
    {
      return point;
    }
  }

  public int Z
  {
    get
    {
      return z;
    }
  }

  public uint TimeStamp
  {
    get
    {
      return timeStamp;
    }
  }
%}