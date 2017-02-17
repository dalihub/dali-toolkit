%rename(Key) Dali::KeyEvent;
%rename(StateType) Dali::KeyEvent::State;

%csmethodmodifiers Dali::KeyEvent::keyPressedName "private";
%csmethodmodifiers Dali::KeyEvent::keyPressed "private";
%csmethodmodifiers Dali::KeyEvent::keyCode "private";
%csmethodmodifiers Dali::KeyEvent::keyModifier "private";
%csmethodmodifiers Dali::KeyEvent::time "private";
%csmethodmodifiers Dali::KeyEvent::state "private";

%typemap(cscode) Dali::KeyEvent %{
  public static Key GetKeyFromPtr(global::System.IntPtr cPtr) {
    Key ret = new Key(cPtr, false);
    if (NDalicPINVOKE.SWIGPendingException.Pending) throw NDalicPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public string KeyPressedName
  {
    get
    {
      return keyPressedName;
    }
  }

  public string KeyPressed
  {
    get
    {
      return keyPressed;
    }
  }

  public int KeyCode
  {
    get
    {
      return keyCode;
    }
  }

  public int KeyModifier
  {
    get
    {
      return keyModifier;
    }
  }

  public uint Time
  {
    get
    {
      return time;
    }
  }

  public Key.StateType State
  {
    get
    {
      return state;
    }
  }
%}