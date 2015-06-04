/*! \page animation-multi-threading-notes Animation: Multi-threading Notes
 *

<h2 class="pg">Multi-threaded Architecture</h2>

DALi animations and rendering occur in a dedicated rendering thread.  This allows animations to run smoothly, regardless of the time taken to process inputs events etc. in application code.

Internally DALi contains a scene-graph, which mirrors the Actor hierachy.  The scene-graph objects perform the actual animation & rendering, whilst Actors provide thread-safe access.

An example actor hierachy is shown below, in which one of the actors is being animated.  The objects in green are created by the application code, whilst the private objects in blue are used in the dedicated rendering thread.

\image html multi-threaded-animation.png

<h2 class="pg">Reading an animated value</h2>

When a property is animatable, it can only be modified in the rendering thread.  The value returned from a getter method, is the value used when the previous frame was rendered.

For example \ref Dali::Actor::GetCurrentPosition "Dali::Actor::GetCurrentPosition" returns the position at which the Actor was last rendered.  Since \ref Dali::Actor::SetPosition "Dali::Actor::SetPosition" is asynchronous, a call to \ref Dali::Actor::GetCurrentPosition "Dali::Actor::GetCurrentPosition" won't immediately return the same value.

@code
// Whilst handling an event...

Actor actor = Actor::New();
Stage::GetCurrent().Add(actor); // initial position is 0,0,0

actor.SetPosition(Vector3(10,10,10));

Vector3 current;
current = actor.GetCurrentPosition();
std::cout << "Current position: " << current.x << ", " << current.y << ", " << current.z << std::endl;

std::cout << "..." << std::endl;

// Whilst handling another event...

current = actor.GetCurrentPosition();
std::cout << "Current position: " << current.x << ", " << current.y << ", " << current.z << std::endl;

@endcode

The example code above would likely output:

@code
"Current position: 0,0,0"
"..."
"Current position: 10,10,10"
@endcode

<h2 class="pg">Setting a property during an animation</h2>

When a property is being animated, the Animation will override any values set e.g. with Actor::SetPosition()

\image html multi-threaded-animation-2.png

The order of execution in the render thread is:

@code
1) Process message => SetPosition
2) Apply animation => SetPosition
3) Render frame
@endcode

*
*/
