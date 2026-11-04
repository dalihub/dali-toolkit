/*
 * Copyright (c) 2026 Samsung Electronics Co., Ltd.
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
 */

#include <dali-toolkit-test-suite-utils.h>
#include <dali-toolkit/internal/text/input-style.h>
#include <dali-toolkit/internal/text/segmentation.h>
#include <dali-toolkit/internal/text/text-io.h>
#include <dali/integration-api/debug.h>
#include <text-model-fixture.h>
#include <toolkit-text-utils.h>
#include <dali-toolkit/internal/text/rendering/view-model.h>
#include <dali-toolkit/internal/text/layouts/layout-parameters.h>
#include <dali-toolkit/internal/text/text-view.h>
#include <toolkit-environment-variable.h>
#include <dali-toolkit/internal/text/rendering/text-typesetter.h>
#include <dali-toolkit/internal/text/cursor-helper-functions.h>
#include <dali-toolkit/public-api/controls/text-controls/text-field.h>
#include <dali-toolkit/internal/controls/text-controls/text-field-impl.h>
#include <dali-toolkit/internal/text/hyphenator.h>
#include <dali-toolkit/internal/text/text-font-style.h>
#include <dali-toolkit/internal/text/text-run-container.h>
#include <dali-toolkit/internal/text/controller/text-controller-impl.h>
#include <dali-toolkit/internal/text/markup-processor/markup-processor-font.h>
#include <dali-toolkit/internal/text/markup-processor/markup-processor-helper-functions.h>
#include <sstream>
#include <cmath>
#include <cstring>
#include <dali-toolkit/public-api/controls/text-controls/text-editor.h>
#include <dali-toolkit/internal/controls/text-controls/text-editor-impl.h>
#include <dali-toolkit/internal/text/glyph-metrics-helper.h>
#include <dali-toolkit/devel-api/text/text-utils-devel.h>

using namespace Dali;
using namespace Dali::Toolkit;
using namespace Dali::Toolkit::Text;

int UtcDaliTextPortableStreamFormatting(void)
{
  ToolkitTestApplication application(480u, 800u, 96.f, 96.f);
  auto model = TextTest::MakeModel(U"abcdefgh");
  std::ostringstream characters;
  characters << model->mLogicalModel->mText << ' ' << 10;
  DALI_TEST_EQUALS(characters.str(), std::string("61 62 63 64 65 66 67 68 10"), TEST_LOCATION);
  auto scripts = model->mLogicalModel->mScriptRuns;
  scripts.PushBack(scripts[0]);
  std::ostringstream scriptStream;
  scriptStream << scripts;
  DALI_TEST_CHECK(scriptStream.str().find("LATIN") != std::string::npos);
  DALI_TEST_CHECK(scriptStream.str().find(", ") != std::string::npos);
  Vector<FontRun> fonts;
  FontRun font;
  font.characterRun = {0u, 4u};
  TextAbstraction::FontDescription description;
  description.family = "Test Sans";
  font.fontId = TextAbstraction::FontClient::Get().GetFontId(description, 12u * 64u);
  fonts.PushBack(font);
  fonts.PushBack(font);
  std::ostringstream fontStream;
  fontStream << fonts;
  DALI_TEST_CHECK(fontStream.str().find("Test Sans") != std::string::npos);
  DALI_TEST_CHECK(fontStream.str().find("size:12") != std::string::npos);
  std::ostringstream lines;
  lines << model->mVisualModel->mLines;
  DALI_TEST_CHECK(lines.str().find("Line 0 Glyphs: 0->4") != std::string::npos);
  DALI_TEST_CHECK(lines.str().find("Line 1 Glyphs: 4->8") != std::string::npos);
  END_TEST;
}

int UtcDaliTextPortableInputStyleCopyAndMask(void)
{
  InputStyle original;
  InputStyle changed;
  DALI_TEST_CHECK(original.Equal(changed));
  DALI_TEST_EQUALS(original.GetInputStyleChangeMask(changed), InputStyle::NONE, TEST_LOCATION);
  changed.textColor = Color::RED;
  changed.familyName = "Test Serif";
  changed.weight = TextAbstraction::FontWeight::BOLD;
  changed.width = TextAbstraction::FontWidth::EXPANDED;
  changed.slant = TextAbstraction::FontSlant::ITALIC;
  changed.size = 24.f;
  changed.lineSpacing = 5.f;
  changed.underlineProperties = "underline";
  changed.shadowProperties = "shadow";
  changed.embossProperties = "emboss";
  changed.outlineProperties = "outline";
  changed.strikethroughProperties = "strike";
  changed.isFamilyDefined = true;
  changed.isSizeDefined = true;
  changed.isStrikethroughDefined = true;
  DALI_TEST_CHECK(!original.Equal(changed));
  DALI_TEST_EQUALS(static_cast<unsigned int>(original.GetInputStyleChangeMask(changed)), 0xfffu, TEST_LOCATION);
  original.Copy(changed);
  DALI_TEST_CHECK(original.Equal(changed));
  DALI_TEST_EQUALS(original.strikethroughProperties, std::string("strike"), TEST_LOCATION);
  DALI_TEST_EQUALS(original.GetInputStyleChangeMask(changed), InputStyle::NONE, TEST_LOCATION);
  END_TEST;
}

int UtcDaliTextPortableSegmentationUpdates(void)
{
  ToolkitTestApplication application;
  auto segmentation = TextAbstraction::Segmentation::Get();
  Vector<Character> text;
  for(auto character : std::u32string(U"ab cd\nef")) text.PushBack(character);
  Vector<LineBreakInfo> lines;
  Vector<WordBreakInfo> words;
  Debug::Filter::SetGlobalLogLevel(Debug::Verbose);
  SetLineBreakInfo(segmentation, text, 0u, text.Count(), lines);
  SetWordBreakInfo(segmentation, text, 0u, text.Count(), words);
  DALI_TEST_EQUALS(lines.Count(), text.Count(), TEST_LOCATION);
  DALI_TEST_EQUALS(words.Count(), text.Count(), TEST_LOCATION);
  DALI_TEST_EQUALS(lines[5], static_cast<LineBreakInfo>(TextAbstraction::LINE_MUST_BREAK), TEST_LOCATION);
  lines.Erase(lines.Begin() + 3u, lines.Begin() + 6u);
  words.Erase(words.Begin() + 3u, words.Begin() + 6u);
  SetLineBreakInfo(segmentation, text, 3u, 3u, lines);
  SetWordBreakInfo(segmentation, text, 3u, 3u, words);
  DALI_TEST_EQUALS(lines.Count(), text.Count(), TEST_LOCATION);
  DALI_TEST_EQUALS(words.Count(), text.Count(), TEST_LOCATION);
  text.Clear();
  lines.Clear();
  words.Clear();
  SetLineBreakInfo(segmentation, text, 0u, 0u, lines);
  SetWordBreakInfo(segmentation, text, 0u, 0u, words);
  DALI_TEST_CHECK(lines.Empty() && words.Empty());
  Debug::Filter::SetGlobalLogLevel(Debug::NoLogging);
  END_TEST;
}

int UtcDaliTextPortableDiagnosticPaths(void)
{
  ToolkitTestApplication application(480u, 800u, 96.f, 96.f);
  Debug::Filter::SetGlobalLogLevel(Debug::Verbose);
  for(const auto& text : {std::string("<span font-size='24' text-color='red'>ab</span> cd"),
                         std::string("abc אבג def\nمرحبا عالم"),
                         std::string("a\nb\nc\n"),
                         std::string("")})
  {
    auto controller = Controller::New();
    ConfigureTextEditor(controller);
    controller->SetMarkupProcessorEnabled(true);
    controller->SetText(text);
    controller->Relayout(Size(64.f, 64.f));
    controller->KeyboardFocusGainEvent();
    controller->SetPrimaryCursorPosition(0u, true);
    controller->TapEvent(1u, 4.f, 4.f);
    controller->TapEvent(2u, 12.f, 4.f);
    controller->SelectWholeText();
    controller->Relayout(Size(64.f, 64.f));
    controller->SelectNone();
    controller->PanEvent(GestureState::STARTED, Vector2(4.f, 4.f));
    controller->PanEvent(GestureState::CONTINUING, Vector2(-4.f, -4.f));
    controller->PanEvent(GestureState::FINISHED, Vector2::ZERO);
    controller->LongPressEvent(GestureState::STARTED, 4.f, 4.f);
    controller->LongPressEvent(GestureState::FINISHED, 4.f, 4.f);
    controller->KeyboardFocusLostEvent();
    DALI_TEST_CHECK(controller->GetTextModel()->GetNumberOfCharacters() <= text.size());
  }
  Debug::Filter::SetGlobalLogLevel(Debug::NoLogging);
  END_TEST;
}

int UtcDaliTextPortableSelectedInputStyles(void)
{
  ToolkitTestApplication application(480u, 800u, 96.f, 96.f);
  for(bool reverse : {false, true})
  {
    auto editor = TextEditor::New();
    editor.SetProperty(Actor::Property::SIZE, Vector2(200.f, 100.f));
    editor.SetProperty(TextEditor::Property::TEXT, "abcdefgh");
    application.GetScene().Add(editor);
    editor.SetKeyInputFocus();
    application.SendNotification();
    application.Render();
    auto controller = GetImpl(editor).GetTextController();
    controller->SelectText(reverse ? 4u : 1u, reverse ? 1u : 4u);
    controller->Relayout(Size(200.f, 100.f));
    DALI_TEST_EQUALS(controller->GetSelectedText(), std::string("bcd"), TEST_LOCATION);
    controller->SetInputFontFamily("Test Serif");
    controller->SetInputFontWeight(TextAbstraction::FontWeight::BOLD);
    controller->SetInputFontWidth(TextAbstraction::FontWidth::EXPANDED);
    controller->SetInputFontSlant(TextAbstraction::FontSlant::ITALIC);
    controller->SetInputFontPointSize(24.f);
    controller->SetInputColor(Color::RED);
    controller->Relayout(Size(200.f, 100.f));
    const auto model = static_cast<const Model*>(controller->GetTextModel());
    DALI_TEST_EQUALS(model->mLogicalModel->mFontDescriptionRuns.Count(), 5u, TEST_LOCATION);
    for(const auto& run : model->mLogicalModel->mFontDescriptionRuns)
    {
      DALI_TEST_EQUALS(run.characterRun.characterIndex, 1u, TEST_LOCATION);
      DALI_TEST_EQUALS(run.characterRun.numberOfCharacters, 3u, TEST_LOCATION);
    }
    DALI_TEST_EQUALS(model->mLogicalModel->mColorRuns.Back().color, Color::RED, TEST_LOCATION);
    DALI_TEST_EQUALS(controller->GetInputFontFamily(), std::string("Test Serif"), TEST_LOCATION);
    DALI_TEST_EQUALS(controller->GetInputFontPointSize(), 24.f, TEST_LOCATION);
    application.GetScene().Remove(editor);
  }
  END_TEST;
}

int UtcDaliTextPortableStyledInsertion(void)
{
  ToolkitTestApplication application(480u, 800u, 96.f, 96.f);
  auto editor = TextEditor::New();
  editor.SetProperty(Actor::Property::SIZE, Vector2(200.f, 100.f));
  editor.SetProperty(TextEditor::Property::TEXT, "abc");
  application.GetScene().Add(editor);
  editor.SetKeyInputFocus();
  application.SendNotification();
  application.Render();
  auto controller = GetImpl(editor).GetTextController();
  controller->SetPrimaryCursorPosition(2u, true);
  controller->Relayout(Size(200.f, 100.f));
  controller->SetInputFontFamily("Test Serif");
  controller->SetInputFontWeight(TextAbstraction::FontWeight::BOLD);
  controller->SetInputFontWidth(TextAbstraction::FontWidth::CONDENSED);
  controller->SetInputFontSlant(TextAbstraction::FontSlant::ITALIC);
  controller->SetInputFontPointSize(18.f);
  controller->SetInputColor(Color::BLUE);
  controller->SetInputEmbossProperties("emboss");
  DALI_TEST_EQUALS(controller->GetInputEmbossProperties(), std::string("emboss"), TEST_LOCATION);
  auto context = Dali::Integration::InputMethodContext::New();
  Dali::Integration::InputMethodContext::EventData event(Dali::Integration::InputMethodContext::COMMIT, Dali::String("Z"), 0, 0);
  controller->OnInputMethodContextEvent(context, event);
  controller->Relayout(Size(200.f, 100.f));
  const auto model = static_cast<const Model*>(controller->GetTextModel());
  DALI_TEST_EQUALS(model->GetNumberOfCharacters(), 4u, TEST_LOCATION);
  DALI_TEST_EQUALS(model->mLogicalModel->mText[2], static_cast<Character>('Z'), TEST_LOCATION);
  const auto& run = model->mLogicalModel->mFontDescriptionRuns.Back();
  DALI_TEST_EQUALS(run.characterRun.characterIndex, 2u, TEST_LOCATION);
  DALI_TEST_CHECK(run.familyDefined && run.weightDefined && run.widthDefined && run.slantDefined && run.sizeDefined);
  DALI_TEST_EQUALS(run.weight, TextAbstraction::FontWeight::BOLD, TEST_LOCATION);
  DALI_TEST_EQUALS(model->mLogicalModel->mColorRuns.Back().color, Color::BLUE, TEST_LOCATION);
  END_TEST;
}

int UtcDaliTextPortableInputStyleFallbacks(void)
{
  ToolkitTestApplication application;
  auto controller = Controller::New();
  controller->SetDefaultFontFamily("Test Serif");
  controller->SetDefaultFontWeight(TextAbstraction::FontWeight::BOLD);
  controller->SetDefaultFontWidth(TextAbstraction::FontWidth::CONDENSED);
  controller->SetDefaultFontSlant(TextAbstraction::FontSlant::ITALIC);
  controller->SetDefaultFontSize(18.f, Controller::POINT_SIZE);
  controller->SetDefaultEmbossProperties("default emboss");
  DALI_TEST_EQUALS(controller->GetInputFontFamily(), std::string("Test Serif"), TEST_LOCATION);
  DALI_TEST_EQUALS(controller->GetInputFontWeight(), TextAbstraction::FontWeight::BOLD, TEST_LOCATION);
  DALI_TEST_EQUALS(controller->GetInputFontWidth(), TextAbstraction::FontWidth::CONDENSED, TEST_LOCATION);
  DALI_TEST_EQUALS(controller->GetInputFontSlant(), TextAbstraction::FontSlant::ITALIC, TEST_LOCATION);
  DALI_TEST_EQUALS(controller->GetInputFontPointSize(), 18.f, TEST_LOCATION);
  DALI_TEST_EQUALS(controller->GetInputEmbossProperties(), std::string("default emboss"), TEST_LOCATION);
  controller->SetInputEmbossProperties("ignored without input");
  DALI_TEST_EQUALS(controller->GetInputEmbossProperties(), std::string("default emboss"), TEST_LOCATION);
  ConfigureTextEditor(controller);
  controller->SetText("abc");
  controller->Relayout(Size(100.f, 100.f));
  controller->KeyboardFocusGainEvent();
  controller->TapEvent(1u, 4.f, 4.f);
  controller->Relayout(Size(100.f, 100.f));
  DALI_TEST_EQUALS(controller->GetInputFontWeight(), TextAbstraction::FontWeight::BOLD, TEST_LOCATION);
  DALI_TEST_EQUALS(controller->GetInputFontWidth(), TextAbstraction::FontWidth::CONDENSED, TEST_LOCATION);
  DALI_TEST_EQUALS(controller->GetInputFontSlant(), TextAbstraction::FontSlant::ITALIC, TEST_LOCATION);
  END_TEST;
}

int UtcDaliTextPortableGlyphGroupMetrics(void)
{
  ToolkitTestApplication application;
  auto client = TextAbstraction::FontClient::Get();
  MetricsPtr metrics = Metrics::New(client);
  GlyphInfo glyphs[2];
  glyphs[0].index = 1u;
  glyphs[0].width = 7.f;
  glyphs[0].height = 20.f;
  glyphs[0].advance = 10.f;
  glyphs[0].xBearing = 2.f;
  glyphs[1] = glyphs[0];
  glyphs[1].advance = 4.f;
  glyphs[1].width = 6.f;
  glyphs[1].xBearing = -15.f;
  const Length charactersPerGlyph[] = {0u, 1u};
  DALI_TEST_EQUALS(GetNumberOfGlyphsOfGroup(0u, 2u, charactersPerGlyph), 2u, TEST_LOCATION);
  GlyphMetrics result;
  GetGlyphsMetrics(0u, 2u, result, glyphs, metrics, 10.f);
  DALI_TEST_EQUALS(result.advance, 14.f, TEST_LOCATION);
  DALI_TEST_EQUALS(result.xBearing, -5.f, TEST_LOCATION);
  DALI_TEST_EQUALS(result.width, 14.f, TEST_LOCATION);
  DALI_TEST_EQUALS(result.fontHeight, 20.f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliTextPortableHandleAndPanEvents(void)
{
  ToolkitTestApplication application(480u, 800u, 96.f, 96.f);
  auto editor = TextEditor::New();
  editor.SetProperty(Actor::Property::SIZE, Vector2(40.f, 32.f));
  editor.SetProperty(TextEditor::Property::TEXT, "abcdefgh ijklmnop qrstuvwx\nabcdefgh ijklmnop\nqrstuvwx");
  application.GetScene().Add(editor);
  editor.SetKeyInputFocus();
  application.SendNotification();
  application.Render();
  auto controller = GetImpl(editor).GetTextController();
  const Size area(40.f, 32.f);
  controller->SetHorizontalScrollEnabled(true);
  controller->SetVerticalScrollEnabled(true);
  for(bool smooth : {false, true})
  {
    controller->SetSmoothHandlePanEnabled(smooth);
    for(auto state : {GestureState::STARTED, GestureState::CONTINUING, GestureState::FINISHED, GestureState::CANCELLED, GestureState::POSSIBLE})
    {
      controller->PanEvent(state, Vector2(-20.f, -20.f));
      controller->Relayout(area);
    }
    for(auto handle : {GRAB_HANDLE, LEFT_SELECTION_HANDLE, RIGHT_SELECTION_HANDLE})
    {
      controller->SelectText(2u, 20u);
      controller->Relayout(area);
      for(auto state : {HANDLE_PRESSED, HANDLE_RELEASED, HANDLE_SCROLLING, HANDLE_STOP_SCROLLING, HANDLE_TAPPED})
      {
        for(float displacement : {-16.f, 0.f, 24.f})
        {
          static_cast<Decorator::ControllerInterface&>(*controller).DecorationEvent(handle, state, displacement, displacement);
          controller->Relayout(area);
          DALI_TEST_CHECK(controller->GetPrimaryCursorPosition() <= controller->GetTextModel()->GetNumberOfCharacters());
        }
      }
    }
  }
  const auto model = static_cast<const Model*>(controller->GetTextModel());
  DALI_TEST_CHECK(std::isfinite(model->mScrollPosition.x) && std::isfinite(model->mScrollPosition.y));
  END_TEST;
}

int UtcDaliTextPortablePixelBufferBoundsAndShadow(void)
{
  ToolkitTestApplication application;
  auto source = PixelBuffer::New(2u, 2u, Pixel::RGBA8888);
  auto destination = PixelBuffer::New(2u, 2u, Pixel::RGBA8888);
  std::memset(source.GetBuffer(), 136, 16u);
  std::memset(destination.GetBuffer(), 17, 16u);
  DevelText::UpdateBuffer(source, destination, 1u, 0u, true);
  for(unsigned int i = 0u; i < 16u; ++i) DALI_TEST_EQUALS(destination.GetBuffer()[i], 17u, TEST_LOCATION);
  DevelText::ShadowParameters parameters;
  parameters.input = source;
  parameters.textColor = Color::RED;
  parameters.color = Color::BLUE;
  parameters.offset = Vector2(1.f, 1.f);
  parameters.blendShadow = true;
  auto shadow = DevelText::CreateShadow(parameters);
  DALI_TEST_CHECK(shadow && shadow.GetWidth() >= source.GetWidth());
  END_TEST;
}

int UtcDaliTextPortableEmptyViewAndModelAccessors(void)
{
  View view;
  DALI_TEST_EQUALS(view.GetControlSize(), Vector2::ZERO, TEST_LOCATION);
  DALI_TEST_EQUALS(view.GetLayoutSize(), Vector2::ZERO, TEST_LOCATION);
  DALI_TEST_EQUALS(view.GetNumberOfGlyphs(), 0u, TEST_LOCATION);
  DALI_TEST_CHECK(!view.GetColors() && !view.GetColorIndices());
  DALI_TEST_CHECK(!view.GetBackgroundColors() && !view.GetBackgroundColorIndices());
  DALI_TEST_CHECK(!view.IsMarkupBackgroundColorSet() && !view.IsUnderlineEnabled());
  DALI_TEST_EQUALS(view.GetTextColor(), Vector4::ZERO, TEST_LOCATION);
  DALI_TEST_EQUALS(view.GetShadowColor(), Vector4::ZERO, TEST_LOCATION);
  DALI_TEST_EQUALS(view.GetShadowOffset(), Vector2::ZERO, TEST_LOCATION);
  DALI_TEST_EQUALS(view.GetUnderlineColor(), Vector4::ZERO, TEST_LOCATION);
  DALI_TEST_EQUALS(view.GetUnderlineHeight(), 0.f, TEST_LOCATION);
  DALI_TEST_CHECK(!view.GetHyphens() && !view.GetHyphenIndices());
  DALI_TEST_EQUALS(view.GetHyphensCount(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(view.GetNumberOfUnderlineRuns(), 0u, TEST_LOCATION);
  DALI_TEST_CHECK(!view.IsMarkupUnderlineSet() && !view.IsMarkupStrikethroughSet());
  DALI_TEST_EQUALS(view.GetOutlineColor(), Vector4::ZERO, TEST_LOCATION);
  DALI_TEST_EQUALS(view.GetOutlineWidth(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(view.GetNumberOfBoundedParagraphRuns(), 0u, TEST_LOCATION);
  DALI_TEST_CHECK(!view.IsCutoutEnabled());
  auto model = TextTest::MakeModel();
  view.SetVisualModel(model->mVisualModel);
  view.SetLogicalModel(model->mLogicalModel);
  DALI_TEST_CHECK(view.GetBoundedParagraphRuns().Empty());
  ViewModel temporary(model.Get());
  DALI_TEST_EQUALS(temporary.GetNumberOfScripts(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(temporary.GetScriptRuns()[0].script, TextAbstraction::LATIN, TEST_LOCATION);
  DALI_TEST_EQUALS(temporary.GetNumberOfBoundedParagraphRuns(), 0u, TEST_LOCATION);
  DALI_TEST_CHECK(temporary.GetBoundedParagraphRuns().Empty());
  DALI_TEST_EQUALS(temporary.GetNumberOfCharacterSpacingGlyphRuns(), 0u, TEST_LOCATION);
  DALI_TEST_CHECK(temporary.GetCharacterDirections().Empty());
  DALI_TEST_CHECK(!model->mLogicalModel->GetCharacterDirection(0u));
  model->mLogicalModel->mCharacterDirections.PushBack(true);
  DALI_TEST_CHECK(model->mLogicalModel->GetCharacterDirection(0u));
  DALI_TEST_CHECK(!model->mLogicalModel->GetCharacterDirection(1u));
  DALI_TEST_EQUALS(model->mLogicalModel->GetNumberOfCharacterSpacingCharacterRuns(), 0u, TEST_LOCATION);
  DALI_TEST_CHECK(model->mLogicalModel->GetCharacterSpacingCharacterRuns().Empty());
  END_TEST;
}

int UtcDaliTextPortableLogicalStyleOverrides(void)
{
  auto model = TextTest::MakeModel();
  FontDescriptionRun run;
  run.characterRun = {1u, 2u};
  run.width = TextAbstraction::FontWidth::CONDENSED;
  run.slant = TextAbstraction::FontSlant::ITALIC;
  run.widthDefined = run.slantDefined = true;
  model->mLogicalModel->mFontDescriptionRuns.PushBack(run);
  InputStyle outside, inside;
  model->mLogicalModel->RetrieveStyle(0u, outside);
  model->mLogicalModel->RetrieveStyle(1u, inside);
  DALI_TEST_CHECK(!outside.isWidthDefined && !outside.isSlantDefined);
  DALI_TEST_CHECK(inside.isWidthDefined && inside.isSlantDefined);
  DALI_TEST_EQUALS(inside.width, TextAbstraction::FontWidth::CONDENSED, TEST_LOCATION);
  DALI_TEST_EQUALS(inside.slant, TextAbstraction::FontSlant::ITALIC, TEST_LOCATION);
  END_TEST;
}

int UtcDaliTextPortableIncrementalTrailingParagraph(void)
{
  ToolkitTestApplication application(480u, 800u, 96.f, 96.f);
  ModelPtr model;
  MetricsPtr metrics;
  Size size;
  LayoutOptions options;
  Vector<FontDescriptionRun> fonts;
  CreateTextModel("a\n", Size(100.f, 100.f), fonts, options, size, model, metrics, false, LineWrap::WORD, false, DevelText::EllipsisPosition::END, 0.f, 0.f);
  auto& lines = model->mVisualModel->mLines;
  if(lines.Count() > 1u) lines.Resize(1u);
  auto client = TextAbstraction::FontClient::Get();
  auto bidi = TextAbstraction::BidirectionalSupport::Get();
  Layout::Engine engine;
  engine.SetMetrics(metrics);
  engine.SetLayout(Layout::Engine::MULTI_LINE_BOX);
  Layout::Parameters parameters(Size(100.f, 100.f), model, client, bidi);
  parameters.isLastNewParagraph = true;
  bool scroll = false;
  engine.LayoutText(parameters, size, false, scroll, false, false, DevelText::EllipsisPosition::END);
  DALI_TEST_EQUALS(lines.Count(), 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(lines[1].characterRun.numberOfCharacters, 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(lines[1].glyphRun.numberOfGlyphs, 0u, TEST_LOCATION);
  DALI_TEST_CHECK(size.height > 16.f);
  engine.LayoutText(parameters, size, false, scroll, false, false, DevelText::EllipsisPosition::END);
  DALI_TEST_EQUALS(lines.Count(), 2u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliTextPortableAtlasFormatsAndCompression(void)
{
  ToolkitTestApplication application(480u, 800u, 96.f, 96.f);
  Debug::Filter::SetGlobalLogLevel(Debug::Verbose);
  for(const auto* policy : {"", "s", "m"})
  {
    EnvironmentVariable::SetTestEnvironmentVariable("DALI_RENDERED_GLYPH_COMPRESS_POLICY", policy);
    for(const auto* family : {"Test Sans", "Test Color", "Test Color RGBA", "Test Color COLRv1"})
    {
      auto editor = TextEditor::New();
      editor.SetProperty(Actor::Property::SIZE, Vector2(200.f, 100.f));
      editor.SetProperty(TextEditor::Property::FONT_FAMILY, family);
      editor.SetProperty(TextEditor::Property::POINT_SIZE, 12.f);
      editor.SetProperty(TextEditor::Property::TEXT, family == std::string("Test Sans") ? "abcd" : "😀😁");
      application.GetScene().Add(editor);
      application.SendNotification();
      application.Render();
      auto controller = GetImpl(editor).GetTextController();
      DALI_TEST_CHECK(controller->GetTextModel()->GetNumberOfGlyphs() > 0u);
      DALI_TEST_CHECK(editor.GetChildCount() > 0u);
      application.GetScene().Remove(editor);
    }
  }
  EnvironmentVariable::SetTestEnvironmentVariable("DALI_RENDERED_GLYPH_COMPRESS_POLICY", "");
  Debug::Filter::SetGlobalLogLevel(Debug::NoLogging);
  END_TEST;
}

int UtcDaliTextPortableTypesetterElisionAndOverhang(void)
{
  ToolkitTestApplication application(480u, 800u, 96.f, 96.f);
  for(const auto& text : {std::string("abcdefghij"), std::string("אבגדהוזחטיכל"),
                         std::string("<background color='red'>abc</background><background color='blue'>def</background> ghijkl"),
                         std::string("a\nb\nc\nd\ne"), std::string("á 😀 x")})
  {
    for(auto position : {DevelText::EllipsisPosition::START, DevelText::EllipsisPosition::MIDDLE, DevelText::EllipsisPosition::END})
    {
      for(float bearing : {-12.f, 0.f, 12.f})
      {
        auto controller = Controller::New();
        ConfigureTextLabel(controller);
        TextAbstraction::FontClient::Get().SetDpi(96u, 96u);
        controller->SetDefaultFontSize(12.f, Controller::POINT_SIZE);
        controller->SetMarkupProcessorEnabled(true);
        controller->SetText(text);
        controller->SetTextElideEnabled(true);
        controller->SetEllipsisPosition(position);
        controller->Relayout(Size(40.f, 32.f));
        const auto model = static_cast<const Model*>(controller->GetTextModel());
        for(auto& glyph : model->mVisualModel->mGlyphs)
        {
          glyph.xBearing = bearing;
          glyph.width += std::abs(bearing);
        }
        Vector<GlyphInfo> viewGlyphs;
        Vector<Vector2> viewPositions;
        viewGlyphs.Resize(model->GetNumberOfGlyphs());
        viewPositions.Resize(model->GetNumberOfGlyphs());
        float lineOffset = 0.f;
        const auto laidOut = controller->GetView().GetGlyphs(viewGlyphs.Begin(), viewPositions.Begin(), lineOffset, 0u, viewGlyphs.Count());
        DALI_TEST_CHECK(laidOut <= viewGlyphs.Count());
        DALI_TEST_CHECK(std::isfinite(lineOffset));
        TypesetterPtr typesetter = Typesetter::New(model);
        for(auto behavior : {Typesetter::RENDER_TEXT_AND_STYLES, Typesetter::RENDER_NO_TEXT, Typesetter::RENDER_MASK})
        {
          auto pixels = typesetter->Render(Size(40.f, 32.f), DevelText::TextDirection::LEFT_TO_RIGHT, behavior);
          DALI_TEST_CHECK(pixels);
          DALI_TEST_EQUALS(pixels.GetWidth(), 40u, TEST_LOCATION);
          DALI_TEST_EQUALS(pixels.GetHeight(), 32u, TEST_LOCATION);
        }
      }
    }
  }
  END_TEST;
}

int UtcDaliTextPortableComplexCursorGroups(void)
{
  ToolkitTestApplication application;
  auto client = TextAbstraction::FontClient::Get();
  MetricsPtr metrics = Metrics::New(client);
  for(Length characters : {1u, 2u})
  {
    auto model = TextTest::MakeModel(U"ابج", 4u, true);
    BidirectionalLineInfoRun bidi{};
    bidi.characterRun = {0u, 3u};
    bidi.direction = true;
    bidi.visualToLogicalMap = static_cast<CharacterIndex*>(std::malloc(3u * sizeof(CharacterIndex)));
    for(unsigned int i = 0u; i < 3u; ++i) bidi.visualToLogicalMap[i] = i;
    model->mLogicalModel->mBidirectionalLineInfo.PushBack(bidi);
    model->mVisualModel->mCharactersPerGlyph[1] = 0u;
    model->mVisualModel->mCharactersPerGlyph[2] = characters;
    for(float x : {-10.f, 0.f, 8.f, 18.f, 28.f, 100.f})
    {
      bool hit = false;
      const auto index = GetClosestCursorIndex(model->mVisualModel, model->mLogicalModel, metrics, x, 5.f, CharacterHitTest::TAP, hit);
      DALI_TEST_CHECK(index <= 3u);
    }
  }
  END_TEST;
}

int UtcDaliTextPortableInputStyleSignalMasks(void)
{
  ToolkitTestApplication application;
  struct Receiver : ConnectionTracker
  {
    void Editor(TextEditor, TextEditor::InputStyle::Mask mask) { editorMask = static_cast<unsigned int>(mask); }
    void Field(TextField, TextField::InputStyle::Mask mask) { fieldMask = static_cast<unsigned int>(mask); }
    unsigned int editorMask{0u};
    unsigned int fieldMask{0u};
  } receiver;
  auto editor = TextEditor::New();
  auto field = TextField::New();
  editor.InputStyleChangedSignal().Connect(&receiver, &Receiver::Editor);
  field.InputStyleChangedSignal().Connect(&receiver, &Receiver::Field);
  const auto allStyles = static_cast<InputStyle::Mask>(0xfffu);
  static_cast<EditableControlInterface&>(GetImpl(editor)).InputStyleChanged(allStyles);
  // Check the legacy style flags supported by TextField's converter.
  const auto fieldStyles = static_cast<InputStyle::Mask>(0x7bfu);
  static_cast<EditableControlInterface&>(GetImpl(field)).InputStyleChanged(fieldStyles);
  DALI_TEST_EQUALS(receiver.editorMask, 0x3ffu, TEST_LOCATION);
  DALI_TEST_EQUALS(receiver.fieldMask, 0xffu, TEST_LOCATION);
  editor.SetProperty(TextEditor::Property::INPUT_EMBOSS, "emboss");
  field.SetProperty(TextField::Property::INPUT_EMBOSS, "emboss");
  DALI_TEST_EQUALS(GetImpl(editor).GetTextController()->GetInputEmbossProperties(), std::string("emboss"), TEST_LOCATION);
  DALI_TEST_EQUALS(GetImpl(field).GetTextController()->GetInputEmbossProperties(), std::string("emboss"), TEST_LOCATION);
  END_TEST;
}

int UtcDaliTextPortableHyphenDictionaryEncodings(void)
{
  ToolkitTestApplication application;
  auto hyphenation = TextAbstraction::Hyphenation::Get();
  for(const auto& word : {std::u32string(U"abcdefghij"), std::u32string(U"αβγδεζηθικ")})
  {
    Vector<Character> characters;
    for(auto character : word) characters.PushBack(character);
    const auto utf8 = GetWordHyphens(hyphenation, characters.Begin(), characters.Count(), "test-utf8");
    const auto utf32 = GetWordHyphens(hyphenation, characters.Begin(), characters.Count(), "test-utf32");
    DALI_TEST_EQUALS(utf8.Count(), word.size(), TEST_LOCATION);
    DALI_TEST_EQUALS(utf32.Count(), word.size(), TEST_LOCATION);
    for(unsigned int i = 0u; i < word.size(); ++i)
    {
      DALI_TEST_EQUALS(utf8[i], utf32[i], TEST_LOCATION);
      DALI_TEST_EQUALS(utf8[i], i == 3u || i == 7u, TEST_LOCATION);
    }
  }
  END_TEST;
}

int UtcDaliTextPortableFontPropertyMetadata(void)
{
  ToolkitTestApplication application;
  auto controller = Controller::New();
  SetFontFamilyProperty(controller, "");
  DALI_TEST_CHECK(controller->GetDefaultFontFamily().empty());
  SetFontFamilyProperty(controller, "{\"family\":\"Test Serif\",\"type\":\"user\"}");
  DALI_TEST_EQUALS(controller->GetDefaultFontFamily(), std::string("Test Serif"), TEST_LOCATION);
  SetFontStyleProperty(controller, "{\"weight\":\"bold\"}", FontStyle::DEFAULT);
  Property::Value style;
  GetFontStyleProperty(controller, style, FontStyle::DEFAULT);
  DALI_TEST_EQUALS(style.Get<Dali::String>(), Dali::String("{\"weight\":\"bold\"}"), TEST_LOCATION);
  auto system = Controller::New();
  system->SetDefaultFontSize(12.f, Controller::POINT_SIZE);
  SetFontFamilyProperty(system, "{\"family\":\"Test System\",\"type\":\"system\"}");
  DALI_TEST_EQUALS(system->GetDefaultFontFamily(), std::string("Test System"), TEST_LOCATION);
  Tag tag{};
  tag.attributes.PushBack({"width", "expanded", 5u, 8u});
  tag.attributes.PushBack({"slant", "italic", 5u, 6u});
  FontDescriptionRun run;
  ProcessFontTag(tag, run);
  DALI_TEST_CHECK(run.widthDefined && run.slantDefined);
  DALI_TEST_EQUALS(run.width, TextAbstraction::FontWidth::EXPANDED, TEST_LOCATION);
  DALI_TEST_EQUALS(run.slant, TextAbstraction::FontSlant::ITALIC, TEST_LOCATION);
  END_TEST;
}

int UtcDaliTextPortableStyleRunInsertionAndRemoval(void)
{
  Vector<ColorRun> runs, removed;
  ColorRun first;
  first.characterRun = {1u, 2u};
  first.color = Color::RED;
  ColorRun second;
  second.characterRun = {6u, 2u};
  second.color = Color::BLUE;
  runs.PushBack(first);
  runs.PushBack(second);
  UpdateCharacterRuns(1u, -2, 10u, runs, removed);
  DALI_TEST_EQUALS(runs.Count(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(removed.Count(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(removed[0].color, Color::RED, TEST_LOCATION);
  DALI_TEST_EQUALS(runs[0].characterRun.characterIndex, 4u, TEST_LOCATION);
  UpdateCharacterRuns(1u, 3, 8u, runs, removed);
  DALI_TEST_EQUALS(runs[0].characterRun.characterIndex, 7u, TEST_LOCATION);
  DALI_TEST_EQUALS(runs[0].characterRun.numberOfCharacters, 2u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliTextPortableControllerWithoutInput(void)
{
  ToolkitTestApplication application;
  Controller::Impl implementation(nullptr, nullptr, nullptr, nullptr);
  implementation.mModel = TextTest::MakeModel(U"  a \n");
  DALI_TEST_EQUALS(implementation.GetNumberOfWhiteSpaces(0u), 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(implementation.GetNumberOfWhiteSpaces(1u), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(implementation.GetNumberOfWhiteSpaces(2u), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(implementation.GetNumberOfWhiteSpaces(3u), 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(implementation.GetNumberOfWhiteSpaces(5u), 0u, TEST_LOCATION);
  DALI_TEST_CHECK(!implementation.ProcessInputEvents());
  DALI_TEST_EQUALS(implementation.GetPrimaryCursorPosition(), 0u, TEST_LOCATION);
  CursorInfo cursor;
  implementation.UpdateCursorPosition(cursor);
  DALI_TEST_CHECK(!implementation.SetPrimaryCursorPosition(0u, true));
  auto empty = TextTest::MakeModel(U"");
  bool matched = true;
  DALI_TEST_EQUALS(GetClosestLine(empty->mVisualModel, 0.f, matched), 0u, TEST_LOCATION);
  DALI_TEST_CHECK(!matched);
  END_TEST;
}
