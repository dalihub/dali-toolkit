 var window= {
        x:800,
         y:500,
       width:880,
       height: 1020,
       transparent: false,
       name:'my-first-dali-app'
 };
var viewMode={
       'stereoscopic-mode':'mono', // stereo-horizontal, stereo-vertical, stereo-interlaced,
       'stereo-base': 65 // Distance in millimeters between left/right cameras typically between (50-70mm)
 };
 var options= {
    'window': window,
    'view-mode': viewMode,
 }

var dali = require('./build/Release/dali')( options );
var netflixRoulette = require('netflix-roulette');


var NUMBER_OF_IMAGES = 40; // for now use 16 ( demo files go up to 30)
var DEMO_IMAGES = []; // array to store Dali Images
var VIDEO_WALL_ACTORS = []; // array to store Image actors
var VIDEO_WALL_ROWS = 5; // use 3 rows for the video wall
var VIDEO_WALL_COLUMNS = 6; // use 12 columns for the video wall
var VIDEO_WALL_TOTAL_ITEMS = VIDEO_WALL_COLUMNS * VIDEO_WALL_ROWS; // total items
var VIDEO_WALL_ITEM_SIZE = 128; // width / height of a item in the video wall
var BORDER_SIZE = 5;
var VIDEO_WALL_ITEM_SIZE_NO_BORDER = VIDEO_WALL_ITEM_SIZE - BORDER_SIZE;
var VIDEO_WALL_WIDTH = VIDEO_WALL_COLUMNS * VIDEO_WALL_ITEM_SIZE;
var VIDEO_WALL_HEIGHT = VIDEO_WALL_ROWS * VIDEO_WALL_ITEM_SIZE;
var daliApp = {};
var posters = [];

var wallRootActor; // the root actor of the video wall


daliApp.loadNetflixImages = function() {

  if( NUMBER_OF_IMAGES >= VIDEO_WALL_TOTAL_ITEMS)
  {
    NUMBER_OF_IMAGES = VIDEO_WALL_TOTAL_ITEMS-1;
  }

  for (index = 0; index < NUMBER_OF_IMAGES; ++index) {

    fileName = posters[ index % (posters.length-1)  ];
    if ( fileName )
    {
      DEMO_IMAGES[index] = new dali.ResourceImage( { url:fileName } );
    }
  }
}


daliApp.createRootActor = function() {
    wallRootActor = new dali.Actor();
    wallRootActor.parentOrigin = dali.CENTER;
    wallRootActor.anchorPoint = dali.CENTER;
    dali.stage.add(wallRootActor);

    var field = new dali.Control("TextField");
    field.parentOrigin = dali.CENTER;
    field.anchorPoint = dali.CENTER;

    field.placeholderText = "DALi netflix netflix-roulette demo";
    dali.stage.add( field );
}



daliApp.getWallActorIndex = function(x, y) {
    return x + y * VIDEO_WALL_COLUMNS;
}

daliApp.createActors = function() {
    daliApp.createRootActor();

    var anim = new dali.Animation(1);
    var animOptions = {
      alpha: "linear",
      delay: 0.0,     // used to delay the start of the animation
      duration: 1,    // duration of the animation
     };

    for (y = 0; y < VIDEO_WALL_ROWS; ++y) {
        for (x = 0; x < VIDEO_WALL_COLUMNS; ++x) {

            var actorIndex = daliApp.getWallActorIndex(x, y);
            var imageActor = new dali.ImageActor();

            // wrap image index between 0 and NUMBER_OF_IMAGES
            var imageIndex = actorIndex % NUMBER_OF_IMAGES;

            imageActor.setImage(DEMO_IMAGES[imageIndex]);

            imageActor.parentOrigin = dali.CENTER;
            imageActor.anchorPoint = dali.CENTER;
            imageActor.size = [VIDEO_WALL_ITEM_SIZE_NO_BORDER, VIDEO_WALL_ITEM_SIZE_NO_BORDER, 1.0]; // start with zero size so it zooms up

            var xPosition = x * VIDEO_WALL_ITEM_SIZE;
            //  as the middle the wall is at zero (relative to wallRootActor), we need to subtract half the wall width.
            // + add half item size because the item anchor point is the center of the wallRootActor.
            xPosition = xPosition - (VIDEO_WALL_WIDTH / 2) + (VIDEO_WALL_ITEM_SIZE / 2);

            var yPosition = y * VIDEO_WALL_ITEM_SIZE;
            yPosition = yPosition - (VIDEO_WALL_HEIGHT / 2) + (VIDEO_WALL_ITEM_SIZE / 2);

            imageActor.position = [0,0,0];

            animOptions.delay+=0.25;
            anim.animateTo( imageActor,"position",[xPosition, yPosition, 0.0],animOptions);
            // store the actor
            VIDEO_WALL_ACTORS[actorIndex] = imageActor;

            // Add to the video wall root actor.
            wallRootActor.add(imageActor);
        }
    }
    anim.play();
}

function Initialise() {

    daliApp.loadNetflixImages();

    daliApp.createActors();
}

function actorLoaded( error, data )
{
  for( i = 0; i < data.length; ++i )
  {
    var entry = data[i];

   if( entry.poster )
    {
      posters.push(entry.poster);
      //console.log(" entry = " + entry.poster );
    }
  }
  Initialise();

}


netflixRoulette.actor('nicolas', actorLoaded );

