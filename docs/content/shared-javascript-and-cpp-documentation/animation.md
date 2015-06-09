<!--
/**-->

# Animation {#animation}

DALi provides a rich and easy to use animation framework which allows the creation of visually rich applications.
Dali::Animation can be used to animate the properties of any number of objects, typically Actors.

## Creating a basic Animation {#animation-basics}

Create an animation object that will take place over 3 seconds:
~~~{.cpp}
Dali::Animation animation = Animation::New( 3.0f );
~~~

### Animating Properties

There are two distint ways in which properties can be animated within DALi:
- **AnimateTo:** The property will animate **TO** the value in the given time.
- **AnimateBy:** The property will animate **BY** the value in the given time.

(Assume actor1 & actor2 are at position 10.0f, 10.0f, 0.0f at the start of the animation)
~~~{.cpp}
// Animate the position of actor1 TO 10.0f, 50.0f, 0.0f
animation.AnimateTo( Property( actor1, Dali::Actor::Property::POSITION ), Vector3(10.0f, 50.0f, 0.0f) ); // End Position: 10.0f, 50.0f, 0.0f

// Animate the position of actor2 BY 10.0f, 50.0f, 0.0f
animation.AnimateBy( Property( actor2, Dali::Actor::Property::POSITION ), Vector3(10.0f, 50.0f, 0.0f) ); // End Position: 20.0f, 60.0f, 0.0f
~~~

### Playback Control

When an animation is created, it can be played:
~~~{.cpp}
animation.Play();
~~~

Stop and Pause are also supported.
~~~{.cpp}
animation.Stop();
animation.Pause();
~~~

### Notifications

Using DALi's signal framework, applications can be notified when the animation finishes:

~~~{.cpp}

void ExampleCallback( Animation& source )
{
  std::cout << "Animation has finished" << std::endl;
}
...
animation.FinishedSignal().Connect( ExampleCallback );
~~~

### Alpha Functions

Alpha functions are used in animations to specify the rate of change of the animation parameter over time.
The built in supported functions can be viewed in Dali::AlphaFunction::BuiltinFunction.

It is possible to specify a different alpha function for each animator in an Animation object:
~~~{.cpp}
animation.AnimateTo( Property( actor1, Dali::Actor::Property::POSITION ), Vector3(10.0f, 50.0f, 0.0f), Dali::AlphaFunction::EASE_IN );
~~~

### Other Actions

An animation can be looped:
~~~{.cpp}
animation.SetLooping( true );
~~~

By default, when an animation ends, the properties that it was animating are BAKED.
However, the property changes can be **discarded** when the animation ends (or is stopped):
~~~{.cpp}
animation.SetEndAction( Animation::Discard );
~~~

## Key-Frame Animation {#animation-key-frame}

DALi provides support for animating between several different values, i.e. key-frames.
A key frame takes a progress value between 0.0f and 1.0f (0 and 100% respectively) and portrays the value of the property when the animation has progressed that much.
You can create several key frames:
~~~{.cpp}
Dali::KeyFrames keyFrames = Dali::KeyFrames::New();
keyFrames.Add( 0.0f, Vector3( 10.0f, 10.0f, 10.0f ) );
keyFrames.Add( 0.7f, Vector3( 200.0f, 200.0f, 200.0f ) );
keyFrames.Add( 1.0f, Vector3( 100.0f, 100.0f, 100.0f ) );
~~~
And then add them to your animation.
~~~{.cpp}
animation.AnimateBetween( Property( actor1, Dali::Actor::Property::POSITION ), keyFrames );
~~~
When you play the animation, DALi will animate the position of actor1 between the key-frames specified.
'actor1' will animate from (10.0f, 10.0f, 10.0f) to (200.0f, 200.f, 200.0f) by 70% of the animation time,
and then spend the remaining time animating back to (100.0f, 100.0f, 100.0f).

The advantage of specifying a key-frame at 0% is that regardless of where 'actor1' is, it will start from position (10.0f, 10.0f, 10.0f).
If AnimateTo was used, then the start position would have been actor1's current position.

## Path Animations {#animation-paths}

A Dali::Path can be used to animate the position and orientation of actors.

![ ](animation/animated-path.png)

The black points in the diagram are points where the DALi logo will travel to.
The red points are the control points which express the curvature of the path on the black points.

This, in code will be represented as follows:
~~~{.cpp}
Path path = Path::New();
path.AddPoint( Vector3( 50.0f, 10.0f, 0.0f ));
path.AddPoint( Vector3( 90.0f, 50.0f, 0.0f ));
path.AddPoint( Vector3( 10.0f, 90.0f, 0.0f ));
~~~
The control points can be added manually using Dali::Path::AddControlPoint or Path can auto-generate them for you:
~~~{.cpp}
path.GenerateControlPoints(0.25f);
~~~
Here 0.25f represents the curvature of the path you require. Please see Dali::Path::GenerateControlPoints for more information.

To animate actor1 along this path:
~~~{.cpp}
animation.Animate( actor1, path, Vector3::ZERO );
~~~
The third parameter is the forward vector (in local space coordinate system) that will be oriented with the path's tangent direction.