#ifndef DALI_TOOLKIT_INTERNAL_TEXT_VISUAL_H
#define DALI_TOOLKIT_INTERNAL_TEXT_VISUAL_H

/*
 * Copyright (c) 2017 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/object/base-object.h>
#include <dali/public-api/object/weak-handle.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/visuals/visual-base-impl.h>
#include <dali-toolkit/internal/text/rendering/text-typesetter.h>
#include <dali-toolkit/internal/text/text-controller.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

class TextVisual;
typedef IntrusivePtr< TextVisual > TextVisualPtr;

/**
 * The visual which renders text
 *
 * The following properties are optional:
 *
 * | %Property Name      | Type    |
 * |---------------------|---------|
 * | renderingBackend    | INTEGER |
 * | text                | STRING  |
 * | fontFamily          | STRING  |
 * | fontStyle           | STRING  |
 * | pointSize           | FLOAT   |
 * | multiLine           | BOOLEAN |
 * | horizontalAlignment | STRING  |
 * | verticalAlignment   | STRING  |
 * | textColor           | VECTOR4 |
 * | enableMarkup        | BOOLEAN |
 * | enableAutoScroll    | BOOLEAN |
 * | autoScrollSpeed     | INTEGER |
 * | autoScrollLoopCount | INTEGER |
 * | autoScrollGap       | INTEGER |
 * | lineSpacing         | FLOAT   |
 * | underline           | STRING  |
 * | shadow              | STRING  |
 * | outline             | STRING  |
 *
 */
class TextVisual : public Visual::Base
{
public:

  /**
   * @brief Create a new text visual.
   *
   * @param[in] factoryCache A pointer pointing to the VisualFactoryCache object
   * @param[in] properties A Property::Map containing settings for this visual
   * @return A smart-pointer to the newly allocated visual.
   */
  static TextVisualPtr New( VisualFactoryCache& factoryCache, const Property::Map& properties );

  /**
   * @brief Converts all strings keys in property map to index keys.  Property Map can then be merged correctly.
   * @param[in,out] propertyMap containing string keys or a mix of strings and indexes. Will be changed to index keys.
   */
  static void ConvertStringKeysToIndexKeys( Property::Map& propertyMap );

  /**
   * @brief Retrieve the text's controller.
   * @param[in] visual The text visual.
   * @return The text controller
   */
  static Text::ControllerPtr GetController( Toolkit::Visual::Base visual )
  {
    return GetVisualObject( visual ).mController;
  };

  /**
   * @brief Set the index of the animatable text color property.
   * @param[in] visual The text visual.
   * @param[in] animatablePropertyIndex The index of the animatable property
   */
  static void SetAnimatableTextColorProperty( Toolkit::Visual::Base visual, Property::Index animatablePropertyIndex )
  {
    GetVisualObject( visual ).mAnimatableTextColorPropertyIndex = animatablePropertyIndex;
  };

  /**
   * @brief Set the flag to trigger the textures to be initialized and renderer to be added to the control.
   * @param[in] visual The text visual.
   */
  static void EnableRendererUpdate( Toolkit::Visual::Base visual )
  {
    GetVisualObject( visual ).mRendererUpdateNeeded = true;
  };

  /**
   * @brief Instantly updates the renderer
   * @param[in] visual The text visual.
   */
  static void UpdateRenderer( Toolkit::Visual::Base visual )
  {
    GetVisualObject( visual ).UpdateRenderer();
  };

public: // from Visual::Base

  /**
   * @copydoc Visual::Base::GetHeightForWidth()
   */
  virtual float GetHeightForWidth( float width );

  /**
   * @copydoc Visual::Base::GetNaturalSize()
   */
  virtual void GetNaturalSize( Vector2& naturalSize );

  /**
   * @copydoc Visual::Base::CreatePropertyMap()
   */
  virtual void DoCreatePropertyMap( Property::Map& map ) const;

  /**
   * @copydoc Visual::Base::CreateInstancePropertyMap
   */
  virtual void DoCreateInstancePropertyMap( Property::Map& map ) const;

protected:

  /**
   * @brief Constructor.
   *
   * @param[in] factoryCache The VisualFactoryCache object
   */
  TextVisual( VisualFactoryCache& factoryCache );

  /**
   * @brief A reference counted object may only be deleted by calling Unreference().
   */
  virtual ~TextVisual();

  // from Visual::Base

  /**
   * @copydoc Visual::Base::DoSetProperties()
   */
  virtual void DoSetProperties( const Property::Map& propertyMap );

  /**
   * @copydoc Visual::Base::DoSetOnStage()
   */
  virtual void DoSetOnStage( Actor& actor );

  /**
   * @copydoc Visual::Base::DoSetOffStage()
   */
  virtual void DoSetOffStage( Actor& actor );

  /**
   * @copydoc Visual::Base::OnSetTransform
   */
  virtual void OnSetTransform();

private:
  /**
   * @brief Set the individual property to the given value.
   *
   * @param[in] index The index key used to reference this value within the initial property map.
   *
   * @param[in] propertyValue The value to set.
   */
  void DoSetProperty( Dali::Property::Index index, const Dali::Property::Value& propertyValue );

  /**
   * @brief Updates the text's renderer.
   */
  void UpdateRenderer();

  /**
   * @brief Removes the texture set from the renderer.
   */
  void RemoveTextureSet();

  /**
   * Get the texture of the text for rendering.
   * @param[in] size The texture size.
   * @param[in] hasMultipleTextColors Whether the text contains multiple colors.
   * @param[in] containsColorGlyph Whether the text contains color glyph.
   * @param[in] styleEnabled Whether the text contains any styles (e.g. shadow, underline, etc.).
   */
  TextureSet GetTextTexture( const Vector2& size, bool hasMultipleTextColors, bool containsColorGlyph, bool styleEnabled );

  /**
   * Get the text rendering shader.
   * @param[in] factoryCache A pointer pointing to the VisualFactoryCache object
   * @param[in] hasMultipleTextColors Whether the text contains multiple colors.
   * @param[in] containsColorGlyph Whether the text contains color glyph.
   * @param[in] styleEnabled Whether the text contains any styles (e.g. shadow, underline, etc.).
   */
  Shader GetTextShader( VisualFactoryCache& factoryCache, bool hasMultipleTextColors, bool containsColorGlyph, bool styleEnabled );

  /**
   * @brief Retrieve the text's controller.
   * @param[in] visual The text visual.
   * @return The text controller
   */
  static TextVisual& GetVisualObject( Toolkit::Visual::Base visual )
  {
    return static_cast<TextVisual&>( visual.GetBaseObject() );
  };

private:

  /**
   * Used as an alternative to boolean so that it is obvious whether the text contains single or multiple text colors, and emoji and styles.
   */
  struct TextType
  {
    enum Type
    {
      SINGLE_COLOR_TEXT = 0, ///< The text contains single color only.
      MULTI_COLOR_TEXT = 1,  ///< The text contains multiple colorｓ.
      NO_EMOJI = 0,          ///< The text contains no emoji.
      HAS_EMOJI = 1,         ///< The text contains emoji.
      NO_STYLES = 0,         ///< The text contains contains no styles.
      HAS_SYLES = 1          ///< The text contains contains styles.
    };
  };

private:
  Text::ControllerPtr mController;                        ///< The text's controller.
  Text::TypesetterPtr mTypesetter;                        ///< The text's typesetter.
  WeakHandle<Actor>   mControl;                           ///< The control where the renderer is added.
  Property::Index     mAnimatableTextColorPropertyIndex;  ///< The index of animatable text color property registered by the control.
  bool                mRendererUpdateNeeded:1;            ///< The flag to indicate whether the renderer needs to be updated.
};

} // namespace Internal

} // namespace Toolkit

} // namespace Dali

#endif /* DALI_TOOLKIT_INTERNAL_TEXT_VISUAL_H */
