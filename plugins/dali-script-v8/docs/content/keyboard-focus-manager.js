/**
 *

## Keyboard Focus Manager API

Keyboard focus manager controls the keyboard focus flow.

It also allows you to set an actor that is used to high light the focused actor.

{{#crossLink "KeyboardFocusManager/setFocusIndicatorActor:method"}}{{/crossLink}}

The application is required to help the manager when moving focus.

![ Focus Manager ](../assets/img/focus-manager/focus-manager.png)

### keyboard-pre-focus-change

Connect to the pre-focus-change call back as follows:
```
// listen for pre-focus change events
dali.keyboardFocusManager.connect("keyboardPreFocusChange", this.preFocusChanged);
  
// example call back handler
  
// currentFocusedActor =  currently focused actor
// proposed = keyboard focus managers guess at what actor should be next
// direction = the direction of the focus is moving in
//
myApp.preFocusChanged = function( currentFocusedActor, proposedActorToFocus, direction)
{
  
  if (direction == "up" )
  {
    return actor above current actor;
    }
  if (direction == "right" )
  {
    return actor to the right of current actor;
  }
}
  
dali.keyboardFocusManager.connect("keyboardPreFocusChange", myCallback)
```

KeyboardFocusManager makes the best guess for which actor to focus towards the given direction, but applications might want to change that.

By connecting with this signal, they can check the proposed actor to focus and return a different actor if they wish.

This signal is only emitted when the navigation key is pressed and KeyboardFocusManager tries to move the focus automatically.

It won't be emitted for focus movement by calling setCurrentFocusActor directly.

### keyboard-focus-changed

This signal is emitted after the current focused actor has been changed.
```
myCallback( originalFocusedActor, currentFocusedActor)
{
}
  
dali.keyboardFocusManager.connect("keyboardFocusChange", myCallback)
```

@class KeyboardFocusManager

 */
