/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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

// FILE HEADER
#include <dali-toolkit/public-api/text/script.h>

namespace Dali
{

namespace Toolkit
{

namespace Text
{

namespace
{
const unsigned int CHAR_ZWS  = 0x200B; ///< Zero width space.
const unsigned int CHAR_ZWNJ = 0x200C; ///< Zero width non joiner.
const unsigned int CHAR_ZWJ  = 0x200D; ///< Zero width joiner.
const unsigned int CHAR_LTRM = 0x200E; ///< Left to Right Mark.
const unsigned int CHAR_RTLM = 0x200F; ///< Right to Left Mark.
const unsigned int CHAR_TS   = 0x2009; ///< Thin Space.
} // namespace

Script GetCharacterScript( Character character )
{
  // Latin script:
  // 0x0000 - 0x007f C0 Controls and Basic Latin
  // 0x0080 - 0x00ff C1 Controls and Latin-1 Supplement
  // 0x0100 - 0x017f Latin Extended-A
  // 0x0180 - 0x024f Latin Extended-B
  // 0x0250 - 0x02af IPA Extensions
  // 0x02b0 - 0x02ff Spacing Modifier Letters
  // 0x1d00 - 0x1d7f Phonetic Extensions
  // 0x1d80 - 0x1dbf Phonetic Extensions Supplement
  // 0x1e00 - 0x1eff Latin Extended Additional
  // 0x2070 - 0x209f Superscripts and Subscripts
  // 0x2100 - 0x214f Letterlike symbols
  // 0x2150 - 0x218f Number Forms
  // 0x2c60 - 0x2c7f Latin Extended-C
  // 0xa720 - 0xa7ff Latin Extended-D
  // 0xab30 - 0xab6f Latin Extended-E
  // 0xfb00 - 0xfb4f Alphabetic Presentation Forms
  // 0xff00 - 0xffef Halfwidth and Fullwidth Forms

  // Brahmic scripts:
  // 0x0900 - 0x097f Devanagari
  // 0x0980 - 0x09ff Bengali
  // 0x0a00 - 0x0a7f Gurmukhi
  // 0x0a80 - 0x0aff Gujarati
  // 0x0b00 - 0x0b7f Oriya
  // 0x0b80 - 0x0bff Tamil
  // 0x0c00 - 0x0c7f Telugu
  // 0x0c80 - 0x0cff Kannada
  // 0x0d00 - 0x0d7f Malayalam

  // Sinhala script.
  // 0x0d80 - 0x0dff Sinhala

  // Arabic script.
  // 0x0600 - 0x06ff Arabic
  // 0x0750 - 0x077f Arabic Supplement
  // 0x08A0 - 0x08ff Arabic Extended-A
  // 0xfb50 - 0xfdff Arabic Presentation Forms-A
  // 0xfe70 - 0xfeff Arabic Presentation Forms-B
  // 0x1ee00 - 0x1eeff Arabic Mathematical Alphabetic Symbols

  // CJK and Vietnamese script.
  // 0x2E80 - 0x2eff CJK Radicals Supplement
  // 0x3000 - 0x303f CJK Symbols and Punctuation
  // 0x3200 - 0x32ff Enclosed CJK Letters and Months
  // 0x3400 - 0x4dbf CJK Unified Ideographs Extension A
  // 0x4e00 - 0x62ff CJK Unified Ideographs
  // 0x6300 - 0x77ff CJK Unified Ideographs
  // 0x7800 - 0x8cff CJK Unified Ideographs
  // 0x8d00 - 0x9fff CJK Unified Ideographs
  // 0x20000 - 0x215ff CJK Unified Ideographs Extension B
  // 0x21600 - 0x230ff CJK Unified Ideographs Extension B
  // 0x23100 - 0x245ff CJK Unified Ideographs Extension B
  // 0x24600 - 0x260ff CJK Unified Ideographs Extension B
  // 0x26100 - 0x275ff CJK Unified Ideographs Extension B
  // 0x27600 - 0x290ff CJK Unified Ideographs Extension B
  // 0x29100 - 0x2a6df CJK Unified Ideographs Extension B
  // 2a700-2b73f. CJK Unified Ideographs Extension C
  // 2b740-2b81f. CJK Unified Ideographs Extension D

  // Hangul script
  // 0x1100 - 0x11ff Hangul jamo
  // 0x3130 - 0x318f Hangul Compatibility Jamo
  // 0xa960 - 0xa97f Hangul Jamo Extended-A
  // 0xac00 - 0xd7af Hangul Syllables
  // 0xd7b0 - 0xd7ff Hangul Jamo Extended-B

  // Khmer script
  // 0x1780 - 0x17ff Khmer
  // 0x19e0 - 0x19ff Khmer Symbols

  // Lao script
  // 0x0e80 - 0x0eff Lao

  // Thai script
  // 0x0e00 - 0x0e7f Thai

  // Burmese script
  // 0x1000 - 0x109f Myanmar


  if( character <= 0x0cff )
  {
    if( character <= 0x09ff )
    {
      if( character <= 0x077f )
      {
        if( character <= 0x02ff )
        {
          return TextAbstraction::LATIN;
        }
        if( ( 0x0600 <= character ) && ( character <= 0x06ff ) )
        {
          return TextAbstraction::ARABIC;
        }
        if( ( 0x0750 <= character ) && ( character <= 0x077f ) )
        {
          return TextAbstraction::ARABIC;
        }
      }
      else // > 0x077f
      {
        if( ( 0x08A0 <= character ) && ( character <= 0x08ff ) )
        {
          return TextAbstraction::ARABIC;
        }
        if( ( 0x0900 <= character ) && ( character <= 0x097f ) )
        {
          return TextAbstraction::DEVANAGARI;
        }
        if( ( 0x0980 <= character ) && ( character <= 0x09ff ) )
        {
          return TextAbstraction::BENGALI;
        }
      }
    }
    else // > 0x09ff
    {
      if( character <= 0x0b7f )
      {
        if( ( 0x0a00 <= character ) && ( character <= 0x0a7f ) )
        {
          return TextAbstraction::GURMUKHI;
        }
        if( ( 0x0a80 <= character ) && ( character <= 0x0aff ) )
        {
          return TextAbstraction::GUJARATI;
        }
        if( ( 0x0b00 <= character ) && ( character <= 0x0b7f ) )
        {
          return TextAbstraction::ORIYA;
        }
      }
      else // > 0x0b7f
      {
        if( ( 0x0b80 <= character ) && ( character <= 0x0bff ) )
        {
          return TextAbstraction::TAMIL;
        }
        if( ( 0x0c00 <= character ) && ( character <= 0x0c7f ) )
        {
          return TextAbstraction::TELUGU;
        }
        if( ( 0x0c80 <= character ) && ( character <= 0x0cff ) )
        {
          return TextAbstraction::KANNADA;
        }
      }
    }
  }
  else // > 0x0cff
  {
    if( character <= 0x2c7f )
    {
      if( character <= 0x1eff )
      {
        if( ( 0x0d00 <= character ) && ( character <= 0x0d7f ) )
        {
          return TextAbstraction::MALAYALAM;
        }
        if( ( 0x0d80 <= character ) && ( character <= 0x0dff ) )
        {
          return TextAbstraction::SINHALA;
        }
        if( ( 0x0e00 <= character ) && ( character <= 0x0e7f ) )
        {
          return TextAbstraction::THAI;
        }
        if( ( 0x0e80 <= character ) && ( character <= 0x0eff ) )
        {
          return TextAbstraction::LAO;
        }
        if( ( 0x1000 <= character ) && ( character <= 0x109f ) )
        {
          return TextAbstraction::BURMESE;
        }
        if( ( 0x1100 <= character ) && ( character <= 0x11ff ) )
        {
          return TextAbstraction::HANGUL;
        }
        if( ( 0x1780 <= character ) && ( character <= 0x17ff ) )
        {
          return TextAbstraction::KHMER;
        }
        if( ( 0x19e0 <= character ) && ( character <= 0x19ff ) )
        {
          return TextAbstraction::KHMER;
        }
        if( ( 0x1d00 <= character ) && ( character <= 0x1eff ) )
        {
          return TextAbstraction::LATIN;
        }
      }
      else // > 0x1eff
      {
        if( ( 0x2070 <= character ) && ( character <= 0x209f ) )
        {
          return TextAbstraction::LATIN;
        }
        if( ( 0x2100 <= character ) && ( character <= 0x218f ) )
        {
          return TextAbstraction::LATIN;
        }
        if( ( 0x2c60 <= character ) && ( character <= 0x2c7f ) )
        {
          return TextAbstraction::LATIN;
        }
      }
    }
    else // > 0x2c7f
    {
      if( character <= 0xfdff )
      {
        if( ( 0x2e80 <= character ) && ( character <= 0x2eff ) )
        {
          return TextAbstraction::CJK;
        }
        if( ( 0x3000 <= character ) && ( character <= 0x303f ) )
        {
          return TextAbstraction::CJK;
        }
        if( ( 0x3130 <= character ) && ( character <= 0x318f ) )
        {
          return TextAbstraction::HANGUL;
        }
        if( ( 0x3200 <= character ) && ( character <= 0x32ff ) )
        {
          return TextAbstraction::CJK;
        }
        if( ( 0x3400 <= character ) && ( character <= 0x4dbf ) )
        {
          return TextAbstraction::CJK;
        }
        if( ( 0x4e00 <= character ) && ( character <= 0x62ff ) )
        {
          return TextAbstraction::CJK;
        }
        if( ( 0x6300 <= character ) && ( character <= 0x77ff ) )
        {
          return TextAbstraction::CJK;
        }
        if( ( 0x7800 <= character ) && ( character <= 0x8cff ) )
        {
          return TextAbstraction::CJK;
        }
        if( ( 0x8d00 <= character ) && ( character <= 0x9fff ) )
        {
          return TextAbstraction::CJK;
        }
        if( ( 0xa720 <= character ) && ( character <= 0xa7ff ) )
        {
          return TextAbstraction::LATIN;
        }
        if( ( 0xa960 <= character ) && ( character <= 0xa97f ) )
        {
          return TextAbstraction::HANGUL;
        }
        if( ( 0xab30 <= character ) && ( character <= 0xab6f ) )
        {
          return TextAbstraction::LATIN;
        }
        if( ( 0xac00 <= character ) && ( character <= 0xd7af ) )
        {
          return TextAbstraction::HANGUL;
        }
        if( ( 0xd7b0 <= character ) && ( character <= 0xd7ff ) )
        {
          return TextAbstraction::HANGUL;
        }
        if( ( 0xfb00 <= character ) && ( character <= 0xfb4f ) )
        {
          return TextAbstraction::LATIN;
        }
        if( ( 0xfb50 <= character ) && ( character <= 0xfdff ) )
        {
          return TextAbstraction::ARABIC;
        }
      }
      else // > 0xfdff
      {
        if( ( 0xfe70 <= character ) && ( character <= 0xfeff ) )
        {
          return TextAbstraction::ARABIC;
        }
        if( ( 0xff00 <= character ) && ( character <= 0xffef ) )
        {
          return TextAbstraction::LATIN;
        }
        if( ( 0x1ee00 <= character ) && ( character <= 0x1eeff ) )
        {
          return TextAbstraction::ARABIC;
        }
        if( ( 0x20000 <= character ) && ( character <= 0x215ff ) )
        {
          return TextAbstraction::CJK;
        }
        if( ( 0x21600 <= character ) && ( character <= 0x230ff ) )
        {
          return TextAbstraction::CJK;
        }
        if( ( 0x23100 <= character ) && ( character <= 0x245ff ) )
        {
          return TextAbstraction::CJK;
        }
        if( ( 0x24600 <= character ) && ( character <= 0x260ff ) )
        {
          return TextAbstraction::CJK;
        }
        if( ( 0x26100 <= character ) && ( character <= 0x275ff ) )
        {
          return TextAbstraction::CJK;
        }
        if( ( 0x27600 <= character ) && ( character <= 0x290ff ) )
        {
          return TextAbstraction::CJK;
        }
        if( ( 0x29100 <= character ) && ( character <= 0x2a6df ) )
        {
          return TextAbstraction::CJK;
        }
        if( ( 0x2a700 <= character ) && ( character <= 0x2b73f ) )
        {
          return TextAbstraction::CJK;
        }
        if( ( 0x2b740 <= character ) && ( character <= 0x2b81f ) )
        {
          return TextAbstraction::CJK;
        }
      }
    }
  }

  return TextAbstraction::UNKNOWN;
}

bool IsZeroWidthNonJoiner( Character character )
{
  return CHAR_ZWNJ == character;
}

bool IsZeroWidthJoiner( Character character )
{
  return CHAR_ZWJ == character;
}

bool IsZeroWidthSpace( Character character )
{
  return CHAR_ZWS == character;
}

bool IsLeftToRightMark( Character character )
{
  return CHAR_LTRM == character;
}

bool IsRightToLeftMark( Character character )
{
  return CHAR_RTLM == character;
}

bool IsThinSpace( Character character )
{
  return CHAR_TS == character;
}

} // namespace Text

} // namespace Toolkit

} // namespace Dali
