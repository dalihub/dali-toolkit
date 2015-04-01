/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

// CLASS HEADER
#include <dali-toolkit/internal/text/decorator/text-decorator.h>

// EXTERNAL INCLUDES
#include <dali/public-api/actors/actor.h>
#include <dali/public-api/adaptor-framework/timer.h>
#include <dali/public-api/actors/image-actor.h>
#include <dali/public-api/actors/layer.h>
#include <dali/public-api/actors/mesh-actor.h>
#include <dali/public-api/common/constants.h>
#include <dali/public-api/events/tap-gesture.h>
#include <dali/public-api/events/tap-gesture-detector.h>
#include <dali/public-api/events/pan-gesture.h>
#include <dali/public-api/events/pan-gesture-detector.h>
#include <dali/public-api/geometry/mesh.h>
#include <dali/public-api/geometry/mesh-data.h>
#include <dali/public-api/images/resource-image.h>
#include <dali/public-api/math/vector2.h>
#include <dali/public-api/math/vector4.h>
//#include <dali/public-api/images/nine-patch-image.h>
#include <dali/public-api/signals/connection-tracker.h>

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/controls/control.h>
#include <dali-toolkit/public-api/controls/control-impl.h>
#include <dali-toolkit/public-api/controls/buttons/push-button.h>
#include <dali-toolkit/public-api/controls/default-controls/solid-color-actor.h>
#include <dali-toolkit/public-api/controls/text-controls/text-label.h>
#include <dali-toolkit/public-api/controls/text-controls/text-selection-popup.h>

#ifdef DEBUG_ENABLED
#define DECORATOR_DEBUG
#endif

// Local Data
namespace
{

const char* DEFAULT_GRAB_HANDLE_IMAGE( DALI_IMAGE_DIR "insertpoint-icon.png" );
const char* DEFAULT_SELECTION_HANDLE_ONE( DALI_IMAGE_DIR "text-input-selection-handle-left.png" );
const char* DEFAULT_SELECTION_HANDLE_TWO( DALI_IMAGE_DIR "text-input-selection-handle-right.png" );
//const char* DEFAULT_SELECTION_HANDLE_ONE_PRESSED( DALI_IMAGE_DIR "text-input-selection-handle-left-press.png" );
//const char* DEFAULT_SELECTION_HANDLE_TWO_PRESSED( DALI_IMAGE_DIR "text-input-selection-handle-right-press.png" );

const Dali::Vector3 DEFAULT_GRAB_HANDLE_RELATIVE_SIZE( 1.5f, 2.0f, 1.0f );
const Dali::Vector3 DEFAULT_SELECTION_HANDLE_RELATIVE_SIZE( 1.5f, 1.5f, 1.0f );

const std::size_t CURSOR_BLINK_INTERVAL = 500; // Cursor blink interval
const std::size_t MILLISECONDS = 1000;

const float DISPLAYED_HIGHLIGHT_Z_OFFSET( -0.05f );

/**
 * structure to hold coordinates of each quad, which will make up the mesh.
 */
struct QuadCoordinates
{
  /**
   * Default constructor
   */
  QuadCoordinates()
  {
  }

  /**
   * Constructor
   * @param[in] x1 left co-ordinate
   * @param[in] y1 top co-ordinate
   * @param[in] x2 right co-ordinate
   * @param[in] y2 bottom co-ordinate
   */
  QuadCoordinates(float x1, float y1, float x2, float y2)
  : min(x1, y1),
    max(x2, y2)
  {
  }

  Dali::Vector2 min;                          ///< top-left (minimum) position of quad
  Dali::Vector2 max;                          ///< bottom-right (maximum) position of quad
};

typedef std::vector<QuadCoordinates> QuadContainer;

} // end of namespace

namespace Dali
{

namespace Toolkit
{

namespace Text
{

struct Decorator::Impl : public ConnectionTracker
{
  struct CursorImpl
  {
    CursorImpl()
    : x(0.0f),
      y(0.0f),
      height(0.0f),
      color(Dali::Color::WHITE)
    {
    }

    float x;
    float y;
    float height;

    Vector4 color;
  };

  struct SelectionHandleImpl
  {
    SelectionHandleImpl()
    : x(0.0f),
      y(0.0f),
      cursorHeight(0.0f),
      flipped(false)
    {
    }

    float x;
    float y;
    float cursorHeight; ///< Not the handle height
    bool flipped;

    ImageActor actor;
    Actor grabArea;

    Image pressedImage;
    Image releasedImage;
  };

  Impl( Dali::Toolkit::Internal::Control& parent, Observer& observer )
  : mTextControlParent(parent),
    mObserver(observer),
    mActiveCursor(ACTIVE_CURSOR_NONE),
    mActiveGrabHandle(false),
    mActiveSelection( false ),
    mActiveCopyPastePopup( false ),
    mCursorBlinkInterval( CURSOR_BLINK_INTERVAL ),
    mCursorBlinkDuration( 0.0f ),
    mCursorBlinkStatus( true ),
    mGrabDisplacementX( 0.0f ),
    mGrabDisplacementY( 0.0f ),
    mHighlightColor( 0.07f, 0.41f, 0.59f, 1.0f ), // light blue
    mBoundingBox( Rect<int>() )
  {
  }

  /**
   * Relayout of the decorations owned by the decorator.
   * @param[in] size The Size of the UI control the decorater is adding it's decorations to.
   */
  void Relayout( const Vector2& size, const Vector2& scrollPosition )
  {
    // TODO - Remove this if nothing is active
    CreateActiveLayer();

    // Show or hide the cursors
    CreateCursors();
    if( mPrimaryCursor )
    {
      mPrimaryCursor.SetPosition( mCursor[PRIMARY_CURSOR].x + scrollPosition.x,
                                  mCursor[PRIMARY_CURSOR].y + scrollPosition.y );
      mPrimaryCursor.SetResizePolicy( FIXED, ALL_DIMENSIONS );
      mPrimaryCursor.SetPreferredSize( Vector2( 1.0f, mCursor[PRIMARY_CURSOR].height ) );
    }
    if( mSecondaryCursor )
    {
      mSecondaryCursor.SetPosition( mCursor[SECONDARY_CURSOR].x + scrollPosition.x,
                                    mCursor[SECONDARY_CURSOR].y + scrollPosition.y );
      mSecondaryCursor.SetResizePolicy( FIXED, ALL_DIMENSIONS );
      mSecondaryCursor.SetPreferredSize( Vector2( 1.0f, mCursor[SECONDARY_CURSOR].height ) );
    }

    // Show or hide the grab handle
    if( mActiveGrabHandle )
    {
      SetupTouchEvents();

      CreateGrabHandle();

      mGrabHandle.SetPosition( mCursor[PRIMARY_CURSOR].x + scrollPosition.x,
                               mCursor[PRIMARY_CURSOR].y + scrollPosition.y + mCursor[PRIMARY_CURSOR].height );
    }
    else if( mGrabHandle )
    {
      UnparentAndReset( mGrabHandle );
    }

    // Show or hide the selection handles/highlight
    if( mActiveSelection )
    {
      SetupTouchEvents();

      CreateSelectionHandles();

      SelectionHandleImpl& primary = mSelectionHandle[ PRIMARY_SELECTION_HANDLE ];
      primary.actor.SetPosition( primary.x + scrollPosition.x,
                                 primary.y + scrollPosition.y + primary.cursorHeight );

      SelectionHandleImpl& secondary = mSelectionHandle[ SECONDARY_SELECTION_HANDLE ];
      secondary.actor.SetPosition( secondary.x + scrollPosition.x,
                                   secondary.y + scrollPosition.y + secondary.cursorHeight );

      CreateHighlight();
      UpdateHighlight();
    }
    else
    {
      UnparentAndReset( mSelectionHandle[ PRIMARY_SELECTION_HANDLE ].actor );
      UnparentAndReset( mSelectionHandle[ SECONDARY_SELECTION_HANDLE ].actor );
      UnparentAndReset( mHighlightMeshActor );
    }

    if ( mActiveCopyPastePopup )
    {
      if ( !mCopyPastePopup )
      {
        mCopyPastePopup = TextSelectionPopup::New();
        mActiveLayer.Add ( mCopyPastePopup );
      }
      mCopyPastePopup.SetPosition( Vector3( 200.0f, -100.0f, 0.0f ) ); //todo grabhandle or selection handle positions to be used
    }
    else
    {
     if ( mCopyPastePopup )
     {
       UnparentAndReset( mCopyPastePopup );
     }
    }
  }

  void CreateCursor( ImageActor& cursor )
  {
    cursor = CreateSolidColorActor( Color::WHITE );
    cursor.SetParentOrigin( ParentOrigin::TOP_LEFT );
    cursor.SetAnchorPoint( AnchorPoint::TOP_CENTER );
  }

  // Add or Remove cursor(s) from parent
  void CreateCursors()
  {
    if( mActiveCursor == ACTIVE_CURSOR_NONE )
    {
      UnparentAndReset( mPrimaryCursor );
      UnparentAndReset( mSecondaryCursor );
    }
    else
    {
      /* Create Primary and or Secondary Cursor(s) if active and add to parent */
      if ( mActiveCursor == ACTIVE_CURSOR_PRIMARY ||
           mActiveCursor == ACTIVE_CURSOR_BOTH )
      {
        if ( !mPrimaryCursor )
        {
          CreateCursor( mPrimaryCursor );
#ifdef DECORATOR_DEBUG
          mPrimaryCursor.SetName( "PrimaryCursorActor" );
#endif
          mActiveLayer.Add( mPrimaryCursor);
        }
      }

      if ( mActiveCursor == ACTIVE_CURSOR_BOTH )
      {
        if ( !mSecondaryCursor )
        {
          CreateCursor( mSecondaryCursor );
#ifdef DECORATOR_DEBUG
          mSecondaryCursor.SetName( "SecondaryCursorActor" );
#endif
          mActiveLayer.Add( mSecondaryCursor);
        }
      }
    }
  }

  bool OnCursorBlinkTimerTick()
  {
    // Cursor blinking
    if ( mPrimaryCursor )
    {
      mPrimaryCursor.SetVisible( mCursorBlinkStatus );
    }
    if ( mSecondaryCursor )
    {
      mSecondaryCursor.SetVisible( mCursorBlinkStatus );
    }

    mCursorBlinkStatus = !mCursorBlinkStatus;

    return true;
  }

  void SetupTouchEvents()
  {
    if ( !mTapDetector )
    {
      mTapDetector = TapGestureDetector::New();
      mTapDetector.DetectedSignal().Connect( this, &Decorator::Impl::OnTap );
    }

    if ( !mPanGestureDetector )
    {
      mPanGestureDetector = PanGestureDetector::New();
      mPanGestureDetector.DetectedSignal().Connect( this, &Decorator::Impl::OnPan );
    }
  }

  void CreateActiveLayer()
  {
    if( !mActiveLayer )
    {
      Actor parent = mTextControlParent.Self();

      mActiveLayer = Layer::New();
#ifdef DECORATOR_DEBUG
      mActiveLayer.SetName ( "ActiveLayerActor" );
#endif

      mActiveLayer.SetAnchorPoint( AnchorPoint::CENTER);
      mActiveLayer.SetParentOrigin( ParentOrigin::CENTER);
      mActiveLayer.SetResizePolicy( FILL_TO_PARENT, ALL_DIMENSIONS );
      mActiveLayer.SetPositionInheritanceMode( USE_PARENT_POSITION );

      parent.Add( mActiveLayer );
    }

    mActiveLayer.RaiseToTop();
  }

  void CreateGrabHandle()
  {
    if( !mGrabHandle )
    {
      if ( !mGrabHandleImage )
      {
        mGrabHandleImage = ResourceImage::New( DEFAULT_GRAB_HANDLE_IMAGE );
      }

      mGrabHandle = ImageActor::New( mGrabHandleImage );
#ifdef DECORATOR_DEBUG
      mGrabHandle.SetName( "GrabHandleActor" );
#endif
      mGrabHandle.SetParentOrigin( ParentOrigin::TOP_LEFT );
      mGrabHandle.SetAnchorPoint( AnchorPoint::TOP_CENTER );
      mGrabHandle.SetDrawMode( DrawMode::OVERLAY );
      // Area that Grab handle responds to, larger than actual handle so easier to move
//#ifdef DECORATOR_DEBUG
//      mGrabArea = Toolkit::CreateSolidColorActor( Vector4(0.0f, 0.0f, 0.0f, 0.0f), true, Color::RED, 1 );
//      mGrabArea.SetName( "GrabArea" );
//#else
      mGrabArea = Actor::New();  //todo Force use of Actor until SolidColorActor fixed in Size Negotiation
      mGrabArea.SetRelayoutEnabled( true );
//#endif
      mGrabArea.SetParentOrigin( ParentOrigin::TOP_CENTER );
      mGrabArea.SetResizePolicy( FILL_TO_PARENT, ALL_DIMENSIONS );
      mGrabArea.SetAnchorPoint( AnchorPoint::TOP_CENTER );
      mGrabArea.SetSizeMode( SIZE_RELATIVE_TO_PARENT );
      mGrabArea.SetSizeModeFactor( DEFAULT_GRAB_HANDLE_RELATIVE_SIZE );
      mGrabHandle.Add( mGrabArea );

      mTapDetector.Attach( mGrabArea );
      mPanGestureDetector.Attach( mGrabArea );

      mActiveLayer.Add(mGrabHandle);
    }
  }

  void CreateSelectionHandles()
  {
    SelectionHandleImpl& primary = mSelectionHandle[ PRIMARY_SELECTION_HANDLE ];
    if ( !primary.actor )
    {
      if ( !primary.releasedImage )
      {
        primary.releasedImage = ResourceImage::New( DEFAULT_SELECTION_HANDLE_ONE );
      }

      primary.actor = ImageActor::New( primary.releasedImage );
#ifdef DECORATOR_DEBUG
      primary.actor.SetName("SelectionHandleOne");
#endif
      primary.actor.SetParentOrigin( ParentOrigin::TOP_LEFT );
      primary.actor.SetAnchorPoint( AnchorPoint::TOP_RIGHT ); // Change to BOTTOM_RIGHT if Look'n'Feel requires handle above text.
      primary.actor.SetDrawMode( DrawMode::OVERLAY ); // ensure grab handle above text
      primary.flipped = false;

      primary.grabArea = Actor::New(); // Area that Grab handle responds to, larger than actual handle so easier to move
#ifdef DECORATOR_DEBUG
      primary.grabArea.SetName("SelectionHandleOneGrabArea");
#endif
      primary.grabArea.SetSizeMode( SIZE_RELATIVE_TO_PARENT );
      primary.grabArea.SetSizeModeFactor( DEFAULT_SELECTION_HANDLE_RELATIVE_SIZE );
      primary.grabArea.SetPositionInheritanceMode( Dali::USE_PARENT_POSITION );

      mTapDetector.Attach( primary.grabArea );
      mPanGestureDetector.Attach( primary.grabArea );
      primary.grabArea.TouchedSignal().Connect( this, &Decorator::Impl::OnHandleOneTouched );

      primary.actor.Add( primary.grabArea );
      mActiveLayer.Add( primary.actor );
    }

    SelectionHandleImpl& secondary = mSelectionHandle[ SECONDARY_SELECTION_HANDLE ];
    if ( !secondary.actor )
    {
      if ( !secondary.releasedImage )
      {
        secondary.releasedImage = ResourceImage::New( DEFAULT_SELECTION_HANDLE_TWO );
      }

      secondary.actor = ImageActor::New( secondary.releasedImage );
#ifdef DECORATOR_DEBUG
      secondary.actor.SetName("SelectionHandleTwo");
#endif
      secondary.actor.SetParentOrigin( ParentOrigin::TOP_LEFT );
      secondary.actor.SetAnchorPoint( AnchorPoint::TOP_LEFT ); // Change to BOTTOM_LEFT if Look'n'Feel requires handle above text.
      secondary.actor.SetDrawMode( DrawMode::OVERLAY ); // ensure grab handle above text
      secondary.flipped = false;

      secondary.grabArea = Actor::New(); // Area that Grab handle responds to, larger than actual handle so easier to move
#ifdef DECORATOR_DEBUG
      secondary.grabArea.SetName("SelectionHandleTwoGrabArea");
#endif
      secondary.grabArea.SetSizeMode( SIZE_RELATIVE_TO_PARENT );
      secondary.grabArea.SetSizeModeFactor( DEFAULT_SELECTION_HANDLE_RELATIVE_SIZE );
      secondary.grabArea.SetPositionInheritanceMode( Dali::USE_PARENT_POSITION );

      mTapDetector.Attach( secondary.grabArea );
      mPanGestureDetector.Attach( secondary.grabArea );
      secondary.grabArea.TouchedSignal().Connect( this, &Decorator::Impl::OnHandleTwoTouched );

      secondary.actor.Add( secondary.grabArea );
      mActiveLayer.Add( secondary.actor );
    }

    //SetUpHandlePropertyNotifications(); TODO
  }

  void CreateHighlight()
  {
    if ( !mHighlightMeshActor )
    {
      mHighlightMaterial = Material::New( "HighlightMaterial" );
      mHighlightMaterial.SetDiffuseColor( mHighlightColor );

      mHighlightMeshData.SetMaterial( mHighlightMaterial );
      mHighlightMeshData.SetHasNormals( true );

      mHighlightMesh = Mesh::New( mHighlightMeshData );

      mHighlightMeshActor = MeshActor::New( mHighlightMesh );
#ifdef DECORATOR_DEBUG
      mHighlightMeshActor.SetName( "HighlightMeshActor" );
#endif
      mHighlightMeshActor.SetParentOrigin( ParentOrigin::TOP_LEFT );
      mHighlightMeshActor.SetAnchorPoint( AnchorPoint::TOP_LEFT );
      mHighlightMeshActor.SetPosition( 0.0f, 0.0f, DISPLAYED_HIGHLIGHT_Z_OFFSET );

      Actor parent = mTextControlParent.Self();
      parent.Add( mHighlightMeshActor );
    }
  }

  void UpdateHighlight()
  {
    //  Construct a Mesh with a texture to be used as the highlight 'box' for selected text
    //
    //  Example scenarios where mesh is made from 3, 1, 2, 2 ,3 or 3 quads.
    //
    //   [ TOP   ]  [ TOP ]      [TOP ]  [ TOP    ]      [ TOP  ]      [ TOP  ]
    //  [ MIDDLE ]             [BOTTOM]  [BOTTOM]      [ MIDDLE ]   [ MIDDLE  ]
    //  [ BOTTOM]                                      [ MIDDLE ]   [ MIDDLE  ]
    //                                                 [BOTTOM]     [ MIDDLE  ]
    //                                                              [BOTTOM]
    //
    //  Each quad is created as 2 triangles.
    //  Middle is just 1 quad regardless of its size.
    //
    //  (0,0)         (0,0)
    //     0*    *2     0*       *2
    //     TOP          TOP
    //     3*    *1     3*       *1
    //  4*       *1     4*     *6
    //     MIDDLE         BOTTOM
    //  6*       *5     7*     *5
    //  6*    *8
    //   BOTTOM
    //  9*    *7
    //

    if ( mHighlightMesh && mHighlightMaterial && !mHighlightQuadList.empty() )
    {
      MeshData::VertexContainer vertices;
      Dali::MeshData::FaceIndices faceIndices;

      std::vector<QuadCoordinates>::iterator iter = mHighlightQuadList.begin();
      std::vector<QuadCoordinates>::iterator endIter = mHighlightQuadList.end();

      // vertex position defaults to (0 0 0)
      MeshData::Vertex vertex;
      // set normal for all vertices as (0 0 1) pointing outward from TextInput Actor.
      vertex.nZ = 1.0f;

      for(std::size_t v = 0; iter != endIter; ++iter,v+=4 )
      {
        // Add each quad geometry (a sub-selection) to the mesh data.

        // 0-----1
        // |\    |
        // | \ A |
        // |  \  |
        // | B \ |
        // |    \|
        // 2-----3

        QuadCoordinates& quad = *iter;
        // top-left (v+0)
        vertex.x = quad.min.x;
        vertex.y = quad.min.y;
        vertices.push_back( vertex );

        // top-right (v+1)
        vertex.x = quad.max.x;
        vertex.y = quad.min.y;
        vertices.push_back( vertex );

        // bottom-left (v+2)
        vertex.x = quad.min.x;
        vertex.y = quad.max.y;
        vertices.push_back( vertex );

        // bottom-right (v+3)
        vertex.x = quad.max.x;
        vertex.y = quad.max.y;
        vertices.push_back( vertex );

        // triangle A (3, 1, 0)
        faceIndices.push_back( v + 3 );
        faceIndices.push_back( v + 1 );
        faceIndices.push_back( v );

        // triangle B (0, 2, 3)
        faceIndices.push_back( v );
        faceIndices.push_back( v + 2 );
        faceIndices.push_back( v + 3 );

        mHighlightMeshData.SetFaceIndices( faceIndices );
      }

      BoneContainer bones(0); // passed empty as bones not required
      mHighlightMeshData.SetData( vertices, faceIndices, bones, mHighlightMaterial );
      mHighlightMesh.UpdateMeshData( mHighlightMeshData );
    }
  }

  void OnTap( Actor actor, const TapGesture& tap )
  {
    if( actor == mGrabHandle )
    {
      // TODO
    }
  }

  void OnPan( Actor actor, const PanGesture& gesture )
  {
    if( actor == mGrabArea )
    {
      if( Gesture::Started == gesture.state )
      {
        mGrabDisplacementX = mGrabDisplacementY = 0;
      }

      mGrabDisplacementX += gesture.displacement.x;
      mGrabDisplacementY += gesture.displacement.y;

      float x = mCursor[PRIMARY_CURSOR].x + mGrabDisplacementX;
      float y = mCursor[PRIMARY_CURSOR].y + mCursor[PRIMARY_CURSOR].height*0.5f + mGrabDisplacementY;

      if( Gesture::Started    == gesture.state ||
          Gesture::Continuing == gesture.state )
      {
        mObserver.GrabHandleEvent( GRAB_HANDLE_PRESSED, x, y );
      }
      else if( Gesture::Finished  == gesture.state ||
               Gesture::Cancelled == gesture.state )
      {
        mObserver.GrabHandleEvent( GRAB_HANDLE_RELEASED, x, y );
      }
    }
  }

  bool OnHandleOneTouched( Actor actor, const TouchEvent& touch )
  {
    // TODO
    return false;
  }

  bool OnHandleTwoTouched( Actor actor, const TouchEvent& touch )
  {
    // TODO
    return false;
  }


  Internal::Control& mTextControlParent;
  Observer& mObserver;

  Layer mActiveLayer; // Layer for active handles and alike that ensures they are above all else.

  unsigned int mActiveCursor;
  bool         mActiveGrabHandle;
  bool         mActiveSelection;
  bool         mActiveCopyPastePopup;

  CursorImpl mCursor[CURSOR_COUNT];

  Timer mCursorBlinkTimer; // Timer to signal cursor to blink
  unsigned int mCursorBlinkInterval;
  float mCursorBlinkDuration;
  bool mCursorBlinkStatus; // Flag to switch between blink on and blink off

  ImageActor mPrimaryCursor;
  ImageActor mSecondaryCursor;

  ImageActor mGrabHandle;
  Actor mGrabArea;
  float mGrabDisplacementX;
  float mGrabDisplacementY;

  SelectionHandleImpl mSelectionHandle[SELECTION_HANDLE_COUNT];

  MeshActor         mHighlightMeshActor;        ///< Mesh Actor to display highlight
  Mesh              mHighlightMesh;             ///< Mesh for highlight
  MeshData          mHighlightMeshData;         ///< Mesh Data for highlight
  Material          mHighlightMaterial;         ///< Material used for highlight
  Vector4           mHighlightColor;            ///< Color of the highlight
  QuadContainer     mHighlightQuadList;         ///< Sub-selections that combine to create the complete selection highlight

  TextSelectionPopup mCopyPastePopup;

  Image mCursorImage;
  Image mGrabHandleImage;

  TapGestureDetector mTapDetector;
  PanGestureDetector mPanGestureDetector;

  Rect<int> mBoundingBox;
};

DecoratorPtr Decorator::New( Internal::Control& parent, Observer& observer )
{
  return DecoratorPtr( new Decorator(parent, observer) );
}

void Decorator::SetBoundingBox( const Rect<int>& boundingBox )
{
  mImpl->mBoundingBox = boundingBox;
}

const Rect<int>& Decorator::GetBoundingBox() const
{
  return mImpl->mBoundingBox;
}

void Decorator::Relayout( const Vector2& size, const Vector2& scrollPosition )
{
  mImpl->Relayout( size, scrollPosition );
}

/** Cursor **/

void Decorator::SetActiveCursor( ActiveCursor activeCursor )
{
  mImpl->mActiveCursor = activeCursor;
}

unsigned int Decorator::GetActiveCursor() const
{
  return mImpl->mActiveCursor;
}

void Decorator::SetPosition( Cursor cursor, float x, float y, float height )
{
  // Adjust grab handle displacement
  mImpl->mGrabDisplacementX -= x - mImpl->mCursor[cursor].x;
  mImpl->mGrabDisplacementY -= y - mImpl->mCursor[cursor].y;

  mImpl->mCursor[cursor].x = x;
  mImpl->mCursor[cursor].y = y;
  mImpl->mCursor[cursor].height = height;
}

void Decorator::GetPosition( Cursor cursor, float& x, float& y, float& height ) const
{
  x = mImpl->mCursor[cursor].x;
  y = mImpl->mCursor[cursor].y;
  height = mImpl->mCursor[cursor].height;
}

void Decorator::SetColor( Cursor cursor, const Dali::Vector4& color )
{
  mImpl->mCursor[cursor].color = color;
}

const Dali::Vector4& Decorator::GetColor( Cursor cursor ) const
{
  return mImpl->mCursor[cursor].color;
}

void Decorator::StartCursorBlink()
{
  if ( !mImpl->mCursorBlinkTimer )
  {
    mImpl->mCursorBlinkTimer = Timer::New( mImpl->mCursorBlinkInterval );
    mImpl->mCursorBlinkTimer.TickSignal().Connect( mImpl, &Decorator::Impl::OnCursorBlinkTimerTick );
  }

  if ( !mImpl->mCursorBlinkTimer.IsRunning() )
  {
    mImpl->mCursorBlinkTimer.Start();
  }
}

void Decorator::StopCursorBlink()
{
  if ( mImpl->mCursorBlinkTimer )
  {
    mImpl->mCursorBlinkTimer.Stop();
  }
}

void Decorator::SetCursorBlinkInterval( float seconds )
{
  mImpl->mCursorBlinkInterval = seconds*MILLISECONDS; // Convert to milliseconds
}

float Decorator::GetCursorBlinkInterval() const
{
  return mImpl->mCursorBlinkInterval;
}

void Decorator::SetCursorBlinkDuration( float seconds )
{
  mImpl->mCursorBlinkDuration = seconds;
}

float Decorator::GetCursorBlinkDuration() const
{
  return mImpl->mCursorBlinkDuration;
}

/** GrabHandle **/

void Decorator::SetGrabHandleActive( bool active )
{
  mImpl->mActiveGrabHandle = active;
}

bool Decorator::IsGrabHandleActive() const
{
  return mImpl->mActiveGrabHandle;
}

void Decorator::SetGrabHandleImage( Dali::Image image )
{
  mImpl->mGrabHandleImage = image;
}

Dali::Image Decorator::GetGrabHandleImage() const
{
  return mImpl->mGrabHandleImage;
}

/** Selection **/

void Decorator::SetSelectionActive( bool active )
{
  mImpl->mActiveSelection = active;
}

bool Decorator::IsSelectionActive() const
{
  return mImpl->mActiveSelection;
}

void Decorator::SetPosition( SelectionHandle handle, float x, float y, float height )
{
  mImpl->mSelectionHandle[handle].x = x;
  mImpl->mSelectionHandle[handle].y = y;
  mImpl->mSelectionHandle[handle].cursorHeight = height;
}

void Decorator::GetPosition( SelectionHandle handle, float& x, float& y, float& height ) const
{
  x = mImpl->mSelectionHandle[handle].x;
  y = mImpl->mSelectionHandle[handle].y;
  height = mImpl->mSelectionHandle[handle].cursorHeight;
}

void Decorator::SetImage( SelectionHandle handle, SelectionHandleState state, Dali::Image image )
{
  if( SELECTION_HANDLE_PRESSED == state )
  {
    mImpl->mSelectionHandle[handle].pressedImage = image;
  }
  else
  {
    mImpl->mSelectionHandle[handle].releasedImage = image;
  }
}

Dali::Image Decorator::GetImage( SelectionHandle handle, SelectionHandleState state ) const
{
  if( SELECTION_HANDLE_PRESSED == state )
  {
    return mImpl->mSelectionHandle[handle].pressedImage;
  }

  return mImpl->mSelectionHandle[handle].releasedImage;
}

void Decorator::AddHighlight( float x1, float y1, float x2, float y2 )
{
  mImpl->mHighlightQuadList.push_back( QuadCoordinates(x1, y1, x2, y2) );
}

void Decorator::ClearHighlights()
{
  mImpl->mHighlightQuadList.clear();
}

void Decorator::SetPopupActive( bool active )
{
  mImpl->mActiveCopyPastePopup = active;
}

bool Decorator::IsPopupActive() const
{
  return mImpl->mActiveCopyPastePopup ;
}

Decorator::~Decorator()
{
  delete mImpl;
}

Decorator::Decorator( Dali::Toolkit::Internal::Control& parent, Observer& observer )
: mImpl( NULL )
{
  mImpl = new Decorator::Impl( parent, observer );
}

} // namespace Text

} // namespace Toolkit

} // namespace Dali
