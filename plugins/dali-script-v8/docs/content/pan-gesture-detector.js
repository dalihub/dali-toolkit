/**
 *
## PanGestureDetector API


PanGestureDetector analyse a stream of touch events and looks for panning (or dragging) gestures.
If an actor is attached to the pan gesture detector, the detector will emit a detected signal to
the application when it recognises a pan gesture on the attached actor in its analysis.

### Simple example

```
// Create a pan gesture detector
var panGestureDetector = new dali.PanGestureDetector();

var actor = new dali.Actor();
dali.stage.add(actor);

// Attach an actor to the detector
panGestureDetector.attach(actor);

// Connect the detected signal
panGestureDetector.on("panDetected", onPan);

onPan = function(actor, panGesture)
{
    log("Pan gesture state: " + panGesture.state + ", number of touches: " + panGesture.numberOfTouches + ", time stamp: " + panGesture.time + "\n");
    log("local position: " + panGesture.position.x + ", " + panGesture.position.y + "\n");
    log("local displacement: " + panGesture.displacement.x + ", " + panGesture.displacement.y + "\n");
    log("local velocity: " + panGesture.velocity.x + ", " + panGesture.velocity.y + "\n");
    log("screen position: " + panGesture.screenPosition.x + ", " + panGesture.screenPosition.y + "\n");
    log("screen displacement: " + panGesture.screenDisplacement.x + ", " + panGesture.screenDisplacement.y + "\n");
    log("screen velocity: " + panGesture.screenVelocity.x + ", " + panGesture.screenVelocity.y + "\n");
}

// Detach the actor from the detector
panGestureDetector.detach(actor);
```
@class PanGestureDetector

*/
