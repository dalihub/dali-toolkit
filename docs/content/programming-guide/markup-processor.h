/*! \page markup-processor Markup processor
 *
 * <h1 class="pg">Overview</h1>
 *
 * Dali::Toolkit::MarkupProcessor functions provide mechanisms to build and modify a Dali::Toolkit::MarkupProcessor::StyledTextArray used to store text with style.
 *
 * <h1 class="pg">Build a styled text array from a markup string</h1>
 *
 * Dali::Toolkit::MarkupProcessor::GetStyledTextArray() could be used to convert an html-ish markup string into a styled text array. This string uses html-ish tags to
 * define the text's style as follows:
 *
 * <ul>
 *   <li>\e \<b\>\</b\> Bold text.
 *   i.e. \<b\>Bold text\</b\>"
 *   \image html text-view/Bold.png
 *
 *   <li>\e \<i\>\</i\> Italic text.
 *   i.e. \<i\>Italic text\</i\>"
 *   \image html text-view/Italic.png
 *
 *   <li>\e \<u\>\</u\> Underlined text.
 *   i.e. \<u\>Underline text\</u\>"
 *   \image html text-view/Underline.png
 *
 *   <li>\e \<br /\> New line.
 *
 *   <li>\e \<font\>\</font\> Specifies font properties:
 *     <ul>
 *       <li> \e face The name of a font or font family.
 *       <li> \e style The style of a font.
 *       <li> \e size Font point size. @see Dali::PointSize.
 *       <li> \e color Font color. See the \ref color section for more details.
 *     </ul>
 *
 *     i.e. \<font face='FreeSerif' style='Regular'\>FreeSerif font\</font\>
 *     \image html text-view/FreeSerifFont.png
 *
 *   <li>\e \<shadow\>\</shadow\> Specifies shadow properties.
 *     <ul>
 *       <li> \e paramx X offset.
 *       <li> \e paramy Y offset.
 *       <li> \e color Shadow color. See the \ref color section for more details.
 *     </ul>
 *
 *     i.e. \<shadow color='black' paramx='1.5' paramy='1.5'\>Black shadow\</shadow\>
 *     \image html text-view/Black-Shadow.png
 *
 *     @see Dali::TextActor::SetShadow()
 *     <br><br>
 *   <li>\e \<glow\>\</glow\> Specifies glow properties.
 *     <ul>
 *       <li> \e param Glow around the text.
 *       <li> \e color Glow color. See the \ref color section for more details.
 *     </ul>
 *
 *     i.e. \<smooth param='0.65'\>\<glow color='blue' param='0.05'\>Blue glow\</glow\>\</smooth\>
 *     \image html text-view/Blue-Glow.png
 *
 *     @see Dali::TextActor::SetGlow()
 *     <br><br>
 *   <li>\e \<outline\>\</outline\> Specifies outline properties.
 *     <ul>
 *       <li> \e paramx X thickness.
 *       <li> \e paramy Y thickness.
 *       <li> \e color Outline color. See the \ref color section for more details.
 *     </ul>
 *
 *     i.e. \<outline color='red' paramx='0.5' paramy='0.5'\>Red outline\</outline\>
 *     \image html text-view/Red-Outline.png
 *
 *     @see Dali::TextActor::SetOutline()
 *     <br><br>
 *   <li>\e \<smooth\>\</smooth\> Specify the smooth edge.
 *     <ul>
 *       <li> \e param Distance field.
 *     </ul>
 *
 *     i.e. \<smooth param='0.3'\>Smooth text\</smooth\>
 *     \image html text-view/Smooth-Text.png
 *
 *     @see Dali::TextActor::SetSmoothEdge()
 * </ul>
 *
 * See also \ref color, \ref special_characters and \ref example for more details.
 *
 * <h1 class="pg">Get a markup string from a styled text array</h1>
 *
 * Dali::Toolkit::MarkupProcessor::GetMarkupString() could be used to convert a styled text array into an html-ish markup string.
 *
 * <h1 class="pg">Modify a styled text array</h1>
 *
 * Different functions are provided to modify whole or part of a styled text array with a given style. A mask could be used to modify only some properties of the style.
 *
 * @see Dali::Toolkit::MarkupProcessor::SetTextStyle( StyledTextArray& styledTextArray, const TextStyle& style, TextStyle::Mask mask )
 * @see Dali::Toolkit::MarkupProcessor::SetTextStyle( const Text& text, StyledTextArray& styledTextArray, const TextStyle& style, TextStyle::Mask mask )
 * @see Dali::Toolkit::MarkupProcessor::SetTextStyleToRange( StyledTextArray& styledTextArray, const TextStyle& style, TextStyle::Mask mask, std::size_t begin, std::size_t end )
 *
 * <h1 class="pg">Appendix</h1>
 * \section color Color
 *
 * Different options could be used to define a color:
 *
 * <ul>
 *   <li> Hexadecimal with alpha channel. 0xAARRGGBB with the alpha channel in the most significant bits.
 *   <li> Hexadecimal without alpha channel. 0xRRGGBB.
 *   <li> Web color format (six or three digits). \#RRGGBB, \#RGB
 *   <li> Some colors could be defined with natural language: <em>black, white, red, green, blue, yellow, magenta, cyan</em> and \e transparent.
 * </ul>
 *
 * \section special_characters Special characters
 *
 * \< and \> characters are used to build the html-ish tags. To type them is needed to add a back slash character in front of them.
 * @note in c and c++ the back slash is represented with a double back slash '\\\\'.
 *
 * i.e. text.SetText("a \\< b \\< c");
 * \image html text-view/AlessBlessC.png
 *
 * It transform any pair CR+LF new line characters into a single LF new line character.
 *
 * \section fonts Font Style, Weight and Smooth
 *
 * This appendix shows the differences and relations between the font style, the font weight, the smoothness and the italics.
 *
 * When a font is loaded, Dali uses different mechanisms to modify the render of glyphs.<br><br>
 * i.e \<font face='Samsung Sans' size='24'\>Hello World\</font\> produces the "Hello World" bellow.
 * \image html text-view/FontAppendix01.png
 * By adding the \<i\> \<b\> \<smooth\> tags to the markup string or using the Dali::TextStyle::SetItalics(), Dali::TextStyle::SetWeight() or Dali::TextStyle::SetSmoothEdge() methods,
 * Dali modifies how glyphs of the same font are rendered.<br><br>
 * i.e \<i\>\<font face='Samsung Sans' size='24'\>Hello World\</font\>\</i\>
 * \image html text-view/FontAppendix02.png
 * i.e \<b\>\<font face='Samsung Sans' size='24'\>Hello World\</font\>\</b\>
 * \image html text-view/FontAppendix03.png
 *
 * The smooth parameter can be used to adjust the thickness of the rendered glyphs.<br><br>
 * i.e<br> \<smooth param=0.65\>\<font face='Samsung Sans' size='24'\>Hello World\</font\>\</smooth\>\<br /\><br>
 *         \<font face='Samsung Sans' size='24'\>Hello World\</font\>\<br /\><br>
 *         \<smooth param=0.4\>\<font face='Samsung Sans' size='24'\>Hello World\</font\>\</smooth\>\<br /\><br>
 *         \<b\>\<font face='Samsung Sans' size='24'\>Hello World\</font\>\</b\>\<br /\><br>
 *         \<smooth param=0.2\>\<font face='Samsung Sans' size='24'\>Hello World\</font\>\</smooth\>
 *
 * \image html text-view/FontAppendix04.png
 *
 * All "Hello World" above have been rendered using the same font, saving some memory. Alternatively, the platform can provide fonts with different styles.<br>
 *
 * i.e. Samsung platform provides among others:<br>
 * Samsung Sans:style=Light<br>
 * Samsung Sans:style=Regular<br>
 * Samsung Sans:style=Medium<br>
 *
 * \<font face='Samsung Sans' style='Light' size='24'\>Hello World\</font\>\<br /\><br>
 * \<font face='Samsung Sans' style='Regular' size='24'\>Hello World\</font\>\<br /\><br>
 * \<font face='Samsung Sans' style='Medium' size='24'\>Hello World\</font\>
 * \image html text-view/FontAppendix05.png
 *
 * The three "Hello World" above have been rendered with three different fonts.<br>
 *
 * The <i>fc-list</i> command can be executed on the platform command line to check with fonts and which styles are supported.
 *
 * \section example Example
 *
 * \code
 *   const std::string text( "<font size='16' color='black'>"
 *                           "<i>Italics: 기울임 꼴</i><br/>"
 *                           "<u>Underline: 밑줄</u><br/>"
 *                           "<b>Bold: 두꺼운</b><br/>"
 *                           "<font face='FreeSerif'>Font FreeSerif</font><br/>"
 *                           "<font color='white'><shadow color='black' paramx='1.5' paramy='1.5'>Shadow: 그림자</shadow></font><br/>"
 *                           "<font color='white'><smooth param='0.75'><glow color='blue' param='0.1'>Glow: 빛나다</glow></smooth></font><br/>"
 *                           "<font color='white'><outline color='red' paramx='0.5' paramy='0.5'>Outline: 윤곽선</outline></font><br/>"
 *                           "<smooth param='0.3'>Smooth: 부드럽게</smooth><br/>"
 *                           "</font>" );
 *
 *   Toolkit::MarkupProcessor::StyledTextArray styledText;
 *   Toolkit::MarkupProcessor::GetStyledTextArray( text, styledText, true );
 *
 *   Toolkit::TextView textView = Toolkit::TextView::New( styledText );
 *   textView.SetParentOrigin( ParentOrigin::CENTER );
 *
 *   Stage::GetCurrent().Add( textView );
 * \endcode
 *
 * \image html text-view/text-view.png
 * Text generated with the example above.
 */
