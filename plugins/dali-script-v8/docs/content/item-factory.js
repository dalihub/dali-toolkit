/**
 *
## ItemFactory API

 ItemFactory is for storing the data of {{#crossLink "ItemView"}}ItemView{{/crossLink}}
 and creating actors for ItemView on request. Each item in ItemView is identified by a
 unique ID, and has a linear order from 0.
 
 A JSON file should be provided to ItemFactory which defines the templates of items
 to be used to create the actors. Multiple templates can be defined in the JSON file
 for different type of items.
 
### Simple example of creating a JSON template for items
 
```
  {
    "templates":
    {
      "template-item":
      {
        "name":"item",
        "type":"Actor",
        "position":[0,0,0],
        "anchorPoint":"TOP_LEFT",
        "parentOrigin":"TOP_LEFT",
        "actors":
         [
          {
            "name":"icon",
            "type":"ImageView",
            "image":
            {
              "rendererType" : "image",
              "url": "{icon_path}"
            },
            "position":[20.0, 0.0, 0.0],
            "size":[70.0, 70.0, 0.0],
            "color":[1.0,1.0,1.0,1.0],
            "anchorPoint":"CENTER_LEFT",
            "parentOrigin":"CENTER_LEFT",
            "actors":
            [
              {
                "name":"title",
                "anchorPoint":"CENTER_LEFT",
                "parentOrigin":"CENTER_RIGHT",
                "type":"TextLabel",
                "position": [30.0, 0.0, 0.0],
                "size":[200.0, 70.0, 0.0],
                "pointSize":30,
                "fontFamily":"HelveticaNeue",
                "fontStyle":"Bold",
                "horizontalAlignment":"BEGIN",
                "verticalAlignment":"CENTER",
                "textColor": [1.0,0.0,1.0,1.0],
                "text":"{title_text}"
              }
            ]
          }
        ]
      }
    }
  }
```
 
 The data of items should be provided to ItemFactory as an array of property maps
 in which each map contains the data for each item, including the template to be used
 to build the actor and the pairs of key/value to be used to replace the constants
 defined in the template. The order of property maps in the array represents the actual
 order of items in ItemView.
 
 ### Example of defining the data of an ItemView with two items
 
```
  var itemViewData = [
                       { "template" : "template-item",
                         "icon_path" : "icon0.png",
                         "title_text" : "Item 0" },
                       { "template" : "template-item",
                         "icon_path" : "icon1.png",
                         "title_text" : "Item 1" }
                     ];
```
 
 This means ItemFactory will use the template "template-item" defined in the JSON file
 to create the item for ItemView and replace the constants "icon_path" and "title_text"
 in the template with their actual values, e.g. "icon0.png" and "Item 0". Each item can
 have different template and different data.
 
 ### Example of creating an ItemFactory with the above JSON template and link it with an ItemView

![ ](../assets/img/item-view/list.png)

```
  // Define the data of 100 items
  var itemViewData = [];
  for (var itemId = 0; itemId < 100; itemId++)
  {
    var itemData = {};
    itemData["template"] = "template-item";
    itemData["icon_path"] = "icon" + itemId + ".png";
    itemData["title_text"] = "Item " + itemId;
    itemViewData[itemId] = itemData;
  }
 
  // Create the item factory and set the JSON template file and item view data
  var itemFactory = new dali.ItemFactory();
  itemFactory.jsonTemplateFile = "./item-template.json"; // Set the JSON template file
  itemFactory.data = itemViewData; // Set the ItemView data
 
  // Create the item view with the given item factory
  var itemView = new dali.Control("ItemView", itemFactory);
  itemView.size = [stageSize.x, stageSize.y, 0.0];
  itemView.parentOrigin = dali.CENTER_LEFT;
  itemView.anchorPoint = dali.CENTER_LEFT;
  dali.stage.add( itemView );

  // Add a list layout to ItemView (multiple layouts can be added to the same ItemView)
  itemView.addLayout(dali.ITEM_LAYOUT_LIST);
 
  // Set custom item size for the list layout
  // If set, this will overide the predefined item size in the list layout
  itemView.setItemSize(0, [350, 100, 0]); // 0 means the first layout added to ItemView
 
  // Acticate the list layout (which will layout the items as a list)
  itemView.activateLayout(0, itemView.size); // 0 means the first layout added to ItemView
```
 
 ### Example of changing the data of items in ItemView dynamically
 
```
  var data = itemFactory.data;
  data[itemId]["icon_path"] = "new-icon.png";
  data[itemId]["title_text"] = "New Item";
  itemFactory.data = data; // ItemView will update the changed items immediately
```
 
 @class ItemFactory

*/

/**
 * Sets the file name of JSON template that contains the templates for items.
 *
 * @example
 *    itemFactory.jsonTemplateFile = "item-template.json"; // ItemFactory will look for the template from this JSON file
 *
 * @type String
 * @property jsonTemplateFile
 */
JSON_TEMPLATE_FILE

/**
 * Sets the data of ItemView
 *
 *  The data is an array of property maps in which each map contains the data
 *  for each item, including the template to be used to build the actor and
 *  the pairs of key/value to be used to replace the constants defined in the
 *  template. The order of property maps in the array represents the actual
 *  order of items in ItemView.
 *
 * @example
 *   var itemViewData = [
 *                        { "template" : "template-item",
 *                          "icon_path" : "icon0.png",
 *                          "title_text" : "Item 0" },
 *                        { "template" : "template-item",
 *                          "icon_path" : "icon1.png",
 *                          "title_text" : "Item 1" }
 *                     ];
 *
 *    itemFactory.data = itemViewData; // ItemFactory will create items from this data
 *
 * @type Array
 * @property data
 */
DATA
