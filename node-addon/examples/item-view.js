 var window= {
           x:0,
           y:0,
           width:1920,
           height: 1080,
           transparent: false,
           name:'itemview-example'
 };

 var viewMode={
       'stereoscopic-mode':'mono', // stereo-horizontal, stereo-vertical, stereo-interlaced,
       'stereoBase': 65 // Distance in millimeters between left/right cameras typically between (50-70mm)
 };

 var options= {
    'window': window,
    'viewMode': viewMode,
 }

//desktop
//var dali = require('../build/Release/dali')( options );

//target
var dali = require('dali')( options );

var items = [];
var button;
var stageSize;

var itemView;
var itemFactory;

var currentLayoutIndex = 0;
var totalItemCount = 100;

var imageDir = "./images/";

var daliApp = {};

daliApp.createItemView = function() {

  // Create item view data
  var itemViewData = [];
  for (var itemId = 0; itemId < totalItemCount; itemId++)
  {
    var data = {};
    data["template"] = "template-item-list"; // Create items initially with list template
    data["icon_path"] = imageDir + "icon-" + itemId % 3 + ".png";
    data["title_text"] = "Item " + itemId;
    itemViewData[itemId] = data;
  }

  // Create item factory and set the data
  itemFactory = new dali.ItemFactory();
  itemFactory.jsonTemplateFile = "./scripts/item-template.json";
  itemFactory.data = itemViewData;

  // Create item view
  stageSize = dali.stage.getSize();
  itemView = new dali.Control("ItemView", itemFactory);
  itemView.size = [stageSize.x, stageSize.y, 0.0];
  itemView.parentOrigin = dali.CENTER_LEFT;
  itemView.anchorPoint = dali.CENTER_LEFT;
  itemView.refreshInterval = 4.0;

  // Add item view to the stage
  dali.stage.add( itemView );

  // Create scroll bar for item view
  var scrollBar = new dali.Control("ScrollBar");
  scrollBar.parentOrigin = dali.TOP_RIGHT;
  scrollBar.anchorPoint = dali.TOP_RIGHT;
  scrollBar.widthResizePolicy = "FIT_TO_CHILDREN";
  scrollBar.heightResizePolicy = "FILL_TO_PARENT";
  itemView.add(scrollBar);

  // Add the list and grid layouts
  itemView.addLayout(dali.ITEM_LAYOUT_LIST); // layout index 0
  itemView.addLayout(dali.ITEM_LAYOUT_GRID); // layout index 1

  // Set custom item size for list layout
  itemView.setItemSize(0, [stageSize.x, stageSize.y * 0.1, 0.0]);

  // Set custom item size for grid layout
  var layoutMargin = 120;
  itemView.setItemSize(1, [(stageSize.x - layoutMargin) / 4, stageSize.y * 0.2, 0.0]);

  // Activate the list layout
  itemView.activateLayout(0, itemView.size);

  // Create button for layout switching
  button = new dali.Control("PushButton");
  button.size = [100.0, 60.0, 0.0];
  button.position = [-20.0, 20.0, 0.0];
  button.parentOrigin = dali.TOP_RIGHT;
  button.anchorPoint = dali.TOP_RIGHT;
  button.labelText = "Switch";
  dali.stage.add( button );

  // Connect a signal callback to button pressed signal
  button.on("pressed", daliApp.buttonPressedEvent);
}

daliApp.buttonPressedEvent = function( button ) {

  // Calculate the layout index for the next layout to switch to
  currentLayoutIndex++;
  currentLayoutIndex = currentLayoutIndex % itemView.getLayoutCount();

  // Activate the next layout
  itemView.activateLayout(currentLayoutIndex, [stageSize.x, stageSize.y, 0.0], 0.0);

  // Change the item template in item view data as we want to change the layout of the items
  var data = itemFactory.data;
  for (var itemId = 0; itemId < totalItemCount; itemId++)
  {
    if(currentLayoutIndex == 0)
    {
      // List layout
      data[itemId]["template"] = "template-item-list"; // Create items with list template
    }
    else
    {
      // Grid layout
      data[itemId]["template"] = "template-item-grid"; // Create items with grid template
    }
  }
  itemFactory.data = data;
}

function startup()
{
  daliApp.init();
}

daliApp.init = function()
{
  daliApp.createItemView();
}

startup();

