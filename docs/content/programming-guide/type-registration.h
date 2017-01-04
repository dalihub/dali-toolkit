/*! \page type-registration Type Registration
 *
@section Overview

DALi has a \link Dali::TypeRegistry type registration \endlink system which can be used to register
a derived actor/control type along with specifying a method which is used to create this type. This
type registration normally takes place at library load time.

Once a type is registered, properties, signals and actions can also be registered for all instances
of this type.

This then allows the application writer to create instances using just the type name; getting and setting properties using a property name or index; connect to
signals using only the signal name; and activate an action by just using the action name.

This topic covers:

 - @ref register-type
 - @ref register-property
 - @ref register-signal
 - @ref register-action
 - @ref using-type
 - @ref using-property
 - @ref using-signal
 - @ref using-action

@section register-type Registering a Type

A type can be registered using Dali::TypeRegistration. This is normally done in an unnamed namespace
within the source file of the deriving control as shown in the code below.

<b>Please note:</b> This snippet assumes knowledge of the \link Dali::Toolkit::Control Control
\endlink / \link Dali::Toolkit::Internal::Control Internal::Control \endlink creation process where
<i><b>MyControl</b></i> derives from a Control and <i><b>MyControlImpl</b></i> derives from Internal::Control.

@code
namespace
{

Dali::BaseHandle CreateMyControl()
{
  // Create an instance of MyControl and return the handle.
  return MyControlImpl::New();
}

DALI_TYPE_REGISTRATION_BEGIN( MyControl, Toolkit::Control, CreateMyControl );
DALI_TYPE_REGISTRATION_END()

} // unnamed namespace
@endcode

This registration macro informs DALi of the existence of MyControl type, which class it derives from, and a method for creating an instance of MyControl.


@section register-property Registering a Property

DALi has a property system which can be extended by registering more properties through the type
registry. The property index is <b><i>very important</i></b> when registering these properties and
all property indices should be between Dali::PROPERTY_REGISTRATION_START_INDEX and
Dali::PROPERTY_REGISTRATION_MAX_INDEX.

Furthermore, if deriving from a \link Dali::Toolkit::Control Control\endlink, the control
writer needs to be aware of their parent class's property range to avoid overlapping indices, so should start their property indices after their parent's range.
Control reserves a property range between
\link Dali::Toolkit::Control::CONTROL_PROPERTY_START_INDEX Control::CONTROL_PROPERTY_START_INDEX\endlink
and \link Dali::Toolkit::Control::CONTROL_PROPERTY_END_INDEX Control::CONTROL_PROPERTY_END_INDEX\endlink.

Any control deriving from \link Dali::Toolkit::Control Control\endlink
should start at
\link Dali::Toolkit::Control::CONTROL_PROPERTY_END_INDEX Control::CONTROL_PROPERTY_END_INDEX\endlink + 1.
Controls deriving from an existing control such as \link Dali::Toolkit::Button Button\endlink should start at
\link Dali::Toolkit::Button::PROPERTY_END_INDEX Button::PROPERTY_END_INDEX\endlink + 1.

Please have a look at \ref property-indices for more information.

The following code shows how a property can be added to a type.

@code
// Define the indices we will use for the properties:

class MyControl : public Control
{
  /**
   * @brief The start and end property ranges for this control.
   */
  enum PropertyRange
  {
    PROPERTY_START_INDEX = Control::CONTROL_PROPERTY_END_INDEX + 1,
    PROPERTY_END_INDEX =   PROPERTY_START_INDEX + 1000,

    ANIMATABLE_PROPERTY_START_INDEX = ANIMATABLE_PROPERTY_REGISTRATION_START_INDEX,
    ANIMATABLE_PROPERTY_END_INDEX =   ANIMATABLE_PROPERTY_REGISTRATION_START_INDEX + 1000
  };

  struct Property
  {
    enum
    {
      // Event side properties

      /**
       * @brief name "propertyOne", type bool
       * @details Enables the feature.
       * @SINCE_1_0.0
       */
      PROPERTY_ONE = PROPERTY_START_INDEX,

      /**
       * @brief name "propertyTwo", type float
       * @details Controls the level of the feature
       * @SINCE_1_0.0
       */
      PROPERTY_TWO,

      /**
       * @brief name "propertyThree", type Vector4
       * @details The foreground color
       * @SINCE_1_0.0
       */
      PROPERTY_THREE,

      // Animatable properties

      /**
       * @brief name "propertyFour", type Vector4
       * @details Animatable parameters of the feature
       * @SINCE_1_0.0
       */
      PROPERTY_FOUR = ANIMATABLE_PROPERTY_START_INDEX,
    };
  };

  /// ...
};
@endcode

The control and properties are registered with the TypeRegistry using the following macros:

@code
DALI_TYPE_REGISTRATION_BEGIN( MyControl, Toolkit::Control, CreateMyControl );
DALI_PROPERTY_REGISTRATION( AppNamespace, MyControl, "property1", BOOLEAN, PROPERTY_ONE )
DALI_PROPERTY_REGISTRATION( AppNamespace, MyControl, "property2", FLOAT, PROPERTY_TWO )
DALI_PROPERTY_REGISTRATION( AppNamespace, MyControl, "property3", VECTOR4, PROPERTY_THREE )

DALI_ANIMATABLE_PROPERTY_REGISTRATION_WITH_DEFAULT( AppNamespace, MyControl, "property4", Vector4(0.f, 0.f, 1.f, 1.f), PROPERTY_FOUR )

DALI_TYPE_REGISTRATION_END()
@endcode

The DALI_PROPERTY_REGISTRATION macro requires that you define the class methods SetProperty() and GetProperty().

The DALI_ANIMATABLE_PROPERTY_REGISTRATION macros automatically create and handle scene-graph values, and do not need any code in your derived class. Just use the property index in animation or constraint methods.

The SetProperty class method has to be static, and follows the format:

@code
void MyControl::SetProperty(
    Dali::BaseObject* object,          // A pointer to an instance of MyControl
    Dali::Property::Index index,       // The index of the property to set
    const Dali::Property::Value& value // The value to set the property to
)
{
  // DownCast to MyControl so that we can do the specific behaviour
  MyControl control = MyControl::DownCast( Dali::BaseHandle ( object ) );

  if ( control )
  {
    MyControlImpl& controlImpl( GetImplementation( control ) );

    switch ( index )
    {
      case PROPERTY_ONE:
      {
        // Assume we already have a method in MyControl which sets the appropriate value and takes in a boolean
        bool property;
        if( value.Get( property ) )
        {
          controlImpl.SetPropertyOne( property );
        }
        break;
      }

      case PROPERTY_TWO
      {
        // Assume we already have a method in MyControl which sets the appropriate value and takes in a float
        float property;
        if( value.Get( property ) )
        {
          controlImpl.SetPropertyTwo( property );
        }
        break;
      }

      case PROPERTY_THREE
      {
        // Assume we already have a method in MyControl which sets the appropriate value and takes in a Vector4
        Vector4 property;
        if( value.Get( property ) )
        {
          controlImpl.SetPropertyThree( property );
        }
        break;
      }
    }
  }
}
@endcode

And the GetProperty method also has to be static and takes the form:

@code
Property::Value MyControl::GetProperty(
    BaseObject* object,     // A pointer to an instance of MyControl
    Property::Index index ) // The index of the property to retrieve
{
  Property::Value value;

  // DownCast to MyControl so that we can do the specific behaviour
  MyControl control = MyControl::DownCast( Dali::BaseHandle ( object ) );

  if ( control )
  {
    MyControlImpl& controlImpl( GetImplementation( control ) );

    switch ( index )
    {
      case PROPERTY_ONE:
      {
        // Assume we have a member variable that stores the value of this property
        value = controlImpl.mPropertyOne;
        break;
      }

      case PROPERTY_TWO:
      {
        // Assume we have a member variable that stores the value of this property
        value = controlImpl.mPropertyTwo;
        break;
      }

      case PROPERTY_THREE:
      {
        // Assume we have a member variable that stores the value of this property
        value = controlImpl.mPropertyThree;
        break;
      }
    }
  }
}
@endcode

@section using-property Setting & Getting Registered Properties

Like other properties, type registered properties can also be set and their values can be retrieved
in a similar manner. The code below shows how this can be done.

@code
Dali::TypeInfo type = Dali::TypeRegistry::Get().GetTypeInfo( "MyControl" );

if ( type )
{
  Dali::BaseHandle baseHandle = type.CreateInstance();

  if ( baseHandle )
  {
    // Handle deals with properties, so DownCast
    Dali::Handle handle = Dali::Handle::DownCast( baseHandle );

    if ( handle )
    {
      // Setting a property
      handle.SetProperty( MyControl::Property::PROPERTY_ONE, 11.0f );

      // Get the property name
      std::cout << "Property1 name is: " << handle.GetPropertyName( MyControl::Property::PROPERTY_ONE ) << std::endl;

      // Get the property
      bool propertyOne = handle.GetProperty< bool >( MyControl::Property::PROPERTY_ONE );

      // Set the second property
      handle.SetProperty( MyControl::Property::PROPERTY_TWO, 4.0f );
    }
  }
}
@endcode

@section register-signal Registering a Signal

Once we've registered a type, we can then inform the type-registry about any signals that our type has:

@code
// Define the names of the signals
static const char * const SIGNAL_ONE( "signal1" );
static const char * const SIGNAL_TWO( "signal2" );
static const char * const SIGNAL_THREE( "signal3" );

Dali::SignalConnectorType signal1(
   type,                       // Reference to type registration object (see above)
   SIGNAL_ONE,                 // Name of our signal
   &MyControl::DoConnectSignal // Function to call when a call to connect to this signal is received
);

// Register more signals
Dali::SignalConnectorType signal2( type, SIGNAL_TWO,   &MyControl::DoConnectSignal );
Dali::SignalConnectorType signal3( type, SIGNAL_THREE, &MyControl::DoConnectSignal );
@endcode

It is recommended to use static members (of MyControl class) for the signal names. That way
applications can also use the static member rather than have to look up the name.

The method that handles the signal connection has to be static and takes the form:

@code
bool MyControl::DoConnectSignal(
    Dali::BaseObject* object,                  // A pointer to an instance of MyControl
    Dali::ConnectionTrackerInterface* tracker, // The object connecting to the signal
    const std::string& signalName,             // The name of the signal to connect to
    Dali::FunctorDelegate* functor             // The functor
)
{
  bool connected( false );

  // DownCast to MyControl so that we can call the signal connection methods
  MyControl control = MyControl::DownCast( Dali::BaseHandle ( object ) );

  if ( control )
  {
    if ( signalName == SIGNAL_ONE )
    {
      control.SignalOne().Connect( tracker, functor );
      connected = true;
    }
    else if ( signalName == SIGNAL_TWO )
    {
      control.SignalTwo().Connect( tracker, functor );
      connected = true;
    }
    else if ( signalName == SIGNAL_THREE )
    {
      control.SignalThree().Connect( tracker, functor );
      connected = true;
    }
  }

  return connected; // Return true if connection successfully created
}
@endcode

@section register-action Registering an Action

Created controls are able to perform a variety of default actions. Registering an action with the
type registry allows application writers to perform this action by using the name.

An action can be added to a type as shown below:

@code
// Define the names of the actions
static const char * const ACTION_ONE( "action1" );
static const char * const ACTION_TWO( "action2" );
static const char * const ACTION_THREE( "action3" );

Dali::TypeAction action1(
    type,                // Reference to type registration object (see above)
    ACTION_ONE,          // Name of the action
    &MyControl::DoAction // Function to call when someone wants to perform this action
);

// Register mode actions
Dali::TypeAction action2( type, ACTION_TWO,   &MyControl::DoAction );
Dali::TypeAction action3( type, ACTION_THREE, &MyControl::DoAction );
@endcode

It is recommended to use static members (of MyControl class) for the action names. That way
applications can also use the static member rather than have to look up the name.

The method that handles the action has to be static and takes the form:

@code
bool MyControl::DoAction(
    Dali::BaseObject* object,                              // A pointer to an instance of MyControl
    const std::string& actionName,                         // The name of the action to perform
    const std::vector< Dali::Property::Value >& attributes // Any passed in attributes
)
{
  bool performed( false );

  Dali::BaseHandle handle(object);

  // DownCast to MyControl so that we can do the specific behaviour
  MyControl control = MyControl::DownCast( Dali::BaseHandle ( object ) );

  if ( control )
  {
    if ( actionName == ACTION_ONE )
    {
      // Do action1 e.g. button click etc.
    }
    else if ( actionName == ACTION_TWO )
    {
      // Do action2, which can have attributes
      if ( !attributes.empty() )
      {
        // Let's assume action2 expects a std::string as an attribute, here's how we'd extract that
        std::cout << "action2 printing out: " << attributes[0].Get< std::string >() ) << std::endl;
      }
    }
    else if ( actionName == ACTION_THREE )
    {
      // Do action3
    }
  }

  return performed; // Return true if action successfully performed
}
@endcode

If the action is not performed by the derived class, it will be propagated to the base class.
For example, in the above case, MyControl can perform "action1" so should return true, but it
cannot perform "action4" so should return false and propagate the action to Control.


@section using-type Creating an instance of a Registered Type

When a type is registered with the \link Dali::TypeRegistry type registry\endlink, it allows the
application writer to get information about the type and even create an instance of it.

@code
Dali::TypeInfo type = Dali::TypeRegistry::Get().GetTypeInfo( "MyControl" );

// If type specified is not found, then type will be NULL.
if ( type )
{
  Dali::BaseHandle handle = type.CreateInstance();

  // Can use DownCast to change to MyControl type if required
  if ( handle )
  {
    MyControl control = MyControl::DownCast( handle );
  }
}
@endcode

Normally we would not do the DownCast, just utilise the signals, actions and properties.

@section using-signal Connecting to a Registered Signal

The advantage of registering a signal using the \link Dali::TypeRegistry type registry \endlink is
that you can connect to a particular signal using just the name of the signal.

The application code would look as follows:

@code
class MyApp
{
public:

  // Assume this is called when creating MyApp
  void Create()
  {
    Dali::TypeInfo type = Dali::TypeRegistry::Get().GetTypeInfo( "MyControl" );

    if ( type )
    {
      mHandle = type.CreateInstance();

      if ( mHandle )
      {
        // Connect to signal1 by using its name
        handle.ConnectSignal( &mConnectionTracker, "signal1", &MyApp::SignalReceived ) )
      }
    }
  }

  // This method will be called when "signal1" is emitted
  void SignalReceived()
  {
    // Do Something when "signal1" is received
    std::cout << "signal1 received" << std::endl;
  }

private:
  Dali::BaseHandle mHandle; // Handle to MyControl created via the type-registry
  Dali::ConnectionTracker mConnectionTracker; // Used for automatic signal disconnection upon its destruction
};
@endcode

@section using-action Performing a Registered Action

Once an action is registered, the application writer can perform that action using the action name:

@code
Dali::TypeInfo type = Dali::TypeRegistry::Get().GetTypeInfo( "MyControl" );

if ( type )
{
  Dali::BaseHandle handle = type.CreateInstance();

  if ( handle )
  {
    // Perform action1, no attributes
    handle.DoAction( "action1", std::vector< Dali::Property::Value >() );

    // Create an attribute vector for action2
    std::vector< Dali::Property::Value > action2Attributes;
    action2Attributes.push_back( Dali::Property::Value( "Hello-Action-2" ) );

    // Perform action2, with attributes
    handle.DoAction( "action2", action2Attributes );
  }
}
@endcode

*
*/
