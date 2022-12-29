/*
 * Copyright (c) 2022 Samsung Electronics Co., Ltd.
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
#include <dali-toolkit/internal/text/controller/text-controller-relayouter.h>

// EXTERNAL INCLUDES
#include <dali/integration-api/debug.h>
#include <dali/integration-api/trace.h>
#include <dali/public-api/math/math-utils.h>
#include <limits>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/text/controller/text-controller-event-handler.h>
#include <dali-toolkit/internal/text/controller/text-controller-impl.h>
#include <dali-toolkit/internal/text/layouts/layout-parameters.h>

namespace
{
#if defined(DEBUG_ENABLED)
Debug::Filter* gLogFilter = Debug::Filter::New(Debug::NoLogging, true, "LOG_TEXT_CONTROLS");
#endif

DALI_INIT_TRACE_FILTER(gTraceFilter, DALI_TRACE_PERFORMANCE_MARKER, false);

constexpr float MAX_FLOAT = std::numeric_limits<float>::max();

float ConvertToEven(float value)
{
  int intValue(static_cast<int>(value));
  return static_cast<float>(intValue + (intValue & 1));
}

} // namespace

namespace Dali
{
namespace Toolkit
{
namespace Text
{
Size Controller::Relayouter::CalculateLayoutSizeOnRequiredControllerSize(Controller& controller, const Size& requestedControllerSize, const OperationsMask& requestedOperationsMask)
{
  DALI_LOG_INFO(gLogFilter, Debug::Verbose, "-->CalculateLayoutSizeOnRequiredControllerSize\n");
  Size calculatedLayoutSize;

  Controller::Impl& impl        = *controller.mImpl;
  ModelPtr&         model       = impl.mModel;
  VisualModelPtr&   visualModel = model->mVisualModel;

  // Operations that can be done only once until the text changes.
  const OperationsMask onlyOnceOperations = static_cast<OperationsMask>(CONVERT_TO_UTF32 |
                                                                        GET_SCRIPTS |
                                                                        VALIDATE_FONTS |
                                                                        GET_LINE_BREAKS |
                                                                        BIDI_INFO |
                                                                        SHAPE_TEXT |
                                                                        GET_GLYPH_METRICS);

  const OperationsMask sizeOperations = static_cast<OperationsMask>(LAYOUT | ALIGN | REORDER);

  // Set the update info to relayout the whole text.
  TextUpdateInfo& textUpdateInfo = impl.mTextUpdateInfo;
  if((0 == textUpdateInfo.mNumberOfCharactersToAdd) &&
     (0 == textUpdateInfo.mPreviousNumberOfCharacters) &&
     ((visualModel->mControlSize.width < Math::MACHINE_EPSILON_1000) || (visualModel->mControlSize.height < Math::MACHINE_EPSILON_1000)))
  {
    textUpdateInfo.mNumberOfCharactersToAdd = model->mLogicalModel->mText.Count();
  }
  textUpdateInfo.mParagraphCharacterIndex     = 0u;
  textUpdateInfo.mRequestedNumberOfCharacters = model->mLogicalModel->mText.Count();

  // Get a reference to the pending operations member
  OperationsMask& operationsPending = impl.mOperationsPending;

  // Store the actual control's size to restore later.
  const Size actualControlSize = visualModel->mControlSize;

  // This is to keep Index to the first character to be updated.
  // Then restore it after calling Clear method.
  auto updateInfoCharIndexBackup = textUpdateInfo.mCharacterIndex;

  // Whether the text control is editable
  const bool isEditable = NULL != impl.mEventData;

  if(!isEditable)
  {
    impl.UpdateModel(onlyOnceOperations);

    // Layout the text for the new width.
    operationsPending = static_cast<OperationsMask>(operationsPending | requestedOperationsMask);

    DoRelayout(impl,
               requestedControllerSize,
               static_cast<OperationsMask>(onlyOnceOperations | requestedOperationsMask),
               calculatedLayoutSize);

    textUpdateInfo.Clear();
    textUpdateInfo.mClearAll = true;

    // Do not do again the only once operations.
    operationsPending = static_cast<OperationsMask>(operationsPending & ~onlyOnceOperations);
  }
  else
  {
    // Layout the text for the new width.
    // Apply the pending operations, requested operations and the only once operations.
    // Then remove onlyOnceOperations
    operationsPending = static_cast<OperationsMask>(operationsPending | requestedOperationsMask | onlyOnceOperations);

    // Make sure the model is up-to-date before layouting
    impl.UpdateModel(static_cast<OperationsMask>(operationsPending & ~UPDATE_LAYOUT_SIZE));

    DoRelayout(impl,
               requestedControllerSize,
               static_cast<OperationsMask>(operationsPending & ~UPDATE_LAYOUT_SIZE),
               calculatedLayoutSize);

    // Clear the update info. This info will be set the next time the text is updated.
    textUpdateInfo.Clear();

    //TODO: Refactor "DoRelayout" and extract common code of size calculation without modifying attributes of mVisualModel,
    //TODO: then calculate GlyphPositions. Lines, Size, Layout for Natural-Size
    //TODO: and utilize the values in OperationsPending and TextUpdateInfo without changing the original one.
    //TODO: Also it will improve performance because there is no need todo FullRelyout on the next need for layouting.
  }

  // FullRelayoutNeeded should be true because DoRelayout is MAX_FLOAT, MAX_FLOAT.
  // By this no need to take backup and restore it.
  textUpdateInfo.mFullRelayoutNeeded = true;

  // Restore mCharacterIndex. Because "Clear" set it to the maximum integer.
  // The "CalculateTextUpdateIndices" does not work proprely because the mCharacterIndex will be greater than mPreviousNumberOfCharacters.
  // Which apply an assumption to update only the last  paragraph. That could cause many of out of index crashes.
  textUpdateInfo.mCharacterIndex = updateInfoCharIndexBackup;

  // Do the size related operations again.
  operationsPending = static_cast<OperationsMask>(operationsPending | sizeOperations);

  // Restore the actual control's size.
  visualModel->mControlSize = actualControlSize;

  return calculatedLayoutSize;
}

Vector3 Controller::Relayouter::GetNaturalSize(Controller& controller)
{
  DALI_LOG_INFO(gLogFilter, Debug::Verbose, "-->Controller::GetNaturalSize\n");
  DALI_TRACE_BEGIN(gTraceFilter, "DALI_TEXT_GET_NATURAL_SIZE");
  Vector3 naturalSizeVec3;

  // Make sure the model is up-to-date before layouting
  EventHandler::ProcessModifyEvents(controller);

  Controller::Impl& impl        = *controller.mImpl;
  ModelPtr&         model       = impl.mModel;
  VisualModelPtr&   visualModel = model->mVisualModel;

  if(impl.mRecalculateNaturalSize)
  {
    Size naturalSize;

    // Layout the text for the new width.
    OperationsMask requestedOperationsMask  = static_cast<OperationsMask>(LAYOUT | REORDER);
    Size           sizeMaxWidthAndMaxHeight = Size(MAX_FLOAT, MAX_FLOAT);

    naturalSize = CalculateLayoutSizeOnRequiredControllerSize(controller, sizeMaxWidthAndMaxHeight, requestedOperationsMask);

    // Stores the natural size to avoid recalculate it again
    // unless the text/style changes.
    visualModel->SetNaturalSize(naturalSize);
    naturalSizeVec3 = naturalSize;

    impl.mRecalculateNaturalSize = false;

    DALI_LOG_INFO(gLogFilter, Debug::Verbose, "<--Controller::GetNaturalSize calculated %f,%f,%f\n", naturalSizeVec3.x, naturalSizeVec3.y, naturalSizeVec3.z);
  }
  else
  {
    naturalSizeVec3 = visualModel->GetNaturalSize();

    DALI_LOG_INFO(gLogFilter, Debug::Verbose, "<--Controller::GetNaturalSize cached %f,%f,%f\n", naturalSizeVec3.x, naturalSizeVec3.y, naturalSizeVec3.z);
  }

  naturalSizeVec3.x = ConvertToEven(naturalSizeVec3.x);
  naturalSizeVec3.y = ConvertToEven(naturalSizeVec3.y);

  DALI_TRACE_END(gTraceFilter, "DALI_TEXT_GET_NATURAL_SIZE");

  return naturalSizeVec3;
}

bool Controller::Relayouter::CheckForTextFit(Controller& controller, float pointSize, const Size& layoutSize)
{
  Size              textSize;
  Controller::Impl& impl            = *controller.mImpl;
  TextUpdateInfo&   textUpdateInfo  = impl.mTextUpdateInfo;
  impl.mFontDefaults->mFitPointSize = pointSize;
  impl.mFontDefaults->sizeDefined   = true;
  impl.ClearFontData();

  // Operations that can be done only once until the text changes.
  const OperationsMask onlyOnceOperations = static_cast<OperationsMask>(CONVERT_TO_UTF32 |
                                                                        GET_SCRIPTS |
                                                                        VALIDATE_FONTS |
                                                                        GET_LINE_BREAKS |
                                                                        BIDI_INFO |
                                                                        SHAPE_TEXT |
                                                                        GET_GLYPH_METRICS);

  textUpdateInfo.mParagraphCharacterIndex     = 0u;
  textUpdateInfo.mRequestedNumberOfCharacters = impl.mModel->mLogicalModel->mText.Count();

  // Make sure the model is up-to-date before layouting
  impl.UpdateModel(onlyOnceOperations);

  DoRelayout(impl,
             Size(layoutSize.width, MAX_FLOAT),
             static_cast<OperationsMask>(onlyOnceOperations | LAYOUT),
             textSize);

  // Clear the update info. This info will be set the next time the text is updated.
  textUpdateInfo.Clear();
  textUpdateInfo.mClearAll = true;

  if(textSize.width >= layoutSize.width || textSize.height >= layoutSize.height)
  {
    return false;
  }
  return true;
}

void Controller::Relayouter::FitPointSizeforLayout(Controller& controller, const Size& layoutSize)
{
  Controller::Impl& impl = *controller.mImpl;

  const OperationsMask operations = impl.mOperationsPending;
  if(NO_OPERATION != (UPDATE_LAYOUT_SIZE & operations) || impl.mTextFitContentSize != layoutSize)
  {
    DALI_TRACE_BEGIN(gTraceFilter, "DALI_TEXT_FIT_LAYOUT");
    ModelPtr& model = impl.mModel;

    bool  actualellipsis      = model->mElideEnabled;
    float minPointSize        = impl.mTextFitMinSize;
    float maxPointSize        = impl.mTextFitMaxSize;
    float pointInterval       = impl.mTextFitStepSize;
    float currentFitPointSize = impl.mFontDefaults->mFitPointSize;

    model->mElideEnabled = false;

    // check zero value
    if(pointInterval < 1.f)
    {
      impl.mTextFitStepSize = pointInterval = 1.0f;
    }
    uint32_t pointSizeRange = static_cast<uint32_t>(ceil((maxPointSize - minPointSize) / pointInterval));

    // Ensure minPointSize + pointSizeRange * pointInverval >= maxPointSize
    while(minPointSize + static_cast<float>(pointSizeRange) * pointInterval < maxPointSize)
    {
      ++pointSizeRange;
    }

    uint32_t bestSizeIndex = 0;
    uint32_t minIndex      = bestSizeIndex + 1u;
    uint32_t maxIndex      = pointSizeRange + 1u;

    bool bestSizeUpdatedLatest = false;
    // Find best size as binary search.
    // Range format as [l r). (left closed, right opened)
    // It mean, we already check all i < l is valid, and r <= i is invalid.
    // Below binary search will check m = (l+r)/2 point.
    // Search area sperate as [l m) or [m+1 r)
    //
    // Basically, we can assume that 0 (minPointSize) is always valid.
    // Now, we will check [1 pointSizeRange] range s.t. pointSizeRange mean the maxPointSize
    while(minIndex < maxIndex)
    {
      uint32_t    testIndex     = minIndex + ((maxIndex - minIndex) >> 1u);
      const float testPointSize = std::min(maxPointSize, minPointSize + static_cast<float>(testIndex) * pointInterval);

      if(CheckForTextFit(controller, testPointSize, layoutSize))
      {
        bestSizeUpdatedLatest = true;

        bestSizeIndex = testIndex;
        minIndex      = testIndex + 1u;
      }
      else
      {
        bestSizeUpdatedLatest = false;
        maxIndex              = testIndex;
      }
    }
    const float bestPointSize = std::min(maxPointSize, minPointSize + static_cast<float>(bestSizeIndex) * pointInterval);

    // Best point size was not updated. re-run so the TextFit should be fitted really.
    if(!bestSizeUpdatedLatest)
    {
      CheckForTextFit(controller, bestPointSize, layoutSize);
    }

    model->mElideEnabled = actualellipsis;
    if(!Dali::Equals(currentFitPointSize, bestPointSize))
    {
      impl.mTextFitChanged = true;
    }
    impl.mFontDefaults->mFitPointSize = bestPointSize;
    impl.mFontDefaults->sizeDefined   = true;
    impl.ClearFontData();

    DALI_TRACE_END(gTraceFilter, "DALI_TEXT_FIT_LAYOUT");
  }
}

float Controller::Relayouter::GetHeightForWidth(Controller& controller, float width)
{
  DALI_LOG_INFO(gLogFilter, Debug::Verbose, "-->Controller::GetHeightForWidth %p width %f\n", &controller, width);
  DALI_TRACE_BEGIN(gTraceFilter, "DALI_TEXT_GET_HEIGHT_FOR_WIDTH");

  // Make sure the model is up-to-date before layouting
  EventHandler::ProcessModifyEvents(controller);

  Controller::Impl& impl           = *controller.mImpl;
  ModelPtr&         model          = impl.mModel;
  VisualModelPtr&   visualModel    = model->mVisualModel;
  TextUpdateInfo&   textUpdateInfo = impl.mTextUpdateInfo;

  Size layoutSize;

  if(fabsf(width - visualModel->mControlSize.width) > Math::MACHINE_EPSILON_1000 ||
     textUpdateInfo.mFullRelayoutNeeded ||
     textUpdateInfo.mClearAll)
  {
    // Layout the text for the new width.
    OperationsMask requestedOperationsMask        = static_cast<OperationsMask>(LAYOUT);
    Size           sizeRequestedWidthAndMaxHeight = Size(width, MAX_FLOAT);

    layoutSize = CalculateLayoutSizeOnRequiredControllerSize(controller, sizeRequestedWidthAndMaxHeight, requestedOperationsMask);

    DALI_LOG_INFO(gLogFilter, Debug::Verbose, "<--Controller::GetHeightForWidth calculated %f\n", layoutSize.height);
  }
  else
  {
    layoutSize = visualModel->GetLayoutSize();
    DALI_LOG_INFO(gLogFilter, Debug::Verbose, "<--Controller::GetHeightForWidth cached %f\n", layoutSize.height);
  }

  DALI_TRACE_END(gTraceFilter, "DALI_TEXT_GET_HEIGHT_FOR_WIDTH");

  return layoutSize.height;
}

Controller::UpdateTextType Controller::Relayouter::Relayout(Controller& controller, const Size& size, Dali::LayoutDirection::Type layoutDirection)
{
  Controller::Impl& impl           = *controller.mImpl;
  ModelPtr&         model          = impl.mModel;
  VisualModelPtr&   visualModel    = model->mVisualModel;
  TextUpdateInfo&   textUpdateInfo = impl.mTextUpdateInfo;

  DALI_LOG_INFO(gLogFilter, Debug::Verbose, "-->Controller::Relayout %p size %f,%f, autoScroll[%s]\n", &controller, size.width, size.height, impl.mIsAutoScrollEnabled ? "true" : "false");
  DALI_TRACE_BEGIN(gTraceFilter, "DALI_TEXT_RELAYOUT");

  UpdateTextType updateTextType = NONE_UPDATED;

  if((size.width < Math::MACHINE_EPSILON_1000) || (size.height < Math::MACHINE_EPSILON_1000))
  {
    if(0u != visualModel->mGlyphPositions.Count())
    {
      visualModel->mGlyphPositions.Clear();
      updateTextType = MODEL_UPDATED;
    }

    // Clear the update info. This info will be set the next time the text is updated.
    textUpdateInfo.Clear();

    // Not worth to relayout if width or height is equal to zero.
    DALI_LOG_INFO(gLogFilter, Debug::Verbose, "<--Controller::Relayout (skipped)\n");
    DALI_TRACE_END(gTraceFilter, "DALI_TEXT_RELAYOUT");

    return updateTextType;
  }

  // Whether a new size has been set.
  const bool newSize = (size != visualModel->mControlSize);

  // Get a reference to the pending operations member
  OperationsMask& operationsPending = impl.mOperationsPending;

  if(newSize)
  {
    DALI_LOG_INFO(gLogFilter, Debug::Verbose, "new size (previous size %f,%f)\n", visualModel->mControlSize.width, visualModel->mControlSize.height);

    if((0 == textUpdateInfo.mNumberOfCharactersToAdd) &&
       (0 == textUpdateInfo.mPreviousNumberOfCharacters) &&
       ((visualModel->mControlSize.width < Math::MACHINE_EPSILON_1000) || (visualModel->mControlSize.height < Math::MACHINE_EPSILON_1000)))
    {
      textUpdateInfo.mNumberOfCharactersToAdd = model->mLogicalModel->mText.Count();
    }

    // Layout operations that need to be done if the size changes.
    operationsPending = static_cast<OperationsMask>(operationsPending |
                                                    LAYOUT |
                                                    ALIGN |
                                                    UPDATE_LAYOUT_SIZE |
                                                    REORDER);
    // Set the update info to relayout the whole text.
    textUpdateInfo.mFullRelayoutNeeded = true;
    textUpdateInfo.mCharacterIndex     = 0u;

    // Store the size used to layout the text.
    visualModel->mControlSize = size;
  }

  // Whether there are modify events.
  if(0u != impl.mModifyEvents.Count())
  {
    // Style operations that need to be done if the text is modified.
    operationsPending = static_cast<OperationsMask>(operationsPending | COLOR);
  }

  // Set the update info to elide the text.
  if(model->mElideEnabled ||
     ((NULL != impl.mEventData) && impl.mEventData->mIsPlaceholderElideEnabled))
  {
    // Update Text layout for applying elided
    operationsPending                  = static_cast<OperationsMask>(operationsPending |
                                                    ALIGN |
                                                    LAYOUT |
                                                    UPDATE_LAYOUT_SIZE |
                                                    REORDER);
    textUpdateInfo.mFullRelayoutNeeded = true;
    textUpdateInfo.mCharacterIndex     = 0u;
  }

  bool layoutDirectionChanged = false;
  if(impl.mLayoutDirection != layoutDirection)
  {
    // Flag to indicate that the layout direction has changed.
    layoutDirectionChanged = true;
    // Clear the update info. This info will be set the next time the text is updated.
    textUpdateInfo.mClearAll = true;
    // Apply modifications to the model
    // Shape the text again is needed because characters like '()[]{}' have to be mirrored and the glyphs generated again.
    operationsPending     = static_cast<OperationsMask>(operationsPending |
                                                    GET_GLYPH_METRICS |
                                                    SHAPE_TEXT |
                                                    UPDATE_DIRECTION |
                                                    ALIGN |
                                                    LAYOUT |
                                                    BIDI_INFO |
                                                    REORDER);
    impl.mLayoutDirection = layoutDirection;
  }

  // Make sure the model is up-to-date before layouting.
  EventHandler::ProcessModifyEvents(controller);
  bool updated = impl.UpdateModel(operationsPending);

  // Layout the text.
  Size layoutSize;
  updated = DoRelayout(impl, size, operationsPending, layoutSize) || updated;

  if(updated)
  {
    updateTextType = MODEL_UPDATED;
  }

  // Do not re-do any operation until something changes.
  operationsPending          = NO_OPERATION;
  model->mScrollPositionLast = model->mScrollPosition;

  // Whether the text control is editable
  const bool isEditable = NULL != impl.mEventData;

  // Keep the current offset as it will be used to update the decorator's positions (if the size changes).
  Vector2 offset;
  if(newSize && isEditable)
  {
    offset = model->mScrollPosition;
  }

  if(!isEditable || !controller.IsMultiLineEnabled())
  {
    // After doing the text layout, the vertical offset to place the actor in the desired position can be calculated.
    CalculateVerticalOffset(impl, size);
  }
  else // TextEditor
  {
    // If layoutSize is bigger than size, vertical align has no meaning.
    if(layoutSize.y < size.y)
    {
      CalculateVerticalOffset(impl, size);
      if(impl.mEventData)
      {
        impl.mEventData->mScrollAfterDelete = false;
      }
    }
  }

  if(isEditable)
  {
    if(newSize || layoutDirectionChanged)
    {
      // If there is a new size or layout direction is changed, the scroll position needs to be clamped.
      impl.ClampHorizontalScroll(layoutSize);

      // Update the decorator's positions is needed if there is a new size.
      impl.mEventData->mDecorator->UpdatePositions(model->mScrollPosition - offset);

      // All decorator elements need to be updated.
      if(EventData::IsEditingState(impl.mEventData->mState))
      {
        impl.mEventData->mScrollAfterUpdatePosition = true;
        impl.mEventData->mUpdateCursorPosition      = true;
        impl.mEventData->mUpdateGrabHandlePosition  = true;
      }
      else if(impl.mEventData->mState == EventData::SELECTING)
      {
        impl.mEventData->mUpdateHighlightBox = true;
      }
    }

    // Move the cursor, grab handle etc.
    if(impl.ProcessInputEvents())
    {
      updateTextType = static_cast<UpdateTextType>(updateTextType | DECORATOR_UPDATED);
    }
  }

  // Clear the update info. This info will be set the next time the text is updated.
  textUpdateInfo.Clear();
  DALI_LOG_INFO(gLogFilter, Debug::Verbose, "<--Controller::Relayout\n");
  DALI_TRACE_END(gTraceFilter, "DALI_TEXT_RELAYOUT");

  return updateTextType;
}

bool Controller::Relayouter::DoRelayout(Controller::Impl& impl, const Size& size, OperationsMask operationsRequired, Size& layoutSize)
{
  DALI_LOG_INFO(gLogFilter, Debug::Verbose, "-->Controller::Relayouter::DoRelayout %p size %f,%f\n", &impl, size.width, size.height);
  DALI_TRACE_BEGIN(gTraceFilter, "DALI_TEXT_DORELAYOUT");
  bool viewUpdated(false);

  // Calculate the operations to be done.
  const OperationsMask operations = static_cast<OperationsMask>(impl.mOperationsPending & operationsRequired);

  TextUpdateInfo&      textUpdateInfo              = impl.mTextUpdateInfo;
  const CharacterIndex startIndex                  = textUpdateInfo.mParagraphCharacterIndex;
  const Length         requestedNumberOfCharacters = textUpdateInfo.mRequestedNumberOfCharacters;

  // Get the current layout size.
  VisualModelPtr& visualModel = impl.mModel->mVisualModel;
  layoutSize                  = visualModel->GetLayoutSize();

  if(NO_OPERATION != (LAYOUT & operations))
  {
    DALI_LOG_INFO(gLogFilter, Debug::Verbose, "-->Controller::DoRelayout LAYOUT & operations\n");

    // Some vectors with data needed to layout and reorder may be void
    // after the first time the text has been laid out.
    // Fill the vectors again.

    // Calculate the number of glyphs to layout.
    const Vector<GlyphIndex>& charactersToGlyph        = visualModel->mCharactersToGlyph;
    const Vector<Length>&     glyphsPerCharacter       = visualModel->mGlyphsPerCharacter;
    const GlyphIndex* const   charactersToGlyphBuffer  = charactersToGlyph.Begin();
    const Length* const       glyphsPerCharacterBuffer = glyphsPerCharacter.Begin();

    const CharacterIndex lastIndex       = startIndex + ((requestedNumberOfCharacters > 0u) ? requestedNumberOfCharacters - 1u : 0u);
    const GlyphIndex     startGlyphIndex = textUpdateInfo.mStartGlyphIndex;

    // Make sure the index is not out of bound
    if(charactersToGlyph.Count() != glyphsPerCharacter.Count() ||
       requestedNumberOfCharacters > charactersToGlyph.Count() ||
       (lastIndex > charactersToGlyph.Count() && charactersToGlyph.Count() > 0u))
    {
      std::string currentText;
      impl.GetText(currentText);

      DALI_LOG_ERROR("Controller::DoRelayout: Attempting to access invalid buffer\n");
      DALI_LOG_ERROR("Current text is: %s\n", currentText.c_str());
      DALI_LOG_ERROR("startIndex: %u, lastIndex: %u, requestedNumberOfCharacters: %u, charactersToGlyph.Count = %lu, glyphsPerCharacter.Count = %lu\n", startIndex, lastIndex, requestedNumberOfCharacters, charactersToGlyph.Count(), glyphsPerCharacter.Count());

      DALI_TRACE_END(gTraceFilter, "DALI_TEXT_DORELAYOUT");

      return false;
    }

    const Length numberOfGlyphs      = (requestedNumberOfCharacters > 0u) ? *(charactersToGlyphBuffer + lastIndex) + *(glyphsPerCharacterBuffer + lastIndex) - startGlyphIndex : 0u;
    const Length totalNumberOfGlyphs = visualModel->mGlyphs.Count();

    if(0u == totalNumberOfGlyphs)
    {
      if(NO_OPERATION != (UPDATE_LAYOUT_SIZE & operations))
      {
        visualModel->SetLayoutSize(Size::ZERO);
      }

      // Nothing else to do if there is no glyphs.
      DALI_LOG_INFO(gLogFilter, Debug::Verbose, "<--Controller::DoRelayout no glyphs, view updated true\n");
      DALI_TRACE_END(gTraceFilter, "DALI_TEXT_DORELAYOUT");
      return true;
    }

    // Set the layout parameters.
    Layout::Parameters layoutParameters(size, impl.mModel);

    // Resize the vector of positions to have the same size than the vector of glyphs.
    Vector<Vector2>& glyphPositions = visualModel->mGlyphPositions;
    glyphPositions.Resize(totalNumberOfGlyphs);

    // Whether the last character is a new paragraph character.
    const Character* const textBuffer           = impl.mModel->mLogicalModel->mText.Begin();
    textUpdateInfo.mIsLastCharacterNewParagraph = TextAbstraction::IsNewParagraph(*(textBuffer + (impl.mModel->mLogicalModel->mText.Count() - 1u)));
    layoutParameters.isLastNewParagraph         = textUpdateInfo.mIsLastCharacterNewParagraph;

    // The initial glyph and the number of glyphs to layout.
    layoutParameters.startGlyphIndex        = startGlyphIndex;
    layoutParameters.numberOfGlyphs         = numberOfGlyphs;
    layoutParameters.startLineIndex         = textUpdateInfo.mStartLineIndex;
    layoutParameters.estimatedNumberOfLines = textUpdateInfo.mEstimatedNumberOfLines;

    // Update the ellipsis
    bool elideTextEnabled = impl.mModel->mElideEnabled;
    auto ellipsisPosition = impl.mModel->mEllipsisPosition;

    if(NULL != impl.mEventData)
    {
      if(impl.mEventData->mPlaceholderEllipsisFlag && impl.IsShowingPlaceholderText())
      {
        elideTextEnabled = impl.mEventData->mIsPlaceholderElideEnabled;
      }
      else if(EventData::INACTIVE != impl.mEventData->mState)
      {
        // Disable ellipsis when editing
        elideTextEnabled = false;
      }

      // Reset the scroll position in inactive state
      if(elideTextEnabled && (impl.mEventData->mState == EventData::INACTIVE))
      {
        impl.ResetScrollPosition();
      }
    }

    // Update the visual model.
    bool isAutoScrollEnabled            = impl.mIsAutoScrollEnabled;
    bool isAutoScrollMaxTextureExceeded = impl.mIsAutoScrollMaxTextureExceeded;
    bool isHiddenInputEnabled           = false;
    if(impl.mHiddenInput && impl.mEventData != nullptr && impl.mHiddenInput->GetHideMode() != Toolkit::HiddenInput::Mode::HIDE_NONE)
    {
      isHiddenInputEnabled = true;
    }

    Size newLayoutSize;
    viewUpdated               = impl.mLayoutEngine.LayoutText(layoutParameters,
                                                newLayoutSize,
                                                elideTextEnabled,
                                                isAutoScrollEnabled,
                                                isAutoScrollMaxTextureExceeded,
                                                isHiddenInputEnabled,
                                                ellipsisPosition);
    impl.mIsAutoScrollEnabled = isAutoScrollEnabled;

    viewUpdated = viewUpdated || (newLayoutSize != layoutSize);

    if(viewUpdated)
    {
      layoutSize = newLayoutSize;

      if(NO_OPERATION != (UPDATE_DIRECTION & operations))
      {
        impl.mIsTextDirectionRTL = false;
      }

      if((NO_OPERATION != (UPDATE_DIRECTION & operations)) && !visualModel->mLines.Empty())
      {
        impl.mIsTextDirectionRTL = visualModel->mLines[0u].direction;
      }

      // Sets the layout size.
      if(NO_OPERATION != (UPDATE_LAYOUT_SIZE & operations))
      {
        visualModel->SetLayoutSize(layoutSize);
      }
    } // view updated
  }

  if(NO_OPERATION != (ALIGN & operations))
  {
    DoRelayoutHorizontalAlignment(impl, size, startIndex, requestedNumberOfCharacters);
    viewUpdated = true;
  }
#if defined(DEBUG_ENABLED)
  std::string currentText;
  impl.GetText(currentText);
  DALI_LOG_INFO(gLogFilter, Debug::Concise, "Controller::Relayouter::DoRelayout [%p] mImpl->mIsTextDirectionRTL[%s] [%s]\n", &impl, (impl.mIsTextDirectionRTL) ? "true" : "false", currentText.c_str());
#endif
  DALI_LOG_INFO(gLogFilter, Debug::Verbose, "<--Controller::Relayouter::DoRelayout, view updated %s\n", (viewUpdated ? "true" : "false"));
  DALI_TRACE_END(gTraceFilter, "DALI_TEXT_DORELAYOUT");
  return viewUpdated;
}

void Controller::Relayouter::DoRelayoutHorizontalAlignment(Controller::Impl&    impl,
                                                           const Size&          size,
                                                           const CharacterIndex startIndex,
                                                           const Length         requestedNumberOfCharacters)
{
  // The visualModel
  VisualModelPtr& visualModel = impl.mModel->mVisualModel;

  // The laid-out lines.
  Vector<LineRun>& lines = visualModel->mLines;

  CharacterIndex alignStartIndex                  = startIndex;
  Length         alignRequestedNumberOfCharacters = requestedNumberOfCharacters;

  // the whole text needs to be full aligned.
  // If you do not do a full aligned, only the last line of the multiline input is aligned.
  if(impl.mEventData && impl.mEventData->mUpdateAlignment)
  {
    alignStartIndex                   = 0u;
    alignRequestedNumberOfCharacters  = impl.mModel->mLogicalModel->mText.Count();
    impl.mEventData->mUpdateAlignment = false;
  }

  // If there is no BoundedParagraphRuns then apply the alignment of controller.
  // Check whether the layout is single line. It's needed to apply one alignment for single-line.
  // In single-line layout case we need to check whether to follow the alignment of controller or the first BoundedParagraph.
  // Apply BoundedParagraph's alignment if and only if there is one BoundedParagraph contains all characters. Otherwise follow controller's alignment.
  const bool isFollowControllerAlignment = ((impl.mModel->GetNumberOfBoundedParagraphRuns() == 0u) ||
                                            ((Layout::Engine::SINGLE_LINE_BOX == impl.mLayoutEngine.GetLayout()) &&
                                             (impl.mModel->GetBoundedParagraphRuns()[0].characterRun.numberOfCharacters != impl.mModel->mLogicalModel->mText.Count())));

  if(isFollowControllerAlignment)
  {
    // Need to align with the control's size as the text may contain lines
    // starting either with left to right text or right to left.
    impl.mLayoutEngine.Align(size,
                             alignStartIndex,
                             alignRequestedNumberOfCharacters,
                             impl.mModel->mHorizontalAlignment,
                             lines,
                             impl.mModel->mAlignmentOffset,
                             impl.mLayoutDirection,
                             (impl.mModel->mMatchLayoutDirection != DevelText::MatchLayoutDirection::CONTENTS));
  }
  else
  {
    //Override the controller horizontal-alignment by horizontal-alignment of bounded paragraph.
    const Length&                      numberOfBoundedParagraphRuns = impl.mModel->GetNumberOfBoundedParagraphRuns();
    const Vector<BoundedParagraphRun>& boundedParagraphRuns         = impl.mModel->GetBoundedParagraphRuns();
    const CharacterIndex               alignEndIndex                = alignStartIndex + alignRequestedNumberOfCharacters - 1u;

    Length alignIndex               = alignStartIndex;
    Length boundedParagraphRunIndex = 0u;

    while(alignIndex <= alignEndIndex && boundedParagraphRunIndex < numberOfBoundedParagraphRuns)
    {
      //BP: BoundedParagraph
      const BoundedParagraphRun& boundedParagraphRun   = boundedParagraphRuns[boundedParagraphRunIndex];
      const CharacterIndex&      characterStartIndexBP = boundedParagraphRun.characterRun.characterIndex;
      const Length&              numberOfCharactersBP  = boundedParagraphRun.characterRun.numberOfCharacters;
      const CharacterIndex       characterEndIndexBP   = characterStartIndexBP + numberOfCharactersBP - 1u;

      CharacterIndex                  decidedAlignStartIndex         = alignIndex;
      Length                          decidedAlignNumberOfCharacters = alignEndIndex - alignIndex + 1u;
      Text::HorizontalAlignment::Type decidedHorizontalAlignment     = impl.mModel->mHorizontalAlignment;

      /*
         * Shortcuts to explain indexes cases:
         *
         * AS: Alignment Start Index
         * AE: Alignment End Index
         * PS: Paragraph Start Index
         * PE: Paragraph End Index
         * B: BoundedParagraph Alignment
         * M: Model Alignment
         *
         */

      if(alignIndex < characterStartIndexBP && characterStartIndexBP <= alignEndIndex) /// AS.MMMMMM.PS--------AE
      {
        // Alignment from "Alignment Start Index" to index before "Paragraph Start Index" according to "Model Alignment"
        decidedAlignStartIndex         = alignIndex;
        decidedAlignNumberOfCharacters = characterStartIndexBP - alignIndex;
        decidedHorizontalAlignment     = impl.mModel->mHorizontalAlignment;

        // Need to re-heck the case of current bounded paragraph
        alignIndex = characterStartIndexBP; // Shift AS to be PS
      }
      else if((characterStartIndexBP <= alignIndex && alignIndex <= characterEndIndexBP) ||     /// ---PS.BBBBBBB.AS.BBBBBBB.PE---
              (characterStartIndexBP <= alignEndIndex && alignEndIndex <= characterEndIndexBP)) /// ---PS.BBBBBB.AE.BBBBBBB.PE---
      {
        // Alignment from "Paragraph Start Index" to "Paragraph End Index" according to "BoundedParagraph Alignment"
        decidedAlignStartIndex         = characterStartIndexBP;
        decidedAlignNumberOfCharacters = numberOfCharactersBP;
        decidedHorizontalAlignment     = boundedParagraphRun.horizontalAlignmentDefined ? boundedParagraphRun.horizontalAlignment : impl.mModel->mHorizontalAlignment;

        alignIndex = characterEndIndexBP + 1u; // Shift AS to be after PE direct
        boundedParagraphRunIndex++;            // Align then check the case of next bounded paragraph
      }
      else
      {
        boundedParagraphRunIndex++; // Check the case of next bounded paragraph
        continue;
      }

      impl.mLayoutEngine.Align(size,
                               decidedAlignStartIndex,
                               decidedAlignNumberOfCharacters,
                               decidedHorizontalAlignment,
                               lines,
                               impl.mModel->mAlignmentOffset,
                               impl.mLayoutDirection,
                               (impl.mModel->mMatchLayoutDirection != DevelText::MatchLayoutDirection::CONTENTS));
    }

    //Align the remaining that is not aligned
    if(alignIndex <= alignEndIndex)
    {
      impl.mLayoutEngine.Align(size,
                               alignIndex,
                               (alignEndIndex - alignIndex + 1u),
                               impl.mModel->mHorizontalAlignment,
                               lines,
                               impl.mModel->mAlignmentOffset,
                               impl.mLayoutDirection,
                               (impl.mModel->mMatchLayoutDirection != DevelText::MatchLayoutDirection::CONTENTS));
    }
  }
}

void Controller::Relayouter::CalculateVerticalOffset(Controller::Impl& impl, const Size& controlSize)
{
  ModelPtr&       model                 = impl.mModel;
  VisualModelPtr& visualModel           = model->mVisualModel;
  Size            layoutSize            = model->mVisualModel->GetLayoutSize();
  Size            oldLayoutSize         = layoutSize;
  float           offsetY               = 0.f;
  bool            needRecalc            = false;
  float           defaultFontLineHeight = impl.GetDefaultFontLineHeight();

  if(fabsf(layoutSize.height) < Math::MACHINE_EPSILON_1000)
  {
    // Get the line height of the default font.
    layoutSize.height = defaultFontLineHeight;
  }

  // Whether the text control is editable
  const bool isEditable = NULL != impl.mEventData;
  if(isEditable && !Dali::Equals(layoutSize.height, defaultFontLineHeight) && impl.IsShowingPlaceholderText())
  {
    // This code prevents the wrong positioning of cursor when the layout size is bigger/smaller than defaultFontLineHeight.
    // This situation occurs when the size of placeholder text is different from the default text.
    layoutSize.height = defaultFontLineHeight;
    needRecalc        = true;
  }

  switch(model->mVerticalAlignment)
  {
    case VerticalAlignment::TOP:
    {
      model->mScrollPosition.y = 0.f;
      offsetY                  = 0.f;
      break;
    }
    case VerticalAlignment::CENTER:
    {
      model->mScrollPosition.y = floorf(0.5f * (controlSize.height - layoutSize.height)); // try to avoid pixel alignment.
      if(needRecalc) offsetY = floorf(0.5f * (layoutSize.height - oldLayoutSize.height));
      break;
    }
    case VerticalAlignment::BOTTOM:
    {
      model->mScrollPosition.y = controlSize.height - layoutSize.height;
      if(needRecalc) offsetY = layoutSize.height - oldLayoutSize.height;
      break;
    }
  }

  if(needRecalc)
  {
    // Update glyphPositions according to recalculation.
    const Length     positionCount  = visualModel->mGlyphPositions.Count();
    Vector<Vector2>& glyphPositions = visualModel->mGlyphPositions;
    for(Length index = 0u; index < positionCount; index++)
    {
      glyphPositions[index].y += offsetY;
    }
  }
}

} // namespace Text

} // namespace Toolkit

} // namespace Dali
