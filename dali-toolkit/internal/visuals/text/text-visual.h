#ifndef DALI_TOOLKIT_INTERNAL_TEXT_VISUAL_H
#define DALI_TOOLKIT_INTERNAL_TEXT_VISUAL_H

/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
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
 * | batchingEnabled     | BOOLEAN |
 *
 */
class TextVisual : public Visual::Base
{
public:

  /**
   * @brief Create a new text visual.
   *
   * @param[in] factoryCache A pointer pointing to the VisualFactoryCache object
   * @return A smart-pointer to the newly allocated visual.
   */
  static TextVisualPtr New( VisualFactoryCache& factoryCache );

public: // from Visual::Base

  /**
   * @copydoc Visual::Base::GetHeightForWidth()
   */
  virtual float GetHeightForWidth( float width ) const;

  /**
   * @copydoc Visual::Base::GetNaturalSize()
   */
  virtual void GetNaturalSize( Vector2& naturalSize );

  /**
   * @copydoc Visual::Base::CreatePropertyMap()
   */
  virtual void DoCreatePropertyMap( Property::Map& map ) const;

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
   * Set the individual property to the given value
   * @param[in] index The index key used to reference this value within the initial
   * property map.
   * @param[in] propertyValue The value to set
   */
  void DoSetProperty( Dali::Property::Index index, const Dali::Property::Value& propertyValue );

  /**
   * @brief Creates the text's renderer.
   */
  void CreateRenderer();

  /**
   * @brief Destroys the text's renderer.
   */
  void DestroyRenderer();

private:
  Text::ControllerPtr mController; ///< The text's controller.
  Text::TypesetterPtr mTypesetter; ///< The text's typesetter.
  WeakHandle<Actor>   mControl;    ///< The control where the renderer is added.
};

} // namespace Internal

} // namespace Toolkit

} // namespace Dali

#endif /* DALI_TOOLKIT_INTERNAL_TEXT_VISUAL_H */
