/*! \page text-input Text Input
 *
 TextInput is a Dali::Actor which allows the input of text from an on-screen virtual keyboard or attached hardware keyboard.


<h2 class="pg">Basic Text Input Set-up</h2>

 The below code creates a new TextInput

   @code
   Dali::Toolkit::TextInput myTextInput;
   myTextInput = Dali::Toolkit::TextInput::New();
   @endcode

 The following code sets the size and adds it to the stage
   @code
   myTextInput.SetParentOrigin(ParentOrigin::CENTER);
   myTextInput.SetSize(stageWidth*0.25f, stageWidth*0.5f);
   Stage::GetCurrent().Add(myTextInput);
   @endcode

 For a TextInput to receive input from the keyboard it must be in edit mode.

 To enter edit mode the below call can be made.  If the virtual on-screen keyboard is supported then it will be displayed.
 Internally TextInput will set focus to this TextInput and key events will be sent to it.

   @code myTextInput.SetEditable(true);@endcode

 After this call the TextInput will receive any key press. If you have more than one TextInput the focus will change between them when the edit mode is
 initiated on any Text Input.

 To automatically start edit mode when the TextInput is "tapped" you can call the following:

   @code myTextInput.SetEditOnTouch()@endcode

 You will then need to end edit mode by making the call below or swiping away the keyboard (Virtual On-screen Keyboard)
   @code myTextInput.SetEditable(false);@endcode

 The call will hide the virtual keyboard if previously shown by Text Input.

 Then the input string as plain text can be retrieved using
   @code Dali::Toolkit::TextInput::GetText()@endcode

<h2 class="pg"> Text Selection </h2>

 The SetTextSelectable API when set to true enables text to be highlighted, once highlighted the text style can be changed,
 the text can be cut, or copied, overwritten with new text or deleted.

 The user does a Long-Press on the text to get the option of text selection.

   @code Dali::Toolkit::TextInput::SetTextSelectable( true ) @endcode

<h2 class="pg"> Text Styling </h2>

 In conjunction with TextView and the Markup processor, TextInput enables text to be styled.

 There are 3 ways to effect the text styling.

   SetActiveStyle, new input text is set to the Red glow style
   @code
   TextStyle style;
   style.SetGlow ( true, Dali::Color::RED );
   myTextInput.SetActiveStyle( style, MarkupProcessor::GLOW );
   @endcode

   ApplyStyle, selected/highlighted text now has the Red glow style
   @code
   TextStyle style;
   style.SetGlow ( true, Dali::Color::RED );
   myTextInput.ApplyStyle( style, MarkupProcessor::GLOW );
   @endcode

   ApplyStyleToAll, all text now has the Red glow style
   @code
   TextStyle style;
   style.SetGlow ( true, Dali::Color::RED );
   myTextInput.ApplyStyleToAll( style, MarkupProcessor::GLOW );
   @endcode

 Then the input string with Mark-up defining the style can be retrieved using
   @code Dali::Toolkit::TextInput::GetMarkupText()@endcode
 This would be usefull if you wish to save the styled text the user has input so it can be re-displayed another time.

 Signals are emitted when style changes.

 See Dali::Toolkit::TextInput::StyleMask for available styling options.




 */

