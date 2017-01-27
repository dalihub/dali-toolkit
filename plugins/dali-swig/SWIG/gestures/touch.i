%rename(Touch) Dali::TouchData;

%typemap(cscode) Dali::TouchData %{
  public static Touch GetTouchFromPtr(global::System.IntPtr cPtr) {
    Touch ret = new Touch(cPtr, false);
    if (NDalicPINVOKE.SWIGPendingException.Pending) throw NDalicPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }
%}