 var window = {
           x:0,
           y:0,
           width:1920,
           height:1080,
           transparent: false,
           name:'Flexbox Demo'
 };

 var options = {
    'window': window
 }

try {
  // target
  var dali = require('dali')( options );
}
catch(err) {
  // desktop
  var dali = require('../build/Release/dali')( options );
}

var imageDir = "./images/";

var imageView;
var currentImageIndex = 0;
var imageNames = ["image-1.jpg", "image-2.jpg", "image-3.jpg"];

var daliApp = {};

daliApp.init = function() {

  // Create the main flex container
  var flexContainer = new dali.Control("FlexContainer");
  flexContainer.parentOrigin = dali.TOP_LEFT;
  flexContainer.anchorPoint = dali.TOP_LEFT;
  flexContainer.widthResizePolicy = "FILL_TO_PARENT";
  flexContainer.heightResizePolicy = "FILL_TO_PARENT";
  flexContainer.backgroundColor = dali.COLOR_WHITE;
  flexContainer.flexDirection = "column"; // display toolbar and content vertically

  dali.stage.add( flexContainer );

  // Create the toolbar area
  var toolBar = new dali.Control("FlexContainer");
  toolBar.parentOrigin = dali.TOP_LEFT;
  toolBar.anchorPoint = dali.TOP_LEFT;
  toolBar.backgroundColor = dali.COLOR_CYAN;
  toolBar.flexDirection = "row"; // display toolbar items horizontally
  toolBar.alignItems = "center"; // align toolbar items vertically center
  toolBar.registerCustomProperty("flex", 0.1, dali.PROPERTY_READ_WRITE); // 10 percent of available space in the cross axis

  flexContainer.add(toolBar);

  // Create the content area
  var content = new dali.Control("FlexContainer");
  content.parentOrigin = dali.TOP_LEFT;
  content.anchorPoint = dali.TOP_LEFT;
  content.flexDirection = "row";
  content.alignItems = "center"; // align items vertically center
  content.justifyContent = "center"; // align items horizontally center
  content.registerCustomProperty("flex", 0.9, dali.PROPERTY_READ_WRITE); // 90 percent of available space in the cross axis

  flexContainer.add(content);

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
  title.pointSize = 28;
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

  // Add an image to the center of the content area
  imageView = new dali.Control("ImageView");
  imageView.image = imageDir + imageNames[currentImageIndex];
  imageView.parentOrigin = dali.TOP_LEFT;
  imageView.anchorPoint = dali.TOP_LEFT;
  content.add( imageView );

  // Connect signal callback to button pressed signal
  prevButton.on("pressed", daliApp.buttonPressedEvent);
  nextButton.on("pressed", daliApp.buttonPressedEvent);
}

daliApp.buttonPressedEvent = function( button ) {

  // Set the size of image view to its natural size
  imageView.widthResizePolicy = "USE_NATURAL_SIZE";
  imageView.heightResizePolicy = "USE_NATURAL_SIZE";

  // Work out the index of the new image
  if (button.name == "Prev") {
    currentImageIndex--;
  }
  else {
    currentImageIndex++;
  }
  currentImageIndex %= imageNames.length;

  // Display the new image
  imageView.image = imageDir + imageNames[Math.abs(currentImageIndex)];
}

function startup() {

  daliApp.init();
}

startup();

