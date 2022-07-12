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

// HEADER
#include <dali-toolkit/internal/text/controller/text-controller-background-actor.h>

// EXTERNAL INCLUDES
#include <dali/public-api/rendering/renderer.h>

// INTERNAL INCLUDES
#include <dali-toolkit/devel-api/controls/control-depth-index-ranges.h>
#include <dali-toolkit/internal/graphics/builtin-shader-extern-gen.h>
#include <dali-toolkit/internal/text/cursor-helper-functions.h>
#include <dali-toolkit/internal/text/rendering/styles/character-spacing-helper-functions.h>
#include <dali-toolkit/internal/text/text-view.h>

namespace Dali::Toolkit::Text
{
namespace
{
struct BackgroundVertex
{
  Vector2 mPosition; ///< Vertex posiiton
  Vector4 mColor;    ///< Vertex color
};

struct BackgroundMesh
{
  Vector<BackgroundVertex> mVertices; ///< container of vertices
  Vector<unsigned short>   mIndices;  ///< container of indices
};
} // unnamed namespace

Length CalculateBackgroundLineHeight(LineRun lineRun)
{
  Length height = lineRun.ascender + -(lineRun.descender);

  if(lineRun.lineSpacing > 0)
  {
    height += lineRun.lineSpacing;
  }

  return height;
}

Actor CreateControllerBackgroundActor(const View& textView, const VisualModelPtr& textVisualModel, const LogicalModelPtr& textLogicalModel, Shader& textShaderBackground)
{
  // NOTE: Currently we only support background color for left-to-right text.

  Actor actor;

  Length numberOfGlyphs = textView.GetNumberOfGlyphs();
  if(numberOfGlyphs > 0u)
  {
    Vector<GlyphInfo> glyphs;
    glyphs.Resize(numberOfGlyphs);

    Vector<Vector2> positions;
    positions.Resize(numberOfGlyphs);

    // Get the line where the glyphs are laid-out.
    const LineRun* lineRun         = textVisualModel->mLines.Begin();
    float          alignmentOffset = lineRun->alignmentOffset;
    numberOfGlyphs                 = textView.GetGlyphs(glyphs.Begin(),
                                        positions.Begin(),
                                        alignmentOffset,
                                        0u,
                                        numberOfGlyphs);

    glyphs.Resize(numberOfGlyphs);
    positions.Resize(numberOfGlyphs);

    const GlyphInfo* const glyphsBuffer    = glyphs.Begin();
    const Vector2* const   positionsBuffer = positions.Begin();

    BackgroundMesh mesh;
    mesh.mVertices.Reserve(4u * glyphs.Size());
    mesh.mIndices.Reserve(6u * glyphs.Size());

    const Vector2 textSize = textView.GetLayoutSize();

    const float offsetX = alignmentOffset + textSize.width * 0.5f;
    const float offsetY = textSize.height * 0.5f;

    const Vector4* const    backgroundColorsBuffer       = textView.GetBackgroundColors();
    const ColorIndex* const backgroundColorIndicesBuffer = textView.GetBackgroundColorIndices();
    const Vector4&          defaultBackgroundColor       = textVisualModel->IsBackgroundEnabled() ? textVisualModel->GetBackgroundColor() : Color::TRANSPARENT;
    const float             modelCharacterSpacing        = textVisualModel->GetCharacterSpacing();
    Vector<CharacterIndex>& glyphToCharacterMap          = textVisualModel->mGlyphsToCharacters;
    const CharacterIndex*   glyphToCharacterMapBuffer    = glyphToCharacterMap.Begin();
    float                   calculatedAdvance            = 0.f;

    // Get the character-spacing runs.
    const Vector<CharacterSpacingGlyphRun>& characterSpacingGlyphRuns = textVisualModel->GetCharacterSpacingGlyphRuns();

    Vector4   quad;
    uint32_t  numberOfQuads = 0u;
    Length    yLineOffset   = 0;
    Length    prevLineIndex = 0;
    LineIndex lineIndex;

    for(uint32_t i = 0, glyphSize = glyphs.Size(); i < glyphSize; ++i)
    {
      const GlyphInfo& glyph = *(glyphsBuffer + i);

      // Get the background color of the character.
      // The color index zero is reserved for the default background color (i.e. Color::TRANSPARENT)
      const bool       isMarkupBackground       = textView.IsMarkupBackgroundColorSet();
      const ColorIndex backgroundColorIndex     = isMarkupBackground ? *(backgroundColorIndicesBuffer + i) : 0u;
      const bool       isDefaultBackgroundColor = (0u == backgroundColorIndex);
      const Vector4&   backgroundColor          = isDefaultBackgroundColor ? defaultBackgroundColor : *(backgroundColorsBuffer + backgroundColorIndex - 1u);

      lineIndex         = textVisualModel->GetLineOfGlyph(i);
      Length lineHeight = CalculateBackgroundLineHeight(lineRun[lineIndex]);

      if(lineIndex != prevLineIndex)
      {
        yLineOffset += CalculateBackgroundLineHeight(lineRun[prevLineIndex]);

        if(lineRun[prevLineIndex].lineSpacing < 0)
        {
          yLineOffset += lineRun[prevLineIndex].lineSpacing;
        }
      }

      // Only create quads for glyphs with a background color
      if(backgroundColor != Color::TRANSPARENT)
      {
        const float characterSpacing = GetGlyphCharacterSpacing(i, characterSpacingGlyphRuns, modelCharacterSpacing);

        const Vector2 position = *(positionsBuffer + i);
        calculatedAdvance      = GetCalculatedAdvance(*(textLogicalModel->mText.Begin() + (*(glyphToCharacterMapBuffer + i))), characterSpacing, glyph.advance);

        if(i == 0u && glyphSize == 1u) // Only one glyph in the whole text
        {
          quad.x = position.x;
          quad.y = yLineOffset;
          quad.z = quad.x + std::max(calculatedAdvance, glyph.xBearing + glyph.width);
          quad.w = lineHeight;
        }
        else if((lineIndex != prevLineIndex) || (i == 0u)) // The first glyph in the line
        {
          quad.x = position.x;
          quad.y = yLineOffset;
          quad.z = quad.x - glyph.xBearing + calculatedAdvance;
          quad.w = quad.y + lineHeight;
        }
        else if(i == glyphSize - 1u) // The last glyph in the whole text
        {
          quad.x = position.x - glyph.xBearing;
          quad.y = yLineOffset;
          quad.z = quad.x + std::max(calculatedAdvance, glyph.xBearing + glyph.width);
          quad.w = quad.y + lineHeight;
        }
        else // The glyph in the middle of the text
        {
          quad.x = position.x - glyph.xBearing;
          quad.y = yLineOffset;
          quad.z = quad.x + calculatedAdvance;
          quad.w = quad.y + lineHeight;
        }

        BackgroundVertex vertex;

        // Top left
        vertex.mPosition.x = quad.x - offsetX;
        vertex.mPosition.y = quad.y - offsetY;
        vertex.mColor      = backgroundColor;
        mesh.mVertices.PushBack(vertex);

        // Top right
        vertex.mPosition.x = quad.z - offsetX;
        vertex.mPosition.y = quad.y - offsetY;
        vertex.mColor      = backgroundColor;
        mesh.mVertices.PushBack(vertex);

        // Bottom left
        vertex.mPosition.x = quad.x - offsetX;
        vertex.mPosition.y = quad.w - offsetY;
        vertex.mColor      = backgroundColor;
        mesh.mVertices.PushBack(vertex);

        // Bottom right
        vertex.mPosition.x = quad.z - offsetX;
        vertex.mPosition.y = quad.w - offsetY;
        vertex.mColor      = backgroundColor;
        mesh.mVertices.PushBack(vertex);

        // Six indices in counter clockwise winding
        mesh.mIndices.PushBack(1u + 4 * numberOfQuads);
        mesh.mIndices.PushBack(0u + 4 * numberOfQuads);
        mesh.mIndices.PushBack(2u + 4 * numberOfQuads);
        mesh.mIndices.PushBack(2u + 4 * numberOfQuads);
        mesh.mIndices.PushBack(3u + 4 * numberOfQuads);
        mesh.mIndices.PushBack(1u + 4 * numberOfQuads);

        numberOfQuads++;
      }

      if(lineIndex != prevLineIndex)
      {
        prevLineIndex = lineIndex;
      }
    }

    // Only create the background actor if there are glyphs with background color
    if(mesh.mVertices.Count() > 0u)
    {
      Property::Map quadVertexFormat;
      quadVertexFormat["aPosition"] = Property::VECTOR2;
      quadVertexFormat["aColor"]    = Property::VECTOR4;

      VertexBuffer quadVertices = VertexBuffer::New(quadVertexFormat);
      quadVertices.SetData(&mesh.mVertices[0], mesh.mVertices.Size());

      Geometry quadGeometry = Geometry::New();
      quadGeometry.AddVertexBuffer(quadVertices);
      quadGeometry.SetIndexBuffer(&mesh.mIndices[0], mesh.mIndices.Size());

      if(!textShaderBackground)
      {
        textShaderBackground = Shader::New(SHADER_TEXT_CONTROLLER_BACKGROUND_SHADER_VERT, SHADER_TEXT_CONTROLLER_BACKGROUND_SHADER_FRAG);
      }

      Dali::Renderer renderer = Dali::Renderer::New(quadGeometry, textShaderBackground);
      renderer.SetProperty(Dali::Renderer::Property::BLEND_MODE, BlendMode::ON);
      renderer.SetProperty(Dali::Renderer::Property::DEPTH_INDEX, DepthIndex::CONTENT);

      actor = Actor::New();
      actor.SetProperty(Dali::Actor::Property::NAME, "TextBackgroundColorActor");
      actor.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
      actor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
      actor.SetProperty(Actor::Property::SIZE, textSize);
      actor.SetProperty(Actor::Property::COLOR_MODE, USE_OWN_MULTIPLY_PARENT_COLOR);
      actor.AddRenderer(renderer);
    }
  }

  return actor;
}

} // namespace Dali::Toolkit::Text
