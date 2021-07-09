/*
 * Copyright (c) 2021 Samsung Electronics Co., Ltd.
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
#include <dali-toolkit/internal/text/text-controller-relayouter.h>

// EXTERNAL INCLUDES
#include <dali/integration-api/debug.h>
#include <limits>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/text/layouts/layout-parameters.h>
#include <dali-toolkit/internal/text/text-controller-impl.h>

namespace
{
#if defined(DEBUG_ENABLED)
Debug::Filter* gLogFilter = Debug::Filter::New(Debug::NoLogging, true, "LOG_TEXT_CONTROLS");
#endif

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
Size Controller::Relayouter::CalculateLayoutSizeOnRequiredControllerSize(Controller& controller, const Size& requestedControllerSize, const OperationsMask& requestedOperationsMask, bool restoreLinesAndGlyphPositions)
{
  DALI_LOG_INFO(gLogFilter, Debug::Verbose, "-->CalculateLayoutSizeOnRequiredControllerSize\n");
  Size calculatedLayoutSize;

  Controller::Impl& impl        = *controller.mImpl;
  ModelPtr&         model       = impl.mModel;
  VisualModelPtr&   visualModel = model->mVisualModel;

  // Store the pending operations mask so that it can be restored later on with no modifications made on it
  // while getting the natural size were reflected on the original mask.
  OperationsMask operationsPendingBackUp = static_cast<OperationsMask>(impl.mOperationsPending);

  // This is a hotfix for side effect on Scrolling, LineWrap and Invalid position of cursor in TextEditor after calling CalculateLayoutSizeOnRequiredControllerSize.
  // The number of lines and glyph-positions inside visualModel have been changed by calling DoRelayout with requestedControllerSize.
  // Store the mLines and mGlyphPositions from visualModel so that they can be restored later on with no modifications made on them.
  //TODO: Refactor "DoRelayout" and extract common code of size calculation without modifying attributes of mVisualModel, and then blah, blah, etc.
  Vector<LineRun> linesBackup          = visualModel->mLines;
  Vector<Vector2> glyphPositionsBackup = visualModel->mGlyphPositions;

  // Operations that can be done only once until the text changes.
  const OperationsMask onlyOnceOperations = static_cast<OperationsMask>(CONVERT_TO_UTF32 |
                                                                        GET_SCRIPTS |
                                                                        VALIDATE_FONTS |
                                                                        GET_LINE_BREAKS |
                                                                        BIDI_INFO |
                                                                        SHAPE_TEXT |
                                                                        GET_GLYPH_METRICS);

  // Set the update info to relayout the whole text.
  TextUpdateInfo& textUpdateInfo              = impl.mTextUpdateInfo;
  textUpdateInfo.mParagraphCharacterIndex     = 0u;
  textUpdateInfo.mRequestedNumberOfCharacters = model->mLogicalModel->mText.Count();

  // Make sure the model is up-to-date before layouting
  impl.UpdateModel(onlyOnceOperations);

  // Get a reference to the pending operations member
  OperationsMask& operationsPending = impl.mOperationsPending;

  // Layout the text for the new width.
  operationsPending = static_cast<OperationsMask>(operationsPending | requestedOperationsMask);

  // Store the actual control's size to restore later.
  const Size actualControlSize = visualModel->mControlSize;

  DoRelayout(controller,
             requestedControllerSize,
             static_cast<OperationsMask>(onlyOnceOperations |
                                         requestedOperationsMask),
             calculatedLayoutSize);

  // Clear the update info. This info will be set the next time the text is updated.
  textUpdateInfo.Clear();
  textUpdateInfo.mClearAll = true;

  // Restore the actual control's size.
  visualModel->mControlSize = actualControlSize;
  // Restore the previously backed-up pending operations' mask without the only once operations.
  impl.mOperationsPending = static_cast<OperationsMask>(operationsPendingBackUp & ~onlyOnceOperations);

  // Restore the previously backed-up mLines and mGlyphPositions from visualModel.
  if(restoreLinesAndGlyphPositions)
  {
    visualModel->mLines          = linesBackup;
    visualModel->mGlyphPositions = glyphPositionsBackup;
  }

  return calculatedLayoutSize;
}

Vector3 Controller::Relayouter::GetNaturalSize(Controller& controller)
{
  DALI_LOG_INFO(gLogFilter, Debug::Verbose, "-->Controller::GetNaturalSize\n");
  Vector3 naturalSizeVec3;

  // Make sure the model is up-to-date before layouting
  controller.ProcessModifyEvents();

  Controller::Impl& impl        = *controller.mImpl;
  ModelPtr&         model       = impl.mModel;
  VisualModelPtr&   visualModel = model->mVisualModel;

  if(impl.mRecalculateNaturalSize)
  {
    Size naturalSize;

    // Layout the text for the new width.
    OperationsMask requestedOperationsMask  = static_cast<OperationsMask>(LAYOUT | REORDER);
    Size           sizeMaxWidthAndMaxHeight = Size(MAX_FLOAT, MAX_FLOAT);

    naturalSize = CalculateLayoutSizeOnRequiredControllerSize(controller, sizeMaxWidthAndMaxHeight, requestedOperationsMask, true);

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

  return naturalSizeVec3;
}

bool Controller::Relayouter::CheckForTextFit(Controller& controller, float pointSize, const Size& layoutSize)
{
  Size              textSize;
  Controller::Impl& impl            = *controller.mImpl;
  TextUpdateInfo&   textUpdateInfo  = impl.mTextUpdateInfo;
  impl.mFontDefaults->mFitPointSize = pointSize;
  impl.mFontDefaults->sizeDefined   = true;
  controller.ClearFontData();

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

  DoRelayout(controller,
             Size(layoutSize.width, MAX_FLOAT),
             static_cast<OperationsMask>(onlyOnceOperations | LAYOUT),
             textSize);

  // Clear the update info. This info will be set the next time the text is updated.
  textUpdateInfo.Clear();
  textUpdateInfo.mClearAll = true;

  if(textSize.width > layoutSize.width || textSize.height > layoutSize.height)
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
    ModelPtr& model = impl.mModel;

    bool  actualellipsis = model->mElideEnabled;
    float minPointSize   = impl.mTextFitMinSize;
    float maxPointSize   = impl.mTextFitMaxSize;
    float pointInterval  = impl.mTextFitStepSize;

    model->mElideEnabled = false;
    Vector<float> pointSizeArray;

    // check zero value
    if(pointInterval < 1.f)
    {
      impl.mTextFitStepSize = pointInterval = 1.0f;
    }

    pointSizeArray.Reserve(static_cast<unsigned int>(ceil((maxPointSize - minPointSize) / pointInterval)));

    for(float i = minPointSize; i < maxPointSize; i += pointInterval)
    {
      pointSizeArray.PushBack(i);
    }

    pointSizeArray.PushBack(maxPointSize);

    int bestSizeIndex = 0;
    int min           = bestSizeIndex + 1;
    int max           = pointSizeArray.Size() - 1;
    while(min <= max)
    {
      int destI = (min + max) / 2;

      if(CheckForTextFit(controller, pointSizeArray[destI], layoutSize))
      {
        bestSizeIndex = min;
        min           = destI + 1;
      }
      else
      {
        max           = destI - 1;
        bestSizeIndex = max;
      }
    }

    model->mElideEnabled              = actualellipsis;
    impl.mFontDefaults->mFitPointSize = pointSizeArray[bestSizeIndex];
    impl.mFontDefaults->sizeDefined   = true;
    controller.ClearFontData();
  }
}

float Controller::Relayouter::GetHeightForWidth(Controller& controller, float width)
{
  DALI_LOG_INFO(gLogFilter, Debug::Verbose, "-->Controller::GetHeightForWidth %p width %f\n", &controller, width);

  // Make sure the model is up-to-date before layouting
  controller.ProcessModifyEvents();

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

    // Skip restore, because if GetHeightForWidth called before rendering and layouting then visualModel->mControlSize will be zero which will make LineCount zero.
    // The implementation of Get LineCount property depends on calling GetHeightForWidth then read mLines.Count() from visualModel direct.
    // If the LineCount property is requested before rendering and layouting then the value will be zero, which is incorrect.
    // So we will not restore the previously backed-up mLines and mGlyphPositions from visualModel in such case.
    // Another case to skip restore is when the requested width equals the Control's width which means the caller need to update the old values.
    // For example, when the text is changed.
    bool restoreLinesAndGlyphPositions = (visualModel->mControlSize.width > 0 && visualModel->mControlSize.height > 0) && (visualModel->mControlSize.width != width);

    layoutSize = CalculateLayoutSizeOnRequiredControllerSize(controller, sizeRequestedWidthAndMaxHeight, requestedOperationsMask, restoreLinesAndGlyphPositions);

    DALI_LOG_INFO(gLogFilter, Debug::Verbose, "<--Controller::GetHeightForWidth calculated %f\n", layoutSize.height);
  }
  else
  {
    layoutSize = visualModel->GetLayoutSize();
    DALI_LOG_INFO(gLogFilter, Debug::Verbose, "<--Controller::GetHeightForWidth cached %f\n", layoutSize.height);
  }

  return layoutSize.height;
}

Controller::UpdateTextType Controller::Relayouter::Relayout(Controller& controller, const Size& size, Dali::LayoutDirection::Type layoutDirection)
{
  Controller::Impl& impl           = *controller.mImpl;
  ModelPtr&         model          = impl.mModel;
  VisualModelPtr&   visualModel    = model->mVisualModel;
  TextUpdateInfo&   textUpdateInfo = impl.mTextUpdateInfo;

  DALI_LOG_INFO(gLogFilter, Debug::Verbose, "-->Controller::Relayout %p size %f,%f, autoScroll[%s]\n", &controller, size.width, size.height, impl.mIsAutoScrollEnabled ? "true" : "false");

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

  if(impl.mLayoutDirection != layoutDirection)
  {
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
  controller.ProcessModifyEvents();
  bool updated = impl.UpdateModel(operationsPending);

  // Layout the text.
  Size layoutSize;
  updated = DoRelayout(controller, size, operationsPending, layoutSize) || updated;

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
    controller.CalculateVerticalOffset(size);
  }

  if(isEditable)
  {
    if(newSize)
    {
      // If there is a new size, the scroll position needs to be clamped.
      impl.ClampHorizontalScroll(layoutSize);

      // Update the decorator's positions is needed if there is a new size.
      impl.mEventData->mDecorator->UpdatePositions(model->mScrollPosition - offset);
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

  return updateTextType;
}

bool Controller::Relayouter::DoRelayout(Controller& controller, const Size& size, OperationsMask operationsRequired, Size& layoutSize)
{
  DALI_LOG_INFO(gLogFilter, Debug::Verbose, "-->Controller::DoRelayout %p size %f,%f\n", &controller, size.width, size.height);
  bool viewUpdated(false);

  Controller::Impl& impl = *controller.mImpl;

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
      controller.GetText(currentText);

      DALI_LOG_ERROR("Controller::DoRelayout: Attempting to access invalid buffer\n");
      DALI_LOG_ERROR("Current text is: %s\n", currentText.c_str());
      DALI_LOG_ERROR("startIndex: %u, lastIndex: %u, requestedNumberOfCharacters: %u, charactersToGlyph.Count = %lu, glyphsPerCharacter.Count = %lu\n", startIndex, lastIndex, requestedNumberOfCharacters, charactersToGlyph.Count(), glyphsPerCharacter.Count());

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
        controller.ResetScrollPosition();
      }
    }

    // Update the visual model.
    bool isAutoScrollEnabled = impl.mIsAutoScrollEnabled;
    Size newLayoutSize;
    viewUpdated               = impl.mLayoutEngine.LayoutText(layoutParameters,
                                                newLayoutSize,
                                                elideTextEnabled,
                                                isAutoScrollEnabled);
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

    viewUpdated = true;
  }
#if defined(DEBUG_ENABLED)
  std::string currentText;
  controller.GetText(currentText);
  DALI_LOG_INFO(gLogFilter, Debug::Concise, "Controller::DoRelayout [%p] mImpl->mIsTextDirectionRTL[%s] [%s]\n", &controller, (impl.mIsTextDirectionRTL) ? "true" : "false", currentText.c_str());
#endif
  DALI_LOG_INFO(gLogFilter, Debug::Verbose, "<--Controller::DoRelayout, view updated %s\n", (viewUpdated ? "true" : "false"));
  return viewUpdated;
}

void Controller::Relayouter::CalculateVerticalOffset(Controller& controller, const Size& controlSize)
{
  Controller::Impl& impl                  = *controller.mImpl;
  ModelPtr&         model                 = impl.mModel;
  VisualModelPtr&   visualModel           = model->mVisualModel;
  Size              layoutSize            = model->mVisualModel->GetLayoutSize();
  Size              oldLayoutSize         = layoutSize;
  float             offsetY               = 0.f;
  bool              needRecalc            = false;
  float             defaultFontLineHeight = impl.GetDefaultFontLineHeight();

  if(fabsf(layoutSize.height) < Math::MACHINE_EPSILON_1000)
  {
    // Get the line height of the default font.
    layoutSize.height = defaultFontLineHeight;
  }

  // Whether the text control is editable
  const bool isEditable = NULL != impl.mEventData;
  if(isEditable && layoutSize.height != defaultFontLineHeight && impl.IsShowingPlaceholderText())
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
