<!--
/**-->

# Item View {#item-view}

An Item view is a scrollable container that contains several items.
It can have several layouts.
There are a few built-in layouts that the application writer can use:

|GRID                    |SPIRAL                    |DEPTH                    |
|:----------------------:|:------------------------:|:-----------------------:|
|![ ](item-view/grid.png)|![ ](item-view/spiral.png)|![ ](item-view/depth.png)|

The application writer can also create their own custom layout by inheriting from Dali::Toolkit::ItemLayout.

## Item Factory

To create an item-view, the application writer has to provide an item-factory.
An ItemFactory provides methods to create items and how many items there are among other things.

~~~{.cpp}
class MyFactory : public Dali::Toolkit::ItemFactory
{
public:
  virtual unsigned int GetNumberOfItems()
  {
    // Should return the number of items
    return MY_ITEM_COUNT;
  }

  virtual Actor NewItem( unsigned int itemId )
  {
    // We should create the actor here that represents our item based on the itemId given.

    // Here we'll create an ImageView which uses the the itemId to parse the image in a particular directory.
    std::ostringstream imageName;
    imageName << "my-image-folder/" << itemId << ".png"; // If item was 10, then this would result in my-image-folder/10.png

    // Create the Image View from the image and return
    return Dali::Toolkit::ImageView::New( imageName.str() );
  }
};
~~~
These overridden methods in our factory will be called by the Item View.

## Creating an ItemView

~~~{.cpp}
MyFactory factory; // Should store this as a member variable
Dali::Toolkit::ItemView itemView = Dali::Toolkit::ItemView::New( factory ); // Pass in our factory
itemView.SetParentOrigin( ParentOrigin::CENTER );
itemView.SetAnchorPoint( AnchorPoint::CENTER );

// Now create a layout
Dali::Toolkit::ItemLayoutPtr spiralLayout = Dali::Toolkit::DefaultItemLayout::New( Dali::Toolkit::DefaultItemLayout::SPIRAL );

// ... and add the layout to the item view
itemView.AddLayout( spiralLayout );

// More layouts can be created and added to the item-view

// Activate the layout
itemView.ActivateLayout(
  0,                                   // The layout ID matches the order in which layouts are added
  Dali::Stage::GetCurrent().GetSize(), // Use the stage's size as our item-view size
  0 );                                 // We want the item-view to appear straight away

// And add to the stage
Dali::Stage::GetCurrent().Add( itemView );
~~~

*/