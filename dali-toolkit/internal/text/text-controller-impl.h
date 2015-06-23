#ifndef __DALI_TOOLKIT_TEXT_CONTROLLER_IMPL_H__
#define __DALI_TOOLKIT_TEXT_CONTROLLER_IMPL_H__

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

// EXTERNAL INCLUDES
#include <dali/devel-api/adaptor-framework/clipboard.h>
#include <dali/devel-api/adaptor-framework/imf-manager.h>
#include <dali/devel-api/text-abstraction/font-client.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/text/layouts/layout-engine.h>
#include <dali-toolkit/internal/text/logical-model-impl.h>
#include <dali-toolkit/internal/text/text-controller.h>
#include <dali-toolkit/internal/text/visual-model-impl.h>

namespace Dali
{

namespace Toolkit
{

namespace Text
{

struct Event
{
  // Used to queue input events until DoRelayout()
  enum Type
  {
    CURSOR_KEY_EVENT,
    TAP_EVENT,
    PAN_EVENT,
    GRAB_HANDLE_EVENT,
    LEFT_SELECTION_HANDLE_EVENT,
    RIGHT_SELECTION_HANDLE_EVENT,
    SELECT,
    SELECT_ALL
  };

  union Param
  {
    int mInt;
    unsigned int mUint;
    float mFloat;
  };

  Event( Type eventType )
  : type( eventType )
  {
    p1.mInt = 0;
    p2.mInt = 0;
    p3.mInt = 0;
  }

  Type type;
  Param p1;
  Param p2;
  Param p3;
};

struct CursorInfo
{
  CursorInfo()
  : primaryPosition(),
    secondaryPosition(),
    lineHeight( 0.f ),
    primaryCursorHeight( 0.f ),
    secondaryCursorHeight( 0.f ),
    isSecondaryCursor( false )
  {}

  ~CursorInfo()
  {}

  Vector2 primaryPosition;       ///< The primary cursor's position.
  Vector2 secondaryPosition;     ///< The secondary cursor's position.
  float   lineHeight;            ///< The height of the line where the cursor is placed.
  float   primaryCursorHeight;   ///< The primary cursor's height.
  float   secondaryCursorHeight; ///< The secondary cursor's height.
  bool    isSecondaryCursor;     ///< Whether the secondary cursor is valid.
};

struct EventData
{
  enum State
  {
    INACTIVE,
    SELECTING,
    SELECTION_CHANGED,
    EDITING,
    EDITING_WITH_POPUP,
    GRAB_HANDLE_PANNING,
    SELECTION_HANDLE_PANNING
  };

  EventData( DecoratorPtr decorator );

  ~EventData();

  DecoratorPtr       mDecorator;               ///< Pointer to the decorator
  std::string        mPlaceholderTextActive;   ///< The text to display when the TextField is empty with key-input focus
  std::string        mPlaceholderTextInactive; ///< The text to display when the TextField is empty and inactive
  Vector4            mPlaceholderTextColor;    ///< The in/active placeholder text color

  /**
   * This is used to delay handling events until after the model has been updated.
   * The number of updates to the model is minimized to improve performance.
   */
  std::vector<Event> mEventQueue;              ///< The queue of touch events etc.

  /**
   * 0,0 means that the top-left corner of the layout matches the top-left corner of the UI control.
   * Typically this will have a negative value with scrolling occurs.
   */
  Vector2            mScrollPosition;          ///< The text is offset by this position when scrolling.

  State              mState;                   ///< Selection mode, edit mode etc.

  CharacterIndex     mPrimaryCursorPosition;   ///< Index into logical model for primary cursor.
  CharacterIndex     mLeftSelectionPosition;   ///< Index into logical model for left selection handle.
  CharacterIndex     mRightSelectionPosition;  ///< Index into logical model for right selection handle.

  CharacterIndex     mPreEditStartPosition;    ///< Used to remove the pre-edit text if necessary.
  Length             mPreEditLength;           ///< Used to remove the pre-edit text if necessary.

  bool mIsShowingPlaceholderText        : 1;   ///< True if the place-holder text is being displayed.
  bool mPreEditFlag                     : 1;   ///< True if the model contains text in pre-edit state.
  bool mDecoratorUpdated                : 1;   ///< True if the decorator was updated during event processing.
  bool mCursorBlinkEnabled              : 1;   ///< True if cursor should blink when active.
  bool mGrabHandleEnabled               : 1;   ///< True if grab handle is enabled.
  bool mGrabHandlePopupEnabled          : 1;   ///< True if the grab handle popu-up should be shown.
  bool mSelectionEnabled                : 1;   ///< True if selection handles, highlight etc. are enabled.
  bool mHorizontalScrollingEnabled      : 1;   ///< True if horizontal scrolling is enabled.
  bool mVerticalScrollingEnabled        : 1;   ///< True if vertical scrolling is enabled.
  bool mUpdateCursorPosition            : 1;   ///< True if the visual position of the cursor must be recalculated.
  bool mUpdateLeftSelectionPosition     : 1;   ///< True if the visual position of the left selection handle must be recalculated.
  bool mUpdateRightSelectionPosition    : 1;   ///< True if the visual position of the right selection handle must be recalculated.
  bool mScrollAfterUpdatePosition       : 1;   ///< Whether to scroll after the cursor position is updated.
  bool mScrollAfterDelete               : 1;   ///< Whether to scroll after delete characters.
};

struct ModifyEvent
{
  enum Type
  {
    TEXT_REPLACED,    ///< The entire text was replaced
    TEXT_INSERTED,    ///< Insert characters at the current cursor position
    TEXT_DELETED      ///< Characters were deleted
  };

  Type type;
};

struct FontDefaults
{
  FontDefaults()
  : mDefaultPointSize(0.0f),
    mFontId(0u)
  {
  }

  FontId GetFontId( TextAbstraction::FontClient& fontClient )
  {
    if( !mFontId )
    {
      Dali::TextAbstraction::PointSize26Dot6 pointSize = mDefaultPointSize*64;
      mFontId = fontClient.GetFontId( mDefaultFontFamily, mDefaultFontStyle, pointSize );
    }

    return mFontId;
  }

  std::string mDefaultFontFamily;
  std::string mDefaultFontStyle;
  float mDefaultPointSize;
  FontId mFontId;
};

struct Controller::Impl
{
  Impl( ControlInterface& controlInterface )
  : mControlInterface( controlInterface ),
    mLogicalModel(),
    mVisualModel(),
    mFontDefaults( NULL ),
    mEventData( NULL ),
    mFontClient(),
    mClipboard(),
    mView(),
    mLayoutEngine(),
    mModifyEvents(),
    mControlSize(),
    mTextColor( Color::BLACK ),
    mAlignmentOffset(),
    mOperationsPending( NO_OPERATION ),
    mMaximumNumberOfCharacters( 50 ),
    mRecalculateNaturalSize( true )
  {
    mLogicalModel = LogicalModel::New();
    mVisualModel  = VisualModel::New();

    mFontClient = TextAbstraction::FontClient::Get();
    mClipboard = Clipboard::Get();

    mView.SetVisualModel( mVisualModel );

    // Set the text properties to default
    mVisualModel->SetUnderlineEnabled( false );
    mVisualModel->SetUnderlineHeight( 0.0f );
  }

  ~Impl()
  {
    delete mEventData;
  }

  /**
   * @brief Request a relayout using the ControlInterface.
   */
  void RequestRelayout();

  /**
   * @brief Request a relayout using the ControlInterface.
   */
  void QueueModifyEvent( ModifyEvent::Type type )
  {
    if( ModifyEvent::TEXT_REPLACED == type)
    {
      // Cancel previously queued inserts etc.
      mModifyEvents.clear();
    }

    ModifyEvent event;
    event.type = type;
    mModifyEvents.push_back( event );

    // The event will be processed during relayout
    RequestRelayout();
  }

  /**
   * @brief Helper to move the cursor, grab handle etc.
   */
  bool ProcessInputEvents();

  /**
   * @brief Helper to check whether any place-holder text is available.
   */
  bool IsPlaceholderAvailable() const
  {
    return ( mEventData &&
             ( !mEventData->mPlaceholderTextInactive.empty() ||
               !mEventData->mPlaceholderTextActive.empty() )
           );
  }

  bool IsShowingPlaceholderText() const
  {
    return ( mEventData && mEventData->mIsShowingPlaceholderText );
  }

  /**
   * @brief Called when placeholder-text is hidden
   */
  void PlaceholderCleared()
  {
    if( mEventData )
    {
      mEventData->mIsShowingPlaceholderText = false;

      // Remove mPlaceholderTextColor
      mVisualModel->SetTextColor( mTextColor );
    }
  }

  void ClearPreEditFlag()
  {
    if( mEventData )
    {
      mEventData->mPreEditFlag = false;
      mEventData->mPreEditStartPosition = 0;
      mEventData->mPreEditLength = 0;
    }
  }

  void ResetImfManager()
  {
    // Reset incase we are in a pre-edit state.
    ImfManager imfManager = ImfManager::Get();
    if ( imfManager )
    {
      imfManager.Reset(); // Will trigger a commit message
    }

    ClearPreEditFlag();
  }

  bool IsClipboardEmpty()
  {
    bool result( mClipboard && mClipboard.NumberOfItems() );
    return !result; // // If NumberOfItems greater than 0, return false
  }

  void UpdateModel( OperationsMask operationsRequired );

  /**
   * @brief Retrieve the default fonts.
   *
   * @param[out] fonts The default font family, style and point sizes.
   * @param[in] numberOfCharacters The number of characters in the logical model.
   */
  void GetDefaultFonts( Dali::Vector<FontRun>& fonts, Length numberOfCharacters );

  void OnCursorKeyEvent( const Event& event );

  void OnTapEvent( const Event& event );

  void OnPanEvent( const Event& event );

  void OnHandleEvent( const Event& event );

  void OnSelectEvent( const Event& event );

  void OnSelectAllEvent();

  void RetreiveSelection( std::string& selectedText, bool deleteAfterRetreival );

  bool CopyStringToClipboard( std::string& source );

  void SendSelectionToClipboard( bool deleteAfterSending );

  void GetTextFromClipboard( unsigned int itemIndex, std::string& retreivedString );

  void RepositionSelectionHandles( CharacterIndex selectionStart, CharacterIndex selectionEnd );
  void RepositionSelectionHandles( float visualX, float visualY );

  void ChangeState( EventData::State newState );

  LineIndex GetClosestLine( float y ) const;

  void FindSelectionIndices( float visualX, float visualY, CharacterIndex& startIndex, CharacterIndex& endIndex );

  /**
   * @brief Retrieves the cursor's logical position for a given touch point x,y
   *
   * @param[in] visualX The touch point x.
   * @param[in] visualY The touch point y.
   *
   * @return The logical cursor position (in characters). 0 is just before the first character, a value equal to the number of characters is just after the last character.
   */
  CharacterIndex GetClosestCursorIndex( float visualX,
                                        float visualY );

  /**
   * @brief Calculates the cursor's position for a given character index in the logical order.
   *
   * It retrieves as well the line's height and the cursor's height and
   * if there is a valid alternative cursor, its position and height.
   *
   * @param[in] logical The logical cursor position (in characters). 0 is just before the first character, a value equal to the number of characters is just after the last character.
   * @param[out] cursorInfo The line's height, the cursor's height, the cursor's position and whether there is an alternative cursor.
   */
  void GetCursorPosition( CharacterIndex logical,
                          CursorInfo& cursorInfo );

  /**
   * @brief Calculates the new cursor index.
   *
   * It takes into account that in some scripts multiple characters can form a glyph and all of them
   * need to be jumped with one key event.
   *
   * @param[in] index The initial new index.
   *
   * @return The new cursor index.
   */
  CharacterIndex CalculateNewCursorIndex( CharacterIndex index ) const;

  /**
   * @brief Updates the cursor position.
   *
   * Retrieves the x,y position of the cursor logical position and sets it into the decorator.
   * It sets the position of the secondary cursor if it's a valid one.
   * Sets which cursors are active.
   */
  void UpdateCursorPosition();

  /**
   * @brief Updates the position of the given selection handle.
   *
   * @param[in] handleType One of the selection handles.
   */
  void UpdateSelectionHandle( HandleType handleType );

  /**
   * @biref Clamps the horizontal scrolling to get the control always filled with text.
   *
   * @param[in] actualSize The size of the laid out text.
   */
  void ClampHorizontalScroll( const Vector2& actualSize );

  /**
   * @biref Clamps the vertical scrolling to get the control always filled with text.
   *
   * @param[in] actualSize The size of the laid out text.
   */
  void ClampVerticalScroll( const Vector2& actualSize );

  /**
   * @brief Scrolls the text to make a position visible.
   *
   * @pre mEventData must not be NULL. (there is a text-input or selection capabilities).
   *
   * @param[in] position A position in decorator coords.
   *
   * This method is called after inserting text, moving the cursor with the grab handle or the keypad,
   * or moving the selection handles.
   */
  void ScrollToMakePositionVisible( const Vector2& position );

  /**
   * @brief Scrolls the text to make the cursor visible.
   *
   * This method is called after deleting text.
   */
  void ScrollTextToMatchCursor();

  ControlInterface& mControlInterface;     ///< Reference to the text controller.
  LogicalModelPtr mLogicalModel;           ///< Pointer to the logical model.
  VisualModelPtr  mVisualModel;            ///< Pointer to the visual model.
  FontDefaults* mFontDefaults;             ///< Avoid allocating this when the user does not specify a font.
  EventData* mEventData;                   ///< Avoid allocating everything for text input until EnableTextInput().
  TextAbstraction::FontClient mFontClient; ///< Handle to the font client.
  Clipboard mClipboard;                   ///< Handle to the system clipboard
  View mView;                              ///< The view interface to the rendering back-end.
  LayoutEngine mLayoutEngine;              ///< The layout engine.
  std::vector<ModifyEvent> mModifyEvents;  ///< Temporary stores the text set until the next relayout.
  Size mControlSize;                       ///< The size of the control.
  Vector4 mTextColor;                      ///< The regular text color
  Vector2 mAlignmentOffset;                ///< Vertical and horizontal offset of the whole text inside the control due to alignment.
  OperationsMask mOperationsPending;       ///< Operations pending to be done to layout the text.
  Length mMaximumNumberOfCharacters;       ///< Maximum number of characters that can be inserted.
  bool mRecalculateNaturalSize:1;          ///< Whether the natural size needs to be recalculated.
};

} // namespace Text

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_TEXT_CONTROLLER_H__
