/*! \page constraints Constraints
 *

<h1 class="pg">Introduction</h1>

Constraints are used to modify the property of an actor, based on other properties of the same actor; properties of the actor's parent; or properties of another actor altogether, when the modification needs to be at run-time.
Custom functions or functors can be supplied, where the desired value of the property can be calculated.
These functions (or functors) are called in every frame so should be fast and not too complex otherwise it will hit performance.

Multiple constraints can be applied to the same actor at the same time.
The order in which constraints are applied is important as this is the order in which they are processed in the update thread.

Constraints are applied after animations have been applied.
This means that Constraints override the values set by Animations.

Not all properties can be used as a constraint input, please see Dali::Handle::IsPropertyAConstraintInput() for more details.

<h1 class="pg">When to use a Constraint</h1>

Constraints are designed as a way of modifying properties that cannot be modified by any existing built in functionality; Like Animations, Size negotiation or Parent anchor, origin settings.
As they provide the ability for the application developer to execute their own code within the update thread, DALi can no-longer guarantee the timeliness of this code, or how optimised it may be.

Generally, you should not use constraints with the SIZE property as constraining the size and size negotiation are mutually exclusive.
Consider the following use cases as an example of when and when not to use a constraint:

<table>
  <tr>
    <td><b>Requirement:</b></td>
    <td><b>Desired Solution:</b></td>
  </tr>
  <tr>
    <td>Need a child to be 50% the size of it's parent.</td>
    <td>Use Size negotiation.</td>
  </tr>
  <tr>
    <td>Need to zoom an actor in to the screen via it's scale property.</td>
    <td>Use an Animation.</td>
  </tr>
  <tr>
    <td>Need an actor to appear centered around the bottom-right corner of it's parent.</td>
    <td>Use ParentOrigin & AnchorPoint.</td>
  </tr>
  <tr>
    <td>Need to lay out a series of controls with various alignment requirements.</td>
    <td>Use either Anchor & origin settings, or a TableView.</td>
  </tr>
  <tr>
    <td>Need to automatically modify the position property of one actor based on the position property of another actor, that is neither a parent OR a child.</td>
    <td>Use a Constraint.</td>
  </tr>
  <tr>
    <td>Need to position an actor relative to it's parent actor in a NON-UNIFORM way, IE. a non-linear calculation needs to be performed that requires a functor.</td>
    <td>Use a Constraint.</td>
  </tr>
  <tr>
    <td>Need to modify an actor's property in real time based on some calculations that require additional data to be stored in-between frames.</td>
    <td>Use a Constraint. The constraint functor can hold any variables within it that need to be preserved frame-to-frame.</td>
  </tr>
</table>

For most general cases, the position and size requirements of a child or parent actor (from it's child or parent) can be calculated with Size Negotiation.

<h1 class="pg">Constraint Sources</h1>

These are properties of this (or another actor) that are used as inputs into the constraint.
The constraint will take these values, optionally perform a calculation on them (if using a custom functor) and write the result to the specified property of the target actor.
The source actor is specified as either the same actor, it's parent or another actor.

<h2 class="pg">Local Source</h2>

A local source is based on the local properties (i.e. size, position, scale, orientation, color) of an actor.
For example, the actor's orientation could be used as a constraint input source.

@code
Dali::ConstraintSource source( Dali::LocalSource( Dali::Actor::Property::ORIENTATION ) );
@endcode

<h2 class="pg">Parent Source</h2>

A parent source is based on properties of the actor's parent.
For example, a parent's position can be used as a constraint input source.

@code
Dali::ConstraintSource source( Dali::ParentSource( Dali::Actor::Property::POSITION ) );
@endcode

<h2 class="pg">Source</h2>

Finally, you can base your source on the properties of another handle altogether.
For example, a sibling actor's color could be used as a constraint input source.

@code
Dali::ConstraintSource source( Dali::Source( anotherHandle, Dali::Actor::Property::COLOR ) );
@endcode

<h1 class="pg">The Constraint Function</h1>

The signature of the constraint function is:

@code
void Function( PropertyType& current, const Dali::PropertyInputContainer& inputs );
@endcode

Here 'current' is a reference to the target property type, e.g. float, Vector2, Vector3 etc.
This is an in/out parameter.
It represents the current value of the property and the expectation is that it will be modified by the function to the desired value.

The 'inputs' parameter holds all the constraint input sources.
Each element is a pointer to the property-input and can be accessed using the indexing operator[].
The order in which the sources are added is the order in which the property-inputs are sorted in the container. For example:

@code
constraint.AddSource( Dali::LocalSource( Dali::Actor::Property::POSITION ) );
constraint.AddSource( Dali::LocalSource( Dali::Actor::Property::SIZE ) );
constraint.AddSource( Dali::ParentSource( Dali::Actor::Property::POSITION ) );
constraint.AddSource( Dali::ParentSource( Dali::Actor::Property::SIZE ) );
@endcode

In the constraint function this equates to:
@code
const Dali::Vector3& position( inputs[0]->GetVector3() );
const Dali::Vector3& size( inputs[1]->GetVector3() );
const Dali::Vector3& parentPosition( inputs[2]->GetVector3() );
const Dali::Vector3& parentSize( inputs[3]->GetVector3() );
@endcode

<h1 class="pg">Creating a Constraint</h1>

<h2 class="pg">Using C Functions</h2>

If you do not have any data that is changed at runtime, then C functions should be used.
For example, the color of an actor could be changed based on its position along the x-axis till a preset distance of 100, beyond which it is transparent.

@code
Dali::Actor actor = Actor::New();

Dali::Constraint constraint = Dali::Constraint::New< Vector4 >( actor, Dali::Actor::Property::COLOR, MyConstraintFunction ); // Creates a constraint that targets actor
constraint.AddSource( Dali::LocalSource( Dali::Actor::Property::POSITION ) ); // Adds the POSITION property as a constraint input
constraint.Apply(); // The constraint is applied
@endcode

And the actual C Function:

@code
void MyConstraintFunction( Dali::Vector4& current, const Dali::PropertyInputContainer& inputs )
{
  const Dali::Vector3& position( inputs[0]->GetVector3() );

  float distance = fabs( position.x );

  // More than 100.0f away, opacity is 0.0f
  if ( distance > 100.0f )
  {
    current.a = 0.0f;
  }
  else
  {
    // Otherwise it will blend between fully opaque and transparent
    current.a = ( 100.0f - distance ) / 100.0f;
  }
}
@endcode

Please have a look at Dali::Constraint::New() for more details.

<h2 class="pg">Using Functors</h2>

If you need to store some data in a struct/class, then a functor can be used.
Reusing the last example, the color of an actor is changed based on its position along the x-axis, but the distance when it is transparent is different for each applied constraint.

@code
Dali::Actor actor = Actor::New();

Dali::Constraint constraint = Dali::Constraint::New< Vector4 >( actor, Dali::Actor::Property::COLOR, MyFunctor( 200 ) ); // Creates a constraint that targets actor, and uses MyFunctor with a distance of 200
constraint.AddSource( Dali::LocalSource( Dali::Actor::Property::POSITION ) ); // Adds the POSITION property as a constraint input
constraint.Apply(); // The constraint is applied
@endcode

And the struct:

@code
struct MyFunctor
{
  /// Constructor which takes the distance at which the actor will be fully transparent
  MyFunctor( float distance )
  : mDistance( distance )
  {
  }

  /// Functor
  void operator()( Dali::Vector4& current, const Dali::PropertyInputContainer& inputs )
  {
    const Dali::Vector3& position( inputs[0]->GetVector3() );

    float distance = fabs( position.x );

    // More than mDistance away, opacity is 0.0f
    if ( distance > mDistance )
    {
      current.a = 0.0f;
    }
    else
    {
      // Otherwise it will blend between fully opaque and transparent
      current.a = ( mDistance - distance ) / mDistance;
    }
  }

  // Data
  const float mDistance;
};
@endcode

MyFunctor could then be used with another constraint with a different distance.

Please have a look at Dali::Constraint::New(Handle, Property::Index, const T&) for more details.

Instead of using the default functor, another method can be declared in the class or struct and used as the constraint function.
Please have a look at the appropriate Dali::Constraint::New() method for more details.

<h1 class="pg">Removing Constraints</h1>

The actor's constraints can later be removed in several ways:

@code
mConstraint.Remove(); // mConstraint is a base-handle to a constraint
actor.RemoveConstraints(); // Removes ALL constraints applied to an actor
actor.RemoveConstraint( tag ); // All constraints with the tag are removed from the actor (tag can be set using SetTag)
@endcode

\section constraints-equal-to Equal To Constraint

The built in Dali::EqualToConstraint can be used if all that is required is setting a property equal to another property:
@code
Dali::Constraint constraint = Dali::Constraint::New< Vector3 >( actor, Dali::Actor::Property::POSITION, Dali::EqualToConstraint() );
constraint.AddSource( Dali::Source( anotherActor, Dali::Actor::Property::POSITION ) );
constraint.Apply();
@endcode
Here actor's position is set to equal the position of anotherActor.

\section constraints-relative-to Relative To Constraint

The built in Dali::RelativeToConstraint and Dali::RelativeToConstraintFloat can be used if all that is required is setting a property relative to another property:
@code
Dali::Constraint constraint = Dali::Constraint::New< Vector3 >( actor, Dali::Actor::Property::POSITION, Dali::RelativeToConstraint( 2.0f ) );
constraint.AddSource( Dali::Source( anotherActor, Dali::Actor::Property::POSITION ) );
constraint.Apply();
@endcode
Here actor's position is relative to the position of anotherActor, i.e., if anotherActor is at (10.0f, 20.0f, 30.0f), actor will be at (20.0f, 40.0f, 60.0f).
*
*/
