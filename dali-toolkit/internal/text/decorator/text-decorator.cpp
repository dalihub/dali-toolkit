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
#include <dali/integration-api/debug.h>
#include <dali/public-api/actors/actor.h>
#include <dali/public-api/adaptor-framework/timer.h>
#include <dali/public-api/actors/image-actor.h>
#include <dali/public-api/actors/layer.h>
#include <dali/devel-api/actors/mesh-actor.h>
#include <dali/public-api/animation/constraint.h>
#include <dali/public-api/common/constants.h>
#include <dali/public-api/common/stage.h>
#include <dali/public-api/events/tap-gesture.h>
#include <dali/public-api/events/tap-gesture-detector.h>
#include <dali/public-api/events/touch-event.h>
#include <dali/public-api/events/pan-gesture.h>
#include <dali/public-api/events/pan-gesture-detector.h>
#include <dali/devel-api/geometry/mesh.h>
#include <dali/devel-api/geometry/mesh-data.h>
#include <dali/public-api/images/resource-image.h>
#include <dali/public-api/math/rect.h>
#include <dali/public-api/math/vector2.h>
#include <dali/public-api/math/vector4.h>
#include <dali/public-api/object/property-notification.h>
#include <dali/public-api/signals/connection-tracker.h>

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/controls/control.h>
#include <dali-toolkit/public-api/controls/control-impl.h>
#include <dali-toolkit/public-api/controls/buttons/push-button.h>
#include <dali-toolkit/public-api/controls/default-controls/solid-color-actor.h>
#include <dali-toolkit/public-api/controls/text-controls/text-label.h>
#include <dali-toolkit/devel-api/controls/text-controls/text-selection-popup.h>

#ifdef DEBUG_ENABLED
#define DECORATOR_DEBUG

#endif

namespace Dali
{
namespace Internal
{
namespace
{
#ifdef DECORATOR_DEBUG
Integration::Log::Filter* gLogFilter( Integration::Log::Filter::New(Debug::NoLogging, false, "LOG_TEXT_DECORATOR") );
#endif
}
}
}


// Local Data
namespace
{

const char* DEFAULT_GRAB_HANDLE_IMAGE_RELEASED( DALI_IMAGE_DIR "insertpoint-icon.png" );
const char* DEFAULT_GRAB_HANDLE_IMAGE_PRESSED( DALI_IMAGE_DIR "insertpoint-icon-pressed.png" );
const char* DEFAULT_SELECTION_HANDLE_ONE( DALI_IMAGE_DIR "text-input-selection-handle-left.png" );
const char* DEFAULT_SELECTION_HANDLE_TWO( DALI_IMAGE_DIR "text-input-selection-handle-right.png" );

const Dali::Vector3 DEFAULT_GRAB_HANDLE_RELATIVE_SIZE( 1.5f, 2.0f, 1.0f );
const Dali::Vector3 DEFAULT_SELECTION_HANDLE_RELATIVE_SIZE( 1.5f, 1.5f, 1.0f );

const Dali::Vector4 LIGHT_BLUE( 0.07f, 0.41f, 0.59f, 1.0f ); // The text highlight color.

const unsigned int CURSOR_BLINK_INTERVAL = 500u; // Cursor blink interval
const float TO_MILLISECONDS = 1000.f;
const float TO_SECONDS = 1.f / TO_MILLISECONDS;

const float DISPLAYED_HIGHLIGHT_Z_OFFSET( -0.05f );

const unsigned int SCROLL_TICK_INTERVAL = 50u;

const float SCROLL_THRESHOLD = 10.f;
const float SCROLL_SPEED = 300.f;
const float SCROLL_DISTANCE = SCROLL_SPEED * SCROLL_TICK_INTERVAL * TO_SECONDS;

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

/**
 * @brief Takes a bounding rectangle in the local coordinates of an actor and returns the world coordinates Bounding Box.
 * @param[in] boundingRectangle local bounding
 * @param[out] Vector4 World coordinate bounding Box.
 */
void LocalToWorldCoordinatesBoundingBox( const Dali::Rect<int>& boundingRectangle, Dali::Vector4& boundingBox )
{
  // Convert to world coordinates and store as a Vector4 to be compatible with Property Notifications.
  Dali::Vector2 stageSize = Dali::Stage::GetCurrent().GetSize();

  const float originX = boundingRectangle.x - 0.5f * stageSize.width;
  const float originY = boundingRectangle.y - 0.5f * stageSize.height;

  boundingBox = Dali::Vector4( originX,
                               originY,
                               originX + boundingRectangle.width,
                               originY + boundingRectangle.height );
}


} // end of namespace

namespace Dali
{

namespace Toolkit
{

namespace Text
{

struct Decorator::Impl : public ConnectionTracker
{
  enum ScrollDirection
  {
    SCROLL_NONE,
    SCROLL_RIGHT,
    SCROLL_LEFT,
    SCROLL_TOP,
    SCROLL_BOTTOM
  };

  struct CursorImpl
  {
    CursorImpl()
    : color( Dali::Color::BLACK ),
      position(),
      cursorHeight( 0.0f ),
      lineHeight( 0.0f )
    {
    }

    Vector4 color;
    Vector2 position;
    float cursorHeight;
    float lineHeight;
  };

  struct HandleImpl
  {
    HandleImpl()
    : position(),
      lineHeight( 0.0f ),
      grabDisplacementX( 0.f ),
      grabDisplacementY( 0.f ),
      active( false ),
      visible( false ),
      flipped( false )
    {
    }

    ImageActor actor;
    Actor grabArea;

    Vector2 position;
    float lineHeight; ///< Not the handle height
    float grabDisplacementX;
    float grabDisplacementY;
    bool active  : 1;
    bool visible : 1;
    bool flipped : 1;
  };

  Impl( Dali::Toolkit::Internal::Control& parent, Observer& observer )
  : mTextControlParent( parent ),
    mObserver( observer ),
    mBoundingBox( Rect<int>() ),
    mHighlightColor( LIGHT_BLUE ),
    mActiveCursor( ACTIVE_CURSOR_NONE ),
    mCursorBlinkInterval( CURSOR_BLINK_INTERVAL ),
    mCursorBlinkDuration( 0.0f ),
    mHandleScrolling( HANDLE_TYPE_COUNT ),
    mScrollDirection( SCROLL_NONE ),
    mScrollThreshold( SCROLL_THRESHOLD ),
    mScrollSpeed( SCROLL_SPEED ),
    mScrollDistance( SCROLL_DISTANCE ),
    mActiveCopyPastePopup( false ),
    mCursorBlinkStatus( true ),
    mPrimaryCursorVisible( false ),
    mSecondaryCursorVisible( false )
  {
  }

  /**
   * Relayout of the decorations owned by the decorator.
   * @param[in] size The Size of the UI control the decorator is adding it's decorations to.
   */
  void Relayout( const Vector2& size )
  {
    // TODO - Remove this if nothing is active
    CreateActiveLayer();

    // Show or hide the cursors
    CreateCursors();
    if( mPrimaryCursor )
    {
      const CursorImpl& cursor = mCursor[PRIMARY_CURSOR];
      mPrimaryCursorVisible = ( cursor.position.x <= size.width ) && ( cursor.position.x >= 0.f );
      if( mPrimaryCursorVisible )
      {
        Vector2 position = cursor.position;
        if( GRAB_HANDLE == mHandleScrolling )
        {
          if( mScrollDirection == SCROLL_RIGHT )
          {
            position.x = 0.f;
          }
          else
          {
            position.x = size.width;
          }
        }

        mPrimaryCursor.SetPosition( position.x,
                                    position.y );
        mPrimaryCursor.SetSize( Size( 1.0f, cursor.cursorHeight ) );
      }
      mPrimaryCursor.SetVisible( mPrimaryCursorVisible );
    }
    if( mSecondaryCursor )
    {
      const CursorImpl& cursor = mCursor[SECONDARY_CURSOR];
      mSecondaryCursorVisible = ( cursor.position.x <= size.width ) && ( cursor.position.x >= 0.f );
      if( mSecondaryCursorVisible )
      {
        mSecondaryCursor.SetPosition( cursor.position.x,
                                      cursor.position.y );
        mSecondaryCursor.SetSize( Size( 1.0f, cursor.cursorHeight ) );
      }
      mSecondaryCursor.SetVisible( mSecondaryCursorVisible );
    }

    // Show or hide the grab handle
    HandleImpl& grabHandle = mHandle[GRAB_HANDLE];
    if( grabHandle.active )
    {
      Vector2 position = grabHandle.position;

      if( GRAB_HANDLE == mHandleScrolling )
      {
        if( mScrollDirection == SCROLL_RIGHT )
        {
          position.x = 0.f;
        }
        else
        {
          position.x = size.width;
        }
      }

      const bool isVisible = ( position.x <= size.width ) && ( position.x >= 0.f );

      if( isVisible )
      {
        SetupTouchEvents();

        CreateGrabHandle();

        grabHandle.actor.SetPosition( position.x,
                                      position.y + grabHandle.lineHeight );
      }
      grabHandle.actor.SetVisible( isVisible );
    }
    else if( grabHandle.actor )
    {
      UnparentAndReset( grabHandle.actor );
    }

    // Show or hide the selection handles/highlight
    HandleImpl& primary = mHandle[ LEFT_SELECTION_HANDLE ];
    HandleImpl& secondary = mHandle[ RIGHT_SELECTION_HANDLE ];
    if( primary.active || secondary.active )
    {
      SetupTouchEvents();

      CreateSelectionHandles();

      primary.actor.SetPosition( primary.position.x,
                                 primary.position.y + primary.lineHeight );

      secondary.actor.SetPosition( secondary.position.x,
                                   secondary.position.y + secondary.lineHeight );

      CreateHighlight();
      UpdateHighlight();
    }
    else
    {
      UnparentAndReset( primary.actor );
      UnparentAndReset( secondary.actor );
      UnparentAndReset( mHighlightMeshActor );
    }

    if ( mActiveCopyPastePopup )
    {
      if ( !mCopyPastePopup )
      {
        mCopyPastePopup = TextSelectionPopup::New();
#ifdef DECORATOR_DEBUG
        mCopyPastePopup.SetName("mCopyPastePopup");
#endif
        mCopyPastePopup.SetAnchorPoint( AnchorPoint::CENTER );
        mCopyPastePopup.OnRelayoutSignal().Connect( this,  &Decorator::Impl::PopUpRelayoutComplete  ); // Position popup after size negotiation
        mActiveLayer.Add ( mCopyPastePopup );
      }
    }
    else
    {
     if ( mCopyPastePopup )
     {
       UnparentAndReset( mCopyPastePopup );
     }
    }
  }

  void UpdatePositions( const Vector2& scrollOffset )
  {
    mCursor[PRIMARY_CURSOR].position += scrollOffset;
    mCursor[SECONDARY_CURSOR].position += scrollOffset;
    mHandle[ GRAB_HANDLE ].position += scrollOffset;
    mHandle[ LEFT_SELECTION_HANDLE ].position += scrollOffset;
    mHandle[ RIGHT_SELECTION_HANDLE ].position += scrollOffset;

    // TODO Highlight box??
  }

  void PopUpRelayoutComplete( Actor actor )
  {
    // Size negotiation for CopyPastePopup complete so can get the size and constrain position within bounding box.

    mCopyPastePopup.OnRelayoutSignal().Disconnect( this, &Decorator::Impl::PopUpRelayoutComplete  );

    Vector3 popupPosition( mCursor[PRIMARY_CURSOR].position.x, mCursor[PRIMARY_CURSOR].position.y -100.0f , 0.0f); //todo 100 to be an offset Property

    Vector3 popupSize = Vector3( mCopyPastePopup.GetRelayoutSize( Dimension::WIDTH ), mCopyPastePopup.GetRelayoutSize( Dimension::HEIGHT ), 0.0f );

    GetConstrainedPopupPosition( popupPosition, popupSize, AnchorPoint::CENTER, mActiveLayer, mBoundingBox );

    SetUpPopUpPositionNotifications();

    mCopyPastePopup.SetPosition( popupPosition ); //todo grabhandle(cursor) or selection handle positions to be used
  }

  void CreateCursor( ImageActor& cursor, const Vector4& color )
  {
    cursor = CreateSolidColorActor( color );
    cursor.SetParentOrigin( ParentOrigin::TOP_LEFT ); // Need to set the default parent origin as CreateSolidColorActor() sets a different one.
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
          CreateCursor( mPrimaryCursor, mCursor[PRIMARY_CURSOR].color );
#ifdef DECORATOR_DEBUG
          mPrimaryCursor.SetName( "PrimaryCursorActor" );
#endif
          mActiveLayer.Add( mPrimaryCursor );
        }
      }

      if ( mActiveCursor == ACTIVE_CURSOR_BOTH )
      {
        if ( !mSecondaryCursor )
        {
          CreateCursor( mSecondaryCursor, mCursor[SECONDARY_CURSOR].color );
#ifdef DECORATOR_DEBUG
          mSecondaryCursor.SetName( "SecondaryCursorActor" );
#endif
          mActiveLayer.Add( mSecondaryCursor );
        }
      }
      else
      {
        UnparentAndReset( mSecondaryCursor );
      }
    }
  }

  bool OnCursorBlinkTimerTick()
  {
    // Cursor blinking
    if ( mPrimaryCursor )
    {
      mPrimaryCursor.SetVisible( mPrimaryCursorVisible && mCursorBlinkStatus );
    }
    if ( mSecondaryCursor )
    {
      mSecondaryCursor.SetVisible( mSecondaryCursorVisible && mCursorBlinkStatus );
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

      mActiveLayer.SetParentOrigin( ParentOrigin::CENTER );
      mActiveLayer.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
      mActiveLayer.SetPositionInheritanceMode( USE_PARENT_POSITION );

      parent.Add( mActiveLayer );
    }

    mActiveLayer.RaiseToTop();
  }

  void CreateGrabHandle()
  {
    HandleImpl& grabHandle = mHandle[GRAB_HANDLE];
    if( !grabHandle.actor )
    {
      if( !mHandleImages[GRAB_HANDLE][HANDLE_IMAGE_RELEASED] )
      {
        mHandleImages[GRAB_HANDLE][HANDLE_IMAGE_RELEASED] = ResourceImage::New( DEFAULT_GRAB_HANDLE_IMAGE_RELEASED );
      }
      if( !mHandleImages[GRAB_HANDLE][HANDLE_IMAGE_PRESSED] )
      {
        mHandleImages[GRAB_HANDLE][HANDLE_IMAGE_PRESSED] = ResourceImage::New( DEFAULT_GRAB_HANDLE_IMAGE_PRESSED );
      }

      grabHandle.actor = ImageActor::New( mHandleImages[GRAB_HANDLE][HANDLE_IMAGE_RELEASED] );
      grabHandle.actor.SetAnchorPoint( AnchorPoint::TOP_CENTER );
      grabHandle.actor.SetDrawMode( DrawMode::OVERLAY );
      // Area that Grab handle responds to, larger than actual handle so easier to move
#ifdef DECORATOR_DEBUG
      grabHandle.actor.SetName( "GrabHandleActor" );
      if ( Dali::Internal::gLogFilter->IsEnabledFor( Debug::Verbose ) )
      {
        grabHandle.grabArea = Toolkit::CreateSolidColorActor( Vector4(0.0f, 0.0f, 0.0f, 0.0f), true, Color::RED, 1 );
        grabHandle.grabArea.SetName( "GrabArea" );
      }
      else
      {
        grabHandle.grabArea = Actor::New();
        grabHandle.grabArea.SetName( "GrabArea" );
      }
#else
      grabHandle.grabArea = Actor::New();
#endif

      grabHandle.grabArea.SetParentOrigin( ParentOrigin::TOP_CENTER );
      grabHandle.grabArea.SetAnchorPoint( AnchorPoint::TOP_CENTER );
      grabHandle.grabArea.SetResizePolicy( ResizePolicy::SIZE_RELATIVE_TO_PARENT, Dimension::ALL_DIMENSIONS );
      grabHandle.grabArea.SetSizeModeFactor( DEFAULT_GRAB_HANDLE_RELATIVE_SIZE );
      grabHandle.actor.Add( grabHandle.grabArea );

      grabHandle.grabArea.TouchedSignal().Connect( this, &Decorator::Impl::OnGrabHandleTouched );
      mTapDetector.Attach( grabHandle.grabArea );
      mPanGestureDetector.Attach( grabHandle.grabArea );

      mActiveLayer.Add( grabHandle.actor );
    }
  }

  void CreateSelectionHandles()
  {
    HandleImpl& primary = mHandle[ LEFT_SELECTION_HANDLE ];
    if( !primary.actor )
    {
      if( !mHandleImages[LEFT_SELECTION_HANDLE][HANDLE_IMAGE_RELEASED] )
      {
        mHandleImages[LEFT_SELECTION_HANDLE][HANDLE_IMAGE_RELEASED] = ResourceImage::New( DEFAULT_SELECTION_HANDLE_ONE );
      }

      primary.actor = ImageActor::New( mHandleImages[LEFT_SELECTION_HANDLE][HANDLE_IMAGE_RELEASED] );
#ifdef DECORATOR_DEBUG
      primary.actor.SetName("SelectionHandleOne");
#endif
      primary.actor.SetAnchorPoint( AnchorPoint::TOP_RIGHT ); // Change to BOTTOM_RIGHT if Look'n'Feel requires handle above text.
      primary.actor.SetDrawMode( DrawMode::OVERLAY ); // ensure grab handle above text
      primary.flipped = false;

      primary.grabArea = Actor::New(); // Area that Grab handle responds to, larger than actual handle so easier to move
#ifdef DECORATOR_DEBUG
      primary.grabArea.SetName("SelectionHandleOneGrabArea");
#endif
      primary.grabArea.SetResizePolicy( ResizePolicy::SIZE_RELATIVE_TO_PARENT, Dimension::ALL_DIMENSIONS );
      primary.grabArea.SetSizeModeFactor( DEFAULT_SELECTION_HANDLE_RELATIVE_SIZE );
      primary.grabArea.SetPositionInheritanceMode( Dali::USE_PARENT_POSITION );

      mTapDetector.Attach( primary.grabArea );
      mPanGestureDetector.Attach( primary.grabArea );
      primary.grabArea.TouchedSignal().Connect( this, &Decorator::Impl::OnHandleOneTouched );

      primary.actor.Add( primary.grabArea );
      mActiveLayer.Add( primary.actor );
    }

    HandleImpl& secondary = mHandle[ RIGHT_SELECTION_HANDLE ];
    if( !secondary.actor )
    {
      if( !mHandleImages[RIGHT_SELECTION_HANDLE][HANDLE_IMAGE_RELEASED] )
      {
        mHandleImages[RIGHT_SELECTION_HANDLE][HANDLE_IMAGE_RELEASED] = ResourceImage::New( DEFAULT_SELECTION_HANDLE_TWO );
      }

      secondary.actor = ImageActor::New( mHandleImages[RIGHT_SELECTION_HANDLE][HANDLE_IMAGE_RELEASED] );
#ifdef DECORATOR_DEBUG
      secondary.actor.SetName("SelectionHandleTwo");
#endif
      secondary.actor.SetAnchorPoint( AnchorPoint::TOP_LEFT ); // Change to BOTTOM_LEFT if Look'n'Feel requires handle above text.
      secondary.actor.SetDrawMode( DrawMode::OVERLAY ); // ensure grab handle above text
      secondary.flipped = false;

      secondary.grabArea = Actor::New(); // Area that Grab handle responds to, larger than actual handle so easier to move
#ifdef DECORATOR_DEBUG
      secondary.grabArea.SetName("SelectionHandleTwoGrabArea");
#endif
      secondary.grabArea.SetResizePolicy( ResizePolicy::SIZE_RELATIVE_TO_PARENT, Dimension::ALL_DIMENSIONS );
      secondary.grabArea.SetSizeModeFactor( DEFAULT_SELECTION_HANDLE_RELATIVE_SIZE );
      secondary.grabArea.SetPositionInheritanceMode( Dali::USE_PARENT_POSITION );

      mTapDetector.Attach( secondary.grabArea );
      mPanGestureDetector.Attach( secondary.grabArea );
      secondary.grabArea.TouchedSignal().Connect( this, &Decorator::Impl::OnHandleTwoTouched );

      secondary.actor.Add( secondary.grabArea );
      mActiveLayer.Add( secondary.actor );
    }
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
    if( actor == mHandle[GRAB_HANDLE].actor )
    {
      // TODO
    }
  }

  void DoPan( HandleImpl& handle, HandleType type, const PanGesture& gesture )
  {
    if( Gesture::Started == gesture.state )
    {
      handle.grabDisplacementX = handle.grabDisplacementY = 0;
    }

    handle.grabDisplacementX += gesture.displacement.x;
    handle.grabDisplacementY += gesture.displacement.y;

    const float x = handle.position.x + handle.grabDisplacementX;
    const float y = handle.position.y + handle.lineHeight*0.5f + handle.grabDisplacementY;

    if( Gesture::Started    == gesture.state ||
        Gesture::Continuing == gesture.state )
    {
      if( x < mScrollThreshold )
      {
        mScrollDirection = SCROLL_RIGHT;
        mHandleScrolling = type;
        StartScrollTimer();
      }
      else if( x > mTextControlParent.Self().GetTargetSize().width - mScrollThreshold )
      {
        mScrollDirection = SCROLL_LEFT;
        mHandleScrolling = type;
        StartScrollTimer();
      }
      else
      {
        mHandleScrolling = HANDLE_TYPE_COUNT;
        StopScrollTimer();
        mObserver.HandleEvent( type, HANDLE_PRESSED, x, y );
      }
    }
    else if( Gesture::Finished  == gesture.state ||
             Gesture::Cancelled == gesture.state )
    {
      if( mScrollTimer && mScrollTimer.IsRunning() )
      {
        mHandleScrolling = HANDLE_TYPE_COUNT;
        StopScrollTimer();
        mObserver.HandleEvent( type, HANDLE_STOP_SCROLLING, x, y );
      }
      else
      {
        mObserver.HandleEvent( type, HANDLE_RELEASED, x, y );
      }
      handle.actor.SetImage( mHandleImages[type][HANDLE_IMAGE_RELEASED] );
    }
  }

  void OnPan( Actor actor, const PanGesture& gesture )
  {
    HandleImpl& grabHandle = mHandle[GRAB_HANDLE];
    HandleImpl& primarySelectionHandle = mHandle[LEFT_SELECTION_HANDLE];
    HandleImpl& secondarySelectionHandle = mHandle[RIGHT_SELECTION_HANDLE];

    if( actor == grabHandle.grabArea )
    {
      DoPan( grabHandle, GRAB_HANDLE, gesture );
    }
    else if( actor == primarySelectionHandle.grabArea )
    {
      DoPan( primarySelectionHandle, LEFT_SELECTION_HANDLE, gesture );
    }
    else if( actor == secondarySelectionHandle.grabArea )
    {
      DoPan( secondarySelectionHandle, RIGHT_SELECTION_HANDLE, gesture );
    }
  }

  bool OnGrabHandleTouched( Actor actor, const TouchEvent& event )
  {
    // Switch between pressed/release grab-handle images
    if( event.GetPointCount() > 0 &&
        mHandle[GRAB_HANDLE].actor )
    {
      const TouchPoint& point = event.GetPoint(0);

      if( TouchPoint::Down == point.state &&
          mHandleImages[GRAB_HANDLE][HANDLE_IMAGE_PRESSED] )
      {
        mHandle[GRAB_HANDLE].actor.SetImage( mHandleImages[GRAB_HANDLE][HANDLE_IMAGE_PRESSED] );
      }
      else if( TouchPoint::Up == point.state &&
               mHandleImages[GRAB_HANDLE][HANDLE_IMAGE_RELEASED] )
      {
        mHandle[GRAB_HANDLE].actor.SetImage( mHandleImages[GRAB_HANDLE][HANDLE_IMAGE_RELEASED] );
      }
    }

    // Consume to avoid pop-ups accidentally closing, when handle is outside of pop-up area
    return true;
  }

  bool OnHandleOneTouched( Actor actor, const TouchEvent& touch )
  {
    // TODO
    // Consume to avoid pop-ups accidentally closing, when handle is outside of pop-up area
    return true;
  }

  bool OnHandleTwoTouched( Actor actor, const TouchEvent& touch )
  {
    // TODO
    // Consume to avoid pop-ups accidentally closing, when handle is outside of pop-up area
    return true;
  }

  // Popup

  float AlternatePopUpPositionRelativeToCursor()
  {
    float alternativePosition=0.0f;;

    if ( mPrimaryCursor ) // Secondary cursor not used for paste
    {
      Cursor cursor = PRIMARY_CURSOR;
      alternativePosition = mCursor[cursor].position.y;
    }

    const float popupHeight = 120.0f; // todo Set as a MaxSize Property in Control or retrieve from CopyPastePopup class.

    if( mHandle[GRAB_HANDLE].active )
    {
      // If grab handle enabled then position pop-up below the grab handle.
      const Vector2 grabHandleSize( 59.0f, 56.0f ); // todo
      const float BOTTOM_HANDLE_BOTTOM_OFFSET = 1.5; //todo Should be a property
      alternativePosition +=  grabHandleSize.height  + popupHeight + BOTTOM_HANDLE_BOTTOM_OFFSET ;
    }
    else
    {
      alternativePosition += popupHeight;
    }

    return alternativePosition;
  }

  void PopUpLeavesVerticalBoundary( PropertyNotification& source )
  {
    float alternativeYPosition=0.0f;
    // todo use AlternatePopUpPositionRelativeToSelectionHandles() if text is highlighted
    // if can't be positioned above, then position below row.
    alternativeYPosition = AlternatePopUpPositionRelativeToCursor();

    mCopyPastePopup.SetY( alternativeYPosition );
  }


  void SetUpPopUpPositionNotifications( )
  {
    // Note Property notifications ignore any set anchor point so conditions must allow for this.  Default is Top Left.

    // Exceeding vertical boundary

    Vector4 worldCoordinatesBoundingBox;
    LocalToWorldCoordinatesBoundingBox( mBoundingBox, worldCoordinatesBoundingBox );

    float popupHeight = mCopyPastePopup.GetRelayoutSize( Dimension::HEIGHT);

    PropertyNotification verticalExceedNotification = mCopyPastePopup.AddPropertyNotification( Actor::Property::WORLD_POSITION_Y,
                                                      OutsideCondition( worldCoordinatesBoundingBox.y + popupHeight/2,
                                                                        worldCoordinatesBoundingBox.w - popupHeight/2 ) );

    verticalExceedNotification.NotifySignal().Connect( this, &Decorator::Impl::PopUpLeavesVerticalBoundary );
  }

  void GetConstrainedPopupPosition( Vector3& requiredPopupPosition, Vector3& popupSize, Vector3 anchorPoint, Actor& parent, Rect<int>& boundingBox )
  {
    DALI_ASSERT_DEBUG ( "Popup parent not on stage" && parent.OnStage() )

    // Parent must already by added to Stage for these Get calls to work
    Vector3 parentAnchorPoint = parent.GetCurrentAnchorPoint();
    Vector3 parentWorldPositionLeftAnchor = parent.GetCurrentWorldPosition() - parent.GetCurrentSize()*parentAnchorPoint;
    Vector3 popupWorldPosition = parentWorldPositionLeftAnchor + requiredPopupPosition;  // Parent World position plus popup local position gives World Position
    Vector3 popupDistanceFromAnchorPoint = popupSize*anchorPoint;

    // Bounding rectangle is supplied as screen coordinates, bounding will be done in world coordinates.
    Vector4 boundingRectangleWorld;
    LocalToWorldCoordinatesBoundingBox( boundingBox, boundingRectangleWorld );

    // Calculate distance to move popup (in local space) so fits within the boundary
    float xOffSetToKeepWithinBounds = 0.0f;
    if( popupWorldPosition.x - popupDistanceFromAnchorPoint.x < boundingRectangleWorld.x )
    {
      xOffSetToKeepWithinBounds = boundingRectangleWorld.x - ( popupWorldPosition.x - popupDistanceFromAnchorPoint.x );
    }
    else if ( popupWorldPosition.x +  popupDistanceFromAnchorPoint.x > boundingRectangleWorld.z )
    {
      xOffSetToKeepWithinBounds = boundingRectangleWorld.z - ( popupWorldPosition.x +  popupDistanceFromAnchorPoint.x );
    }

    // Ensure initial display of Popup is in alternative position if can not fit above. As Property notification will be a frame behind.
    if ( popupWorldPosition.y - popupDistanceFromAnchorPoint.y < boundingRectangleWorld.y )
    {
      requiredPopupPosition.y = AlternatePopUpPositionRelativeToCursor();
    }

    requiredPopupPosition.x = requiredPopupPosition.x + xOffSetToKeepWithinBounds;
  }

  void SetScrollThreshold( float threshold )
  {
    mScrollThreshold = threshold;
  }

  float GetScrollThreshold() const
  {
    return mScrollThreshold;
  }

  void SetScrollSpeed( float speed )
  {
    mScrollSpeed = speed;
    mScrollDistance = speed * SCROLL_TICK_INTERVAL * TO_SECONDS;
  }

  float GetScrollSpeed() const
  {
    return mScrollSpeed;
  }

  /**
   * Creates and starts a timer to scroll the text when handles are close to the edges of the text.
   *
   * It only starts the timer if it's already created.
   */
  void StartScrollTimer()
  {
    if( !mScrollTimer )
    {
      mScrollTimer = Timer::New( SCROLL_TICK_INTERVAL );
      mScrollTimer.TickSignal().Connect( this, &Decorator::Impl::OnScrollTimerTick );
    }

    if( !mScrollTimer.IsRunning() )
    {
      mScrollTimer.Start();
    }
  }

  /**
   * Stops the timer used to scroll the text.
   */
  void StopScrollTimer()
  {
    if( mScrollTimer )
    {
      mScrollTimer.Stop();
    }
  }

  /**
   * Callback called by the timer used to scroll the text.
   *
   * It calculates and sets a new scroll position.
   */
  bool OnScrollTimerTick()
  {
    if( HANDLE_TYPE_COUNT != mHandleScrolling )
    {
      mObserver.HandleEvent( mHandleScrolling,
                             HANDLE_SCROLLING,
                             mScrollDirection == SCROLL_RIGHT ? mScrollDistance : -mScrollDistance,
                             0.f );
    }

    return true;
  }

  Internal::Control&  mTextControlParent;
  Observer&           mObserver;

  TapGestureDetector  mTapDetector;
  PanGestureDetector  mPanGestureDetector;
  Timer               mCursorBlinkTimer;          ///< Timer to signal cursor to blink
  Timer               mScrollTimer;               ///< Timer used to scroll the text when the grab handle is moved close to the edges.

  Layer               mActiveLayer;               ///< Layer for active handles and alike that ensures they are above all else.
  ImageActor          mPrimaryCursor;
  ImageActor          mSecondaryCursor;
  MeshActor           mHighlightMeshActor;        ///< Mesh Actor to display highlight
  TextSelectionPopup  mCopyPastePopup;

  Image               mHandleImages[HANDLE_TYPE_COUNT][HANDLE_IMAGE_TYPE_COUNT];
  Image               mCursorImage;
  Mesh                mHighlightMesh;             ///< Mesh for highlight
  MeshData            mHighlightMeshData;         ///< Mesh Data for highlight
  Material            mHighlightMaterial;         ///< Material used for highlight

  CursorImpl          mCursor[CURSOR_COUNT];
  HandleImpl          mHandle[HANDLE_TYPE_COUNT];
  QuadContainer       mHighlightQuadList;         ///< Sub-selections that combine to create the complete selection highlight

  Rect<int>           mBoundingBox;
  Vector4             mHighlightColor;            ///< Color of the highlight

  unsigned int        mActiveCursor;
  unsigned int        mCursorBlinkInterval;
  float               mCursorBlinkDuration;
  HandleType          mHandleScrolling;         ///< The handle which is scrolling.
  ScrollDirection     mScrollDirection;         ///< The direction of the scroll.
  float               mScrollThreshold;         ///< Defines a square area inside the control, close to the edge. A cursor entering this area will trigger scroll events.
  float               mScrollSpeed;             ///< The scroll speed in pixels per second.
  float               mScrollDistance;          ///< Distance the text scrolls during a scroll interval.
  unsigned int        mScrollInterval;          ///< Time in milliseconds of a scroll interval.

  bool                mActiveCopyPastePopup   : 1;
  bool                mCursorBlinkStatus      : 1; ///< Flag to switch between blink on and blink off.
  bool                mPrimaryCursorVisible   : 1; ///< Whether the primary cursor is visible.
  bool                mSecondaryCursorVisible : 1; ///< Whether the secondary cursor is visible.
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

void Decorator::Relayout( const Vector2& size )
{
  mImpl->Relayout( size );
}

void Decorator::UpdatePositions( const Vector2& scrollOffset )
{
  mImpl->UpdatePositions( scrollOffset );
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

void Decorator::SetPosition( Cursor cursor, float x, float y, float cursorHeight, float lineHeight )
{
  mImpl->mCursor[cursor].position.x = x;
  mImpl->mCursor[cursor].position.y = y;
  mImpl->mCursor[cursor].cursorHeight = cursorHeight;
  mImpl->mCursor[cursor].lineHeight = lineHeight;
}

void Decorator::GetPosition( Cursor cursor, float& x, float& y, float& cursorHeight, float& lineHeight ) const
{
  x = mImpl->mCursor[cursor].position.x;
  y = mImpl->mCursor[cursor].position.y;
  cursorHeight = mImpl->mCursor[cursor].cursorHeight;
  lineHeight = mImpl->mCursor[cursor].lineHeight;
}

const Vector2& Decorator::GetPosition( Cursor cursor ) const
{
  return mImpl->mCursor[cursor].position;
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
  mImpl->mCursorBlinkInterval = static_cast<unsigned int>( seconds * TO_MILLISECONDS ); // Convert to milliseconds
}

float Decorator::GetCursorBlinkInterval() const
{
  return static_cast<float>( mImpl->mCursorBlinkInterval ) * TO_SECONDS;
}

void Decorator::SetCursorBlinkDuration( float seconds )
{
  mImpl->mCursorBlinkDuration = seconds;
}

float Decorator::GetCursorBlinkDuration() const
{
  return mImpl->mCursorBlinkDuration;
}

/** Handles **/

void Decorator::SetHandleActive( HandleType handleType, bool active )
{
  mImpl->mHandle[handleType].active = active;
}

bool Decorator::IsHandleActive( HandleType handleType ) const
{
  return mImpl->mHandle[handleType].active ;
}

void Decorator::SetHandleImage( HandleType handleType, HandleImageType handleImageType, Dali::Image image )
{
  mImpl->mHandleImages[handleType][handleImageType] = image;
}

Dali::Image Decorator::GetHandleImage( HandleType handleType, HandleImageType handleImageType ) const
{
  return mImpl->mHandleImages[handleType][handleImageType];
}

void Decorator::SetPosition( HandleType handleType, float x, float y, float height )
{
  // Adjust grab handle displacement
  Impl::HandleImpl& handle = mImpl->mHandle[handleType];

  handle.grabDisplacementX -= x - handle.position.x;
  handle.grabDisplacementY -= y - handle.position.y;

  handle.position.x = x;
  handle.position.y = y;
  handle.lineHeight = height;
}

void Decorator::GetPosition( HandleType handleType, float& x, float& y, float& height ) const
{
  Impl::HandleImpl& handle = mImpl->mHandle[handleType];

  x = handle.position.x;
  y = handle.position.y;
  height = handle.lineHeight;
}

void Decorator::AddHighlight( float x1, float y1, float x2, float y2 )
{
  mImpl->mHighlightQuadList.push_back( QuadCoordinates(x1, y1, x2, y2) );
}

void Decorator::ClearHighlights()
{
  mImpl->mHighlightQuadList.clear();
}

void Decorator::SetHighlightColor( const Vector4& color )
{
  mImpl->mHighlightColor = color;
}

const Vector4& Decorator::GetHighlightColor() const
{
  return mImpl->mHighlightColor;
}

void Decorator::SetPopupActive( bool active )
{
  mImpl->mActiveCopyPastePopup = active;
}

bool Decorator::IsPopupActive() const
{
  return mImpl->mActiveCopyPastePopup ;
}

/** Scroll **/

void Decorator::SetScrollThreshold( float threshold )
{
  mImpl->SetScrollThreshold( threshold );
}

float Decorator::GetScrollThreshold() const
{
  return mImpl->GetScrollThreshold();
}

void Decorator::SetScrollSpeed( float speed )
{
  mImpl->SetScrollSpeed( speed );
}

float Decorator::GetScrollSpeed() const
{
  return mImpl->GetScrollSpeed();
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
