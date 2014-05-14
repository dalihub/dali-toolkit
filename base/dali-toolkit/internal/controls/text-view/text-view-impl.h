#ifndef __DALI_TOOLKIT_INTERNAL_TEXT_VIEW_H__
#define __DALI_TOOLKIT_INTERNAL_TEXT_VIEW_H__

//
// Copyright (c) 2014 Samsung Electronics Co., Ltd.
//
// Licensed under the Flora License, Version 1.0 (the License);
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://floralicense.org/license/
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an AS IS BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

// INTERNAL INCLUDES
#include <dali/public-api/common/dali-vector.h>

#include <dali/public-api/actors/text-actor.h>
#include <dali-toolkit/public-api/controls/control-impl.h>
#include <dali-toolkit/public-api/controls/text-view/text-view.h>
#include "text-actor-cache.h"
#include "text-view-processor-types.h"

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

/**
 * TextView is a custom control for text aligning and multiline support
 */
class TextView : public Control
{
public:

  // Properties
  enum
  {
    TEXTVIEW_PROPERTY_START_INDEX = Control::CONTROL_PROPERTY_END_INDEX + 1,
    TEXTVIEW_PROPERTY_END_INDEX = TEXTVIEW_PROPERTY_START_INDEX + 1000 ///< Reserving 1000 property indices
  };

  /**
   * Internal exceed policy with the valid combinations.
   */
  enum ExceedPolicy
  {
    Original,             ///< Original size (even if it exceeds the witdh or the height).
    OriginalFade,         ///< Original size if it exceeds the width but faded if it exceeds the height.
    OriginalShrink,       ///< Shrunk if it exceeds the height.
    SplitOriginal,        ///< Split if it exceeds the width but no action if it exceeds the height.
    SplitFade,            ///< Split if it exceeds the width and faded if it exceeds the height.
    SplitShrink,          ///< Split if it exceeds the width and shrunk if it exceeds the height.
    SplitEllipsizeEnd,    ///< Split if it exceeds the width and ellipsize if it exceeds the height.
    Fade,                 ///< Faded if it exceeds any boundary.
    FadeOriginal,         ///< Faded if it exceeds the width but no action if it exceeds the height.
    ShrinkOriginal,       ///< Shrunk if it exceeds the width but no action if it exceeds the height.
    ShrinkFade,           ///< Shrunk if it exceeds the width and faded if it exceeds the height.
    Shrink,               ///< Shrunk if it exceeds any boundary.
    EllipsizeEndOriginal, ///< Ellipsized by the end if it exceeds the width but no action if it exceeds the height.
    EllipsizeEnd          ///< Ellipsized by the end if it exceeds the width and/or the height.
  };

  // Between two OnRelaidOut methods, several calls to InsertTextAt, RemoveTextFrom or SetText can happen.
  // TextViewProcessorMetadata stores the type of operation. A vector stores all operations between two OnRelaidOut calls.

  enum TextViewProcessorMetadataType
  {
    TextSet,             ///< Sets new text.
    TextInserted,        ///< Inserts text into current text.
    TextReplaced,        ///< Replaces some text from current text.
    TextRemoved,         ///< Removes some text from current text.
    NewLineHeight,       ///< Sets a new line height offset.
    NewStyle             ///< Sets a new style to the whole text.
  };

  /**
   * Stores info about which data structures need to be modified when the OnRelaidOut() method is called
   */
  struct TextViewProcessorMetadata
  {
    TextViewProcessorMetadata();

    TextViewProcessorMetadataType    mType;               ///< Stores the type of operation.
    std::size_t                      mPosition;           ///< Character position within the text.
    std::size_t                      mNumberOfCharacters; ///< Number of characters to be removed/ replaced.
    MarkupProcessor::StyledTextArray mText;               ///< The new text.
    TextStyle::Mask                  mStyleMask;          ///< The style mask.
  };

  /**
   * Defines which operations have to be done in the relayout process.
   */
  enum RelayoutOperationMask
  {
    NO_RELAYOUT = 0x0,                         ///< Does nothing.
    RELAYOUT_REMOVE_TEXT_ACTORS = 0x1,         ///< Removes current text-actors from the text-view.
    RELAYOUT_SIZE_POSITION = 0x2,              ///< Calculates size and position of the text but it doesn't calculate alignment.
    RELAYOUT_ALIGNMENT = 0x4,                  ///< Aligns the whole text.
    RELAYOUT_VISIBILITY = 0x8,                 ///< Calculates the visibility.
    RELAYOUT_INITIALIZE_TEXT_ACTORS = 0x10,    ///< Initialize text-actors (create handles).
    RELAYOUT_TEXT_ACTOR_UPDATE = 0x20,         ///< Updates text-actors (set size, position, style, ...)
    RELAYOUT_INSERT_TO_TEXT_VIEW = 0x40,       ///< Adds the text-actors to the text-view.
    RELAYOUT_INSERT_TO_TEXT_ACTOR_LIST = 0x80, ///< Inserts the text-actors to the text-actor list.
    RELAYOUT_ALL = 0xFF                        ///< Does all operations.
  };

  //////////////////////////////////////////////

  /**
   * Create a new TextView.
   * @return A smart-pointer to the newly allocated TextView.
   */
  static Toolkit::TextView New();

  /**
   * @copydoc SetText( const std::string& text )
   */
  void SetText( const std::string& text );

  /**
   * @copydoc SetText( const StyledTextArray& text )
   */
  void SetText( const MarkupProcessor::StyledTextArray& text );

  /**
   * @copydoc InsertTextAt( std::size_t position, const std::string& text )
   */
  void InsertTextAt( std::size_t position, const std::string& text );

  /**
   * @copydoc InsertTextAt( std::size_t position, const std::string& text )
   */
  void InsertTextAt( std::size_t position, const MarkupProcessor::StyledTextArray& text );

  /**
   * @copydoc RemoveTextFrom( std::size_t position, std::size_t numberOfCharacters )
   */
  void RemoveTextFrom( std::size_t position, std::size_t numberOfCharacters );

  /**
   * @copydoc ReplaceTextFromTo( std::size_t position, std::size_t numberOfCharacters, const std::string& text )
   */
  void ReplaceTextFromTo( std::size_t position, std::size_t numberOfCharacters, const std::string& text );

  /**
   * @copydoc ReplaceTextFromTo( std::size_t position, std::size_t numberOfCharacters, const std::string& text )
   */
  void ReplaceTextFromTo( std::size_t position, std::size_t numberOfCharacters, const MarkupProcessor::StyledTextArray& text );

  /**
   * @copydoc GetText()
   */
  std::string GetText() const;

  /**
   * @copydoc SetFont( const Font newFont )
   */
  void SetFont( const Font newFont );

  /**
   * @copydoc SetLineHeightOffset()
   */
  void SetLineHeightOffset( PointSize offset );

  /**
   * @copydoc GetLineHeightOffset()
   */
  PointSize GetLineHeightOffset() const;

  /**
   * @copydoc SetStyleToCurrentText()
   */
  void SetStyleToCurrentText( const TextStyle& style, TextStyle::Mask mask );

  /**
   * @copydoc SetTextAlignment( Toolkit::Alignment::Type align )
   */
  void SetTextAlignment( Toolkit::Alignment::Type align );

  /**
   * @copydoc GetTextAlignment()
   */
  Toolkit::Alignment::Type GetTextAlignment() const;

  /**
   * @copydoc SetMultilinePolicy()
   */
  void SetMultilinePolicy( Toolkit::TextView::MultilinePolicy policy );

  /**
   * @copydoc GetMultilinePolicy()
   */
  Toolkit::TextView::MultilinePolicy GetMultilinePolicy() const;

  /**
   * @copydoc SetWidthExceedPolicy()
   */
  void SetWidthExceedPolicy( Toolkit::TextView::ExceedPolicy policy );

  /**
   * @copydoc GetWidthExceedPolicy()
   */
  Toolkit::TextView::ExceedPolicy GetWidthExceedPolicy() const;

  /**
   * @copydoc SetHeightExceedPolicy()
   */
  void SetHeightExceedPolicy( Toolkit::TextView::ExceedPolicy policy );

  /**
   * @copydoc GetHeightExceedPolicy()
   */
  Toolkit::TextView::ExceedPolicy GetHeightExceedPolicy() const;

  /**
   * @copydoc SetLineJustification()
   */
  void SetLineJustification( Toolkit::TextView::LineJustification justification );

  /**
   * @copydoc GetLineJustification()
   */
  Toolkit::TextView::LineJustification GetLineJustification() const;

  /**
   * @copydoc SetFadeBoundary()
   */
  void SetFadeBoundary( const Toolkit::TextView::FadeBoundary& fadeBoundary );

  /**
   * @copydoc GetFadeBoundary()
   */
  const Toolkit::TextView::FadeBoundary& GetFadeBoundary() const;

  /**
   * @copydoc SetEllipsizeText()
   */
  void SetEllipsizeText( const std::string& ellipsizeText );

  /**
   * @copydoc SetEllipsizeText()
   */
  void SetEllipsizeText( const MarkupProcessor::StyledTextArray& ellipsizeText );

  /**
   * @copydoc GetEllipsizeText()
   */
  std::string GetEllipsizeText() const;

  /**
   * Checks if relayout the text is needed. If it is, it relais out the text
   * by calling DoRelayOut().
   */
  void GetTextLayoutInfo();

  /**
   * Calls GetTextLayoutInfo() and fills the given data structure.
   *
   * @see GetTextLayoutInfo()
   */
  void GetTextLayoutInfo( Toolkit::TextView::TextLayoutInfo& textLayoutInfo );

  /**
   * @copydoc SetSortModifier()
   */
  void SetSortModifier( float depthOffset );

  /**
   * @copydoc SetSnapshotModeEnabled()
   */
  void SetSnapshotModeEnabled( bool enable );

  /**
   * @copydoc IsSnapshotModeEnabled()
   */
  bool IsSnapshotModeEnabled() const;

  /**
   * @brief Sets whether markup processing should be carried out.
   *
   * @param[in] enable whether markup processing is carried out or not.
   */
  void SetMarkupProcessingEnabled( bool enable );

  /**
   * @brief Returns whether markup processing is enabled or not
   *
   * @return true is markup processing is enabled
   */
  bool IsMarkupProcessingEnabled() const;

  /**
   * @copydoc SetScrollEnabled()
   */
  void SetScrollEnabled( bool enable );

  /**
   * @copydoc IsScrollEnabled()
   */
  bool IsScrollEnabled() const;

  /**
   * @copydoc SetScrollPosition()
   * @see DoSetScrollPosition()
   */
  void SetScrollPosition( const Vector2& position );

  /**
   * @copydoc GetScrollPosition()
   */
  const Vector2& GetScrollPosition() const;

  /**
   * @copydoc IsScrollPositionTrimmed()
   */
  bool IsScrollPositionTrimmed() const;

  /**
   * @copydoc ScrolledSignal()
   */
  Toolkit::TextView::ScrolledSignalV2& ScrolledSignal();

  /**
   * Connects a callback function with the object's signals.
   * @param[in] object The object providing the signal.
   * @param[in] tracker Used to disconnect the signal.
   * @param[in] signalName The signal to connect to.
   * @param[in] functor A newly allocated FunctorDelegate.
   * @return True if the signal was connected.
   * @post If a signal was connected, ownership of functor was passed to CallbackBase. Otherwise the caller is responsible for deleting the unused functor.
   */
  static bool DoConnectSignal( BaseObject* object, ConnectionTrackerInterface* tracker, const std::string& signalName, FunctorDelegate* functor );

  // Properties

  /**
   * Called when a property of an object of this type is set.
   * @param[in] object The object whose property is set.
   * @param[in] index The property index.
   * @param[in] value The new property value.
   */
  static void SetProperty( BaseObject* object, Property::Index index, const Property::Value& value );

  /**
   * Called to retrieve a property of an object of this type.
   * @param[in] object The object whose property is to be retrieved.
   * @param[in] index The property index.
   * @return The current value of the property.
   */
  static Property::Value GetProperty( BaseObject* object, Property::Index index );


private: // From Control

  /**
   * @copydoc Toolkit::Control::OnInitialize()
   */
  virtual void OnInitialize();

  /**
   * @copydoc Toolkit::Control::OnStyleChange()
   */
  virtual void OnStyleChange( StyleChange change );

  /**
   * @copydoc Toolkit::Control::OnControlSizeSet()
   */
  virtual void OnControlSizeSet( const Vector3& size );

  /**
   * @copydoc Toolkit::Control::OnRelaidOut()
   *
   * Removes text-actor and calls DoRelayOut()..
   */
  virtual void OnRelaidOut( Vector2 size, ActorSizeContainer& container );

  /**
   * Retrieves the text-view's natural size.
   *
   * @return The natural size.
   */
  virtual Vector3 GetNaturalSize();

  /**
   * Retrieves the text-view's \e height for a given \e width.
   *
   * @param[in] width The given \e width.
   *
   * @return The \e height for the given \e width.
   */
  virtual float GetHeightForWidth( float width );

  /**
   * Retrieves the text-view's \e width for a given \e height.
   *
   * @param[in] height The given \e height.
   *
   * @return The \e width for the given \e height.
   */
  virtual float GetWidthForHeight( float height );

protected:

  /**
   * Construct a new TextView.
   */
  TextView();

  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~TextView();

private:

  // Undefined
  TextView( const TextView& );

  // Undefined
  TextView& operator=( const TextView& rhs );

  /**
   * Executes synchronously relayout operations such as set, insert, remove or replace text, or
   * split groups of characters, etc.
   */
  void PerformTextViewProcessorOperations();

  /**
   * Optimizes some text-view processor operations.
   */
  void OptimizeTextViewProcessorOperations();

  /**
   * Synchronously relays-out all text-actors.
   *
   * Perform text-view-processor operations, sets the new size and position of text-actors and adds them to the text-view.
   *
   * @param[in] textViewSize The new text-view's size.
   * @param[in] relayoutOperationMask Defines which operations need to be done in the relayout process.
   */
  void DoRelayOut( const Size& textViewSize, RelayoutOperationMask relayoutOperationMask );

  /**
   * Process Snapshot. It refresh the render-task in order to generate a new snapshot image.
   *
   * ProcessSnapshot is called from OnRelaidOut() only if text has been relaid out.
   * It creates a new image buffer only if the size of the text has changed.
   *
   * @param[in] textViewSize The new text-view's size.
   */
  void ProcessSnapshot( const Size& textViewSize );

  /**
   * Aligns the offscreen rendering camera actor to cover the whole text and the resulting image actor accordingly with the text view's alignment.
   *
   * @param[in] textViewSize The text view's size.
   * @param[in] offscreenSize The offscreen's frame buffer's size.
   */
  void AlignOffscreenCameraActor( const Size& textViewSize, const Size& offscreenSize );

  /**
   * Callback called when the render task has processed.
   *
   * It makes the root actor invisible.
   *
   * @param[in] renderTask The processed render task.
   */
  void RenderTaskFinished( Dali::RenderTask& renderTask );

  /**
   * Destroys offscreen rendering resources.
   *
   * It disconects the render task finished signal from the TextView::RenderTaskFinished() method,
   * removes the render task from the render task list and resets the offscreen camera actor, root actor,
   * image actor and the frame buffer.
   */
  void DestroyOffscreenRenderingResources();

  /**
   * Called when text-view is scrolled.
   *
   * Sets the new scroll position. @see DoSetScrollPosition()
   *
   * @param[in] actor Handle of the text-view.
   * @param[in] gesture Data structure with the parameters of the gesture.
   */
  void OnTextPan( Actor actor, PanGesture gesture );

  /**
   * Ensures the text-view's boundaries are fully covered of text.
   *
   * i.e. if the text-view's size is 100x100 and the text's size is 150x100, the scroll position
   * can be in the range -50,0 and 50,0.
   */
  void TrimScrollPosition();

  /**
   * Called from SetScrollPosition() and OnTextPan()
   *
   * Updates the stored scroll position ensuring the text-view is always covered with text by calling
   * TrimScrollPosition(), calculates the difference with the previous one and emits the Toolkit::TextView::SignalScrolled() signal.
   *
   * @param[in] position The new scroll position.
   */
  void DoSetScrollPosition( const Vector2& position );

  /**
   * Combines width and height exceed policies.
   *
   * This method is a big switch() which combines two exceed policies into one.
   * The aim is avoid this switch inside the relayout code.
   *
   * i.e.  Width policy = Split. Height policy = Original. Internally the policy is SplitOriginal.
   */
  void CombineExceedPolicies();

  /**
   * Retrieves the text-view's root actor which stores all text-actors.
   * It could be the text-view itself or an actor used in the snapshot mode.
   *
   * @return the root actor.
   */
  Actor GetRootActor() const;

  /**
   * Handle SetProperty for markup processing.
   * @param[in] propertyValue The new property value.
   */
  void OnMarkupEnabledPeopertySet( Property::Value propertyValue );

  /**
   * Handles SetProperty for multiline policy.
   * @param[in] propertyValue The new property value.
   */
  void OnMultilinePolicyPropertySet( Property::Value propertyValue );

  /**
   * Handles SetProperty for width exceed policy.
   * @param[in] propertyValue The new property value.
   */
  void OnWidthExceedPolicyPropertySet( Property::Value propertyValue );

  /**
   * Handles SetProperty for height exceed policy.
   * @param[in] propertyValue The new property value.
   */
  void OnHeightExceedPolicyPropertySet( Property::Value propertyValue );

  /**
   * Handles SetProperty for line justification.
   * @param[in] propertyValue The new property value.
   */
  void OnLineJustificationPropertySet( Property::Value propertyValue );

  /**
   * Handles SetProperty for fade boundary.
   * @param[in] propertyValue The new property value.
   */
  void OnFadeBoundaryPropertySet( Property::Value propertyValue );

  /**
   * Handles SetProperty for alignment property.
   * @param[in] propertyIndex The property index.
   * @param[in] propertyValue The new property value.
   */
  void OnAlignmentPropertySet( Property::Index propertyIndex, Property::Value propertyValue );

  /**
   * Handles GetProperty for horizontal alignment property.
   * @return The property value of horizontal alignment.
   */
  std::string OnHorizontalAlignmentPropertyGet();

  /**
   * Handles GetProperty for vertical alignment property.
   * @return The property value of vertical alignment.
   */
  std::string OnVerticalAlignmentPropertyGet();

public:

  /**
   * The parameters which affects the layout of the text.
   */
  struct LayoutParameters
  {
    /**
     * Default constructor.
     */
    LayoutParameters();

    /**
     * Constructor
     */
    LayoutParameters( Toolkit::TextView::MultilinePolicy     multilinePolicy,
                      Toolkit::TextView::ExceedPolicy        widthExceedPolicy,
                      Toolkit::TextView::ExceedPolicy        heightExceedPolicy,
                      Toolkit::Alignment::Type               alignment,
                      Toolkit::TextView::LineJustification   lineJustification,
                      float                                  lineHeightOffset,
                      const std::string&                     ellipsizeText,
                      const bool                             markUpEnabled );

    /**
     * Copy constructor
     */
    LayoutParameters( const LayoutParameters& layoutParameters );

    /**
     * Assignment operator.
     */
    LayoutParameters& operator=( const LayoutParameters& layoutParameters );

    Toolkit::TextView::MultilinePolicy   mMultilinePolicy;     ///< Stores the multiline policy.
    TextView::ExceedPolicy               mExceedPolicy;        ///< Stores a combination of both policies;
    Toolkit::TextView::ExceedPolicy      mWidthExceedPolicy;   ///< Stores the text width exceed policy.
    Toolkit::TextView::ExceedPolicy      mHeightExceedPolicy;  ///< Stores the text height exceed policy.
    Toolkit::Alignment::Type             mHorizontalAlignment; ///< Stores the horizontal alignment for the whole text.
    Toolkit::Alignment::Type             mVerticalAlignment;   ///< Stores the vertical alignment for the whole text.
    Toolkit::TextView::LineJustification mLineJustification;   ///< Stores the line justification.
    float                                mLineHeightOffset;    ///< Line height offset to be addded to the font line height (measured in PointSize).
    MarkupProcessor::StyledTextArray     mEllipsizeText;       ///< Stores the ellipsize text
    bool                                 mMarkUpEnabled:1;     ///< Is markup string scanning enabled
  };

  /**
   * Some parameters which affects the text view visualization.
   */
  struct VisualParameters
  {
    /**
     * Default constructor.
     */
    VisualParameters();

    /**
     * Copy constructor.
     */
    VisualParameters( const VisualParameters& visualParameters );

    /**
     * Assignment operator.
     */
    VisualParameters& operator=( const VisualParameters& visualParameters );

    Toolkit::TextView::FadeBoundary mFadeBoundary;            ///< Fade boundary used in fade mode.
    float                           mSortModifier;            ///< Stores the sort modifier for all text-actors.
    Vector2                         mCameraScrollPosition;    ///< The scroll offset.
    bool                            mSnapshotModeEnabled:1;   ///< Whether text-view is rendered offscreen.
    bool                            mScrollEnabled:1;         ///< Whether the text scroll is enabled.
    bool                            mScrollPositionTrimmed:1; ///< Whether the last scroll position set was trimmed.
  };

  /**
   * Temporary data used to calculate line justification.
   */
  struct LineJustificationInfo
  {
    TextViewProcessor::TextInfoIndices mIndices;    ///< Indices to the first character of the new line.
    float                              mLineLength; ///< Length of the line (or portion of line).
  };

  /**
   * The results of the relayout process.
   */
  struct RelayoutData
  {
    /**
     * Default constructor.
     */
    RelayoutData();

    /**
     * Copy constructor
     */
    RelayoutData( const RelayoutData& relayoutData );

    /**
     * Assignment operator.
     */
    RelayoutData& operator=( const RelayoutData& relayoutData );

    Size                                            mTextViewSize;                ///< The text-view's size used to relaid-out the text.
    float                                           mShrinkFactor;                ///< Shrink factor used when the exceed policy contains ShrinkToFit.
    TextViewProcessor::TextLayoutInfo               mTextLayoutInfo;              ///< Stores metrics, layout info (size, direction, type of word) and text-actor info for the whole text.
    std::vector<int>                                mCharacterLogicalToVisualMap; ///< Reorder map that stores each character's visual (output) index according to its logical (input) index
    std::vector<int>                                mCharacterVisualToLogicalMap; ///< Reorder map that stores each character's logical (input) index according to its visual (output) index
    std::vector<RenderableActor>                    mGlyphActors;                 ///< Stores handles of those text-actors which are currently added to the text-view.
    std::vector<RenderableActor>                    mEllipsizedGlyphActors;       ///< Stores handles of those text-actors which are used to ellipsize the text.
    Toolkit::TextView::CharacterLayoutInfoContainer mCharacterLayoutInfoTable;    ///< Stores layout info per character sorted by the character's visual index.
    Toolkit::TextView::LineLayoutInfoContainer      mLines;                       ///< Stores an index to the first character of each line.
    Size                                            mTextSizeForRelayoutOption;   ///< Stores the text size after relayout.
    std::vector<LineJustificationInfo>              mLineJustificationInfo;       ///< Stores justification info per line.
    TextActorCache                                  mTextActorCache;              ///< Stores previously created text-actors to be reused.
  };

private:

  MarkupProcessor::StyledTextArray       mCurrentStyledText;           ///< text currently displayed by the view
  std::vector<TextViewProcessorMetadata> mTextViewProcessorOperations; ///< Stores all relayout operations which arrive between two consecutive OnRelaidOut() calls.

  LayoutParameters                       mLayoutParameters;            ///< Stores some layout parameters in a struct. To be passed in layout functions.
  VisualParameters                       mVisualParameters;            ///< Some parameters which afects text-view visualization.
  RelayoutData                           mRelayoutData;                ///< struct with text-view's data structures used to pass all of them in one parameter.
  RelayoutOperationMask                  mRelayoutOperations;          ///< Which relayout operations have to be done.

  Layer                                  mOffscreenRootActor;          ///< Root actor for offscreen rendering.
  ImageActor                             mOffscreenImageActor;         ///< Image actor for offscreen rendering.
  CameraActor                            mOffscreenCameraActor;        ///< Camera actor for offscreen rendering.
  Size                                   mCurrentOffscreenSize;        ///< Current used ofscreen size.
  FrameBufferImage                       mFrameBufferImage;            ///< Frame buffer used for offscreen rendering.
  RenderTask                             mRenderTask;                  ///< Used to generate an offscreen rendering.

  PanGestureDetector                     mPanGestureDetector;          ///< Pan gesture for text scrolling.

  /**
   * Helper class used to prevent the modification of some members.
   */
  struct Lock
  {
    Lock( bool& lock )
    : mLock( lock )
    {
      mLock = true;
    }

    ~Lock()
    {
      mLock = false;
    }

    bool& mLock;
  };

  bool                                            mLockPreviousSnapshotMode;      ///< Whether previous stored snapshot mode should be modified.
  bool                                            mPreviousSnapshotModeEnabled:1; ///< Stores the previous snapshot mode value.
  bool                                            mMarkUpEnabled:1;               ///< enable to scan for mark-up

  Toolkit::TextView::ScrolledSignalV2             mScrolledSignalV2;              ///< Signal emitted when text is scrolled.
};

} // namespace Internal

// Helpers for public-api forwarding methods

inline Internal::TextView& GetImpl( TextView& textView )
{
  DALI_ASSERT_ALWAYS( textView );

  RefObject& handle = textView.GetImplementation();

  return static_cast< Internal::TextView& >( handle );
}

inline const Internal::TextView& GetImpl( const TextView& textView )
{
  DALI_ASSERT_ALWAYS( textView );

  const RefObject& handle = textView.GetImplementation();

  return static_cast< const Internal::TextView& >( handle );
}

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_INTERNAL_ITEM_VIEW_H__
