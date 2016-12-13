/*! \page properties Properties
 *
@section what-is-a-property What is a property?

A property is a value used by an object that can be modified or read externally to that object.
This could be from within DALi or externally by an application.

<h2 class="pg">What is a property used for?</h2>

Properties can be set externally by an application, allowing that application to change the configuration or behaviour of an actor.
This could include the physical geometry of the actor, or how it is drawn or moves.

Properties can also be read. This feature can be used in conjunction with constraints to allow changes to a property within one actor to cause changes to the property of another actor. For example, an actor following the movement of another separate actor (that it is not a child of).

Properties can be used to expose any useful information or behaviour of an actor.
Other actor variables that are used to implement this bevahiour, or do not make useful sense from an application developers point of view should not be exposed.

<h2 class="pg">How to implement a property within DALi Core:</h2>

<b>There are two stages:</b>

- Define the properties as an enum in the public-api header file.
- Define the property details using the pre-defined macros to build up a table of property information.

There are some pre-defined macros designed to help with and standardise the definition of the propery details table per class.

These macros generate an array of property details which allow efficient lookup of flags like "animatable" or "constraint input".

<b>Example: Layer</b>

Within the public-api header file; layer.h:

@code
  /**
   * @brief An enumeration of properties belonging to the Layer class.
   *
   * Properties additional to Actor.
   */
  struct Property
  {
    enum
    {
      CLIPPING_ENABLE = DEFAULT_DERIVED_ACTOR_PROPERTY_START_INDEX, ///< name "clippingEnable",   type bool @SINCE_1_0.0
      CLIPPING_BOX,                                                 ///< name "clippingBox",      type Rect<int> @SINCE_1_0.0
      BEHAVIOR,                                                     ///< name "behavior",         type String @SINCE_1_0.0
    };
  };
@endcode
From @ref Dali::Layer::Property

<b>Notes:</b>

- The properties are enumerated within a named struct to give them a namespace.
- The properties are then refered to as &lt;OBJECT&gt;::%Property::&lt;PROPERTY_NAME&gt;.

Within the internal implementation; <b>layer-impl.cpp</b>:

@code
namespace // Unnamed namespace
{

// Properties

//              Name                Type      writable animatable constraint-input  enum for index-checking
DALI_PROPERTY_TABLE_BEGIN
DALI_PROPERTY( "clippingEnable",    BOOLEAN,    true,    false,   true,             Dali::Layer::Property::CLIPPING_ENABLE )
DALI_PROPERTY( "clippingBox",       RECTANGLE,  true,    false,   true,             Dali::Layer::Property::CLIPPING_BOX    )
DALI_PROPERTY( "behavior",          STRING,     true,    false,   false,            Dali::Layer::Property::BEHAVIOR        )
DALI_PROPERTY_TABLE_END( DEFAULT_DERIVED_ACTOR_PROPERTY_START_INDEX )
@endcode

<b>Notes:</b>

- The table lays within an unnamed namespace.
- The table should be in the same order as the enum.
- The table should be the only place where the text names of the properties are defined.
- The information in the table should be used within the classes IsDefaultPropertyWritable / Animatable / ConstraintInput methods for quick lookup.
- The last entry in the table is optionally used in debug builds for index checking.
- The parameter to DALI_PROPERTY_TABLE_END should match the start index of the property enumeration.

<br>
<h2 class="pg">How to implement a property within DALi Toolkit controls and application-side custom controls:</h2>

Macros are used to define properties for the following reasons:

- To standardise the way properties are defined.
- To handle type-registering for properties, signals and actions in one place.
- To facilitate the posibility of running the code with the type-registry disabled.

Two different macros are provided depending on whether the property is to be an event-side only property or an animatable property.

<b>There are two stages:</b>

- Define the properties as an enum in the public-api header file, along with a definition of the property ranges.
- Define the property details using the pre-defined macros to perform the type-registering of the properties. This is done for signals and actions also.

<b>Example: ImageView</b>

Source file: <b>image-view.h</b>:
Note that the “PropertyRange” contents “PROPERTY_START_INDEX” & "ANIMATABLE_PROPERTY_REGISTRATION_START_INDEX" are also used by the macro for order checking.

@code
  /**
   * @brief The start and end property ranges for this control.
   */
  enum PropertyRange
  {
    PROPERTY_START_INDEX = Control::CONTROL_PROPERTY_END_INDEX + 1,  ///< @SINCE_1_0.0
    PROPERTY_END_INDEX =   PROPERTY_START_INDEX + 1000,              ///< Reserve property indices @SINCE_1_0.0

    ANIMATABLE_PROPERTY_START_INDEX = ANIMATABLE_PROPERTY_REGISTRATION_START_INDEX,        ///< @SINCE_1_1.18
    ANIMATABLE_PROPERTY_END_INDEX =   ANIMATABLE_PROPERTY_REGISTRATION_START_INDEX + 1000  ///< Reserve animatable property indices, @SINCE_1_1.18
  };

  /**
   * @brief An enumeration of properties belonging to the Button class.
   */
  struct Property
  {
    enum
    {
      // Event side properties

      /**
       * @DEPRECATED_1_1.16. Use IMAGE instead.
       * @brief name "resourceUrl", type string
       * @SINCE_1_0.0
       */
      RESOURCE_URL = PROPERTY_START_INDEX,
      /**
       * @brief name "image", type string if it is a url, map otherwise
       * @SINCE_1_0.0
       */
      IMAGE,
      /**
       * @brief name "preMultipliedAlpha", type Boolean
       * @SINCE_1_1.18
       * @pre image must be initialized.
       */
      PRE_MULTIPLIED_ALPHA,

      // Animatable properties

      /**
       * @brief name "pixelArea", type Vector4
       * @details Pixel area is a relative value with the whole image area as [0.0, 0.0, 1.0, 1.0].
       * @SINCE_1_0.18
       */
      PIXEL_AREA = ANIMATABLE_PROPERTY_START_INDEX,
    };
  };
@endcode

Source file: <b>image-view-impl.cpp</b>, within an unnamed namespace:

@code
#include <dali/public-api/object/type-registry-helper.h>
@endcode

@clip{"image-view-impl.cpp",DALI_TYPE_REGISTRATION_BEGIN,DALI_TYPE_REGISTRATION_END}

<b>Notes:</b>

- The “Create” parameter to the begin macro is the function pointer to the creation function.
- Properties should be in the same order as in the enum.
- Signals and actions are registered likewise in that order.
- Properties type-registered using these macros will have their order checked at compile time. If you get an indexing compile error, check the order matches the enum order.
    The error will look like this: " error: invalid application of 'sizeof' to incomplete type 'Dali::CompileTimeAssertBool<false>' "
- If using the Pimpl design pattern when creating a custom control from within an application, the Handle (public) and Object (internal) classes should have the same name. They can be separated by different namespaces.
    This requirement is actually due to how the type-registry in DALi looks up properties.

<br>
<hr>
@section property-indices Property Indices

The properties are enumerated to give them a unique index. This index can be used to access them.
The indecies must be unique per flattened derivation heirachy.
EG:
- CameraActor derives from Actor. No property indicies in either CameraActor or Actor should collide with each other.
- ActiveConstraintBase derives from Object. It CAN have property indices that match Actor or CameraActor.

There are some predefined start indecies and ranges that should be used for common cases, these are defined below:


DALi has a property system and provides several different kinds of properties. The following table
shows the index range of the different properties in place.

| Kind                  | Description                                                                                       | Start Index                                                                                                | End Index                                                                                                                          |
|:----------------------|:--------------------------------------------------------------------------------------------------|:----------------------------------------------------------------------------------------------------------:|:----------------------------------------------------------------------------------------------------------------------------------:|
| Default               | Properties defined within DALi Core, e.g. Dali::Actor default properties etc.                     | \link Dali::DEFAULT_OBJECT_PROPERTY_START_INDEX DEFAULT_OBJECT_PROPERTY_START_INDEX\endlink                | \link Dali::DEFAULT_PROPERTY_MAX_COUNT DEFAULT_PROPERTY_MAX_COUNT\endlink (9999999)                                                |
| Registered            | Properties registered using Dali::PropertyRegistration                                            | \link Dali::PROPERTY_REGISTRATION_START_INDEX PROPERTY_REGISTRATION_START_INDEX\endlink (10000000)         | \link Dali::PROPERTY_REGISTRATION_MAX_INDEX PROPERTY_REGISTRATION_MAX_INDEX\endlink (19999999)                                     |
| Control               | Property range reserved by Dali::Toolkit::Control                                                 | \link Dali::Toolkit::Control::CONTROL_PROPERTY_START_INDEX CONTROL_PROPERTY_START_INDEX\endlink (10000000) | \link Dali::Toolkit::Control::CONTROL_PROPERTY_END_INDEX CONTROL_PROPERTY_END_INDEX\endlink (10001000)                             |
| Derived Control       | Property range for control deriving directly from Dali::Toolkit::Control                          | 10001001                                                                                                   | \link Dali::PROPERTY_REGISTRATION_MAX_INDEX PROPERTY_REGISTRATION_MAX_INDEX\endlink (19999999)                                     |
| Registered Animatable | Animatable properties registered using Dali::AnimatablePropertyRegistration                       | \link Dali::ANIMATABLE_PROPERTY_REGISTRATION_START_INDEX ANIMATABLE_PROPERTY_REGISTRATION_START_INDEX\endlink (20000000) | \link Dali::ANIMATABLE_PROPERTY_REGISTRATION_MAX_INDEX ANIMATABLE_PROPERTY_REGISTRATION_MAX_INDEX\endlink (29999999) |
| Registered Child      | Child properties (which parent supports in its children) registered using Dali::ChildPropertyRegistration   | \link Dali::CHILD_PROPERTY_REGISTRATION_START_INDEX CHILD_PROPERTY_REGISTRATION_START_INDEX\endlink (45000000) | \link Dali::CHILD_PROPERTY_REGISTRATION_MAX_INDEX CHILD_PROPERTY_REGISTRATION_MAX_INDEX\endlink (49999999) |
| Custom                | Custom properties added to instance using Dali::Handle::RegisterProperty                          | \link Dali::PROPERTY_CUSTOM_START_INDEX PROPERTY_CUSTOM_START_INDEX\endlink (50000000)                     | Onwards...                                                                                                                         |

<br>
<hr>
@section property-use-example-cpp Property use example C++

Common uses for properties are constraints and animations.

An application developer can use an existing property, or, if necessary, register their own.

Here is a code example.

This example shows how to register and look-up custom properties.
An image is added to the screen which changes and a custom property is added to the image-view.
This value is incremented every time the image is touched and the text-label is updated.
When touched, the property is looked up by index (as this is much faster than a text lookup of the property name).

Property lookup via index should always be used unless the indicies cannot be known. If the property reader was completely decoupled from the creation, e.g. A custom control with a custom property being used by external application code, then it may be necessary. In this case the application writer should aim to perform the text lookup once at start-up, and cache the property index locally.

@clip{"properties.cpp", // C++ EXAMPLE, // C++ EXAMPLE END}

<br>
<hr>
@section property-use-example-js Property use in JavaScript

Note that constraints cannot be used within JavaScript, so below is a simple example that sets one of the default properties; scale:

@code
var imageView = new dali.Control( "ImageView" );

// by default an actor is anchored to the top-left of it's parent actor
// change it to the middle
imageView.parentOrigin = dali.CENTER;

// Set an image view property
imageView.image = {
  "visualType" : "IMAGE",
  "url": "images/icon-0.png",
  "desiredWidth" : 100,
  "desiredHeight" : 100
};

// add to the stage
dali.stage.add( imageView );
@endcode

<br>
<hr>
@section property-use-example-json Property use in JSON

This is a basic example of a button defined in JSON by setting the default properties.

@code
{
  "stage":
  [
    {
      "type": "ImageView",
      "parentOrigin": "CENTER",
      "anchorPoint": "CENTER",
      "position": [0, 0, 0],
      "image":
      {
        "visualType" : "IMAGE",
        "url" : "images/icon-0.png",
        "desiredWidth" : 100,
        "desiredHeight" : 100
      }
    }
  ]
}
@endcode

*
*/
