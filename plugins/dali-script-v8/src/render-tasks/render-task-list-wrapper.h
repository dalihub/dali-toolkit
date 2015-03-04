#ifndef __DALI_V8PLUGIN_RENDER_TASK_LIST_WRAPPER_H__
#define __DALI_V8PLUGIN_RENDER_TASK_LIST_WRAPPER_H__

/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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

// EXTERNAL INCLUDES
#include <v8.h>
#include <dali/public-api/render-tasks/render-task-list.h>

// INTERNAL INCLUDES
#include <shared/base-wrapped-object.h>


namespace Dali
{

namespace V8Plugin
{

/**
 * RenderTaskList  wrapper.
 * Provides access to RenderTaskList specific functionality
 */
class RenderTaskListWrapper : public BaseWrappedObject
{

public:

  RenderTaskListWrapper( const RenderTaskList& renderTaskList,
                GarbageCollectorInterface& gc );

  virtual ~RenderTaskListWrapper();

  /**
   * Wraps an renderTaskList
   */
  static v8::Handle<v8::Object> WrapRenderTaskList(v8::Isolate* isolate, const Dali::RenderTaskList& );


  // The RenderTaskListAttribute ObjectTemplate, we cache templates so we don't have
  // keep generating them everytime we want to create an RenderTaskList
  static v8::Persistent<v8::ObjectTemplate> mRenderTaskListTemplate;

  RenderTaskList GetRenderTaskList();


private:

  RenderTaskList mRenderTaskList;  ///< renderTaskList object

  static v8::Handle<v8::ObjectTemplate> MakeRenderTaskListTemplate( v8::Isolate* isolate );
  static v8::Local<v8::ObjectTemplate> GetRenderTaskListTemplate( v8::Isolate* isolate );

};

} // namespace V8Plugin

} // namespace Dali

#endif // __DALI_V8PLUGIN_RENDER_TASK_LIST_WRAPPER_H__
