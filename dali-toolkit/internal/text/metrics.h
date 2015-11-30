#ifndef __DALI_TOOLKIT_TEXT_METRICS_H__
#define __DALI_TOOLKIT_TEXT_METRICS_H__

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

// EXTERNAL INCLUDES
#include <dali/public-api/common/intrusive-ptr.h>
#include <dali/devel-api/text-abstraction/font-client.h>

namespace Dali
{

namespace Toolkit
{

namespace Text
{

class Metrics;
typedef IntrusivePtr<Metrics> MetricsPtr;

/**
 * @brief A wrapper around FontClient used to get metrics & potentially down-scaled Emoji metrics.
 */
class Metrics : public RefObject
{
public:

  /**
   * Create a new Metrics object
   */
  static Metrics* New( TextAbstraction::FontClient& fontClient )
  {
    return new Metrics( fontClient );
  }

  /**
   * @brief Used to switch between bitmap & vector based glyphs
   *
   * @param[in] glyphType The type of glyph; note that metrics for bitmap & vector based glyphs are different.
   */
  void SetGlyphType( TextAbstraction::GlyphType glyphType )
  {
    mGlyphType = glyphType;
  }

  /**
   * @brief Set the maximum Emoji size.
   *
   * @param[in] emojiSize Emoticons will be scaled to fit this size in pixels.
   */
  void SetMaxEmojiSize( int emojiSize )
  {
    mEmojiSize = emojiSize;
  }

  /**
   * @brief Get the maximum Emoji size.
   *
   * @return The maximum Emoji size.
   */
  int GetMaxEmojiSize() const
  {
    return mEmojiSize;
  }

  /**
   * @brief Query the metrics for a font.
   *
   * @param[in] fontId The ID of the font for the required glyph.
   * @param[out] metrics The font metrics.
   */
  void GetFontMetrics( FontId fontId, FontMetrics& metrics )
  {
    mFontClient.GetFontMetrics( fontId, metrics, mEmojiSize ); // inline for performance
  }

  /**
   * @brief Retrieve the metrics for a series of glyphs.
   *
   * @param[in,out] array An array of glyph-info structures with initialized FontId & GlyphIndex values.
   * It may contain the advance and an offset set into the bearing from the shaping tool.
   * On return, the glyph's size value will be initialized. The bearing value will be updated by adding the font's glyph bearing to the one set by the shaping tool.
   * @param[in] size The size of the array.
   * @return True if all of the requested metrics were found.
   */
  bool GetGlyphMetrics( GlyphInfo* array, uint32_t size )
  {
    return mFontClient.GetGlyphMetrics( array, size, mGlyphType, true, mEmojiSize ); // inline for performance
  }

protected:

  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~Metrics() {}

private:

  /**
   * Constructor.
   */
  Metrics( TextAbstraction::FontClient& fontClient )
  : mFontClient( fontClient ),
    mGlyphType( TextAbstraction::BITMAP_GLYPH ),
    mEmojiSize( 0 )
  {
  }

  // Undefined
  Metrics(const Metrics&);

  // Undefined
  Metrics& operator=(const Metrics& rhs);

private:

  TextAbstraction::FontClient mFontClient;
  TextAbstraction::GlyphType mGlyphType;
  int mEmojiSize;
};

} // namespace Text

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_TEXT_METRICS_H__
