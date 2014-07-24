/*! \page constraints-intro Constraints
 *

<h2 class="pg">Introduction</h2>

Constraints can be used to modify the property of an actor, based on the properties of another actor.  Custom a functions or functors can be supplied, to describe the relationship between these properties.  A common example is alignment e.g. modifying an actor's position, when the size of the parent actor changes.  Multiple constraints can be applied to the same actor at the same time.

Constraints are applied in the dedicated render thread, after animations have been applied.  This means that Constraints override the values set by Animations.  Constraints may behave in a similar way to animations, since they can be applied or removed gradually.  By default the apply-time is zero, meaning that the constraint will be applied immediately.

<h2 class="pg">Local Constraints</h2>

A local constraint is based on the local properties (i.e. size, position, scale, rotation, color) of an actor.  For example you could change the color of an actor, based its rotation.

<h2 class="pg">Parent Constraints</h2>

A parent constraint is based on properties of the actor's parent.  The following example shows how to constrain an actor to be 80% of its parent's size:

@code
Actor actor = Actor::New();
Vector3 scale(0.8f, 0.8f, 0.8f); // Set width/height/depth at 80% of parent
Constraint constraint = ParentConstraint::Size::New(Dali::ParentSize(scale));
actor.ApplyConstraint(constraint);
@endcode

The actor's constraints can later be removed:

@code
  actor.RemoveConstraints();
@endcode

*
*/
