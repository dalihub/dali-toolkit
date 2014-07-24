/*! \page event-system Event Handling

Dali emits several signals to an application to inform it of user actions.

<h2 class="pg">Touch Events</h2>

An application can be notified when a user interacts with the touch screen on the device by connecting to the touch signal provided by Dali::Actor.  This signal will be emitted by Dali whenever the touch occurs within the connected actor's bounds.

Each point on the screen that is currently being touched or where touch has stopped is represented by a Dali::TouchPoint.  This object stores information about the state of the touch point (down, up, motion etc.) and the co-ordinates of the touch.

A collection of touch points at a specific moment in time is collated into a Dali::TouchEvent.  When a multi-touch event occurs, each touch point represents the points that are currently being touched or the points where touch has stopped.

The following example shows how a connection to a touch event signal can be established:

@code
bool OnTouch(Actor actor, const TouchEvent& touch)
{
  bool handled = false;

  switch(touch.GetPointCount())
  {
    case 1: // Single touch
      if (touch.GetPoint(0).GetState == TouchPoint::Down)
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
Actor actor = Actor::New();
actor.SignalTouch().Connect(&OnTouch);
@endcode

The primary touch point is the first point that the user touches.

The touch event is first emitted to the actor which is hit by the primary touch point.  If this hit actor does not handle the event, then the event is offered to the hit actor's parent.  Again, if the parent does not handle this event, it is then offered to its parent and so on until the stage is reached or the event is consumed.

If a parent and child both connect to the Touch signal, then the touch event is first offered to the child.  If it is consumed by the child, then the parent will not be informed.

<h2 class="pg">Gestures</h2>

A Dali::GestureDetector analyses a stream of touch events and attempts to determine the intention of the user.  An actor is attached to a gesture detector and if the detector recognises a pattern, it will emit a detected signal to the application.

The following gesture detectors are currently supported in Dali:

- Dali::PinchGestureDetector - When the user moves two fingers towards or away from each other.
- Dali::PanGestureDetector - When the user moves one or more fingers in the same direction.

The example below shows how an application can be notified of a pinch gesture:

@code
void OnPinch(Actor actor, PinchGesture pinch)
{
  // Scale your actor according to the pinch scale
  Vector3 newSize = actor.GetCurrentSize() * pinch.GetScale();
  actor.SetSize(newSize);
}

// Elsewhere
PinchDetector detector = PinchDetector::New();
detector.Attach(myActor);
detector.SignalDetected().Connect(&OnPinch);
@endcode

 */

// @TODO: Add "Key Events" section
