/*! \page scroll-view Example and Usage

   We will start by showing the steps to creating a ScrollView, adding to the stage, and adding content to the ScrollView.

   Then we look at some of the options to achieve commonly desired ScrollView effects, from ruler snap points to domains.

   \section intro Simple ScrollView setup, and ruler configuration.

   We declare a ScrollView component called myScrollView

   @code
   Dali::Toolkit::ScrollView myScrollView;
   @endcode

   A new ScrollView instance is then created by calling the following
   @code
   myScrollView = ScrollView::New();
   @endcode

   We then add it to the stage.
   @code
   Stage::GetCurrent().Add(myScrollView);
   @endcode

   Then we specify the size. We'll make it cover the entire stage
   @code
   Stage stage = Dali::Stage::GetCurrent();
   Size size = stage.GetSize();
   myScrollView.SetSize( size );
   @endcode

   Add Actors to this ScrollView
   @code
   ImageView imageActor = Toolkit::ImageView::New(DALI_IMAGE_DIR "button-background.png");
   myScrollView.Add( imageActor );
   @endcode

   The ScrollView contents are now draggable by the user using touch (panning gestures).

   To enforce horizontal-only scrolling, the Y-axis ruler can be disabled
   @code
   RulerPtr rulerY = new DefaultRuler();
   rulerY->Disable();
   myScrollView.SetRulerY(rulerY);
   @endcode

   To enable snapping, a FixedRuler can be applied to the X-axis, with snap points spaced to the width of the stage.
   @code
   Stage stage = Dali::Stage::GetCurrent();
   Size size = stage.GetSize();
   RulerPtr rulerX = new FixedRuler(size.width);
   myScrollView.SetRulerX(rulerX);
   @endcode

   A domain can be applied to rulers to prevent scrolling beyond this boundary. In this case to 4 times the width of the stage, allowing for 4 pages to be scrolled.
   @code
   Stage stage = Dali::Stage::GetCurrent();
   Size size = stage.GetSize();
   RulerPtr rulerX = new FixedRuler(size.width);
   rulerX->SetDomain(RulerDomain(0.0f, size.width*4.0f));
   myScrollView.SetRulerX(rulerX);
   @endcode

   Ruler      Domain      Wrap      Behaviour
   =====      ======      ====      =========

   Disabled   Disabled    No-Wrap   "No Movement in axis"
   Disabled   Disabled    Wrap      "No Movement in axis"
   Disabled   Enabled     No-Wrap   "No Movement in axis"
   Disabled   Enable      Wrap      "No Movement in axis"
   Enabled    Disabled    No-Wrap   "Free Movement in axis"
   Enabled    Disabled    Wrap      "Free Movement in axis, but will wrap based on domain min-max"
   Enabled    Enabled     No-Wrap   "Movement limited to domain min-max"
   Enabled    Enabled     Wrap      "Movement limited to domain min-max"

   @note It is important to note that Actors within ScrollView are controlled by constraints,
   and thus undefined behaviour will occur when applying constraints to these Actors externally.
   If you wish to apply additional constraints that may conflict with the ScrollView's constraints,
   then it is recommended that you place the Actors within container Actors. So that the container
   Actors are affected by the constraints.

 */

