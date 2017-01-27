%rename(GestureType) Dali::Gesture::Type;
%rename(StateType) Dali::Gesture::State;

%csmethodmodifiers Dali::Gesture::type "private";
%csmethodmodifiers Dali::Gesture::state "private";
%csmethodmodifiers Dali::Gesture::time "private";

%typemap(cscode) Dali::Gesture %{
  public Gesture.GestureType Type
  {
    get
    {
      return type;
    }
  }

  public Gesture.StateType State
  {
    get
    {
      return state;
    }
  }

  public uint Time
  {
    get
    {
      return time;
    }
  }
%}