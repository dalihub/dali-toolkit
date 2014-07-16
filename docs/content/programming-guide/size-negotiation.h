/*! \page size-negotiation Size Negotiation
 *
<h2 class="pg">Overview</h2>

Size negotiation provides the ability for controls to be resized without the application having to set a size.
A parent control can resize itself according to its children. Each control can provide hints to using policies for the width and height.
Controls will be relaid just once and only when requested to, rather than relaid out several times when different properties are set.
Using size negotiation avoids the need for using size constraints to resize children, which would need to be calculated in the update
thread every time even minor changes occur.

This topic covers size policies, deriving from ControlImpl, size policy examples and the size negotiation algorithm.

<h2 class="pg">Size Policies</h2>

Each control has a policy for both width and height:
- <b>Fixed:</b> The size is fixed to the size set by the application. If no size is set, then the size is fixed to the <i>natural</i> size of the control.
- <b>Mininum:</b> The size can grow and shrink but cannot be smaller than the <i>minimum</i> size specified.
- <b>Maximum:</b> The size can grow and shrink but cannot be larger than the <i>maximum</i> size specified.
- <b>Range:</b> The size can grow or shrink but within the <i>minimum</i> and <i>maximum</i> sizes specified.
- <b>Flexible:</b> The size of the control can grow or shrink as required without any limits.

Currently, by default, controls use a fixed size policy for width and height. If one dimension is set, and the other dimension is set to zero, then the latter
dimension is assumed to have a non fixed policy.

<h2 class="pg">Deriving from ControlImpl</h2>

The size negotiation utilises several methods to work out the best size of a control. The default behaviour is in the ControlImpl class.
The following methods can be overridden.
@code Vector3 GetNaturalSize() @endcode
This method should return the natural size of the control. This can be dependent on the control's children or the control may decide to have a fixed size.

@code float GetHeightForWidth( float width ) @endcode
This method should return the height it needs to be when given a certain width.

@code float GetWidthForHeight( float height ) @endcode
This method should return the width it needs to be when given a certain height.

All these methods can be overridden by deriving classes. This allows each control to dictate what size it would like to be. If you want the default behaviour,
then you do not have to override these methods. A parent control can call the child's methods to determine its size if it needs to.

<h2 class="pg">Size Policies and Virtual Methods</h2>

The table below shows the methods that are called when certain width and height policies are in place.

<table>
  <tr>
    <td></td>
    <td><b>Fixed Height</b></td>
    <td><b>Height Not Fixed (All other policies)</b></td>
  </tr>
  <tr>
    <td><b>Fixed Width</b></td>
    <td>
    - Use size set by application
    - If only height set by application
      - Use height set by application
      - Use <b>GetWidthForHeight()</b> for width
    - If only width set by application
      - Use width set by application
      - Use <b>GetHeightForWidth()</b> for height
    - If not set, then get size by calling <b>GetNaturalSize()</b>
    </td>
    <td>
    - Use width set by application
    - Use allocated width if not set
      - Ensure it satisfies our width policy
      - Adjust if required
    - Use <b>GetHeightForWidth()</b> for height
    - Ensure height satisfies our height policy
      - Adjust if required
    </td>
  </tr>
  <tr>
    <td><b>Width Not Fixed (All other policies)</b></td>
    <td>
    - Use height set by application
    - Use allocated height if not set
      - Ensure it satisfies our height policy
      - Adjust if required
    - Use <b>GetWidthForHeight()</b> for width
    - Ensure width satisfies our width policy
      - Adjust if required
    </td>
    <td>
    - Constrain the allocated width and height according to the two policies
    </td>
  </tr>
</table>

<h2 class="pg">Size Policy Examples</h2>

<h3 class="pg">Policy: Fixed Width and Height (1)</h3>

<table border=0 cellpadding=10><tr>
<td>
\image html FixedWidthHeight.png
</td>
<td>
The application/control has set the following settings:
- <b>SetSize:</b> 200 x 300
- <b>Natural Size:</b> 400 x 400
- <b>Width To Height Ratio:</b> 1 to 1
- <b>Width Policy:</b> Fixed
- <b>Height Policy:</b> Fixed
- <b>ParentOrigin:</b> TopLeft
- <b>AnchorPoint:</b> TopLeft

Control methods called:
- None

Result
- <b>Allocated size:</b> 200 x 300
</td>
</tr></table>

<h3 class="pg">Policy: Fixed Width and Height (2)</h3>

<table border=0 cellpadding=10><tr>
<td>
\image html FixedWidthHeight2.png
</td>
<td>
The application/control has set the following settings:
- <b>SetSize:</b> 0 x 0 (No size set)
- <b>Natural Size:</b> 400 x 400
- <b>Width To Height Ratio:</b> 1 to 1
- <b>Width Policy:</b> Fixed
- <b>Height Policy:</b> Fixed
- <b>ParentOrigin:</b> TopLeft
- <b>AnchorPoint:</b> TopLeft

Control methods called:
- GetNaturalSize() = 400 x 400

Result
- <b>Allocated size:</b> 400 x 400
</td>
</tr></table>

<h3 class="pg">Policy: Flexible Width and Height</h3>

<table border=0 cellpadding=10><tr>
<td>
\image html FlexibleWidthHeight.png
</td>
<td>
The application/control has set the following settings:
- <b>SetSize:</b> 200 x 300
- <b>Natural Size:</b> 400 x 400
- <b>Width To Height Ratio:</b> 1 to 1
- <b>Width Policy:</b> Flexible
- <b>Height Policy:</b> Flexible
- <b>ParentOrigin:</b> TopLeft
- <b>AnchorPoint:</b> TopLeft

Control methods called:
- None

Result
- <b>Allocated size:</b> 480 x 800  (Assume stage size 480 x 800)
</td>
</tr></table>

<h3 class="pg">Policy: Fixed Width and Flexible Height (1)</h3>

<table border=0 cellpadding=10><tr>
<td>
\image html FixedWidthFlexibleHeight.png
</td>
<td>
The application/control has set the following settings:
- <b>SetSize:</b> 200 x 300
- <b>Natural Size:</b> 400 x 400
- <b>Width To Height Ratio:</b> 1 to 1
- <b>Width Policy:</b> Fixed
- <b>Height Policy:</b> Flexible
- <b>ParentOrigin:</b> TopLeft
- <b>AnchorPoint:</b> TopLeft

Control methods called:
- GetHeightForWidth( 200 ) = 200

Result
- <b>Allocated size:</b> 200 x 200
</td>
</tr></table>

<h3 class="pg">Policy: Fixed Width and Flexible Height (2)</h3>

<table border=0 cellpadding=10><tr>
<td>
\image html FixedWidthFlexibleHeight.png
</td>
<td>
The application/control has set the following settings:
- <b>SetSize:</b> 200 x 0 (No height set)
- <b>Natural Size:</b> 400 x 400
- <b>Width To Height Ratio:</b> 1 to 1
- <b>Width Policy:</b> Fixed
- <b>Height Policy:</b> Flexible
- <b>ParentOrigin:</b> TopLeft
- <b>AnchorPoint:</b> TopLeft

Control methods called:
- GetHeightForWidth( 200 ) = 200

Result
- <b>Allocated size:</b> 200 x 200
</td>
</tr></table>

<h3 class="pg">Policy: Fixed Width and Flexible Height (3)</h3>

If the control did not have the GetHeightForWidth() method, then the <i>size set</i> is used to calculate the ratio.

<table border=0 cellpadding=10><tr>
<td>
\image html FixedWidthFlexibleHeight2.png
</td>
<td>
The application/control has set the following settings:
- <b>SetSize:</b> 200 x 0 (No height set)
- <b>Natural Size:</b> 400 x 400
- <b>Width To Height Ratio:</b> Not set
- <b>Width Policy:</b> Fixed
- <b>Height Policy:</b> Flexible
- <b>ParentOrigin:</b> TopLeft
- <b>AnchorPoint:</b> TopLeft

Control methods called:
- GetHeightForWidth( 200 ) = 200  <i>(Unable to calculate ratio using size set)</i>

Result
- <b>Allocated size:</b> 200 x 800  <i>(Allocate entire height)</i>
</td>
</tr></table>

<h3 class="pg">Policy: Fixed Width and Flexible Height (4)</h3>

<table border=0 cellpadding=10><tr>
<td>
\image html FlexibleWidthHeight.png
</td>
<td>
The application/control has set the following settings:
- <b>SetSize:</b> 0 x 0 (No size set)
- <b>Natural Size:</b> 400 x 400
- <b>Width To Height Ratio:</b> 1 to 1
- <b>Width Policy:</b> Fixed
- <b>Height Policy:</b> Flexible
- <b>ParentOrigin:</b> TopLeft
- <b>AnchorPoint:</b> TopLeft

Control methods called:
- GetHeightForWidth( 0 ) = 0

Result
- <b>Allocated size:</b> 480 x 800  <i>(Allocate entire size)</i>
</td>
</tr></table>

<h3 class="pg">Policy: Flexible Width and Fixed Height (1)</h3>

<table border=0 cellpadding=10><tr>
<td>
\image html FlexibleWidthFixedHeight.png
</td>
<td>
The application/control has set the following settings:
- <b>SetSize:</b> 0 x 300 (No width set)
- <b>Natural Size:</b> 400 x 400
- <b>Width To Height Ratio:</b> 1 to 1
- <b>Width Policy:</b> Flexible
- <b>Height Policy:</b> Fixed
- <b>ParentOrigin:</b> TopLeft
- <b>AnchorPoint:</b> TopLeft

Control methods called:
- GetWidthForHeight( 300 ) = 300

Result
- <b>Allocated size:</b> 300 x 300
</td>
</tr></table>

<h3 class="pg">Policy: Flexible Width and Fixed Height (2)</h3>

If the control did not have the GetWidthForHeight() method, then the <i>size set</i> is used to calculate the ratio.

<table border=0 cellpadding=10><tr>
<td>
\image html FlexibleWidthFixedHeight2.png
</td>
<td>
The application/control has set the following settings:
- <b>SetSize:</b> 0 x 300 (No width set)
- <b>Natural Size:</b> 400 x 400
- <b>Width To Height Ratio:</b> Not set
- <b>Width Policy:</b> Flexible
- <b>Height Policy:</b> Fixed
- <b>ParentOrigin:</b> TopLeft
- <b>AnchorPoint:</b> TopLeft

Control methods called:
- GetWidthForHeight( 300 ) = 0  <i>(Unable to calculate ratio using size set)</i>

Result
- <b>Allocated size:</b> 480 x 300  <i>(Allocate entire width)</i>
</td>
</tr></table>

<h2 class="pg">The Size Negotiation Algorithm</h2>

<h3 class="pg">The Algorithm</h3>

-# The algorithm starts at the stage
  - All top level controls are found and offered the size of the stage
  - The control negotiates the size offered by using the policy rules to determine the size that it should be allocated
  - The control is then set to that allocated size
-# The control is responsible for setting the sizes of all its children
  - Can set a size on an Actor
  - Or can call relayout on a Control directly
-# Children that a control does not handle, the control can add to a container so that the top-level algorithm delas with it instead
  - The control should call Relayout with the child and size of itself as parameters

<table border=0 cellpadding=10><tr>
<td>
\image html Algorithm1.png
</td>
<td>
\image html Algorithm2.png
</td>
</tr></table>

<h3 class="pg">A closer look at Control A</h3>

Taking a closer look at Control A we see in this example that children should share the width equally and that the height of Control A
is the maximum height of the children.

<table border=0 cellpadding=10><tr>
<td>
\image html Algorithm3.png
</td>
<td>
<table border=0 cellpadding=10><tr>
<td>
\image html Algorithm4.png
</td>
</tr></table>
</td>
</tr></table>

@code
class ControlA : public Control
{
  ...
private:
  // Data
  Actor mActorC; // Container to store children
  TextView mControlD; // Some text we want to display
  ImageActor mActorD; // An image
};
@endcode

@code
Vector3 ControlA::GetNaturalSize()
{
  // Width is total of ControlD and ActorD
  // Height is max of ControlD and ActorD
  // Don't care about the Depth
  Vector3 naturalSize;  // Initialised to (0.0f, 0.0f, 0.0f)

  if ( mControlD )
  {
    // We know ControlD is a control, so just ask its natural size
    naturalSize = mControlD.GetNaturalSize();
  }

  if ( mActorD )
  {
    // We know ActorD is an ImageActor, we can get the image's natural size
    Image image = mActorD.GetImage();
    naturalSize.width += image.GetWidth;
    naturalSize.height = std::max( naturalSize.height, image.GetHeight() );
  }

  return naturalSize;
}
@endcode

GetHeightForWidth() and GetWidthForHeight() can be created in a similar manner.

@code
void ControlA::OnRelaidOut( Vector2 size, ActorSizeContainer& container )
{
  // Width to be shared between ControlD and ActorD
  Vector2 childSize( size.width * 0.5f, size.height );

  if ( mActorC )
  {
    // ActorC is the same size as ControlA
    mActorC.SetSize( size );
  }

  if ( mControlD )
  {
    Relayout( mControlD, childSize );
    // Can add more complex calculations to preserve aspect ratio etc.
  }

  if ( mActorD )
  {
    mActorD.SetSize( childSize );
    // Can add more complex calculations to preserve aspect ratio etc.
  }
}
@endcode

The allocated layout is as follows.
<table border=0 cellpadding=10><tr>
<td>
\image html Algorithm8.png
</td>
</tr></table>

<h3 class="pg">A closer look at Control B</h3>

In this example we have the following requirements:
- Control B creates a small border around its children
- Control B just allocates whatever its given to its children (minus the border)
- Actor E is a simple container actor and contains one control (Control E)
- Control B is not aware of the actors it contains

<table border=0 cellpadding=10><tr>
<td>
\image html Algorithm9.png
</td>
<td>
\image html Algorithm10.png
</td>
</tr></table>

@code
void ControlA::OnRelaidOut( Vector2 size, ActorSizeContainer& container )
{
  // Width of children is our size minus border
  Vector3 childSize( size.width - mBorder.width * 2.0f,
                     size.height - mBorder.height * 2.0f );

  // Our children should be set to the childSize
  ActorContainer children( Self().GetChildren() );
  for ( ActorIter iter = children.begin(), endIter = children.end();
        iter != endIter;
        ++iter )
  {
    Relayout( *iter, childSize, container );
  }
}
@endcode

The Relayout method will add ControlB's children to the size negotiation algorithm container where the child's size will be negotiated. Control E's
size will be negotiated with the childSize as the allocation.

*
*/
