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

#include <dali-toolkit-test-suite-utils.h>
#include <dali-toolkit/internal/text/visual-model-impl.h>
using namespace Dali;
using namespace Dali::Toolkit::Text;
namespace
{
VisualModelPtr MakeMapping()
{
  auto visual = VisualModel::New();
  const CharacterIndex map[] = {0u, 1u, 1u, 3u};
  const Length counts[] = {1u, 0u, 2u, 1u};
  for(unsigned int i = 0u; i < 4u; ++i)
  {
    visual->mGlyphsToCharacters.PushBack(map[i]);
    visual->mCharactersPerGlyph.PushBack(counts[i]);
  }
  return visual;
}
}
int UtcDaliSetGlyphsPerCharacter(void)
{
  auto visual = MakeMapping();
  visual->CreateGlyphsPerCharacterTable(0u, 0u, 0u);
  DALI_TEST_CHECK(visual->mGlyphsPerCharacter.Empty());
  visual->CreateGlyphsPerCharacterTable(0u, 0u, 4u);
  const Length expected[] = {1u, 0u, 2u, 1u};
  DALI_TEST_EQUALS(visual->mGlyphsPerCharacter.Count(), 4u, TEST_LOCATION);
  for(unsigned int i = 0u; i < 4u; ++i) DALI_TEST_EQUALS(visual->mGlyphsPerCharacter[i], expected[i], TEST_LOCATION);
  visual->mGlyphsPerCharacter.Erase(visual->mGlyphsPerCharacter.Begin() + 1u, visual->mGlyphsPerCharacter.Begin() + 3u);
  visual->CreateGlyphsPerCharacterTable(1u, 1u, 2u);
  for(unsigned int i = 0u; i < 4u; ++i) DALI_TEST_EQUALS(visual->mGlyphsPerCharacter[i], expected[i], TEST_LOCATION);
  END_TEST;
}
int UtcDaliSetCharacterToGlyph(void)
{
  auto visual = MakeMapping();
  visual->CreateGlyphsPerCharacterTable(0u, 0u, 4u);
  visual->CreateCharacterToGlyphTable(0u, 0u, 0u);
  DALI_TEST_CHECK(visual->mCharactersToGlyph.Empty());
  visual->CreateCharacterToGlyphTable(0u, 0u, 4u);
  const GlyphIndex expected[] = {0u, 1u, 1u, 3u};
  DALI_TEST_EQUALS(visual->mCharactersToGlyph.Count(), 4u, TEST_LOCATION);
  for(unsigned int i = 0u; i < 4u; ++i) DALI_TEST_EQUALS(visual->mCharactersToGlyph[i], expected[i], TEST_LOCATION);
  visual->mCharactersToGlyph.Erase(visual->mCharactersToGlyph.Begin() + 1u, visual->mCharactersToGlyph.Begin() + 3u);
  visual->mCharactersToGlyph[1] -= 2u;
  visual->CreateCharacterToGlyphTable(1u, 1u, 2u);
  for(unsigned int i = 0u; i < 4u; ++i) DALI_TEST_EQUALS(visual->mCharactersToGlyph[i], expected[i], TEST_LOCATION);
  END_TEST;
}
