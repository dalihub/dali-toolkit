namespace Dali {

class NDalicManualPINVOKE {

  [global::System.Runtime.InteropServices.DllImport("NDalic", EntryPoint="CSharp_new_KeyboardFocusManager")]
  public static extern global::System.IntPtr new_KeyboardFocusManager();

  [global::System.Runtime.InteropServices.DllImport("NDalic", EntryPoint="CSharp_delete_KeyboardFocusManager")]
  public static extern void delete_KeyboardFocusManager(global::System.Runtime.InteropServices.HandleRef jarg1);

  [global::System.Runtime.InteropServices.DllImport("NDalic", EntryPoint="CSharp_KeyboardFocusManager_Get")]
  public static extern global::System.IntPtr KeyboardFocusManager_Get();

  [global::System.Runtime.InteropServices.DllImport("NDalic", EntryPoint="CSharp_KeyboardFocusManager_SetCurrentFocusActor")]
  public static extern bool KeyboardFocusManager_SetCurrentFocusActor(global::System.Runtime.InteropServices.HandleRef jarg1, global::System.Runtime.InteropServices.HandleRef jarg2);

  [global::System.Runtime.InteropServices.DllImport("NDalic", EntryPoint="CSharp_KeyboardFocusManager_GetCurrentFocusActor")]
  public static extern global::System.IntPtr KeyboardFocusManager_GetCurrentFocusActor(global::System.Runtime.InteropServices.HandleRef jarg1);

  [global::System.Runtime.InteropServices.DllImport("NDalic", EntryPoint="CSharp_KeyboardFocusManager_MoveFocus")]
  public static extern bool KeyboardFocusManager_MoveFocus(global::System.Runtime.InteropServices.HandleRef jarg1, int jarg2);

  [global::System.Runtime.InteropServices.DllImport("NDalic", EntryPoint="CSharp_KeyboardFocusManager_ClearFocus")]
  public static extern void KeyboardFocusManager_ClearFocus(global::System.Runtime.InteropServices.HandleRef jarg1);

  [global::System.Runtime.InteropServices.DllImport("NDalic", EntryPoint="CSharp_KeyboardFocusManager_SetFocusGroupLoop")]
  public static extern void KeyboardFocusManager_SetFocusGroupLoop(global::System.Runtime.InteropServices.HandleRef jarg1, bool jarg2);

  [global::System.Runtime.InteropServices.DllImport("NDalic", EntryPoint="CSharp_KeyboardFocusManager_GetFocusGroupLoop")]
  public static extern bool KeyboardFocusManager_GetFocusGroupLoop(global::System.Runtime.InteropServices.HandleRef jarg1);

  [global::System.Runtime.InteropServices.DllImport("NDalic", EntryPoint="CSharp_KeyboardFocusManager_SetAsFocusGroup")]
  public static extern void KeyboardFocusManager_SetAsFocusGroup(global::System.Runtime.InteropServices.HandleRef jarg1, global::System.Runtime.InteropServices.HandleRef jarg2, bool jarg3);

  [global::System.Runtime.InteropServices.DllImport("NDalic", EntryPoint="CSharp_KeyboardFocusManager_IsFocusGroup")]
  public static extern bool KeyboardFocusManager_IsFocusGroup(global::System.Runtime.InteropServices.HandleRef jarg1, global::System.Runtime.InteropServices.HandleRef jarg2);

  [global::System.Runtime.InteropServices.DllImport("NDalic", EntryPoint="CSharp_KeyboardFocusManager_GetFocusGroup")]
  public static extern global::System.IntPtr KeyboardFocusManager_GetFocusGroup(global::System.Runtime.InteropServices.HandleRef jarg1, global::System.Runtime.InteropServices.HandleRef jarg2);

  [global::System.Runtime.InteropServices.DllImport("NDalic", EntryPoint="CSharp_KeyboardFocusManager_SetFocusIndicatorActor")]
  public static extern void KeyboardFocusManager_SetFocusIndicatorActor(global::System.Runtime.InteropServices.HandleRef jarg1, global::System.Runtime.InteropServices.HandleRef jarg2);

  [global::System.Runtime.InteropServices.DllImport("NDalic", EntryPoint="CSharp_KeyboardFocusManager_GetFocusIndicatorActor")]
  public static extern global::System.IntPtr KeyboardFocusManager_GetFocusIndicatorActor(global::System.Runtime.InteropServices.HandleRef jarg1);

  [global::System.Runtime.InteropServices.DllImport("NDalic", EntryPoint="CSharp_KeyboardFocusManager_PreFocusChangeSignal")]
  public static extern global::System.IntPtr KeyboardFocusManager_PreFocusChangeSignal(global::System.Runtime.InteropServices.HandleRef jarg1);

  [global::System.Runtime.InteropServices.DllImport("NDalic", EntryPoint="CSharp_KeyboardFocusManager_FocusChangedSignal")]
  public static extern global::System.IntPtr KeyboardFocusManager_FocusChangedSignal(global::System.Runtime.InteropServices.HandleRef jarg1);

  [global::System.Runtime.InteropServices.DllImport("NDalic", EntryPoint="CSharp_KeyboardFocusManager_FocusGroupChangedSignal")]
  public static extern global::System.IntPtr KeyboardFocusManager_FocusGroupChangedSignal(global::System.Runtime.InteropServices.HandleRef jarg1);

  [global::System.Runtime.InteropServices.DllImport("NDalic", EntryPoint="CSharp_KeyboardFocusManager_FocusedActorEnterKeySignal")]
  public static extern global::System.IntPtr KeyboardFocusManager_FocusedActorEnterKeySignal(global::System.Runtime.InteropServices.HandleRef jarg1);

  [global::System.Runtime.InteropServices.DllImport("NDalic", EntryPoint="CSharp_KeyboardPreFocusChangeSignal_Empty")]
  public static extern bool KeyboardPreFocusChangeSignal_Empty(global::System.Runtime.InteropServices.HandleRef jarg1);

  [global::System.Runtime.InteropServices.DllImport("NDalic", EntryPoint="CSharp_KeyboardPreFocusChangeSignal_GetConnectionCount")]
  public static extern uint KeyboardPreFocusChangeSignal_GetConnectionCount(global::System.Runtime.InteropServices.HandleRef jarg1);

  [global::System.Runtime.InteropServices.DllImport("NDalic", EntryPoint="CSharp_KeyboardPreFocusChangeSignal_Connect")]
  public static extern void KeyboardPreFocusChangeSignal_Connect(global::System.Runtime.InteropServices.HandleRef jarg1, KeyboardFocusManager.PreFocusChangeEventCallbackDelegate delegate1);

  [global::System.Runtime.InteropServices.DllImport("NDalic", EntryPoint="CSharp_KeyboardPreFocusChangeSignal_Disconnect")]
  public static extern void KeyboardPreFocusChangeSignal_Disconnect(global::System.Runtime.InteropServices.HandleRef jarg1, global::System.Runtime.InteropServices.HandleRef jarg2);

  [global::System.Runtime.InteropServices.DllImport("NDalic", EntryPoint="CSharp_KeyboardPreFocusChangeSignal_Emit")]
  public static extern global::System.IntPtr KeyboardPreFocusChangeSignal_Emit(global::System.Runtime.InteropServices.HandleRef jarg1, global::System.Runtime.InteropServices.HandleRef jarg2, global::System.Runtime.InteropServices.HandleRef jarg3, int jarg4);

  [global::System.Runtime.InteropServices.DllImport("NDalic", EntryPoint="CSharp_new_KeyboardPreFocusChangeSignal")]
  public static extern global::System.IntPtr new_KeyboardPreFocusChangeSignal();

  [global::System.Runtime.InteropServices.DllImport("NDalic", EntryPoint="CSharp_delete_KeyboardPreFocusChangeSignal")]
  public static extern void delete_KeyboardPreFocusChangeSignal(global::System.Runtime.InteropServices.HandleRef jarg1);

  [global::System.Runtime.InteropServices.DllImport("NDalic", EntryPoint="CSharp_KeyboardFocusManager_SWIGUpcast")]
  public static extern global::System.IntPtr KeyboardFocusManager_SWIGUpcast(global::System.IntPtr jarg1);

}

}
