/*! \page dali-application DALi Application
 *
<h2 class="pg">Creating an Application</h2>

The Adaptor framework provides provides a Dali::Application class which initialises and sets up Dali appropriately so that the application writer does not have to.
This provides many platform related services.

Several signals can be connected to so that the application writer is informed when certain platform related activities occur.
It also ensures that, upon system events, DALi is called in a thread-safe manner.

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
  Dali::Application app = Application::New(&argc, &argv);
  app.InitSignal().Connect(&CreateProgram);
  app.MainLoop();
}
@endcode

Please see the Dali::Application class for other signals to which the application can connect.

<h2 class="pg">Window</h2>
DALi provides a Window class to manage drawing to a default surface. It is also responsible for drawing the Indicator bar if required. The Application class automatically creates a Window which the application author can access after the SignalInit has fired.

@code
void CreateProgram(Application& app)
{
  app.GetWindow().ShowIndicator(Dali::Window::VISIBLE);
}

int main (int argc, char **argv)
{
  Dali::Application app = Application::New(argc, argv);
  app.SignalInit().Connect(&CreateProgram);
  app.MainLoop();
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
Dali::Timer timer = Dali::Timer::New(2000); // 2 second timeout
timer.SignalTick().Connect(&Tick);
...
@endcode

 */
