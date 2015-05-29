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

TODO