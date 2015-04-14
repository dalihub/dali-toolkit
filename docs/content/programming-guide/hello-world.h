/*! \page hello-world Hello World - explained

The following steps are required for displaying the sentence 'Hello World' with Dali:

- initialize the Dali library
- create an Actor showing text
- add it to the Stage

To understand the basic building blocks of the UI make sure to read the chapter on \link fundamentals Dali Fundamentals\endlink first.

Let's take a look at the code for this test application.

<h2 class="pg"> Example code </h2>
\code

#include <dali-toolkit/dali-toolkit.h>

using namespace Dali;
using Dali::Toolkit::TextLabel;

// This example shows how to create and display Hello World! using a simple TextActor
//
class HelloWorldController : public ConnectionTracker
{
public:

  HelloWorldController( Application& application )
  : mApplication( application )
  {
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect( this, &HelloWorldController::Create );
  }

  ~HelloWorldController()
  {
    // Remove Hello World actor from stage
    Stage::GetCurrent().Remove(mTextLabel);
  }

  // The Init signal is received once (only) during the Application lifetime
  void Create( Application& application )
  {
    // Get a handle to the stage
    Stage stage = Stage::GetCurrent();

    mTextLabel = TextLabel::New( "Hello World" );
    mTextLabel.SetAnchorPoint( AnchorPoint::TOP_LEFT );
    stage.Add( mTextLabel );

    // Respond to a click anywhere on the stage
    stage.GetRootLayer().TouchedSignal().Connect( this, &HelloWorldController::OnTouch );
  }

  bool OnTouch( Actor actor, const TouchEvent& touch )
  {
    // quit the application
    mApplication.Quit();
    return true;
  }

private:
  Application&  mApplication;
  TextLabel mTextLabel;
};

void RunTest( Application& application )
{
  HelloWorldController test( application );

  application.MainLoop();
}

// Entry point for Linux & Tizen applications
//
int main( int argc, char **argv )
{
  Application application = Application::New( &argc, &argv );

  RunTest( application );

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
   mTextLabel = TextLabel::New("Hello World");
   ...
 \endcode
 Even if the TextLabel is removed from the stage, it will be kept alive through our reference.\n
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

<table border=0 cellpadding=10>
<tr>
  <td>
  \image html Text-Label.png "Hello world example"
  </td>
</tr>
</table>

*/
