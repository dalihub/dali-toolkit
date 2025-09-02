/*
 * Copyright (c) 2021 Samsung Electronics Co., Ltd.
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

// EXTERNAL INCLUDES
#include <cstring> // for strlen()

// FILE HEADER
#include "xhtml-entities.h"

namespace Dali
{
namespace Toolkit
{
namespace Text
{
namespace
{
/**
 * Implementation of the XHTML Entity matching
 */
struct XHTMLEntityLookup
{
  const char* const entityName; // XHTML Named Entity string
  const char* const entityCode; // Corresponding UTF-8
};

/* table of html name entities supported in DALi
 *
 * these are stored as pair with Named entity as Key and
 * its utf 8 as value
 */
// clang-format off
const XHTMLEntityLookup XHTMLEntityLookupTable[] =
{
  {"&quot;\0"    ,"\x22\0"         },
  {"&amp;\0"     ,"\x26\0"         },
  {"&apos;\0"    ,"\x27\0"         },
  {"&lt;\0"      ,"\x3c\0"         },
  {"&gt;\0"      ,"\x3e\0"         },
  {"&nbsp;\0"    ,"\xc2\xa0\0"     },
  {"&iexcl;\0"   ,"\xc2\xa1\0"     },
  {"&cent;\0"    ,"\xc2\xa2\0"     },
  {"&pound;\0"   ,"\xc2\xa3\0"     },
  {"&curren;\0"  ,"\xc2\xa4\0"     },
  {"&yen;\0"     ,"\xc2\xa5\0"     },
  {"&brvbar;\0"  ,"\xc2\xa6\0"     },
  {"&sect;\0"    ,"\xc2\xa7\0"     },
  {"&uml;\0"     ,"\xc2\xa8\0"     },
  {"&copy;\0"    ,"\xc2\xa9\0"     },
  {"&ordf;\0"    ,"\xc2\xaa\0"     },
  {"&laquo;\0"   ,"\xc2\xab\0"     },
  {"&not;\0"     ,"\xc2\xac\0"     },
  {"&shy;\0"     ,"\xc2\xad\0"     },
  {"&reg;\0"     ,"\xc2\xae\0"     },
  {"&macr;\0"    ,"\xc2\xaf\0"     },
  {"&deg;\0"     ,"\xc2\xb0\0"     },
  {"&plusmn;\0"  ,"\xc2\xb1\0"     },
  {"&sup2;\0"    ,"\xc2\xb2\0"     },
  {"&sup3;\0"    ,"\xc2\xb3\0"     },
  {"&acute;\0"   ,"\xc2\xb4\0"     },
  {"&micro;\0"   ,"\xc2\xb5\0"     },
  {"&para;\0"    ,"\xc2\xb6\0"     },
  {"&middot;\0"  ,"\xc2\xb7\0"     },
  {"&cedil;\0"   ,"\xc2\xb8\0"     },
  {"&sup1;\0"    ,"\xc2\xb9\0"     },
  {"&ordm;\0"    ,"\xc2\xba\0"     },
  {"&raquo;\0"   ,"\xc2\xbb\0"     },
  {"&frac14;\0"  ,"\xc2\xbc\0"     },
  {"&frac12;\0"  ,"\xc2\xbd\0"     },
  {"&frac34;\0"  ,"\xc2\xbe\0"     },
  {"&iquest;\0"  ,"\xc2\xbf\0"     },
  {"&Agrave;\0"  ,"\xc3\x80\0"     },
  {"&Aacute;\0"  ,"\xc3\x81\0"     },
  {"&Acirc;\0"   ,"\xc3\x82\0"     },
  {"&Atilde;\0"  ,"\xc3\x83\0"     },
  {"&Auml;\0"    ,"\xc3\x84\0"     },
  {"&Aring;\0"   ,"\xc3\x85\0"     },
  {"&AElig;\0"   ,"\xc3\x86\0"     },
  {"&Ccedil;\0"  ,"\xc3\x87\0"     },
  {"&Egrave;\0"  ,"\xc3\x88\0"     },
  {"&Eacute;\0"  ,"\xc3\x89\0"     },
  {"&Ecirc;\0"   ,"\xc3\x8a\0"     },
  {"&Euml;\0"    ,"\xc3\x8b\0"     },
  {"&Igrave;\0"  ,"\xc3\x8c\0"     },
  {"&Iacute;\0"  ,"\xc3\x8d\0"     },
  {"&Icirc;\0"   ,"\xc3\x8e\0"     },
  {"&Iuml;\0"    ,"\xc3\x8f\0"     },
  {"&ETH;\0"     ,"\xc3\x90\0"     },
  {"&Ntilde;\0"  ,"\xc3\x91\0"     },
  {"&Ograve;\0"  ,"\xc3\x92\0"     },
  {"&Oacute;\0"  ,"\xc3\x93\0"     },
  {"&Ocirc;\0"   ,"\xc3\x94\0"     },
  {"&Otilde;\0"  ,"\xc3\x95\0"     },
  {"&Ouml;\0"    ,"\xc3\x96\0"     },
  {"&times;\0"   ,"\xc3\x97\0"     },
  {"&Oslash;\0"  ,"\xc3\x98\0"     },
  {"&Ugrave;\0"  ,"\xc3\x99\0"     },
  {"&Uacute;\0"  ,"\xc3\x9a\0"     },
  {"&Ucirc;\0"   ,"\xc3\x9b\0"     },
  {"&Uuml;\0"    ,"\xc3\x9c\0"     },
  {"&Yacute;\0"  ,"\xc3\x9d\0"     },
  {"&THORN;\0"   ,"\xc3\x9e\0"     },
  {"&szlig;\0"   ,"\xc3\x9f\0"     },
  {"&agrave;\0"  ,"\xc3\xa0\0"     },
  {"&aacute;\0"  ,"\xc3\xa1\0"     },
  {"&acirc;\0"   ,"\xc3\xa2\0"     },
  {"&atilde;\0"  ,"\xc3\xa3\0"     },
  {"&auml;\0"    ,"\xc3\xa4\0"     },
  {"&aring;\0"   ,"\xc3\xa5\0"     },
  {"&aelig;\0"   ,"\xc3\xa6\0"     },
  {"&ccedil;\0"  ,"\xc3\xa7\0"     },
  {"&egrave;\0"  ,"\xc3\xa8\0"     },
  {"&eacute;\0"  ,"\xc3\xa9\0"     },
  {"&ecirc;\0"   ,"\xc3\xaa\0"     },
  {"&euml;\0"    ,"\xc3\xab\0"     },
  {"&igrave;\0"  ,"\xc3\xac\0"     },
  {"&iacute;\0"  ,"\xc3\xad\0"     },
  {"&icirc;\0"   ,"\xc3\xae\0"     },
  {"&iuml;\0"    ,"\xc3\xaf\0"     },
  {"&eth;\0"     ,"\xc3\xb0\0"     },
  {"&ntilde;\0"  ,"\xc3\xb1\0"     },
  {"&ograve;\0"  ,"\xc3\xb2\0"     },
  {"&oacute;\0"  ,"\xc3\xb3\0"     },
  {"&ocirc;\0"   ,"\xc3\xb4\0"     },
  {"&otilde;\0"  ,"\xc3\xb5\0"     },
  {"&ouml;\0"    ,"\xc3\xb6\0"     },
  {"&divide;\0"  ,"\xc3\xb7\0"     },
  {"&oslash;\0"  ,"\xc3\xb8\0"     },
  {"&ugrave;\0"  ,"\xc3\xb9\0"     },
  {"&uacute;\0"  ,"\xc3\xba\0"     },
  {"&ucirc;\0"   ,"\xc3\xbb\0"     },
  {"&uuml;\0"    ,"\xc3\xbc\0"     },
  {"&yacute;\0"  ,"\xc3\xbd\0"     },
  {"&thorn;\0"   ,"\xc3\xbe\0"     },
  {"&yuml;\0"    ,"\xc3\xbf\0"     },
  {"&OElig;\0"   ,"\xc5\x92\0"     },
  {"&oelig;\0"   ,"\xc5\x93\0"     },
  {"&Scaron;\0"  ,"\xc5\xa0\0"     },
  {"&scaron;\0"  ,"\xc5\xa1\0"     },
  {"&Yuml;\0"    ,"\xc5\xb8\0"     },
  {"&fnof;\0"    ,"\xc6\x92\0"     },
  {"&circ;\0"    ,"\xcb\x86\0"     },
  {"&tilde;\0"   ,"\xcb\x9c\0"     },
  {"&Alpha;\0"   ,"\xce\x91\0"     },
  {"&Beta;\0"    ,"\xce\x92\0"     },
  {"&Gamma;\0"   ,"\xce\x93\0"     },
  {"&Delta;\0"   ,"\xce\x94\0"     },
  {"&Epsilon;\0" ,"\xce\x95\0"     },
  {"&Zeta;\0"    ,"\xce\x96\0"     },
  {"&Eta;\0"     ,"\xce\x97\0"     },
  {"&Theta;\0"   ,"\xce\x98\0"     },
  {"&Iota;\0"    ,"\xce\x99\0"     },
  {"&Kappa;\0"   ,"\xce\x9a\0"     },
  {"&Lambda;\0"  ,"\xce\x9b\0"     },
  {"&Mu;\0"      ,"\xce\x9c\0"     },
  {"&Nu;\0"      ,"\xce\x9d\0"     },
  {"&Xi;\0"      ,"\xce\x9e\0"     },
  {"&Omicron;\0" ,"\xce\x9f\0"     },
  {"&Pi;\0"      ,"\xce\xa0\0"     },
  {"&Rho;\0"     ,"\xce\xa1\0"     },
  {"&Sigma;\0"   ,"\xce\xa3\0"     },
  {"&Tau;\0"     ,"\xce\xa4\0"     },
  {"&Upsilon;\0" ,"\xce\xa5\0"     },
  {"&Phi;\0"     ,"\xce\xa6\0"     },
  {"&Chi;\0"     ,"\xce\xa7\0"     },
  {"&Psi;\0"     ,"\xce\xa8\0"     },
  {"&Omega;\0"   ,"\xce\xa9\0"     },
  {"&alpha;\0"   ,"\xce\xb1\0"     },
  {"&beta;\0"    ,"\xce\xb2\0"     },
  {"&gamma;\0"   ,"\xce\xb3\0"     },
  {"&delta;\0"   ,"\xce\xb4\0"     },
  {"&epsilon;\0" ,"\xce\xb5\0"     },
  {"&zeta;\0"    ,"\xce\xb6\0"     },
  {"&eta;\0"     ,"\xce\xb7\0"     },
  {"&theta;\0"   ,"\xce\xb8\0"     },
  {"&iota;\0"    ,"\xce\xb9\0"     },
  {"&kappa;\0"   ,"\xce\xba\0"     },
  {"&lambda;\0"  ,"\xce\xbb\0"     },
  {"&mu;\0"      ,"\xce\xbc\0"     },
  {"&nu;\0"      ,"\xce\xbd\0"     },
  {"&xi;\0"      ,"\xce\xbe\0"     },
  {"&omicron;\0" ,"\xce\xbf\0"     },
  {"&pi;\0"      ,"\xcf\x80\0"     },
  {"&rho;\0"     ,"\xcf\x81\0"     },
  {"&sigmaf;\0"  ,"\xcf\x82\0"     },
  {"&sigma;\0"   ,"\xcf\x83\0"     },
  {"&tau;\0"     ,"\xcf\x84\0"     },
  {"&upsilon;\0" ,"\xcf\x85\0"     },
  {"&phi;\0"     ,"\xcf\x86\0"     },
  {"&chi;\0"     ,"\xcf\x87\0"     },
  {"&psi;\0"     ,"\xcf\x88\0"     },
  {"&omega;\0"   ,"\xcf\x89\0"     },
  {"&thetasym;\0","\xcf\x91\0"     },
  {"&upsih;\0"   ,"\xcf\x92\0"     },
  {"&piv;\0"     ,"\xcf\x96\0"     },
  {"&ensp;\0"    ,"\xe2\x80\x82\0" },
  {"&emsp;\0"    ,"\xe2\x80\x83\0" },
  {"&thinsp;\0"  ,"\xe2\x80\x89\0" },
  {"&zwnj;\0"    ,"\xe2\x80\x8c\0" },
  {"&zwj;\0"     ,"\xe2\x80\x8d\0" },
  {"&lrm;\0"     ,"\xe2\x80\x8e\0" },
  {"&rlm;\0"     ,"\xe2\x80\x8f\0" },
  {"&ndash;\0"   ,"\xe2\x80\x93\0" },
  {"&mdash;\0"   ,"\xe2\x80\x94\0" },
  {"&lsquo;\0"   ,"\xe2\x80\x98\0" },
  {"&rsquo;\0"   ,"\xe2\x80\x99\0" },
  {"&sbquo;\0"   ,"\xe2\x80\x9a\0" },
  {"&ldquo;\0"   ,"\xe2\x80\x9c\0" },
  {"&rdquo;\0"   ,"\xe2\x80\x9d\0" },
  {"&bdquo;\0"   ,"\xe2\x80\x9e\0" },
  {"&dagger;\0"  ,"\xe2\x80\xa0\0" },
  {"&Dagger;\0"  ,"\xe2\x80\xa1\0" },
  {"&bull;\0"    ,"\xe2\x80\xa2\0" },
  {"&hellip;\0"  ,"\xe2\x80\xa6\0" },
  {"&permil;\0"  ,"\xe2\x80\xb0\0" },
  {"&prime;\0"   ,"\xe2\x80\xb2\0" },
  {"&Prime;\0"   ,"\xe2\x80\xb3\0" },
  {"&lsaquo;\0"  ,"\xe2\x80\xb9\0" },
  {"&rsaquo;\0"  ,"\xe2\x80\xba\0" },
  {"&oline;\0"   ,"\xe2\x80\xbe\0" },
  {"&frasl;\0"   ,"\xe2\x81\x84\0" },
  {"&euro;\0"    ,"\xe2\x82\xac\0" },
  {"&image;\0"   ,"\xe2\x84\x91\0" },
  {"&weierp;\0"  ,"\xe2\x84\x98\0" },
  {"&real;\0"    ,"\xe2\x84\x9c\0" },
  {"&trade;\0"   ,"\xe2\x84\xa2\0" },
  {"&alefsym;\0" ,"\xe2\x84\xb5\0" },
  {"&larr;\0"    ,"\xe2\x86\x90\0" },
  {"&uarr;\0"    ,"\xe2\x86\x91\0" },
  {"&rarr;\0"    ,"\xe2\x86\x92\0" },
  {"&darr;\0"    ,"\xe2\x86\x93\0" },
  {"&harr;\0"    ,"\xe2\x86\x94\0" },
  {"&crarr;\0"   ,"\xe2\x86\xb5\0" },
  {"&lArr;\0"    ,"\xe2\x87\x90\0" },
  {"&uArr;\0"    ,"\xe2\x87\x91\0" },
  {"&rArr;\0"    ,"\xe2\x87\x92\0" },
  {"&dArr;\0"    ,"\xe2\x87\x93\0" },
  {"&hArr;\0"    ,"\xe2\x87\x94\0" },
  {"&forall;\0"  ,"\xe2\x88\x80\0" },
  {"&part;\0"    ,"\xe2\x88\x82\0" },
  {"&exist;\0"   ,"\xe2\x88\x83\0" },
  {"&empty;\0"   ,"\xe2\x88\x85\0" },
  {"&nabla;\0"   ,"\xe2\x88\x87\0" },
  {"&isin;\0"    ,"\xe2\x88\x88\0" },
  {"&notin;\0"   ,"\xe2\x88\x89\0" },
  {"&ni;\0"      ,"\xe2\x88\x8b\0" },
  {"&prod;\0"    ,"\xe2\x88\x8f\0" },
  {"&sum;\0"     ,"\xe2\x88\x91\0" },
  {"&minus;\0"   ,"\xe2\x88\x92\0" },
  {"&lowast;\0"  ,"\xe2\x88\x97\0" },
  {"&radic;\0"   ,"\xe2\x88\x9a\0" },
  {"&prop;\0"    ,"\xe2\x88\x9d\0" },
  {"&infin;\0"   ,"\xe2\x88\x9e\0" },
  {"&ang;\0"     ,"\xe2\x88\xa0\0" },
  {"&and;\0"     ,"\xe2\x88\xa7\0" },
  {"&or;\0"      ,"\xe2\x88\xa8\0" },
  {"&cap;\0"     ,"\xe2\x88\xa9\0" },
  {"&cup;\0"     ,"\xe2\x88\xaa\0" },
  {"&int;\0"     ,"\xe2\x88\xab\0" },
  {"&there4;\0"  ,"\xe2\x88\xb4\0" },
  {"&sim;\0"     ,"\xe2\x88\xbc\0" },
  {"&cong;\0"    ,"\xe2\x89\x85\0" },
  {"&asymp;\0"   ,"\xe2\x89\x88\0" },
  {"&ne;\0"      ,"\xe2\x89\xa0\0" },
  {"&equiv;\0"   ,"\xe2\x89\xa1\0" },
  {"&le;\0"      ,"\xe2\x89\xa4\0" },
  {"&ge;\0"      ,"\xe2\x89\xa5\0" },
  {"&sub;\0"     ,"\xe2\x8a\x82\0" },
  {"&sup;\0"     ,"\xe2\x8a\x83\0" },
  {"&nsub;\0"    ,"\xe2\x8a\x84\0" },
  {"&sube;\0"    ,"\xe2\x8a\x86\0" },
  {"&supe;\0"    ,"\xe2\x8a\x87\0" },
  {"&oplus;\0"   ,"\xe2\x8a\x95\0" },
  {"&otimes;\0"  ,"\xe2\x8a\x97\0" },
  {"&perp;\0"    ,"\xe2\x8a\xa5\0" },
  {"&sdot;\0"    ,"\xe2\x8b\x85\0" },
  {"&lceil;\0"   ,"\xe2\x8c\x88\0" },
  {"&rceil;\0"   ,"\xe2\x8c\x89\0" },
  {"&lfloor;\0"  ,"\xe2\x8c\x8a\0" },
  {"&rfloor;\0"  ,"\xe2\x8c\x8b\0" },
  {"&loz;\0"     ,"\xe2\x97\x8a\0" },
  {"&spades;\0"  ,"\xe2\x99\xa0\0" },
  {"&clubs;\0"   ,"\xe2\x99\xa3\0" },
  {"&hearts;\0"  ,"\xe2\x99\xa5\0" },
  {"&diams;\0"   ,"\xe2\x99\xa6\0" },
  {"&lang;\0"    ,"\xe2\x9f\xa8\0" },
  {"&rang;\0"    ,"\xe2\x9f\xa9\0" }
};

const XHTMLEntityLookup XHTMLEntityLegacyLookupTable[] =
{
  {"&Aelig;\0"   ,"\xc3\x86\0"     }
};
// clang-format on

const std::size_t XHTMLENTITY_LOOKUP_COUNT = (sizeof(XHTMLEntityLookupTable)) / (sizeof(XHTMLEntityLookup));
const std::size_t XHTMLENTITY_LEGACY_LOOPKUP_COUNT =  (sizeof(XHTMLEntityLegacyLookupTable)) / (sizeof(XHTMLEntityLookup));

} // unnamed namespace

const char* const NamedEntityToUtf8(const char* const markupText, unsigned int len)
{
  // finding if given XHTML named entity is supported or not
  for(size_t i = 0; i < XHTMLENTITY_LOOKUP_COUNT; ++i)
  {
    unsigned int entityLen = strlen(XHTMLEntityLookupTable[i].entityName);
    if(len == entityLen)
    {
      if(strncmp(markupText, XHTMLEntityLookupTable[i].entityName, len) == 0) // if named Entity found in table
      {
        return XHTMLEntityLookupTable[i].entityCode;
      }
    }
  }

  // finding in legacy lookup table which was supported in EFL.
  for(size_t i = 0; i < XHTMLENTITY_LEGACY_LOOPKUP_COUNT; ++i)
  {
    unsigned int entityLen = strlen(XHTMLEntityLegacyLookupTable[i].entityName);
    if(len == entityLen)
    {
      if(strncmp(markupText, XHTMLEntityLegacyLookupTable[i].entityName, len) == 0) // if named Entity found in table
      {
        return XHTMLEntityLegacyLookupTable[i].entityCode;
      }
    }
  }

  return NULL;
}

} // namespace  Text

} // namespace  Toolkit

} // namespace  Dali
