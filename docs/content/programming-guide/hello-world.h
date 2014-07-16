/*! \page hello-world Hello World - explained

The following steps are required for displaying the sentence 'Hello World' with Dali:

- initialize the Dali library
- create an Actor showing text
- add it to the Stage

To understand the basic building blocks of the UI make sure to read the chapter on \link fundamentals Dali Fundamentals\endlink first.

Let's take a look at the code for this test application.

<h2 class="pg"> Example code </h2>
\code
#include <dali/dali.h>

using namespace Dali;

/******************************************************
 * Demonstrates how to display "Hello World" on screen
 ******************************************************/

class ExampleApp
{
public:
  ExampleApp(Application &app)
  : mApp(app)
  {
    // Connect to Dali::Application init signal. Do not make calls to Dali before this signal is received.
    app.SignalInit().Connect(this, &ExampleApp::Create);
  }

  ~ExampleApp()
  {
    // Remove Hello World TextActor from stage
    Stage::GetCurrent().Remove(mTextActor);
  }

public:

  void Create(Application& app)
  {
    // Initialize the actor
    mTextActor = TextActor::New("Hello World");

    // Center the actor. Note: default anchor point is CENTER
    mTextActor.SetParentOrigin(ParentOrigin::CENTER);

    // Display the actor on the stage
    Stage::GetCurrent().Add(mTextActor);
  }

private:
  Application& mApp;
  TextActor mTextActor;
};

int
main(int argc, char **argv)
{
  Application daliApp(&argc, &argv);

  ExampleApp helloApp (daliApp);
  daliApp.MainLoop();

  return 0;
}

\endcode

 There are a couple of steps which are very important to understand.

 <h2 class="pg"> Initializing Dali </h2>
 The application should not use the Dali library until it has sent the init complete signal!
 That's why we connect our ExampleApp::Create callback to Dali::Application's SignalInit signal:
 \code
   ...
   app.SignalInit().Connect(this, &ExampleApp::Create);
   ...
 \endcode

 <h2 class="pg"> Reference counting </h2>
 The application should store Actors' and resources' handles.
 Dali objects are reference counted, which makes sure they exist only as long as they are needed.
 That's why we store the Actor's handle:
 \code
   ...
   mTextActor = TextActor::New("Hello World");
   ...
 \endcode
 Even if the TextActor is removed from the stage, it will be kept alive through our reference.\n
 You can read more about implicit smart-pointer semantics in chapter \link handle-body-idiom Handle – body\endlink.

 <h2 class="pg"> Main loop </h2>
 To 'run' the application, it's main loop should be started.
 This ensures that images are displayed, events, signals are dispatched and captured and so on.
 \code
   ...
   daliApp.MainLoop();
   ...
 \endcode
 \n \n
 On X11 platform you can compile the above example with:
 \code
 g++ `pkg-config --libs --cflags dali` hello-dali.cpp -o hello
 \endcode

 After running './hello' this should be visible on the screen:

 \image html Text-Actor.png "Hello world example"

*/
