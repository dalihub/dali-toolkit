/*! \page dali-application Dali Application and Adaptor
 *
<h2 class="pg">Creating an Application</h2>

The Adaptor framework provides several classes which intialises and sets up Dali appropriately so that the application writer does not have to.  These classes also provides many platform related services (e.g. orienation change notifications, timer services etc.).

The simplest way to create an application that uses Dali is to utilise the Dali::Application class.  In addition to initialising the environment used by Dali, it also provides several signals which the user can connect to when certain platform related activities occur.  It also ensures that, upon system events, Dali is called in a thread-safe manner.

The following example shows how to create a Dali::Application instance and connect to its initialise signal (which is where a Dali::Actor hierarchy should be created).

@code
void CreateProgram(Application& app)
{
  // Create Dali components...
  Dali::Actor actor = Actor::New();
  ...
}

int main (int argc, char **argv)
{
  Application app = Application::New(argc, argv);
  app.SignalInit().Connect(&CreateProgram);
  app.MainLoop();
}
@endcode

Please see the Dali::Application class for other signals to which the application can connect.

<h2 class="pg">Using an Adaptor or EvasPlugin instead of the Application class</h2>

If the application requires finer grained control, an Dali::Adaptor can be created instead.  This allows the application writer to create other platform related functionality themselves (e.g managing the main loop, providing a surface to render to etc.).

When using the Adaptor, the application writer can specify the use of normal window creation and drawing by using the New method with an appropriate Window.

If the application writer wants Dali to draw to a specific surface then they need to create a Dali::RenderSurface instance and use the Adaptor constructor which takes the Dali::RenderSurface as the parameter.

The only signal provided by the adaptors is a <i>surface resized signal</i>; the application writer will have to handle system signals like <i>initialise, pause, terminate </i> etc. themselves.  It is also important that any calls to Dali are made in a thread-safe manner from your application when using the adaptor directly.

An adaptor can be created as shown below:

@code
void CreateProgram(void* data)
{
  // Start Adaptor
  Dali::Adaptor* adaptor = reinterpret_cast<Dali::Adaptor*>(data);
  adaptor->Start();

  // Create Dali components...
  // Can instantiate here, if required
}

int main ()
{
  // Initialise platform
  MyPlatform.Init();

  // Create an 800 by 1280 window positioned at (0,0).
  Dali::PositionSize positionSize(0, 0, 800, 1280);
  Dali::Window window = Dali::Window::New( positionSize, "My Application" );
  Dali::Adaptor& adaptor = Dali::Adaptor::New( window );

  // Assuming second parameter takes in data which is passed back to the callback function
  MyPlatform.InitialisationConnection(&CreateProgram, &adaptor);

  // Start Main Loop of your platform
  MyPlatform.StartMainLoop();

  return 0;
}
@endcode

A Dali::EvasPlugin instance can be created by EFL applications that wish to use Dali.  Like the Adaptor, it also provides a means for initialising the resources required by the Dali::Core.

The Dali::EvasPlugin emits several signals which the user can connect to.  The user should not create any Dali objects in the main function and instead should connect to the Init signal of the EvasPlugin and create the Dali objects in the connected callback.

A Dali::EvasPlugin can be used in an EFL application as shown below:

@code
void Created(EvasPlugin& evasPlugin)
{
  // Create Dali components...
  // Can instantiate here, if required
}

void Resized(EvasPlugin& evasPlugin)
{
  // Set size properties of Dali components
  // Set screen layout
}

int main (int argc, char **argv)
{
  // Initialise Elementary
  elm_init(&argc, &argv);

  // Create an Evas Window
  Evas_Object* win = elm_win_add(...);

  // Get the actual window
  Evas* e = evas_object_evas_get(win);

  // Create the EvasPlugin and pass the actual window
  Dali::EvasPlugin evasPlugin = Dali::EvasPlugin(e);

  evasPlugin.SignalInit().Connect(&Created);
  evasPlugin.SignalResize().Connect(&Resized);

  // Retrieve the Evas_Object from the plugin and show it.
  Evas_Object* evasObject = evasPlugin.GetEvasObject();
  evas_object_show(evasObject);

  // add evasObject to layout such as elm_box

  // Start main loop
  elm_run();
}
@endcode

<h2 class="pg">Window</h2>
Dali provides a Window class to manage drawing to a default surface. It is also responsible for drawing the Indicator bar if required. The Application class automatically creates a Window which the application author can access after the SignalInit has fired.

@code
void CreateProgram(Application& app)
{
  app.GetWindow().ShowIndicator(Dali::Window::VISIBLE);
}

int main (int argc, char **argv)
{
  Application app = Application::New(argc, argv);
  app.SignalInit().Connect(&CreateProgram);
  app.MainLoop();
}
@endcode

<h2 class="pg">Orientation</h2>

The Adaptor Framework also provides a means of retrieving the current device orientation and connection to a signal when the orientation of the device changes.  The Dali::Application class provides access to an already created Dali::Orientation object.  If using a Dali::Adaptor, an instance of the Dali::Orientation class has to be created in the application.

The following example shows how to connect to an orientation changed signal through the Dali::Application class:

@code
void OrientationChanged(const Orientation& orientation)
{
  int degrees = orientation.GetDegrees();
  ...
}

int main(int argc, char **argv)
{
  Application app = Application::New(&argc, &argv);
  app.GetOrientation().SignalChanged().Connect(&OrientationChanged);
}
@endcode

<h2 class="pg">Timers</h2>

Timers are also provided by the Adaptor Framework so that the application writer can execute a portion of their code periodically or just once, after a delay.  The example below shows how a Dali::Timer can be created and used:

@code
bool Tick()
{
  ...
  return true; // Timer continues, i.e. this function will be called again after the specified time has elapsed.
}

...

// Elsewhere
Timer timer = Timer::New(2000); // 2 second timeout
timer.SignalTick().Connect(&Tick);
...
@endcode

 */
