<!--
/**-->

# Flex Container  {#flex-container}

Flexbox is a CSS3 web layout model which allows responsive elements within a container, automatically arranged to different size screens or devices.
 
FlexContainer implements a subset of the Flexbox spec (defined by W3C) at: https://www.w3.org/TR/css-flexbox-1/
 
The flex container has the ability to alter the width and/or height of its children (i.e. flex items) to best fill the available space on any display device.
The container expands items to fill available free space, or shrinks them to prevent overflow.
 
Below is an illustration of the various directions and terms as applied to a flex container with the "flex direction" defined as "row".
 
![ ](../assets/img/flex-container/flex-container.jpg)
![ ](flex-container/flex-container.jpg)
 
DALi supports the following subset of Flexbox properties.
 
## Properties supported by flex container:

 + [contentDirection](@ref content-direction)
 + [flexDirection](@ref flex-direction)
 + [flexWrap](@ref flex-wrap)
 + [justifyContent](@ref justify-content)
 + [alignItems](@ref align-items)
 + [alignContent](@ref align-content)
 
___________________________________________________________________________________________________

## contentDirection {#content-direction}
 
contentDirection specifies the primary direction in which content is ordered on a line.
 
| LTR (left-to-right) | RTL (right-to-left) |
|--------|--------|
| ![ ](../assets/img/flex-container/content-direction-ltr.jpg) ![ ](flex-container/content-direction-ltr.jpg) | ![ ](../assets/img/flex-container/content-direction-rtl.jpg) ![ ](flex-container/content-direction-rtl.jpg) |
 
The possible values for this property are:
 
| Property Value | Description                                 |
|----------------|---------------------------------------------|
| INHERIT        | Inherits the same direction from the parent |
| LTR            | From left to right                          |
| RTL            | From right to left                          |
 
### Usage

~~~{.cpp}
// C++
Dali::Toolkit::FlexContainer flexContainer = Dali::Toolkit::FlexContainer::New();
flexContainer.SetProperty( Dali::Toolkit::FlexContainer::Property::CONTENT_DIRECTION, Dali::Toolkit::FlexContainer::RTL );
~~~

~~~{.js}
// JavaScript
var flexContainer = new dali.Control("FlexContainer");
flexContainer.contentDirection = "RTL";
~~~
___________________________________________________________________________________________________

## flexDirection {#flex-direction}
 
flexDirection specifies the direction of the main axis which determines the direction that flex items are laid out.
 
![ ](../assets/img/flex-container/flex-direction.jpg)
![ ](flex-container/flex-direction.jpg)
 
The possible values for this property are:
 
| Property Value | Description                                 |
|----------------|---------------------------------------------|
| COLUMN         | The flex items are laid out vertically as a column                          |
| COLUMN_REVERSE | The flex items are laid out vertically as a column, but in reverse order    |
| ROW            | The flex items are laid out horizontally as a row                       |
| ROW_REVERSE    | The flex items are laid out horizontally as a row, but in reverse order |
 
### Usage

~~~{.cpp}
// C++
Dali::Toolkit::FlexContainer flexContainer = Dali::Toolkit::FlexContainer::New();
flexContainer.SetProperty( Dali::Toolkit::FlexContainer::Property::FLEX_DIRECTION, Dali::Toolkit::FlexContainer::ROW_REVERSE );
~~~

~~~{.js}
// JavaScript
var flexContainer = new dali.Control("FlexContainer");
flexContainer.flexDirection = "rowReverse";
~~~
___________________________________________________________________________________________________

## flexWrap {#flex-wrap}
 
flexWrap specifies whether the flex items should wrap or not if there is no enough room for them on one flex line.
 
![ ](../assets/img/flex-container/flex-wrap.jpg)
![ ](flex-container/flex-wrap.jpg)
 
The possible values for this property are:
 
| Property Value | Description                                 |
|----------------|---------------------------------------------|
| NO_WRAP        | Flex items laid out in single line (shrunk to fit the flex container along the main axis) |
| WRAP           | Flex items laid out in multiple lines if needed                                           |
 
### Usage

~~~{.cpp}
// C++
Dali::Toolkit::FlexContainer flexContainer = Dali::Toolkit::FlexContainer::New();
flexContainer.SetProperty( Dali::Toolkit::FlexContainer::Property::FLEX_WRAP, Dali::Toolkit::FlexContainer::NO_WRAP );
~~~

~~~{.js}
// JavaScript
var flexContainer = new dali.Control("FlexContainer");
flexContainer.flexWrap = "noWrap";
~~~
___________________________________________________________________________________________________

## justifyContent {#justify-content}
 
justifyContent specifies the alignment of flex items when they do not use all available space on the main axis.
 
![ ](../assets/img/flex-container/justify-content.jpg)
![ ](flex-container/justify-content.jpg)
 
The possible values for this property are:
 
| Property Value | Description                                 |
|----------------|---------------------------------------------|
| JUSTIFY_FLEX_START      | Items are positioned at the beginning of the container                     |
| JUSTIFY_CENTER          | Items are positioned at the center of the container                        |
| JUSTIFY_FLEX_END        | Items are positioned at the end of the container                           |
| JUSTIFY_SPACE_BETWEEN   | Items are positioned with equal space between the lines                    |
| JUSTIFY_SPACE_AROUND    | Items are positioned with equal space before, between, and after the lines |
 
### Usage

~~~{.cpp}
// C++
Dali::Toolkit::FlexContainer flexContainer = Dali::Toolkit::FlexContainer::New();
flexContainer.SetProperty( Dali::Toolkit::FlexContainer::Property::JUSTIFY_CONTENT, Dali::Toolkit::FlexContainer::JUSTIFY_SPACE_BETWEEN );
~~~

~~~{.js}
// JavaScript
var flexContainer = new dali.Control("FlexContainer");
flexContainer.justifyContent = "spaceBetween";
~~~
___________________________________________________________________________________________________

## alignItems {#align-items}
 
alignItems specifies the alignment of flex items when they do not use all available space on the cross axis.
 
![ ](../assets/img/flex-container/align-items.jpg)
![ ](flex-container/align-items.jpg)
 
The possible values for this property are:
 
| Property Value | Description                                 |
|----------------|---------------------------------------------|
| ALIGN_FLEX_START | Items are aligned at the beginning of the container |
| ALIGN_CENTER     | Items are aligned at the center of the container    |
| ALIGN_FLEX_END   | Items are aligned at the end of the container       |
| ALIGN_STRETCH    | Items are stretched to fit the container            |
 
### Usage

~~~{.cpp}
// C++
Dali::Toolkit::FlexContainer flexContainer = Dali::Toolkit::FlexContainer::New();
flexContainer.SetProperty( Dali::Toolkit::FlexContainer::Property::ALIGN_ITEMS, Dali::Toolkit::FlexContainer::ALIGN_FLEX_START );
~~~

~~~{.js}
// JavaScript
var flexContainer = new dali.Control("FlexContainer");
flexContainer.alignItems = "flexStart";
~~~
___________________________________________________________________________________________________

## alignContent {#align-content}
 
alignContent specifies the alignment of flex lines when they do not use all available space on the cross axis, so only works when there are multiple lines.
 
![ ](../assets/img/flex-container/align-content.jpg)
![ ](flex-container/align-content.jpg)
 
The possible values for this property are:
 
| Property Value | Description                                 |
|----------------|---------------------------------------------|
| ALIGN_FLEX_START | Items are aligned at the beginning of the container |
| ALIGN_CENTER     | Items are aligned at the center of the container    |
| ALIGN_FLEX_END   | Items are aligned at the end of the container       |
 
### Usage

~~~{.cpp}
// C++
Dali::Toolkit::FlexContainer flexContainer = Dali::Toolkit::FlexContainer::New();
flexContainer.SetProperty( Dali::Toolkit::FlexContainer::Property::ALIGN_CONTENT, Dali::Toolkit::FlexContainer::ALIGN_FLEX_END );
~~~

~~~{.js}
// JavaScript
var flexContainer = new dali.Control("FlexContainer");
flexContainer.alignContent = "flexEnd";
~~~
___________________________________________________________________________________________________

## Custom properties supported by flex item:

 + [flex](@ref flex)
 + [alignSelf](@ref align-self)
 + [flexMargin](@ref flex-margin)
 
These non-animatable properties are registered dynamically to each child which would be added to the flex container, and once added their values can not be changed.
 
When an actor is added to the flex container, these properties are checked to decide how to lay out the actor inside the flex container.
 
___________________________________________________________________________________________________

## flex {#flex}
 
By default, the items in the flex container are not flexible. If set, this property makes the item flexible, which means the item can alter its width/height in order to receive the specified proportion of the free space in the flex container.
If all items in the flex container use this pattern, their sizes will be proportional to the specified flex factor.
Flex items will not shrink below their minimum size (if set using Dali::Actor::SetMinimumSize).
 
![ ](../assets/img/flex-container/flex.jpg)
![ ](flex-container/flex.jpg)
 

### Usage

Below is the example code for the items to achieve the proportion of free space as illustrated above.
 
~~~{.cpp}
// C++

// Create the flex container
Dali::Toolkit::FlexContainer flexContainer = Dali::Toolkit::FlexContainer::New();

// Set the flex direction to lay out the items horizontally
flexContainer.SetProperty( Dali::Toolkit::FlexContainer::Property::FLEX_DIRECTION, Dali::Toolkit::FlexContainer::ROW );

// Create flex items and set the proportion
Dali::Toolkit::Control item1 = Dali::Toolkit::Control::New();
item1.SetProperty( Dali::Toolkit::FlexContainer::ChildProperty::FLEX, 1.0f );
flexContainer.Add( item1 );

Dali::Toolkit::Control item2 = Dali::Toolkit::Control::New();
item2.SetProperty( Dali::Toolkit::FlexContainer::ChildProperty::FLEX, 3.0f );
flexContainer.Add( item2 );

Dali::Toolkit::Control item3 = Dali::Toolkit::Control::New();
item3.SetProperty( Dali::Toolkit::FlexContainer::ChildProperty::FLEX, 1.0f );
flexContainer.Add( item3 );

Dali::Toolkit::Control item4 = Dali::Toolkit::Control::New();
item4.SetProperty( Dali::Toolkit::FlexContainer::ChildProperty::FLEX, 2.0f );
flexContainer.Add( item4 );

Dali::Toolkit::Control item5 = Dali::Toolkit::Control::New();
item5.SetProperty( Dali::Toolkit::FlexContainer::ChildProperty::FLEX, 1.0f );
flexContainer.Add( item5 );

~~~

~~~{.js}
// JavaScript

// Create the flex container
var flexContainer = new dali.Control("FlexContainer");

// Set the flex direction to lay out the items horizontally
flexContainer.flexDirection = "row";

// Create flex items and set the proportion
var item1 = new dali.Control();
item1.registerCustomProperty("flex", 1.0, dali.PROPERTY_READ_WRITE);
flexContainer.add(item1);

var item2 = new dali.Control();
item2.registerCustomProperty("flex", 3.0, dali.PROPERTY_READ_WRITE);
flexContainer.add(item2);

var item3 = new dali.Control();
item3.registerCustomProperty("flex", 1.0, dali.PROPERTY_READ_WRITE);
flexContainer.add(item3);

var item4 = new dali.Control();
item4.registerCustomProperty("flex", 2.0, dali.PROPERTY_READ_WRITE);
flexContainer.add(item4);

var item5 = new dali.Control();
item5.registerCustomProperty("flex", 1.0, dali.PROPERTY_READ_WRITE);
flexContainer.add(item5);

~~~
___________________________________________________________________________________________________

## alignSelf {#align-self}
 
alignSelf specifies how the item will align along the cross axis, if set, this overrides the default alignment for all items defined by the container’s [alignItems](@ref align-items) property.
 
![ ](../assets/img/flex-container/align-self.jpg)
![ ](flex-container/align-self.jpg)
 
The possible values for this property are:
 
| Property Value | Description                                 |
|----------------|---------------------------------------------|
| ALIGN_AUTO       | Items inherit the same alignment from the parent by default (as specified by the container’s [alignItems](@ref align-items) property) |
| ALIGN_FLEX_START | Items are aligned at the beginning of the container |
| ALIGN_CENTER     | Items are aligned at the center of the container    |
| ALIGN_FLEX_END   | Items are aligned at the end of the container       |
| ALIGN_STRETCH    | Items are stretched to fit the container            |
 
### Usage

Below is the example code for the items to achieve the alignment on the cross axis as illustrated above.
 
~~~{.cpp}
// C++

// Create the flex container
Dali::Toolkit::FlexContainer flexContainer = Dali::Toolkit::FlexContainer::New();

// Set the flex direction to lay out the items horizontally
flexContainer.SetProperty( Dali::Toolkit::FlexContainer::Property::FLEX_DIRECTION, Dali::Toolkit::FlexContainer::ROW );

// Set the items to be aligned at the beginning of the container on the cross axis by default
flexContainer.SetProperty( Dali::Toolkit::FlexContainer::Property::ALIGN_ITEMS, Dali::Toolkit::FlexContainer::ALIGN_FLEX_START );

// Create flex items and add them to the flex container
Dali::Toolkit::Control item1 = Dali::Toolkit::Control::New();
item1.SetProperty( Dali::Toolkit::FlexContainer::ChildProperty::ALIGN_SELF, Dali::Toolkit::FlexContainer::ALIGN_CENTER ); // Align item1 at the center of the container
flexContainer.Add( item1 );

Dali::Toolkit::Control item2 = Dali::Toolkit::Control::New();
flexContainer.Add( item2 ); // item2 is aligned at the beginning of ther container

Dali::Toolkit::Control item3 = Dali::Toolkit::Control::New();
item3.SetProperty( Dali::Toolkit::FlexContainer::ChildProperty::ALIGN_SELF, Dali::Toolkit::FlexContainer::ALIGN_FLEX_END ); // Align item3 at the bottom of the container
flexContainer.Add( item3 );

Dali::Toolkit::Control item4 = Dali::Toolkit::Control::New();
flexContainer.Add( item4 ); // item4 is aligned at the beginning of ther container

~~~

~~~{.js}
// JavaScript

// Create the flex container
var flexContainer = new dali.Control("FlexContainer");

// Set the flex direction to lay out the items horizontally
flexContainer.flexDirection = "row";

// Set the items to be aligned at the beginning of the container on the cross axis by default
flexContainer.alignItems = "flexStart";

// Create flex items and add them to the flex container
var item1 = new dali.Control();
item1.registerCustomProperty("alignSelf", "center", dali.PROPERTY_READ_WRITE); // Align item1 at the center of the container
flexContainer.add(item1);

var item2 = new dali.Control();
flexContainer.add(item2); // item2 is aligned at the beginning of ther container

var item3 = new dali.Control();
item1.registerCustomProperty("alignSelf", "flexEnd", dali.PROPERTY_READ_WRITE); // Align item3 at the bottom of the container
flexContainer.add(item3);

var item4 = new dali.Control();
flexContainer.add(item4); // item4 is aligned at the beginning of ther container

~~~
___________________________________________________________________________________________________

## flexMargin {#flex-margin}
 
Each flex item inside the flex container is treated as a box (in CSS term) which is made up of:

 + content: The content of the item.
 + padding: The space around the content (inside the border) of the item.
 + border: The border that goes around the padding and the content of the item.
 + flexMargin: The space outside the border.
 
![ ](../assets/img/flex-container/flex-margin.jpg)
![ ](flex-container/flex-margin.jpg)
 
In DALi, the size of the flex item = content size + padding + border.
 
flexMargin specifies the space around the flex item.
 
### Usage

~~~{.cpp}
// C++

// Create the flex container
Dali::Toolkit::FlexContainer flexContainer = Dali::Toolkit::FlexContainer::New();

// Create flex item
Dali::Toolkit::Control item = Dali::Toolkit::Control::New();

// Add the margin around the item
item.SetProperty( Dali::Toolkit::FlexContainer::ChildProperty::FLEX_MARGIN, Vector4(10.0f, 10.0f, 10.0f, 10.0f) );

// Add the item to the container
flexContainer.Add( item );
~~~

~~~{.js}
// JavaScript

// Create the flex container
var flexContainer = new dali.Control("FlexContainer");

// Create flex items
var item = new dali.Control();

// Add the margin around the item
item.registerCustomProperty("flexMargin", [10.0, 10.0, 10.0, 10.0], dali.PROPERTY_READ_WRITE);

// Add the item to the container
flexContainer.add(item);
~~~

___________________________________________________________________________________________________

## Example of creating Flexbox layout using FlexContainer

Now let's see how to create a Gallery like layout (as shown below) using FlexContainer.

![ ](../assets/img/flex-container/flexbox-demo.jpg)
![ ](flex-container/flexbox-demo.jpg)
 
Firstly, we create a flex container as the whole view and set its resize policy to fill its parent (i.e. the stage).

~~~{.cpp}
// C++

// Create the main flex container
Dali::Toolkit::FlexContainer flexContainer = Dali::Toolkit::FlexContainer::New();
flexContainer.SetParentOrigin( Dali::ParentOrigin::TOP_LEFT );
flexContainer.SetAnchorPoint( Dali::AnchorPoint::TOP_LEFT );
flexContainer.SetResizePolicy( Dali::ResizePolicy::FILL_TO_PARENT, Dali::Dimension::ALL_DIMENSIONS );
flexContainer.SetBackgroundColor( Dali::Color::WHITE ); // set the background color to be white

// Add it to the stage
Dali::Stage::GetCurrent().Add( flexContainer );
~~~

~~~{.js}
// JavaScript

// Create the main flex container
var flexContainer = new dali.Control("FlexContainer");
flexContainer.parentOrigin = dali.TOP_LEFT;
flexContainer.anchorPoint = dali.TOP_LEFT;
flexContainer.widthResizePolicy = "FILL_TO_PARENT";
flexContainer.heightResizePolicy = "FILL_TO_PARENT";
flexContainer.backgroundColor = dali.COLOR_WHITE; // set the background color to be white

// Add it to the stage
dali.stage.add( flexContainer );
~~~
 
We want to set the flex direction of this main container to column, as we want the toolbar and the actual content to be displayed vertically.
 
~~~{.cpp}
// C++

// Display toolbar and content vertically
flexContainer.SetProperty( Dali::Toolkit::FlexContainer::Property::FLEX_DIRECTION, Dali::Toolkit::FlexContainer::COLUMN );
~~~

~~~{.js}
// JavaScript

// Display toolbar and content vertically
flexContainer.flexDirection = "column";
~~~
 
Now we create a flex container as the toolbar and add it to the main container. Because the flex direction in the main container is column, the toolbar will be arranged on the top of the main container.
 
~~~{.cpp}
// C++

// Create the toolbar
Dali::Toolkit::FlexContainer toolBar = Dali::Toolkit::FlexContainer::New();
toolBar.SetParentOrigin( Dali::ParentOrigin::TOP_LEFT );
toolBar.SetAnchorPoint( Dali::AnchorPoint::TOP_LEFT );
toolBar.SetBackgroundColor( Dali::Color::CYAN ); // Set the background color for the toolbar

// Add it to the main container
flexContainer.Add( toolBar );
~~~

~~~{.js}
// JavaScript

// Create the toolbar area
var toolBar = new dali.Control("FlexContainer");
toolBar.parentOrigin = dali.TOP_LEFT;
toolBar.anchorPoint = dali.TOP_LEFT;
toolBar.backgroundColor = dali.COLOR_CYAN; // Set the background color for the toolbar

// Add it to the main container
flexContainer.add(toolBar);
~~~
 
We want the buttons and title to be displayed horizontally and vertically aligned to the center of the toolbar, so we set its flex direction to row and set its alignItems property to center.
We also want the toolbar and the actual content to share the height of the main container, so that the toolbar will occupy 10 percent of the whole vertical space and the content will occupy the rest of the vertical space.
This can be achieved by setting the flex property.
 
~~~{.cpp}
// C++

toolBar.SetProperty( Dali::Toolkit::FlexContainer::Property::FLEX_DIRECTION, Dali::Toolkit::FlexContainer::ROW ); // display toolbar items horizontally
toolBar.SetProperty( Dali::Toolkit::FlexContainer::Property::ALIGN_ITEMS, Dali::Toolkit::FlexContainer::ALIGN_CENTER ); // align toolbar items vertically center
toolBar.SetProperty( Dali::Toolkit::FlexContainer::ChildProperty::FLEX, 0.1f ); // 10 percent of available space in the cross axis
~~~

~~~{.js}
// JavaScript

toolBar.flexDirection = "row"; // display toolbar items horizontally
toolBar.alignItems = "center"; // align toolbar items vertically center
toolBar.registerCustomProperty("flex", 0.1, dali.PROPERTY_READ_WRITE); // 10 percent of available space in the cross axis
~~~
 
Then we create another flex container as the content area to display the image, and it will be displayed in the bottom of the main container.
We want the item inside it to be horizontally and vertically centered, so that the image will always be in the center of the content area.

~~~{.cpp}
// C++

// Create the content area
Dali::Toolkit::FlexContainer content = Dali::Toolkit::FlexContainer::New();
content.SetParentOrigin( Dali::ParentOrigin::TOP_LEFT );
content.SetAnchorPoint( Dali::AnchorPoint::TOP_LEFT );
content.SetProperty( Dali::Toolkit::FlexContainer::Property::FLEX_DIRECTION, Dali::Toolkit::FlexContainer::ROW ); // display items horizontally
content.SetProperty( Dali::Toolkit::FlexContainer::Property::JUSTIFY_CONTENT, Dali::Toolkit::FlexContainer::JUSTIFY_CENTER ); // align items horizontally center
content.SetProperty( Dali::Toolkit::FlexContainer::Property::ALIGN_ITEMS, Dali::Toolkit::FlexContainer::ALIGN_CENTER ); // align items vertically center
content.SetProperty( Dali::Toolkit::FlexContainer::ChildProperty::FLEX, 0.9f ); // 90 percent of available space in the cross axis

// Add it to the main container
flexContainer.Add( content );
~~~

~~~{.js}
// JavaScript

// Create the content area
var content = new dali.Control("FlexContainer");
content.parentOrigin = dali.TOP_LEFT;
content.anchorPoint = dali.TOP_LEFT;
content.flexDirection = "row";
content.alignItems = "center"; // align items vertically center
content.justifyContent = "center"; // align items horizontally center
content.registerCustomProperty("flex", 0.9, dali.PROPERTY_READ_WRITE); // 90 percent of available space in the cross axis

// Add it to the main container
flexContainer.add(content);
~~~
 
Now we start to add items to the toolbar. The toolbar will have one button on the left, one button on the right, and a title always in the center (regardless of the screen size).
To achieve that, we can simply make the title flexible so that it will automatically take all the available horizontal space left.
We will also add some space around the items so that the layout looks nicer.
 
~~~{.cpp}
// C++

// Add a button to the left of the toolbar
Dali::Toolkit::PushButton prevButton = Dali::Toolkit::PushButton::New();
prevButton.SetParentOrigin( Dali::ParentOrigin::TOP_LEFT );
prevButton.SetAnchorPoint( Dali::AnchorPoint::TOP_LEFT );
prevButton.SetMinimumSize( Dali::Vector2( 100.0f, 60.0f ) ); // this is the minimum size the button should keep
prevButton.SetProperty( Dali::Toolkit::FlexContainer::ChildProperty::FLEX_MARGIN, Dali::Vector4(10.0f, 10.0f, 10.0f, 10.0f) ); // set 10 pixel margin around the button
toolBar.Add( prevButton );

// Set the button text
Dali::Property::Map labelMap;
labelMap[ "text" ]      = "Prev";
labelMap[ "textColor" ] = Dali::Color::BLACK;
prevButton.SetProperty( Dali::Toolkit::Button::Property::LABEL, labelMap );

// Add a title to the center of the toolbar
Dali::Toolkit::TextLabel title = Dali::Toolkit::TextLabel::New( "Gallery" );
title.SetParentOrigin( Dali::ParentOrigin::TOP_LEFT );
title.SetAnchorPoint( Dali::AnchorPoint::TOP_LEFT );
title.SetResizePolicy( Dali::ResizePolicy::USE_NATURAL_SIZE, Dali::Dimension::ALL_DIMENSIONS );
title.SetProperty( Dali::Toolkit::TextLabel::Property::HORIZONTAL_ALIGNMENT, "CENTER" );
title.SetProperty( Dali::Toolkit::TextLabel::Property::VERTICAL_ALIGNMENT, "CENTER" );
title.SetProperty( Dali::Toolkit::FlexContainer::ChildProperty::FLEX, 1.0f ); // take all the available space left apart from the two buttons
title.SetProperty( Dali::Toolkit::FlexContainer::ChildProperty::FLEX_MARGIN, Dali::Vector4(10.0f, 10.0f, 10.0f, 10.0f) ); // set 10 pixel margin around the title
toolBar.Add( title );

// Add a button to the right of the toolbar
Dali::Toolkit::PushButton nextButton = Dali::Toolkit::PushButton::New();
nextButton.SetParentOrigin( Dali::ParentOrigin::TOP_LEFT );
nextButton.SetAnchorPoint( Dali::AnchorPoint::TOP_LEFT );
nextButton.SetMinimumSize( Dali::Vector2( 100.0f, 60.0f ) ); // this is the minimum size the button should keep
nextButton.SetProperty( Dali::Toolkit::FlexContainer::ChildProperty::FLEX_MARGIN, Dali::Vector4(10.0f, 10.0f, 10.0f, 10.0f) ); // set 10 pixel margin around the button
toolBar.Add( nextButton );

// Set the button text
labelMap[ "text" ] = "Next";
nextButton.SetProperty( Dali::Toolkit::Button::Property::LABEL, labelMap );
~~~

~~~{.js}
// JavaScript

// Add a button to the left of the toolbar
var prevButton = new dali.Control("PushButton");
prevButton.name = "Prev";
prevButton.parentOrigin = dali.TOP_LEFT;
prevButton.anchorPoint = dali.TOP_LEFT;
prevButton.minimumSize = [100.0, 60.0]; // this is the minimum size the button should keep
prevButton.labelText = "Prev";
prevButton.registerCustomProperty("flexMargin", [10, 10, 10, 10], dali.PROPERTY_READ_WRITE); // set 10 pixel margin around the button

toolBar.add( prevButton );

// Add a title to the center of the toolbar
var title = new dali.Control("TextLabel");
title.parentOrigin = dali.TOP_LEFT;
title.anchorPoint = dali.TOP_LEFT;
title.widthResizePolicy = "USE_NATURAL_SIZE";
title.heightResizePolicy = "USE_NATURAL_SIZE";
title.horizontalAlignment = "CENTER";
title.verticalAlignment = "CENTER";
title.text = "Gallery";
title.registerCustomProperty("flex", 1.0, dali.PROPERTY_READ_WRITE); // take all the available space left apart from the two buttons
title.registerCustomProperty("flexMargin", [10, 10, 10, 10], dali.PROPERTY_READ_WRITE); // set 10 pixel margin around the title

toolBar.add( title );

// Add a button to the right of the toolbar
var nextButton = new dali.Control("PushButton");
nextButton.name = "Next";
nextButton.parentOrigin = dali.TOP_LEFT;
nextButton.anchorPoint = dali.TOP_LEFT;
nextButton.minimumSize = [100.0, 60.0]; // this is the minimum size the button should keep
nextButton.labelText = "Next";
nextButton.registerCustomProperty("flexMargin", [10, 10, 10, 10], dali.PROPERTY_READ_WRITE); // set 10 pixel margin around the button

toolBar.add( nextButton );
~~~
 
This is really neat when running on devices with different size or changing from different orientation, because the toolbar will expand or shrink based on the available space and the title will always be in the center, therefore the layout of the toolbar will keep the same.
 
Finally, we will add the image to the content area.
 
~~~{.cpp}
// C++

// Add an image to the center of the content area
Dali::Toolkit::ImageView imageView = Dali::Toolkit::ImageView::New( "image.jpg" );
imageView.SetParentOrigin( Dali::ParentOrigin::TOP_LEFT );
imageView.SetAnchorPoint( Dali::AnchorPoint::TOP_LEFT );
content.Add( imageView );
~~~

~~~{.js}
// JavaScript

// Add an image to the center of the content area
imageView = new dali.Control("ImageView");
imageView.image = "image.jpg";
imageView.parentOrigin = dali.TOP_LEFT;
imageView.anchorPoint = dali.TOP_LEFT;
content.add( imageView );
~~~
 
As you can see, it is easy to make flexible containers in DALi. We can use these concepts to create responsive layouts.
 
@class _Guide_Flex_Container

*/
