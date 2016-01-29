/**
 *
## ScrollView API
 
ScrollView is a container where the actors inside it can be scrolled manually
(via touch or pan gesture) or automatically.
 
By default, ScrollView can scroll to any position both horizontally and
vertically and no snapping is enabled.
 
Scroll mode can be specified to define how ScrollView should handle scrolling
in X and Y axes respectively (i.e. whether scrolling is enabled horizontally
or vertically, how scrolling is snapped, and the boundary to prevent ScrollView
to scroll beyond a certain position in the axes).
 
![ ](../assets/img/scroll-view/scroll-view.png)

### Example of creating a ScrollView
 
```
  // Create a scroll view
  var scrollView = new dali.Control("ScrollView");
  var stageSize = dali.stage.getSize();
  scrollView.size = [stageSize.x, stageSize.y, 0.0];
  scrollView.parentOrigin = dali.CENTER;
  scrollView.anchorPoint = dali.CENTER;
 
  dali.stage.add( scrollView );
 
  // Add actors to a scroll view with 3 pages
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
 
      // Add images in a 3x4 grid layout for each page
      var imageRows = 4;
      var imageColumns = 3;
      var margin = 10.0;
      var imageSize = [(stageSize.x / imageColumns) - margin, (stageSize.y / imageRows) - margin, 0.0];
 
      for(var row = 0; row < imageRows; row++)
      {
        for(var column = 0; column < imageColumns;column++)
        {
          var imageView = new dali.Control("ImageView");
          var imageId = row * imageColumns + column;
          imageView.image = "./image-" + imageId + ".jpg";
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
 
```
 
 @class ScrollView
 @extends Actor

*/
