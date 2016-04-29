/*! \page type-registration Type Registration
 *
@section Overview

DALi has a \link Dali::TypeRegistry type registration \endlink system which can be used to register
a derived actor/control type along with specifying a method which is used to create this type. This
type registration normally takes place at library load time.

Once a type is registered, signals, actions and properties can also be registered for all instances
of this type.

This then allows the application writer to create instances using just the type name; connect to
signals using only the signal name; activate an action by just using the action name; and finally,
getting and setting properties using a property name or index.

This topic covers:

 - @ref register-type
 - @ref register-signal
 - @ref register-action
 - @ref register-property
 - @ref using-type
 - @ref using-signal
 - @ref using-action
 - @ref using-property

@section register-type Registering a Type

A type can be registered using Dali::TypeRegistration. This is normally done in an unnamed namespace
within the source file of the deriving control as shown in the code below.

<b>Please note:</b> This snippet assumes knowledge of the \link Dali::Toolkit::Control Control
\endlink / \link Dali::Toolkit::Internal::Control Internal::Control \endlink creation process where
<i><b>MyControl</b></i> derives from a Control and <i><b>MyControlImpl</b></i> derives from Internal::Control.

@code
namespace
{

Dali::BaseHandle Create()
{
  // Create an instance of MyControl and return the handle.
  return MyControlImpl::New();
}

Dali::TypeRegistration type(
    typeid( MyControl ),              // Type ID of our Control
    typeid( Dali::Toolkit::Control ), // Type ID of what our Control derives from
    Create                            // Function which creates our Control, signature shown above
);

} // unnamed namespace
@endcode

This registration informs DALi of the existence of MyControl type.

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

@section register-property Registering a Property

DALi has a property system which can be extended by registering more properties through the type
registry. The property index is <b><i>very important</i></b> when registering these properties and
all property indices should be between Dali::PROPERTY_REGISTRATION_START_INDEX and
Dali::PROPERTY_REGISTRATION_MAX_INDEX.

Furthermore, if deriving from \link Dali::Toolkit::Control Control\endlink, the control writer
needs to be aware of their parent class's property range. Control reserves a property range between
\link Dali::Toolkit::Control::CONTROL_PROPERTY_START_INDEX ControlImpl::CONTROL_PROPERTY_START_INDEX\endlink
and \link Dali::Toolkit::Control::CONTROL_PROPERTY_END_INDEX Control::CONTROL_PROPERTY_END_INDEX\endlink.
Any deriving control should start their property indices from
\link Dali::Toolkit::Control::CONTROL_PROPERTY_END_INDEX Control::CONTROL_PROPERTY_END_INDEX\endlink + 1.

Please have a look at \ref property-indices for more information.

The following code shows how a property can be added to a type.

@code
// Define the indices we will use for the properties
static const int PROPERTY_ONE( Dali::Toolkit::Internal::Control::CONTROL_PROPERTY_END_INDEX + 1 );
static const int PROPERTY_TWO( Dali::Toolkit::Internal::Control::CONTROL_PROPERTY_END_INDEX + 2 );
static const int PROPERTY_THREE( Dali::Toolkit::Internal::Control::CONTROL_PROPERTY_END_INDEX + 3 );

Dali::PropertyRegistration property1(
    type,                    // Reference to type registration object (see above)
    "property1",             // Name of the property
    PROPERTY_ONE,            // Index of this property
    Dali::Property::BOOLEAN, // The property type
    &MyControl::SetProperty, // Method called when property is set
    &MyControl::GetProperty  // Method called when retrieving the value of the property
);

// Register more properties
Dali::PropertyRegistration property2(
    type, "property2", PROPERTY_TWO, Dali::Property::FLOAT,
    NULL, // SetProperty is NULL, means that this property is a read-only property
    &MyControl::GetProperty
);
Dali::PropertyRegistration property3( type, "property3", PROPERTY_THREE, Dali::Property::FLOAT, &MyControl::SetProperty, &MyControl::GetProperty);
@endcode

It is recommended to use static members (of MyControl class) for the property indices. That way
applications can also use the static member as well. If they require the property name, they can
just call the Dali::Handle::GetPropertyName().

The method that deals with setting the property has to be static, and follows the format:

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
        controlImpl.SetPropertyOne( value.Get< bool >() );
        break;
      }

      // PROPERTY_TWO is read-only so does not need to be handled

      case PROPERTY_THREE
      {
        // Assume we already have a method in MyControl which sets the appropriate value and takes in a float
        controlImpl.SetPropertyThree( value.Get< float >() );
        break;
      }
    }
  }
}
@endcode

And the function to retrieve the property value also has to be static and takes the form:

@code
Property::Value MyControl::GetProperty(
    BaseObject* object,   // A pointer to an instance of MyControl
    Property::Index index // The index of the property to retrieve
)
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
      handle.SetProperty( PROPERTY_ONE, true ); // Assume Property indices are publicly accessible

      // Get the property name
      std::cout << "Property1 name is: " << handle.GetPropertyName( PROPERTY_ONE ) << std::endl;

      // Get the property
      bool propertyOne = handle.GetProperty< bool >( PROPERTY_ONE );

      // Attempt to write a read-only property...
      handle.SetProperty( PROPERTY_TWO, 4.0f ); // !!! Will assert as PROPERTY_TWO is read-only !!!
    }
  }
}
@endcode
*
*/
