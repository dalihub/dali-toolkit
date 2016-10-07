#ifndef CSHARP_KEYBOARD_FOCUS_MANAGER
#define CSHARP_KEYBOARD_FOCUS_MANAGER
#endif

#include "common.h"

SWIGINTERN bool Dali_Signal_Sl_Dali_Actor_Sp_Dali_Actor_Sc_Dali_Actor_Sc_Dali_Toolkit_Control_KeyboardFocus_Direction_SP__Sg__Empty(Dali::Signal< Dali::Actor (Dali::Actor,Dali::Actor,Dali::Toolkit::Control::KeyboardFocus::Direction) > const *self){
         return self->Empty();
      }
SWIGINTERN std::size_t Dali_Signal_Sl_Dali_Actor_Sp_Dali_Actor_Sc_Dali_Actor_Sc_Dali_Toolkit_Control_KeyboardFocus_Direction_SP__Sg__GetConnectionCount(Dali::Signal< Dali::Actor (Dali::Actor,Dali::Actor,Dali::Toolkit::Control::KeyboardFocus::Direction) > const *self){
        return self->GetConnectionCount();
      }
SWIGINTERN void Dali_Signal_Sl_Dali_Actor_Sp_Dali_Actor_Sc_Dali_Actor_Sc_Dali_Toolkit_Control_KeyboardFocus_Direction_SP__Sg__Connect(Dali::Signal< Dali::Actor (Dali::Actor,Dali::Actor,Dali::Toolkit::Control::KeyboardFocus::Direction) > *self,Dali::Actor (*func)(Dali::Actor,Dali::Actor,Dali::Toolkit::Control::KeyboardFocus::Direction)){
          return self->Connect( func );
      }
SWIGINTERN void Dali_Signal_Sl_Dali_Actor_Sp_Dali_Actor_Sc_Dali_Actor_Sc_Dali_Toolkit_Control_KeyboardFocus_Direction_SP__Sg__Disconnect(Dali::Signal< Dali::Actor (Dali::Actor,Dali::Actor,Dali::Toolkit::Control::KeyboardFocus::Direction) > *self,Dali::Actor (*func)(Dali::Actor,Dali::Actor,Dali::Toolkit::Control::KeyboardFocus::Direction)){
          self->Disconnect( func );
      }
SWIGINTERN Dali::Actor Dali_Signal_Sl_Dali_Actor_Sp_Dali_Actor_Sc_Dali_Actor_Sc_Dali_Toolkit_Control_KeyboardFocus_Direction_SP__Sg__Emit(Dali::Signal< Dali::Actor (Dali::Actor,Dali::Actor,Dali::Toolkit::Control::KeyboardFocus::Direction) > *self,Dali::Actor arg1,Dali::Actor arg2,Dali::Toolkit::Control::KeyboardFocus::Direction arg3){
          return self->Emit( arg1, arg2, arg3 );
      }

#ifdef __cplusplus
extern "C" {
#endif

SWIGEXPORT void * SWIGSTDCALL CSharp_new_KeyboardFocusManager() {
  void * jresult;
  Dali::Toolkit::KeyboardFocusManager *result = 0 ;

  {
    try {
      result = (Dali::Toolkit::KeyboardFocusManager *)new Dali::Toolkit::KeyboardFocusManager();
    } catch (std::out_of_range& e) {
      {
        SWIG_CSharpException(SWIG_IndexError, const_cast<char*>(e.what())); return 0;
      };
    } catch (std::exception& e) {
      {
        SWIG_CSharpException(SWIG_RuntimeError, const_cast<char*>(e.what())); return 0;
      };
    } catch (...) {
      {
        SWIG_CSharpException(SWIG_UnknownError, "unknown error"); return 0;
      };
    }
  }
  jresult = (void *)result;
  return jresult;
}


SWIGEXPORT void SWIGSTDCALL CSharp_delete_KeyboardFocusManager(void * jarg1) {
  Dali::Toolkit::KeyboardFocusManager *arg1 = (Dali::Toolkit::KeyboardFocusManager *) 0 ;

  arg1 = (Dali::Toolkit::KeyboardFocusManager *)jarg1;
  {
    try {
      delete arg1;
    } catch (std::out_of_range& e) {
      {
        SWIG_CSharpException(SWIG_IndexError, const_cast<char*>(e.what())); return ;
      };
    } catch (std::exception& e) {
      {
        SWIG_CSharpException(SWIG_RuntimeError, const_cast<char*>(e.what())); return ;
      };
    } catch (...) {
      {
        SWIG_CSharpException(SWIG_UnknownError, "unknown error"); return ;
      };
    }
  }
}


SWIGEXPORT void * SWIGSTDCALL CSharp_KeyboardFocusManager_Get() {
  void * jresult;
  Dali::Toolkit::KeyboardFocusManager result;

  {
    try {
      result = Dali::Toolkit::KeyboardFocusManager::Get();
    } catch (std::out_of_range& e) {
      {
        SWIG_CSharpException(SWIG_IndexError, const_cast<char*>(e.what())); return 0;
      };
    } catch (std::exception& e) {
      {
        SWIG_CSharpException(SWIG_RuntimeError, const_cast<char*>(e.what())); return 0;
      };
    } catch (...) {
      {
        SWIG_CSharpException(SWIG_UnknownError, "unknown error"); return 0;
      };
    }
  }
  jresult = new Dali::Toolkit::KeyboardFocusManager((const Dali::Toolkit::KeyboardFocusManager &)result);
  return jresult;
}


SWIGEXPORT unsigned int SWIGSTDCALL CSharp_KeyboardFocusManager_SetCurrentFocusActor(void * jarg1, void * jarg2) {
  unsigned int jresult ;
  Dali::Toolkit::KeyboardFocusManager *arg1 = (Dali::Toolkit::KeyboardFocusManager *) 0 ;
  Dali::Actor arg2;
  Dali::Actor *argp2 ;
  bool result;

  arg1 = (Dali::Toolkit::KeyboardFocusManager *)jarg1;
  argp2 = (Dali::Actor *)jarg2;
  if (!argp2) {
    SWIG_CSharpSetPendingExceptionArgument(SWIG_CSharpArgumentNullException, "Attempt to dereference null Dali::Actor", 0);
    return 0;
  }
  arg2 = *argp2;
  {
    try {
      result = (bool)(arg1)->SetCurrentFocusActor(arg2);
    } catch (std::out_of_range& e) {
      {
        SWIG_CSharpException(SWIG_IndexError, const_cast<char*>(e.what())); return 0;
      };
    } catch (std::exception& e) {
      {
        SWIG_CSharpException(SWIG_RuntimeError, const_cast<char*>(e.what())); return 0;
      };
    } catch (...) {
      {
        SWIG_CSharpException(SWIG_UnknownError, "unknown error"); return 0;
      };
    }
  }
  jresult = result;
  return jresult;
}


SWIGEXPORT void * SWIGSTDCALL CSharp_KeyboardFocusManager_GetCurrentFocusActor(void * jarg1) {
  void * jresult ;
  Dali::Toolkit::KeyboardFocusManager *arg1 = (Dali::Toolkit::KeyboardFocusManager *) 0 ;
  Dali::Actor result;

  arg1 = (Dali::Toolkit::KeyboardFocusManager *)jarg1;
  {
    try {
      result = (arg1)->GetCurrentFocusActor();
    } catch (std::out_of_range& e) {
      {
        SWIG_CSharpException(SWIG_IndexError, const_cast<char*>(e.what())); return 0;
      };
    } catch (std::exception& e) {
      {
        SWIG_CSharpException(SWIG_RuntimeError, const_cast<char*>(e.what())); return 0;
      };
    } catch (...) {
      {
        SWIG_CSharpException(SWIG_UnknownError, "unknown error"); return 0;
      };
    }
  }
  jresult = new Dali::Actor((const Dali::Actor &)result);
  return jresult;
}


SWIGEXPORT unsigned int SWIGSTDCALL CSharp_KeyboardFocusManager_MoveFocus(void * jarg1, int jarg2) {
  unsigned int jresult ;
  Dali::Toolkit::KeyboardFocusManager *arg1 = (Dali::Toolkit::KeyboardFocusManager *) 0 ;
  Dali::Toolkit::Control::KeyboardFocus::Direction arg2;
  bool result;

  arg1 = (Dali::Toolkit::KeyboardFocusManager *)jarg1;
  arg2 = (Dali::Toolkit::Control::KeyboardFocus::Direction)jarg2;
  {
    try {
      result = (bool)(arg1)->MoveFocus(arg2);
    } catch (std::out_of_range& e) {
      {
        SWIG_CSharpException(SWIG_IndexError, const_cast<char*>(e.what())); return 0;
      };
    } catch (std::exception& e) {
      {
        SWIG_CSharpException(SWIG_RuntimeError, const_cast<char*>(e.what())); return 0;
      };
    } catch (...) {
      {
        SWIG_CSharpException(SWIG_UnknownError, "unknown error"); return 0;
      };
    }
  }
  jresult = result;
  return jresult;
}


SWIGEXPORT void SWIGSTDCALL CSharp_KeyboardFocusManager_ClearFocus(void * jarg1) {
  Dali::Toolkit::KeyboardFocusManager *arg1 = (Dali::Toolkit::KeyboardFocusManager *) 0 ;

  arg1 = (Dali::Toolkit::KeyboardFocusManager *)jarg1;
  {
    try {
      (arg1)->ClearFocus();
    } catch (std::out_of_range& e) {
      {
        SWIG_CSharpException(SWIG_IndexError, const_cast<char*>(e.what())); return;
      };
    } catch (std::exception& e) {
      {
        SWIG_CSharpException(SWIG_RuntimeError, const_cast<char*>(e.what())); return;
      };
    } catch (...) {
      {
        SWIG_CSharpException(SWIG_UnknownError, "unknown error"); return;
      };
    }
  }
}


SWIGEXPORT void SWIGSTDCALL CSharp_KeyboardFocusManager_SetFocusGroupLoop(void * jarg1, unsigned int jarg2) {
  Dali::Toolkit::KeyboardFocusManager *arg1 = (Dali::Toolkit::KeyboardFocusManager *) 0;
  bool arg2;

  arg1 = (Dali::Toolkit::KeyboardFocusManager *)jarg1;
  arg2 = jarg2 ? true : false;
  {
    try {
      (arg1)->SetFocusGroupLoop(arg2);
    } catch (std::out_of_range& e) {
      {
        SWIG_CSharpException(SWIG_IndexError, const_cast<char*>(e.what())); return;
      };
    } catch (std::exception& e) {
      {
        SWIG_CSharpException(SWIG_RuntimeError, const_cast<char*>(e.what())); return;
      };
    } catch (...) {
      {
        SWIG_CSharpException(SWIG_UnknownError, "unknown error"); return ;
      };
    }
  }
}


SWIGEXPORT unsigned int SWIGSTDCALL CSharp_KeyboardFocusManager_GetFocusGroupLoop(void * jarg1) {
  unsigned int jresult ;
  Dali::Toolkit::KeyboardFocusManager *arg1 = (Dali::Toolkit::KeyboardFocusManager *) 0 ;
  bool result;

  arg1 = (Dali::Toolkit::KeyboardFocusManager *)jarg1;
  {
    try {
      result = (bool)((Dali::Toolkit::KeyboardFocusManager const *)arg1)->GetFocusGroupLoop();
    } catch (std::out_of_range& e) {
      {
        SWIG_CSharpException(SWIG_IndexError, const_cast<char*>(e.what())); return 0;
      };
    } catch (std::exception& e) {
      {
        SWIG_CSharpException(SWIG_RuntimeError, const_cast<char*>(e.what())); return 0;
      };
    } catch (...) {
      {
        SWIG_CSharpException(SWIG_UnknownError, "unknown error"); return 0;
      };
    }
  }
  jresult = result;
  return jresult;
}


SWIGEXPORT void SWIGSTDCALL CSharp_KeyboardFocusManager_SetAsFocusGroup(void * jarg1, void * jarg2, unsigned int jarg3) {
  Dali::Toolkit::KeyboardFocusManager *arg1 = (Dali::Toolkit::KeyboardFocusManager *) 0 ;
  Dali::Actor arg2 ;
  bool arg3 ;
  Dali::Actor *argp2 ;

  arg1 = (Dali::Toolkit::KeyboardFocusManager *)jarg1;
  argp2 = (Dali::Actor *)jarg2;
  if (!argp2) {
    SWIG_CSharpSetPendingExceptionArgument(SWIG_CSharpArgumentNullException, "Attempt to dereference null Dali::Actor", 0);
    return ;
  }
  arg2 = *argp2;
  arg3 = jarg3 ? true : false;
  {
    try {
      (arg1)->SetAsFocusGroup(arg2,arg3);
    } catch (std::out_of_range& e) {
      {
        SWIG_CSharpException(SWIG_IndexError, const_cast<char*>(e.what())); return ;
      };
    } catch (std::exception& e) {
      {
        SWIG_CSharpException(SWIG_RuntimeError, const_cast<char*>(e.what())); return ;
      };
    } catch (...) {
      {
        SWIG_CSharpException(SWIG_UnknownError, "unknown error"); return ;
      };
    }
  }
}


SWIGEXPORT unsigned int SWIGSTDCALL CSharp_KeyboardFocusManager_IsFocusGroup(void * jarg1, void * jarg2) {
  unsigned int jresult ;
  Dali::Toolkit::KeyboardFocusManager *arg1 = (Dali::Toolkit::KeyboardFocusManager *) 0 ;
  Dali::Actor arg2 ;
  Dali::Actor *argp2 ;
  bool result;

  arg1 = (Dali::Toolkit::KeyboardFocusManager *)jarg1;
  argp2 = (Dali::Actor *)jarg2;
  if (!argp2) {
    SWIG_CSharpSetPendingExceptionArgument(SWIG_CSharpArgumentNullException, "Attempt to dereference null Dali::Actor", 0);
    return 0;
  }
  arg2 = *argp2;
  {
    try {
      result = (bool)((Dali::Toolkit::KeyboardFocusManager const *)arg1)->IsFocusGroup(arg2);
    } catch (std::out_of_range& e) {
      {
        SWIG_CSharpException(SWIG_IndexError, const_cast<char*>(e.what())); return 0;
      };
    } catch (std::exception& e) {
      {
        SWIG_CSharpException(SWIG_RuntimeError, const_cast<char*>(e.what())); return 0;
      };
    } catch (...) {
      {
        SWIG_CSharpException(SWIG_UnknownError, "unknown error"); return 0;
      };
    }
  }
  jresult = result;
  return jresult;
}


SWIGEXPORT void * SWIGSTDCALL CSharp_KeyboardFocusManager_GetFocusGroup(void * jarg1, void * jarg2) {
  void * jresult ;
  Dali::Toolkit::KeyboardFocusManager *arg1 = (Dali::Toolkit::KeyboardFocusManager *) 0 ;
  Dali::Actor arg2 ;
  Dali::Actor *argp2 ;
  Dali::Actor result;

  arg1 = (Dali::Toolkit::KeyboardFocusManager *)jarg1;
  argp2 = (Dali::Actor *)jarg2;
  if (!argp2) {
    SWIG_CSharpSetPendingExceptionArgument(SWIG_CSharpArgumentNullException, "Attempt to dereference null Dali::Actor", 0);
    return 0;
  }
  arg2 = *argp2;
  {
    try {
      result = (arg1)->GetFocusGroup(arg2);
    } catch (std::out_of_range& e) {
      {
        SWIG_CSharpException(SWIG_IndexError, const_cast<char*>(e.what())); return 0;
      };
    } catch (std::exception& e) {
      {
        SWIG_CSharpException(SWIG_RuntimeError, const_cast<char*>(e.what())); return 0;
      };
    } catch (...) {
      {
        SWIG_CSharpException(SWIG_UnknownError, "unknown error"); return 0;
      };
    }
  }
  jresult = new Dali::Actor((const Dali::Actor &)result);
  return jresult;
}


SWIGEXPORT void SWIGSTDCALL CSharp_KeyboardFocusManager_SetFocusIndicatorActor(void * jarg1, void * jarg2) {
  Dali::Toolkit::KeyboardFocusManager *arg1 = (Dali::Toolkit::KeyboardFocusManager *) 0 ;
  Dali::Actor arg2 ;
  Dali::Actor *argp2 ;

  arg1 = (Dali::Toolkit::KeyboardFocusManager *)jarg1;
  argp2 = (Dali::Actor *)jarg2;
  if (!argp2) {
    SWIG_CSharpSetPendingExceptionArgument(SWIG_CSharpArgumentNullException, "Attempt to dereference null Dali::Actor", 0);
    return ;
  }
  arg2 = *argp2;
  {
    try {
      (arg1)->SetFocusIndicatorActor(arg2);
    } catch (std::out_of_range& e) {
      {
        SWIG_CSharpException(SWIG_IndexError, const_cast<char*>(e.what())); return ;
      };
    } catch (std::exception& e) {
      {
        SWIG_CSharpException(SWIG_RuntimeError, const_cast<char*>(e.what())); return ;
      };
    } catch (...) {
      {
        SWIG_CSharpException(SWIG_UnknownError, "unknown error"); return ;
      };
    }
  }
}


SWIGEXPORT void * SWIGSTDCALL CSharp_KeyboardFocusManager_GetFocusIndicatorActor(void * jarg1) {
  void * jresult ;
  Dali::Toolkit::KeyboardFocusManager *arg1 = (Dali::Toolkit::KeyboardFocusManager *) 0 ;
  Dali::Actor result;

  arg1 = (Dali::Toolkit::KeyboardFocusManager *)jarg1;
  {
    try {
      result = (arg1)->GetFocusIndicatorActor();
    } catch (std::out_of_range& e) {
      {
        SWIG_CSharpException(SWIG_IndexError, const_cast<char*>(e.what())); return 0;
      };
    } catch (std::exception& e) {
      {
        SWIG_CSharpException(SWIG_RuntimeError, const_cast<char*>(e.what())); return 0;
      };
    } catch (...) {
      {
        SWIG_CSharpException(SWIG_UnknownError, "unknown error"); return 0;
      };
    }
  }
  jresult = new Dali::Actor((const Dali::Actor &)result);
  return jresult;
}


SWIGEXPORT void * SWIGSTDCALL CSharp_KeyboardFocusManager_PreFocusChangeSignal(void * jarg1) {
  void * jresult ;
  Dali::Toolkit::KeyboardFocusManager *arg1 = (Dali::Toolkit::KeyboardFocusManager *) 0 ;
  Dali::Toolkit::KeyboardFocusManager::PreFocusChangeSignalType *result = 0 ;

  arg1 = (Dali::Toolkit::KeyboardFocusManager *)jarg1;
  {
    try {
      result = (Dali::Toolkit::KeyboardFocusManager::PreFocusChangeSignalType *) &(arg1)->PreFocusChangeSignal();
    } catch (std::out_of_range& e) {
      {
        SWIG_CSharpException(SWIG_IndexError, const_cast<char*>(e.what())); return 0;
      };
    } catch (std::exception& e) {
      {
        SWIG_CSharpException(SWIG_RuntimeError, const_cast<char*>(e.what())); return 0;
      };
    } catch (...) {
      {
        SWIG_CSharpException(SWIG_UnknownError, "unknown error"); return 0;
      };
    }
  }
  jresult = (void *)result;
  return jresult;
}


SWIGEXPORT void * SWIGSTDCALL CSharp_KeyboardFocusManager_FocusChangedSignal(void * jarg1) {
  void * jresult ;
  Dali::Toolkit::KeyboardFocusManager *arg1 = (Dali::Toolkit::KeyboardFocusManager *) 0 ;
  Dali::Toolkit::KeyboardFocusManager::FocusChangedSignalType *result = 0 ;

  arg1 = (Dali::Toolkit::KeyboardFocusManager *)jarg1;
  {
    try {
      result = (Dali::Toolkit::KeyboardFocusManager::FocusChangedSignalType *) &(arg1)->FocusChangedSignal();
    } catch (std::out_of_range& e) {
      {
        SWIG_CSharpException(SWIG_IndexError, const_cast<char*>(e.what())); return 0;
      };
    } catch (std::exception& e) {
      {
        SWIG_CSharpException(SWIG_RuntimeError, const_cast<char*>(e.what())); return 0;
      };
    } catch (...) {
      {
        SWIG_CSharpException(SWIG_UnknownError, "unknown error"); return 0;
      };
    }
  }
  jresult = (void *)result;
  return jresult;
}


SWIGEXPORT void * SWIGSTDCALL CSharp_KeyboardFocusManager_FocusGroupChangedSignal(void * jarg1) {
  void * jresult ;
  Dali::Toolkit::KeyboardFocusManager *arg1 = (Dali::Toolkit::KeyboardFocusManager *) 0 ;
  Dali::Toolkit::KeyboardFocusManager::FocusGroupChangedSignalType *result = 0 ;

  arg1 = (Dali::Toolkit::KeyboardFocusManager *)jarg1;
  {
    try {
      result = (Dali::Toolkit::KeyboardFocusManager::FocusGroupChangedSignalType *) &(arg1)->FocusGroupChangedSignal();
    } catch (std::out_of_range& e) {
      {
        SWIG_CSharpException(SWIG_IndexError, const_cast<char*>(e.what())); return 0;
      };
    } catch (std::exception& e) {
      {
        SWIG_CSharpException(SWIG_RuntimeError, const_cast<char*>(e.what())); return 0;
      };
    } catch (...) {
      {
        SWIG_CSharpException(SWIG_UnknownError, "unknown error"); return 0;
      };
    }
  }
  jresult = (void *)result;
  return jresult;
}


SWIGEXPORT void * SWIGSTDCALL CSharp_KeyboardFocusManager_FocusedActorEnterKeySignal(void * jarg1) {
  void * jresult ;
  Dali::Toolkit::KeyboardFocusManager *arg1 = (Dali::Toolkit::KeyboardFocusManager *) 0 ;
  Dali::Toolkit::KeyboardFocusManager::FocusedActorEnterKeySignalType *result = 0 ;

  arg1 = (Dali::Toolkit::KeyboardFocusManager *)jarg1;
  {
    try {
      result = (Dali::Toolkit::KeyboardFocusManager::FocusedActorEnterKeySignalType *) &(arg1)->FocusedActorEnterKeySignal();
    } catch (std::out_of_range& e) {
      {
        SWIG_CSharpException(SWIG_IndexError, const_cast<char*>(e.what())); return 0;
      };
    } catch (std::exception& e) {
      {
        SWIG_CSharpException(SWIG_RuntimeError, const_cast<char*>(e.what())); return 0;
      };
    } catch (...) {
      {
        SWIG_CSharpException(SWIG_UnknownError, "unknown error"); return 0;
      };
    }
  }
  jresult = (void *)result;
  return jresult;
}



SWIGEXPORT unsigned int SWIGSTDCALL CSharp_KeyboardPreFocusChangeSignal_Empty(void * jarg1) {
  unsigned int jresult ;
  Dali::Signal< Dali::Actor (Dali::Actor,Dali::Actor,Dali::Toolkit::Control::KeyboardFocus::Direction) > *arg1 = (Dali::Signal< Dali::Actor (Dali::Actor,Dali::Actor,Dali::Toolkit::Control::KeyboardFocus::Direction) > *) 0 ;
  bool result;

  arg1 = (Dali::Signal< Dali::Actor (Dali::Actor,Dali::Actor,Dali::Toolkit::Control::KeyboardFocus::Direction) > *)jarg1;
  {
    try {
      result = (bool)Dali_Signal_Sl_Dali_Actor_Sp_Dali_Actor_Sc_Dali_Actor_Sc_Dali_Toolkit_Control_KeyboardFocus_Direction_SP__Sg__Empty((Dali::Signal< Dali::Actor (Dali::Actor,Dali::Actor,Dali::Toolkit::Control::KeyboardFocus::Direction) > const *)arg1);
    } catch (std::out_of_range& e) {
      {
        SWIG_CSharpException(SWIG_IndexError, const_cast<char*>(e.what())); return 0;
      };
    } catch (std::exception& e) {
      {
        SWIG_CSharpException(SWIG_RuntimeError, const_cast<char*>(e.what())); return 0;
      };
    } catch (...) {
      {
        SWIG_CSharpException(SWIG_UnknownError, "unknown error"); return 0;
      };
    }
  }
  jresult = result;
  return jresult;
}


SWIGEXPORT unsigned long SWIGSTDCALL CSharp_KeyboardPreFocusChangeSignal_GetConnectionCount(void * jarg1) {
  unsigned long jresult ;
  Dali::Signal< Dali::Actor (Dali::Actor,Dali::Actor,Dali::Toolkit::Control::KeyboardFocus::Direction) > *arg1 = (Dali::Signal< Dali::Actor (Dali::Actor,Dali::Actor,Dali::Toolkit::Control::KeyboardFocus::Direction) > *) 0 ;
  std::size_t result;

  arg1 = (Dali::Signal< Dali::Actor (Dali::Actor,Dali::Actor,Dali::Toolkit::Control::KeyboardFocus::Direction) > *)jarg1;
  {
    try {
      result = Dali_Signal_Sl_Dali_Actor_Sp_Dali_Actor_Sc_Dali_Actor_Sc_Dali_Toolkit_Control_KeyboardFocus_Direction_SP__Sg__GetConnectionCount((Dali::Signal< Dali::Actor (Dali::Actor,Dali::Actor,Dali::Toolkit::Control::KeyboardFocus::Direction) > const *)arg1);
    } catch (std::out_of_range& e) {
      {
        SWIG_CSharpException(SWIG_IndexError, const_cast<char*>(e.what())); return 0;
      };
    } catch (std::exception& e) {
      {
        SWIG_CSharpException(SWIG_RuntimeError, const_cast<char*>(e.what())); return 0;
      };
    } catch (...) {
      {
        SWIG_CSharpException(SWIG_UnknownError, "unknown error"); return 0;
      };
    }
  }
  jresult = (unsigned long)result;
  return jresult;
}

typedef void* (SWIGSTDCALL* SWIG_CallbackKbPreFocusChange)(void *, void *, int);
SWIG_CallbackKbPreFocusChange swig_callbackOnKbPreFocusChange;

Dali::Actor OnKbPreFocusChangeCallback(Dali::Actor current, Dali::Actor proposed, Dali::Toolkit::Control::KeyboardFocus::Direction direction)
{
    Dali::Actor *actorp;
    Dali::Actor actor;

    actorp = (Dali::Actor *)swig_callbackOnKbPreFocusChange((void *)&current, (void *)&proposed, direction);
    actor = *actorp;

    return actor;
}

SWIGEXPORT void SWIGSTDCALL CSharp_KeyboardPreFocusChangeSignal_Connect(void * jarg1, SWIG_CallbackKbPreFocusChange callbackOnKbPreFocusChange) {

  swig_callbackOnKbPreFocusChange = callbackOnKbPreFocusChange;

  Dali::Signal< Dali::Actor (Dali::Actor,Dali::Actor,Dali::Toolkit::Control::KeyboardFocus::Direction) > *arg1 = (Dali::Signal< Dali::Actor (Dali::Actor,Dali::Actor,Dali::Toolkit::Control::KeyboardFocus::Direction) > *) 0 ;
  Dali::Actor (*arg2)(Dali::Actor,Dali::Actor,Dali::Toolkit::Control::KeyboardFocus::Direction) = (Dali::Actor (*)(Dali::Actor,Dali::Actor,Dali::Toolkit::Control::KeyboardFocus::Direction)) 0 ;

  arg1 = (Dali::Signal< Dali::Actor (Dali::Actor,Dali::Actor,Dali::Toolkit::Control::KeyboardFocus::Direction) > *)jarg1;
  arg2 = (Dali::Actor (*)(Dali::Actor,Dali::Actor,Dali::Toolkit::Control::KeyboardFocus::Direction))OnKbPreFocusChangeCallback;

  {
    try {
      Dali_Signal_Sl_Dali_Actor_Sp_Dali_Actor_Sc_Dali_Actor_Sc_Dali_Toolkit_Control_KeyboardFocus_Direction_SP__Sg__Connect(arg1,arg2);
    } catch (std::out_of_range& e) {
      {
        SWIG_CSharpException(SWIG_IndexError, const_cast<char*>(e.what())); return ;
      };
    } catch (std::exception& e) {
      {
        SWIG_CSharpException(SWIG_RuntimeError, const_cast<char*>(e.what())); return ;
      };
    } catch (...) {
      {
        SWIG_CSharpException(SWIG_UnknownError, "unknown error"); return ;
      };
    }
  }
}


SWIGEXPORT void SWIGSTDCALL CSharp_KeyboardPreFocusChangeSignal_Disconnect(void * jarg1, void * jarg2) {
  Dali::Signal< Dali::Actor (Dali::Actor,Dali::Actor,Dali::Toolkit::Control::KeyboardFocus::Direction) > *arg1 = (Dali::Signal< Dali::Actor (Dali::Actor,Dali::Actor,Dali::Toolkit::Control::KeyboardFocus::Direction) > *) 0 ;
  Dali::Actor (*arg2)(Dali::Actor,Dali::Actor,Dali::Toolkit::Control::KeyboardFocus::Direction) = (Dali::Actor (*)(Dali::Actor,Dali::Actor,Dali::Toolkit::Control::KeyboardFocus::Direction)) 0 ;

  arg1 = (Dali::Signal< Dali::Actor (Dali::Actor,Dali::Actor,Dali::Toolkit::Control::KeyboardFocus::Direction) > *)jarg1;
  arg2 = (Dali::Actor (*)(Dali::Actor,Dali::Actor,Dali::Toolkit::Control::KeyboardFocus::Direction))jarg2;
  {
    try {
      Dali_Signal_Sl_Dali_Actor_Sp_Dali_Actor_Sc_Dali_Actor_Sc_Dali_Toolkit_Control_KeyboardFocus_Direction_SP__Sg__Disconnect(arg1,arg2);
    } catch (std::out_of_range& e) {
      {
        SWIG_CSharpException(SWIG_IndexError, const_cast<char*>(e.what())); return ;
      };
    } catch (std::exception& e) {
      {
        SWIG_CSharpException(SWIG_RuntimeError, const_cast<char*>(e.what())); return ;
      };
    } catch (...) {
      {
        SWIG_CSharpException(SWIG_UnknownError, "unknown error"); return ;
      };
    }
  }
}


SWIGEXPORT void * SWIGSTDCALL CSharp_KeyboardPreFocusChangeSignal_Emit(void * jarg1, void * jarg2, void * jarg3, int jarg4) {
  void * jresult ;
  Dali::Signal< Dali::Actor (Dali::Actor,Dali::Actor,Dali::Toolkit::Control::KeyboardFocus::Direction) > *arg1 = (Dali::Signal< Dali::Actor (Dali::Actor,Dali::Actor,Dali::Toolkit::Control::KeyboardFocus::Direction) > *) 0 ;
  Dali::Actor arg2 ;
  Dali::Actor arg3 ;
  Dali::Toolkit::Control::KeyboardFocus::Direction arg4 ;
  Dali::Actor *argp2 ;
  Dali::Actor *argp3 ;
  Dali::Actor result;

  arg1 = (Dali::Signal< Dali::Actor (Dali::Actor,Dali::Actor,Dali::Toolkit::Control::KeyboardFocus::Direction) > *)jarg1;
  argp2 = (Dali::Actor *)jarg2;
  if (!argp2) {
    SWIG_CSharpSetPendingExceptionArgument(SWIG_CSharpArgumentNullException, "Attempt to dereference null Dali::Actor", 0);
    return 0;
  }
  arg2 = *argp2;
  argp3 = (Dali::Actor *)jarg3;
  if (!argp3) {
    SWIG_CSharpSetPendingExceptionArgument(SWIG_CSharpArgumentNullException, "Attempt to dereference null Dali::Actor", 0);
    return 0;
  }
  arg3 = *argp3;
  arg4 = (Dali::Toolkit::Control::KeyboardFocus::Direction)jarg4;
  {
    try {
      result = Dali_Signal_Sl_Dali_Actor_Sp_Dali_Actor_Sc_Dali_Actor_Sc_Dali_Toolkit_Control_KeyboardFocus_Direction_SP__Sg__Emit(arg1,arg2,arg3,arg4);
    } catch (std::out_of_range& e) {
      {
        SWIG_CSharpException(SWIG_IndexError, const_cast<char*>(e.what())); return 0;
      };
    } catch (std::exception& e) {
      {
        SWIG_CSharpException(SWIG_RuntimeError, const_cast<char*>(e.what())); return 0;
      };
    } catch (...) {
      {
        SWIG_CSharpException(SWIG_UnknownError, "unknown error"); return 0;
      };
    }
  }
  jresult = new Dali::Actor((const Dali::Actor &)result);
  return jresult;
}


SWIGEXPORT void * SWIGSTDCALL CSharp_new_KeyboardPreFocusChangeSignal() {
  void * jresult ;
  Dali::Signal< Dali::Actor (Dali::Actor,Dali::Actor,Dali::Toolkit::Control::KeyboardFocus::Direction) > *result = 0 ;

  {
    try {
      result = (Dali::Signal< Dali::Actor (Dali::Actor,Dali::Actor,Dali::Toolkit::Control::KeyboardFocus::Direction) > *)new Dali::Signal< Dali::Actor (Dali::Actor,Dali::Actor,Dali::Toolkit::Control::KeyboardFocus::Direction) >();
    } catch (std::out_of_range& e) {
      {
        SWIG_CSharpException(SWIG_IndexError, const_cast<char*>(e.what())); return 0;
      };
    } catch (std::exception& e) {
      {
        SWIG_CSharpException(SWIG_RuntimeError, const_cast<char*>(e.what())); return 0;
      };
    } catch (...) {
      {
        SWIG_CSharpException(SWIG_UnknownError, "unknown error"); return 0;
      };
    }
  }
  jresult = (void *)result;
  return jresult;
}


SWIGEXPORT void SWIGSTDCALL CSharp_delete_KeyboardPreFocusChangeSignal(void * jarg1) {
  Dali::Signal< Dali::Actor (Dali::Actor,Dali::Actor,Dali::Toolkit::Control::KeyboardFocus::Direction) > *arg1 = (Dali::Signal< Dali::Actor (Dali::Actor,Dali::Actor,Dali::Toolkit::Control::KeyboardFocus::Direction) > *) 0 ;

  arg1 = (Dali::Signal< Dali::Actor (Dali::Actor,Dali::Actor,Dali::Toolkit::Control::KeyboardFocus::Direction) > *)jarg1;
  {
    try {
      delete arg1;
    } catch (std::out_of_range& e) {
      {
        SWIG_CSharpException(SWIG_IndexError, const_cast<char*>(e.what())); return ;
      };
    } catch (std::exception& e) {
      {
        SWIG_CSharpException(SWIG_RuntimeError, const_cast<char*>(e.what())); return ;
      };
    } catch (...) {
      {
        SWIG_CSharpException(SWIG_UnknownError, "unknown error"); return ;
      };
    }
  }
}

SWIGEXPORT Dali::BaseHandle * SWIGSTDCALL CSharp_KeyboardFocusManager_SWIGUpcast(Dali::Toolkit::KeyboardFocusManager *jarg1) {
    return (Dali::BaseHandle *)jarg1;
}

#ifdef __cplusplus
}
#endif

