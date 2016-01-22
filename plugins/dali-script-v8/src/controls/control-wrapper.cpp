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

// CLASS HEADER
#include <actors/actor-wrapper.h>
#include "control-wrapper.h"

// EXTERNAL INCLUDES
#include <dali/public-api/object/type-registry.h>

// INTERNAL INCLUDES
#include <controls/item-view-api.h>
#include <v8-utils.h>
#include <dali-wrapper.h>

namespace Dali
{

namespace V8Plugin
{

v8::Persistent<v8::ObjectTemplate> ControlWrapper::mControlTemplate;
v8::Persistent<v8::ObjectTemplate> ControlWrapper::mItemViewTemplate;

Vector< void* > ControlWrapper::mControlGarbageContainer;

namespace
{


/**
 * pointer to a persistent template handle
 */
struct ControlTemplate
{
  v8::Persistent<v8::ObjectTemplate>* controlTemplate;
};

/**
 * array of templates for each type of control
 */
const ControlTemplate ControlTemplateLookup[]=
{
    { &ControlWrapper::mControlTemplate },        // CONTROL
    { &ControlWrapper::mItemViewTemplate }        // ITEMVIEW
};

/**
 * Bitmask of API's that an control can support
 */
enum ControlApiBitMask
{
  CONTROL_API             = 1 << 0,
  ITEMVIEW_API            = 1 << 1
};

/**
 * structure used for the ControlApiLookup.
 */
struct ControlApiStruct
{
  const char* controlName;
  ControlWrapper::ControlType controlType;
  Toolkit::Control (*constructor)( const v8::FunctionCallbackInfo< v8::Value >& args);
  int supportApis;
};

/**
 * Lookup table to match a control type with a constructor and supported API's.
 * ControlWrapper::ControlType is used to index this table
 */
const ControlApiStruct ControlApiLookup[]=
{
  {"Control",      ControlWrapper::CONTROL,      NULL,                 CONTROL_API },
  {"ItemView",     ControlWrapper::ITEMVIEW,     ItemViewApi::New,     CONTROL_API | ITEMVIEW_API },
};

const unsigned int ControlApiLookupCount = sizeof(ControlApiLookup)/sizeof(ControlApiLookup[0]);


/**
 * Creates a control given a type name
 * Uses the type registry to create an control of the correct type
 */
Toolkit::Control CreateControl( const v8::FunctionCallbackInfo< v8::Value >& args,
                        const std::string& typeName )
{
  Toolkit::Control control;

  ControlWrapper::ControlType controlType = ControlWrapper::GetControlType( typeName );

  // if we don't currently have specific binding for the given control type,
  // try to use type registry to create it
  if( controlType == ControlWrapper::UNKNOWN_CONTROL )
  {
    Dali::TypeInfo typeInfo = Dali::TypeRegistry::Get().GetTypeInfo( typeName );
    if( typeInfo )
    {
      Dali::BaseHandle handle = typeInfo.CreateInstance();
      if( handle )
      {
        control = Toolkit::Control::DownCast( handle );
        if( !control )
        {
          DALI_SCRIPT_EXCEPTION( args.GetIsolate(), "Unknown control type" );
          return Toolkit::Control();
        }
      }
    }
  }
  else
  {
    // run the constructor for this type of control so it can pull out custom parameters
    control = (ControlApiLookup[controlType].constructor)( args );
  }

  return control;
}

/**
 * given a control type return what api's it supports
 */
int GetControlSupportedApis( ControlWrapper::ControlType type )
{
  return ControlApiLookup[type].supportApis;
}

/**
 * Used for the ControlFunctionTable to map function names to functions
 * with for a specific API
 */
struct ControlFunctions
{
  const char* name;               ///< function name
  void (*function)( const v8::FunctionCallbackInfo< v8::Value >& args);
  ControlApiBitMask api;
};

/**
 * Contains a list of all functions that can be called in
 * ItemView
 */
const ControlFunctions ControlFunctionTable[]=
{

    /**************************************
     * ItemView  API
     **************************************/
    { "GetLayoutCount",                  ItemViewApi::GetLayoutCount,                   ITEMVIEW_API  },
    { "AddLayout",                       ItemViewApi::AddLayout,                        ITEMVIEW_API  },
    { "RemoveLayout",                    ItemViewApi::RemoveLayout,                     ITEMVIEW_API  },
    { "ActivateLayout",                  ItemViewApi::ActivateLayout,                   ITEMVIEW_API  },
    { "GetItemSize",                     ItemViewApi::GetItemSize,                      ITEMVIEW_API  },
    { "SetItemSize",                     ItemViewApi::SetItemSize,                      ITEMVIEW_API  },
    { "ScrollToItem",                    ItemViewApi::ScrollToItem,                     ITEMVIEW_API  },
    { "GetItem",                         ItemViewApi::GetItem,                          ITEMVIEW_API  },
    { "GetItemId",                       ItemViewApi::GetItemId,                        ITEMVIEW_API  },
    { "GetItemsRange",                   ItemViewApi::GetItemsRange,                    ITEMVIEW_API  },

};

const unsigned int ControlFunctionTableCount = sizeof(ControlFunctionTable)/sizeof(ControlFunctionTable[0]);
} //un-named space


ControlWrapper::ControlWrapper( Toolkit::Control control,
              GarbageCollectorInterface& gc )
: ActorWrapper( control, gc ),
  mControl( control )

{
}

ControlWrapper::~ControlWrapper()
{
  mControlGarbageContainer.Release();
}

v8::Handle<v8::Object> ControlWrapper::WrapControl(v8::Isolate* isolate, Toolkit::Control control )
{
  v8::EscapableHandleScope handleScope( isolate );

  // Check whether the control is a Control
  ControlWrapper::ControlType controlType = GetControlType( control.GetTypeName() );

  if( controlType == ControlWrapper::UNKNOWN_CONTROL && Toolkit::Control::DownCast(control) )
  {
    controlType = ControlWrapper::CONTROL;
  }

  v8::Local<v8::Object> object = WrapControl( isolate, control, controlType );

  return handleScope.Escape( object );
}

Toolkit::Control ControlWrapper::GetControl()
{
  return mControl;
}

v8::Handle<v8::Object> ControlWrapper::WrapControl( v8::Isolate* isolate, Toolkit::Control control, ControlType controlType )
{
  v8::EscapableHandleScope handleScope( isolate );
  v8::Local<v8::ObjectTemplate> objectTemplate;

  objectTemplate = GetControlTemplate( isolate, controlType );

  // create an instance of the template
  v8::Local<v8::Object> localObject = objectTemplate->NewInstance();

  // create the control object
  ControlWrapper* pointer = new ControlWrapper( control, Dali::V8Plugin::DaliWrapper::Get().GetDaliGarbageCollector() );

  // assign the JavaScript object to the wrapper.
  // This also stores Dali object, in an internal field inside the JavaScript object.
  pointer->SetJavascriptObject( isolate, localObject );

  return handleScope.Escape( localObject );
}

v8::Local<v8::ObjectTemplate> ControlWrapper::GetControlTemplate( v8::Isolate* isolate, ControlWrapper::ControlType type )
{
  v8::EscapableHandleScope handleScope( isolate );
  v8::Local<v8::ObjectTemplate> objectTemplate;

  if( ControlTemplateLookup[type].controlTemplate->IsEmpty() )
  {
    objectTemplate = MakeDaliControlTemplate( isolate, type );
    ControlTemplateLookup[type].controlTemplate->Reset( isolate, objectTemplate );
  }
  else
  {
    // get the object template
    objectTemplate = v8::Local<v8::ObjectTemplate>::New( isolate, *ControlTemplateLookup[type].controlTemplate );
  }

  return handleScope.Escape( objectTemplate );
}

v8::Handle<v8::ObjectTemplate> ControlWrapper::MakeDaliControlTemplate( v8::Isolate* isolate, ControlType controlType )
{
  v8::EscapableHandleScope handleScope( isolate );

  // all the controls support actor APIs
  v8::Local<v8::ObjectTemplate> objTemplate = ActorWrapper::MakeDaliActorTemplate( isolate, ActorWrapper::ACTOR );

  // find out what API's this control supports
  int supportApis = GetControlSupportedApis( controlType );

  // add our function properties
  for( unsigned int i = 0; i < ControlFunctionTableCount; ++i )
  {
    const ControlFunctions property =  ControlFunctionTable[i];

    // check to see if the control supports a certain type of API
    // e.g. ItemView will support CONTROL_API and ITEMVIEW_API
    if( supportApis &  property.api )
    {
      std::string funcName = V8Utils::GetJavaScriptFunctionName( property.name);

      objTemplate->Set( v8::String::NewFromUtf8( isolate, funcName.c_str() ),
                      v8::FunctionTemplate::New( isolate, property.function ) );
    }
  }

  return handleScope.Escape( objTemplate );
}

void ControlWrapper::NewControl( const v8::FunctionCallbackInfo< v8::Value >& args)
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );

  if( !args.IsConstructCall() )
  {
    DALI_SCRIPT_EXCEPTION( isolate, "constructor called without 'new" );
    return;
  }

  bool found( false );
  std::string controlName = V8Utils::GetStringParameter( PARAMETER_0, found, isolate,  args );

  Toolkit::Control control;
  if( found && controlName != ControlApiLookup[0].controlName )
  {
    control = CreateControl( args, controlName ); // create the control with the given type
  }
  else
  {
    control = Toolkit::Control::New(); // no given type, so create the base type of control
  }

  if( control )
  {
    v8::Local<v8::Object> localObject = WrapControl( isolate, control );
    args.GetReturnValue().Set( localObject );
  }
  else
  {
    DALI_SCRIPT_EXCEPTION( isolate, "unsupported control type" );
  }
}

/**
 * Given a control type name, e.g. ItemView returns the type, e.g. ControlWrapper::ITEMVIEW
 */
ControlWrapper::ControlType ControlWrapper::GetControlType( const std::string& name )
{
  for( unsigned int i = 0 ; i < ControlApiLookupCount ; i++ )
  {
    if( ControlApiLookup[i].controlName == name )
    {
      return ControlApiLookup[i].controlType;
    }
  }
  return ControlWrapper::UNKNOWN_CONTROL;
}

void ControlWrapper::RegisterGarbage(void* garbage)
{
  mControlGarbageContainer.PushBack(garbage);
}

} // namespace V8Plugin

} // namespace Dali
