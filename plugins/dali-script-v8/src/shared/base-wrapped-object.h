#ifndef __DALI_V8PLUGIN_BASE_WRAPPED_OBJECT_H__
#define __DALI_V8PLUGIN_BASE_WRAPPED_OBJECT_H__

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

// INTERNAL INCLUDES
#include <interfaces/garbage-collector-interface.h>


namespace Dali
{

namespace V8Plugin
{

class SignalManager;

/**
 * @brief Used as a base class for all wrapped objects.
 *
 *  Dali JavaScript objects look like this
 *
 * \code
 *  _______________________________________         ________________________________________
 * | JavaScript Object                     |       |  C++ WrappedObject  (e.g. ImageWrapper)|
 * |---------------------------------------|       |----------------------------------------|
 * | Hidden internal fields                |       | Handle to a Dali::Image object         |
 * | *Pointer to a BaseWrappedObject       | ----> |________________________________________|
 * | Type of wrapped object (e.g. Image)   |
 * -----------------------------------------
 *
 * \endcode
 *
 * Whenever we want to access functions / properties of that wrapped object, we unwrap it
 * to get access to the Dali object.
 *
 * Each wrapped object registers with Dali garbage collector so they can be deleted
 * when Dali shuts down
 */
class BaseWrappedObject
{

public:

  /*
   * A type enum for the wrapped object
   */
  enum Type
    {
      UNKNOWN              = -1,
      HANDLE               = 0 ,
      HANDLE_SIGNAL,
      CONNECTION,
      ANIMATION,
      PATH,
      PATH_CONSTRAINER,
      LINEAR_CONSTRAINER,
      BUILDER,
      STAGE,
      FONT,
      IMAGE,
      IMAGE_ATTRIBUTES,
      ACTOR,
      ACTOR_PROPERTY,
      ITEMVIEW,
      ITEMFACTORY,
      SCROLLVIEW,
      RENDER_TASK,
      RENDER_TASK_LIST,
      TIMER,
      SHADER,
      SAMPLER,
      MATERIAL,
      GEOMETRY,
      RENDERER,
      PROPERTY_BUFFER,
      TAP_GESTURE,
      SHADER_EFFECT,
      PAN_GESTURE,
      LONGPRESS_GESTURE,
      PINCH_GESTURE,
      KEYBOARD_FOCUS_MANAGER,
      PAN_GESTURE_DETECTOR,
      ////////////////////////////////////////////////////////
      PROPERTY_VALUE_START_RANGE,  // start of property values
      VECTOR2,
      VECTOR3,
      VECTOR4,
      MATRIX,
      MATRIX3,
      RECTANGLE,
      ROTATION,
      PROPERTY_VALUE_END_RANGE,  // end of property values
      ///////////////////////////////////////////////////////
    };

  /*
   * A type enum for the wrapped object internal field
   */
  enum Field
    {
      FIELD_POINTER   = 0,
      FIELD_TYPE      = 1,
      FIELD_COUNT     = 2 // increase if more fields are added
    };

   /**
   * @brief virtual destructor
   */
  virtual ~BaseWrappedObject();

  /**
   * @return true if nothing else is referencing this object
   */
  bool IsReferenced();

  /**
   * @brief Called when the v8 garbage collector decides the JavaScript object (which contains the Dali wrapped object)
   * is no longer used / reachable.
   */
  static void WeakCallback( const v8::WeakCallbackData<v8::Object,BaseWrappedObject >& data);

  /**
   * @brief create and set the internal fields of a JavaScript object.
   * Currently has 2 internal fields, 1 is a pointer to dali wrapped object, 2nd is the type.
   */
  void SetJavascriptObject( v8::Isolate* isolate, v8::Local<v8::Object>& object );

  /**
   * @return true if the object is of a certain wrapped type (e.g. Animation )
   */
  static bool IsWrappedType( v8::Isolate* isolate, const v8::Local<v8::Object>& object,  BaseWrappedObject::Type type);

  /**
   * @return true if the object is a wrapped property value
   */
  static bool IsWrappedTypeAPropertyValue(const v8::Local<v8::Object>& object);

  /**
   * @brief Extracts the Dali wrapped object from the javascript object
   */
  static BaseWrappedObject* UnWrap( v8::Isolate* isolate, const v8::Local<v8::Object>& object);

  /**
   * @return the wrapped type
   */
  Type GetType();

  virtual SignalManager* GetSignalManager();

protected:

  /**
   * Constructor
   * @param type wrapped type
   * @param gc garbage collector interface
   */
  BaseWrappedObject( Type type, GarbageCollectorInterface& gc );

private:

  BaseWrappedObject();

  /**
   * We store a  persistent handle to the JavaScript object and then set it to weak.
   * This means we be notified when v8 decides it's no longer required and we can delete
   * the associated Dali object
   */
  v8::Persistent<v8::Object > mWeakPersistentHandle;
  Type mWrappedType;                                    ///< wrapped type
  GarbageCollectorInterface& mGarbageCollector;         ///< Dali garbage collector
};

} // V8Plugin

} // Dali

#endif // header __DALI_V8PLUGIN_DALI_OBJECT_LIFETIME_MANAGER_H__
