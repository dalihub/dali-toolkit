/*! \page size-negotiation Size Negotiation
 *
<h2 class="pg">Overview</h2>

Size negotiation, also known as layout management, is responsible for allocating sizes to all actors on the stage based on rules of dependency between
the actors. Requests for relayout on actors are collected during the frame with the actual relayout performed at the end of the frame.

This document details how to use the size negotiation API and is intended for application writters.

The topics covered are:
- Dimensions
- Resize policies
- Actor
- Debugging

<h2 class="pg">Dimensions</h2>

The notion of width and height is generalised into the concept of a Dimension. Several methods take a Dimension parameter.

The Dimension enum specifies the available dimensions as bitfields:
- Dimension::WIDTH
- Dimension::HEIGHT

If a method can process width and height at the same time then the Dimension::ALL_DIMENSIONS mask can be specified.

<h2 class="pg">Resize Policies</h2>

<h3>Policies</h3>
The ResizePolicy enum specifies a range of options for controlling the way actors resize. These are powerful rules that enable much automatic
resizing behaviour. They are as following:

- ResizePolicy::FIXED: This is the option to use when you want the specific definite size as set by SetSize (This is the default for all actors)
- ResizePolicy::USE_NATURAL_SIZE: Use this option for objects such as images or text to get their natural size e.g. The dimensions of the image, or the size of the text without wrapping. Also use this on TableViews when the size of the table is dependent on its children.
- ResizePolicy::FILL_TO_PARENT: Size will fill up to the size of its parent's size, taking a size factor into account to allow proportionate filling
- ResizePolicy::SIZE_RELATIVE_TO_PARENT: Fill up the parent with a relative scale. Use SetSizeModeFactor to specify the ratio to fill up to the parent.
- ResizePolicy::SIZE_FIXED_OFFSET_FROM_PARENT: Fill up the parent and add a fixed offset using SetSizeModeFactor.
- ResizePolicy::FIT_TO_CHILDREN: Size will scale around the size of the actor's children. E.g. A popup's height may resize itself around it's contents.
- ResizePolicy::DIMENSION_DEPENDENCY: This covers rules such as width-for-height and height-for-width. You specify that one dimension depends on another.

\image html size-negotiation/ResizePolicies.png

<h2 class="pg">Actor</h2>

This section details how an actor may be used with size negotiation.

<h3>Enabling Size Negotiation</h3>

Text and image actors have relayout enabled by default, while a plain Actor is disabled unless a call to SetResizePolicy is made.

<h3>Specifying Size Policies</h3>

Actors have different size policies by default. For example ImageView is set to USE_NATURAL_SIZE. This ensures that when an image actor is
placed on the stage it will use its natural size by default. However if the user calls SetSize with non-zero sizes on the image actor then the current
size policy is overridden by the FIXED size policy and the actor will take on the size specified.

The next step is to specify how an actor will be size negotiated. The resize policies for an actor may be specified by the following method:
@code void SetResizePolicy( ResizePolicy::Type policy, Dimension::Type dimension ) @endcode
It is common to specifiy different policies for the different dimensions of width and height to achive different layouts. Different actors have
different resize policies specified by default. For example ImageViews are set to use USE_NATURAL_SIZE.

The following example code snippet shows rootActor having its width policy set to ResizePolicy::FILL_TO_PARENT and its height policy set to ResizePolicy::FIT_TO_CHILDREN.
It has an ImageView added to it with an explicit call to USE_NATURAL_SIZE in both dimensions called on it. This will make an actor that will
fill up the space of its parent in the width dimension and fit to its child in the height dimension. As the image actor child is using natural size
the height of the root actor will fit to the height of the child image.

@code
Actor rootActor = Actor::New();
rootActor.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH );
rootActor.SetResizePolicy( ResizePolicy::FIT_TO_CHILDREN, Dimension::HEIGHT );
Toolkit::ImageView image = Toolkit::ImageView::New( MY_IMAGE_PATH );
image.SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::ALL_DIMENSIONS );
rootActor.Add( image );
@endcode

The following images show the before and after layouts for this code example.

Before:
\image html size-negotiation/SizeNegotiationExample_Before.png
After:
\image html size-negotiation/SizeNegotiationExample_After.png

This example shows an actor rootActor set to expand to its parent's width and contract/expand around its child's height. The child image actor
is set to natural size which means it will display at the acutal size of the image.

To specify that a dimension has a dependency on another dimension use ResizePolicy::DIMENSION_DEPENDENCY. For example if dimension is Dimension::HEIGHT and dependency is
Dimension::WIDTH then there is a height-for-width dependency in effect. The classic use case for this
is a text view that wraps its text. The following example snippet shows a text view that expands its width to the size of its parent, wraps its
contents and then determines its height based on the width.
@code
TextLabel text = TextLabel::New( "Example" );
text.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH );
text.SetResizePolicy( ResizePolicy::DIMENSION_DEPENDENCY, Dimension::HEIGHT );
@endcode

<h3>Specifying Sizes and Size Limits</h3>

When wanting a specific fixed size for an actor then specify the resize policy to be FIXED and set the desired, or preferred size using SetSize.
If only one dimension is FIXED then the other value in the size parameter will be ignored, so it is safe to set it to zero.

To constrain the final negotiated size of an actor, set the following for minimum and maximum sizes respectively.
@code
void SetMinimumSize( const Vector2& size )
void SetMaximumSize( const Vector2& size )
@endcode

<h3>Altering Negotiated Size</h3>

When an actor is required to maintain the aspect ratio of its natural size the following method can be used. This is useful for size negotiating images
to ensure they maintain their aspect ratio while still fitting within the bounds they have been allocated. This can be one of SizeScalePolicy::USE_SIZE_SET, SizeScalePolicy::FIT_WITH_ASPECT_RATIO
or SizeScalePolicy::FILL_WITH_ASPECT_RATIO. The first is the default. The second will fit the actor within the bounds it has been allocated while maintaining aspect ratio. The
third will fill all available space, potentially overflowing its bounds, while maintaining apsect ratio.
@code void SetSizeScalePolicy( SizeScalePolicy::Type policy ) @endcode

<h3>Using Actors in Containers</h3>

When laying out actors in containers such as TableView it is useful to be able to specify padding that surrounds the actor. E.g. You may
want some white space around an image actor placed in a table cell. The padding specifies the left, right, bottom and top padding values.
@code void SetPadding( const Padding& padding ) @endcode

<h2 class="pg">An Example</h2>

This section shows a more complex example of how to configure size negotiation. It creates a popup and adds a table view to it with a text view,
an image and a sub-table. The sub-table contains a checkbox and another text view.
@code
mPopup = CreatePopup();
mPopup.SetTitle( "Warning" );

// Content
Toolkit::TableView content = Toolkit::TableView::New( 2, 2 );
content.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH );
content.SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::HEIGHT );
content.SetFitHeight( 0 );
content.SetFitHeight( 1 );
content.SetPadding( Padding( 20.0f, 20.0f, 20.0f, 0.0f ) );

// Text
Toolkit::TextLabel text = Toolkit::TextLabel::New( "Do you really want to quit?" );
text.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH );
text.SetResizePolicy( ResizePolicy::DIMENSION_DEPENDENCY, Dimension::HEIGHT );

content.AddChild( text, Toolkit::TableView::CellPosition( 0, 0 ) );

// Image
Toolkit::ImageView image = Toolkit::ImageView::New( IMAGE_PATH );
image.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH );
image.SetResizePolicy( ResizePolicy::DIMENSION_DEPENDENCY, Dimension::HEIGHT );
image.SetPadding( Padding( 20.0f, 0.0f, 0.0f, 0.0f ) );
content.AddChild( image, Toolkit::TableView::CellPosition( 0, 1 ) );

// Checkbox and text
Toolkit::TableView root = Toolkit::TableView::New( 1, 2 );
root.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH );
root.SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::HEIGHT );
root.SetFitHeight( 0 );
root.SetFitWidth( 0 );
root.SetPadding( Padding( 0.0f, 0.0f, 0.0f, 20.0f ) );

Dali::Image unchecked = Dali::ResourceImage::New( CHECKBOX_UNCHECKED_IMAGE );
Dali::Image checked = Dali::ResourceImage::New( CHECKBOX_CHECKED_IMAGE );
Toolkit::CheckBoxButton checkBox = Toolkit::CheckBoxButton::New();
checkBox.SetBackgroundImage( unchecked );
checkBox.SetSelectedImage( checked );
checkBox.SetSize( Vector2( 48, 48 ) );

root.AddChild( checkBox, Toolkit::TableView::CellPosition( 0, 0 ) );

Toolkit::TextLabel text2 = Toolkit::TextLabel::New( "Don't show again" );
text2.SetPadding( Padding( 20.0f, 0.0f, 0.0f, 10.0f ) );

root.AddChild( text2, Toolkit::TableView::CellPosition( 0, 1 ) );

content.AddChild( root, Toolkit::TableView::CellPosition( 1, 0, 0, 2 ) );  // Column span 2

mPopup.Add( content );
@endcode

The resulting popup with additional buttons added is shown below.
\image html size-negotiation/Popup.png

The key things to pick out from this example are the use of the size negotiation API.
The content table view is set to ResizePolicy::FILL_TO_PARENT for its width and USE_NATURAL_SIZE for its height. This will result
in the table view expanding its width to fit the available space in the popup while also expanding/contracting its
height based on the size of the contents in its cells.
@code
content.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH );
content.SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::HEIGHT );
@endcode
To add a little space around the left, right and bottom of the table view, some padding is added.
@code
content.SetPadding( Padding( 20.0f, 20.0f, 20.0f, 0.0f ) );
@endcode
The first text view has its width set to ResizePolicy::FILL_TO_PARENT and its height has a dimension dependency on its width. This
will result in a text view that fills up its width to available space in the table cell and then then calculates its
height based on its new width. The table view will then fit its height taking the height of the text view into account.
@code
text.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH );
text.SetResizePolicy( ResizePolicy::DIMENSION_DEPENDENCY, Dimension::HEIGHT );
@endcode
The image view performs a similar relayout. It fits its width to the size of the cell and calculates its height based on the new
width. Some padding is added to the left of it as well to center it more.
@code
image.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH );
image.SetResizePolicy( ResizePolicy::DIMENSION_DEPENDENCY, Dimension::HEIGHT );
image.SetPadding( Padding( 20.0f, 0.0f, 0.0f, 0.0f ) );
@endcode
The sub table view is similar as well in that it expands its width to the size of its cell. When it is added to the table view it
will span two columns. Its height is set to natural size so that it will grow or shrink based on its children cells. Note that for
a container like table view, USE_NATURAL_SIZE acts in a similar manner to ResizePolicy::FIT_TO_CHILDREN in that the size of the container could
grow or shrink based on the sizes of the child actors.
@code
root.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH );
root.SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::HEIGHT );
@endcode
The checkbox is set to have a fixed size.
@code
checkBox.SetResizePolicy( ResizePolicy::FIXED, Dimension::ALL_DIMENSIONS );
@endcode
The second text view has not specified a resize policy so will use its default of USE_NATURAL_SIZE.

<h2 class="pg">Debugging</h2>

When constructing large scenes using interacting resize policies it is useful to be able to debug the relayout process. The following sections
describe a pitfall to avoid when creating scenes and a way to print debug output about the actors.

<h3>Infinite Dependency Loops</h3>
Despite the power of the resize rules there is one pitfall to be aware of: infinite dependency loops. The most simplest form of this is
shown by a parent actor with resize policy set to ResizePolicy::FIT_TO_CHILDREN with a child that has a resize policy of ResizePolicy::FILL_TO_PARENT. Who should
determine the size in this case? A more complex loop occurs when ResizePolicy::DIMENSION_DEPENDENCY comes into play. Say a parent has a width policy
of ResizePolicy::DIMENSION_DEPENDENCY with height and a height policy of ResizePolicy::FIT_TO_CHILDREN. The parent has a single child with a height policy ResizePolicy::DIMENSION_DEPENDENCY
with width. If the child's width policy is ResizePolicy::FILL_TO_PARENT then a loop will occur. These are two simple examples but the loops could occur
over larger spreads of parent child relationships. These loops are detected by the relayout algorithm with the result being that actors will
receive zero sizes. These loops are not common but are still something to watch out for.

<h3 class="pg">Inspecting Actor Relayout Properties</h3>
To get a print out of the stage hierarchy before and after negotiation, with a list of actors that were negotiated set the LOG_RELAYOUT_CONTROLLER environment variable to 3,true.

E.g. On desktop run:

$  LOG_RELAYOUT_CONTROLLER=3,true dali-demo

Example output from the logging is as follows:

PushButton, OKAY_BUTTON - Pos: [185, 0, 0.1] Size: [165, 76, 76], Dirty: (FALSE,FALSE), Negotiated: (TRUE,TRUE), Enabled: TRUE, (0x1649850)

The format is as follows:

[Actor type], [Actor name] ? Pos:[X, Y, Z] Size[Dimension::WIDTH, Dimension::HEIGHT, DEPTH], Dirty:(Dimension::WIDTH, Dimension::HEIGHT), Negotiated: (Dimension::WIDTH, Dimension::HEIGHT), Enabled: BOOLEAN, (Object address)
- <i>Actor type</i>: The type name of the actor E.g. PushButton
- <i>Actor name</i>: The name set on the actor with SetName(). Useful for debugging.
- <i>Pos</i>: The position of the actor
- <i>Size</i>: The current size of the actor. Check this to see if the actor has been negotiated correctly.
- <i>Dirty</i>: Booleans to say if the width or height has been marked as dirty by the relayout dirty flag propagation algorithm
- <i>Negotiated</i>: Booleans to say if the width or height has been negotiated by the size negotiation algorithm
- <i>Enabled</i>: Boolean to say if the actor is enabled for size negotitation
- <i>Object address</i>: The address of the actor object in memory
*
*/
