// FILE HEADER
#include <dali-toolkit/internal/text/characters-helper-functions.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/text/emoji-helper.h>

namespace Dali
{
namespace Toolkit
{
namespace Text
{
CharacterRun RetrieveClusteredCharactersOfCharacterIndex(const VisualModelPtr&  visualModel,
                                                         const LogicalModelPtr& logicalModel,
                                                         const CharacterIndex&  characterIndex)
{
  // Initialization
  CharacterRun clusteredCharacters;
  clusteredCharacters.characterIndex     = characterIndex;
  clusteredCharacters.numberOfCharacters = 1u;

  const GlyphIndex* const     charactersToGlyphBuffer  = visualModel->mCharactersToGlyph.Begin();
  const Length* const         charactersPerGlyphBuffer = visualModel->mCharactersPerGlyph.Begin();
  const CharacterIndex* const glyphsToCharacters       = visualModel->mGlyphsToCharacters.Begin();

  GlyphIndex glyphIndex               = *(charactersToGlyphBuffer + characterIndex);
  Length     actualNumberOfCharacters = *(charactersPerGlyphBuffer + glyphIndex);

  if(actualNumberOfCharacters > 1u)
  {
    const Script script = logicalModel->GetScript(characterIndex);
    // Prevents to break the Latin ligatures like fi, ff, or Arabic ﻻ, ...
    // Keep actual index of character as is. Because these characters cannot be clustered.

    if(!HasLigatureMustBreak(script))
    {
      clusteredCharacters.numberOfCharacters = actualNumberOfCharacters;
      clusteredCharacters.characterIndex     = *(glyphsToCharacters + glyphIndex); // firstCharacterIndex
    }
  }
  else
  {
    while(0u == actualNumberOfCharacters)
    {
      ++glyphIndex;
      actualNumberOfCharacters = *(charactersPerGlyphBuffer + glyphIndex);
    }

    clusteredCharacters.characterIndex     = *(glyphsToCharacters + glyphIndex); // firstCharacterIndex
    clusteredCharacters.numberOfCharacters = actualNumberOfCharacters;
  }

  return clusteredCharacters;
}

} // namespace Text

} // namespace Toolkit

} // namespace Dali
