/*! \page animation-example Example and Usage

   We will start by showing the steps for animating an actors position and opacity so the actor moves whilst the opacity changes.

   Next more interesting animations methods (effects) with individual timing will be explained.

   \section simple-anim Simple Animation moving an actor whilst altering opacity.

   We declare an animation called myAnimation

   @code
   Dali::Animation myAnimation
   @endcode

   A new animation instance is created with a specified duration.  Here the duration of the animation will be 0.5 seconds.
   Some other settings are incorporated by default and will be explained later.
   @code
   myAnimation = Animation::New(0.5f);
   @endcode

   Here we add the animators, these are the building blocks (functions) of the animation and define what we want to see
   @code
   myAnimation.OpacityTo(actor, 1.0f);
   myAnimation.MoveTo(actor, x, y, z);
   @endcode

   start animation, when this is called we want the animation to start playing so here the actor will move whilst the opacity changes.
   @code
   myAnimation.Play();
   @endcode
   \section advanced-anims Adding more advanced animators, if simply moving the actor is not enough we have further animations methods.

   For actor there exists a range of methods ranging from Move, Rotate, Scale, Opacity, Color to Resize.  The API explains in detail what each does and the parameters to these methods but there are some points worth noting;

   xxxxBy and xxxxTo, method names are appended by 'By' or 'To' either animate to a supplied value or animate by a supplied value.  For example an actor at (10,10,10) calling MoveBy(actor, 50,0,0) would mean its location is (60,0,0) whilst MoveTo(actor, 50,0,0) would result in a location (50,0,0).

   Dali::AlphaFunctions can be used to give interesting effects to the animation, for example the MOVEment of an actor on screen can speed up and slow down following a sine curve by using the Dali::AlphaFunctions::EaseInOutSine instead of AlphaFunctions::Linear.

   @code
   myAnimation.MoveTo(actor, Vector3(x, y, z), AlphaFunctions::Linear, delay, ANIMATOR_DURATION);
   myAnimation.Resize(actor, actorSize, AlphaFunctions::EaseIn, delay, ANIMATOR_DURATION);
   @endcode
   \section playback-control Controlling a playing animation

   The 3 controls we have are start, pause and stop
   @code
   Dali::Animation::Play();
   Dali::Animation::Stop();
   Dali::Animation::Pause();
   @endcode

   \section default-settings Default settings
   The call Dali::Animation::New provides some default settings but each has a setter function if the defaults are not suitable. (Getters also exist)
   @code
   Dali::Animation::SetEndAction
   Dali::Animation::SetLooping
   Dali::Animation::SetDefaultAlphaFunction
   @endcode

   \section end-signal End Signal
   The following signal can be connected to if it is required to know when an animation is complete.
   /code Dali::Animation::SignalFinish(); /endcode

 */

