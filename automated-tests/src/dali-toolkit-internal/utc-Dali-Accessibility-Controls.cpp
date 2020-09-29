#include <dali-toolkit-test-suite-utils.h>
#include <automated-tests/src/dali-toolkit-internal/dali-toolkit-test-utils/accessibility-test-utils.h>
#include <dali-toolkit/dali-toolkit.h>

#include <dali-toolkit/devel-api/controls/control-devel.h>
#include <dali-toolkit/devel-api/controls/table-view/table-view.h>
#include <dali/devel-api/adaptor-framework/accessibility-impl.h>

#include <automated-tests/src/dali-toolkit-internal/dali-toolkit-test-utils/dbus-wrapper.h>

using namespace Dali::Toolkit;

void utc_dali_accessibility_controls_startup(void)
{
  test_return_value = TET_UNDEF;
  DBusWrapper::Install(std::unique_ptr<DBusWrapper>(new TestDBusWrapper));
}

void utc_dali_accessibility_controls_cleanup(void)
{
  test_return_value = TET_PASS;
  //DBusWrapper::Install({}) is a de-install
  DBusWrapper::Install({});
}

int UtcDaliControlPropertyAccessibilityTranslationDomain(void)
{
  ToolkitTestApplication application;

  auto control = Control::New();

  auto accessibility_translation_domain = DevelControl::Property::ACCESSIBILITY_TRANSLATION_DOMAIN;
  DALI_TEST_EQUALS( Property::NONE , control.GetProperty( accessibility_translation_domain ).GetType(), TEST_LOCATION );

  control.SetProperty( accessibility_translation_domain, "translation_domain_test_1" );
  DALI_TEST_EQUALS( "translation_domain_test_1" , control.GetProperty( accessibility_translation_domain ).Get<  std::string  >(), TEST_LOCATION );

  control.SetProperty( accessibility_translation_domain, "translation_domain_test_2" );
  DALI_TEST_EQUALS( "translation_domain_test_2" , control.GetProperty( accessibility_translation_domain ).Get<  std::string  >(), TEST_LOCATION );

  END_TEST;
}

// This test shows that when the accessibility bridge is
// not up, there is no possibility to grab or clear highlight
int UtcDaliControlAccessibilityHighlight(void)
{
  ToolkitTestApplication application;
  auto controla = Control::New();
  auto controlb = Control::New();

  DALI_TEST_EQUALS( false, DevelControl::GrabAccessibilityHighlight(controla), TEST_LOCATION );
  DALI_TEST_EQUALS( false, DevelControl::GrabAccessibilityHighlight(controlb), TEST_LOCATION );
  DALI_TEST_EQUALS( false, DevelControl::ClearAccessibilityHighlight(controla), TEST_LOCATION );
  DALI_TEST_EQUALS( false, DevelControl::ClearAccessibilityHighlight(controlb), TEST_LOCATION );

  END_TEST;
}

#include <dali-toolkit/devel-api/controls/tool-bar/tool-bar.h>
int UtcDaliAccessibilityToolBarConstructor(void)
{
  ToolkitTestApplication application;

  auto toolbar = ToolBar::New();
  DALI_TEST_CHECK( toolbar );

  auto accessible = Dali::Accessibility::Accessible::Get( toolbar );
  DALI_TEST_CHECK( accessible );
  DALI_TEST_EQUALS( accessible->GetRole(), Accessibility::Role::TOOL_BAR, TEST_LOCATION );

  END_TEST;
}

int UtcDaliAccessibilityPushButtonConstructor(void)
{
  ToolkitTestApplication application;

  auto pushbutton = PushButton::New();
  DALI_TEST_CHECK( pushbutton );

  auto accessible = Dali::Accessibility::Accessible::Get( pushbutton );
  DALI_TEST_CHECK( accessible );
  DALI_TEST_EQUALS( accessible->GetRole(), Accessibility::Role::PUSH_BUTTON, TEST_LOCATION );

  END_TEST;
}

int UtcDaliAccessibilityPushButtonStates(void)
{
  ToolkitTestApplication application;

  auto pushbutton = PushButton::New();
  DALI_TEST_CHECK( pushbutton );

  auto accessible = Dali::Accessibility::Accessible::Get( pushbutton );
  DALI_TEST_CHECK( accessible );

  Dali::Accessibility::TestEnableSC( true );

  auto states = accessible->GetStates();
  DALI_TEST_EQUALS( static_cast< unsigned int >( states[ Accessibility::State::PRESSED ] ), false, TEST_LOCATION );

  // auto button = dynamic_cast<Dali::Toolkit::Button* >( accessible ) ;
  pushbutton.SetProperty( Toolkit::Button::Property::TOGGLABLE, true );
  pushbutton.SetProperty( Toolkit::Button::Property::SELECTED, true );

  states = accessible->GetStates();
  DALI_TEST_EQUALS( static_cast< unsigned int >( states[ Accessibility::State::PRESSED ] ), true, TEST_LOCATION );

  Dali::Accessibility::TestEnableSC( false );

  END_TEST;
}

#include <dali-toolkit/devel-api/controls/buttons/toggle-button.h>
int UtcDaliAccessibilityToggleButtonConstructor(void)
{
  ToolkitTestApplication application;

  auto togglebutton = ToggleButton::New();
  DALI_TEST_CHECK( togglebutton );

  auto accessible = Dali::Accessibility::Accessible::Get( togglebutton );
  DALI_TEST_CHECK( accessible );
  DALI_TEST_EQUALS( accessible->GetRole(), Accessibility::Role::TOGGLE_BUTTON, TEST_LOCATION );

  END_TEST;
}

#include <dali-toolkit/devel-api/controls/text-controls/text-selection-popup.h>
int UtcDaliAccessibilityTextSelectionPopupConstructor(void)
{
  ToolkitTestApplication application;

  auto textselectionpopup = TextSelectionPopup::New( NULL );
  DALI_TEST_CHECK( textselectionpopup );

  auto accessible = Dali::Accessibility::Accessible::Get( textselectionpopup );
  DALI_TEST_CHECK( accessible );
  DALI_TEST_EQUALS( accessible->GetRole(), Accessibility::Role::DIALOG, TEST_LOCATION );

  END_TEST;
}

int UtcDaliAccessibilityAlignmentConstructor(void)
{
  ToolkitTestApplication application;

  auto alignment = Alignment::New();
  DALI_TEST_CHECK( alignment );

  auto accessible = Dali::Accessibility::Accessible::Get( alignment );
  DALI_TEST_CHECK( accessible );
  DALI_TEST_EQUALS( accessible->GetRole(), Accessibility::Role::FILLER, TEST_LOCATION );

  END_TEST;
}

int UtcDaliAccessibilityRadioButtonStates(void)
{
  ToolkitTestApplication application;

  auto radiobutton = RadioButton::New();
  DALI_TEST_CHECK( radiobutton );

  auto accessible = Dali::Accessibility::Accessible::Get( radiobutton );
  DALI_TEST_CHECK( accessible );

  Dali::Accessibility::TestEnableSC( true );

  auto states = accessible->GetStates();
  DALI_TEST_CHECK( states );
  DALI_TEST_EQUALS( static_cast< unsigned int >( states[ Accessibility::State::CHECKED ] ), false, TEST_LOCATION );
  radiobutton.SetProperty( Toolkit::RadioButton::Property::SELECTED, true );
  states = accessible->GetStates();
  DALI_TEST_CHECK( states );
  DALI_TEST_EQUALS( static_cast< unsigned int >( states[ Accessibility::State::CHECKED ] ), true, TEST_LOCATION );

  Dali::Accessibility::TestEnableSC( false );

  END_TEST;
}

int UtcDaliAccessibilityFlexContainerConstructor(void)
{
  ToolkitTestApplication application;

  auto flexcontainer = FlexContainer::New();
  DALI_TEST_CHECK( flexcontainer );

  auto accessible = Dali::Accessibility::Accessible::Get( flexcontainer );
  DALI_TEST_CHECK( accessible );
  DALI_TEST_EQUALS( accessible->GetRole(), Accessibility::Role::FILLER, TEST_LOCATION );

  END_TEST;
}

int UtcDaliAccessibilityCheckBoxButton(void)
{
  ToolkitTestApplication application;

  auto checkboxbutton = CheckBoxButton::New();
  DALI_TEST_CHECK( checkboxbutton );

  auto accessible = Dali::Accessibility::Accessible::Get( checkboxbutton );
  DALI_TEST_CHECK( accessible );

  Dali::Accessibility::TestEnableSC( true );

  auto states = accessible->GetStates();
  DALI_TEST_EQUALS( static_cast< unsigned int >( states[ Accessibility::State::CHECKED ] ), false, TEST_LOCATION );
  checkboxbutton.SetProperty( Toolkit::CheckBoxButton::Property::SELECTED, true );
  states = accessible->GetStates();
  DALI_TEST_EQUALS( static_cast< unsigned int >( states[ Accessibility::State::CHECKED ] ), true, TEST_LOCATION );

  Dali::Accessibility::TestEnableSC( false );

  END_TEST;
}

#include <dali-toolkit/devel-api/controls/text-controls/text-selection-toolbar.h>
int UtcDaliAccessibilityTextSelectionConstructor(void)
{
  ToolkitTestApplication application;

  auto textselectiontoolbar = TextSelectionToolbar::New();
  DALI_TEST_CHECK( textselectiontoolbar );

  auto accessible = Dali::Accessibility::Accessible::Get( textselectiontoolbar );
  DALI_TEST_CHECK( accessible );
  DALI_TEST_EQUALS( accessible->GetRole(), Accessibility::Role::TOOL_BAR, TEST_LOCATION );

  END_TEST;
}

#include <dali-toolkit/internal/accessibility-manager/accessibility-manager-impl.h>
int UtcDaliAccessibilityManager(void)
{
  ToolkitTestApplication application;

  auto accessmanager = new Dali::Toolkit::Internal::AccessibilityManager;
  auto actor = Dali::Actor::New();

  auto str = accessmanager->GetAccessibilityAttribute( actor, Toolkit::AccessibilityManager::AccessibilityAttribute::ACCESSIBILITY_LABEL );
  DALI_TEST_EQUALS( str, "", TEST_LOCATION );
  DALI_TEST_EQUALS( accessmanager->GetFocusOrder( actor ), 0, TEST_LOCATION );
  DALI_TEST_EQUALS( accessmanager->GenerateNewFocusOrder(), 1, TEST_LOCATION );
  accessmanager->GetActorByFocusOrder( 0 );
  accessmanager->GetCurrentFocusActor();
  accessmanager->GetCurrentFocusGroup();
  DALI_TEST_EQUALS( accessmanager->GetCurrentFocusOrder(), 0, TEST_LOCATION );
  DALI_TEST_EQUALS( accessmanager->MoveFocusForward(), false, TEST_LOCATION );
  DALI_TEST_EQUALS( accessmanager->MoveFocusBackward(), false, TEST_LOCATION );
  DALI_TEST_EQUALS( accessmanager->IsFocusGroup( actor ), false, TEST_LOCATION );
  accessmanager->GetFocusGroup( actor );
  auto vector = accessmanager->GetReadPosition();
  DALI_TEST_EQUALS( vector.x, 0.0f, TEST_LOCATION );
  DALI_TEST_EQUALS( vector.y, 0.0f, TEST_LOCATION );
  DALI_TEST_EQUALS( accessmanager->GetGroupMode(), false, TEST_LOCATION );
  DALI_TEST_EQUALS( accessmanager->GetWrapMode(), true, TEST_LOCATION );
  accessmanager->GetFocusIndicatorActor();

  END_TEST;
}

int UtcDaliAccessibilityModel3dViewConstructor(void)
{
  ToolkitTestApplication application;

  auto model3dview = Model3dView::New();
  DALI_TEST_CHECK( model3dview );

  auto accessible = Dali::Accessibility::Accessible::Get( model3dview );
  DALI_TEST_CHECK( accessible );
  DALI_TEST_EQUALS( accessible->GetRole(), Accessibility::Role::IMAGE, TEST_LOCATION );

  END_TEST;
}

#include <dali-toolkit/internal/controls/effects-view/effects-view-impl.h>
int UtcDaliAccessibilityEffectsViewConstructor(void)
{
  ToolkitTestApplication application;

  auto etype = Dali::Toolkit::EffectsView::EffectType::DROP_SHADOW;
  auto effectsview = EffectsView::New( etype );
  DALI_TEST_CHECK( effectsview );

  auto accessible = Dali::Accessibility::Accessible::Get( effectsview );
  DALI_TEST_CHECK( accessible );
  DALI_TEST_EQUALS( accessible->GetRole(), Accessibility::Role::FILLER, TEST_LOCATION );

  END_TEST;
}

#include <dali-toolkit/internal/controls/super-blur-view/super-blur-view-impl.h>
int UtcDaliAccessibilitySuperBlurViewConstructor(void)
{
  ToolkitTestApplication application;

  auto superblurview = SuperBlurView::New( 1 );
  DALI_TEST_CHECK( superblurview );

  auto accessible = Dali::Accessibility::Accessible::Get( superblurview );
  DALI_TEST_CHECK( accessible );
  DALI_TEST_EQUALS( accessible->GetRole(), Accessibility::Role::FILLER, TEST_LOCATION );

  END_TEST;
}

int UtcDaliAccessibilityImageViewConstructor(void)
{
  ToolkitTestApplication application;

  auto imageview = ImageView::New();
  DALI_TEST_CHECK( imageview );

  auto accessible = Dali::Accessibility::Accessible::Get( imageview );
  DALI_TEST_CHECK( accessible );
  DALI_TEST_EQUALS( accessible->GetRole(), Accessibility::Role::IMAGE, TEST_LOCATION );

  END_TEST;
}

#include <dali-toolkit/devel-api/controls/page-turn-view/page-factory.h>
class TestPageFactory : public PageFactory
{
public:

  TestPageFactory( bool returnValidTexture = true )
  : mValidTexture( returnValidTexture )
  {
    mTotalPageNumber = 100;
  }

  /**
   * Query the number of pages available from the factory.
   * The maximum available page has an ID of GetNumberOfPages()-1.
   */
  virtual unsigned int GetNumberOfPages()
  {
    return mTotalPageNumber;
  }

  /**
   * Create an texture to represent a page content.
   * @param[in] pageId The ID of the page to create.
   * @return An image, or an empty handle if the ID is out of range.
   */
  virtual Texture NewPage( unsigned int pageId )
  {
    if( mValidTexture )
    {
      return Texture::New( Dali::TextureType::TEXTURE_2D, Pixel::RGB888, 100, 100 );
    }
    return Texture(); // empty handle
  }

private:
  unsigned int            mTotalPageNumber;
  bool                    mValidTexture;
};

#include <dali-toolkit/internal/controls/page-turn-view/page-turn-landscape-view-impl.h>
int UtcDaliAccessibilityPageTurnViewConstructor(void)
{
  ToolkitTestApplication application;

  auto testpagefactory = TestPageFactory();
  auto vector2 = Vector2( 1.0, 1.0 );
  auto pageturnlandscapeview = PageTurnLandscapeView::New( testpagefactory, vector2 );
  DALI_TEST_CHECK( pageturnlandscapeview );

  auto accessible = Dali::Accessibility::Accessible::Get( pageturnlandscapeview );
  DALI_TEST_CHECK( accessible );
  DALI_TEST_EQUALS( accessible->GetRole(), Accessibility::Role::PAGE_TAB_LIST, TEST_LOCATION );

  END_TEST;
}

int UtcDaliAccessibilityGaussianBlurViewConstructor(void)
{
  ToolkitTestApplication application;

  auto gaussianblurview = GaussianBlurView::New();
  DALI_TEST_CHECK( gaussianblurview );

  auto accessible = Dali::Accessibility::Accessible::Get( gaussianblurview );
  DALI_TEST_CHECK( accessible );
  DALI_TEST_EQUALS( accessible->GetRole(), Accessibility::Role::FILLER, TEST_LOCATION );

  END_TEST;
}

int UtcDaliAccessibilityShadowViewConstructor(void)
{
  ToolkitTestApplication application;

  auto shadowview = ShadowView::New();
  DALI_TEST_CHECK( shadowview );

  auto accessible = Dali::Accessibility::Accessible::Get( shadowview );
  DALI_TEST_CHECK( accessible );
  DALI_TEST_EQUALS( accessible->GetRole(), Accessibility::Role::FILLER, TEST_LOCATION );

  END_TEST;
}

#include <dali-toolkit/public-api/controls/control-impl.h>
#include <dali-toolkit/internal/controls/scrollable/scroll-view/scroll-view-impl.h>
int UtcDaliAccessibilityScrollableConstructor(void)
{
  ToolkitTestApplication application;

  auto scrollview = ScrollView::New();
  DALI_TEST_CHECK( scrollview );

  auto accessible = Dali::Accessibility::Accessible::Get( scrollview );
  DALI_TEST_CHECK( accessible );
  DALI_TEST_EQUALS( accessible->GetRole(), Accessibility::Role::SCROLL_PANE, TEST_LOCATION );

  END_TEST;
}

#include <dali-toolkit/internal/controls/magnifier/magnifier-impl.h>
int UtcDaliAccessibilityMagnifierConstructor(void)
{
  ToolkitTestApplication application;

  auto magnifier = Magnifier::New();
  DALI_TEST_CHECK( magnifier );

  auto accessible = Dali::Accessibility::Accessible::Get( magnifier );
  DALI_TEST_CHECK( accessible );
  DALI_TEST_EQUALS( accessible->GetRole(), Accessibility::Role::FILLER, TEST_LOCATION );

  END_TEST;
}

int UtcDaliAccessibilityTableViewConstructor(void)
{
  ToolkitTestApplication application;

  auto tableview = TableView::New( 10, 10 );
  DALI_TEST_CHECK( tableview );

  auto accessible = Dali::Accessibility::Accessible::Get( tableview );
  DALI_TEST_CHECK( accessible );
  DALI_TEST_EQUALS( accessible->GetRole(), Accessibility::Role::TABLE, TEST_LOCATION );

  END_TEST;
}

#include <dali-toolkit/internal/controls/bloom-view/bloom-view-impl.h>
int UtcDaliAccessibilityBloomViewConstructor(void)
{
  ToolkitTestApplication application;

  auto bloomview = BloomView::New();
  DALI_TEST_CHECK( bloomview );

  auto accessible = Dali::Accessibility::Accessible::Get( bloomview );
  DALI_TEST_CHECK( accessible );
  DALI_TEST_EQUALS( accessible->GetRole(), Accessibility::Role::ANIMATION, TEST_LOCATION );

  END_TEST;
}

#include <dali-toolkit/internal/controls/text-controls/text-field-impl.h>
int UtcDaliAccessibilityTextField(void)
{
  ToolkitTestApplication application;

  auto textfield = TextField::New();
  DALI_TEST_CHECK( textfield );

  textfield.SetProperty(Actor::Property::NAME, "test" );
  DALI_TEST_EQUALS( textfield.GetProperty<std::string>(Actor::Property::NAME), "test", TEST_LOCATION );

  auto accessible = Dali::Accessibility::Accessible::Get( textfield );
  DALI_TEST_CHECK( accessible );

  DALI_TEST_EQUALS( accessible->GetName(), "", TEST_LOCATION );
  DALI_TEST_EQUALS( accessible->GetRole(), Accessibility::Role::ENTRY, TEST_LOCATION );
  auto states = accessible->GetStates();
  DALI_TEST_EQUALS( static_cast< unsigned int >( states[ Accessibility::State::EDITABLE ] ), true, TEST_LOCATION );

  Dali::Accessibility::TestEnableSC( true );

  textfield.SetProperty( Toolkit::TextField::Property::TEXT, "test" );
  auto text = dynamic_cast< Dali::Accessibility::Text* >( accessible );
  DALI_TEST_CHECK( text );
  DALI_TEST_EQUALS( text->GetText( 0, 10 ), "", TEST_LOCATION );
  DALI_TEST_EQUALS( text->SetCaretOffset(100), false, TEST_LOCATION );
  DALI_TEST_EQUALS( text->SetCaretOffset(2), true, TEST_LOCATION );
  DALI_TEST_EQUALS( text->GetCaretOffset(), 2, TEST_LOCATION );

  auto editabletext = dynamic_cast< Dali::Accessibility::EditableText* >( accessible );
  DALI_TEST_CHECK( editabletext );
  DALI_TEST_EQUALS( editabletext->CopyText( 3, 1 ), false, TEST_LOCATION );
  DALI_TEST_EQUALS( editabletext->CopyText( 1, 3 ), true, TEST_LOCATION );
  DALI_TEST_EQUALS( editabletext->CutText( 3, 1 ), false, TEST_LOCATION );
  DALI_TEST_EQUALS( editabletext->CutText( 1, 3 ), true, TEST_LOCATION );
  DALI_TEST_EQUALS( text->GetText( 0, 1 ), "t", TEST_LOCATION );

  auto range = text->GetSelection( 1 );
  DALI_TEST_EQUALS( range.startOffset, 0, TEST_LOCATION );
  DALI_TEST_EQUALS( range.endOffset, 0, TEST_LOCATION );
  DALI_TEST_EQUALS( range.content, "", TEST_LOCATION );
  DALI_TEST_EQUALS( text->SetSelection( 1, 0, 1 ), false, TEST_LOCATION );
  DALI_TEST_EQUALS( text->RemoveSelection( 1 ), false, TEST_LOCATION );

  Dali::Accessibility::TestEnableSC( false );

  END_TEST;
}

#include <dali-toolkit/internal/controls/text-controls/text-editor-impl.h>
int UtcDaliAccessibilityTextEditor(void)
{
  ToolkitTestApplication application;

  auto texteditor = TextEditor::New();
  DALI_TEST_CHECK( texteditor );

  texteditor.SetProperty(Actor::Property::NAME, "test" );
  DALI_TEST_EQUALS( texteditor.GetProperty<std::string>(Actor::Property::NAME), "test", TEST_LOCATION );

  auto accessible = Dali::Accessibility::Accessible::Get( texteditor );
  DALI_TEST_CHECK( accessible );

  DALI_TEST_EQUALS( accessible->GetName(), "", TEST_LOCATION );
  DALI_TEST_EQUALS( accessible->GetRole(), Accessibility::Role::ENTRY, TEST_LOCATION );
  auto states = accessible->GetStates();
  DALI_TEST_EQUALS( static_cast< unsigned int >( states[ Accessibility::State::EDITABLE ] ), true, TEST_LOCATION );

  Dali::Accessibility::TestEnableSC( true );

  texteditor.SetProperty( Toolkit::TextEditor::Property::TEXT, "test" );
  auto text = dynamic_cast< Dali::Accessibility::Text* >( accessible );
  DALI_TEST_CHECK( text );
  DALI_TEST_EQUALS( text->GetText( 0, 10 ), "", TEST_LOCATION );
  DALI_TEST_EQUALS( text->SetCaretOffset(100), false, TEST_LOCATION );
  DALI_TEST_EQUALS( text->SetCaretOffset(2), true, TEST_LOCATION );
  DALI_TEST_EQUALS( text->GetCaretOffset(), 2, TEST_LOCATION );

  auto editabletext = dynamic_cast< Dali::Accessibility::EditableText* >( accessible );
  DALI_TEST_CHECK( editabletext );
  DALI_TEST_EQUALS( editabletext->CopyText( 3, 1 ), false, TEST_LOCATION );
  DALI_TEST_EQUALS( editabletext->CopyText( 1, 3 ), true, TEST_LOCATION );
  DALI_TEST_EQUALS( editabletext->CutText( 3, 1 ), false, TEST_LOCATION );
  DALI_TEST_EQUALS( editabletext->CutText( 1, 3 ), true, TEST_LOCATION );
  DALI_TEST_EQUALS( text->GetText( 0, 1 ), "t", TEST_LOCATION );

  auto range = text->GetSelection( 1 );
  DALI_TEST_EQUALS( range.startOffset, 0, TEST_LOCATION );
  DALI_TEST_EQUALS( range.endOffset, 0, TEST_LOCATION );
  DALI_TEST_EQUALS( range.content, "", TEST_LOCATION );
  DALI_TEST_EQUALS( text->SetSelection( 1, 0, 1 ), false, TEST_LOCATION );
  DALI_TEST_EQUALS( text->RemoveSelection( 1 ), false, TEST_LOCATION );

  Dali::Accessibility::TestEnableSC( false );

  END_TEST;
}

int UtcDaliAccessibilityTextLabel(void)
{
  ToolkitTestApplication application;

  auto textlabel = TextLabel::New();
  DALI_TEST_CHECK( textlabel );

  textlabel.SetProperty(Actor::Property::NAME, "test" );
  DALI_TEST_EQUALS( textlabel.GetProperty<std::string>(Actor::Property::NAME), "test", TEST_LOCATION );

  auto accessible = Dali::Accessibility::Accessible::Get( textlabel );
  DALI_TEST_CHECK( accessible );

  DALI_TEST_EQUALS( accessible->GetName(), "test", TEST_LOCATION );
  DALI_TEST_EQUALS( accessible->GetRole(), Accessibility::Role::LABEL, TEST_LOCATION );

  Dali::Accessibility::TestEnableSC( true );

  textlabel.SetProperty( Toolkit::TextLabel::Property::TEXT, "test" );
  auto text = dynamic_cast< Dali::Accessibility::Text* >( accessible );
  DALI_TEST_CHECK( text );
  DALI_TEST_EQUALS( text->GetText( 0, 10 ), "", TEST_LOCATION );
  DALI_TEST_EQUALS( text->GetText( 0, 4 ), "test", TEST_LOCATION );
  DALI_TEST_EQUALS( text->SetCaretOffset(0), false, TEST_LOCATION );
  DALI_TEST_EQUALS( text->GetCaretOffset(), 0, TEST_LOCATION );

  auto range = text->GetSelection( 1 );
  DALI_TEST_EQUALS( range.startOffset, 0, TEST_LOCATION );
  DALI_TEST_EQUALS( range.endOffset, 0, TEST_LOCATION );
  DALI_TEST_EQUALS( range.content, "", TEST_LOCATION );
  DALI_TEST_EQUALS( text->SetSelection( 1, 0, 1 ), false, TEST_LOCATION );
  DALI_TEST_EQUALS( text->RemoveSelection( 1 ), false, TEST_LOCATION );

  Dali::Accessibility::TestEnableSC( false );

  END_TEST;
}

#include <dali-toolkit/internal/controls/navigation-view/navigation-view-impl.h>
int UtcDaliAccessibilityNavigationViewConstructor(void)
{
  ToolkitTestApplication application;

  auto navigationview = NavigationView::New();
  DALI_TEST_CHECK( navigationview );

  auto accessible = Dali::Accessibility::Accessible::Get( navigationview );
  DALI_TEST_CHECK( accessible );

  DALI_TEST_EQUALS( accessible->GetRole(), Dali::Accessibility::Role::FILLER, TEST_LOCATION );

  END_TEST;
}

int UtcDaliAccessibilityVideoViewConstructor(void)
{
  ToolkitTestApplication application;

  auto videoview = VideoView::New();
  DALI_TEST_CHECK( videoview );

  auto accessible = Dali::Accessibility::Accessible::Get( videoview );
  DALI_TEST_CHECK( accessible );

  DALI_TEST_EQUALS( accessible->GetRole(), Dali::Accessibility::Role::VIDEO, TEST_LOCATION );

  END_TEST;
}
