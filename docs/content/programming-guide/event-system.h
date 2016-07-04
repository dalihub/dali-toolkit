/*! \page event-system Event Handling

DALi emits several signals to an application to inform it of user actions.

<h2 class="pg">Touch</h2>

An application can be notified when a user interacts with the touch screen on the device by connecting to the touch signal provided by Dali::Actor.
This signal will be emitted by DALi whenever the touch occurs within the connected actor's bounds.

Each point on the screen that is currently being touched or where touch has stopped is represented by a point.
Dali::TouchData stores information about the state of each point (down, up, motion etc.) and the co-ordinates of the touch.

When a multi-touch event occurs, each point represents the points that are currently being touched or the points where touch has stopped.

The following example shows how a connection to a touch event signal can be established:

@code
bool OnTouch( Dali::Actor actor, const Dali::TouchData& touch )
{
  bool handled = false;

  switch( touch.GetPointCount() )
  {
    case 1: // Single touch
      if ( touch.GetState( 0 ) == Dali::PointState::DOWN )
      {
        // Do action when first touches the touch screen.
        ...
        handled = true;
      }
      ...
      break;

    case 2: // Multi-touch event
      ...
      break;
    ...
  }

  return handled;  // true if we have handled the touch, false otherwise
}

// Elsewhere
Dali::Actor actor = Actor::New();
actor.TouchSignal().Connect( &OnTouch );
@endcode

The primary touch point is the first point that the user touches.

The touch signal is first emitted to the actor which is hit by the primary touch point.
If this hit actor does not handle (consume) the event, then the event is offered to the hit actor's parent.
Again, if the parent does not handle this event, it is then offered to its parent and so on until the stage is reached or the event is consumed.

If the TouchSignal of both a parent and child are connected to, then the touch event is first offered to the child's listener.
If it is consumed by the child's listener, then the parent will not be informed.

<h2 class="pg">Gestures</h2>

A Dali::GestureDetector analyses a stream of touch input and attempts to determine the intention of the user.
An actor is attached to a gesture detector and if the detector recognises a pattern, it will emit a detected signal to the application.

The following gesture detectors are currently supported in DALi:

- Dali::LongPressGestureDetector - When the user presses and holds a particular point on the screen for a specified length of time.
- Dali::PinchGestureDetector - When the user moves two fingers towards or away from each other.
- Dali::PanGestureDetector - When the user moves one or more fingers in the same direction.
- Dali::TapGestureDetector - When the user taps the screen.

The example below shows how an application can be notified of a pinch gesture:

@code
void OnPinch( Dali::Actor actor, const Dali::PinchGesture& pinch )
{
  // Scale your actor according to the pinch scale
  Vector3 newSize = actor.GetCurrentSize() * pinch.scale;
  actor.SetSize(newSize);
}

// Elsewhere
Dali::PinchGestureDetector detector = Dali::PinchDetector::New();
detector.Attach( myActor );
detector.DetectedSignal().Connect( &OnPinch );
@endcode

*/
