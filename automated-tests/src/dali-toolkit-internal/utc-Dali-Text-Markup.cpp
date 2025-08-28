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

#include <iostream>

#include <stdlib.h>
#include <limits>

#include <dali-toolkit-test-suite-utils.h>
#include <dali-toolkit/dali-toolkit.h>
#include <dali-toolkit/internal/text/color-run.h>
#include <dali-toolkit/internal/text/font-description-run.h>
#include <dali-toolkit/internal/text/markup-processor/markup-processor-helper-functions.h>
#include <dali-toolkit/internal/text/markup-processor/markup-processor.h>
#include <dali-toolkit/internal/text/text-definitions.h>
#include <dali-toolkit/internal/text/text-io.h>
#include <toolkit-text-utils.h>

using namespace Dali;
using namespace Toolkit;
using namespace Text;

namespace
{
///////////////////////////////////////////////////////////

struct TokenComparisonData
{
  std::string description;
  std::string string1; ///< must be in lower case!!!!
  std::string string2;
  bool        expectedResult;
};

bool TokenComparisonTest(const TokenComparisonData& data)
{
  std::cout << "  testing " << data.description << std::endl;

  const bool result = TokenComparison(data.string1,
                                      data.string2.c_str(),
                                      data.string2.size());

  if(result != data.expectedResult)
  {
    std::cout << "  different conparison result : " << result << ", expected : " << data.expectedResult << std::endl;
    std::cout << "  comparing : [" << data.string1 << "] and [" << data.string2 << "]" << std::endl;

    return false;
  }

  return true;
}

///////////////////////////////////////////////////////////

struct ColorStringToVector4Data
{
  std::string description;
  std::string colorStr;
  Vector4     expectedColor;
};

bool ColorStringToVector4Test(const ColorStringToVector4Data& data)
{
  std::cout << "  testing " << data.description << std::endl;

  Vector4 color;
  ColorStringToVector4(data.colorStr.c_str(), data.colorStr.size(), color);

  if(color != data.expectedColor)
  {
    std::cout << "  different color : " << color << ", expected : " << data.expectedColor << std::endl;
    return false;
  }

  return true;
}

///////////////////////////////////////////////////////////

struct Vector4ToColorStringData
{
  std::string description;
  Vector4     color;
  std::string expectedColorStr;
};

bool Vector4ToColorStringTest(const Vector4ToColorStringData& data)
{
  std::cout << "  testing " << data.description << std::endl;

  std::string colorStr;
  Vector4ToColorString(data.color, colorStr);

  if(colorStr != data.expectedColorStr)
  {
    std::cout << "  different color : [" << colorStr << "], expected : [" << data.expectedColorStr << "]" << std::endl;
    return false;
  }

  return true;
}

///////////////////////////////////////////////////////////

struct StringToVector2Data
{
  std::string description;
  std::string vector2Str;
  Vector2     expectedVector2;
};

bool StringToVector2Test(const StringToVector2Data& data)
{
  std::cout << "  testing " << data.description << std::endl;

  Vector2 vector2;
  StringToVector2(data.vector2Str.c_str(), data.vector2Str.size(), vector2);

  if(vector2 != data.expectedVector2)
  {
    std::cout << "  different vector2 : " << vector2 << ", expected : " << data.expectedVector2 << std::endl;
    return false;
  }

  return true;
}

///////////////////////////////////////////////////////////

struct Vector2ToStringData
{
  std::string description;
  Vector2     vector2;
  std::string expectedVector2Str;
};

bool Vector2ToStringTest(const Vector2ToStringData& data)
{
  std::cout << "  testing " << data.description << std::endl;

  std::string vector2Str;
  Vector2ToString(data.vector2, vector2Str);

  if(vector2Str != data.expectedVector2Str)
  {
    std::cout << "  different vector2 : [" << vector2Str << "], expected : [" << data.expectedVector2Str << "]" << std::endl;
    return false;
  }

  return true;
}

///////////////////////////////////////////////////////////

struct XHTMLEntityToUTF8Data
{
  std::string description;
  std::string xHTMLEntityString;
  std::string expectedString;
};

bool XHTMLEntityToUTF8Test(const XHTMLEntityToUTF8Data& data)
{
  std::cout << "  testing " << data.description << std::endl;

  Vector<ColorRun>                     colorRuns;
  Vector<FontDescriptionRun>           fontRuns;
  Vector<EmbeddedItem>                 items;
  Vector<Anchor>                       anchors;
  Vector<UnderlinedCharacterRun>       underlinedCharacterRuns;
  Vector<ColorRun>                     backgroundColorRuns;
  Vector<StrikethroughCharacterRun>    strikethroughCharacterRuns;
  Vector<BoundedParagraphRun>          boundedParagraphRuns;
  Vector<CharacterSpacingCharacterRun> characterSpacingCharacterRuns;
  MarkupProcessData                    markupProcessData(colorRuns, fontRuns, items, anchors, underlinedCharacterRuns, backgroundColorRuns, strikethroughCharacterRuns, boundedParagraphRuns, characterSpacingCharacterRuns);

  MarkupPropertyData markupPropertyData(Color::MEDIUM_BLUE, Color::DARK_MAGENTA);
  ProcessMarkupString(data.xHTMLEntityString, markupPropertyData, markupProcessData);

  for(Vector<EmbeddedItem>::Iterator it    = items.Begin(),
                                     endIt = items.End();
      it != endIt;
      ++it)
  {
    EmbeddedItem& item = *it;
    delete[] item.url;
  }
  items.Clear();

  if(markupProcessData.markupProcessedText != data.expectedString)
  {
    std::cout << "  different output string : " << markupProcessData.markupProcessedText << ", expected : " << data.expectedString << " " << std::endl;
    return false;
  }

  return true;
}

} // namespace

int UtcDaliTextTokenComparison(void)
{
  tet_infoline(" UtcDaliTextTokenComparison");

  const TokenComparisonData data[] =
    {
      {"void texts",
       "",
       "",
       true},
      {"different size text",
       "hello",
       "world!",
       false},
      {"different texts",
       "hello",
       "world",
       false},
      {"same texts",
       "world",
       "wOrLD",
       true},
      {"some punctuation characters, numbers, ...",
       "hello0123456789.![?]",
       "Hello0123456789.![?]",
       true}

    };
  const unsigned int numberOfTests = 5u;

  for(unsigned int index = 0u; index < numberOfTests; ++index)
  {
    ToolkitTestApplication application;
    if(!TokenComparisonTest(data[index]))
    {
      tet_result(TET_FAIL);
    }
  }

  tet_result(TET_PASS);
  END_TEST;
}

int UtcDaliTextColorStringToVector4(void)
{
  tet_infoline(" UtcDaliTextColorStringToVector4");

  const ColorStringToVector4Data data[] =
    {
      {"black string",
       "bLack",
       Color::BLACK},
      {"white string",
       "White",
       Color::WHITE},
      {"red string",
       "reD",
       Color::RED},
      {"green string",
       "green",
       Color::GREEN},
      {"blue string",
       "blue",
       Color::BLUE},
      {"yellow string",
       "yeLloW",
       Color::YELLOW},
      {"magenta string",
       "MagEnta",
       Color::MAGENTA},
      {"cyan string",
       "CyaN",
       Color::CYAN},
      {"transparent string",
       "transparent",
       Color::TRANSPARENT},
      {"3 component web color",
       "#F00",
       Color::RED},
      {"6 component web color",
       "#fF0000",
       Color::RED},
      {"hex color red (ARGB)",
       "0xffff0000",
       Color::RED},
      {"hex color green (ARGB)",
       "0xFf00FF00",
       Color::GREEN},
      {"undefined color",
       "undefined",
       Vector4::ZERO},
    };
  const unsigned int numberOfTests = 14u;

  for(unsigned int index = 0u; index < numberOfTests; ++index)
  {
    ToolkitTestApplication application;
    if(!ColorStringToVector4Test(data[index]))
    {
      tet_result(TET_FAIL);
    }
  }

  tet_result(TET_PASS);
  END_TEST;
}

int UtcDaliTextVector4ToColorString(void)
{
  tet_infoline(" UtcDaliTextVector4ToColorString");

  const Vector4ToColorStringData data[] =
    {
      {"black color",
       Color::BLACK,
       "black"},
      {"white string",
       Color::WHITE,
       "white"},
      {"red string",
       Color::RED,
       "red"},
      {"green string",
       Color::GREEN,
       "green"},
      {"blue string",
       Color::BLUE,
       "blue"},
      {"yellow string",
       Color::YELLOW,
       "yellow"},
      {
        "magenta string",
        Color::MAGENTA,
        "magenta",
      },
      {"cyan string",
       Color::CYAN,
       "cyan"},
      {"transparent string",
       Color::TRANSPARENT,
       "transparent"},
      {"hex color",
       Vector4(0.4f, 0.5f, 0.6f, 1.f),
       "0xff667f99"},
    };
  const unsigned int numberOfTests = 10u;

  for(unsigned int index = 0u; index < numberOfTests; ++index)
  {
    ToolkitTestApplication application;
    if(!Vector4ToColorStringTest(data[index]))
    {
      tet_result(TET_FAIL);
    }
  }

  tet_result(TET_PASS);
  END_TEST;
}

int UtcDaliTextStringToVector2(void)
{
  tet_infoline(" UtcDaliTextStringToVector2");
  const StringToVector2Data data[] =
    {
      {"void text",
       "",
       Vector2::ZERO},
      {"zero zero",
       "0 0",
       Vector2::ZERO},
      {"five four",
       "5 4",
       Vector2(5.f, 4.f)}};
  const unsigned int numberOfTests = 3u;

  for(unsigned int index = 0u; index < numberOfTests; ++index)
  {
    ToolkitTestApplication application;
    if(!StringToVector2Test(data[index]))
    {
      tet_result(TET_FAIL);
    }
  }

  tet_result(TET_PASS);
  END_TEST;
}

int UtcDaliTextVector2ToString(void)
{
  tet_infoline(" UtcDaliTextVector2ToString");
  const Vector2ToStringData data[] =
    {
      {
        "zero zero",
        Vector2::ZERO,
        "0 0",
      },
      {
        "five four",
        Vector2(5.f, 4.f),
        "5 4",
      }};
  const unsigned int numberOfTests = 2u;

  for(unsigned int index = 0u; index < numberOfTests; ++index)
  {
    ToolkitTestApplication application;
    if(!Vector2ToStringTest(data[index]))
    {
      tet_result(TET_FAIL);
    }
  }

  tet_result(TET_PASS);
  END_TEST;
}

int UtcDaliTextXHTMLEntityToUTF8(void)
{
  tet_infoline(" UtcDaliTextXHTMLEntityToUTF8");
  const XHTMLEntityToUTF8Data data[] =
    {
      {"Text Without XHTML Entity",
       "Checking XHTML Entitities",
       "Checking XHTML Entitities"},
      {"Text With XHTML Entity in Numeric form",
       "Checking Numeric Entitities &#x26; &#x27; &#x3C; &#x3E; &#xA1; &#xA2; &#xA3; &#xA4; &#xA5; &#xA6; &#xA7; &#xA8; &#xA9; &#xAA; &#xAB; &#xAC; &#xAD; &#xAE; &#xAF; &#xB0; &#xB1; &#xB2; &#xB3; &#xB4; &#xB5; &#xB6; &#xB7; &#xB8; &#xB9; &#xBA; &#xBB; &#xBC; &#xBD; &#xBE; &#xBF; &#xC0; &#xC1; &#xC2; &#xC3; &#xC4; &#xC5; &#xE6; &#xC7; &#xC8; &#xC9; &#xCA; &#xCB; &#xCC; &#xCD; &#xCE; &#xCF; &#xF0; &#xD1; &#xD2; &#xD3; &#xD4; &#xD5; &#xD6; &#xD7; &#xD8; &#xD9; &#xDA; &#xDB; &#xDD; &#xFE; &#xDF; &#xE0; &#xE1; &#xE2; &#xE3; &#xE4; &#xE5; &#xE6; &#xE7; &#xE8; &#xE9; &#xEA; &#xEB; &#xEC; &#xED; &#xEE; &#xEF; &#xF0; &#xF1; &#xF2; &#xF3; &#xF4; &#xF5; &#xF6; &#xF7; &#xF8; &#xF9; &#xFA; &#xFB; &#xFC; &#xFD; &#xFE; &#xFF; &#x3B1; &#x3B2; &#x3B3; &#x3B4; &#x3B5; &#x3B6; &#x3B7; &#x3B8; &#x3B9; &#x3BA; &#x3BB; &#x3BC; &#x3BD; &#x3BE; &#x3BF; &#x3C0; &#x3C1; &#x3C3; &#x3C4; &#x3C5; &#x3C6; &#x3C7; &#x3C8; &#x3C9; &#x2026; &#x20AC; &#x2190; &#x2191; &#x2192; &#x2193; &#x2194; &#x2190; &#x2192; &#x2200; &#x2203; &#x2207; &#x220F; &#x2211; &#x2227; &#x2228; &#x222B; &#x2260; &#x2261; &#x2295; &#x22A5; &#x2020; &#x2021; &#x2022; ",
       "Checking Numeric Entitities & ' < > ¡ ¢ £ ¤ ¥ ¦ § ¨ © ª « ¬ ­ ® ¯ ° ± ² ³ ´ µ ¶ · ¸ ¹ º » ¼ ½ ¾ ¿ À Á Â Ã Ä Å æ Ç È É Ê Ë Ì Í Î Ï ð Ñ Ò Ó Ô Õ Ö × Ø Ù Ú Û Ý þ ß à á â ã ä å æ ç è é ê ë ì í î ï ð ñ ò ó ô õ ö ÷ ø ù ú û ü ý þ ÿ α β γ δ ε ζ η θ ι κ λ μ ν ξ ο π ρ σ τ υ φ χ ψ ω … € ← ↑ → ↓ ↔ ← → ∀ ∃ ∇ ∏ ∑ ∧ ∨ ∫ ≠ ≡ ⊕ ⊥ † ‡ • "},
      {"Text With XHTML Named Entities",
       "Checking Named Entitities &amp; &apos; &lt; &gt; &iexcl; &cent; &pound; &curren; &yen; &brvbar; &sect; &uml; &copy; &ordf; &laquo; &not; &shy; &reg; &macr; &deg; &plusmn; &sup2; &sup3; &acute; &micro; &para; &middot; &cedil; &sup1; &ordm; &raquo; &frac14; &frac12; &frac34; &iquest; &Agrave; &Aacute; &Acirc; &Atilde; &Auml; &Aring; &aelig; &Ccedil; &Egrave; &Eacute; &Ecirc; &Euml; &Igrave; &Iacute; &Icirc; &Iuml; &eth; &Ntilde; &Ograve; &Oacute; &Ocirc; &Otilde; &Ouml; &times; &Oslash; &Ugrave; &Uacute; &Ucirc; &Yacute; &thorn; &szlig; &agrave; &aacute; &acirc; &atilde; &auml; &aring; &aelig; &ccedil; &egrave; &eacute; &ecirc; &euml; &igrave; &iacute; &icirc; &iuml; &eth; &ntilde; &ograve; &oacute; &ocirc; &otilde; &ouml; &divide; &oslash; &ugrave; &uacute; &ucirc; &uuml; &yacute; &thorn; &yuml; &alpha; &beta; &gamma; &delta; &epsilon; &zeta; &eta; &theta; &iota; &kappa; &lambda; &mu; &nu; &xi; &omicron; &pi; &rho; &sigma; &tau; &upsilon; &phi; &chi; &psi; &omega; &hellip; &euro; &larr; &uarr; &rarr; &darr; &harr; &larr; &rarr; &forall; &exist; &nabla; &prod; &sum; &and; &or; &int; &ne; &equiv; &oplus; &perp; &dagger; &Dagger; &bull; ",
       "Checking Named Entitities & ' < > ¡ ¢ £ ¤ ¥ ¦ § ¨ © ª « ¬ ­ ® ¯ ° ± ² ³ ´ µ ¶ · ¸ ¹ º » ¼ ½ ¾ ¿ À Á Â Ã Ä Å æ Ç È É Ê Ë Ì Í Î Ï ð Ñ Ò Ó Ô Õ Ö × Ø Ù Ú Û Ý þ ß à á â ã ä å æ ç è é ê ë ì í î ï ð ñ ò ó ô õ ö ÷ ø ù ú û ü ý þ ÿ α β γ δ ε ζ η θ ι κ λ μ ν ξ ο π ρ σ τ υ φ χ ψ ω … € ← ↑ → ↓ ↔ ← → ∀ ∃ ∇ ∏ ∑ ∧ ∨ ∫ ≠ ≡ ⊕ ⊥ † ‡ • "},
      {"Testing of < special character",
       "Testing of < special character",
       "Testing of "},
      {"Testing of & special character",
       "Testing of & special character",
       "Testing of "},
      {"Testing of & < > special character",
       "Testing of \\& \\< \\> special character",
       "Testing of & < > special character"},
      {"Testing of Legacy XHTML Named Entities",
        "Checking Named Entitities &Aelig;",
        "Checking Named Entitities Æ"}};
  const unsigned int numberOfTests = 7u;

  for(unsigned int index = 0u; index < numberOfTests; ++index)
  {
    ToolkitTestApplication application;
    if(!XHTMLEntityToUTF8Test(data[index]))
    {
      tet_result(TET_FAIL);
    }
  }

  tet_result(TET_PASS);
  END_TEST;
}
