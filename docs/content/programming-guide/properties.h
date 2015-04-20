/*! \page properties Properties
 *
@section what-is-a-property What is a property?

A property is a value used by an object that can be modified or read externally to that object.
This could be from within Dali or externally by an application.

<h2 class="pg">What is a property used for?</h2>

Properties can be set externally by an application, allowing that application to change the configuration or behaviour of an actor.
This could include the physical geometry of the actor, or how it is drawn or moves.

Properties can also be read. This feature can be used in conjunction with constraints to allow changes to a property within one actor to cause changes to the property of another actor. For example, an actor following the movement of another separate actor (that it is not a child of). 

Properties can be used to expose any useful information or behaviour of an actor.
Other actor variables that are used to implement this bevahiour, or do not make useful sense from an application developers point of view should not be exposed.

<h2 class="pg">How to implement a property within Dali-core:</h2>

<b>There are two stages:</b>

- Define the properties as an enum in the public-api header file.
- Define the property details using the pre-defined macros to build up a table of property information.

There are some pre-defined macros designed to help with and standardise the definition of the propery details table per class.

These macros generate an array of property details which allow efficient lookup of flags like "animatable" or "constraint input".

<b>Example: ImageActor</b>

Within the public-api header file; image-actor.h:

@code
/**
 * @brief An enumeration of properties belonging to the ImageActor class.
 * Properties additional to RenderableActor.
 */
struct Property
{
  enum
  {
    PIXEL_AREA = DEFAULT_DERIVED_ACTOR_PROPERTY_START_INDEX, ///< name "pixel-area",  type Rect<int>
    STYLE,                                                   ///< name "style",       type std::string
    BORDER,                                                  ///< name "border",      type Vector4
    IMAGE,                                                   ///< name "image",       type Map {"filename":"", "load-policy":...}
  };
};
@endcode
From @ref Dali::ImageActor::Property

<b>Notes:</b>

- The properties are enumerated within a named struct to give them a namespace.
- The properties are then refered to as &lt;OBJECT&gt;::%Property::&lt;PROPERTY_NAME&gt;.

Within the internal implementation; <b>image-actor-impl.cpp</b>:

@code
namespace // Unnamed namespace
{

// Properties

//              Name           Type   writable animatable constraint-input  enum for index-checking
DALI_PROPERTY_TABLE_BEGIN
DALI_PROPERTY( "pixel-area",   RECTANGLE, true,    false,   true,    Dali::ImageActor::Property::PIXEL_AREA )
DALI_PROPERTY( "style",        STRING,    true,    false,   true,    Dali::ImageActor::Property::STYLE      )
DALI_PROPERTY( "border",       VECTOR4,   true,    false,   true,    Dali::ImageActor::Property::BORDER     )
DALI_PROPERTY( "image",        MAP,       true,    false,   false,   Dali::ImageActor::Property::IMAGE      )
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
<h2 class="pg">How to implement a property within Dali-toolkit:</h2>

Note that toolkit properties have extra limitations in that they cannot be animated or used as a constraint input. For this reason there is no requirement for a table of property details.
Macros are still used to define properties, but for the following reasons:

To standardise the way properties are defined.
To handle type-registering for properties, signals and actions in one place.
To facilitate the posibility of running the code with the type-registry disabled.

<b>There are two stages:</b>

- Define the properties as an enum in the public-api header file, along with a definition of the property ranges.
- Define the property details using the pre-defined macros to perform the type-registering of the properties. This is done for signals and actions also.

<b>Example: Button</b>

Source file: <b>button.h</b>:
Note that the “PropertyRange” contents “PROPERTY_START_INDEX” is also used by the macro for order checking.

@code
  /**
   * @brief The start and end property ranges for this control.
   */
  enum PropertyRange
  {
    PROPERTY_START_INDEX = Control::CONTROL_PROPERTY_END_INDEX + 1,
    PROPERTY_END_INDEX =   PROPERTY_START_INDEX + 1000              ///< Reserve property indices
  };

  /**
   * @brief An enumeration of properties belonging to the Button class.
   */
  struct Property
  {
    enum
    {
      DISABLED = PROPERTY_START_INDEX, ///< name "disabled",                     @see SetDisabled(),                  type bool
      AUTO_REPEATING,                  ///< name "auto-repeating",               @see SetAutoRepeating(),             type bool
      INITIAL_AUTO_REPEATING_DELAY,    ///< name "initial-auto-repeating-delay", @see SetInitialAutoRepeatingDelay(), type float
      NEXT_AUTO_REPEATING_DELAY,       ///< name "next-auto-repeating-delay",    @see SetNextAutoRepeatingDelay(),    type float
      TOGGLABLE,                       ///< name "togglable",                    @see SetTogglableButton(),           type bool
      SELECTED,                        ///< name "selected",                     @see SetSelected(),                  type bool
      NORMAL_STATE_ACTOR,              ///< name "normal-state-actor",           @see SetButtonImage(),               type Map
      SELECTED_STATE_ACTOR,            ///< name "selected-state-actor",         @see SetSelectedImage(),             type Map
      DISABLED_STATE_ACTOR,            ///< name "disabled-state-actor",         @see SetDisabledImage(),             type Map
      LABEL_ACTOR,                     ///< name "label-actor",                  @see SetLabel(),                     type Map
    };
  };
@endcode

Source file: <b>button-impl.cpp</b>, within an unnamed namespace:

@clip{"button-impl.cpp",DALI_TYPE_REGISTRATION_BEGIN,DALI_TYPE_REGISTRATION_END}

<b>Notes:</b>

- The “Create” parameter to the begin macro is the function pointer to the creation function.
- Properties should be in the same order as in the enum.
- Signals and actions are registered likewise in that order.
- Properties type-registered using these macros will have their order checked at compile time. If you get an indexing compile error, check the order matches the enum order.


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

<table>
  <tr> <td><b>Kind</b></td>     <td><b>Description</b></td>                                                                                <td style="text-align:center;"><b>Start Index</b></td><td><b>End Index</b></td>         </tr>
  <tr> <td>Default</td>         <td>Properties defined within DALi Core, e.g. Dali::Actor, Dali::ShaderEffect default properties etc.</td> <td style="text-align:center;">\link Dali::DEFAULT_OBJECT_PROPERTY_START_INDEX DEFAULT_OBJECT_PROPERTY_START_INDEX\endlink (0)</td><td>9999999</td>         </tr>
  <tr> <td>Registered</td>      <td>Properties registered using Dali::PropertyRegistration</td>                                            <td style="text-align:center;">\link Dali::PROPERTY_REGISTRATION_START_INDEX PROPERTY_REGISTRATION_START_INDEX\endlink (10000000)</td><td>\link Dali::PROPERTY_REGISTRATION_MAX_INDEX PROPERTY_REGISTRATION_MAX_INDEX\endlink (19999999)</td> </tr>
  <tr> <td>Control</td>         <td>Property range reserved by Dali::Toolkit::Control</td>                                                 <td style="text-align:center;">\link Dali::Toolkit::Control::CONTROL_PROPERTY_START_INDEX CONTROL_PROPERTY_START_INDEX\endlink (10000000)</td><td>
  \link Dali::Toolkit::Control::CONTROL_PROPERTY_END_INDEX CONTROL_PROPERTY_END_INDEX\endlink (10001000)</td></tr>
  <tr> <td>Derived Control</td> <td>Property range for control deriving directly from Dali::Toolkit::Control</td>                          <td style="text-align:center;">10001001</td><td>\link Dali::PROPERTY_REGISTRATION_MAX_INDEX PROPERTY_REGISTRATION_MAX_INDEX\endlink (19999999)</td> </tr>
  <tr> <td>Custom</td>          <td>Custom properties added to instance using Dali::Handle::RegisterProperty</td>                          <td style="text-align:center;">\link Dali::PROPERTY_CUSTOM_START_INDEX PROPERTY_CUSTOM_START_INDEX\endlink (50000000)</td><td>Onwards...</td>     </tr>
</table>


<br>
<hr>
@section property-use-example-cpp Property use example C++

Common uses for properties are constraints and animations.

An application developer can use an existing property, or, if necessary, register their own.

Here is a code example.

This example shows how to register and look-up custom properties.
A grid of buttons is created, each with a new "tag" property which is set to a unique value. The index to this property is cached for later use.
When pressed, the property is looked up by index (as this is much faster than a text lookup of the property name).

Property lookup via index should always be used unless the indecies cannot be known. If the property reader was completely decoupled from the creation, EG. A custom control with a custom property being used by external application code, then it may be necessary. In this case the application writer should aim to perform the text lookup once at start-up, and cache the property index locally.

@clip{"property-example.cpp", void Create, return true;}

Once run, a grid of buttons will appear. When a button is pressed, the unique number stored in the property (in this case the index) is displayed at the bottom of the screen.

<br>
<hr>
@section property-use-example-js Property use in JavaScript

Note that constraints cannot be used within JavaScript, so below is a simple example that sets one of the default properties; scale:

@code
var image = new dali.ResourceImage( {url:"background.png"} );
var imageActor = new dali.ImageActor( image );

// by default an actor is anchored to the top-left of it's parent actor
// change it to the middle
imageActor.parentOrigin = dali.CENTER;

// scale it up by 2 times  in x,y
imageActor.scale = [ 2, 2, 1  ];

// add to the stage
dali.stage.add( imageActor );
@endcode

For a more detailed example see the ShaderEffect example in the JavaScript documentation.

<br>
<hr>
@section property-use-example-json Property use in JSON

This is a basic example of a button defined in JSON by setting the default properties.

@code
{
  "constants": {
    "CONFIG_SCRIPT_LOG_LEVEL": "Verbose"
  },
  "stage": [
    // First Button
    {
      "type": "PushButton",
      "parent-origin": "TOP_CENTER",
      "anchor-point": "TOP_CENTER",
      "position": [0, 0, 0],
      "size": [0, 200, 0],
      "normal-state-actor": {
        "type": "ImageActor",
        "image": {
          "filename": "{DALI_IMAGE_DIR}blocks-brick-1.png"
        }
      },
      "selected-state-actor": {
        "type": "ImageActor",
        "image": {
          "filename": "{DALI_IMAGE_DIR}blocks-brick-2.png"
        }
      },
      "label-actor": {
        "type": "TextLabel",
        "text": "Normal"
      }
    }
  ]
}
@endcode

*
*/
