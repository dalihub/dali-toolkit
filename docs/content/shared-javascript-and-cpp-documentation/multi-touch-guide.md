/**
 *

Multi-Touch Events
==================

Touch events are received via signals.

For C++ API see Dali::Actor::TouchedSignal() and Dali::Actor::HoveredSignal() for more details.

For JavaScript use actor.connect( "touched", myCallback ) and actor.connect("hovered", myCallback );

### Hit Testing Rules Summary:

 - An actor is only hittable if the actor's touch signal has a connection.
 - An actor is only hittable when it is between the camera's near and far planes.
 - If an actor is made insensitive, then the actor and its children are not hittable; see Dali::Actor::IsSensitive()
 - If an actor's visibility flag is unset, then none of its children are hittable either; see Dali::Actor::IsVisible()
 - To be hittable, an actor must have a non-zero size.
 - If an actor's world color is fully transparent, then it is not hittable; see GetCurrentWorldColor()

### Hit Test Algorithm:

 - RenderTasks
   - Hit testing is dependent on the camera used, which is specific to each RenderTask.

 - Layers
   - For each RenderTask, hit testing starts from the top-most layer and we go through all the
     layers until we have a hit or there are none left.
   - Before we perform a hit test within a layer, we check if all the layer's parents are visible
     and sensitive.
   - If they are not, we skip hit testing the actors in that layer altogether.
   - If a layer is set to consume all touch, then we do not check any layers behind this layer.

 - Actors
   - The final part of hit testing is performed by walking through the actor tree within a layer.
   - The following pseudocode shows the algorithm used:


~~~
 HIT-TEST-WITHIN-LAYER( ACTOR )
 {
   // Only hit-test the actor and its children if it is sensitive and visible
   IF ( ACTOR-IS-SENSITIVE &&
           ACTOR-IS-VISIBLE )
      {
         // Depth-first traversal within current layer, visiting parent first

         // Check whether current actor should be hit-tested
         IF ( TOUCH-SIGNAL-NOT-EMPTY &&
             ACTOR-HAS-NON-ZERO-SIZE &&
             ACTOR-WORLD-COLOR-IS-NOT-TRANSPARENT )
         {
           // Hit-test current actor
           IF ( ACTOR-HIT )
           {
               IF ( ACTOR-IS-OVERLAY || ( DISTANCE-TO-ACTOR < DISTANCE-TO-LAST-HIT-ACTOR ) )
               {
                 // The current actor is the closest actor that was underneath the touch
                 LAST-HIT-ACTOR = CURRENT-ACTOR
               }
           }
         }

       // Keep checking children, in case we hit something closer
        FOR-EACH CHILD (in order)
       {
         IF ( CHILD-IS-NOT-A-LAYER )
         {
             // Continue traversal for this child's sub-tree
             HIT-TEST-WITHIN-LAYER ( CHILD )
         }
          // else we skip hit-testing the child's sub-tree altogether
       }
     }
   }
~~~
 - Overlays always take priority (i.e. they're considered closer) regardless of distance.
     The overlay children take priority over their parents, and overlay siblings take priority
     over their previous siblings (i.e. reverse of rendering order):

~~~
      1
     / \
    /   \
   2     5
  / \     \
 /   \     \
3     4     6

Hit Priority of above Actor tree (all overlays): 1 - Lowest. 6 - Highest.
~~~

 - Stencil Actors can be used to influence the result of hits within a layer.
     If a Stencil Actor exists on a layer and that Actor is marked visible then a successful
     hit can only take place in the area that the stencil Actor marks as visible.
     The hit can be in any Stencil Actor in that layer, but must be in the region of one of them.
     Stencil Actor inheritance behaves as with rendering in that any child of a Stencil Actor will
     also be considered a Stencil Actor.

 <i>Touch Event Delivery:</i>

 - Delivery
   - The hit actor's touch signal is emitted first; if it is not consumed by any of the listeners,
     the parent's touch signal is emitted, and so on.
   - If there are several touch points, then the delivery is only to the first touch point's hit
     actor (and its parents).  There will be NO touch signal delivery for the hit actors of the
     other touch points.
   - The local coordinates are from the top-left (0.0f, 0.0f, 0.5f) of the hit actor.
   - The following pseudocode shows the delivery mechanism:

~~~
  EMIT-TOUCH-SIGNAL( ACTOR )
  {
    IF ( TOUCH-SIGNAL-NOT-EMPTY )
    {
      // Only do the emission if touch signal of actor has connections.
        CONSUMED = TOUCHED-SIGNAL( TOUCH-EVENT )
    }

    IF ( NOT-CONSUMED )
    {
        // If event is not consumed then deliver it to the parent unless we reach the root actor
        IF ( ACTOR-PARENT )
        {
          EMIT-TOUCH-SIGNAL( ACTOR-PARENT )
        }
    }
  }
~~~
 - Leave State
   - A "Leave" state is set when the first point exits the bounds of the previous first point's
     hit actor (primary hit actor).
   - When this happens, the last primary hit actor's touch signal is emitted with a "Leave" state
     (only if it requires leave signals); see the actor property leaveRequired.


 - Interrupted State
   - If a system event occurs which interrupts the touch processing, then the last primary hit
     actor's touch signals are emitted with an "Interrupted" state.
   - If the last primary hit actor, or one of its parents, is no longer touchable, then its
     touch signals are also emitted with an "Interrupted" state.
   - If the consumed actor on touch-down is not the same as the consumed actor on touch-up, then
     touch signals are also emitted from the touch-down actor with an "Interrupted" state.

@class MultiTouch
*
*/
