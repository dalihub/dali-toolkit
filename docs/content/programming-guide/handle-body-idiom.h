/*! \page handle-body-idiom Handle – body
<h2 class="pg">What is the Handle/Body (Pimpl) pattern?</h2>
It is a technique for hiding implementation details in the header file.
DALi achieves it by using "handles" in the public API. These handles internally contain a reference counted pointer to the concrete implementation.

<h2 class="pg">Why does Dali::Object use the Handle/Body (Pimpl) pattern?</h2>
It provides:
- Better encapsulation
- Easier memory management

\par Better encapsulation
Implementation details are hidden, only the required API is visible for the application developer.
This way the danger of API/ABI breaks is also reduced since the implementation of a class can change without modifying the public API.

\par Easier memory management
DALi objects have implicit smart-pointer semantics.
Each Dali::Object contains a single reference counted object which can be intitialized with the static "New" methods in the DALi API.
This means that C++ new/delete operators do not have to be used (or paired) in the user code (RAII idiom).
Of course there's no way of stopping users from allocating heap memory, but calls to the new operator can be minimised.

<h2 class="pg">What does 'implicit smart-pointer semantics' mean in the case of Dali?</h2>

Since DALi objects are just handles, they can be copied by value. When a DALi object is copied, both the copy and original will point to the same DALi resource.
The internal DALi resources are reference counted; copying a DALi object will increase the reference count. A resource will not be deleted until all its Dali::Object handles are destroyed, or reset.

\code
class AnimationTest
{
...
private:
  Animation mAnimation; // animation handle
}

void AnimationTest::Initialize ()
{
    mAnimation = Animation::New(10.0f); // ref.count will be 1, animation object stays alive when method returns
    ...
}

void AnimationTest::SetAnimation (Animation anim)
{
    mAnimation = anim; // ref.count of original animation decreased, 'anim' is referenced instead
                       // if nobody else had a reference on the initial animation, the object is destroyed
}
\endcode

In some cases an internal resource may be referenced by other internal objects.
A common example is when an actor is added to a container with Dali::Actor::Add() i.e. the container will reference its child.

\code
// At this point we own a Dali::Actor named "container"
// Enter a code block
{
  // Create an image view
  Actor actor = Toolkit::ImageView::New(SomeImageFile);

  // Add the image actor to a container
  container.Add(actor);
}
// Exit the code block
// At this stage the image actor is still alive
// We don't keep a Dali::Object handle to the image actor, but it can be retrieved from the container
\endcode

Objects can be implicitly converted to pointer-to-member type for validity checks.
\code
// Enter a code block
{
  // Create a NULL object
  Object object;
  // At this stage we cannot call any of the objects methods

  if (!object) // This test is will pass, since the object is NULL
  {
    object = SomeClass::New();
    ...
  }
  ...
}
\endcode

Objects can be compared, this actually checks if the handles point to the same resource or not.
\code
void AnimationTest::SetAnimation (Animation anim)
{
  if (anim != mAnimation)
  {
    mAnimation = anim;  // ref.count of original animation decremented (if valid), anim's reference count is increased
    ...
  }
}
\endcode

To sum up implicit pointer semantics, Objects can be:
- compared
- passed by value; this increases the reference count
- tested as a boolean value
- used directly as member data
- returned from functions

*/

