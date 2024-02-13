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
#include <dali-toolkit/internal/text/controller/text-controller-input-font-handler.h>

// EXTERNAL INCLUDES
#include <memory.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/text/controller/text-controller-impl.h>

namespace Dali
{
namespace Toolkit
{
namespace Text
{
namespace
{
/**
 * @brief Adds a new font description run for the selected text.
 *
 * The new font parameters are added after the call to this method.
 *
 * @param[in] eventData The event data pointer.
 * @param[in] logicalModel The logical model where to add the new font description run.
 * @param[out] startOfSelectedText Index to the first selected character.
 * @param[out] lengthOfSelectedText Number of selected characters.
 */
FontDescriptionRun& UpdateSelectionFontStyleRun(EventData*      eventData,
                                                LogicalModelPtr logicalModel,
                                                CharacterIndex& startOfSelectedText,
                                                Length&         lengthOfSelectedText)
{
  const bool handlesCrossed = eventData->mLeftSelectionPosition > eventData->mRightSelectionPosition;

  // Get start and end position of selection
  startOfSelectedText  = handlesCrossed ? eventData->mRightSelectionPosition : eventData->mLeftSelectionPosition;
  lengthOfSelectedText = (handlesCrossed ? eventData->mLeftSelectionPosition : eventData->mRightSelectionPosition) - startOfSelectedText;

  // Add the font run.
  const VectorBase::SizeType numberOfRuns = logicalModel->mFontDescriptionRuns.Count();
  logicalModel->mFontDescriptionRuns.Resize(numberOfRuns + 1u);

  FontDescriptionRun& fontDescriptionRun = *(logicalModel->mFontDescriptionRuns.Begin() + numberOfRuns);

  fontDescriptionRun.characterRun.characterIndex     = startOfSelectedText;
  fontDescriptionRun.characterRun.numberOfCharacters = lengthOfSelectedText;

  // Recalculate the selection highlight as the metrics may have changed.
  eventData->mUpdateLeftSelectionPosition  = true;
  eventData->mUpdateRightSelectionPosition = true;
  eventData->mUpdateHighlightBox           = true;

  return fontDescriptionRun;
}

} // unnamed namespace

void Controller::InputFontHandler::SetInputFontFamily(Controller& controller, const std::string& fontFamily)
{
  if(NULL != controller.mImpl->mEventData)
  {
    controller.mImpl->mEventData->mInputStyle.familyName      = fontFamily;
    controller.mImpl->mEventData->mInputStyle.isFamilyDefined = true;

    if(EventData::SELECTING == controller.mImpl->mEventData->mState || EventData::EDITING == controller.mImpl->mEventData->mState || EventData::INACTIVE == controller.mImpl->mEventData->mState)
    {
      CharacterIndex startOfSelectedText  = 0u;
      Length         lengthOfSelectedText = 0u;

      if(EventData::SELECTING == controller.mImpl->mEventData->mState)
      {
        // Update a font description run for the selecting state.
        FontDescriptionRun& fontDescriptionRun = UpdateSelectionFontStyleRun(controller.mImpl->mEventData,
                                                                             controller.mImpl->mModel->mLogicalModel,
                                                                             startOfSelectedText,
                                                                             lengthOfSelectedText);

        fontDescriptionRun.familyLength = fontFamily.size();
        fontDescriptionRun.familyName   = new char[fontDescriptionRun.familyLength];
        memcpy(fontDescriptionRun.familyName, fontFamily.c_str(), fontDescriptionRun.familyLength);
        fontDescriptionRun.familyDefined = true;

        // The memory allocated for the font family name is freed when the font description is removed from the logical model.

        controller.mImpl->mTextUpdateInfo.mCharacterIndex             = startOfSelectedText;
        controller.mImpl->mTextUpdateInfo.mNumberOfCharactersToRemove = lengthOfSelectedText;
        controller.mImpl->mTextUpdateInfo.mNumberOfCharactersToAdd    = lengthOfSelectedText;
      }
      else
      {
        controller.mImpl->mTextUpdateInfo.mCharacterIndex             = 0;
        controller.mImpl->mTextUpdateInfo.mNumberOfCharactersToRemove = controller.mImpl->mTextUpdateInfo.mPreviousNumberOfCharacters;
        controller.mImpl->mTextUpdateInfo.mNumberOfCharactersToAdd    = controller.mImpl->mModel->mLogicalModel->mText.Count();
      }

      // Request to relayout.
      controller.mImpl->mOperationsPending      = static_cast<OperationsMask>(controller.mImpl->mOperationsPending |
                                                                         VALIDATE_FONTS |
                                                                         SHAPE_TEXT |
                                                                         GET_GLYPH_METRICS |
                                                                         LAYOUT |
                                                                         UPDATE_LAYOUT_SIZE |
                                                                         REORDER |
                                                                         ALIGN);
      controller.mImpl->mRecalculateNaturalSize = true;
      controller.mImpl->RequestRelayout();

      // As the font changes, recalculate the handle positions is needed.
      controller.mImpl->mEventData->mUpdateLeftSelectionPosition  = true;
      controller.mImpl->mEventData->mUpdateRightSelectionPosition = true;
      controller.mImpl->mEventData->mUpdateHighlightBox           = true;
      controller.mImpl->mEventData->mScrollAfterUpdatePosition    = true;
    }
  }
}

std::string Controller::InputFontHandler::GetInputFontFamily(const Controller& controller)
{
  if(NULL != controller.mImpl->mEventData)
  {
    return controller.mImpl->mEventData->mInputStyle.familyName;
  }

  // Return the default font's family if there is no EventData.
  return controller.GetDefaultFontFamily();
}

void Controller::InputFontHandler::SetInputFontWeight(const Controller& controller, FontWeight weight)
{
  if(NULL != controller.mImpl->mEventData)
  {
    controller.mImpl->mEventData->mInputStyle.weight          = weight;
    controller.mImpl->mEventData->mInputStyle.isWeightDefined = true;

    if(EventData::SELECTING == controller.mImpl->mEventData->mState || EventData::EDITING == controller.mImpl->mEventData->mState || EventData::INACTIVE == controller.mImpl->mEventData->mState)
    {
      CharacterIndex startOfSelectedText  = 0u;
      Length         lengthOfSelectedText = 0u;

      if(EventData::SELECTING == controller.mImpl->mEventData->mState)
      {
        // Update a font description run for the selecting state.
        FontDescriptionRun& fontDescriptionRun = UpdateSelectionFontStyleRun(controller.mImpl->mEventData,
                                                                             controller.mImpl->mModel->mLogicalModel,
                                                                             startOfSelectedText,
                                                                             lengthOfSelectedText);

        fontDescriptionRun.weight        = weight;
        fontDescriptionRun.weightDefined = true;

        controller.mImpl->mTextUpdateInfo.mCharacterIndex             = startOfSelectedText;
        controller.mImpl->mTextUpdateInfo.mNumberOfCharactersToRemove = lengthOfSelectedText;
        controller.mImpl->mTextUpdateInfo.mNumberOfCharactersToAdd    = lengthOfSelectedText;
      }
      else
      {
        controller.mImpl->mTextUpdateInfo.mCharacterIndex             = 0;
        controller.mImpl->mTextUpdateInfo.mNumberOfCharactersToRemove = controller.mImpl->mTextUpdateInfo.mPreviousNumberOfCharacters;
        controller.mImpl->mTextUpdateInfo.mNumberOfCharactersToAdd    = controller.mImpl->mModel->mLogicalModel->mText.Count();
      }

      // Request to relayout.
      controller.mImpl->mOperationsPending      = static_cast<OperationsMask>(controller.mImpl->mOperationsPending |
                                                                         VALIDATE_FONTS |
                                                                         SHAPE_TEXT |
                                                                         GET_GLYPH_METRICS |
                                                                         LAYOUT |
                                                                         UPDATE_LAYOUT_SIZE |
                                                                         REORDER |
                                                                         ALIGN);
      controller.mImpl->mRecalculateNaturalSize = true;
      controller.mImpl->RequestRelayout();

      // As the font might change, recalculate the handle positions is needed.
      controller.mImpl->mEventData->mUpdateLeftSelectionPosition  = true;
      controller.mImpl->mEventData->mUpdateRightSelectionPosition = true;
      controller.mImpl->mEventData->mUpdateHighlightBox           = true;
      controller.mImpl->mEventData->mScrollAfterUpdatePosition    = true;
    }
  }
}

bool Controller::InputFontHandler::IsInputFontWeightDefined(const Controller& controller)
{
  bool defined = false;

  if(NULL != controller.mImpl->mEventData)
  {
    defined = controller.mImpl->mEventData->mInputStyle.isWeightDefined;
  }

  return defined;
}

FontWeight Controller::InputFontHandler::GetInputFontWeight(const Controller& controller)
{
  if(NULL != controller.mImpl->mEventData)
  {
    return controller.mImpl->mEventData->mInputStyle.weight;
  }

  return controller.GetDefaultFontWeight();
}

void Controller::InputFontHandler::SetInputFontWidth(Controller& controller, FontWidth width)
{
  if(NULL != controller.mImpl->mEventData)
  {
    controller.mImpl->mEventData->mInputStyle.width          = width;
    controller.mImpl->mEventData->mInputStyle.isWidthDefined = true;

    if(EventData::SELECTING == controller.mImpl->mEventData->mState || EventData::EDITING == controller.mImpl->mEventData->mState || EventData::INACTIVE == controller.mImpl->mEventData->mState)
    {
      CharacterIndex startOfSelectedText  = 0u;
      Length         lengthOfSelectedText = 0u;

      if(EventData::SELECTING == controller.mImpl->mEventData->mState)
      {
        // Update a font description run for the selecting state.
        FontDescriptionRun& fontDescriptionRun = UpdateSelectionFontStyleRun(controller.mImpl->mEventData,
                                                                             controller.mImpl->mModel->mLogicalModel,
                                                                             startOfSelectedText,
                                                                             lengthOfSelectedText);

        fontDescriptionRun.width        = width;
        fontDescriptionRun.widthDefined = true;

        controller.mImpl->mTextUpdateInfo.mCharacterIndex             = startOfSelectedText;
        controller.mImpl->mTextUpdateInfo.mNumberOfCharactersToRemove = lengthOfSelectedText;
        controller.mImpl->mTextUpdateInfo.mNumberOfCharactersToAdd    = lengthOfSelectedText;
      }
      else
      {
        controller.mImpl->mTextUpdateInfo.mCharacterIndex             = 0;
        controller.mImpl->mTextUpdateInfo.mNumberOfCharactersToRemove = controller.mImpl->mTextUpdateInfo.mPreviousNumberOfCharacters;
        controller.mImpl->mTextUpdateInfo.mNumberOfCharactersToAdd    = controller.mImpl->mModel->mLogicalModel->mText.Count();
      }

      // Request to relayout.
      controller.mImpl->mOperationsPending      = static_cast<OperationsMask>(controller.mImpl->mOperationsPending |
                                                                         VALIDATE_FONTS |
                                                                         SHAPE_TEXT |
                                                                         GET_GLYPH_METRICS |
                                                                         LAYOUT |
                                                                         UPDATE_LAYOUT_SIZE |
                                                                         REORDER |
                                                                         ALIGN);
      controller.mImpl->mRecalculateNaturalSize = true;
      controller.mImpl->RequestRelayout();

      // As the font might change, recalculate the handle positions is needed.
      controller.mImpl->mEventData->mUpdateLeftSelectionPosition  = true;
      controller.mImpl->mEventData->mUpdateRightSelectionPosition = true;
      controller.mImpl->mEventData->mUpdateHighlightBox           = true;
      controller.mImpl->mEventData->mScrollAfterUpdatePosition    = true;
    }
  }
}

bool Controller::InputFontHandler::IsInputFontWidthDefined(const Controller& controller)
{
  bool defined = false;

  if(NULL != controller.mImpl->mEventData)
  {
    defined = controller.mImpl->mEventData->mInputStyle.isWidthDefined;
  }

  return defined;
}

FontWidth Controller::InputFontHandler::GetInputFontWidth(const Controller& controller)
{
  if(NULL != controller.mImpl->mEventData)
  {
    return controller.mImpl->mEventData->mInputStyle.width;
  }

  return controller.GetDefaultFontWidth();
}

void Controller::InputFontHandler::SetInputFontSlant(Controller& controller, FontSlant slant)
{
  if(NULL != controller.mImpl->mEventData)
  {
    controller.mImpl->mEventData->mInputStyle.slant          = slant;
    controller.mImpl->mEventData->mInputStyle.isSlantDefined = true;

    if(EventData::SELECTING == controller.mImpl->mEventData->mState || EventData::EDITING == controller.mImpl->mEventData->mState || EventData::INACTIVE == controller.mImpl->mEventData->mState)
    {
      CharacterIndex startOfSelectedText  = 0u;
      Length         lengthOfSelectedText = 0u;

      if(EventData::SELECTING == controller.mImpl->mEventData->mState)
      {
        // Update a font description run for the selecting state.
        FontDescriptionRun& fontDescriptionRun = UpdateSelectionFontStyleRun(controller.mImpl->mEventData,
                                                                             controller.mImpl->mModel->mLogicalModel,
                                                                             startOfSelectedText,
                                                                             lengthOfSelectedText);

        fontDescriptionRun.slant        = slant;
        fontDescriptionRun.slantDefined = true;

        controller.mImpl->mTextUpdateInfo.mCharacterIndex             = startOfSelectedText;
        controller.mImpl->mTextUpdateInfo.mNumberOfCharactersToRemove = lengthOfSelectedText;
        controller.mImpl->mTextUpdateInfo.mNumberOfCharactersToAdd    = lengthOfSelectedText;
      }
      else
      {
        controller.mImpl->mTextUpdateInfo.mCharacterIndex             = 0;
        controller.mImpl->mTextUpdateInfo.mNumberOfCharactersToRemove = controller.mImpl->mTextUpdateInfo.mPreviousNumberOfCharacters;
        controller.mImpl->mTextUpdateInfo.mNumberOfCharactersToAdd    = controller.mImpl->mModel->mLogicalModel->mText.Count();
      }

      // Request to relayout.
      controller.mImpl->mOperationsPending      = static_cast<OperationsMask>(controller.mImpl->mOperationsPending |
                                                                         VALIDATE_FONTS |
                                                                         SHAPE_TEXT |
                                                                         GET_GLYPH_METRICS |
                                                                         LAYOUT |
                                                                         UPDATE_LAYOUT_SIZE |
                                                                         REORDER |
                                                                         ALIGN);
      controller.mImpl->mRecalculateNaturalSize = true;
      controller.mImpl->RequestRelayout();

      // As the font might change, recalculate the handle positions is needed.
      controller.mImpl->mEventData->mUpdateLeftSelectionPosition  = true;
      controller.mImpl->mEventData->mUpdateRightSelectionPosition = true;
      controller.mImpl->mEventData->mUpdateHighlightBox           = true;
      controller.mImpl->mEventData->mScrollAfterUpdatePosition    = true;
    }
  }
}

bool Controller::InputFontHandler::IsInputFontSlantDefined(const Controller& controller)
{
  bool defined = false;

  if(NULL != controller.mImpl->mEventData)
  {
    defined = controller.mImpl->mEventData->mInputStyle.isSlantDefined;
  }

  return defined;
}

FontSlant Controller::InputFontHandler::GetInputFontSlant(const Controller& controller)
{
  if(NULL != controller.mImpl->mEventData)
  {
    return controller.mImpl->mEventData->mInputStyle.slant;
  }

  return controller.GetDefaultFontSlant();
}

void Controller::InputFontHandler::SetInputFontPointSize(Controller& controller, float size, bool defaultFontSizeUpdated)
{
  if(NULL != controller.mImpl->mEventData)
  {
    controller.mImpl->mEventData->mInputStyle.size          = size;
    controller.mImpl->mEventData->mInputStyle.isSizeDefined = true;

    if(EventData::SELECTING == controller.mImpl->mEventData->mState || EventData::EDITING == controller.mImpl->mEventData->mState || EventData::INACTIVE == controller.mImpl->mEventData->mState)
    {
      CharacterIndex startOfSelectedText  = 0u;
      Length         lengthOfSelectedText = 0u;

      if(EventData::SELECTING == controller.mImpl->mEventData->mState && !defaultFontSizeUpdated)
      {
        // Update a font description run for the selecting state.
        FontDescriptionRun& fontDescriptionRun = UpdateSelectionFontStyleRun(controller.mImpl->mEventData,
                                                                             controller.mImpl->mModel->mLogicalModel,
                                                                             startOfSelectedText,
                                                                             lengthOfSelectedText);

        fontDescriptionRun.size        = static_cast<PointSize26Dot6>(size * controller.mImpl->GetFontSizeScale() * 64.f);
        fontDescriptionRun.sizeDefined = true;

        controller.mImpl->mTextUpdateInfo.mCharacterIndex             = startOfSelectedText;
        controller.mImpl->mTextUpdateInfo.mNumberOfCharactersToRemove = lengthOfSelectedText;
        controller.mImpl->mTextUpdateInfo.mNumberOfCharactersToAdd    = lengthOfSelectedText;
      }
      else
      {
        controller.mImpl->mTextUpdateInfo.mCharacterIndex             = 0;
        controller.mImpl->mTextUpdateInfo.mNumberOfCharactersToRemove = controller.mImpl->mTextUpdateInfo.mPreviousNumberOfCharacters;
        controller.mImpl->mTextUpdateInfo.mNumberOfCharactersToAdd    = controller.mImpl->mModel->mLogicalModel->mText.Count();
      }

      // Request to relayout.
      controller.mImpl->mOperationsPending      = static_cast<OperationsMask>(controller.mImpl->mOperationsPending |
                                                                         VALIDATE_FONTS |
                                                                         SHAPE_TEXT |
                                                                         GET_GLYPH_METRICS |
                                                                         LAYOUT |
                                                                         UPDATE_LAYOUT_SIZE |
                                                                         REORDER |
                                                                         ALIGN);
      controller.mImpl->mRecalculateNaturalSize = true;
      controller.mImpl->RequestRelayout();

      // As the font might change, recalculate the handle positions is needed.
      controller.mImpl->mEventData->mUpdateLeftSelectionPosition  = true;
      controller.mImpl->mEventData->mUpdateRightSelectionPosition = true;
      controller.mImpl->mEventData->mUpdateHighlightBox           = true;
      controller.mImpl->mEventData->mScrollAfterUpdatePosition    = true;
    }
  }
}

float Controller::InputFontHandler::GetInputFontPointSize(const Controller& controller)
{
  if(NULL != controller.mImpl->mEventData)
  {
    return controller.mImpl->mEventData->mInputStyle.size;
  }

  // Return the default font's point size if there is no EventData.
  return controller.GetDefaultFontSize(Text::Controller::POINT_SIZE);
}
} // namespace Text

} // namespace Toolkit

} // namespace Dali
