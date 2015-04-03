/*! \page size-negotiation-controls Size Negotiation for Controls
 *
<h2 class="pg">Overview</h2>

This document details how to create controls using the size negotiation API and is intended for UI control writters. For an introduction to
size negotiation please see the <i>Size Negotiation Programming Guide</i>.

The topics covered are:
- The Relayout Controller
- Resize Policies
- Creating a Control: Popups
- Size Negotiation API
- Creating a Control: TableView

<h2 class="pg">The Relayout Controller</h2>

<h3>Overview</h3>
The RelayoutController is an object that is private in DALi Core. It's main job is to take relayout requests from actors.
It can be enabled or disabled internally. If disabled, then all relayout requests are ignored. By default the relayout controller is disabled until just after the
initial application initialize. This allows the scene for an application to be created without generating many relayout requests. After the application
has initialized the scene, then the relayout controller is automatically enabled and a relayout request is called on the root of the scene. This request spreads down the scene
hierarchy and requests relayout on all actors that have size negotiation enabled.

Relayout requests are put in automatically when a property is changed on an actor or a change to the stage hierarchy is made and manual requests are usually not necessary.

<h2 class="pg">Resize Policies</h2>

In addition to the resize policies detailed in the Size Negotiation Programming Guide there is one additional policy available to control writers:

- USE_ASSIGNED_SIZE: Tells the actor to use the size that was passed into the size negotiation algorithm for it. This is used in the OnRelayout
method derived from Actor when passing back controls to be negotiated using the container argument to the method.

<h2 class="pg">Creating a Control: Popups</h2>

<h3>Initialization</h3>
Size negotiation is enabled on controls by default. If a control is desired to not have size negotiation enabled then simply pass in the
NO_SIZE_NEGOTIATION flag into the Control constructor. This will then call SetRelayoutEnabled( false ) on the base class.

The other step to perform is to set default resize policies for width and height.

<h3>A Simple Example: Popup</h3>

This example shows how to set up a popup for use with size negotiation. The popup contains a layer to raise it above all other controls,
a semi-transparent full-screen backing image to dim the screen, a background image with a shadow border, and buttons that are positioned
and resized by the popup. The following screen shot shows an example popup.

\image html size-negotiation/PopupExample.png

The first step is to set the default resize policies. This is done in the OnInitialize method. In the following snippet the popup
is set to have a height resize policy of FIT_TO_CHILDREN. This assumes that the width of the popup will be specified by the user of
the popup and that the desired behaviour is to fit the height of the popup to the size of its content.
@code
void Popup::OnInitialize()
...
Actor self = Self();
self.SetResizePolicy( FIT_TO_CHILDREN, HEIGHT );
@endcode
The popup will use a layer to place its content in. The layer is created and specified to fill the whole screen by using the following command.
@code
mLayer.SetResizePolicy( FILL_TO_PARENT, ALL_DIMENSIONS );
@endcode
A half transparent backing image is added to the layer and told to fill the layer with the following.
@code
mBacking.SetResizePolicy( FILL_TO_PARENT, ALL_DIMENSIONS );
@endcode
The popup control is added to the layer and a background image is specified to fill the size of the popup and add a border by the following.
@code
mBackgroundImage.SetResizePolicy( SIZE_FIXED_OFFSET_FROM_PARENT, ALL_DIMENSIONS );
Vector3 border( mPopupStyle->backgroundOuterBorder.x, mPopupStyle->backgroundOuterBorder.z, 0.0f );
mBackgroundImage.SetSizeModeFactor( border );
@endcode
A table view is added to the popup to specify layout. It will fill to the width of the popup and expand/contract around its children cell heights.
@code
mPopupLayout.SetResizePolicy( FILL_TO_PARENT, WIDTH );
mPopupLayout.SetResizePolicy( USE_NATURAL_SIZE, HEIGHT );
@endcode
Override the OnRelayout method to position and resize the buttons. The trick to this is that the buttons do not know they are part of a popup and are
about to be resized so could already have had their sizes negotiated. The call to PropagateRelayoutFlags on the button will ensure that it and all
its child dependents are ready for a new size negotiation pass. The container.Add call will add the button to the relayout queue to be processed this frame.
The buttonSize parameter is the desired size for the button. The desired size will only be set if the size policy of the button has already been changed to
USE_ASSIGNED_SIZE, which is what happens when a button is added to the popup.
@code
void Popup::AddButton( Toolkit::Button button )
...
button.SetResizePolicy( USE_ASSIGNED_SIZE, ALL_DIMENSIONS );
...

void Popup::OnRelayout( const Vector2& size, RelayoutContainer& container )
...
button.SetPosition( buttonPosition );

button.PropagateRelayoutFlags();    // Reset relayout flags for relayout
container.Add( button, buttonSize );
...
@endcode
Another aspect to the popup is that depending which resize policies are active on it then the inner table view requires different resize policies itself.
OnSetResizePolicy can be overridden to receive notice that the resize policy has changed on the control and action can be taken.
@code
void Popup::OnSetResizePolicy( ResizePolicy policy, Dimension dimension )
...
if( policy == FIT_TO_CHILDREN )
{
  // Make content fit to children
  mPopupLayout.SetResizePolicy( USE_NATURAL_SIZE, dimension );
  if( dimension & HEIGHT )
  {
    mPopupLayout.SetFitHeight( 1 );
  }
}
else
{
  mPopupLayout.SetResizePolicy( FILL_TO_PARENT, dimension );
  // Make the content cell fill the whole of the available space
  if( dimension & HEIGHT )
  {
    mPopupLayout.SetRelativeHeight( 1, 1.0f );
  }
}
@endcode
Popup also implements the following methods for use with the relevant resize policies:
- GetNaturalSize
- GetHeightForWidth
- GetWidthForHeight

<h2 class="pg">Size Negotiation API</h2>

<h3>Base Class Methods</h3>

The base class methods are used to call functionality held in Actor and are defined in CustomActorImpl.

There is a RelayoutRequest method defined. This method is available for deriving controls to call when they would like themselves to be relaid out.
@code void RelayoutRequest() @endcode

<h3>Overridable Methods</h3>
These overridable methods in control provide customization points for the size negotiation algorithm.

<h4>Responding to the Change of Size on a Control</h4>

OnRelayout is called during the relayout process at the end of the frame immediately after the new size has been set on the actor. If the actor has calculated
the size of child actors then add them to container with their desired size and set the USE_ASSIGNED_SIZE resize policy on them.
At this point the size of the actor has been calculated so it is a good place to calculate positions of child actors etc.
@code virtual void OnRelayout( const Vector2& size, RelayoutContainer& container ) @endcode

The OnRelayoutSignal signal is raised after SetSize and OnRelayout have been called during the relayout processing at the end of the frame. If the control is deriving
from Control then the OnRelayout virtual is preferred over this signal. The signal is provided for instance when custom code needs to be run on the
children of an actor that is not a control.
@code OnRelayoutSignalType& OnRelayoutSignal() @endcode

The OnCalculateRelayoutSize is called right before the size is calculated for an actor's dimension during the size negotiation phase. At this point all other actors this actor is
dependent on have been negotiated so calculations depending on these actors can be performed before the size for this actor is calculated. Useful for container size calculations.
@code virtual void OnCalculateRelayoutSize( Dimension dimension ) @endcode

OnLayoutNegotiated is called right after the size in a given dimension has been negotiated for an actor. This allows calculations to be performed in response to the change
in a given dimension but before OnRelayout is called.
@code virtual void OnLayoutNegotiated( float size, Dimension dimension ) @endcode

<h4>Calculating Sizes</h4>

Calculate the natural size for this control. This will be called when a control's resize policy is set to USE_NATURAL_SIZE.
For example, TableView will calculated the size of the table given its various cell properties.
@code virtual Vector3 GetNaturalSize() @endcode

Given an input width return the correct height for this control. This will be called when the resize policy is set to DIMENSION_DEPENDENCY and
height has a dependency on width.
@code virtual float GetHeightForWidth( float width ) @endcode

Given the input height return the correct width for this control. This will be called when the resize policy is set to DIMENSION_DEPENDENCY and
width has a dependency on height.
@code virtual float GetWidthForHeight( float height ) @endcode

<h4>Relayout Dependencies</h4>

Return true from this method if this control is dependent on any of its children to calculate its own size. All relayout containers that can be dependent on their
children for their own size need to return true from this.
@code virtual bool RelayoutDependentOnChildren( Dimension dimension = ALL_DIMENSIONS ) @endcode

This will be called by children when they are using the FILL_TO_PARENT resize policy. It is the parent's responsibility to calculate the child's correct size.
@code virtual float CalculateChildSize( const Dali::Actor& child, Dimension dimension ) @endcode

<h4>Events</h4>

OnSetResizePolicy is called when the resize policy is set on an actor. Allows deriving actors to respond to changes in resize policy.
@code virtual void OnSetResizePolicy( ResizePolicy policy, Dimension dimension ) @endcode

<h2 class="pg">Creating a Control: TableView</h2>

This section demonstrates how size negotiation may be used when creating a table view.

First we define some policies for how table row and columns may resize. These are:
- Fixed: Use a fixed size
- Relative: Use a ratio size of empty remaining space
- Fill: Fill up to all remaining space, distributing evenly between all "fill" row or columns

A data structure is defined to hold information for each row and column regarding their cell size policy and their assigned and calculated sizes.

We need to be able to calculate the fixed sizes of all actors placed into table cells. The place to do this is in OnCalculateRelayoutSize. When
this is called every actor the table view is dependent on has already had their sizes calculated. Calculations can be made that the main calculation
for the actor can then use.
@code
void TableView::OnCalculateRelayoutSize( Dimension dimension )
...
CalculateRowColumnData();

if( dimension & WIDTH )
{
  CalculateFixedSizes( mColumnData, WIDTH );
  mFixedTotals.width = CalculateTotalFixedSize( mColumnData );
}

if( dimension & HEIGHT )
{
  CalculateFixedSizes( mRowData, HEIGHT );
  mFixedTotals.height = CalculateTotalFixedSize( mRowData );
}
...
@endcode

An important override is GetNaturalSize. This will simply return the total sum of the fixed cells for each row and column.
@code
Vector3 TableView::GetNaturalSize()
...
return Vector3( mFixedTotals.width, mFixedTotals.height, 1.0f );
...
@endcode

When the time comes to calculate the size of each child in the table cells the following method will be called.
@code
float TableView::CalculateChildSize( const Actor& child, Dimension dimension )
...
// Use cell data to calculate child size
@endcode

The table view is dependent on its children if its size policy is set to USE_NATURAL_SIZE or a row or column is set to "fit" an actor.
The following code shows calling the base class RelayoutDependentOnChildren to check the resize policy and then searches for fit row or columns.
@code
bool TableView::RelayoutDependentOnChildren( Dimension dimension )
{
  if ( Control::RelayoutDependentOnChildren( dimension ) )
  {
    return true;
  }

  return FindFit( mRowData ) || FindFit( mColumnData );
}
@endcode

With the cell sizes already calculated, the job of OnRelayout is to position all the actors in the table view in their respective positions.
@code
void TableView::OnRelayout( const Vector2& size, RelayoutContainer& container )
...
// Find each actor and position it, taking padding into account
@endcode

*
*/
