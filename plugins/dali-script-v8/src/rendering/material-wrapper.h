#ifndef __DALI_V8PLUGIN_MATERIAL_WRAPPER_H__
#define __DALI_V8PLUGIN_MATERIAL_WRAPPER_H__

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
#include <dali/devel-api/rendering/material.h>

// INTERNAL INCLUDES
#include <object/handle-wrapper.h>

namespace Dali
{

namespace V8Plugin
{


/**
 * A Material wrapper.
 * Provides access to Material specific functionality and V8 memory handling.
 */
class MaterialWrapper : public HandleWrapper
{

public:

  /**
   * @brief Constructor
   * @param[in] material DALi material
   * @param[in] gc garbage collection interface
   */
  MaterialWrapper( const Material& material,
                GarbageCollectorInterface& gc );

  /**
   * @brief Destructor
   */
  virtual ~MaterialWrapper()
  {
  };

  /**
   * @brief Creates a new Material wrapped inside a Javascript Object.
   * @param[in] args v8 function call arguments interpreted
   */
  static void NewMaterial( const v8::FunctionCallbackInfo< v8::Value >& args);

  /**
   * Wraps a sampler
   */
  static v8::Handle<v8::Object> WrapMaterial(v8::Isolate* isolate, const Dali::Material& );


  // The MaterialAttribute ObjectTemplate, we cache templates so we don't have
  // keep generating them everytime we want to create a Material
  static v8::Persistent<v8::ObjectTemplate> mMaterialTemplate;

  /**
   * @brief Gets the handle of material
   * @return the handle of material
   */
  Material GetMaterial();


private:

  // Material
  Material mMaterial;

  /**
   * @brief Creates the object template that used to create material at runtime
   * @param[in] isolate v8 isolated instance
   * @return the object template
   */
  static v8::Handle<v8::ObjectTemplate> MakeMaterialTemplate( v8::Isolate* isolate );

  /**
   * @brief Gets the object template that used to create material at runtime
   * @param[in] isolate v8 isolated instance
   * @return the object template
   */
  static v8::Local<v8::ObjectTemplate> GetMaterialTemplate( v8::Isolate* isolate );

};

} // namespace V8Plugin

} // namespace Dali

#endif // __DALI_V8PLUGIN_MATERIAL_WRAPPER_H__
