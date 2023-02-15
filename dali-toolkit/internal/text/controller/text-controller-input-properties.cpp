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
#include <dali-toolkit/internal/text/controller/text-controller-input-properties.h>

// EXTERNAL INCLUDES
//#include <dali-toolkit/devel-api/controls/control-depth-index-ranges.h>
//#include <dali/devel-api/adaptor-framework/window-devel.h>
//#include <dali/integration-api/debug.h>
#include <memory.h>
#include <cmath>
#include <limits>

// INTERNAL INCLUDES
#include <dali-toolkit/devel-api/text/text-enumerations-devel.h>
#include <dali-toolkit/internal/text/controller/text-controller-event-handler.h>
#include <dali-toolkit/internal/text/controller/text-controller-impl.h>
#include <dali-toolkit/internal/text/controller/text-controller-input-font-handler.h>
#include <dali-toolkit/internal/text/controller/text-controller-placeholder-handler.h>
#include <dali-toolkit/internal/text/controller/text-controller-relayouter.h>
#include <dali-toolkit/internal/text/controller/text-controller-text-updater.h>
#include <dali-toolkit/internal/text/text-editable-control-interface.h>

namespace
{
const char* EMPTY_STRING = "";
}

namespace Dali::Toolkit::Text
{

void Controller::InputProperties::SetInputColor(Controller& controller, const Vector4& color)
{
  if(controller.mImpl->mEventData)
  {
    controller.mImpl->mEventData->mInputStyle.textColor      = color;
    controller.mImpl->mEventData->mInputStyle.isDefaultColor = false;

    if(EventData::SELECTING == controller.mImpl->mEventData->mState || EventData::EDITING == controller.mImpl->mEventData->mState || EventData::INACTIVE == controller.mImpl->mEventData->mState)
    {
      if(EventData::SELECTING == controller.mImpl->mEventData->mState)
      {
        const bool handlesCrossed = controller.mImpl->mEventData->mLeftSelectionPosition > controller.mImpl->mEventData->mRightSelectionPosition;

        // Get start and end position of selection
        const CharacterIndex startOfSelectedText  = handlesCrossed ? controller.mImpl->mEventData->mRightSelectionPosition : controller.mImpl->mEventData->mLeftSelectionPosition;
        const Length         lengthOfSelectedText = (handlesCrossed ? controller.mImpl->mEventData->mLeftSelectionPosition : controller.mImpl->mEventData->mRightSelectionPosition) - startOfSelectedText;

        // Add the color run.
        const VectorBase::SizeType numberOfRuns = controller.mImpl->mModel->mLogicalModel->mColorRuns.Count();
        controller.mImpl->mModel->mLogicalModel->mColorRuns.Resize(numberOfRuns + 1u);

        ColorRun& colorRun                       = *(controller.mImpl->mModel->mLogicalModel->mColorRuns.Begin() + numberOfRuns);
        colorRun.color                           = color;
        colorRun.characterRun.characterIndex     = startOfSelectedText;
        colorRun.characterRun.numberOfCharacters = lengthOfSelectedText;

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
      controller.mImpl->mOperationsPending = static_cast<OperationsMask>(controller.mImpl->mOperationsPending | COLOR);
      controller.mImpl->RequestRelayout();
    }
  }
}

const Vector4& Controller::InputProperties::GetInputColor(const Controller& controller)
{
  // Return event text input color if we have it, otherwise just return the default text's color
  return controller.mImpl->mEventData ? controller.mImpl->mEventData->mInputStyle.textColor : controller.mImpl->mTextColor;
}

void Controller::InputProperties::SetInputLineSpacing(Controller& controller, float lineSpacing)
{
  if(controller.mImpl->mEventData)
  {
    controller.mImpl->mEventData->mInputStyle.lineSpacing          = lineSpacing;
    controller.mImpl->mEventData->mInputStyle.isLineSpacingDefined = true;
  }
}

float Controller::InputProperties::GetInputLineSpacing(const Controller& controller)
{
  return controller.mImpl->mEventData ? controller.mImpl->mEventData->mInputStyle.lineSpacing : 0.0f;
}

void Controller::InputProperties::SetInputShadowProperties(Controller& controller, const std::string& shadowProperties)
{
  if(controller.mImpl->mEventData)
  {
    controller.mImpl->mEventData->mInputStyle.shadowProperties = shadowProperties;
  }
}

std::string Controller::InputProperties::GetInputShadowProperties(const Controller& controller)
{
  return controller.mImpl->mEventData ? controller.mImpl->mEventData->mInputStyle.shadowProperties : EMPTY_STRING;
}

void Controller::InputProperties::SetInputUnderlineProperties(Controller& controller, const std::string& underlineProperties)
{
  if(controller.mImpl->mEventData)
  {
    controller.mImpl->mEventData->mInputStyle.underlineProperties = underlineProperties;
  }
}

std::string Controller::InputProperties::GetInputUnderlineProperties(const Controller& controller)
{
  return controller.mImpl->mEventData ? controller.mImpl->mEventData->mInputStyle.underlineProperties : EMPTY_STRING;
}

void Controller::InputProperties::SetInputEmbossProperties(Controller& controller, const std::string& embossProperties)
{
  if(controller.mImpl->mEventData)
  {
    controller.mImpl->mEventData->mInputStyle.embossProperties = embossProperties;
  }
}

std::string Controller::InputProperties::GetInputEmbossProperties(const Controller& controller)
{
  return controller.mImpl->mEventData ? controller.mImpl->mEventData->mInputStyle.embossProperties : controller.GetDefaultEmbossProperties();
}

void Controller::InputProperties::SetInputOutlineProperties(Controller& controller, const std::string& outlineProperties)
{
  if(controller.mImpl->mEventData)
  {
    controller.mImpl->mEventData->mInputStyle.outlineProperties = outlineProperties;
  }
}

std::string Controller::InputProperties::GetInputOutlineProperties(const Controller& controller)
{
  return controller.mImpl->mEventData ? controller.mImpl->mEventData->mInputStyle.outlineProperties : controller.GetDefaultOutlineProperties();
}

void Controller::InputProperties::SetInputModePassword(Controller& controller, bool passwordInput)
{
  if(controller.mImpl->mEventData)
  {
    controller.mImpl->mEventData->mPasswordInput = passwordInput;
  }
}

bool Controller::InputProperties::IsInputModePassword(Controller& controller)
{
  return controller.mImpl->mEventData && controller.mImpl->mEventData->mPasswordInput;
}

} // namespace Dali::Toolkit::Text
