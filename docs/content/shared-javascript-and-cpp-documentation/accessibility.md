<!--
/**-->

[TOC]

# Accessibility{#accessibility}


## Introduction - What is Accessibility? {#accessibilityintroduction}

Accessibility describes functionality designed to aid usage by the visually impaired.
  
This includes:
- Reading out selections or other on-screen items via text-to-speech.
- Item selection being controlled with gestures to aid selecting other small hard to select entities.
  

## Accessibility within DALi {#accessibilitydali}

DALi will pick up the system's current accessibility state (and subsequent changes to it) and enable its internal accessibility mode based on this.
  
DALi includes an Accessibility Manager which provides public API control of the order of object selection by gesture, and text to be read out per actor or control.
  
It further provides many signals that represent accessibility gestures. These gestures can range from a simple actor selection through to a more control-specific concept like "page-up", which an application developer may want to provide an implementation for.
  
Furthermore, these APIs can be used not only with existing actors and controls, but also when creating a custom control.
  
The AccessibilityManager lives within DALi Toolkit. Please see accessibility-manager.h for the full API.


## Accessibility Focus {#accessibilityfocus}

<!-- Float image to the right of the text -->
<div style="float: right">
    ![ ](../assets/img/accessibility/accessibility-focus.png) ![ ](./accessibility-focus.png)
</div>

Visibly, when enabled, accessibility will typically show an actor (or actors) as focused. This is represented by default with yellow rectangular frame around the actor. See this section for [modifying the appearance of the accessibility focus](#accessibilityfocusappearance).
  
Once in accessibility mode, normal control is disabled and accessibility gestures must be used to access content.
DALi actors and controls will no longer receive tap gestures or click events when they are touched once (as they normally would).
  
Note: The accessibility focus is also referred to as the Focus Indicator.


### Moving the focus with gestures {#accessibilitygestures}

Accessibility recognizes many gesture types to move the accessibility focus from actor to actor.
  
Note:
  
- Some of these gestures have pre-defined, automatic behaviour.
- Some gestures require an specific implementation to be added to use.
- All can be caught as signals if extra control is needed.
  
To test (and understand) this behaviour, you can use the Tizen adaptor which uses the following gestures to perform basic operation:
  
Note: The gestures that perform these actions are platform specific. These are the gestures implemented in the Tizen adaptor for example only.
  
- Swiping right and left will move the focus forward and backward one item.
- Doing a left or right swipe-return (where a direction is swiped forwards and backwards quickly) will move to the previous or next page - DALi cannot know what a page is within your application so these gestures can only work if implemented manually.
  

![ ](../assets/img/accessibility/accessibility-focus-order.png) ![ ](./accessibility-focus-order.png)

### Activation {#accessibilityactivation}

Activation describes an operation performed on a selected actor, typically an on-tap or on-click event.
  
Activating an actor in accessibility mode will call a virtual function, as well as signal, for that actor.
Depending on the platform this can be triggered in different ways.
When activated, the built in actor types (like PushButton) will do the equivalent of a tap.
  
Custom-built actor types will need to implement activation in order to perform a specific behaviour. See the [Custom Controls](#accessibilitycustomcontrol) section.
  
Therefore, to tap an actor (EG. Click a button) in accessibility mode, the following must be done:
  
- The actor needs to be selected first (using gestures specific to the platform).
- Then activated, by using the activation gesture.
  
## Scrolling {#accessibilityscrolling}

Scrolling around a view outside of accessibility is normally performed by simply holding one finger and dragging (in the appropriate direction).
Within accessibility this can be overridden and performed with a different gesture type to achieve the same effect.
  
Example: For the Tizen platform scrolling is performed with a two-finger drag.


## Basic functionality {#accessibilitybasicfunctionality}

### Using the Accessibility Manager functionality {#accessibilityfunctionality}

Accessibility information is stored within the accessibility manager itself rather than within actors.
This allows the manager to have a global view of focusable actors and their order.
  
The Accessibility Manager is a singleton (owned by the singleton service) and can be accessed via its static Get() method:
  
~~~{.cpp}
// Get the accessibility manager singleton.
accessibilityManager accessibilityManager = AccessibilityManager::Get();
~~~


### Controlling where the focus will move {#accessibilitymovingfocus}

In order to provide automatic focus movement, the accessibility manager must be told the focus order of any actors to be selected.
This order is a linear order. It can move forwards or backwards only (there is no concept of "up" or "down").
  
The order of a particular actor can be set with a call to the accessibility manager like so:
  
~~~{.cpp}
// 6 is an int representing this actor's position in the focus chain.
accessibilityManager.SetFocusOrder( actor, 6 );
~~~
  
The focus order of each actor in the focus chain is unique. If there is another actor assigned with the same focus order already, the new actor will be inserted to the focus chain with that focus order, and the focus order of the original actor and all the actors followed in the focus chain will be increased accordingly.
  
If the focus order assigned to the actor is 0, it means that actor's focus order is undefined (e.g. the actor has a description but with no focus order being set yet) and therefore that actor is not focusable.
  
Moving focus to a particular actor directly can be done with SetCurrentFocusActor like so:
  
~~~{.cpp}
// Move focus to the first item on our applications page.
AccessibilityManager accessibilityManager = AccessibilityManager::Get();
  
accessibilityManager.SetCurrentFocusActor( table.GetChildAt( 0 ) );
~~~

### Modifying the appearance of the accessibility focus {#accessibilityfocusappearance}

The focus graphic itself can be customized.
It can be an image (EG. A nine-patch border) or any other type of actor.
  
It can be set using this method within C++:
  
~~~{.cpp}
accessibilityManager.SetFocusIndicatorActor( myCustomIndicatorActor );
~~~


### Using activation {#accessibilityusingactivation}

If the application would like to perform specific behaviour when an entity is activated, it can catch the activation by connecting to a signal like this:
  
~~~{.cpp}
AccessibilityManager::Get().FocusedActorActivatedSignal().Connect( this, &MyClass::OnFocusedActorActivated );
~~~
  
Controlling the activation behaviour within a custom control is covered in the [custom control section](#accessibilitycustomcontrol)


## Focus groups {#accessibilityfocusgroups}

<!-- Float image to the right of the text -->
<div style="float: right">
    ![ ](../assets/img/accessibility/accessibility-focus-group.png) ![ ](./accessibility-focus-group.png)
</div>

Group mode allows the limiting of focusable actors.
  
Example: If a popup appears, you may want the focus to be limited to only the OK and Cancel buttons. You can do this by setting the popup as a focus group and turning on group mode, the focus will be limited.
  
~~~{.cpp}
// Create a parent actor and add two children to it.
Actor groupActor = Actor::New();

Actor child1 = Actor::New();
groupActor.Add( child1 );

Actor child2 = Actor::New();
groupActor.Add( child2 );

AccessibilityManager accessibilityManager = AccessibilityManager::Get();

// Mark the parent as a focus group. Now focus movement *can* be limited to the children of this actor.
// Note: That this is not enabled until specified.
accessibilityManager.SetFocusGroup( groupActor, true );

// Enable the focus group mode.
accessibilityManager.SetGroupMode( true );
~~~


### Wrap mode {#accessibilitywrapmode}

Wrap mode allows the focus to wrap back to the beginning once the end is reached.
  
In group mode this will move to the beginning of the current focus group.
  
~~~{.cpp}
AccessibilityManager accessibilityManager = AccessibilityManager::Get();

// Enable wrap mode.
accessibilityManager.SetWrapMode( true );
~~~


## Using Accessibility {#accessibilityusage}

### Using accessibility with existing actors {#accessibilityactors}

This example sets up a 3 by 3 grid of actors with the following accessibility functionality:
  
  - They have a focus order that moves from top left to bottom right (when using the accessibility next and previous gestures).
  - They contain text that will be spoken out loud (via text-to-speech) when the focus changes.
  
Note that all the above is set via the AccessibilityManager and not as properties within the actors.
  
The text spoken per tile will be the LABEL, TRAIT and HINT (in that order).
  
~~~{.cpp}
Toolkit::TableView table = Toolkit::TableView::New( 3, 3 );
int tileNumber = 0;
for( int row = 0; row < 3; ++row )
{
  for( int column = 0; column < 3; ++column )
  {
    // Create a solid color actor, with some text.
    Actor tile = Toolkit::CreateSolidColorActor( Vector4( 1.0f, 1.0f, 0.0f, 1.0f ) );
    Toolkit::TextLabel text = Toolkit::TextLabel::New( tileNames[tileNumber] );
    tile.Add( text );

    // Get the accessibility manager singleton.
    accessibilityManager accessibilityManager = AccessibilityManager::Get();

    // Set the focus order of this actor.
    accessibilityManager.SetFocusOrder( tile, tileNumber );

    // Set up the accessibility information for this actor (this will be read out with text-to-speech).
    accessibilityManager.SetAccessibilityAttribute( tile, Dali::Toolkit::AccessibilityManager::ACCESSIBILITY_LABEL, tileNames[tileNumber] );
    accessibilityManager.SetAccessibilityAttribute( tile, Dali::Toolkit::AccessibilityManager::ACCESSIBILITY_TRAIT, "Tile" );
    accessibilityManager.SetAccessibilityAttribute( tile, Dali::Toolkit::AccessibilityManager::ACCESSIBILITY_HINT, "You can run this example");

    // Lay out our actor within the table view.
    table.AddChild( tile, Toolkit::TableView::CellPosition( row, column ) );

    tileNumber++;
  }
}
Stage::GetCurrent().Add( table );
~~~

### Using accessibility within a custom control (C++) {#accessibilitycustomcontrol}

Accessibility behaviour can be customized in a custom UI control by overriding all or some of the following methods.

| Method                     | Description                                                                                                                                                                |
|----------------------------|----------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| OnAccessibilityActivated   | When the control is *activated* or selected, in accessibility mode.                                                                                                        |
| OnAccessibilityPan         | When an accessibility pan gesture occurs while this control is focused.                                                                                                   |
| OnAccessibilityTouch       | Touch events are delivered differently in Accessibility mode. This method should be overridden if some special behaviour is required when these touch events are received. |
| OnAccessibilityValueChange | When a value is changed while this control is focused (e.g. value change of a slider control).                                                                            |
| OnAccessibilityZoom        | Should be overridden if behaviour is required when the magnification level changes when this control is focused.                                                          |
 
If these events are consumed, then the method should return true.
The default behaviour in the control base classes returns false, i.e. not consumed.
 
### Using accessibility signals for extra control {#accessibilitysignals}

For more specific control of functionality when accessibility is enabled, there are several signals within the accessibility manager's public API that can be connected to.
  
The main categories of signals are:
  
- The signal when the accessibility status is detected as being toggled on or off: StatusChangedSignal()
- Focus changes can cause FocusChangedSignal() and FocusOvershotSignal(). These can be connected to in order to provide custom actions when the focus is moved around the screen.
- The activated signal when an actor has been activated (typically by a focus, then double tap): ActionActivateSignal()
- Gesture received signals: There are many of these. They are each linked to the many accessibility gestures that can be received by the system.
  
Please see accessibility-manager.h within DALi Toolkit for the full API.


@class _Guide_Accessibility
*/
