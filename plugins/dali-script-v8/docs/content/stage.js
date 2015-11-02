/**
## Stage API

The Stage is a top-level object used for displaying a tree of Actors.

 - Stage is a static object used by accessing the dali.stage object.
 - Stage provides a top-level "root" actor.
 - Stage provides Key Events to the application
 - Stage has a background color you can set, see  {{#crossLink "stage/setBackgroundColor:method"}}{{/crossLink}}
  
When an actor is not required, remove it from the stage to save memory and help performance.
```
dali.stage.setBackgroundColor( dali.COLOR_WHITE);
  
var stageSize = dali.stage.getSize();
  
var imageActor = new dali.ImageActor( );
  
dali.stage.add( imageActor );
  
// when the actor is no longer required
dali.stage.remove( imageActor );
```

![ ](../assets/img/stage.png)


### Key Events

Key events are received through the keyEvent callback.

```
daliApp.myCallback = function (keyEvent)
{
  switch (keyEvent.keyDescription) {
          case "Up": log("up");
              break;
  
          case "Down": log("down");
              break;
  
          case "Left": log("left");
              break;
  
          case "Right": log("right");
              break;
  
  }
}

dali.stage.connect("keyEvent", daliApp.myCallback);
```

The key event object has the following properties
```
  KeyEvent = {
    state,             // boolean  = "down" or "up" key state
    shiftPressed,      // boolean, shift key is held
    ctrlPressed,       // boolean, ctrl key is held
    altPressed,        // boolean, alt key is held
    keyModifiers,      // key modifiers
    keyCode,           // key code
    keyDescription,    // key description
    timestamp          // The time in milli-seconds when the event occured
  }
```
 @class Stage
*/
