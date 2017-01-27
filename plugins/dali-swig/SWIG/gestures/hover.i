%rename(Hover) Dali::HoverEvent;

%csmethodmodifiers Dali::HoverEvent::points "private";
%csmethodmodifiers Dali::HoverEvent::time "private";

%typemap(cscode) Dali::HoverEvent %{
  public static Hover GetHoverFromPtr(global::System.IntPtr cPtr) {
    Hover ret = new Hover(cPtr, false);
    if (NDalicPINVOKE.SWIGPendingException.Pending) throw NDalicPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public uint Time
  {
    get
    {
      return time;
    }
  }

  public int GetDeviceId(uint point) {
    if( point < points.Count )
    {
      return points[(int)point].deviceId;
    }
    return -1;
  }

  public PointStateType GetState(uint point) {
    if( point < points.Count )
    {
      return (Dali.PointStateType)(points[(int)point].state);
    }
    return PointStateType.FINISHED;
  }

  public Actor GetHitActor(uint point) {
    if( point < points.Count )
    {
      return points[(int)point].hitActor;
    }
    else
    {
      // Return a native empty handle
      Actor actor = new Actor();
      actor.Reset();
      return actor;
    }
  }

  public Vector2 GetLocalPosition(uint point) {
    if( point < points.Count )
    {
      return points[(int)point].local;
    }
    return new Vector2(0.0f, 0.0f);
  }

  public Vector2 GetScreenPosition(uint point) {
    if( point < points.Count )
    {
      return points[(int)point].screen;
    }
    return new Vector2(0.0f, 0.0f);
  }
%}