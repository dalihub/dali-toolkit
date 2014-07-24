/*! \page text-view Text View
 *
 * \section overview Overview
 *
 * The Dali::Toolkit::TextView class is a UI Dali::Toolkit::Control designed to extend the capabilities of the basic Dali::TextActor.
 * It provides support for multi-line wrapping, multi-language font detection, text alignment, scrolling and styling.
 *
 * Dali::Toolkit::TextView also provides text layout information which could be used in other UI Dali::Toolkit::Control classes or other applications.
 *
 * \section multiline Multi-line wrapping
 *
 * Different multi-line and exceed policies could be set to layout the given text.
 *
 * Both multi-line and exceed policies work together.
 * Dali::Toolkit::TextView::MultilinePolicy policies define how to wrap a line if it doesn't fit inside the boundary's width
 * whereas Dali::Toolkit::TextView::ExceedPolicy policies define what to do if the wrapped text is bigger than the text view's boundary.
 *
 * i.e. \e SplitByWord could be used as 'multi-line policy' to wrap a line if it's too long. If one of the words is longer than the text-view's width, \e Split could be
 *      used as 'width exceed policy' to split a word in different lines. If the text is too long and exceeds the text-view's height, \e EllipsizedEnd could be
 *      used as 'height exceed policy' to render only the text which fits inside the boundaries of the text-view.
 *
 * @see more \ref examples.
 *
 * \subsection multiline_policies Multi-line policies
 *
 * <ul>
 *   <li><em>Split by new line character</em>.
 *           Text will be wrapped when an <em>end of line \\n</em> or <em>\<br /\></em> is found.
 *
 *   <li><em>Split by word</em>.
 *           Text will be wrapped when an <em>end of line \\n</em> or <em>\<br /\></em> is found or if the text doesn't fit in the text view width.
 *           In that case, some words will be moved to a new line.
 *
 *   <li><em>Split by character</em>.
 *           Text will be wrapped when an <em>end of line \\n</em> or <em>\<br /\></em> is found or if the text doesn't fit in the text view width.
 *           In that case, words which don't fit will be wrapped in two and the remaining text moved to a new line.
 * </ul>
 * Dali::Toolkit::TextView::SplitByNewLineChar is set by default.
 *
 * \subsection exceed_policies Exceed policies
 *
 * <ul>
 *   <li><em>Original size</em>.
 *           Text will be displayed with its original size.
 *
 *   <li><em>Fade</em>.
 *           Text will be faded out.
 *
 *   <li><em>Split</em>.
 *           Text will be wrapped and moved to a new line.
 *
 *   <li><em>Shrink to fit</em>.
 *           Text will be shrunk to fit in the text view's boundary.
 *
 *   <li><em>Ellipsize at the end</em>.
 *           Text will be truncated to fit in the text view's boundary and the ellipsize text will be added. ( '...' by default).
 * </ul>
 * Dali::Toolkit::TextView::Original is set by default.
 *
 * @see Dali::Toolkit::TextView::SetMultilinePolicy
 * @see Dali::Toolkit::TextView::SetWidthExceedPolicy
 * @see Dali::Toolkit::TextView::SetHeightExceedPolicy
 * @see Dali::Toolkit::TextView::SetFadeBoundary
 * @see Dali::Toolkit::TextView::SetEllipsizeText
 *
 * @note Multiple combinations are possible but not all of them are already implemented.
 * @see \ref exceed_policies_combinations table to check which combinations are implemented
 *
 * \section scroll Scroll
 *
 * Text could be scrolled if it exceeds the boundaries of the text-view.
 *
 * @see  Dali::Toolkit::TextView::SetScrollEnabled
 * @see  Dali::Toolkit::TextView::SetScrollPosition
 *
 * \section line_height_spacing Line height spacing
 *
 * The default space between lines could be modified by setting an offset with Dali::Toolkit::TextView::SetLineHeightOffset().
 *
 * <h1 class="pg">Font support and multi-language detection</h1>
 *
 * Dali::Toolit::TextView uses the font specified in the styled text array to display the given text.
 *
 * See \link markup-processor Markup Processor \endlink for more details on how to create styling markup strings and styled text arrays.
 *
 * To support multi-language texts, text-view does the following actions per character:
 * <ul>
 *   <li> Check if there is a font defined in the styled text array.
 *   <li> If there isn't, try to use the default platform font.
 *   <li> Check if the character is supported by the font.
 *   <li> If isn't, find the most suitable font for the character.
 * </ul>
 *
 * \section text_alignment Text alignment and justification
 *
 * Dali::Toolkit::TextView provides a method to align the whole text inside the text view's boundary as well as a method to justify each line
 * inside the text.
 *
 * The Dali::Toolkit::Alignment::Type is used to align the whole text in the text view's area.
 * A text could be horizontally aligned (left, center, right) and/or vertically aligned (top, center, bottom).
 * Dali::Toolkit::Alignment::HorizontalCenter | Dali::Toolkit::Alignment::VerticalCenter is set by default.
 *
 * The Dali::Toolkit::TextView::LineJustification is used to justify each line inside the text (left, center, right, justified).
 * Dali::Toolkit::TextView::Left is set by default.
 *
 * @see Dali::Toolkit::TextView::SetTextAlignment @see Dali::Toolkit::TextView::SetLineJustification
 *
 * \section text_styling Text styling
 *
 * Dali::Toolkit::TextView supports all text styling features provided by Dali::TextActor (font type, color, size, outline, etc).
 *
 *  Different techniques are provided to set or modify the text view's style:
 *
 * <ul>
 *  <li> By setting a Dali::Toolkit::MarkupProcessor::StyledTextArray with the Dali::Toolkit::TextView::SetText(const MarkupProcessor::StyledTextArray& text) method.
 *  <li> By setting a new Dali::TextStyle to the current text with the Dali::Toolkit::TextView::SetStyleToCurrentText() method.
 *  <li> By setting an html-ish markup string which contains both text and style with the Dali::Toolkit::TextView::SetText(const std::string& text) method.
         @note By default the style markup processor is disabled. @see Dali::Toolkit::TextView::SetMarkupProcessingEnabled to enable the markup processing.
 * </ul>
 *
 * See \link markup-processor Markup Processor \endlink for more details on how to create styling markup strings and styled text arrays.
 *
 * \section retrieve Retrieve text layout information
 *
 * The Dali::Toolkit::TextView::GetTextLayoutInfo() retrieves how the input text has been laid out.
 *
 * For each character it retrieves its size and position, visibility, etc. @see Dali::Toolkit::TextView::CharacterLayoutInfo.
 *
 * For each laid-out line it retrieves the index of the first character of the line, size, etc. @see Dali::Toolkit::TextView::LineLayoutInfo.
 *
 * \section appendix Appendix
 * \subsection examples Examples
 *
 * The following examples show how to use TextView. The grey square is an actor which has been added just to show the size of the text-view.
 *
 * Creation of a text view actor with all its parameters by default.
 * \code
 *   Toolkit::TextView textView = Toolkit::TextView::New( "Hello world!" );
 *   textView.SetParentOrigin( ParentOrigin::CENTER );
 *
 *   Stage::GetCurrent().Add( textView );
 * \endcode
 *
 * This example wraps the text in lines only when a \\n character is found. The size of the text-view will be automatically resized to fit the whole text inside.
 * \code
 *   const std::string text( "<font color='black'>"
 *                           "Lorem ipsum dolor sit amet, consectetur adipisicing elit,\n"
 *                           "sed do eiusmod tempor incididunt ut labore et dolore magna aliqua."
 *                           "</font>" );
 *
 *   Toolkit::TextView textView = Toolkit::TextView::New( "" );
 *   textView.SetMarkupProcessingEnabled( true );
 *   textView.SetText( text );
 *   textView.SetParentOrigin( ParentOrigin::CENTER );
 *
 *   textView.SetMultilinePolicy( Toolkit::TextView::SplitByNewLineChar );
 *   textView.SetWidthExceedPolicy( Toolkit::TextView::Original );
 *   textView.SetHeightExceedPolicy( Toolkit::TextView::Original );
 *   textView.SetLineJustification( Toolkit::TextView::Center );
 *
 *   Stage::GetCurrent().Add( textView );
 * \endcode
 * \image html text-view/text-view-example-01.png
 *
 * This example wraps the lines by the next word when it exceeds the width of the text-view. The height exceed policy is set to \e Original so it may exceed the height of the text-view.
 *
 * \code
 *   const std::string text( "<font color='black'>"
 *                           "Lorem ipsum dolor sit amet, consectetur adipisicing elit, "
 *                           "sed do eiusmod tempor incididunt ut labore et dolore magna aliqua."
 *                           "</font>" );
 *
 *   Toolkit::TextView textView = Toolkit::TextView::New( "" );
 *   textView.SetMarkupProcessingEnabled( true );
 *   textView.SetText( text );
 *   textView.SetParentOrigin( ParentOrigin::CENTER );
 *   textView.SetSize( 300.f, 125.f );
 *
 *   textView.SetMultilinePolicy( Toolkit::TextView::SplitByWord );
 *   textView.SetWidthExceedPolicy( Toolkit::TextView::Original );
 *   textView.SetHeightExceedPolicy( Toolkit::TextView::Original );
 *   textView.SetLineJustification( Toolkit::TextView::Center );
 *
 *   Stage::GetCurrent().Add( textView );
 * \endcode
 * \image html text-view/text-view-example-02.png
 *
 * This example wraps the lines by the next word when it exceeds the width of the text-view. If a word is bigger than the text-view's width, it splits the word. If the text exceeds the height of the text-view, the text is ellipsized.
 *
 * \code
 *   const std::string text( "<font color='black'>"
 *                           "Loremipsumdolorsitametconsectetur adipisicing elit,\n"
 *                           "sed do eiusmod tempor incididunt ut labore et dolore magna aliqua."
 *                           "</font>" );
 *   const std::string ellipsizeText( "<font color='black'>...</font>" );
 *
 *   Toolkit::TextView textView = Toolkit::TextView::New( "" );
 *   textView.SetMarkupProcessingEnabled( true );
 *   textView.SetText( text );
 *   textView.SetEllipsizeText( ellipsizeText );
 *   textView.SetParentOrigin( ParentOrigin::CENTER );
 *   textView.SetSize( 300.f, 125.f );
 *
 *   textView.SetMultilinePolicy( Toolkit::TextView::SplitByWord );
 *   textView.SetWidthExceedPolicy( Toolkit::TextView::Split );
 *   textView.SetHeightExceedPolicy( Toolkit::TextView::EllipsizeEnd );
 *   textView.SetLineJustification( Toolkit::TextView::Center );
 *
 *   Stage::GetCurrent().Add( textView );
 * \endcode
 * \image html text-view/text-view-example-03.png
 *
 * This example is similar to the one above but the ellipsized text has been set to "" so nothing is shown.
 *
 * \code
 *   const std::string text( "<font color='black'>"
 *                           "Loremipsumdolorsitametconsecteturadipisicingelit"
 *                           "seddoeiusmodtemporincididuntutlaboreetdoloremagnaaliqua."
 *                           "</font>" );
 *   const std::string ellipsizeText( "" );
 *
 *   Toolkit::TextView textView = Toolkit::TextView::New( "" );
 *   textView.SetMarkupProcessingEnabled( true );
 *   textView.SetText( text );
 *   textView.SetEllipsizeText( ellipsizeText );
 *   textView.SetParentOrigin( ParentOrigin::CENTER );
 *   textView.SetSize( 300.f, 125.f );
 *
 *   textView.SetMultilinePolicy( Toolkit::TextView::SplitByWord );
 *   textView.SetWidthExceedPolicy( Toolkit::TextView::Split );
 *   textView.SetHeightExceedPolicy( Toolkit::TextView::EllipsizeEnd );
 *   textView.SetLineJustification( Toolkit::TextView::Center );
 *
 *   Stage::GetCurrent().Add( textView );
 * \endcode
 * \image html text-view/text-view-example-04.png
 *
 * This example shows how to fade the text out when it exceeds the boundaries of the text-view.
 *
 * \code
 *   const std::string text( "<font color='black'>"
 *                           "Lorem ipsum dolor sit amet,\n"
 *                           "consectetur adipisicing elit,\n"
 *                           "sed do eiusmod tempor incididunt\n"
 *                           "ut labore et dolore magna aliqua."
 *                           "</font>" );
 *
 *   Toolkit::TextView textView = Toolkit::TextView::New();
 *   textView.SetMarkupProcessingEnabled( true );
 *   textView.SetText( text );
 *   textView.SetParentOrigin( ParentOrigin::CENTER );
 *   textView.SetSize( 300.f, 100.f );
 *
 *   Toolkit::TextView::FadeBoundary fadeBoundary( PixelSize( 10 ), PixelSize( 10 ), PixelSize( 10 ), PixelSize( 10 ) );
 *   textView.SetFadeBoundary( fadeBoundary );
 *
 *   textView.SetMultilinePolicy( Toolkit::TextView::SplitByNewLineChar );
 *   textView.SetWidthExceedPolicy( Toolkit::TextView::Fade );
 *   textView.SetHeightExceedPolicy( Toolkit::TextView::Fade );
 *   textView.SetLineJustification( Toolkit::TextView::Center );
 *
 *   Stage::GetCurrent().Add( textView );
 * \endcode
 * \image html text-view/text-view-example-05.png
 *
 * This example enables the scroll feature. The screen-shots show three different images of the same text in different scroll positions.
 *
 * \code
 *   const std::string text( "<font color='black'>"
 *                           "Lorem ipsum dolor sit amet, consectetur adipisicing elit,\n"
 *                           "sed do eiusmod tempor incididunt ut labore et dolore magna aliqua."
 *                           "</font>" );
 *
 *   Toolkit::TextView textView = Toolkit::TextView::New();
 *   textView.SetMarkupProcessingEnabled( true );
 *   textView.SetText( text );
 *   textView.SetParentOrigin( ParentOrigin::CENTER );
 *   textView.SetSize( 300.f, 60.f );
 *
 *   textView.SetMultilinePolicy( Toolkit::TextView::SplitByNewLineChar );
 *   textView.SetWidthExceedPolicy( Toolkit::TextView::Original );
 *   textView.SetHeightExceedPolicy( Toolkit::TextView::Original );
 *   textView.SetLineJustification( Toolkit::TextView::Center );
 *
 *   textView.SetScrollEnabled( true );
 *
 *   Stage::GetCurrent().Add( textView );
 * \endcode
 * \image html text-view/text-view-example-06.png
 * \image html text-view/text-view-example-07.png
 * \image html text-view/text-view-example-08.png
 *
 * See \link markup-processor Markup Processor \endlink \ref example for more styling markup string examples.
 *
 * \subsection exceed_policies_combinations Implemented exceed policies combinations
 *
 * The following tables show which exceed policies are implemented for each multi-line policy. Each column has one width exceed policy (Original, Fade, Split, ShrinkToFit and EllipsizeEnd),
 * each row has one height exceed policy (Original, Fade, ShrinkToFit and EllipsizeEnd).
 *
 * @note The Split value is not valid for the height exceed policy.
 *
 * \htmlonly
 * <table border="1" align="center">
 * <tr align="center">
 *   <th align="center" colspan="7">SplitByNewLineChar</th>
 * </tr>
 * <tr align="center">
 *   <th colspan="2" rowspan="2"></th><th align="center" colspan="6">Width exceed policies</th>
 * </tr>
 * <tr>
 *   <th>Original</th><th>Fade</th><th>Split</th><th>ShrinkToFit</th><th>EllipsizeEnd</th>
 * </tr>
 * <tr align="center">
 *   <th rowspan="4">Height<br />exceed<br />policies</th>
 *         <th>Original</th>
 *                        <td><font color=#0A0>&#x2713</font></td>
 *                        <td><font color=#0A0>&#x2713</font></td>
 *                        <td><font color=#0A0>&#x2713</font></td>
 *                        <td><font color=#0A0>&#x2713</font></td>
 *                        <td><font color=#0A0>&#x2713</font></td>
 * </tr>
 * <tr align="center">
 *         <th>Fade</th>
 *                        <td><font color=#0A0>&#x2713</font></td>
 *                        <td><font color=#0A0>&#x2713</font></td>
 *                        <td><font color=#0A0>&#x2713</font></td>
 *                        <td><font color=#A00>&#x2717</font></td>
 *                        <td><font color=#A00>&#x2717</font></td>
 * </tr>
 * <tr align="center">
 *         <th>ShrinkToFit</th>
 *                        <td><font color=#A00>&#x2717</font></td>
 *                        <td><font color=#A00>&#x2717</font></td>
 *                        <td><font color=#A00>&#x2717</font></td>
 *                        <td><font color=#0A0>&#x2713</font></td>
 *                        <td><font color=#A00>&#x2717</font></td>
 * </tr>
 * <tr align="center">
 *         <th>EllipsizeEnd</th>
 *                        <td><font color=#A00>&#x2717</font></td>
 *                        <td><font color=#A00>&#x2717</font></td>
 *                        <td><font color=#0A0>&#x2713</font></td>
 *                        <td><font color=#A00>&#x2717</font></td>
 *                        <td><font color=#0A0>&#x2713</font></td>
 * </tr>
 * </table>
 * \endhtmlonly
 *
 * \n
 *
 * \htmlonly
 * <table border="1" align="center">
 * <tr align="center">
 *   <th align="center" colspan="7">SplitByWord</th>
 * </tr>
 * <tr align="center">
 *   <th colspan="2" rowspan="2"></th><th align="center" colspan="6">Width exceed policies</th>
 * </tr>
 * <tr>
 *   <th>Original</th><th>Fade</th><th>Split</th><th>ShrinkToFit</th><th>EllipsizeEnd</th>
 * </tr>
 * <tr align="center">
 *   <th rowspan="4">Height<br />exceed<br />policies</th>
 *         <th>Original</th>
 *                        <td><font color=#0A0>&#x2713</font></td>
 *                        <td><font color=#0A0>&#x2713</font></td>
 *                        <td><font color=#0A0>&#x2713</font></td>
 *                        <td><font color=#0A0>&#x2713</font></td>
 *                        <td><font color=#0A0>&#x2713</font></td>
 * </tr>
 * <tr align="center">
 *         <th>Fade</th>
 *                        <td><font color=#0A0>&#x2713</font></td>
 *                        <td><font color=#0A0>&#x2713</font></td>
 *                        <td><font color=#0A0>&#x2713</font></td>
 *                        <td><font color=#A00>&#x2717</font></td>
 *                        <td><font color=#A00>&#x2717</font></td>
 * </tr>
 * <tr align="center">
 *         <th>ShrinkToFit</th>
 *                        <td><font color=#A00>&#x2717</font></td>
 *                        <td><font color=#A00>&#x2717</font></td>
 *                        <td><font color=#A00>&#x2717</font></td>
 *                        <td><font color=#0A0>&#x2713</font></td>
 *                        <td><font color=#A00>&#x2717</font></td>
 * </tr>
 * <tr align="center">
 *         <th>EllipsizeEnd</th>
 *                        <td><font color=#A00>&#x2717</font></td>
 *                        <td><font color=#A00>&#x2717</font></td>
 *                        <td><font color=#0A0>&#x2713</font></td>
 *                        <td><font color=#A00>&#x2717</font></td>
 *                        <td><font color=#0A0>&#x2713</font></td>
 * </tr>
 * </table>
 * \endhtmlonly
 *
 * \n
 *
 * \htmlonly
 * <table border="1" align="center">
 * <tr align="center">
 *   <th align="center" colspan="7">SplitByChar</th>
 * </tr>
 * <tr align="center">
 *   <th colspan="2" rowspan="2"></th><th align="center" colspan="6">Width exceed policies</th>
 * </tr>
 * <tr>
 *   <th>Original</th><th>Fade</th><th>Split</th><th>ShrinkToFit</th><th>EllipsizeEnd</th>
 * </tr>
 * <tr align="center">
 *   <th rowspan="4">Height<br />exceed<br />policies</th>
 *         <th>Original</th>
 *                        <td><font color=#0A0>&#x2713</font></td>
 *                        <td><font color=#0A0>&#x2713</font></td>
 *                        <td><font color=#A00>&#x2717</font></td>
 *                        <td><font color=#A00>&#x2717</font></td>
 *                        <td><font color=#A00>&#x2717</font></td>
 * </tr>
 * <tr align="center">
 *         <th>Fade</th>
 *                        <td><font color=#0A0>&#x2713</font></td>
 *                        <td><font color=#0A0>&#x2713</font></td>
 *                        <td><font color=#A00>&#x2717</font></td>
 *                        <td><font color=#A00>&#x2717</font></td>
 *                        <td><font color=#A00>&#x2717</font></td>
 * </tr>
 * <tr align="center">
 *         <th>ShrinkToFit</th>
 *                        <td><font color=#A00>&#x2717</font></td>
 *                        <td><font color=#A00>&#x2717</font></td>
 *                        <td><font color=#A00>&#x2717</font></td>
 *                        <td><font color=#A00>&#x2717</font></td>
 *                        <td><font color=#A00>&#x2717</font></td>
 * </tr>
 * <tr align="center">
 *         <th>EllipsizeEnd</th>
 *                        <td><font color=#A00>&#x2717</font></td>
 *                        <td><font color=#A00>&#x2717</font></td>
 *                        <td><font color=#A00>&#x2717</font></td>
 *                        <td><font color=#A00>&#x2717</font></td>
 *                        <td><font color=#A00>&#x2717</font></td>
 * </tr>
 * </table>
 * \endhtmlonly
 */

