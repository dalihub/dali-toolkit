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

// CLASS HEADER
#include <dali-toolkit/internal/text/text-io.h>

// EXTERNAL INCLUDES
#include <iostream>
#include <dali/devel-api/text-abstraction/font-client.h>
#include <dali/devel-api/text-abstraction/script.h>

namespace Dali
{

namespace Toolkit
{

namespace Text
{

std::ostream& operator<< (std::ostream& o, const Vector<Character>& text)
{
  o << std::hex;

  for( unsigned int i=0; i<text.Count(); ++i )
  {
    o << text[i];
    if( i+1 < text.Count() )
    {
      o << " ";
    }
  }

  return o << std::dec;
}

std::ostream& operator<< (std::ostream& o, const Vector<ScriptRun>& scriptRun)
{
  for( unsigned int i=0; i<scriptRun.Count(); ++i )
  {
    // e.g. Print "0->9: LATIN" for a ten character run staring from beginning of the model
    o << scriptRun[i].characterRun.characterIndex << "->" << (scriptRun[i].characterRun.characterIndex + scriptRun[i].characterRun.numberOfCharacters ) << ": ";
    o << TextAbstraction::ScriptName[scriptRun[i].script];

    if( i+1 < scriptRun.Count() )
    {
      o << ", ";
    }
  }

  return o << std::dec;
}

std::ostream& operator<< (std::ostream& o, const Vector<FontRun>& fontRun)
{
  TextAbstraction::FontClient fontClient = TextAbstraction::FontClient::Get();

  for( unsigned int i=0; i<fontRun.Count(); ++i )
  {
    // e.g. Print "0->9: ID:1 TizenSansKorean style:Regular size:10.0" for a ten character run staring from beginning of the model
    o << fontRun[i].characterRun.characterIndex << "->" << (fontRun[i].characterRun.characterIndex + fontRun[i].characterRun.numberOfCharacters ) << ": ";

    FontId id = fontRun[i].fontId;
    TextAbstraction::FontDescription fontDescription;
    fontClient.GetDescription( id, fontDescription );
    o << "ID:" << id << ", " << fontDescription.family << " style:" << fontDescription.style << " size:" << (fontClient.GetPointSize(id) / 64);

    if( i+1 < fontRun.Count() )
    {
      o << ", ";
    }
  }

  return o << std::dec;
}

std::ostream& operator<< (std::ostream& o, const Vector<LineRun>& lineRuns)
{
  for( unsigned int i=0; i<lineRuns.Count(); ++i )
  {
    // e.g. Print "Line 0 Glyphs: 0->9 Characters: 0->9 (10)" for a ten character run staring from beginning of the model
    o << "Line " << i << " Glyphs: " << lineRuns[i].glyphRun.glyphIndex << "->" << (lineRuns[i].glyphRun.glyphIndex + lineRuns[i].glyphRun.numberOfGlyphs );
    o << " Characters: " << lineRuns[i].characterRun.characterIndex << "->" << (lineRuns[i].characterRun.characterIndex + lineRuns[i].characterRun.numberOfCharacters );
    o << " Width: " << lineRuns[i].width;
    o << " Ascender: " << lineRuns[i].ascender;
    o << " Descender: " << lineRuns[i].descender;

    if( i+1 < lineRuns.Count() )
    {
      o << ", ";
    }
  }

  return o << std::dec;
}

} // namespace Text

} // namespace Toolkit

} // namespace Dali
