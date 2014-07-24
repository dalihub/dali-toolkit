#ifndef __DALI_TOOLKIT_INTERNAL_TEXT_HIGHLIGHT_H__
#define __DALI_TOOLKIT_INTERNAL_TEXT_HIGHLIGHT_H__

/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
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
#include <dali/dali.h>
#include <dali-toolkit/internal/controls/text-input/textview-character-positions-impl.h>

#include <dali-toolkit/public-api/controls/text-view/text-view.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

/**
 * @brief TextHighlight is a decoration which highlights selected text.
 *
 * The class creates a highlight mesh used to show selected text between handles.
 * Not responsible for positioning.
 */
class TextHighlight
{
  /**
   * structure to hold coordinates of each quad, which will make up the mesh.
   */
  struct QuadCoordinates
  {
    /**
     * Default constructor
     */
    QuadCoordinates()
    {
    }

    /**
     * Constructor
     * @param[in] x1 left co-ordinate
     * @param[in] y1 top co-ordinate
     * @param[in] x2 right co-ordinate
     * @param[in] y2 bottom co-ordinate
     */
    QuadCoordinates(float x1, float y1, float x2, float y2)
    : min(x1, y1),
      max(x2, y2)
    {
    }

    Vector2 min;                          ///< top-left (minimum) position of quad
    Vector2 max;                          ///< bottom-right (maximum) position of quad
  };

  typedef std::vector<QuadCoordinates> QuadContainer;

public:

  /**
   * structure for information required to build the highlight mesh
   */
  struct HighlightInfo
  {
    /**
     * Adds a Quad (2D rectangular sub-selection)
     * @param[in] x1 left co-ordinate
     * @param[in] y1 top co-ordinate
     * @param[in] x2 right co-ordinate
     * @param[in] y2 bottom co-ordinate
     */
    void AddQuad( float x1, float y1, float x2, float y2 );

    /**
     * Clamps all quads to fit within a min -> max 2D boundary.
     */
    void Clamp2D(const Vector2& min, const Vector2& max);

    QuadContainer mQuadList;                                 ///< List of quads (sub-selections that form to create complete selection)
  };

  /**
   * Constructor
   * @param[in] textViewCharacterPositioning TextViewCharacterPositioning to be used for positioning information.
   */
  TextHighlight( TextViewCharacterPositioning& textViewCharacterPositioning );

  /**
   * Destructor
   */
  ~TextHighlight();

  /**
   * Gets the table of the visual text positions which has a flag
   * for each Character. The flag is either true (character selected)
   * or false (character de-selected)
   * @note startSelection can be greater or less than endSelection
   *
   * @param[in,out] selectedVisualText The vector to be resized and populated with the selected flags
   * @param[in] startSelection The start selection point for the text
   * @param[in] endSelection The end selection point for the text
   * @param[in] endSelection The end selection point for the text
   * @param[in] textLayoutInfo TextView character layout information
   */
  void GetVisualTextSelection(std::vector<bool>& selectedVisualText, std::size_t startSelection, std::size_t endSelection,
                              Toolkit::TextView::TextLayoutInfo& textLayoutInfo);

  /**
   * Iterates between selection handles and computes the info required to build the highlight mesh
   * @param[in] handlePositionStart starting handle position
   * @param[in] handlePositionEnd ending handle position
   * @return textHighlight target TextHighlight
   */
  TextHighlight::HighlightInfo CalculateHighlightInfo( std::size_t handlePositionStart, std::size_t handlePositionEnd, Toolkit::TextView::TextLayoutInfo& textLayoutInfo );

  /**
   * Calculates new Mesh data so highlight moves with selection handles.
   * @param[in] newHighlightInfo HighlightInfo calculated by CalculateHighlightInfo
   */
  void UpdateHighlight( TextHighlight::HighlightInfo& newHighlightInfo );

  /**
   * Creates the Mesh data needed by the Mesh Actor
   */
  Mesh CreateHighLightMesh();

private:

  /**
   * @brief Copy Constructor
   * @param[in] textHight
   * Undefined/Hidden.
   */
  TextHighlight(const TextHighlight& textHight );

  /**
   * @Assignment Constructor
   * @param[in] rhs
   * Undefined/Hidden.
   */
  TextHighlight& operator=(const TextHighlight& rhs);

private:

  TextViewCharacterPositioning& mTextViewCharacterPositioning;

  Mesh              mHighlightMesh;             ///< Mesh Data for highlight
  MeshData          mMeshData;                  ///< Container to hold meshData for highlight
  Material          mCustomMaterial;            ///< Custom material used for highlight
  HighlightInfo     mNewHighlightInfo;          ///< Geometry info to create highlight.

};


} // namespace Internal


} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_INTERNAL_TEXT_HIGHLIGHT_H__
