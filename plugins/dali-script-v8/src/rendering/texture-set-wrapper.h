#ifndef __DALI_V8PLUGIN_TEXTURE_SET_WRAPPER_H__
#define __DALI_V8PLUGIN_TEXTURE_SET_WRAPPER_H__

/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
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
#include <dali/devel-api/rendering/texture-set.h>

// INTERNAL INCLUDES
#include <object/handle-wrapper.h>

namespace Dali
{

namespace V8Plugin
{


/**
 * A TextureSet wrapper.
 * Provides access to TextureSet specific functionality and V8 memory handling.
 */
class TextureSetWrapper : public HandleWrapper
{

public:

  /**
   * @brief Constructor
   * @param[in] textureSet DALi TextureSet
   * @param[in] gc garbage collection interface
   */
  TextureSetWrapper( const TextureSet& textureSet,
                GarbageCollectorInterface& gc );

  /**
   * @brief Destructor
   */
  virtual ~TextureSetWrapper()
  {
  };

  /**
   * @brief Creates a new TextureSet wrapped inside a Javascript Object.
   * @param[in] args v8 function call arguments interpreted
   */
  static void NewTextureSet( const v8::FunctionCallbackInfo< v8::Value >& args);

  /**
   * Wraps a TextureSet
   */
  static v8::Handle<v8::Object> WrapTextureSet(v8::Isolate* isolate, const Dali::TextureSet& );


  // The TextureSetAttribute ObjectTemplate, we cache templates so we don't have
  // keep generating them everytime we want to create a TextureSet
  static v8::Persistent<v8::ObjectTemplate> mTextureSetTemplate;

  /**
   * @brief Gets the handle of TextureSet
   * @return the handle of the TextureSet
   */
  TextureSet GetTextureSet();


private:

  // TextureSet
  TextureSet mTextureSet;

  /**
   * @brief Creates the object template that used to create TextureSets at runtime
   * @param[in] isolate v8 isolated instance
   * @return the object template
   */
  static v8::Handle<v8::ObjectTemplate> MakeTextureSetTemplate( v8::Isolate* isolate );

  /**
   * @brief Gets the object template that used to create TextureSets at runtime
   * @param[in] isolate v8 isolated instance
   * @return the object template
   */
  static v8::Local<v8::ObjectTemplate> GetTextureSetTemplate( v8::Isolate* isolate );

};

} // namespace V8Plugin

} // namespace Dali

#endif // __DALI_V8PLUGIN_TEXTURE_SET_WRAPPER_H__
