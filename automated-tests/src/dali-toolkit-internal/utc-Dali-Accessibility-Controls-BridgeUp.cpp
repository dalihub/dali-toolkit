#include <dali-toolkit-test-suite-utils.h>
#include <automated-tests/src/dali-toolkit-internal/dali-toolkit-test-utils/accessibility-test-utils.h>
#include <automated-tests/src/dali-toolkit-internal/dali-toolkit-test-utils/dbus-wrapper.h>
#include <dali-toolkit/dali-toolkit.h>
#include <dali/devel-api/adaptor-framework/accessibility.h>
#include <dali/devel-api/adaptor-framework/accessibility-impl.h>
#include <dali-toolkit/devel-api/controls/control-devel.h>
#include <dali-toolkit/devel-api/controls/text-controls/text-selection-popup.h>
#include <dali/devel-api/actors/actor-devel.h>
#include <dali/devel-api/common/stage.h>
#include <cstdlib>

#include <automated-tests/src/dali-toolkit-internal/dali-toolkit-test-utils/dbus-wrapper.h>

using namespace Dali::Toolkit;

void utc_dali_accessibility_controls_bridge_up_startup(void)
{
  test_return_value = TET_UNDEF;
  DBusWrapper::Install(std::unique_ptr<DBusWrapper>(new TestDBusWrapper));
}

void utc_dali_accessibility_controls_bridge_up_cleanup(void)
{
  test_return_value = TET_PASS;
  //DBusWrapper::Install({}) is a de-install
  DBusWrapper::Install({});
}

namespace Dali {
  namespace Accessibility {
    std::ostream & operator<< (std::ostream & stream, const Address & address)
    {
      stream << address.ToString();
      return stream;
    }
  }
}

int UtcDaliControlAccessibilityRaiseBridge(void)
{
  ToolkitTestApplication application;

  DALI_TEST_CHECK(!Accessibility::IsUp());

  Dali::Accessibility::TestEnableSC(true);

  DALI_TEST_CHECK(Accessibility::IsUp());

  // auto wr = static_cast<TestDBusWrapper*>(DBusWrapper::Installed());
  // for(auto &a : wr->daliMethods) {
  //   const char *mt;
  //   if (std::get<3>(a.first) == MethodType::Method) mt = "MethodType::Method";
  //   else if (std::get<3>(a.first) == MethodType::Getter) mt = "MethodType::Getter";
  //   else if (std::get<3>(a.first) == MethodType::Setter) mt = "MethodType::Setter";
  //   else assert(0);
  //   printf("%s %s %s %s\n", std::get<0>(a.first).c_str(), std::get<1>(a.first).c_str(), std::get<2>(a.first).c_str(), mt);
  // }

  Dali::Accessibility::TestEnableSC(false);

  DALI_TEST_CHECK(!Accessibility::IsUp());

  END_TEST;
}

int UtcDaliControlAccessibilityName(void)
{
  ToolkitTestApplication application;

  auto control = Control::New();
  Stage::GetCurrent().Add( control );

  auto q = Dali::Accessibility::Accessible::Get( control );
  DALI_TEST_CHECK( q );

  DALI_TEST_EQUALS( "" , q->GetName(), TEST_LOCATION );

  control.SetProperty( DevelControl::Property::ACCESSIBILITY_NAME, "Accessibility_Name" );
  DALI_TEST_EQUALS( "Accessibility_Name" , q->GetName(), TEST_LOCATION );
  DALI_TEST_EQUALS( control.GetProperty( DevelControl::Property::ACCESSIBILITY_NAME ).Get< std::string >() , "Accessibility_Name", TEST_LOCATION );

  DevelControl::AccessibilityGetNameSignal(control).Connect( [] (std::string &accessibility_name) {
    accessibility_name = "Accessibility_Name_With_Callback"; } );

  DALI_TEST_EQUALS( "Accessibility_Name_With_Callback" , q->GetName(), TEST_LOCATION );

  Dali::Accessibility::TestEnableSC(true);

  DALI_TEST_EQUALS( "Accessibility_Name_With_Callback" , TestGetName( q->GetAddress()), TEST_LOCATION );

  Dali::Accessibility::TestEnableSC( false );

  END_TEST;
}

int UtcDaliControlAccessibilityDescription(void)
{
  ToolkitTestApplication application;

  auto control = Control::New();

  auto q = Dali::Accessibility::Accessible::Get( control );
  DALI_TEST_CHECK( q );

  DALI_TEST_EQUALS( "" , q->GetDescription(), TEST_LOCATION );

  control.SetProperty( DevelControl::Property::ACCESSIBILITY_DESCRIPTION, "Accessibility_Description" );
  DALI_TEST_EQUALS( "Accessibility_Description" , q->GetDescription(), TEST_LOCATION );

  auto property = control.GetProperty( DevelControl::Property::ACCESSIBILITY_DESCRIPTION ).Get<std::string>();
  DALI_TEST_EQUALS( "Accessibility_Description", property, TEST_LOCATION );

  DevelControl::AccessibilityGetDescriptionSignal(control).Connect( [] (std::string &accessibility_description) {
    accessibility_description = "Accessibility_Description_With_Callback"; } );

  DALI_TEST_EQUALS( "Accessibility_Description_With_Callback" , q->GetDescription(), TEST_LOCATION );

  Dali::Accessibility::TestEnableSC( true );

  DALI_TEST_EQUALS( "Accessibility_Description_With_Callback" , TestGetDescription( q->GetAddress()), TEST_LOCATION );

  Dali::Accessibility::TestEnableSC( false );

  END_TEST;
}

int UtcDaliControlAccessibilityRole(void)
{
  ToolkitTestApplication application;

  auto control = Control::New();
  auto role_unknown = Dali::Accessibility::Role::UNKNOWN;
  auto role_pushbutton = Dali::Accessibility::Role::PUSH_BUTTON;

  DALI_TEST_EQUALS( role_unknown,control.GetProperty( DevelControl::Property::ACCESSIBILITY_ROLE ).Get< Dali::Accessibility::Role >(), TEST_LOCATION );

  auto q = Dali::Accessibility::Accessible::Get( control );
  DALI_TEST_EQUALS( role_unknown , q->GetRole(), TEST_LOCATION);
  DALI_TEST_EQUALS( "unknown" , q->GetRoleName(), TEST_LOCATION );

  Dali::Accessibility::TestEnableSC( true );
  DALI_TEST_CHECK( q );
  DALI_TEST_EQUALS( static_cast< uint32_t >( role_unknown ), TestGetRole( q->GetAddress() ), TEST_LOCATION );
  DALI_TEST_EQUALS( "unknown" , TestGetRoleName( q->GetAddress() ), TEST_LOCATION );
  DALI_TEST_EQUALS( "unknown" , TestGetLocalizedRoleName( q->GetAddress() ), TEST_LOCATION );

  control.SetProperty( DevelControl::Property::ACCESSIBILITY_ROLE, role_pushbutton );

  DALI_TEST_EQUALS( static_cast< uint32_t >( role_pushbutton ), TestGetRole( q->GetAddress() ), TEST_LOCATION );
  DALI_TEST_EQUALS( "push button" ,TestGetRoleName( q->GetAddress() ), TEST_LOCATION );
  DALI_TEST_EQUALS( "push button" , TestGetLocalizedRoleName( q->GetAddress() ), TEST_LOCATION );

  Dali::Accessibility::TestEnableSC( false );

  DALI_TEST_EQUALS( role_pushbutton , q->GetRole(), TEST_LOCATION);
  DALI_TEST_EQUALS( "push button" , q->GetRoleName(), TEST_LOCATION );

  END_TEST;
}

int UtcDaliControlAccessibilityState(void)
{
  ToolkitTestApplication application;

  auto control = Control::New();
  auto q = Dali::Accessibility::Accessible::Get( control );

  Dali::Accessibility::TestEnableSC( true );

  auto states_by_bridge = Dali::Accessibility::States { TestGetStates( q->GetAddress() )};
  auto states = DevelControl::GetAccessibilityStates(control);
  DALI_TEST_CHECK( states_by_bridge == states );

  Dali::Accessibility::TestEnableSC( false );

  END_TEST;
}

int UtcDaliControlAccessibilityModal(void)
{
  ToolkitTestApplication application;
  auto control = TextSelectionPopup::New( NULL );
  auto states = DevelControl::GetAccessibilityStates(control);

  DALI_TEST_CHECK( states[ Dali::Accessibility::State::MODAL ] );

  END_TEST;
}

int UtcDaliControlAccessibilityHighlightable(void)
{
  ToolkitTestApplication application;
  auto control = Control::New();

  auto noneset = control.GetProperty( DevelControl::Property::ACCESSIBILITY_HIGHLIGHTABLE );
  DALI_TEST_EQUALS( Property::NONE, noneset.GetType(), TEST_LOCATION );

  auto q = Dali::Accessibility::Accessible::Get( control );

  Dali::Accessibility::TestEnableSC( true );

  auto states_by_bridge = Dali::Accessibility::States { TestGetStates( q->GetAddress() )};
  DALI_TEST_CHECK( states_by_bridge[ Dali::Accessibility::State::HIGHLIGHTABLE ] );

  control.SetProperty( DevelControl::Property::ACCESSIBILITY_HIGHLIGHTABLE, true );
  DALI_TEST_EQUALS( Property::BOOLEAN, control.GetProperty( DevelControl::Property::ACCESSIBILITY_HIGHLIGHTABLE ).GetType(), TEST_LOCATION );
  DALI_TEST_EQUALS( true, control.GetProperty( DevelControl::Property::ACCESSIBILITY_HIGHLIGHTABLE ).Get< bool >(), TEST_LOCATION );

  states_by_bridge = Dali::Accessibility::States { TestGetStates( q->GetAddress() )};
  DALI_TEST_CHECK( states_by_bridge[ Dali::Accessibility::State::HIGHLIGHTABLE ] );

  control.SetProperty( DevelControl::Property::ACCESSIBILITY_HIGHLIGHTABLE, false );
  DALI_TEST_EQUALS( Property::BOOLEAN, control.GetProperty( DevelControl::Property::ACCESSIBILITY_HIGHLIGHTABLE ).GetType(), TEST_LOCATION );
  DALI_TEST_EQUALS( false, control.GetProperty( DevelControl::Property::ACCESSIBILITY_HIGHLIGHTABLE ).Get< bool >(), TEST_LOCATION );

  states_by_bridge = Dali::Accessibility::States { TestGetStates( q->GetAddress() )};
  DALI_TEST_CHECK( !states_by_bridge[ Dali::Accessibility::State::HIGHLIGHTABLE ] );

  Dali::Accessibility::TestEnableSC( false );

  END_TEST;
}

int UtcDaliControlAccessibilityHighlightBridgeUp(void)
{
  ToolkitTestApplication application;

  auto controla = Control::New();
  auto controlb = Control::New();

  Dali::Accessibility::TestEnableSC( true );

  auto accessible_a = Dali::Accessibility::Accessible::Get( controla );
  auto accessible_b = Dali::Accessibility::Accessible::Get( controlb );

  auto a = dynamic_cast<Dali::Accessibility::Component*>( accessible_a );
  auto b = dynamic_cast<Dali::Accessibility::Component*>( accessible_b );

  auto states_by_bridge_a = Dali::Accessibility::States { TestGetStates( a->GetAddress() )};
  auto states_by_bridge_b = Dali::Accessibility::States { TestGetStates( b->GetAddress() )};

  DALI_TEST_CHECK( !states_by_bridge_a[ Dali::Accessibility::State::HIGHLIGHTED ] );
  DALI_TEST_CHECK( !states_by_bridge_b[ Dali::Accessibility::State::HIGHLIGHTED ] );

  DALI_TEST_EQUALS( true, DevelControl::GrabAccessibilityHighlight(controla), TEST_LOCATION );

  states_by_bridge_a = Dali::Accessibility::States { TestGetStates( a->GetAddress() )};
  states_by_bridge_b = Dali::Accessibility::States { TestGetStates( b->GetAddress() )};

  DALI_TEST_CHECK( states_by_bridge_a[ Dali::Accessibility::State::HIGHLIGHTED ] );
  DALI_TEST_CHECK( !states_by_bridge_b[ Dali::Accessibility::State::HIGHLIGHTED ] );

  DALI_TEST_EQUALS( true, DevelControl::GrabAccessibilityHighlight(controlb), TEST_LOCATION );
  DALI_TEST_EQUALS( true, DevelControl::GrabAccessibilityHighlight(controlb), TEST_LOCATION );

  states_by_bridge_a = Dali::Accessibility::States { TestGetStates( a->GetAddress() )};
  states_by_bridge_b = Dali::Accessibility::States { TestGetStates( b->GetAddress() )};

  DALI_TEST_CHECK( !states_by_bridge_a[ Dali::Accessibility::State::HIGHLIGHTED ] );
  DALI_TEST_CHECK( states_by_bridge_b[ Dali::Accessibility::State::HIGHLIGHTED ] );

  DALI_TEST_EQUALS( false, DevelControl::ClearAccessibilityHighlight(controla), TEST_LOCATION );

  states_by_bridge_a = Dali::Accessibility::States { TestGetStates( a->GetAddress() )};
  states_by_bridge_b = Dali::Accessibility::States { TestGetStates( b->GetAddress() )};

  DALI_TEST_CHECK( !states_by_bridge_a[ Dali::Accessibility::State::HIGHLIGHTED ] );
  DALI_TEST_CHECK( states_by_bridge_b[ Dali::Accessibility::State::HIGHLIGHTED ] );

  DALI_TEST_EQUALS( true, DevelControl::ClearAccessibilityHighlight(controlb), TEST_LOCATION );

  states_by_bridge_a = Dali::Accessibility::States { TestGetStates( a->GetAddress() )};
  states_by_bridge_b = Dali::Accessibility::States { TestGetStates( b->GetAddress() )};

  DALI_TEST_CHECK( !states_by_bridge_a[ Dali::Accessibility::State::HIGHLIGHTED ] );
  DALI_TEST_CHECK( !states_by_bridge_b[ Dali::Accessibility::State::HIGHLIGHTED ] );

  DALI_TEST_CHECK( TestGrabHighlight( a -> GetAddress() ) );

  states_by_bridge_a = Dali::Accessibility::States { TestGetStates( a->GetAddress() )};
  states_by_bridge_b = Dali::Accessibility::States { TestGetStates( b->GetAddress() )};

  DALI_TEST_CHECK( states_by_bridge_a[ Dali::Accessibility::State::HIGHLIGHTED ] );
  DALI_TEST_CHECK( !states_by_bridge_b[ Dali::Accessibility::State::HIGHLIGHTED ] );

  DALI_TEST_CHECK( TestGrabHighlight( b -> GetAddress() ) );

  states_by_bridge_a = Dali::Accessibility::States { TestGetStates( a->GetAddress() )};
  states_by_bridge_b = Dali::Accessibility::States { TestGetStates( b->GetAddress() )};

  DALI_TEST_CHECK( !states_by_bridge_a[ Dali::Accessibility::State::HIGHLIGHTED ] );
  DALI_TEST_CHECK( states_by_bridge_b[ Dali::Accessibility::State::HIGHLIGHTED ] );

  DALI_TEST_CHECK( TestClearHighlight( b -> GetAddress() ) );

  states_by_bridge_a = Dali::Accessibility::States { TestGetStates( a->GetAddress() )};
  states_by_bridge_b = Dali::Accessibility::States { TestGetStates( b->GetAddress() )};

  DALI_TEST_CHECK( !states_by_bridge_a[ Dali::Accessibility::State::HIGHLIGHTED ] );
  DALI_TEST_CHECK( !states_by_bridge_b[ Dali::Accessibility::State::HIGHLIGHTED ] );


  Dali::Accessibility::TestEnableSC( false );

  END_TEST;
}

int utcDaliAccessibilityControlAttributes(void)
{
  ToolkitTestApplication application;
  auto check_box_button = Toolkit::Control::New();

  std::string value;


  auto attributes = check_box_button.GetProperty( Toolkit::DevelControl::Property::ACCESSIBILITY_ATTRIBUTES );
  auto attributes_map = attributes.GetMap();

  auto position = attributes_map->Find( "access_key1" );
  DALI_TEST_CHECK( !position );

  DevelControl::AppendAccessibilityAttribute( check_box_button, "access_key1", "access_value1" );
  attributes = check_box_button.GetProperty( Toolkit::DevelControl::Property::ACCESSIBILITY_ATTRIBUTES );
  attributes_map = attributes.GetMap();

  DALI_TEST_EQUALS( (attributes_map->Find( "access_key1" ))->Get<std::string>(), "access_value1", TEST_LOCATION );

  DevelControl::AppendAccessibilityAttribute( check_box_button, "access_key2", "access_value2_a" );
  attributes = check_box_button.GetProperty( Toolkit::DevelControl::Property::ACCESSIBILITY_ATTRIBUTES );
  attributes_map = attributes.GetMap();

  DALI_TEST_EQUALS( (attributes_map->Find( "access_key1" ))->Get<std::string>(), "access_value1", TEST_LOCATION );
  DALI_TEST_EQUALS( (attributes_map->Find( "access_key2" ))->Get<std::string>(), "access_value2_a", TEST_LOCATION );

  DevelControl::AppendAccessibilityAttribute( check_box_button, "access_key2", "access_value2_b" );
  attributes = check_box_button.GetProperty( Toolkit::DevelControl::Property::ACCESSIBILITY_ATTRIBUTES );
  attributes_map = attributes.GetMap();

  DALI_TEST_EQUALS( (attributes_map->Find( "access_key2" ))->Get<std::string>(), "access_value2_b", TEST_LOCATION );

  DevelControl::RemoveAccessibilityAttribute( check_box_button, "access_key2" );
  attributes = check_box_button.GetProperty( Toolkit::DevelControl::Property::ACCESSIBILITY_ATTRIBUTES );
  attributes_map = attributes.GetMap();

  // In case when we are removing one of attributes the property is setting for NONE type.
  DALI_TEST_EQUALS( (attributes_map->Find( "access_key2" ))->GetType(), Property::NONE, TEST_LOCATION );

  Dali::Accessibility::TestEnableSC( true );

  auto ptr = Dali::Accessibility::Accessible::Get( check_box_button );
  auto attribute_map_bridge = TestGetAttributes( ptr->GetAddress() );
  auto counter = 0u;
  for (auto i = 0u; i<attributes_map->Count();++i)
    if((attributes_map->GetValue(i)).GetType() != Property::NONE )
      ++counter;

  DALI_TEST_EQUALS( counter, attribute_map_bridge.size(), TEST_LOCATION );

  for (auto it : attribute_map_bridge)
    DALI_TEST_EQUALS( (attributes_map->Find( it.first ))->Get<std::string>(), it.second, TEST_LOCATION );

  DevelControl::ClearAccessibilityAttributes(check_box_button);
  attributes = check_box_button.GetProperty( Toolkit::DevelControl::Property::ACCESSIBILITY_ATTRIBUTES );
  attributes_map = attributes.GetMap();

  position = attributes_map->Find( "access_key1" );
  DALI_TEST_CHECK( !position );

  position = attributes_map->Find( "access_key2" );
  DALI_TEST_CHECK( !position );

  Dali::Accessibility::TestEnableSC( false );

  END_TEST;
}

int UtcDaliControlReadingInfoType(void)
{
  ToolkitTestApplication application;
  auto control = Control::New();

  auto reading_info_type = DevelControl::GetAccessibilityReadingInfoType(control);
  reading_info_type[Dali::Accessibility::ReadingInfoType::DESCRIPTION] = true;
  reading_info_type[Dali::Accessibility::ReadingInfoType::STATE] = true;
  reading_info_type[Dali::Accessibility::ReadingInfoType::NAME] = true;
  reading_info_type[Dali::Accessibility::ReadingInfoType::ROLE] = true;

  DevelControl::SetAccessibilityReadingInfoType(control, reading_info_type);

  auto q = control.GetProperty( Toolkit::DevelControl::Property::ACCESSIBILITY_ATTRIBUTES );
  auto z = q.GetMap();

  DALI_TEST_EQUALS( (z->Find( "reading_info_type" ))->Get<std::string>(), "name|role|description|state", TEST_LOCATION );
  reading_info_type = DevelControl::GetAccessibilityReadingInfoType(control);
  for ( auto i = 0u; i < 4; ++i)
    DALI_TEST_CHECK ( reading_info_type[ static_cast< Dali::Accessibility::ReadingInfoType >( i ) ]);

  END_TEST;
}

int UtcDaliControlDoGesture(void)
{
  ToolkitTestApplication application;
  auto control = Control::New();
  Dali::Accessibility::TestEnableSC( true );

  auto accessible = Dali::Accessibility::Accessible::Get( control );
  auto gesture_one = Dali::Accessibility::GestureInfo { Dali::Accessibility::Gesture::ONE_FINGER_FLICK_LEFT, 600, 100, 500, 500, Dali::Accessibility::GestureState::BEGIN, 1000 };
  auto gesture_two = Dali::Accessibility::GestureInfo { Dali::Accessibility::Gesture::ONE_FINGER_FLICK_RIGHT, 600, 100, 500, 500, Dali::Accessibility::GestureState::BEGIN, 1000 };

  DALI_TEST_CHECK( !accessible->DoGesture( gesture_one ) );
  DALI_TEST_CHECK( !TestDoGesture( accessible->GetAddress(), Dali::Accessibility::Gesture::ONE_FINGER_FLICK_LEFT, 600, 100, 500, 500, Dali::Accessibility::GestureState::BEGIN, 1000 ) );

  DevelControl::AccessibilityDoGestureSignal(control).Connect( [] ( std::pair< Dali::Accessibility::GestureInfo, bool > &gesture ) {
  if ( gesture.first.type == Dali::Accessibility::Gesture::ONE_FINGER_FLICK_LEFT )
    gesture.second = true;
  else
    gesture.second = false;
  } );

  DALI_TEST_CHECK( accessible->DoGesture( gesture_one ) );
  DALI_TEST_CHECK( TestDoGesture( accessible->GetAddress(), Dali::Accessibility::Gesture::ONE_FINGER_FLICK_LEFT, 600, 100, 500, 500, Dali::Accessibility::GestureState::BEGIN, 1000 ) );

  DALI_TEST_CHECK( !accessible->DoGesture( gesture_two ) );
  DALI_TEST_CHECK( !TestDoGesture( accessible->GetAddress(), Dali::Accessibility::Gesture::ONE_FINGER_FLICK_RIGHT, 600, 100, 500, 500, Dali::Accessibility::GestureState::BEGIN, 1000 ) );

  Dali::Accessibility::TestEnableSC( false );

  END_TEST;
}

int UtcDaliAccessibilityRelation(void)
{
  ToolkitTestApplication application;
  Dali::Accessibility::TestEnableSC( true );

  auto rel = Accessibility::RelationType::FLOWS_TO;
  auto number = static_cast< size_t >( rel );
  auto control = Control::New();
  auto destination1 = Control::New();
  auto destination2 = Control::New();

  DevelControl::AppendAccessibilityRelation( control, destination1, rel );
  auto relations = DevelControl::GetAccessibilityRelations(control);
  DALI_TEST_CHECK( relations[ number ].size() == 1 );

  DevelControl::AppendAccessibilityRelation( control, destination2, rel );
  relations = DevelControl::GetAccessibilityRelations(control);
  DALI_TEST_CHECK( relations[ number ].size() == 2 );

  auto accessible = Dali::Accessibility::Accessible::Get( control );
  auto accessible_destination1 = Dali::Accessibility::Accessible::Get( destination1 );
  auto accessible_destination2 = Dali::Accessibility::Accessible::Get( destination2 );
  auto relationset = accessible->GetRelationSet();

  DALI_TEST_CHECK( relationset[0].relationType == rel );
  DALI_TEST_CHECK( relationset[0].targets[0] == accessible_destination1->GetAddress() || relationset[0].targets[1] == accessible_destination1->GetAddress() );
  DALI_TEST_CHECK( relationset[0].targets[0] == accessible_destination2->GetAddress() || relationset[0].targets[1] == accessible_destination2->GetAddress() );

  auto relationset_bridge = TestGetRelationSet( accessible -> GetAddress() );
  DALI_TEST_CHECK( static_cast< uint32_t >( relationset[0].relationType ) == std::get<0>( relationset_bridge[0] ) );

  DALI_TEST_CHECK( relationset[0].targets[0] == std::get<1>( relationset_bridge[0] )[0] || relationset[0].targets[1] == std::get<1>( relationset_bridge[0] )[0] );
  DALI_TEST_CHECK( relationset[0].targets[0] == std::get<1>( relationset_bridge[0] )[1] || relationset[0].targets[1] == std::get<1>( relationset_bridge[0] )[1] );

  DevelControl::RemoveAccessibilityRelation(control,destination2,rel);
  relations = DevelControl::GetAccessibilityRelations(control);
  DALI_TEST_CHECK( relations[ number ].size() == 1 );

  DevelControl::ClearAccessibilityRelations(control);
  relations = DevelControl::GetAccessibilityRelations(control);
  DALI_TEST_CHECK( relations[ number ].size() == 0 );

  Dali::Accessibility::TestEnableSC( false );

  END_TEST;
}

int UtcDaliAccessibilityParentChildren(void)
{
  ToolkitTestApplication application;
  Dali::Accessibility::TestEnableSC( true );

  auto parent = Control::New();
  auto child_1 = Control::New();
  auto child_2 = Control::New();

  auto parent_accessible = Dali::Accessibility::Accessible::Get( parent );
  auto child_1_accessible = Dali::Accessibility::Accessible::Get( child_1 );
  auto child_2_accessible = Dali::Accessibility::Accessible::Get( child_2 );

  auto children = TestGetChildren( parent_accessible -> GetAddress() );
  DALI_TEST_EQUALS( children.size(), 0, TEST_LOCATION );

  try
  {
    TestGetIndexInParent( child_1_accessible -> GetAddress() );
    DALI_ABORT("Object has parent, test abort");
  }
  catch(TestDBusWrapper::error &){}

  try
  {
    TestGetChildAtIndex( parent_accessible -> GetAddress(), -1 );
    DALI_ABORT("Positive index, test abort");
  }
  catch(TestDBusWrapper::error &){}

  DALI_TEST_EQUALS( parent_accessible -> GetChildCount(), 0, TEST_LOCATION );

  try
  {
    child_1_accessible -> GetIndexInParent();
    DALI_ABORT("Object has parent, test abort");
  }
  catch (Dali::DaliException &){}

  parent.Add(child_1);
  parent.Add(child_2);

  children = TestGetChildren( parent_accessible -> GetAddress() );
  DALI_TEST_EQUALS( children.size(), 2, TEST_LOCATION );

  DALI_TEST_EQUALS( parent_accessible -> GetAddress(), TestGetParent( child_1_accessible -> GetAddress() ), TEST_LOCATION );
  DALI_TEST_EQUALS( child_2_accessible -> GetAddress(), TestGetChildAtIndex( parent_accessible -> GetAddress(), TestGetIndexInParent( child_2_accessible -> GetAddress() ) ), TEST_LOCATION );

  DALI_TEST_EQUALS( parent_accessible,  child_1_accessible -> GetParent(), TEST_LOCATION );
  DALI_TEST_EQUALS( child_2_accessible, parent_accessible -> GetChildAtIndex( child_2_accessible -> GetIndexInParent() ) , TEST_LOCATION );

  Dali::Accessibility::TestEnableSC( false );

  END_TEST;
}

int UtcDaliAccessibilityGetLayer(void)
{
  ToolkitTestApplication application;

  Dali::Accessibility::TestEnableSC( true );

  auto control = Control::New();
  auto accessible_obj = Dali::Accessibility::Accessible::Get( control );
  auto accessible_component = dynamic_cast<Dali::Accessibility::Component*>(accessible_obj);
  DALI_TEST_CHECK( accessible_component );
  DALI_TEST_EQUALS( Dali::Accessibility::ComponentLayer::WINDOW, accessible_component -> GetLayer(), TEST_LOCATION );
  DALI_TEST_EQUALS( Dali::Accessibility::ComponentLayer::WINDOW, TestGetLayer( accessible_component -> GetAddress() ), TEST_LOCATION );

  Dali::Accessibility::TestEnableSC( false );

  END_TEST;
}

int UtcDaliAccessibilityGrabFocus(void)
{
  ToolkitTestApplication application;

  Dali::Accessibility::TestEnableSC( true );

  auto controla = Control::New();
  auto controlb = Control::New();

  Stage::GetCurrent().Add( controla );
  Stage::GetCurrent().Add( controlb );

  controla.SetProperty(Actor::Property::KEYBOARD_FOCUSABLE, true);
  controlb.SetProperty(Actor::Property::KEYBOARD_FOCUSABLE, true);

  auto a = Dali::Accessibility::Accessible::Get( controla );
  auto b = Dali::Accessibility::Accessible::Get( controlb );

  auto a_component = dynamic_cast<Dali::Accessibility::Component*>( a );
  auto b_component = dynamic_cast<Dali::Accessibility::Component*>( b );

  auto states_by_bridge_a = Dali::Accessibility::States { TestGetStates( a_component -> GetAddress() )};
  auto states_by_bridge_b = Dali::Accessibility::States { TestGetStates( b_component -> GetAddress() )};

  DALI_TEST_CHECK( !states_by_bridge_a[ Dali::Accessibility::State::FOCUSED ] );
  DALI_TEST_CHECK( !states_by_bridge_b[ Dali::Accessibility::State::FOCUSED ] );

  DALI_TEST_CHECK( a_component -> GrabFocus() );

  states_by_bridge_a = Dali::Accessibility::States { TestGetStates( a_component -> GetAddress() )};
  states_by_bridge_b = Dali::Accessibility::States { TestGetStates( b_component -> GetAddress() )};

  DALI_TEST_CHECK( states_by_bridge_a[ Dali::Accessibility::State::FOCUSED ] );
  DALI_TEST_CHECK( !states_by_bridge_b[ Dali::Accessibility::State::FOCUSED ] );

  DALI_TEST_CHECK( TestGrabFocus( b_component -> GetAddress() ) );

  states_by_bridge_a = Dali::Accessibility::States { TestGetStates( a_component -> GetAddress() )};
  states_by_bridge_b = Dali::Accessibility::States { TestGetStates( b_component -> GetAddress() )};

  DALI_TEST_CHECK( !states_by_bridge_a[ Dali::Accessibility::State::FOCUSED ] );
  DALI_TEST_CHECK( states_by_bridge_b[ Dali::Accessibility::State::FOCUSED ] );

  Dali::Accessibility::TestEnableSC( false );

  END_TEST;
}

int UtcDaliAccessibilityGetExtents(void)
{
  ToolkitTestApplication application;

  Dali::Accessibility::TestEnableSC( true );

  auto control = Control::New();
  Stage::GetCurrent().GetRootLayer().Add( control );

  control.SetProperty(Actor::Property::POSITION, Vector3(10, 10, 100));
  control.SetProperty(Actor::Property::SIZE, Vector2(10, 10));

  application.SendNotification();
  application.Render( 1 );

  auto a = Dali::Accessibility::Accessible::Get( control );
  auto a_component = dynamic_cast<Dali::Accessibility::Component*>( a );

  auto extents = a_component->GetExtents(Dali::Accessibility::CoordType::SCREEN);
  DALI_TEST_EQUALS( extents.x, 5.0f, TEST_LOCATION );
  DALI_TEST_EQUALS( extents.y, 5.0f, TEST_LOCATION );
  DALI_TEST_EQUALS( extents.height, 10.0f, TEST_LOCATION );
  DALI_TEST_EQUALS( extents.width, 10.0f, TEST_LOCATION );

  auto bridge_extents = TestGetExtents( a_component -> GetAddress() );
  DALI_TEST_EQUALS( std::get< 0 >( bridge_extents ), 5, TEST_LOCATION );
  DALI_TEST_EQUALS( std::get< 1 >( bridge_extents ), 5, TEST_LOCATION );
  DALI_TEST_EQUALS( std::get< 2 >( bridge_extents ), 10, TEST_LOCATION );
  DALI_TEST_EQUALS( std::get< 3 >( bridge_extents ), 10, TEST_LOCATION );

  control.SetProperty( Dali::DevelActor::Property::POSITION_USES_ANCHOR_POINT, false );
  application.SendNotification();
  application.Render( 1 );

  extents = a_component->GetExtents(Dali::Accessibility::CoordType::SCREEN);
  DALI_TEST_EQUALS( extents.x, 10.0f, TEST_LOCATION );
  DALI_TEST_EQUALS( extents.y, 10.0f, TEST_LOCATION );
  DALI_TEST_EQUALS( extents.height, 10.0f, TEST_LOCATION );
  DALI_TEST_EQUALS( extents.width, 10.0f, TEST_LOCATION );

  bridge_extents = TestGetExtents( a_component -> GetAddress() );
  DALI_TEST_EQUALS( std::get< 0 >( bridge_extents ), 10, TEST_LOCATION );
  DALI_TEST_EQUALS( std::get< 1 >( bridge_extents ), 10, TEST_LOCATION );
  DALI_TEST_EQUALS( std::get< 2 >( bridge_extents ), 10, TEST_LOCATION );
  DALI_TEST_EQUALS( std::get< 3 >( bridge_extents ), 10, TEST_LOCATION );

  Dali::Accessibility::TestEnableSC( false );

  END_TEST;
}

int UtcDaliAccessibilityGetAlpha(void)
{
  ToolkitTestApplication application;

  Dali::Accessibility::TestEnableSC( true );

  auto control = Control::New();
  auto a = Dali::Accessibility::Accessible::Get( control );
  auto a_component = dynamic_cast<Dali::Accessibility::Component*>( a );

  DALI_TEST_EQUALS( 0.0, a_component -> GetAlpha(), TEST_LOCATION );
  DALI_TEST_EQUALS( 0.0, TestGetAlpha( a_component -> GetAddress() ), TEST_LOCATION );

  Dali::Accessibility::TestEnableSC( false );

  END_TEST;
}

int UtcDaliAccessibilityGetMdiZOrder(void)
{
  ToolkitTestApplication application;

  Dali::Accessibility::TestEnableSC( true );

  auto control = Control::New();
  auto a = Dali::Accessibility::Accessible::Get( control );
  auto a_component = dynamic_cast<Dali::Accessibility::Component*>( a );

  DALI_TEST_EQUALS( 0, static_cast< int >( a_component -> GetMdiZOrder() ), TEST_LOCATION );
  DALI_TEST_EQUALS( 0, TestGetMdiZOrder( a_component -> GetAddress() ), TEST_LOCATION );

  Dali::Accessibility::TestEnableSC( false );

  END_TEST;
}

int UtcDaliAccessibilityAction(void)
{
  ToolkitTestApplication application;

  Dali::Accessibility::TestEnableSC( true );

  auto control = Control::New( );
  auto a = Dali::Accessibility::Accessible::Get( control );
  auto b = dynamic_cast<Dali::Accessibility::Action*>( a );

  std::vector< std::string > actions { "activate", "accessibilityActivated", "ReadingSkipped", "ReadingCancelled", "ReadingStopped", "show", "hide" };
  auto count = b -> GetActionCount();

  DALI_TEST_EQUALS( count, 7, TEST_LOCATION );

  for (auto i = 0u; i<count; ++i)
  {
    DALI_TEST_CHECK( Dali::Accessibility::Find( actions, b -> GetActionName( i ) ) );
    DALI_TEST_EQUALS( b -> GetActionName( i ), b -> GetLocalizedActionName( i ), TEST_LOCATION );
    DALI_TEST_EQUALS( b -> GetActionDescription( i ), "", TEST_LOCATION );
    DALI_TEST_EQUALS( b -> GetActionKeyBinding( i ), "", TEST_LOCATION );
  }

  try
  {
    b ->GetActionDescription( count );
    DALI_ABORT( "Correct index, abort" );
  }
  catch( std::domain_error &){}

  try
  {
    b ->GetActionName( count );
    DALI_ABORT( "Correct index, abort" );
  }
  catch( std::domain_error &){}

  try
  {
    b ->GetLocalizedActionName( count );
    DALI_ABORT( "Correct index, abort" );
  }
  catch( std::domain_error &){}

  try
  {
    b ->GetActionKeyBinding( count );
    DALI_ABORT( "Correct index, abort" );
  }
  catch( std::domain_error &){}

  count = TestGetActionCount(b -> GetAddress());

  DALI_TEST_EQUALS( count, 7, TEST_LOCATION );

  for (auto i = 0u; i<count; ++i)
  {
    DALI_TEST_CHECK( Dali::Accessibility::Find( actions, TestGetActionName( b->GetAddress(), i ) ) );
    DALI_TEST_EQUALS( TestGetActionName( b->GetAddress(), i ), TestGetLocalizedActionName( b->GetAddress(), i ), TEST_LOCATION );
    DALI_TEST_EQUALS( TestGetActionDescription( b->GetAddress(), i ), "", TEST_LOCATION );
    DALI_TEST_EQUALS( TestGetActionKeyBinding( b->GetAddress(), i ), "", TEST_LOCATION );
  }

  try
  {
    TestGetActionDescription( b->GetAddress(), count );
    DALI_ABORT( "Correct index, abort" );
  }
  catch( TestDBusWrapper::error& ){}

  try
  {
    TestGetActionName( b->GetAddress(), count );
    DALI_ABORT( "Correct index, abort" );
  }
  catch( TestDBusWrapper::error& ){}

  try
  {
    TestGetLocalizedActionName( b->GetAddress(), count );
    DALI_ABORT( "Correct index, abort" );
  }
  catch( TestDBusWrapper::error& ){}

  try
  {
    TestGetActionKeyBinding( b->GetAddress(), count );
    DALI_ABORT( "Correct index, abort" );
  }
  catch( TestDBusWrapper::error& ){}


  Dali::Accessibility::TestEnableSC( false );

  END_TEST;
}

int UtcDaliAccessibilityDoAction(void)
{
  ToolkitTestApplication application;

  Dali::Accessibility::TestEnableSC( true );
  thread_local std::vector< bool > actions_done { false, false, false, false };

  auto control = Control::New(  );
  auto a = Dali::Accessibility::Accessible::Get( control );
  auto b = dynamic_cast<Dali::Accessibility::Action*>( a );
  std::vector< std::string > actions { "activate", "accessibilityActivated", "ReadingSkipped", "ReadingCancelled", "ReadingStopped", "show", "hide" };

  DALI_TEST_CHECK( b -> DoAction( actions[2] ) );
  DALI_TEST_CHECK( b -> DoAction( actions[4] ) );
  DALI_TEST_CHECK( b -> DoAction( actions[4] ) );

  DevelControl::AccessibilityReadingSkippedSignal(control).Connect( [] () {
    actions_done[ 1 ] = true;
  } );
  DevelControl::AccessibilityReadingCancelledSignal(control).Connect( [] () {
    actions_done[ 2 ] = true;
  } );
  DevelControl::AccessibilityReadingStoppedSignal(control).Connect( [] () {
    actions_done[ 3 ] = true;
  } );
  DevelControl::AccessibilityActivateSignal(control).Connect( [] () {
    actions_done[ 0 ] = true;
  } );

  DALI_TEST_CHECK( b -> DoAction( 1 ) );
  DALI_TEST_CHECK( b -> DoAction( 2 ) );
  DALI_TEST_CHECK( b -> DoAction( 3 ) );
  DALI_TEST_CHECK( b -> DoAction( 4 ) );

  for ( auto i = 0u; i < actions_done.size(); ++i )
    {
      DALI_TEST_CHECK( actions_done[ i ] );
      actions_done[ i ] = false;
    }

  DALI_TEST_CHECK( TestDoAction( b -> GetAddress(), 1 ) );
  DALI_TEST_CHECK( TestDoAction( b -> GetAddress(), 2 ) );
  DALI_TEST_CHECK( TestDoAction( b -> GetAddress(), 3 ) );
  DALI_TEST_CHECK( TestDoAction( b -> GetAddress(), 4 ) );

  for ( auto i = 0u; i < actions_done.size(); ++i )
    {
      DALI_TEST_CHECK( actions_done[ i ] );
      actions_done[ i ] = false;
    }

  DALI_TEST_CHECK( TestDoAction( b -> GetAddress(), actions[ 1 ] ) );
  DALI_TEST_CHECK( TestDoAction( b -> GetAddress(), actions[ 2 ] ) );
  DALI_TEST_CHECK( TestDoAction( b -> GetAddress(), actions[ 3 ] ) );
  DALI_TEST_CHECK( TestDoAction( b -> GetAddress(), actions[ 4 ] ) );

  for ( auto i = 0u; i < actions_done.size(); ++i )
      DALI_TEST_CHECK( actions_done[ i ] );

  Dali::Accessibility::TestEnableSC( false );

  END_TEST;
}
