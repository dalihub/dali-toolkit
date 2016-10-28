<!--
/**-->

[TOC]

# Creating Custom UI Controls {#creating-custom-controls}

DALi provides the ability to create custom UI controls.
This can be done by extending Dali::Toolkit::Control and Dali::Toolkit::Internal::Control classes.
 
Custom controls are created using the [handle/body idiom](@ref handle-body-idiom) used in DALi.
 
![ ](../assets/img/creating-custom-controls/control-handle-body.png)
![ ](creating-custom-controls/control-handle-body.png)
 
Namespaces are important
+ The handle & body classes should have the same name but in different namespaces
+ TypeRegistry relies on this convention
+ Here our custom control requires
  + MyControl
  + Internal::MyControl
 
### General Guidelines:
+ Try to avoid adding C++ APIs as they become difficult to maintain.
  + Use **properties** as much as possible as Controls should be data driven.
  + These controls will be used through JavaScript and JSON files so need to be compatible.
+ Bear in mind that the Control can be updated when the properties change (e.g. style change)
  + Ensure control deals with these property changes gracefully
  + Not just the first time they are set
+ Use Visuals rather than creating several child Actors
  + DALi rendering pipeline more efficient
+ Accessibility actions should be considered when designing the Control.
+ Consider using signals if the application needs to be react to changes in the control state.
+ Use of Gestures should be preferred over analysing raw touch events
+ Check if you need to chain up to base class if overriding certain methods
 
___________________________________________________________________________________________________

## Rendering Content {#creating-controls-rendering-content}

To render content, the required actors can be created and added to the control itself as its children.
However, this solution is not fully optimised and means extra actors will be added, and thus, need to be processed by DALi.
 
Controls should be as generic as possible so the recommendation is to re-use visuals to create the content required as described in the [Visuals](@ref visuals) section.
Currently, this is devel-api though, so is subject to change.
 
![ ](../assets/img/creating-custom-controls/rendering.png)
![ ](creating-custom-controls/rendering.png)
 
___________________________________________________________________________________________________

## Ensuring Control is Stylable {#creating-controls-stylable}

DALi's property system allows custom controls to be easily styled.
The [JSON Syntax](@ref script-json-specification) is used in the stylesheets:
 
**JSON Styling Syntax Example:**
~~~
{
  "styles":
  {
    "textfield":
    {
      "pointSize":18,
      "primaryCursorColor":[0.0,0.72,0.9,1.0],
      "secondaryCursorColor":[0.0,0.72,0.9,1.0],
      "cursorWidth":1,
      "selectionHighlightColor":[0.75,0.96,1.0,1.0],
      "grabHandleImage" : "{DALI_STYLE_IMAGE_DIR}cursor_handler_drop_center.png",
      "selectionHandleImageLeft" : {"filename":"{DALI_STYLE_IMAGE_DIR}selection_handle_drop_left.png" },
      "selectionHandleImageRight": {"filename":"{DALI_STYLE_IMAGE_DIR}selection_handle_drop_right.png" }
    }
  }
}
~~~
 
Styling gives the UI designer the ability to change the look and feel of the control without any code changes.
 
| Normal Style | Customized Style |
|:------------:|:----------------:|
|![ ](../assets/img/creating-custom-controls/popup-normal.png) ![ ](creating-custom-controls/popup-normal.png) | ![ ](../assets/img/creating-custom-controls/popup-styled.png) ![ ](creating-custom-controls/popup-styled.png)|
 
More information regarding styling can be found in the [Styling](@ref styling) section.
___________________________________________________________________________________________________

### Type Registration {#creating-controls-type-registration}

The TypeRegistry is used to register your custom control.
This allows the creation of the control via a JSON file, as well as registering properties, signals and actions.
 
To ensure your control is stylable, the process described in [Type Registration](@ref type-registration) should be followed.
To aid development, some macros are provided for registering properties which are described in the [Property](@ref properties) section.
 
Control properties can be one of three types:
 + **Event-side only:** A function is called to set this property or to retrieve the value of this property.
                        Usually, the value is stored as a member parameter of the Impl class.
                        Other operations can also be done, as required, in this called function.
 + **Animatable Properties:** These are double-buffered properties that can be animated.
 + **Custom Properties:** These are dynamic properties that are created for every single instance of the control.
                          Therefore, these tend to take a lot of memory and are usually used by applications or other controls to dynamically set certain attributes on their children.
                          The index for these properties can also be different for every instance.
 
Careful consideration must be taken when choosing which property type to use for the properties of the custom control.
For example, an Animatable property type can be animated but requires a lot more resources (both in its execution and memory footprint) compared to an event-side only property.
___________________________________________________________________________________________________

## Control Services {#creating-controls-control-services}

### Initialization {#creating-controls-init}

Controls are initialized in two steps: in the constructor, and then in the Initialize() method.
This is so that a handle/body connection is made within DALi Core.
See Dali::CustomActor & Dali::CustomActorImpl for more information.
 
It is recommended to do provide a New() method in the custom control implementation where the Initialize() method should be called.

~~~{.cpp}
// C++
MyUIControl MyUIControlImpl::New()
{
  // Create the implementation, temporarily owned on stack
  IntrusivePtr< MyUIControlImpl > controlImpl = new MyUIControlImpl;

  // Pass ownership to handle
  MyUIControl handle( *controlImpl );

  // Second-phase init of the implementation
  controlImpl->Initialize();

  return handle;
}
~~~
Another advantage of using a New() method is that the constructor for MyUIControl can be made private (or protected).
 
This will trigger the Dali::Toolkit::Internal::Control Initialize() method which will in-turn, call the virtual method OnInitialize().
This should be overridden by the custom ui control.
~~~{.cpp}
// C++
void MyUIControlImpl::OnInitialize()
{
  // Create visuals using the VisualFactory, register events etc.
  // Register any created visuals with Control base class
}
~~~
___________________________________________________________________________________________________

### Control Behaviour {#creating-controls-behaviour}

Dali::Toolkit::Internal::Control provides several behaviours which are specified through its constructor (@ref Dali::Toolkit::Internal::Control::Control()).
 
| Behaviour                            | Description                                                                                                    |
|--------------------------------------|----------------------------------------------------------------------------------------------------------------|
| CONTROL_BEHAVIOUR_DEFAULT              | Default behavior (size negotiation is on, style change is monitored, event callbacks are not called.                                      |
| DISABLE_SIZE_NEGOTIATION             | If our control does not need size negotiation, i.e. control will be skipped by the size negotiation algorithm. |
| REQUIRES_HOVER_EVENTS                | If our control requires [hover events](@ref creating-controls-events).                                         |
| REQUIRES_WHEEL_EVENTS                | If our control requires [wheel events](@ref creating-controls-events).                                         |
| DISABLE_STYLE_CHANGE_SIGNALS         | True if control should not monitor style change signals such as Theme/Font change.                                         |
| REQUIRES_KEYBOARD_NAVIGATION_SUPPORT | True if need to support keyboard navigation.                                                                   |
___________________________________________________________________________________________________

### Touch, Hover & Wheel Events {#creating-controls-events}

+ A **touch** is when any touch occurs within the bounds of the custom actor. Connect to Dali::Actor::TouchSignal().
+ A **hover event** is when a pointer moves within the bounds of a custom actor (e.g. mouse pointer or hover pointer).
+ A **wheel event** is when the mouse wheel (or similar) is moved while hovering over an actor (via a mouse pointer or hover pointer).
 
If the control needs to utilize hover and wheel events, then the correct behaviour flag should be used when constructing the control and then the appropriate method should be overridden.
~~~{.cpp}
// C++
bool MyUIControlImpl::OnHoverEvent( const HoverEvent& event )
{
  bool consumed = false;

  // Handle hover event

  // Return true if handled/consumed, false otherwise
  return consumed;
}
~~~
~~~{.cpp}
// C++
bool MyUIControlImpl::OnWheelEvent( const WheelEvent& event )
{
  bool consumed = false;

  // Handle wheel event

  // Return true if handled/consumed, false otherwise
  return consumed;
}
~~~
___________________________________________________________________________________________________

### Gestures {#creating-controls-gestures}

DALi has a gesture system which analyses a stream of touch events and attempts to determine the intention of the user.
The following gesture detectors are provided:
 
 + **Pan:** When the user starts panning (or dragging) one or more fingers.
            The panning should start from within the bounds of the control.
 + **Pinch:** Detects when two touch points move towards or away from each other.
              The center point of the pinch should be within the bounds of the control.
 + **Tap:** When the user taps within the bounds of the control.
 + **LongPress:** When the user presses and holds on a certain point within the bounds of a control.
 
The control base class provides basic set up to detect these gestures.
If any of these detectors are required then this can be specified in the OnInitialize() method (or as required).
 
~~~{.cpp}
// C++
void MyUIControlImpl::OnInitialize()
{
  // Only enable pan gesture detection
  EnableGestureDetection( Gesture::Pan );

  // Or if several gestures are required
  EnableGestureDetection( Gesture::Type( Gesture::Pinch | Gesture::Tap | Gesture::LongPress ) );
}
~~~
 
The above snippet of code will only enable the default gesture detection for each type.
If customization of the gesture detection is required, then the gesture-detector can be retrieved and set up accordingly in the same method.
 
~~~{.cpp}
// C++
PanGestureDetector panGestureDetector = GetPanGestureDetector();
panGestureDetector.AddDirection( PanGestureDetector::DIRECTION_VERTICAL );
~~~
 
Finally, the appropriate method should be overridden:
~~~{.cpp}
// C++
void MyUIControlImpl::OnPan( const PanGesture& pan )
{
  // Handle pan-gesture
}
~~~
~~~{.cpp}
// C++
void MyUIControlImpl::OnPinch( const PinchGesture& pinch )
{
  // Handle pinch-event
}
~~~
~~~{.cpp}
// C++
void MyUIControlImpl::OnTap( const TapGesture& tap )
{
  // Handle tap-gesture
}
~~~
~~~{.cpp}
// C++
void MyUIControlImpl::OnLongPress( const LongPressGesture& longPress )
{
  // Handle long-press-gesture
}
~~~
 
___________________________________________________________________________________________________

### Accessibility {#creating-controls-accessibility}

Accessibility is functionality that has been designed to aid usage by the visually impaired.
More information can be found in the [Accessibility](@ref accessibility) section.
 
Accessibility behaviour can be customized in the control by overriding certain virtual methods.
This is detailed [here](@ref accessibilitycustomcontrol).
 
___________________________________________________________________________________________________

### Signals {#creating-controls-signals}

If applications need to react to changes in the control state, controls can inform those applications using Dali::Signal.

First, create a signature of the function the signal will call in the handle header file:
~~~{.cpp}
// C++: my-ui-control.h
typedef Signal< void () > SignalType;
~~~
 
Then Create methods to get to the signal:
~~~{.cpp}
// C++: my-ui-control.h
MyUIControl::SignalType& MyCustomSignal();
~~~

The source file should just call the impl:
~~~{.cpp}
// C++: my-ui-control.cpp
MyUIControl::SignalType& MyUIControl::MyCustomSignal()
{
  return Dali::Toolkit::GetImplementation( *this ).MyCustomSignal();
}
~~~
 
In the impl file, create an instance of the signal as follows and return it in the appropriate method:
~~~{.cpp}
// C++: my-ui-control-impl.h
public:

  MyUIControl::SignalType MyUIControl::MyCustomSignal()
  {
    return mMyCustomSignal;
  }

private:

  MyUIControl::SignalType mMyCustomSignal;
~~~
 
Then, when you wish to emit this signal:
~~~{.cpp}
// C++: my-ui-control-impl.cpp
mMyCustomSignal.Emit();
~~~
There is no need to check if there is anything connected to this signal as this is done by the framework.
 
The application can then connect to the signal as follows:
~~~{.cpp}
void AppFunction()
{
  // Do Something
}

...

customControl.MyCustomSignal.Connect( this, &AppFunction );
~~~
 
___________________________________________________________________________________________________

### Children Added/Removed {#creating-controls-children}

Methods are provided that can be overridden if notification is required when a child is added or removed from our control.
An up call to the Control class is necessary if these methods are overridden.
 
~~~{.cpp}
// C++
void MyUIControlImpl::OnChildAdd( Actor& child );
{
  // Up call to Control first
  Control::OnChildAdd( child );

  // Do any other operations required upon child addition
}
~~~
~~~{.cpp}
// C++
void MyUIControlImpl::OnChildRemove( Actor& child );
{
  // Do any other operations required upon child removal

  // Up call to Control at the end
  Control::OnChildRemove( child );
}
~~~
 
Avoid adding or removing the child again within these methods.
 
___________________________________________________________________________________________________

### Stage Connection {#creating-controls-stage}

Methods are provided that can be overridden if notification is required when our control is connected to or disconnected from the stage.
An up call to the Control class is necessary if these methods are overridden.
 
~~~{.cpp}
// C++
void MyUIControlImpl::OnStageConnection( int depth )
{
  // Up call to Control first
  Control::OnStageConnection( depth );

  // Do any other operations required upon stage connection
}
~~~
~~~{.cpp}
// C++
void MyUIControlImpl::OnStageDisconnection()
{
  // Do any other operations required upon stage disconnection

  // Up call to Control at the end
  Control::OnStageDisconnection();
}
~~~
 
___________________________________________________________________________________________________

### Size Negotiation {#creating-controls-size-negotiation}

The following methods must be overridden for size negotiation to work correctly with a custom control.
 
~~~{.cpp}
// C++
Vector3 MyUIControlImpl::GetNaturalSize()
{
  // Return the natural size of the control
  // This depends on our layout
  // If we have one visual, then we can return the natural size of that
  // If we have more visuals, then we need to calculate their positions within our control and work out the overall size we would like our control to be

  // After working out the natural size of visuals that belong to this control,
  // should also chain up to ensure other visuals belonging to the base class are
  // also taken into account:
  Vector2 baseSize = Control::GetNaturalSize(); // returns the size of the background.
}
~~~
~~~{.cpp}
// C++
float MyUIControlImpl::GetHeightForWidth( float width )
{
  // Called by the size negotiation algorithm if we have a fixed width
  // We should calculate the height we would like our control to be for that width

  // Should also chain up to determine the base class's preferred height:
  float baseHeight = Control::GetHeightForWidth( width );

}
~~~
~~~{.cpp}
// C++
float MyUIControlImpl::GetWidthForHeight( float height )
{
  // Called by the size negotiation algorithm if we have a fixed height
  // We should calculate the width we would like our control to be for that height

  // Should also chain up to determine the base class's preferred width:
  float baseWidth = Control::GetWidth( height );
}
~~~
~~~{.cpp}
// C++
void MyUIControlImpl::OnRelayout( const Vector2& size, RelayoutContainer& container )
{
  // The size is what we have been given and what our control needs to fit into
  // Here, we need to set the position and the size of our visuals
  // If we have other controls/actors as children
  //  - Add the control/actor to the container paired with the size required
  //  - To ensure this works, you need to set up the control with a relayout policy of USE_ASSIGNED_SIZE
  //  - DO NOT CALL SetSize on this control: This will trigger another size negotiation calculation
  // DO NOT chain up to base class.
}
~~~
More information on size negotiation can be found [here](@ref size-negotiation-controls).
 
___________________________________________________________________________________________________

### Other Features {#creating-controls-other}

 + [Background](@ref background)
 
___________________________________________________________________________________________________

@class _Guide_Creating_UI_Controls

*/
