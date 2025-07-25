/*
 * Copyright (c) 2025 Samsung Electronics Co., Ltd.
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
#include <dali-toolkit/internal/controls/text-controls/text-label-impl.h>

// EXTERNAL INCLUDES
#include <dali/devel-api/actors/actor-devel.h>
#include <dali/devel-api/adaptor-framework/image-loading.h>
#include <dali/devel-api/common/stage.h>
#include <dali/devel-api/object/property-helper-devel.h>
#include <dali/integration-api/adaptor-framework/adaptor.h>
#include <dali/integration-api/debug.h>
#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/object/type-registry-helper.h>

// INTERNAL INCLUDES
#include <dali-toolkit/devel-api/controls/control-depth-index-ranges.h>
#include <dali-toolkit/devel-api/text/rendering-backend.h>
#include <dali-toolkit/internal/controls/render-effects/mask-effect-impl.h>
#include <dali-toolkit/internal/controls/text-controls/common-text-utils.h>
#include <dali-toolkit/internal/styling/style-manager-impl.h>
#include <dali-toolkit/internal/text/property-string-parser.h>
#include <dali-toolkit/internal/text/rendering/text-backend.h>
#include <dali-toolkit/internal/text/text-definitions.h>
#include <dali-toolkit/internal/text/text-effects-style.h>
#include <dali-toolkit/internal/text/text-font-style.h>
#include <dali-toolkit/internal/text/text-view.h>
#include <dali-toolkit/public-api/text/text-enumerations.h>

#include <dali-toolkit/devel-api/controls/control-devel.h>
#include <dali-toolkit/devel-api/visual-factory/visual-base.h>
#include <dali-toolkit/devel-api/visual-factory/visual-factory.h>
#include <dali-toolkit/internal/text/text-enumerations-impl.h>
#include <dali-toolkit/public-api/align-enumerations.h>
#include <dali-toolkit/public-api/controls/render-effects/mask-effect.h>
#include <dali-toolkit/public-api/visuals/color-visual-properties.h>
#include <dali-toolkit/public-api/visuals/text-visual-properties.h>
#include <dali-toolkit/public-api/visuals/visual-properties.h>

// DEVEL INCLUDES
#include <dali-toolkit/devel-api/controls/text-controls/text-label-devel.h>

using namespace Dali::Toolkit::Text;

namespace Dali
{
namespace Toolkit
{
namespace Internal
{
namespace
{
static constexpr uint32_t NUMBER_OF_RENDER_MODE = 3;

const unsigned int DEFAULT_RENDERING_BACKEND = Dali::Toolkit::DevelText::DEFAULT_RENDERING_BACKEND;

/**
 * @brief How the text visual should be aligned vertically inside the control.
 *
 * 0.0f aligns the text to the top, 0.5f aligns the text to the center, 1.0f aligns the text to the bottom.
 * The alignment depends on the alignment value of the text label (Use Text::VerticalAlignment enumerations).
 */
const float VERTICAL_ALIGNMENT_TABLE[Text::VerticalAlignment::BOTTOM + 1] =
  {
    0.0f, // VerticalAlignment::TOP
    0.5f, // VerticalAlignment::CENTER
    1.0f  // VerticalAlignment::BOTTOM
};

const char* TEXT_FIT_ENABLE_KEY("enable");
const char* TEXT_FIT_MIN_SIZE_KEY("minSize");
const char* TEXT_FIT_MAX_SIZE_KEY("maxSize");
const char* TEXT_FIT_STEP_SIZE_KEY("stepSize");
const char* TEXT_FIT_FONT_SIZE_KEY("fontSize");
const char* TEXT_FIT_FONT_SIZE_TYPE_KEY("fontSizeType");

#if defined(DEBUG_ENABLED)
Debug::Filter* gLogFilter = Debug::Filter::New(Debug::NoLogging, true, "LOG_TEXT_CONTROLS");
#endif

const Scripting::StringEnum AUTO_SCROLL_STOP_MODE_TABLE[] =
  {
    {"IMMEDIATE", Toolkit::TextLabel::AutoScrollStopMode::IMMEDIATE},
    {"FINISH_LOOP", Toolkit::TextLabel::AutoScrollStopMode::FINISH_LOOP},
};
const unsigned int AUTO_SCROLL_STOP_MODE_TABLE_COUNT = sizeof(AUTO_SCROLL_STOP_MODE_TABLE) / sizeof(AUTO_SCROLL_STOP_MODE_TABLE[0]);

// Type registration
BaseHandle Create()
{
  return Toolkit::TextLabel::New();
}

// clang-format off
// Setup properties, signals and actions using the type-registry.
DALI_TYPE_REGISTRATION_BEGIN(Toolkit::TextLabel, Toolkit::Control, Create);

DALI_PROPERTY_REGISTRATION(Toolkit,                 TextLabel, "text",                         STRING,  TEXT                           )
DALI_PROPERTY_REGISTRATION(Toolkit,                 TextLabel, "fontFamily",                   STRING,  FONT_FAMILY                    )
DALI_PROPERTY_REGISTRATION(Toolkit,                 TextLabel, "fontStyle",                    MAP,     FONT_STYLE                     )
DALI_PROPERTY_REGISTRATION(Toolkit,                 TextLabel, "pointSize",                    FLOAT,   POINT_SIZE                     )
DALI_PROPERTY_REGISTRATION(Toolkit,                 TextLabel, "multiLine",                    BOOLEAN, MULTI_LINE                     )
DALI_PROPERTY_REGISTRATION(Toolkit,                 TextLabel, "horizontalAlignment",          STRING,  HORIZONTAL_ALIGNMENT           )
DALI_PROPERTY_REGISTRATION(Toolkit,                 TextLabel, "verticalAlignment",            STRING,  VERTICAL_ALIGNMENT             )
DALI_PROPERTY_REGISTRATION(Toolkit,                 TextLabel, "enableMarkup",                 BOOLEAN, ENABLE_MARKUP                  )
DALI_PROPERTY_REGISTRATION(Toolkit,                 TextLabel, "enableAutoScroll",             BOOLEAN, ENABLE_AUTO_SCROLL             )
DALI_PROPERTY_REGISTRATION(Toolkit,                 TextLabel, "autoScrollSpeed",              INTEGER, AUTO_SCROLL_SPEED              )
DALI_PROPERTY_REGISTRATION(Toolkit,                 TextLabel, "autoScrollLoopCount",          INTEGER, AUTO_SCROLL_LOOP_COUNT         )
DALI_PROPERTY_REGISTRATION(Toolkit,                 TextLabel, "autoScrollGap",                FLOAT,   AUTO_SCROLL_GAP                )
DALI_PROPERTY_REGISTRATION(Toolkit,                 TextLabel, "lineSpacing",                  FLOAT,   LINE_SPACING                   )
DALI_PROPERTY_REGISTRATION(Toolkit,                 TextLabel, "underline",                    MAP,     UNDERLINE                      )
DALI_PROPERTY_REGISTRATION(Toolkit,                 TextLabel, "shadow",                       MAP,     SHADOW                         )
DALI_PROPERTY_REGISTRATION(Toolkit,                 TextLabel, "emboss",                       MAP,     EMBOSS                         )
DALI_PROPERTY_REGISTRATION(Toolkit,                 TextLabel, "outline",                      MAP,     OUTLINE                        )
DALI_PROPERTY_REGISTRATION(Toolkit,                 TextLabel, "pixelSize",                    FLOAT,   PIXEL_SIZE                     )
DALI_PROPERTY_REGISTRATION(Toolkit,                 TextLabel, "ellipsis",                     BOOLEAN, ELLIPSIS                       )
DALI_PROPERTY_REGISTRATION(Toolkit,                 TextLabel, "autoScrollLoopDelay",          FLOAT,   AUTO_SCROLL_LOOP_DELAY         )
DALI_PROPERTY_REGISTRATION(Toolkit,                 TextLabel, "autoScrollStopMode",           STRING,  AUTO_SCROLL_STOP_MODE          )
DALI_PROPERTY_REGISTRATION_READ_ONLY(Toolkit,       TextLabel, "lineCount",                    INTEGER, LINE_COUNT                     )
DALI_PROPERTY_REGISTRATION(Toolkit,                 TextLabel, "lineWrapMode",                 INTEGER, LINE_WRAP_MODE                 )
DALI_DEVEL_PROPERTY_REGISTRATION_READ_ONLY(Toolkit, TextLabel, "textDirection",                INTEGER, TEXT_DIRECTION                 )
DALI_DEVEL_PROPERTY_REGISTRATION(Toolkit,           TextLabel, "verticalLineAlignment",        INTEGER, VERTICAL_LINE_ALIGNMENT        )
DALI_DEVEL_PROPERTY_REGISTRATION(Toolkit,           TextLabel, "textBackground",               MAP,     BACKGROUND                     )
DALI_DEVEL_PROPERTY_REGISTRATION(Toolkit,           TextLabel, "ignoreSpacesAfterText",        BOOLEAN, IGNORE_SPACES_AFTER_TEXT       )
DALI_DEVEL_PROPERTY_REGISTRATION(Toolkit,           TextLabel, "matchSystemLanguageDirection", BOOLEAN, MATCH_SYSTEM_LANGUAGE_DIRECTION)
DALI_DEVEL_PROPERTY_REGISTRATION(Toolkit,           TextLabel, "textFit",                      MAP,     TEXT_FIT                       )
DALI_DEVEL_PROPERTY_REGISTRATION(Toolkit,           TextLabel, "minLineSize",                  FLOAT,   MIN_LINE_SIZE                  )
DALI_DEVEL_PROPERTY_REGISTRATION(Toolkit,           TextLabel, "renderingBackend",             INTEGER, RENDERING_BACKEND              )
DALI_DEVEL_PROPERTY_REGISTRATION(Toolkit,           TextLabel, "fontSizeScale",                FLOAT,   FONT_SIZE_SCALE                )
DALI_DEVEL_PROPERTY_REGISTRATION(Toolkit,           TextLabel, "enableFontSizeScale",          BOOLEAN, ENABLE_FONT_SIZE_SCALE         )
DALI_DEVEL_PROPERTY_REGISTRATION(Toolkit,           TextLabel, "ellipsisPosition",             INTEGER, ELLIPSIS_POSITION              )
DALI_DEVEL_PROPERTY_REGISTRATION(Toolkit,           TextLabel, "strikethrough",                MAP,     STRIKETHROUGH                  )
DALI_DEVEL_PROPERTY_REGISTRATION(Toolkit,           TextLabel, "characterSpacing",             FLOAT,   CHARACTER_SPACING              )
DALI_DEVEL_PROPERTY_REGISTRATION(Toolkit,           TextLabel, "relativeLineSize",             FLOAT,   RELATIVE_LINE_SIZE             )
DALI_DEVEL_PROPERTY_REGISTRATION(Toolkit,           TextLabel, "anchorColor",                  VECTOR4, ANCHOR_COLOR                   )
DALI_DEVEL_PROPERTY_REGISTRATION(Toolkit,           TextLabel, "anchorClickedColor",           VECTOR4, ANCHOR_CLICKED_COLOR           )
DALI_DEVEL_PROPERTY_REGISTRATION(Toolkit,           TextLabel, "removeFrontInset",             BOOLEAN, REMOVE_FRONT_INSET             )
DALI_DEVEL_PROPERTY_REGISTRATION(Toolkit,           TextLabel, "removeBackInset",              BOOLEAN, REMOVE_BACK_INSET              )
DALI_DEVEL_PROPERTY_REGISTRATION(Toolkit,           TextLabel, "cutout",                       BOOLEAN, CUTOUT                         )
DALI_DEVEL_PROPERTY_REGISTRATION(Toolkit,           TextLabel, "renderMode",                   INTEGER, RENDER_MODE                    )
DALI_DEVEL_PROPERTY_REGISTRATION_READ_ONLY(Toolkit, TextLabel, "manualRendered",               BOOLEAN, MANUAL_RENDERED                )
DALI_DEVEL_PROPERTY_REGISTRATION_READ_ONLY(Toolkit, TextLabel, "asyncLineCount",               INTEGER, ASYNC_LINE_COUNT               )
DALI_DEVEL_PROPERTY_REGISTRATION(Toolkit,           TextLabel, "ellipsisMode",                 INTEGER, ELLIPSIS_MODE                  )
DALI_DEVEL_PROPERTY_REGISTRATION_READ_ONLY(Toolkit, TextLabel, "isScrolling",                  BOOLEAN, IS_SCROLLING                   )
DALI_DEVEL_PROPERTY_REGISTRATION(Toolkit,           TextLabel, "fontVariations",               MAP,     FONT_VARIATIONS                )
DALI_DEVEL_PROPERTY_REGISTRATION(Toolkit,           TextLabel, "renderScale",                  FLOAT,   RENDER_SCALE                   )
DALI_DEVEL_PROPERTY_REGISTRATION_READ_ONLY(Toolkit, TextLabel, "needRequestAsyncRender",       BOOLEAN, NEED_REQUEST_ASYNC_RENDER      )
DALI_DEVEL_PROPERTY_REGISTRATION(Toolkit,           TextLabel, "layoutDirectionPolicy",        INTEGER, LAYOUT_DIRECTION_POLICY        )

DALI_ANIMATABLE_PROPERTY_REGISTRATION_WITH_DEFAULT(Toolkit, TextLabel, "textColor",       Color::BLACK,     TEXT_COLOR       )
DALI_ANIMATABLE_PROPERTY_COMPONENT_REGISTRATION(Toolkit,    TextLabel, "textColorRed",    TEXT_COLOR_RED,   TEXT_COLOR,     0)
DALI_ANIMATABLE_PROPERTY_COMPONENT_REGISTRATION(Toolkit,    TextLabel, "textColorGreen",  TEXT_COLOR_GREEN, TEXT_COLOR,     1)
DALI_ANIMATABLE_PROPERTY_COMPONENT_REGISTRATION(Toolkit,    TextLabel, "textColorBlue",   TEXT_COLOR_BLUE,  TEXT_COLOR,     2)
DALI_ANIMATABLE_PROPERTY_COMPONENT_REGISTRATION(Toolkit,    TextLabel, "textColorAlpha",  TEXT_COLOR_ALPHA, TEXT_COLOR,     3)
DALI_ANIMATABLE_PROPERTY_REGISTRATION(Toolkit,              TextLabel, "pixelSnapFactor", FLOAT,            PIXEL_SNAP_FACTOR)

DALI_SIGNAL_REGISTRATION(Toolkit, TextLabel, "anchorClicked",               SIGNAL_ANCHOR_CLICKED                 )
DALI_SIGNAL_REGISTRATION(Toolkit, TextLabel, "textFitChanged",              SIGNAL_TEXT_FIT_CHANGED               )
DALI_SIGNAL_REGISTRATION(Toolkit, TextLabel, "asyncTextRendered",           SIGNAL_ASYNC_TEXT_RENDERED            )
DALI_SIGNAL_REGISTRATION(Toolkit, TextLabel, "asyncNaturalSizeComputed",    SIGNAL_ASYNC_NATURAL_SIZE_COMPUTED    )
DALI_SIGNAL_REGISTRATION(Toolkit, TextLabel, "asyncHeightForWidthComputed", SIGNAL_ASYNC_HEIGHT_FOR_WIDTH_COMPUTED)

DALI_TYPE_REGISTRATION_END()
// clang-format on

/// Parses the property map for the TEXT_FIT property
void ParseTextFitProperty(Text::ControllerPtr& controller, const Property::Map* propertiesMap)
{
  if(propertiesMap && !propertiesMap->Empty())
  {
    bool                     enabled      = false;
    float                    minSize      = 0.f;
    float                    maxSize      = 0.f;
    float                    stepSize     = 0.f;
    bool                     isMinSizeSet = false, isMaxSizeSet = false, isStepSizeSet = false;
    Controller::FontSizeType type = Controller::FontSizeType::POINT_SIZE;

    const unsigned int numberOfItems = propertiesMap->Count();

    // Parses and applies
    for(unsigned int index = 0u; index < numberOfItems; ++index)
    {
      const KeyValuePair& valueGet = propertiesMap->GetKeyValue(index);

      if((Controller::TextFitInfo::Property::TEXT_FIT_ENABLE == valueGet.first.indexKey) || (TEXT_FIT_ENABLE_KEY == valueGet.first.stringKey))
      {
        /// Enable key.
        enabled = valueGet.second.Get<bool>();
      }
      else if((Controller::TextFitInfo::Property::TEXT_FIT_MIN_SIZE == valueGet.first.indexKey) || (TEXT_FIT_MIN_SIZE_KEY == valueGet.first.stringKey))
      {
        /// min size.
        minSize      = valueGet.second.Get<float>();
        isMinSizeSet = true;
      }
      else if((Controller::TextFitInfo::Property::TEXT_FIT_MAX_SIZE == valueGet.first.indexKey) || (TEXT_FIT_MAX_SIZE_KEY == valueGet.first.stringKey))
      {
        /// max size.
        maxSize      = valueGet.second.Get<float>();
        isMaxSizeSet = true;
      }
      else if((Controller::TextFitInfo::Property::TEXT_FIT_STEP_SIZE == valueGet.first.indexKey) || (TEXT_FIT_STEP_SIZE_KEY == valueGet.first.stringKey))
      {
        /// step size.
        stepSize      = valueGet.second.Get<float>();
        isStepSizeSet = true;
      }
      else if((Controller::TextFitInfo::Property::TEXT_FIT_FONT_SIZE_TYPE == valueGet.first.indexKey) || (TEXT_FIT_FONT_SIZE_TYPE_KEY == valueGet.first.stringKey))
      {
        if("pixelSize" == valueGet.second.Get<std::string>())
        {
          type = Controller::FontSizeType::PIXEL_SIZE;
        }
      }
    }

    controller->SetTextFitEnabled(enabled);
    // The TextFit operation is performed based on the MinLineSize set in the TextLabel at the moment when the TextFit property is set.
    // So, if you change the TextLabel's MinLineSize after setting the TextFit property, it does not affect the operation of TextFit.
    // This may require a new LineSize item in TextFit.
    controller->SetTextFitLineSize(controller->GetDefaultLineSize());
    if(isMinSizeSet)
    {
      controller->SetTextFitMinSize(minSize, type);
    }
    if(isMaxSizeSet)
    {
      controller->SetTextFitMaxSize(maxSize, type);
    }
    if(isStepSizeSet)
    {
      controller->SetTextFitStepSize(stepSize, type);
    }
  }
}

/**
 * @brief Discard the given visual into VisualFactory. The visual will be destroyed at next idle time.
 *
 * @param[in,out] visual Visual to be discarded. It will be reset to an empty handle.
 */
void DiscardTextLabelVisual(Dali::Toolkit::Visual::Base& visual)
{
  if(DALI_LIKELY(Dali::Adaptor::IsAvailable() && visual))
  {
    Dali::Toolkit::VisualFactory::Get().DiscardVisual(visual);
  }
  visual.Reset();
}

} // namespace

Toolkit::TextLabel TextLabel::New(ControlBehaviour additionalBehaviour)
{
  // Create the implementation, temporarily owned by this handle on stack
  IntrusivePtr<TextLabel> impl = new TextLabel(additionalBehaviour);

  // Pass ownership to CustomActor handle
  Toolkit::TextLabel handle(*impl);

  // Second-phase init of the implementation
  // This can only be done after the CustomActor connection has been made...
  impl->Initialize();

  return handle;
}

void TextLabel::SetProperty(BaseObject* object, Property::Index index, const Property::Value& value)
{
  Toolkit::TextLabel label = Toolkit::TextLabel::DownCast(Dali::BaseHandle(object));

  if(label)
  {
    TextLabel& impl(GetImpl(label));
    DALI_ASSERT_ALWAYS(impl.mController && "No text contoller");

    switch(index)
    {
      case Toolkit::DevelTextLabel::Property::RENDERING_BACKEND:
      {
        int backend = value.Get<int>();

#ifndef ENABLE_VECTOR_BASED_TEXT_RENDERING
        if(DevelText::RENDERING_VECTOR_BASED == backend)
        {
          backend = TextAbstraction::BITMAP_GLYPH; // Fallback to bitmap-based rendering
        }
#endif
        if(impl.mRenderingBackend != backend)
        {
          impl.mRenderingBackend = backend;
          impl.mTextUpdateNeeded = true;

          // When using the vector-based rendering, the size of the GLyphs are different
          TextAbstraction::GlyphType glyphType = (DevelText::RENDERING_VECTOR_BASED == impl.mRenderingBackend) ? TextAbstraction::VECTOR_GLYPH : TextAbstraction::BITMAP_GLYPH;
          impl.mController->SetGlyphType(glyphType);
        }
        break;
      }
      case Toolkit::TextLabel::Property::TEXT:
      {
        impl.mController->SetText(value.Get<std::string>());
        impl.mTextUpdateNeeded = true;

        if(impl.mController->HasAnchors())
        {
          // Forward input events to controller
          impl.EnableGestureDetection(static_cast<GestureType::Value>(GestureType::TAP));
        }
        else
        {
          impl.DisableGestureDetection(static_cast<GestureType::Value>(GestureType::TAP));
        }

        break;
      }
      case Toolkit::TextLabel::Property::FONT_FAMILY:
      {
        const std::string& fontFamily = value.Get<std::string>();

        DALI_LOG_INFO(gLogFilter, Debug::Verbose, "TextLabel::SetProperty Property::FONT_FAMILY newFont(%s)\n", fontFamily.c_str());
        impl.mController->SetDefaultFontFamily(fontFamily);
        impl.mIsAsyncRenderNeeded = true;
        break;
      }
      case Toolkit::TextLabel::Property::FONT_STYLE:
      {
        SetFontStyleProperty(impl.mController, value, Text::FontStyle::DEFAULT);
        impl.mIsAsyncRenderNeeded = true;
        break;
      }
      case Toolkit::TextLabel::Property::POINT_SIZE:
      {
        const float pointSize = value.Get<float>();

        if(!Equals(impl.mController->GetDefaultFontSize(Text::Controller::POINT_SIZE), pointSize))
        {
          impl.mController->SetDefaultFontSize(pointSize, Text::Controller::POINT_SIZE);
          impl.mIsAsyncRenderNeeded = true;
        }
        break;
      }
      case Toolkit::TextLabel::Property::MULTI_LINE:
      {
        impl.mController->SetMultiLineEnabled(value.Get<bool>());
        impl.mIsAsyncRenderNeeded = true;
        break;
      }
      case Toolkit::TextLabel::Property::HORIZONTAL_ALIGNMENT:
      {
        Text::HorizontalAlignment::Type alignment(static_cast<Text::HorizontalAlignment::Type>(-1)); // Set to invalid value to ensure a valid mode does get set
        if(Text::GetHorizontalAlignmentEnumeration(value, alignment))
        {
          impl.mController->SetHorizontalAlignment(alignment);
          impl.mIsAsyncRenderNeeded = true;
        }
        break;
      }
      case Toolkit::TextLabel::Property::VERTICAL_ALIGNMENT:
      {
        Toolkit::Text::VerticalAlignment::Type alignment(static_cast<Text::VerticalAlignment::Type>(-1)); // Set to invalid value to ensure a valid mode does get set
        if(Text::GetVerticalAlignmentEnumeration(value, alignment))
        {
          impl.mController->SetVerticalAlignment(alignment);
          impl.mIsAsyncRenderNeeded = true;
        }
        break;
      }
      case Toolkit::TextLabel::Property::ENABLE_MARKUP:
      {
        const bool enableMarkup = value.Get<bool>();
        impl.mController->SetMarkupProcessorEnabled(enableMarkup);

        if(impl.mController->HasAnchors())
        {
          // Forward input events to controller
          impl.EnableGestureDetection(static_cast<GestureType::Value>(GestureType::TAP));
        }
        else
        {
          impl.DisableGestureDetection(static_cast<GestureType::Value>(GestureType::TAP));
        }
        break;
      }
      case Toolkit::TextLabel::Property::ENABLE_AUTO_SCROLL:
      {
        if(impl.mController->IsTextElideEnabled() && impl.mController->GetEllipsisMode() == DevelText::Ellipsize::AUTO_SCROLL)
        {
          DALI_LOG_DEBUG_INFO("Tried to autoscroll while in ellipsize auto scroll mode, request ignored.\n");
        }
        else
        {
          const bool enableAutoScroll = value.Get<bool>();
          impl.mLastAutoScrollEnabled = enableAutoScroll;
          // If request to auto scroll is the same as current state then do nothing.
          if(enableAutoScroll != impl.mController->IsAutoScrollEnabled())
          {
            // If request is disable (false) and auto scrolling is enabled then need to stop it
            if(enableAutoScroll == false)
            {
              if(impl.mTextScroller)
              {
                impl.mTextScroller->StopScrolling();
              }
            }
            // If request is enable (true) then start autoscroll as not already running
            else
            {
              impl.mController->SetAutoScrollEnabled(enableAutoScroll);
            }
            impl.mIsAsyncRenderNeeded = true;
          }
        }
        break;
      }
      case Toolkit::TextLabel::Property::AUTO_SCROLL_STOP_MODE:
      {
        Text::TextScrollerPtr                        textScroller = impl.GetTextScroller();
        Toolkit::TextLabel::AutoScrollStopMode::Type stopMode     = textScroller->GetStopMode();
        if(Scripting::GetEnumerationProperty<Toolkit::TextLabel::AutoScrollStopMode::Type>(value,
                                                                                           AUTO_SCROLL_STOP_MODE_TABLE,
                                                                                           AUTO_SCROLL_STOP_MODE_TABLE_COUNT,
                                                                                           stopMode))
        {
          textScroller->SetStopMode(stopMode);
        }
        break;
      }
      case Toolkit::TextLabel::Property::AUTO_SCROLL_SPEED:
      {
        impl.GetTextScroller()->SetSpeed(value.Get<int>());
        break;
      }
      case Toolkit::TextLabel::Property::AUTO_SCROLL_LOOP_COUNT:
      {
        impl.GetTextScroller()->SetLoopCount(value.Get<int>());
        break;
      }
      case Toolkit::TextLabel::Property::AUTO_SCROLL_LOOP_DELAY:
      {
        impl.GetTextScroller()->SetLoopDelay(value.Get<float>());
        break;
      }
      case Toolkit::TextLabel::Property::AUTO_SCROLL_GAP:
      {
        impl.GetTextScroller()->SetGap(value.Get<float>());
        break;
      }
      case Toolkit::TextLabel::Property::LINE_SPACING:
      {
        const float lineSpacing = value.Get<float>();
        impl.mTextUpdateNeeded  = impl.mController->SetDefaultLineSpacing(lineSpacing) || impl.mTextUpdateNeeded;
        break;
      }
      case Toolkit::TextLabel::Property::UNDERLINE:
      {
        impl.mTextUpdateNeeded = SetUnderlineProperties(impl.mController, value, Text::EffectStyle::DEFAULT) || impl.mTextUpdateNeeded;
        break;
      }
      case Toolkit::TextLabel::Property::SHADOW:
      {
        impl.mTextUpdateNeeded = SetShadowProperties(impl.mController, value, Text::EffectStyle::DEFAULT) || impl.mTextUpdateNeeded;
        break;
      }
      case Toolkit::TextLabel::Property::EMBOSS:
      {
        impl.mTextUpdateNeeded = SetEmbossProperties(impl.mController, value, Text::EffectStyle::DEFAULT) || impl.mTextUpdateNeeded;
        break;
      }
      case Toolkit::TextLabel::Property::OUTLINE:
      {
        impl.mTextUpdateNeeded = SetOutlineProperties(impl.mController, value, Text::EffectStyle::DEFAULT) || impl.mTextUpdateNeeded;
        break;
      }
      case Toolkit::TextLabel::Property::PIXEL_SIZE:
      {
        const float pixelSize = value.Get<float>();
        DALI_LOG_INFO(gLogFilter, Debug::General, "TextLabel %p PIXEL_SIZE %f\n", impl.mController.Get(), pixelSize);

        if(!Equals(impl.mController->GetDefaultFontSize(Text::Controller::PIXEL_SIZE), pixelSize))
        {
          impl.mController->SetDefaultFontSize(pixelSize, Text::Controller::PIXEL_SIZE);
          impl.mIsAsyncRenderNeeded = true;
        }
        break;
      }
      case Toolkit::TextLabel::Property::ELLIPSIS:
      {
        const bool ellipsis = value.Get<bool>();
        DALI_LOG_INFO(gLogFilter, Debug::General, "TextLabel %p ELLIPSIS %d\n", impl.mController.Get(), ellipsis);

        impl.mController->SetTextElideEnabled(ellipsis);
        impl.mIsAsyncRenderNeeded = true;
        impl.RequestTextRelayout();
        break;
      }
      case Toolkit::TextLabel::Property::LINE_WRAP_MODE:
      {
        Text::LineWrap::Mode lineWrapMode(static_cast<Text::LineWrap::Mode>(-1)); // Set to invalid value to ensure a valid mode does get set
        if(GetLineWrapModeEnumeration(value, lineWrapMode))
        {
          DALI_LOG_INFO(gLogFilter, Debug::General, "TextLabel %p LineWrap::MODE %d\n", impl.mController.Get(), lineWrapMode);
          impl.mController->SetLineWrapMode(lineWrapMode);
          impl.mIsAsyncRenderNeeded = true;
        }
        break;
      }
      case Toolkit::DevelTextLabel::Property::VERTICAL_LINE_ALIGNMENT:
      {
        if(impl.mController->GetTextModel())
        {
          DevelText::VerticalLineAlignment::Type alignment = static_cast<DevelText::VerticalLineAlignment::Type>(value.Get<int>());

          impl.mController->SetVerticalLineAlignment(alignment);

          // Property doesn't affect the layout, only Visual must be updated
          TextVisual::EnableRendererUpdate(impl.mVisual);

          // No need to trigger full re-layout. Instead call UpdateRenderer() directly
          TextVisual::UpdateRenderer(impl.mVisual);
          impl.mIsAsyncRenderNeeded = true;
        }
        break;
      }
      case Toolkit::DevelTextLabel::Property::BACKGROUND:
      {
        impl.mTextUpdateNeeded = SetBackgroundProperties(impl.mController, value, Text::EffectStyle::DEFAULT) || impl.mTextUpdateNeeded;
        break;
      }
      case Toolkit::DevelTextLabel::Property::IGNORE_SPACES_AFTER_TEXT:
      {
        impl.mController->SetIgnoreSpacesAfterText(value.Get<bool>());
        impl.mIsAsyncRenderNeeded = true;
        break;
      }
      case Toolkit::DevelTextLabel::Property::MATCH_SYSTEM_LANGUAGE_DIRECTION:
      {
        impl.mController->SetMatchLayoutDirection(value.Get<bool>() ? DevelText::MatchLayoutDirection::LOCALE : DevelText::MatchLayoutDirection::CONTENTS);
        impl.mIsAsyncRenderNeeded = true;
        break;
      }
      case Toolkit::DevelTextLabel::Property::TEXT_FIT:
      {
        // If TextFitArray is enabled, this should be disabled.
        if(impl.mController->IsTextFitArrayEnabled())
        {
          impl.mController->SetDefaultLineSize(impl.mController->GetCurrentLineSize());
          impl.mController->SetTextFitArrayEnabled(false);
        }

        ParseTextFitProperty(impl.mController, value.GetMap());
        impl.mController->SetTextFitChanged(true);
        impl.mIsAsyncRenderNeeded = true;
        break;
      }
      case Toolkit::DevelTextLabel::Property::MIN_LINE_SIZE:
      {
        const float lineSize = value.Get<float>();
        // If TextFitArray is enabled, do not update the default line size.
        if(!impl.mController->IsTextFitArrayEnabled())
        {
          impl.mTextUpdateNeeded = impl.mController->SetDefaultLineSize(lineSize) || impl.mTextUpdateNeeded;
        }
        impl.mController->SetCurrentLineSize(lineSize);
        impl.mIsAsyncRenderNeeded = true;
        break;
      }
      case Toolkit::DevelTextLabel::Property::FONT_SIZE_SCALE:
      {
        const float scale = value.Get<float>();
        DALI_LOG_INFO(gLogFilter, Debug::General, "TextLabel %p FONT_SIZE_SCALE %f\n", impl.mController.Get(), scale);

        if(!Equals(impl.mController->GetFontSizeScale(), scale))
        {
          impl.mController->SetFontSizeScale(scale);
          impl.mTextUpdateNeeded = true;
        }
        break;
      }
      case Toolkit::DevelTextLabel::Property::ENABLE_FONT_SIZE_SCALE:
      {
        const bool enableFontSizeScale = value.Get<bool>();
        if(!Equals(impl.mController->IsFontSizeScaleEnabled(), enableFontSizeScale))
        {
          impl.mController->SetFontSizeScaleEnabled(enableFontSizeScale);
          impl.mTextUpdateNeeded = true;
        }
        break;
      }
      case Toolkit::DevelTextLabel::Property::ELLIPSIS_POSITION:
      {
        DevelText::EllipsisPosition::Type ellipsisPositionType(static_cast<DevelText::EllipsisPosition::Type>(-1)); // Set to invalid value to ensure a valid mode does get set
        if(GetEllipsisPositionTypeEnumeration(value, ellipsisPositionType))
        {
          DALI_LOG_INFO(gLogFilter, Debug::General, "TextLabel %p EllipsisPosition::Type %d\n", impl.mController.Get(), ellipsisPositionType);
          impl.mController->SetEllipsisPosition(ellipsisPositionType);
          impl.mIsAsyncRenderNeeded = true;
          impl.RequestTextRelayout();
        }
        break;
      }
      case Toolkit::DevelTextLabel::Property::STRIKETHROUGH:
      {
        impl.mTextUpdateNeeded = SetStrikethroughProperties(impl.mController, value, Text::EffectStyle::DEFAULT) || impl.mTextUpdateNeeded;
        break;
      }
      case Toolkit::DevelTextLabel::Property::CHARACTER_SPACING:
      {
        const float characterSpacing = value.Get<float>();
        impl.mController->SetCharacterSpacing(characterSpacing);
        impl.mIsAsyncRenderNeeded = true;
        break;
      }
      case Toolkit::DevelTextLabel::Property::RELATIVE_LINE_SIZE:
      {
        const float relativeLineSize = value.Get<float>();
        DALI_LOG_INFO(gLogFilter, Debug::Verbose, "TextLabel %p RELATIVE_LINE_SIZE %f\n", impl.mController.Get(), relativeLineSize);

        impl.mController->SetRelativeLineSize(relativeLineSize);
        impl.mIsAsyncRenderNeeded = true;
        break;
      }
      case Toolkit::DevelTextLabel::Property::ANCHOR_COLOR:
      {
        const Vector4& anchorColor = value.Get<Vector4>();
        if(impl.mController->GetAnchorColor() != anchorColor)
        {
          impl.mController->SetAnchorColor(anchorColor);
          impl.mTextUpdateNeeded = true;
        }
        break;
      }
      case Toolkit::DevelTextLabel::Property::ANCHOR_CLICKED_COLOR:
      {
        const Vector4& anchorClickedColor = value.Get<Vector4>();
        if(impl.mController->GetAnchorClickedColor() != anchorClickedColor)
        {
          impl.mController->SetAnchorClickedColor(anchorClickedColor);
          impl.mTextUpdateNeeded = true;
        }
        break;
      }
      case Toolkit::DevelTextLabel::Property::REMOVE_FRONT_INSET:
      {
        const bool remove = value.Get<bool>();
        impl.mController->SetRemoveFrontInset(remove);
        impl.mIsAsyncRenderNeeded = true;
        break;
      }
      case Toolkit::DevelTextLabel::Property::REMOVE_BACK_INSET:
      {
        const bool remove = value.Get<bool>();
        impl.mController->SetRemoveBackInset(remove);
        impl.mIsAsyncRenderNeeded = true;
        break;
      }
      case Toolkit::DevelTextLabel::Property::CUTOUT:
      {
        const bool cutout = value.Get<bool>();

        impl.mController->SetTextCutout(cutout);
        impl.mIsAsyncRenderNeeded = true;
        break;
      }
      case Toolkit::DevelTextLabel::Property::RENDER_MODE:
      {
        DevelTextLabel::Render::Mode renderMode = static_cast<DevelTextLabel::Render::Mode>(value.Get<int>());
        if(renderMode < 0 || renderMode >= NUMBER_OF_RENDER_MODE)
        {
          renderMode = DevelTextLabel::Render::SYNC;
        }

        if(impl.mController->GetRenderMode() != renderMode)
        {
          impl.mController->SetRenderMode(renderMode);
          if(renderMode == DevelTextLabel::Render::ASYNC_AUTO)
          {
            impl.RequestTextRelayout();
          }
        }
        break;
      }
      case Toolkit::DevelTextLabel::Property::ELLIPSIS_MODE:
      {
        DevelText::Ellipsize::Mode ellipsisMode = static_cast<DevelText::Ellipsize::Mode>(value.Get<int>());
        if(impl.mController->GetEllipsisMode() != ellipsisMode)
        {
          impl.mController->SetEllipsisMode(ellipsisMode);
          Text::TextScrollerPtr textScroller = impl.GetTextScroller();
          if(textScroller)
          {
            textScroller->SetStopMode(Toolkit::TextLabel::AutoScrollStopMode::IMMEDIATE);
            textScroller->StopScrolling();
          }
          impl.mIsAsyncRenderNeeded = true;
          impl.RequestTextRelayout();
        }
        break;
      }
      case Toolkit::DevelTextLabel::Property::FONT_VARIATIONS:
      {
        const Property::Map variationsMap = value.Get<Property::Map>();
        impl.mController->SetVariationsMap(variationsMap);

        impl.RequestTextRelayout();
        impl.mIsAsyncRenderNeeded = true;
        break;
      }
      case Toolkit::DevelTextLabel::Property::RENDER_SCALE:
      {
        float renderScale = value.Get<float>();
        if(renderScale < 1.0f)
        {
          DALI_LOG_DEBUG_INFO("RenderScale must be greater than or equal to 1.0f. It will change as follows:%f -> 1.0\n", renderScale);
          renderScale = 1.0f;
        }

        if(fabsf(renderScale - impl.mController->GetRenderScale()) > Math::MACHINE_EPSILON_1)
        {
          impl.mController->SetRenderScale(renderScale);
          impl.mIsAsyncRenderNeeded = true;
          impl.RequestTextRelayout();
        }
        break;
      }
      case Toolkit::DevelTextLabel::Property::LAYOUT_DIRECTION_POLICY:
      {
        DevelText::MatchLayoutDirection layoutDirectionPolicy = static_cast<DevelText::MatchLayoutDirection>(value.Get<int>());
        if(impl.mController->GetMatchLayoutDirection() != layoutDirectionPolicy)
        {
          impl.mController->SetMatchLayoutDirection(layoutDirectionPolicy);
          impl.mIsAsyncRenderNeeded = true;
        }
        break;
      }
    }

    // Request relayout when text update is needed. It's necessary to call it
    // as changing the property not via UI interaction brings no effect if only
    // the mTextUpdateNeeded is changed.
    if(impl.mTextUpdateNeeded)
    {
      // need to request relayout as size of text may have changed
      impl.RequestTextRelayout();
      impl.mIsAsyncRenderNeeded = true;
    }
  }
}

Text::ControllerPtr TextLabel::GetTextController()
{
  return mController;
}

Property::Value TextLabel::GetProperty(BaseObject* object, Property::Index index)
{
  Property::Value value;

  Toolkit::TextLabel label = Toolkit::TextLabel::DownCast(Dali::BaseHandle(object));

  if(label)
  {
    TextLabel& impl(GetImpl(label));
    DALI_ASSERT_DEBUG(impl.mController && "No text contoller");

    switch(index)
    {
      case Toolkit::DevelTextLabel::Property::RENDERING_BACKEND:
      {
        value = impl.mRenderingBackend;
        break;
      }
      case Toolkit::TextLabel::Property::TEXT:
      {
        std::string text;
        impl.mController->GetText(text);
        value = text;
        break;
      }
      case Toolkit::TextLabel::Property::FONT_FAMILY:
      {
        value = impl.mController->GetDefaultFontFamily();
        break;
      }
      case Toolkit::TextLabel::Property::FONT_STYLE:
      {
        GetFontStyleProperty(impl.mController, value, Text::FontStyle::DEFAULT);
        break;
      }
      case Toolkit::TextLabel::Property::POINT_SIZE:
      {
        value = impl.mController->GetDefaultFontSize(Text::Controller::POINT_SIZE);
        break;
      }
      case Toolkit::TextLabel::Property::MULTI_LINE:
      {
        value = impl.mController->IsMultiLineEnabled();
        break;
      }
      case Toolkit::TextLabel::Property::HORIZONTAL_ALIGNMENT:
      {
        const char* name = Text::GetHorizontalAlignmentString(impl.mController->GetHorizontalAlignment());

        if(name)
        {
          value = std::string(name);
        }
        break;
      }
      case Toolkit::TextLabel::Property::VERTICAL_ALIGNMENT:
      {
        const char* name = Text::GetVerticalAlignmentString(impl.mController->GetVerticalAlignment());
        if(name)
        {
          value = std::string(name);
        }
        break;
      }
      case Toolkit::TextLabel::Property::ENABLE_MARKUP:
      {
        value = impl.mController->IsMarkupProcessorEnabled();
        break;
      }
      case Toolkit::TextLabel::Property::ENABLE_AUTO_SCROLL:
      {
        value = impl.mController->IsAutoScrollEnabled();
        break;
      }
      case Toolkit::TextLabel::Property::AUTO_SCROLL_STOP_MODE:
      {
        if(impl.mTextScroller)
        {
          const char* mode = Scripting::GetEnumerationName<Toolkit::TextLabel::AutoScrollStopMode::Type>(impl.mTextScroller->GetStopMode(),
                                                                                                         AUTO_SCROLL_STOP_MODE_TABLE,
                                                                                                         AUTO_SCROLL_STOP_MODE_TABLE_COUNT);
          if(mode)
          {
            value = std::string(mode);
          }
        }
        break;
      }
      case Toolkit::TextLabel::Property::AUTO_SCROLL_SPEED:
      {
        if(impl.mTextScroller)
        {
          value = impl.mTextScroller->GetSpeed();
        }
        break;
      }
      case Toolkit::TextLabel::Property::AUTO_SCROLL_LOOP_COUNT:
      {
        if(impl.mTextScroller)
        {
          value = impl.mTextScroller->GetLoopCount();
        }
        break;
      }
      case Toolkit::TextLabel::Property::AUTO_SCROLL_LOOP_DELAY:
      {
        if(impl.mTextScroller)
        {
          value = impl.mTextScroller->GetLoopDelay();
        }
        break;
      }
      case Toolkit::TextLabel::Property::AUTO_SCROLL_GAP:
      {
        if(impl.mTextScroller)
        {
          value = impl.mTextScroller->GetGap();
        }
        break;
      }
      case Toolkit::TextLabel::Property::LINE_SPACING:
      {
        value = impl.mController->GetDefaultLineSpacing();
        break;
      }
      case Toolkit::TextLabel::Property::UNDERLINE:
      {
        GetUnderlineProperties(impl.mController, value, Text::EffectStyle::DEFAULT);
        break;
      }
      case Toolkit::TextLabel::Property::SHADOW:
      {
        GetShadowProperties(impl.mController, value, Text::EffectStyle::DEFAULT);
        break;
      }
      case Toolkit::TextLabel::Property::EMBOSS:
      {
        GetEmbossProperties(impl.mController, value, Text::EffectStyle::DEFAULT);
        break;
      }
      case Toolkit::TextLabel::Property::OUTLINE:
      {
        GetOutlineProperties(impl.mController, value, Text::EffectStyle::DEFAULT);
        break;
      }
      case Toolkit::TextLabel::Property::PIXEL_SIZE:
      {
        value = impl.mController->GetDefaultFontSize(Text::Controller::PIXEL_SIZE);
        break;
      }
      case Toolkit::TextLabel::Property::ELLIPSIS:
      {
        value = impl.mController->IsTextElideEnabled();
        break;
      }
      case Toolkit::TextLabel::Property::LINE_WRAP_MODE:
      {
        value = impl.mController->GetLineWrapMode();
        break;
      }
      case Toolkit::TextLabel::Property::LINE_COUNT:
      {
        float width = label.GetProperty(Actor::Property::SIZE_WIDTH).Get<float>();
        value       = impl.mController->GetLineCount(width);
        break;
      }
      case Toolkit::DevelTextLabel::Property::TEXT_DIRECTION:
      {
        value = impl.mController->GetTextDirection();
        break;
      }
      case Toolkit::DevelTextLabel::Property::VERTICAL_LINE_ALIGNMENT:
      {
        value = impl.mController->GetVerticalLineAlignment();
        break;
      }
      case Toolkit::DevelTextLabel::Property::BACKGROUND:
      {
        GetBackgroundProperties(impl.mController, value, Text::EffectStyle::DEFAULT);
        break;
      }
      case Toolkit::DevelTextLabel::Property::IGNORE_SPACES_AFTER_TEXT:
      {
        value = impl.mController->IsIgnoreSpacesAfterText();
        break;
      }
      case Toolkit::DevelTextLabel::Property::MATCH_SYSTEM_LANGUAGE_DIRECTION:
      {
        value = impl.mController->GetMatchLayoutDirection() != DevelText::MatchLayoutDirection::CONTENTS;
        break;
      }
      case Toolkit::DevelTextLabel::Property::TEXT_FIT:
      {
        const bool  enabled   = impl.mController->IsTextFitEnabled();
        const float minSize   = impl.mController->GetTextFitMinSize();
        const float maxSize   = impl.mController->GetTextFitMaxSize();
        const float stepSize  = impl.mController->GetTextFitStepSize();
        const float pointSize = impl.mController->GetTextFitPointSize();

        Property::Map map;
        map.Insert(TEXT_FIT_ENABLE_KEY, enabled);
        map.Insert(TEXT_FIT_MIN_SIZE_KEY, minSize);
        map.Insert(TEXT_FIT_MAX_SIZE_KEY, maxSize);
        map.Insert(TEXT_FIT_STEP_SIZE_KEY, stepSize);
        map.Insert(TEXT_FIT_FONT_SIZE_KEY, pointSize);
        map.Insert(TEXT_FIT_FONT_SIZE_TYPE_KEY, "pointSize");

        value = map;
        break;
      }
      case Toolkit::DevelTextLabel::Property::MIN_LINE_SIZE:
      {
        // If TextFitArray is enabled, the stored value (MIN_LINE_SIZE set by the user) is retrun.
        value = impl.mController->IsTextFitArrayEnabled() ? impl.mController->GetCurrentLineSize() : impl.mController->GetDefaultLineSize();
        break;
      }
      case Toolkit::DevelTextLabel::Property::FONT_SIZE_SCALE:
      {
        value = impl.mController->GetFontSizeScale();
        break;
      }
      case Toolkit::DevelTextLabel::Property::ENABLE_FONT_SIZE_SCALE:
      {
        value = impl.mController->IsFontSizeScaleEnabled();
        break;
      }
      case Toolkit::DevelTextLabel::Property::ELLIPSIS_POSITION:
      {
        value = impl.mController->GetEllipsisPosition();
        break;
      }
      case Toolkit::DevelTextLabel::Property::STRIKETHROUGH:
      {
        GetStrikethroughProperties(impl.mController, value, Text::EffectStyle::DEFAULT);
        break;
      }
      case Toolkit::DevelTextLabel::Property::CHARACTER_SPACING:
      {
        value = impl.mController->GetCharacterSpacing();
        break;
      }
      case Toolkit::DevelTextLabel::Property::RELATIVE_LINE_SIZE:
      {
        value = impl.mController->GetRelativeLineSize();
        break;
      }
      case Toolkit::DevelTextLabel::Property::ANCHOR_COLOR:
      {
        value = impl.mController->GetAnchorColor();
        break;
      }
      case Toolkit::DevelTextLabel::Property::ANCHOR_CLICKED_COLOR:
      {
        value = impl.mController->GetAnchorClickedColor();
        break;
      }
      case Toolkit::DevelTextLabel::Property::REMOVE_FRONT_INSET:
      {
        value = impl.mController->IsRemoveFrontInset();
        break;
      }
      case Toolkit::DevelTextLabel::Property::REMOVE_BACK_INSET:
      {
        value = impl.mController->IsRemoveBackInset();
        break;
      }
      case Toolkit::DevelTextLabel::Property::CUTOUT:
      {
        value = impl.mController->IsTextCutout();
        break;
      }
      case Toolkit::DevelTextLabel::Property::RENDER_MODE:
      {
        value = impl.mController->GetRenderMode();
        break;
      }
      case Toolkit::DevelTextLabel::Property::MANUAL_RENDERED:
      {
        value = impl.mManualRendered;
        break;
      }
      case Toolkit::DevelTextLabel::Property::ASYNC_LINE_COUNT:
      {
        value = impl.mAsyncLineCount;
        break;
      }
      case Toolkit::DevelTextLabel::Property::ELLIPSIS_MODE:
      {
        value = impl.mController->GetEllipsisMode();
        break;
      }
      case Toolkit::DevelTextLabel::Property::IS_SCROLLING:
      {
        value = impl.mTextScroller && impl.mTextScroller->IsScrolling() ? true : false;
        break;
      }
      case Toolkit::DevelTextLabel::Property::FONT_VARIATIONS:
      {
        Property::Map variationsMap;
        impl.mController->GetVariationsMap(variationsMap);

        value = variationsMap;
        break;
      }
      case Toolkit::DevelTextLabel::Property::RENDER_SCALE:
      {
        value = impl.mController->GetRenderScale();
        break;
      }
      case Toolkit::DevelTextLabel::Property::NEED_REQUEST_ASYNC_RENDER:
      {
        value = impl.mIsAsyncRenderNeeded || impl.mTextUpdateNeeded;
        break;
      }
      case Toolkit::DevelTextLabel::Property::LAYOUT_DIRECTION_POLICY:
      {
        value = impl.mController->GetMatchLayoutDirection();
        break;
      }
    }
  }

  return value;
}

bool TextLabel::DoConnectSignal(BaseObject* object, ConnectionTrackerInterface* tracker, const std::string& signalName, FunctorDelegate* functor)
{
  Dali::BaseHandle handle(object);

  bool               connected(true);
  Toolkit::TextLabel label = Toolkit::TextLabel::DownCast(handle);

  if(0 == strcmp(signalName.c_str(), SIGNAL_ANCHOR_CLICKED))
  {
    if(label)
    {
      Internal::TextLabel& labelImpl(GetImpl(label));
      labelImpl.AnchorClickedSignal().Connect(tracker, functor);
    }
  }
  else if(0 == strcmp(signalName.c_str(), SIGNAL_TEXT_FIT_CHANGED))
  {
    if(label)
    {
      Internal::TextLabel& labelImpl(GetImpl(label));
      labelImpl.TextFitChangedSignal().Connect(tracker, functor);
    }
  }
  else if(0 == strcmp(signalName.c_str(), SIGNAL_ASYNC_TEXT_RENDERED))
  {
    if(label)
    {
      Internal::TextLabel& labelImpl(GetImpl(label));
      labelImpl.AsyncTextRenderedSignal().Connect(tracker, functor);
    }
  }
  else if(0 == strcmp(signalName.c_str(), SIGNAL_ASYNC_NATURAL_SIZE_COMPUTED))
  {
    if(label)
    {
      Internal::TextLabel& labelImpl(GetImpl(label));
      labelImpl.AsyncNaturalSizeComputedSignal().Connect(tracker, functor);
    }
  }
  else if(0 == strcmp(signalName.c_str(), SIGNAL_ASYNC_HEIGHT_FOR_WIDTH_COMPUTED))
  {
    if(label)
    {
      Internal::TextLabel& labelImpl(GetImpl(label));
      labelImpl.AsyncHeightForWidthComputedSignal().Connect(tracker, functor);
    }
  }
  else
  {
    // signalName does not match any signal
    connected = false;
  }

  return connected;
}

DevelTextLabel::AnchorClickedSignalType& TextLabel::AnchorClickedSignal()
{
  return mAnchorClickedSignal;
}

DevelTextLabel::TextFitChangedSignalType& TextLabel::TextFitChangedSignal()
{
  return mTextFitChangedSignal;
}

DevelTextLabel::AsyncTextRenderedSignalType& TextLabel::AsyncTextRenderedSignal()
{
  return mAsyncTextRenderedSignal;
}

DevelTextLabel::AsyncNaturalSizeComputedSignalType& TextLabel::AsyncNaturalSizeComputedSignal()
{
  return mAsyncNaturalSizeComputedSignal;
}

DevelTextLabel::AsyncHeightForWidthComputedSignalType& TextLabel::AsyncHeightForWidthComputedSignal()
{
  return mAsyncHeightForWidthComputedSignal;
}

void TextLabel::OnInitialize()
{
  Actor self = Self();

  Property::Map propertyMap;
  propertyMap.Add(Toolkit::Visual::Property::TYPE, Toolkit::Visual::TEXT);

  mVisual = Toolkit::VisualFactory::Get().CreateVisual(propertyMap);
  DevelControl::RegisterVisual(*this, Toolkit::TextLabel::Property::TEXT, mVisual, DepthIndex::CONTENT);

  TextVisual::SetAsyncTextInterface(mVisual, this);
  TextVisual::SetAnimatableTextColorProperty(mVisual, Toolkit::TextLabel::Property::TEXT_COLOR);
  self.SetProperty(Toolkit::TextLabel::Property::PIXEL_SNAP_FACTOR, 0.0f);

  mController = TextVisual::GetController(mVisual);
  DALI_ASSERT_DEBUG(mController && "Invalid Text Controller")

  mController->SetControlInterface(this);
  mController->SetAnchorControlInterface(this);

  // Use height-for-width negotiation by default
  self.SetResizePolicy(ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH);
  self.SetResizePolicy(ResizePolicy::DIMENSION_DEPENDENCY, Dimension::HEIGHT);

  // Enable the text ellipsis.
  mController->SetTextElideEnabled(true); // If false then text larger than control will overflow

  // Sets layoutDirection value
  Dali::Stage                 stage           = Dali::Stage::GetCurrent();
  Dali::LayoutDirection::Type layoutDirection = static_cast<Dali::LayoutDirection::Type>(stage.GetRootLayer().GetProperty(Dali::Actor::Property::LAYOUT_DIRECTION).Get<int>());
  mController->SetLayoutDirection(layoutDirection);

  self.LayoutDirectionChangedSignal().Connect(this, &TextLabel::OnLayoutDirectionChanged);

  if(Dali::Adaptor::IsAvailable())
  {
    Dali::Adaptor::Get().LocaleChangedSignal().Connect(this, &TextLabel::OnLocaleChanged);
  }

  Layout::Engine& engine = mController->GetLayoutEngine();
  engine.SetCursorWidth(0u); // Do not layout space for the cursor.

  // Accessibility
  self.SetProperty(DevelControl::Property::ACCESSIBILITY_ROLE, DevelControl::AccessibilityRole::TEXT);

  Accessibility::Bridge::EnabledSignal().Connect(this, &TextLabel::OnAccessibilityStatusChanged);
  Accessibility::Bridge::DisabledSignal().Connect(this, &TextLabel::OnAccessibilityStatusChanged);
}

DevelControl::ControlAccessible* TextLabel::CreateAccessibleObject()
{
  return new TextLabelAccessible(Self());
}

void TextLabel::OnStyleChange(Toolkit::StyleManager styleManager, StyleChange::Type change)
{
  DALI_LOG_INFO(gLogFilter, Debug::Verbose, "TextLabel::OnStyleChange\n");

  switch(change)
  {
    case StyleChange::DEFAULT_FONT_CHANGE:
    {
      // Property system did not set the font so should update it.
      const std::string& newFont = GetImpl(styleManager).GetDefaultFontFamily();
      DALI_LOG_INFO(gLogFilter, Debug::General, "TextLabel::OnStyleChange StyleChange::DEFAULT_FONT_CHANGE newFont(%s)\n", newFont.c_str());
      mController->UpdateAfterFontChange(newFont);
      RelayoutRequest();
      break;
    }
    case StyleChange::DEFAULT_FONT_SIZE_CHANGE:
    {
      GetImpl(styleManager).ApplyThemeStyle(Toolkit::Control(GetOwner()));
      RelayoutRequest();
      break;
    }
    case StyleChange::THEME_CHANGE:
    {
      // Nothing to do, let control base class handle this
      break;
    }
  }

  // Up call to Control
  Control::OnStyleChange(styleManager, change);
}

void TextLabel::OnTap(const TapGesture& gesture)
{
  DALI_LOG_INFO(gLogFilter, Debug::Verbose, "TextLabel::OnTap %p\n", mController.Get());

  // Deliver the tap before the focus event to controller; this allows us to detect when focus is gained due to tap-gestures
  Extents padding;
  padding                   = Self().GetProperty<Extents>(Toolkit::Control::Property::PADDING);
  const Vector2& localPoint = gesture.GetLocalPoint();
  mController->AnchorEvent(localPoint.x - padding.start, localPoint.y - padding.top);
}

void TextLabel::AnchorClicked(const std::string& href)
{
  Dali::Toolkit::TextLabel handle(GetOwner());
  mAnchorClickedSignal.Emit(handle, href.c_str(), href.length());
}

Vector3 TextLabel::GetNaturalSize()
{
  Extents padding;
  padding = Self().GetProperty<Extents>(Toolkit::Control::Property::PADDING);

  Vector3 naturalSize = mController->GetNaturalSize();
  naturalSize.width += (padding.start + padding.end);
  naturalSize.height += (padding.top + padding.bottom);

  return naturalSize;
}

float TextLabel::GetHeightForWidth(float width)
{
  Extents padding;
  padding = Self().GetProperty<Extents>(Toolkit::Control::Property::PADDING);

  return mController->GetHeightForWidth(width) + padding.top + padding.bottom;
}

void TextLabel::OnPropertySet(Property::Index index, const Property::Value& propertyValue)
{
  DALI_LOG_INFO(gLogFilter, Debug::Verbose, "TextLabel::OnPropertySet index[%d]\n", index);

  switch(index)
  {
    case Dali::Actor::Property::SIZE:
    {
      const Vector2& size = propertyValue.Get<Vector2>();
      if(mSize != size)
      {
        mSize          = size;
        mIsSizeChanged = true;
      }
      break;
    }
    case Dali::Actor::Property::SIZE_WIDTH:
    {
      const float width = propertyValue.Get<float>();
      if(mSize.width != width)
      {
        mSize.width    = width;
        mIsSizeChanged = true;
      }
      break;
    }
    case Dali::Actor::Property::SIZE_HEIGHT:
    {
      const float height = propertyValue.Get<float>();
      if(mSize.height != height)
      {
        mSize.height   = height;
        mIsSizeChanged = true;
      }
      break;
    }
    case Toolkit::TextLabel::Property::TEXT_COLOR:
    {
      const Vector4& textColor = propertyValue.Get<Vector4>();
      if(mController->GetDefaultColor() != textColor)
      {
        mController->SetDefaultColor(textColor);
        mTextUpdateNeeded    = true;
        mIsAsyncRenderNeeded = mIsAsyncRenderNeeded ? true : (mController->IsUnderlineEnabled() || mController->IsStrikethroughEnabled());
      }
      break;
    }
    case Toolkit::Control::Property::BACKGROUND:
    {
      if(mController->IsTextCutout())
      {
        const Vector4 backgroundColor = propertyValue.Get<Vector4>();
        mController->SetBackgroundColorWithCutout(backgroundColor);
        mController->SetBackgroundWithCutoutEnabled(true);

        if(mController->GetRenderMode() == DevelTextLabel::Render::SYNC)
        {
          EnableControlBackground(false);
        }
        mIsAsyncRenderNeeded = true;
      }
      break;
    }
    case Toolkit::DevelTextLabel::Property::CUTOUT:
    {
      const bool cutoutEnabled = propertyValue.Get<bool>();
      mController->SetBackgroundWithCutoutEnabled(cutoutEnabled);
      if(cutoutEnabled)
      {
        const Property::Map backgroundMap   = Self().GetProperty(Toolkit::Control::Property::BACKGROUND).Get<Property::Map>();
        Property::Value*    backgroundValue = backgroundMap.Find(ColorVisual::Property::MIX_COLOR);
        if(backgroundValue)
        {
          Vector4 backgroundColor = Vector4::ZERO;
          backgroundColor         = backgroundValue->Get<Vector4>();
          mController->SetBackgroundColorWithCutout(backgroundColor);
        }
      }
      if(mController->GetRenderMode() == DevelTextLabel::Render::SYNC)
      {
        EnableControlBackground(!cutoutEnabled);
        TextVisual::SetRequireRender(mVisual, cutoutEnabled);
      }
      mIsAsyncRenderNeeded = true;
      break;
    }
    default:
    {
      if(Self().DoesCustomPropertyExist(index) && mVariationIndexMap.find(index) != mVariationIndexMap.end())
      {
        std::string tag   = mVariationIndexMap[index];
        float       value = propertyValue.Get<float>();

        Property::Map map;
        mController->GetVariationsMap(map);
        map[tag.data()] = value;

        mController->SetVariationsMap(map);
      }
      else
      {
        Control::OnPropertySet(index, propertyValue); // up call to control for non-handled properties
      }
      break;
    }
  }
}

void TextLabel::OnSceneConnection(int depth)
{
  mIsAsyncRenderNeeded = true;

  if(mController->IsAutoScrollEnabled() || mLastAutoScrollEnabled)
  {
    mController->SetAutoScrollEnabled(true);
  }
  Control::OnSceneConnection(depth);
}

void TextLabel::OnSceneDisconnection()
{
  mIsSizeChanged    = false;
  mIsManualRender   = false;
  mIsManualRendered = false;

  if(mTextScroller)
  {
    if(mLastAutoScrollEnabled && !mController->IsAutoScrollEnabled())
    {
      mLastAutoScrollEnabled = false;
    }

    if(mTextScroller->IsScrolling())
    {
      const Toolkit::TextLabel::AutoScrollStopMode::Type stopMode = mTextScroller->GetStopMode();
      mTextScroller->SetStopMode(Toolkit::TextLabel::AutoScrollStopMode::IMMEDIATE);
      mTextScroller->StopScrolling();
      mTextScroller->SetStopMode(stopMode);
    }
  }
  Control::OnSceneDisconnection();
}

void TextLabel::OnRelayout(const Vector2& size, RelayoutContainer& container)
{
  DALI_LOG_INFO(gLogFilter, Debug::General, "TextLabel::OnRelayout\n");
  bool sizeChanged    = mIsSizeChanged;
  bool manualRendered = mIsManualRendered;
  mIsSizeChanged      = false;
  mIsManualRendered   = false;

  if(mController->GetRenderMode() == DevelTextLabel::Render::ASYNC_MANUAL)
  {
    return;
  }

  if(mTextScroller && mTextScroller->IsStop())
  {
    // When auto scroll is playing, it triggers a relayout only when an update is absolutely necessary.
    return;
  }

  Actor self = Self();

  Extents padding;
  padding = self.GetProperty<Extents>(Toolkit::Control::Property::PADDING);

  float   width  = std::max(size.x - (padding.start + padding.end), 0.0f);
  float   height = std::max(size.y - (padding.top + padding.bottom), 0.0f);
  Vector2 contentSize(width, height);

  // Support Right-To-Left
  Dali::LayoutDirection::Type layoutDirection = mController->GetLayoutDirection(self);

  // Support Right-To-Left of padding
  if(Dali::LayoutDirection::RIGHT_TO_LEFT == layoutDirection)
  {
    std::swap(padding.start, padding.end);
  }

  if(mController->GetRenderMode() == DevelTextLabel::Render::ASYNC_AUTO ||
     mController->GetRenderMode() == DevelTextLabel::Render::ASYNC_MANUAL)
  {
    if(mController->GetRenderMode() == DevelTextLabel::Render::ASYNC_AUTO && mTextScroller && mTextScroller->IsScrolling() && !(mTextUpdateNeeded || sizeChanged))
    {
      // When auto scroll is playing, a text load request is made only if a text update is absolutely necessary.
      return;
    }

    if(mIsManualRender || !(sizeChanged || mIsAsyncRenderNeeded))
    {
      // Do not request async render if the manual render is still ongoing or if there are no size or property updates.
      return;
    }

    if(manualRendered && sizeChanged && !mIsAsyncRenderNeeded)
    {
      // Do not request async render if only the size has changed when manual render is completed.
      // Users may attempt to change the size inside the completed callback post manual render.
      // In case of ASYNC_AUTO, this could potentially trigger relayout and engender further computation.
      // This is needed to avoid recomputations, but it may have some limitations.
      return;
    }

    DALI_LOG_RELEASE_INFO("Request render, size : %f, %f [%p]\n", contentSize.width, contentSize.height, static_cast<void *>(mController.Get()));
    AsyncTextParameters parameters = GetAsyncTextParameters(Async::RENDER_FIXED_SIZE, contentSize, padding, layoutDirection);
    TextVisual::UpdateAsyncRenderer(mVisual, parameters);
    mTextUpdateNeeded    = false;
    mIsAsyncRenderNeeded = false;
    return;
  }

  if(mController->IsTextFitArrayEnabled())
  {
    mController->FitArrayPointSizeforLayout(contentSize);
    mController->SetTextFitContentSize(contentSize);
  }
  else if(mController->IsTextFitEnabled())
  {
    mController->FitPointSizeforLayout(contentSize);
    mController->SetTextFitContentSize(contentSize);
  }

  if(mController->IsTextElideEnabled() && mController->GetEllipsisMode() == DevelText::Ellipsize::AUTO_SCROLL)
  {
    if(mController->IsMultiLineEnabled())
    {
      DALI_LOG_DEBUG_INFO("Attempted ellipsize auto scroll on a non SINGLE_LINE_BOX, request ignored\n");
    }
    else
    {
      const Size naturalSize       = GetNaturalSize().GetVectorXY();
      bool       autoScrollEnabled = contentSize.width < naturalSize.width ? true : false;
      bool       requestRelayout   = false;

      if(autoScrollEnabled != mController->IsAutoScrollEnabled())
      {
        mController->SetAutoScrollEnabled(autoScrollEnabled, requestRelayout);
      }
    }
  }

  const Text::Controller::UpdateTextType updateTextType = mController->Relayout(contentSize, layoutDirection);

  if((Text::Controller::NONE_UPDATED != (Text::Controller::MODEL_UPDATED & updateTextType)) || mTextUpdateNeeded)
  {
    DALI_LOG_INFO(gLogFilter, Debug::General, "TextLabel::OnRelayout IsAutoScrollEnabled[%s] [%p]\n", (mController->IsAutoScrollEnabled()) ? "true" : "false", static_cast<void *>(mController.Get()));

    // Update the visual
    TextVisual::EnableRendererUpdate(mVisual);

    // Calculate the size of the visual that can fit the text
    Size layoutSize = mController->GetTextModel()->GetLayoutSize();
    layoutSize.x    = contentSize.x;

    const Vector2& shadowOffset = mController->GetTextModel()->GetShadowOffset();
    if(shadowOffset.y > Math::MACHINE_EPSILON_1)
    {
      layoutSize.y += shadowOffset.y;
    }

    float outlineWidth = mController->GetTextModel()->GetOutlineWidth();
    layoutSize.y += outlineWidth * 2.0f;
    layoutSize.y = std::min(layoutSize.y, contentSize.y);

    // Calculate the offset for vertical alignment only, as the layout engine will do the horizontal alignment.
    Vector2 alignmentOffset;
    alignmentOffset.x = 0.0f;
    alignmentOffset.y = (contentSize.y - layoutSize.y) * VERTICAL_ALIGNMENT_TABLE[mController->GetVerticalAlignment()];

    const int maxTextureSize = Dali::GetMaxTextureSize();
    if(layoutSize.width > maxTextureSize)
    {
      DALI_LOG_DEBUG_INFO("layoutSize(%f) > maxTextureSize(%d): To guarantee the behavior of Texture::New, layoutSize must not be bigger than maxTextureSize\n", layoutSize.width, maxTextureSize);
      layoutSize.width = maxTextureSize;
    }

    // This affects font rendering quality.
    // It need to be integerized.
    Vector2 visualTransformOffset;
    visualTransformOffset.x = roundf(padding.start + alignmentOffset.x);
    visualTransformOffset.y = roundf(padding.top + alignmentOffset.y);

    mController->SetVisualTransformOffset(visualTransformOffset);

    Property::Map visualTransform;
    visualTransform.Add(Toolkit::Visual::Transform::Property::SIZE, layoutSize)
      .Add(Toolkit::Visual::Transform::Property::SIZE_POLICY, Vector2(Toolkit::Visual::Transform::Policy::ABSOLUTE, Toolkit::Visual::Transform::Policy::ABSOLUTE))
      .Add(Toolkit::Visual::Transform::Property::OFFSET, visualTransformOffset)
      .Add(Toolkit::Visual::Transform::Property::OFFSET_POLICY, Vector2(Toolkit::Visual::Transform::Policy::ABSOLUTE, Toolkit::Visual::Transform::Policy::ABSOLUTE))
      .Add(Toolkit::Visual::Transform::Property::ORIGIN, Toolkit::Align::TOP_BEGIN)
      .Add(Toolkit::Visual::Transform::Property::ANCHOR_POINT, Toolkit::Align::TOP_BEGIN);
    mVisual.SetTransformAndSize(visualTransform, size);

    if(mController->IsAutoScrollEnabled())
    {
      SetUpAutoScrolling();
    }

    if(Dali::Accessibility::IsUp())
    {
      CommonTextUtils::SynchronizeTextAnchorsInParent(Self(), mController, mAnchorActors);
    }

    mTextUpdateNeeded = false;
  }

  if(mController->IsTextFitChanged())
  {
    EmitTextFitChangedSignal();
    mController->SetTextFitChanged(false);
  }
}

void TextLabel::RequestTextRelayout()
{
  RelayoutRequest();
  // Signal that a Relayout may be needed
}

AsyncTextParameters TextLabel::GetAsyncTextParameters(const Async::RequestType requestType, const Vector2& contentSize, const Extents& padding, const Dali::LayoutDirection::Type layoutDirection)
{
  // Logically, all properties of the text label should be passed.

  std::string text;
  mController->GetRawText(text);

  AsyncTextParameters parameters;
  parameters.requestType     = requestType;
  parameters.textWidth       = contentSize.width;
  parameters.textHeight      = contentSize.height;
  parameters.padding         = padding;
  parameters.layoutDirection = layoutDirection;
  parameters.text            = text;

  parameters.maxTextureSize         = Dali::GetMaxTextureSize();
  parameters.fontSize               = mController->GetDefaultFontSize(Text::Controller::POINT_SIZE);
  parameters.textColor              = mController->GetDefaultColor();
  parameters.fontFamily             = mController->GetDefaultFontFamily();
  parameters.fontWeight             = mController->GetDefaultFontWeight();
  parameters.fontWidth              = mController->GetDefaultFontWidth();
  parameters.fontSlant              = mController->GetDefaultFontSlant();
  parameters.isMultiLine            = mController->IsMultiLineEnabled();
  parameters.ellipsis               = mController->IsTextElideEnabled();
  parameters.enableMarkup           = mController->IsMarkupProcessorEnabled();
  parameters.removeFrontInset       = mController->IsRemoveFrontInset();
  parameters.removeBackInset        = mController->IsRemoveBackInset();
  parameters.minLineSize            = mController->GetDefaultLineSize();
  parameters.lineSpacing            = mController->GetDefaultLineSpacing();
  parameters.relativeLineSize       = mController->GetRelativeLineSize();
  parameters.characterSpacing       = mController->GetCharacterSpacing();
  parameters.fontSizeScale          = mController->IsFontSizeScaleEnabled() ? mController->GetFontSizeScale() : 1.f;
  parameters.horizontalAlignment    = mController->GetHorizontalAlignment();
  parameters.verticalAlignment      = mController->GetVerticalAlignment();
  parameters.verticalLineAlignment  = mController->GetVerticalLineAlignment();
  parameters.lineWrapMode           = mController->GetLineWrapMode();
  parameters.layoutDirectionPolicy  = mController->GetMatchLayoutDirection();
  parameters.ellipsisPosition       = mController->GetEllipsisPosition();
  parameters.isUnderlineEnabled     = mController->IsUnderlineEnabled();
  parameters.underlineType          = mController->GetUnderlineType();
  parameters.underlineColor         = mController->GetUnderlineColor();
  parameters.underlineHeight        = mController->GetUnderlineHeight();
  parameters.dashedUnderlineWidth   = mController->GetDashedUnderlineWidth();
  parameters.dashedUnderlineGap     = mController->GetDashedUnderlineGap();
  parameters.isStrikethroughEnabled = mController->IsStrikethroughEnabled();
  parameters.strikethroughColor     = mController->GetStrikethroughColor();
  parameters.strikethroughHeight    = mController->GetStrikethroughHeight();
  parameters.shadowBlurRadius       = mController->GetShadowBlurRadius();
  parameters.shadowColor            = mController->GetShadowColor();
  parameters.shadowOffset           = mController->GetShadowOffset();
  parameters.outlineWidth           = mController->GetOutlineWidth();
  parameters.outlineColor           = mController->GetOutlineColor();
  parameters.outlineBlurRadius      = mController->GetOutlineBlurRadius();
  parameters.outlineOffset          = mController->GetOutlineOffset();
  parameters.isTextFitEnabled       = mController->IsTextFitEnabled();
  parameters.textFitMinSize         = mController->GetTextFitMinSize();
  parameters.textFitMaxSize         = mController->GetTextFitMaxSize();
  parameters.textFitStepSize        = mController->GetTextFitStepSize();
  parameters.isTextFitArrayEnabled  = mController->IsTextFitArrayEnabled();
  parameters.textFitArray           = mController->GetTextFitArray();
  parameters.isAutoScrollEnabled    = mController->IsAutoScrollEnabled();
  parameters.ellipsisMode           = mController->GetEllipsisMode();
  if(parameters.isAutoScrollEnabled || parameters.ellipsisMode == DevelText::Ellipsize::AUTO_SCROLL)
  {
    parameters.autoScrollStopMode  = GetTextScroller()->GetStopMode();
    parameters.autoScrollSpeed     = GetTextScroller()->GetSpeed();
    parameters.autoScrollLoopCount = GetTextScroller()->GetLoopCount();
    parameters.autoScrollLoopDelay = GetTextScroller()->GetLoopDelay();
    parameters.autoScrollGap       = GetTextScroller()->GetGap();
  }
  parameters.cutout                      = mController->IsTextCutout();
  parameters.backgroundWithCutoutEnabled = mController->IsBackgroundWithCutoutEnabled();
  parameters.backgroundColorWithCutout   = mController->GetBackgroundColorWithCutout();
  Property::Map variationsMap;
  mController->GetVariationsMap(variationsMap);
  parameters.variationsMap = variationsMap;
  parameters.renderScale   = mController->GetRenderScale();

  return parameters;
}

void TextLabel::SetUpAutoScrolling()
{
  const Size&                    controlSize     = mController->GetView().GetControlSize();
  const Size                     textNaturalSize = GetNaturalSize().GetVectorXY(); // As relayout of text may not be done at this point natural size is used to get size. Single line scrolling only.
  const Text::CharacterDirection direction       = mController->GetAutoScrollDirection();

  DALI_LOG_INFO(gLogFilter, Debug::General, "TextLabel::SetUpAutoScrolling textNaturalSize[%f,%f] controlSize[%f,%f]\n", textNaturalSize.x, textNaturalSize.y, controlSize.x, controlSize.y);

  if(!mTextScroller)
  {
    DALI_LOG_INFO(gLogFilter, Debug::General, "TextLabel::SetUpAutoScrolling Creating default TextScoller\n");
    // If speed, loopCount or gap not set via property system then will need to create a TextScroller with defaults
    mTextScroller = Text::TextScroller::New(*this);
  }

  // Calculate the actual gap before scrolling wraps.
  int     textPadding = std::max(controlSize.x - textNaturalSize.x, 0.0f);
  float   wrapGap     = std::max(mTextScroller->GetGap(), textPadding);
  Vector2 textureSize = textNaturalSize + Vector2(wrapGap, 0.0f); // Add the gap as a part of the texture

  // Create a texture of the text for scrolling
  Size      verifiedSize   = textureSize;
  const int maxTextureSize = Dali::GetMaxTextureSize();

  //if the texture size width exceed maxTextureSize, modify the visual model size and enabled the ellipsis
  bool actualellipsis = mController->IsTextElideEnabled();
  if(verifiedSize.width > maxTextureSize)
  {
    verifiedSize.width = maxTextureSize;
    if(textNaturalSize.width > maxTextureSize)
    {
      mController->SetTextElideEnabled(true);
      mController->SetAutoScrollMaxTextureExceeded(true);
    }
    GetHeightForWidth(maxTextureSize);
    wrapGap = std::max(maxTextureSize - textNaturalSize.width, 0.0f);
  }

  Text::TypesetterPtr typesetter = Text::Typesetter::New(mController->GetTextModel());

  PixelData data    = typesetter->Render(verifiedSize, mController->GetTextDirection(), Text::Typesetter::RENDER_TEXT_AND_STYLES, true, Pixel::RGBA8888); // ignore the horizontal alignment
  Texture   texture = Texture::New(Dali::TextureType::TEXTURE_2D,
                                 data.GetPixelFormat(),
                                 data.GetWidth(),
                                 data.GetHeight());

#if defined(ENABLE_GPU_MEMORY_PROFILE)
  texture.Upload(data,"TextLabel");
#else
  texture.Upload(data);
#endif

  TextureSet textureSet = TextureSet::New();
  textureSet.SetTexture(0u, texture);

  // Filter mode needs to be set to linear to produce better quality while scaling.
  Sampler sampler = Sampler::New();
  sampler.SetFilterMode(FilterMode::LINEAR, FilterMode::LINEAR);
  sampler.SetWrapMode(Dali::WrapMode::DEFAULT, Dali::WrapMode::REPEAT, Dali::WrapMode::DEFAULT); // Wrap the texture in the x direction
  textureSet.SetSampler(0u, sampler);

  // Set parameters for scrolling
  Renderer renderer = static_cast<Internal::Visual::Base&>(GetImplementation(mVisual)).GetRenderer();
  mTextScroller->SetParameters(Self(), renderer, textureSet, controlSize, verifiedSize, wrapGap, direction, mController->GetHorizontalAlignment(), mController->GetVerticalAlignment());
  mController->SetTextElideEnabled(actualellipsis);
  mController->SetAutoScrollMaxTextureExceeded(false);
}

void TextLabel::AsyncSetupAutoScroll(Text::AsyncTextRenderInfo renderInfo)
{
  // Pure Virtual from AsyncTextInterface
  Size      verifiedSize = renderInfo.size;
  Size      controlSize  = renderInfo.controlSize;
  float     wrapGap      = renderInfo.autoScrollWrapGap;
  PixelData data         = renderInfo.autoScrollPixelData;
  Texture   texture      = Texture::New(Dali::TextureType::TEXTURE_2D, data.GetPixelFormat(), data.GetWidth(), data.GetHeight());
#if defined(ENABLE_GPU_MEMORY_PROFILE)
  texture.Upload(data,"TextLabel");
#else
  texture.Upload(data);
#endif

  TextureSet textureSet = TextureSet::New();
  textureSet.SetTexture(0u, texture);

  // Filter mode needs to be set to linear to produce better quality while scaling.
  Sampler sampler = Sampler::New();
  sampler.SetFilterMode(FilterMode::LINEAR, FilterMode::LINEAR);
  sampler.SetWrapMode(Dali::WrapMode::DEFAULT, Dali::WrapMode::REPEAT, Dali::WrapMode::DEFAULT); // Wrap the texture in the x direction
  textureSet.SetSampler(0u, sampler);

  // Set parameters for scrolling
  Renderer renderer = static_cast<Internal::Visual::Base&>(GetImplementation(mVisual)).GetRenderer();
  mTextScroller->SetParameters(Self(), renderer, textureSet, controlSize, verifiedSize, wrapGap, renderInfo.isTextDirectionRTL, mController->GetHorizontalAlignment(), mController->GetVerticalAlignment());
}

void TextLabel::ScrollingFinished()
{
  // Pure Virtual from TextScroller Interface
  DALI_LOG_INFO(gLogFilter, Debug::General, "TextLabel::ScrollingFinished\n");
  mController->SetAutoScrollEnabled(false);
  mIsAsyncRenderNeeded = true;
  RequestTextRelayout();
}

void TextLabel::AsyncTextFitChanged(float pointSize)
{
  // Pure Virtual from AsyncTextInterface
  DALI_LOG_INFO(gLogFilter, Debug::General, "TextLabel::AsyncTextFitChanged pointSize : %f\n", pointSize);
  if(mController->IsTextFitEnabled())
  {
    mController->SetTextFitPointSize(pointSize);
    EmitTextFitChangedSignal();
  }
}

void TextLabel::AsyncSizeComputed(Text::AsyncTextRenderInfo renderInfo)
{
  switch(renderInfo.requestType)
  {
    case Async::COMPUTE_NATURAL_SIZE:
    {
      DALI_LOG_RELEASE_INFO("Natural size : %f, %f, line count : %d [%p]\n", renderInfo.renderedSize.width, renderInfo.renderedSize.height, renderInfo.lineCount, static_cast<void *>(mController.Get()));
      mAsyncLineCount = renderInfo.lineCount;
      EmitAsyncNaturalSizeComputedSignal(renderInfo.renderedSize.width, renderInfo.renderedSize.height);
      break;
    }
    case Async::COMPUTE_HEIGHT_FOR_WIDTH:
    {
      DALI_LOG_RELEASE_INFO("Height for width : %f, %f, line count : %d [%p]\n", renderInfo.renderedSize.width, renderInfo.renderedSize.height, renderInfo.lineCount, static_cast<void *>(mController.Get()));
      mAsyncLineCount = renderInfo.lineCount;
      EmitAsyncHeightForWidthComputedSignal(renderInfo.renderedSize.width, renderInfo.renderedSize.height);
      break;
    }
    default:
    {
      DALI_LOG_ERROR("Unexpected request type recieved : %d\n", renderInfo.requestType);
      break;
    }
  }
}

void TextLabel::AsyncLoadComplete(Text::AsyncTextRenderInfo renderInfo)
{
  // Pure Virtual from AsyncTextInterface
  DALI_LOG_RELEASE_INFO("Rendered size : %f, %f, line count : %d [%p]\n", renderInfo.renderedSize.width, renderInfo.renderedSize.height, renderInfo.lineCount, static_cast<void *>(mController.Get()));

  // To avoid flickering issues, enable/disable the background visual when async load is completed.
  EnableControlBackground(!mController->IsTextCutout());

  Actor self = Self();

  Extents padding;
  padding = self.GetProperty<Extents>(Toolkit::Control::Property::PADDING);

  if(mIsManualRender)
  {
    mIsManualRender   = false;
    mIsManualRendered = true;
  }

  mManualRendered = renderInfo.manualRendered;
  mAsyncLineCount = renderInfo.lineCount;

  if(renderInfo.isCutout)
  {
    EmitAsyncTextRenderedSignal(renderInfo.renderedSize.width, renderInfo.renderedSize.height);
  }
  else
  {
    EmitAsyncTextRenderedSignal(renderInfo.renderedSize.width + (padding.start + padding.end), renderInfo.renderedSize.height + (padding.top + padding.bottom));
  }
}

void TextLabel::OnLayoutDirectionChanged(Actor actor, LayoutDirection::Type type)
{
  mController->ChangedLayoutDirection();
}

void TextLabel::OnLocaleChanged(std::string locale)
{
  mLocale = locale;
  mController->ResetFontAndStyleData();
}

std::string TextLabel::GetLocale()
{
  return mLocale;
}

void TextLabel::EmitTextFitChangedSignal()
{
  Dali::Toolkit::TextLabel handle(GetOwner());
  mTextFitChangedSignal.Emit(handle);
}

void TextLabel::EmitAsyncTextRenderedSignal(float width, float height)
{
  Dali::Toolkit::TextLabel handle(GetOwner());
  mAsyncTextRenderedSignal.Emit(handle, width, height);
}

void TextLabel::EmitAsyncNaturalSizeComputedSignal(float width, float height)
{
  Dali::Toolkit::TextLabel handle(GetOwner());
  Extents                  padding;
  padding = Self().GetProperty<Extents>(Toolkit::Control::Property::PADDING);
  mAsyncNaturalSizeComputedSignal.Emit(handle, width + (padding.start + padding.end), height + (padding.top + padding.bottom));
}

void TextLabel::EmitAsyncHeightForWidthComputedSignal(float width, float height)
{
  Dali::Toolkit::TextLabel handle(GetOwner());
  Extents                  padding;
  padding = Self().GetProperty<Extents>(Toolkit::Control::Property::PADDING);
  mAsyncHeightForWidthComputedSignal.Emit(handle, width, height + (padding.top + padding.bottom));
}

void TextLabel::OnAccessibilityStatusChanged()
{
  CommonTextUtils::SynchronizeTextAnchorsInParent(Self(), mController, mAnchorActors);
}

TextLabel::TextLabel(ControlBehaviour additionalBehaviour)
: Control(ControlBehaviour(CONTROL_BEHAVIOUR_DEFAULT | additionalBehaviour)),
  mLocale(std::string()),
  mSize(),
  mRenderingBackend(DEFAULT_RENDERING_BACKEND),
  mAsyncLineCount(0),
  mTextUpdateNeeded(false),
  mLastAutoScrollEnabled(false),
  mControlBackgroundEnabled(true),
  mIsAsyncRenderNeeded(false),
  mIsSizeChanged(false),
  mIsManualRender(false),
  mIsManualRendered(false),
  mManualRendered(false)
{
  mLocale = TextAbstraction::GetLocaleFull();
}

TextLabel::~TextLabel()
{
  // This prevents access to the async text interface until the visual is actually destroyed.
  TextVisual::SetAsyncTextInterface(mVisual, nullptr);
  DiscardTextLabelVisual(mVisual);
}

Vector<Vector2> TextLabel::GetTextSize(const uint32_t startIndex, const uint32_t endIndex) const
{
  return mController->GetTextSize(startIndex, endIndex);
}

Vector<Vector2> TextLabel::GetTextPosition(const uint32_t startIndex, const uint32_t endIndex) const
{
  return mController->GetTextPosition(startIndex, endIndex);
}

Rect<float> TextLabel::GetLineBoundingRectangle(const uint32_t lineIndex) const
{
  return mController->GetLineBoundingRectangle(lineIndex);
}

Rect<float> TextLabel::GetCharacterBoundingRectangle(const uint32_t charIndex) const
{
  return mController->GetCharacterBoundingRectangle(charIndex);
}

int TextLabel::GetCharacterIndexAtPosition(float visualX, float visualY) const
{
  return mController->GetCharacterIndexAtPosition(visualX, visualY);
}

Rect<> TextLabel::GetTextBoundingRectangle(uint32_t startIndex, uint32_t endIndex) const
{
  return mController->GetTextBoundingRectangle(startIndex, endIndex);
}

void TextLabel::SetSpannedText(const Text::Spanned& spannedText)
{
  mController->SetSpannedText(spannedText);
}

void TextLabel::SetTextFitArray(const bool enable, std::vector<Toolkit::DevelTextLabel::FitOption>& fitOptions)
{
  if(!enable)
  {
    // If TextFitArray is disabled, MinLineSize shoud be restored to its original size.
    mController->SetDefaultLineSize(mController->GetCurrentLineSize());
  }
  mController->SetTextFitArrayEnabled(enable);
  mController->SetTextFitArray(fitOptions);

  RequestTextRelayout();
  mIsAsyncRenderNeeded = true;
}

std::vector<Toolkit::DevelTextLabel::FitOption>& TextLabel::GetTextFitArray()
{
  return mController->GetTextFitArray();
}

bool TextLabel::IsTextFitArrayEnabled() const
{
  return mController->IsTextFitArrayEnabled();
}

void TextLabel::SetRemoveFrontInset(bool remove)
{
  mController->SetRemoveFrontInset(remove);
}

bool TextLabel::IsRemoveFrontInset() const
{
  return mController->IsRemoveFrontInset();
}

void TextLabel::SetRemoveBackInset(bool remove)
{
  mController->SetRemoveBackInset(remove);
}

bool TextLabel::IsRemoveBackInset() const
{
  return mController->IsRemoveBackInset();
}

void TextLabel::EnableControlBackground(const bool enable)
{
  // Avoid function calls if there is no change.
  if(!DevelControl::GetVisual(*this, Toolkit::Control::Property::BACKGROUND))
  {
    return;
  }

  if(mControlBackgroundEnabled != enable)
  {
    mControlBackgroundEnabled = enable;
    DevelControl::EnableVisual(*this, Toolkit::Control::Property::BACKGROUND, enable);
  }
}

int TextLabel::GetLineCount(float width)
{
  return mController->GetLineCount(width);
}

void TextLabel::RequestAsyncNaturalSize()
{
  Actor                       self = Self();
  Extents                     padding;
  Size                        contentSize     = Size::ZERO;
  Dali::LayoutDirection::Type layoutDirection = mController->GetLayoutDirection(self);

  AsyncTextParameters parameters = GetAsyncTextParameters(Async::COMPUTE_NATURAL_SIZE, contentSize, padding, layoutDirection);
  TextVisual::RequestAsyncSizeComputation(mVisual, parameters);
}

void TextLabel::RequestAsyncHeightForWidth(float width)
{
  Actor                       self = Self();
  Extents                     padding;
  Size                        contentSize(width, 0.0f);
  Dali::LayoutDirection::Type layoutDirection = mController->GetLayoutDirection(self);

  AsyncTextParameters parameters = GetAsyncTextParameters(Async::COMPUTE_HEIGHT_FOR_WIDTH, contentSize, padding, layoutDirection);
  TextVisual::RequestAsyncSizeComputation(mVisual, parameters);
}

void TextLabel::RequestAsyncRenderWithFixedSize(float width, float height)
{
  DALI_LOG_RELEASE_INFO("Request size : %f, %f [%p]\n", width, height, static_cast<void *>(mController.Get()));

  if(mController->GetRenderMode() == DevelTextLabel::Render::SYNC)
  {
    DALI_LOG_DEBUG_INFO("Render mode is sync, return\n");
    return;
  }

  Actor   self = Self();
  Extents padding;
  padding = self.GetProperty<Extents>(Toolkit::Control::Property::PADDING);

  float contentWidth  = std::max(width - (padding.start + padding.end), 0.0f);
  float contentHeight = std::max(height - (padding.top + padding.bottom), 0.0f);
  Size  contentSize(contentWidth, contentHeight);

  Dali::LayoutDirection::Type layoutDirection = mController->GetLayoutDirection(self);
  if(Dali::LayoutDirection::RIGHT_TO_LEFT == layoutDirection)
  {
    std::swap(padding.start, padding.end);
  }

  AsyncTextParameters parameters = GetAsyncTextParameters(Async::RENDER_FIXED_SIZE, contentSize, padding, layoutDirection);
  parameters.manualRender        = true;

  mIsManualRender      = TextVisual::UpdateAsyncRenderer(mVisual, parameters);
  mTextUpdateNeeded    = false;
  mIsAsyncRenderNeeded = false;
}

void TextLabel::RequestAsyncRenderWithFixedWidth(float width, float heightConstraint)
{
  DALI_LOG_RELEASE_INFO("Request width : %f, height constraint : %f [%p]\n", width, heightConstraint, static_cast<void *>(mController.Get()));

  if(mController->GetRenderMode() == DevelTextLabel::Render::SYNC)
  {
    DALI_LOG_DEBUG_INFO("Render mode is sync, return\n");
    return;
  }

  Actor   self = Self();
  Extents padding;
  padding = self.GetProperty<Extents>(Toolkit::Control::Property::PADDING);

  float contentWidth            = std::max(width - (padding.start + padding.end), 0.0f);
  float contentHeightConstraint = std::max(heightConstraint - (padding.top + padding.bottom), 0.0f);
  Size  contentSize(contentWidth, contentHeightConstraint);

  Dali::LayoutDirection::Type layoutDirection = mController->GetLayoutDirection(self);
  if(Dali::LayoutDirection::RIGHT_TO_LEFT == layoutDirection)
  {
    std::swap(padding.start, padding.end);
  }

  AsyncTextParameters parameters = GetAsyncTextParameters(Async::RENDER_FIXED_WIDTH, contentSize, padding, layoutDirection);
  parameters.manualRender        = true;

  mIsManualRender      = TextVisual::UpdateAsyncRenderer(mVisual, parameters);
  mTextUpdateNeeded    = false;
  mIsAsyncRenderNeeded = false;
}

void TextLabel::RequestAsyncRenderWithFixedHeight(float widthConstraint, float height)
{
  DALI_LOG_RELEASE_INFO("Request width constraint : %f, height : %f [%p]\n", widthConstraint, height, static_cast<void *>(mController.Get()));

  if(mController->GetRenderMode() == DevelTextLabel::Render::SYNC)
  {
    DALI_LOG_DEBUG_INFO("Render mode is sync, return\n");
    return;
  }

  Actor   self = Self();
  Extents padding;
  padding = self.GetProperty<Extents>(Toolkit::Control::Property::PADDING);

  float contentWidthConstraint = std::max(widthConstraint - (padding.start + padding.end), 0.0f);
  float contentHeight          = std::max(height - (padding.top + padding.bottom), 0.0f);
  Size  contentSize(contentWidthConstraint, contentHeight);

  Dali::LayoutDirection::Type layoutDirection = mController->GetLayoutDirection(self);
  if(Dali::LayoutDirection::RIGHT_TO_LEFT == layoutDirection)
  {
    std::swap(padding.start, padding.end);
  }

  AsyncTextParameters parameters = GetAsyncTextParameters(Async::RENDER_FIXED_HEIGHT, contentSize, padding, layoutDirection);
  parameters.manualRender        = true;

  mIsManualRender      = TextVisual::UpdateAsyncRenderer(mVisual, parameters);
  mTextUpdateNeeded    = false;
  mIsAsyncRenderNeeded = false;
}

void TextLabel::RequestAsyncRenderWithConstraint(float widthConstraint, float heightConstraint)
{
  DALI_LOG_RELEASE_INFO("Request constraint : %f, %f [%p]\n", widthConstraint, heightConstraint, static_cast<void *>(mController.Get()));

  if(mController->GetRenderMode() == DevelTextLabel::Render::SYNC)
  {
    DALI_LOG_DEBUG_INFO("Render mode is sync, return\n");
    return;
  }

  Actor   self = Self();
  Extents padding;
  padding = self.GetProperty<Extents>(Toolkit::Control::Property::PADDING);

  float contentWidthConstraint  = std::max(widthConstraint - (padding.start + padding.end), 0.0f);
  float contentHeightConstraint = std::max(heightConstraint - (padding.top + padding.bottom), 0.0f);
  Size  contentSize(contentWidthConstraint, contentHeightConstraint);

  Dali::LayoutDirection::Type layoutDirection = mController->GetLayoutDirection(self);
  if(Dali::LayoutDirection::RIGHT_TO_LEFT == layoutDirection)
  {
    std::swap(padding.start, padding.end);
  }

  AsyncTextParameters parameters = GetAsyncTextParameters(Async::RENDER_CONSTRAINT, contentSize, padding, layoutDirection);
  parameters.manualRender        = true;

  mIsManualRender      = TextVisual::UpdateAsyncRenderer(mVisual, parameters);
  mTextUpdateNeeded    = false;
  mIsAsyncRenderNeeded = false;
}

Dali::Property::Index TextLabel::RegisterFontVariationProperty(std::string tag)
{
  if(tag.length() != 4) // Variable tag must be 4-length string.
  {
    DALI_LOG_ERROR("Font Variation Register Failed. The length of tag is not 4.\n");
    return Property::INVALID_INDEX;
  }

  Actor self = Self();

  Property::Map variationsMap;
  mController->GetVariationsMap(variationsMap);

  float variationValue = 0.f;
  auto  tagPtr         = variationsMap.Find(tag);

  if(tagPtr)
  {
    variationValue = tagPtr->Get<float>();
  }

  Dali::Property::Index index = self.RegisterProperty(tag.data(), variationValue);
  if(mVariationIndexMap.find(index) == mVariationIndexMap.end())
  {
    PropertyNotification customFontVariationNotification = self.AddPropertyNotification(index, StepCondition(1.0f));
    // TODO: Make step value customizable by user.
    customFontVariationNotification.NotifySignal().Connect(this, &TextLabel::OnVariationPropertyNotify);

    mVariationIndexMap[index] = tag;
    // TODO: Make UnregisterProperty() to remove tag from mVariationIndexMap.
  }

  return index;
}

void TextLabel::OnVariationPropertyNotify(PropertyNotification& source)
{
  Property::Map map;
  mController->GetVariationsMap(map);

  for(auto& [index, tag] : mVariationIndexMap)
  {
    if(Self().DoesCustomPropertyExist(index))
    {
      float value     = Self().GetCurrentProperty(index).Get<float>();
      map[tag.data()] = std::round(value);
    }
  }

  // Full Variation Update.
  mController->SetVariationsMap(map);
  mIsAsyncRenderNeeded = true;
}

void TextLabel::SetMaskEffect(Toolkit::Control control)
{
  Actor self = Self();
  Toolkit::Control selfControl = Toolkit::Control::DownCast(self);

  // Add control to this component
  self.Add(control);

  MaskEffect maskEffect = MaskEffect::New(control);
  GetImplementation(maskEffect).SetReverseMaskDirection(true);
  selfControl.SetRenderEffect(maskEffect);
}

std::string TextLabel::TextLabelAccessible::GetNameRaw() const
{
  return GetWholeText();
}

Property::Index TextLabel::TextLabelAccessible::GetNamePropertyIndex()
{
  return Toolkit::TextLabel::Property::TEXT;
}

const std::vector<Toolkit::TextAnchor>& TextLabel::TextLabelAccessible::GetTextAnchors() const
{
  auto self = Toolkit::TextLabel::DownCast(Self());

  return Toolkit::GetImpl(self).mAnchorActors;
}

Toolkit::Text::ControllerPtr TextLabel::TextLabelAccessible::GetTextController() const
{
  auto self = Toolkit::TextLabel::DownCast(Self());

  return Toolkit::GetImpl(self).GetTextController();
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
