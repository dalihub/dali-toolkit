/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include "toolkit-timer.h"

#include <Ecore.h>

// INTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/object/base-object.h>
#include <dali/public-api/signals/dali-signal-v2.h>


namespace
{
bool ecore_timer_running = false;
Ecore_Task_Cb timer_callback_func=NULL;
const void* timer_callback_data=NULL;
int timerId = 0;
}// anon namespace

extern "C"
{
Ecore_Timer* ecore_timer_add(double in,
                             Ecore_Task_Cb func,
                             const void   *data)
{
  ecore_timer_running = true;
  timer_callback_func = func;
  timer_callback_data = data;
  timerId += sizeof(Ecore_Timer*);
  return (Ecore_Timer*)timerId;
}

void* ecore_timer_del(Ecore_Timer *timer)
{
  ecore_timer_running = false;
  timer_callback_func = NULL;
  return NULL;
}

}
