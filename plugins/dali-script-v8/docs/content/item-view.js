/**
 *
## ItemView API

 ItemView is a scrollable layout container with built-in layouts to determine
 the logical position of each item in a layout.
 
 Actors are provided from an external {{#crossLink "ItemFactory"}}ItemFactory{{/crossLink}},
 to display the currently visible items. ItemFactory is for storing the data of ItemView and
 creating actors for ItemView on request. Each item in ItemView is identified by a unique ID,
 and has a linear order from 0.
 
 ### Example of creating an ItemView (see {{#crossLink "ItemFactory"}}ItemFactory{{/crossLink}} API for a full example)
 
```
  // Define the data of 100 items
  var itemViewData = [];
  for (var itemId = 0; itemId < 100; itemId++)
  {
    var itemData = {};
    itemData["template"] = "template-item";
    itemData["title_text"] = "Item " + itemId;
    itemViewData[itemId] = itemData;
  }
 
  // Create an item factory and set the JSON template file and item view data
  var itemFactory = new dali.ItemFactory();
  itemFactory.jsonTemplateFile = "./item-template.json"; // Set the JSON template file
  itemFactory.data = itemViewData; // Set the ItemView data
 
  // Create the item view with the given item factory
  var itemView = new dali.Control("ItemView", itemFactory);
  itemView.size = [stageSize.x, stageSize.y, 0.0];
  itemView.parentOrigin = dali.CENTER_LEFT;
  itemView.anchorPoint = dali.CENTER_LEFT;
  dali.stage.add( itemView );
 
  // Add a scroll bar to ItemView (optional)
  var scrollBar = new dali.Control("ScrollBar");
  scrollBar.parentOrigin = dali.TOP_RIGHT;
  scrollBar.anchorPoint = dali.TOP_RIGHT;
  scrollBar.widthResizePolicy = "FIT_TO_CHILDREN";
  scrollBar.heightResizePolicy = "FILL_TO_PARENT";
  scrollBar.indicatorHeightPolicy = "Fixed";
  scrollBar.indicatorFixedHeight = 60.0;
  itemView.add(scrollBar);
 
  // Add a list layout to ItemView (multiple layouts can be added to the same ItemView)
  itemView.addLayout(dali.ITEM_LAYOUT_LIST);
 
  // Set custom item size for the list layout
  // If set, this will overide the predefined item size in the list layout
  itemView.setItemSize(0, [350, 100, 0]); // 0 means the first layout added to ItemView
 
  // Acticate the list layout (which will layout the items as a list)
  itemView.activateLayout(0, itemView.size); // 0 means the first layout added to ItemView
```
 
 @class ItemView
 @extends Actor

*/
