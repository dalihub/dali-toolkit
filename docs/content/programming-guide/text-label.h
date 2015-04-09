/*! \page text-label Text Label
 *
\section overview Overview
The Dali::Toolkit::TextLabel is a Dali::Toolkit::Control which renders a short text string.\n
Text labels are lightweight, non-editable and do not respond to user input.

\subsection basictextlabelusage Basic usage

To display a TextLabel the TEXT property must be set using a UTF-8 string.

\code
TextLabel label = TextLabel::New();
label.SetProperty( TextLabel::Property::TEXT, "Hello World" );
Stage::GetCurrent().Add( label );
\endcode

The label must also be added to the stage, or to an actor which is on the stage.\n
In this example the text-label will be automatically given a natural size i.e. large enough to fit the text.\n
The position of the label on-screen is dependent on the parent-origin and anchor-point properties:

<table border=0 cellpadding=10>
<tr>
  <td>
  \image html TextLabelTopLeft.png
  </td>
  <td>
  \image html TextLabelCenter.png
  </td>
</tr>
<tr>
  <td>
  (ParentOrigin::TOP_LEFT, AnchorPoint::TOP_LEFT)
  </td>
  <td>
  (ParentOrigin::CENTER, AnchorPoint::CENTER)
  </td>
</tr>
</table>

\subsection fontselection Font Selection

By default TextLabel will automatically select a suitable font from the platform.\n
Typically fonts do not support all scripts, for example Latin fonts often do not provide Arabic glyphs.\n
Therefore you should expect TextLabel to select different fonts for each script.

Alternatively a font may be requested using eiter or all of FONT_FAMILY, FONT_STYLE, and POINT_SIZE properties:
\code
label.SetProperty( TextLabel::Property::FONT_FAMILY, "HelveticaNue" );
label.SetProperty( TextLabel::Property::FONT_STYLE,  "Regular" );
label.SetProperty( TextLabel::Property::POINT_SIZE,  12.0f );
\endcode
However the TextLabel will fall-back to using the default font, if the requested font does not support the required scripts.

\subsection fontselection Text Alignment

Wrapping can be enabled using the MULTI_LINE property:\n
\code
label.SetProperty( TextLabel::Property::MULTI_LINE, true );
\endcode

The text can be either aligned to the start, end, or center of the available area:
\code
label.SetProperty( TextLabel::Property::ALIGNMENT, "BEGIN" ); // "CENTER" or "END"
\endcode

<table border=0 cellpadding=10>
<tr>
  <td>
  Here is the "BEGIN" alignment shown for left-to-right (Latin) and right-to-left (Arabic) scripts:
  </td>
</tr>
<tr>
  <td>
  \image html LatinBegin.png
  </td>
  <td>
  \image html ArabicBegin.png
  </td>
</tr>
<tr>
  <td>
  Here is the "CENTER" alignment shown for left-to-right (Latin) and right-to-left (Arabic) scripts:
  </td>
</tr>
<tr>
  <td>
  \image html LatinCenter.png
  </td>
  <td>
  \image html ArabicCenter.png
  </td>
</tr>
<tr>
  <td>
  Here is the "END" alignment shown for left-to-right (Latin) and right-to-left (Arabic) scripts:
  </td>
</tr>
<tr>
  <td>
  \image html LatinEnd.png
  </td>
  <td>
  \image html ArabicEnd.png
  </td>
</tr>
</table>

*/
