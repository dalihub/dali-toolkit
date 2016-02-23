 var window= {
           x:0,
           y:0,
           width:1920,
           height:1080,
           transparent: false,
           name:'scrollview-example'
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

var stageSize;
var scrollView;
var scrollBar;

var imageDir = "./images/";

var daliApp = {};

daliApp.createScrollView = function() {

  // Create a scroll view
  scrollView = new dali.Control("ScrollView");
  stageSize = dali.stage.getSize();
  scrollView.size = [stageSize.x, stageSize.y, 0.0];
  scrollView.parentOrigin = dali.CENTER;
  scrollView.anchorPoint = dali.CENTER;

  dali.stage.add( scrollView );

  // Add actors to a scroll view with 4 pages
  var pageRows = 1;
  var pageColumns = 3;
  for(var pageRow = 0; pageRow < pageRows; pageRow++)
  {
    for(var pageColumn = 0; pageColumn < pageColumns; pageColumn++)
    {
      var pageActor = new dali.Control();
      pageActor.widthResizePolicy = "FILL_TO_PARENT";
      pageActor.heightResizePolicy = "FILL_TO_PARENT";
      pageActor.parentOrigin = dali.CENTER;
      pageActor.anchorPoint = dali.CENTER;
      pageActor.position = [pageColumn * stageSize.x, pageRow * stageSize.y, 0.0];
      pageActor.name = "pageActor" + pageColumn;

      // Add images in a 5x4 grid layout for each page
      var imageRows = 4;
      var imageColumns = 5;
      var margin = 10.0;
      var imageSize = [(stageSize.x / imageColumns) - margin, (stageSize.y / imageRows) - margin, 0.0];

      for(var row = 0; row < imageRows; row++)
      {
        for(var column = 0; column < imageColumns;column++)
        {
          var imageView = new dali.Control("ImageView");
          var imageId = (row * imageColumns + column) % 2 + 1;
          imageView.image = imageDir + "image-" + imageId + ".jpg";
          imageView.parentOrigin = dali.CENTER;
          imageView.anchorPoint = dali.CENTER;
          imageView.size = imageSize;
          imageView.position = [ margin * 0.5 + (imageSize[0] + margin) * column - stageSize.x * 0.5 + imageSize[0] * 0.5,
                                 margin * 0.5 + (imageSize[1] + margin) * row - stageSize.y * 0.5 + imageSize[1] * 0.5,
                                 0.0 ];
          pageActor.add(imageView);
          var position = imageView.position;
        }
      }

      scrollView.add( pageActor );
    }
  }

  // Set scroll view to have 3 pages in X axis and allow page snapping,
  // and also disable scrolling in Y axis.
  var scrollMode = {
                     xAxisScrollEnabled  : true,
                     xAxisSnapToInterval : stageSize.x,                // Define the snap points
                     xAxisScrollBoundary : stageSize.x * pageColumns,  // i.e. Define 3 pages
                     yAxisScrollEnabled  : false
                   }

  scrollView.setScrollMode(scrollMode);

  // Create a horizontal scroll bar in the bottom of scroll view (optional)
  scrollBar = new dali.Control("ScrollBar");
  scrollBar.parentOrigin = dali.BOTTOM_LEFT;
  scrollBar.anchorPoint = dali.TOP_LEFT;
  scrollBar.widthResizePolicy = "FIT_TO_CHILDREN";
  scrollBar.heightResizePolicy = "FILL_TO_PARENT";
  scrollBar.orientation = [0, 0, 270];
  scrollBar.scrollDirection = "Horizontal";
  scrollView.add(scrollBar);

  // Connect to the onRelayout signal
  scrollView.on("onRelayout", daliApp.onScrollViewRelayout);
}

daliApp.onScrollViewRelayout = function( button ) {

  // Set the correct scroll bar size after size negotiation of scroll view is done
  scrollBar.size = [0.0, scrollView.getRelayoutSize(dali.DIMENSION_WIDTH), 0.0 ];
}

function startup()
{
  daliApp.init();
}

daliApp.init = function()
{
  daliApp.createScrollView();
}

startup();

