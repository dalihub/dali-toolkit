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

// CLASS HEADER
#include <dali-toolkit/internal/controls/text-input/text-input-text-highlight-impl.h>

// EXTERNAL INCLUDES
#include <math.h>
#include <sstream>
#include <algorithm>
#include <libintl.h>
#include <dali/integration-api/debug.h>

// INTERNAL INCLUDES

using namespace Dali;
using namespace std;

namespace
{
  /**
   * Selection state enumeration (FSM)
   */
  enum SelectionState
  {
    SelectionNone,                            ///< Currently not encountered selected section.
    SelectionStarted,                         ///< Encountered selected section
    SelectionFinished                         ///< Finished selected section
  };

  const Vector4 LIGHTBLUE( 10.0f/255.0f, 140.0f/255.0f, 210.0f/255.0f, 1.0f );    // todo make this a setting

  const float CHARACTER_THRESHOLD( 2.5f ); // todo check if unified method to do this in Text
}

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

// Default constructor
TextHighlight::TextHighlight( TextViewCharacterPositioning& textViewCharacterPositioning ) :
    mTextViewCharacterPositioning( textViewCharacterPositioning )
{
}

TextHighlight::~TextHighlight()
{
}

void TextHighlight::GetVisualTextSelection(std::vector<bool>& selectedVisualText, std::size_t startSelection, std::size_t endSelection,
                                           Toolkit::TextView::TextLayoutInfo& textLayoutInfo )
{
  std::vector<int>::iterator it = textLayoutInfo.mCharacterLogicalToVisualMap.begin();
  std::vector<int>::iterator startSelectionIt = textLayoutInfo.mCharacterLogicalToVisualMap.begin() + std::min(startSelection, endSelection);
  std::vector<int>::iterator endSelectionIt = textLayoutInfo.mCharacterLogicalToVisualMap.begin() + std::max(startSelection, endSelection);
  std::vector<int>::iterator end = textLayoutInfo.mCharacterLogicalToVisualMap.end();

  selectedVisualText.resize( mTextViewCharacterPositioning.GetNumberOfCharactersInText() );

  // Deselect text prior to startSelectionIt
  for(;it!=startSelectionIt;++it)
  {
    selectedVisualText[*it] = false;
  }

  // Select text from startSelectionIt -> endSelectionIt
  for(;it!=endSelectionIt;++it)
  {
    selectedVisualText[*it] = true;
  }

  // Deselect text after endSelection
  for(;it!=end;++it)
  {
    selectedVisualText[*it] = false;
  }

  selectedVisualText.resize( mTextViewCharacterPositioning.GetNumberOfCharactersInText(), false );
}

// Calculate the dimensions of the quads they will make the highlight mesh
TextHighlight::HighlightInfo TextHighlight::CalculateHighlightInfo( std::size_t handlePositionStart, std::size_t handlePositionEnd, Toolkit::TextView::TextLayoutInfo& textLayoutInfo )
{
  // At the moment there is no public API to modify the block alignment option.

  TextHighlight::HighlightInfo newHighlightInfo;
  //newHighlightInfo.mQuadList.clear(); // clear last quad information.

  if ( !mTextViewCharacterPositioning.IsTextEmpty() && !textLayoutInfo.mCharacterLogicalToVisualMap.empty() )
  {
    // Get vector of flags representing characters that are selected (true) vs unselected (false).
    std::vector<bool> selectedVisualText;
    GetVisualTextSelection(selectedVisualText, handlePositionStart, handlePositionEnd, textLayoutInfo );
    std::vector<bool>::iterator selectedIt(selectedVisualText.begin());
    std::vector<bool>::iterator selectedEndIt(selectedVisualText.end());

    SelectionState selectionState = SelectionNone;          ///< Current selection status of cursor over entire text.
    float rowLeft = 0.0f;
    float rowRight = 0.0f;
    // Keep track of the TextView's min/max extents. Should be able to query this from TextView.
    float maxRowLeft = std::numeric_limits<float>::max();
    float maxRowRight = 0.0f;

    Toolkit::TextView::CharacterLayoutInfoContainer::iterator it = textLayoutInfo.mCharacterLayoutInfoTable.begin();
    Toolkit::TextView::CharacterLayoutInfoContainer::iterator end = textLayoutInfo.mCharacterLayoutInfoTable.end();

    Toolkit::TextView::CharacterLayoutInfoContainer::iterator lastIt = it;

    // Scan through entire text.
    while(it != end)
    {
      // selectionState: None when not in selection, Started when in selection, and Ended when reached end of selection.

      Toolkit::TextView::CharacterLayoutInfo& charInfo(*it);
      bool charSelected( false );
      if( selectedIt != selectedEndIt )
      {
        charSelected = *selectedIt++;
      }

      if(selectionState == SelectionNone)
      {
        if(charSelected)
        {
          selectionState = SelectionStarted;
          rowLeft = charInfo.mPosition.x - textLayoutInfo.mScrollOffset.x;
          rowRight = rowLeft + charInfo.mSize.width;
        }
      }
      else if(selectionState == SelectionStarted)
      {
        // break selection on:
        // 1. new line causing selection break. (\n or wordwrap)
        // 2. character not selected.
        if(charInfo.mPosition.y - lastIt->mPosition.y > CHARACTER_THRESHOLD ||
           !charSelected)
        {
          // finished selection.
          // TODO: TextView should have a table of visual rows, and each character a reference to the row
          // that it resides on. That way this enumeration is not necessary.
          Vector2 min, max;
          if(lastIt->mIsNewParagraphChar)
          {
            // If the last character is a new line, then to get the row rect, we need to scan from the character before the new line.
            lastIt = std::max( textLayoutInfo.mCharacterLayoutInfoTable.begin(), lastIt - 1 );
          }
          const Size rowSize( mTextViewCharacterPositioning.GetRowRectFromCharacterPosition( lastIt - textLayoutInfo.mCharacterLayoutInfoTable.begin(), min, max ) );
          maxRowLeft = std::min(maxRowLeft, min.x);
          maxRowRight = std::max(maxRowRight, max.x);
          float rowBottom = lastIt->mPosition.y - textLayoutInfo.mScrollOffset.y;
          float rowTop = rowBottom - rowSize.height;

          // Still selected, and block-align mode then set rowRight to max, so it can be clamped afterwards
          if(charSelected)
          {
            rowRight = std::numeric_limits<float>::max();
          }
          newHighlightInfo.AddQuad( rowLeft, rowTop, rowRight, rowBottom );

          selectionState = SelectionNone;

          // Still selected? start a new selection
          if( charSelected )
          {
            // if block-align mode then set rowLeft to min, so it can be clamped afterwards
            rowLeft = 0.0f;
            rowRight = ( charInfo.mPosition.x - textLayoutInfo.mScrollOffset.x ) + charInfo.mSize.width;
            selectionState = SelectionStarted;
          }
        }
        else
        {
          // build up highlight(s) with this selection data.
          rowLeft = std::min( charInfo.mPosition.x - textLayoutInfo.mScrollOffset.x, rowLeft );
          rowRight = std::max( ( charInfo.mPosition.x - textLayoutInfo.mScrollOffset.x ) + charInfo.mSize.width, rowRight );
        }
      }

      lastIt = it++;
    }

    // If reached end, and still on selection, then close selection.
    if(it == end)
    {
      if(selectionState == SelectionStarted)
      {
        // finished selection.
        Vector2 min, max;
        if(lastIt != end && lastIt->mIsNewParagraphChar)
        {
          lastIt = std::max( textLayoutInfo.mCharacterLayoutInfoTable.begin(), lastIt - 1 );
        }
        const Size rowSize( mTextViewCharacterPositioning.GetRowRectFromCharacterPosition( lastIt - textLayoutInfo.mCharacterLayoutInfoTable.begin(), min, max ) );
        maxRowLeft = std::min(maxRowLeft, min.x);
        maxRowRight = std::max(maxRowRight, max.x);
        float rowBottom = lastIt->mPosition.y - textLayoutInfo.mScrollOffset.y;
        float rowTop = rowBottom - rowSize.height;
        newHighlightInfo.AddQuad( rowLeft, rowTop, rowRight, rowBottom );
      }
    }

    // Get the top left and bottom right corners.
    const Toolkit::TextView::CharacterLayoutInfo& firstCharacter( *textLayoutInfo.mCharacterLayoutInfoTable.begin() );
    const Vector2 topLeft( maxRowLeft, firstCharacter.mPosition.y - firstCharacter.mSize.height );
    const Vector2 bottomRight( topLeft.x + textLayoutInfo.mTextSize.width, topLeft.y + textLayoutInfo.mTextSize.height );

    // Clamp quads so they appear to clip to borders of the whole text.
    newHighlightInfo.Clamp2D( topLeft, bottomRight );

    // For block-align align Further Clamp quads to max left and right extents
      // BlockAlign: Will adjust highlight to block:
      // i.e.
      //   H[ello] (top row right = max of all rows right)
      // [--this-] (middle rows' left = min of all rows left, middle rows' right = max of all rows right)
      // [is some] (middle rows' left = min of all rows left, middle rows' right = max of all rows right)
      // [text] (bottom row left = min of all rows left)
      // (common in SMS messaging selection)
      //
      // As opposed to the default which is tight text highlighting.
      //   H[ello]
      //   [this]
      // [is some]
      // [text]
      // (common in regular text editors/web browser selection)
      newHighlightInfo.Clamp2D( Vector2(maxRowLeft, topLeft.y), Vector2(maxRowRight, bottomRight.y ) );

  }

  return newHighlightInfo;
}

void TextHighlight::UpdateHighlight( TextHighlight::HighlightInfo& newHighlightInfo )
{
//  Construct a Mesh with a texture to be used as the highlight 'box' for selected text
//
//  Example scenarios where mesh is made from 3, 1, 2, 2 ,3 or 3 quads.
//
//   [ TOP   ]  [ TOP ]      [TOP ]  [ TOP    ]      [ TOP  ]      [ TOP  ]
//  [ MIDDLE ]             [BOTTOM]  [BOTTOM]      [ MIDDLE ]   [ MIDDLE  ]
//  [ BOTTOM]                                      [ MIDDLE ]   [ MIDDLE  ]
//                                                 [BOTTOM]     [ MIDDLE  ]
//                                                              [BOTTOM]
//
//  Each quad is created as 2 triangles.
//  Middle is just 1 quad regardless of its size.
//
//  (0,0)         (0,0)
//     0*    *2     0*       *2
//     TOP          TOP
//     3*    *1     3*       *1
//  4*       *1     4*     *6
//     MIDDLE         BOTTOM
//  6*       *5     7*     *5
//  6*    *8
//   BOTTOM
//  9*    *7
//

  // vertex and triangle buffers should always be present if MeshActor is alive.
  //HighlightInfo newHighlightInfo = CalculateHighlightInfo( handlePositionStart, handlePositionEnd );
  MeshData::VertexContainer vertices;
  Dali::MeshData::FaceIndices faceIndices;

  if( !newHighlightInfo.mQuadList.empty() )
  {
    std::vector<QuadCoordinates>::iterator iter = newHighlightInfo.mQuadList.begin();
    std::vector<QuadCoordinates>::iterator endIter = newHighlightInfo.mQuadList.end();

    // vertex position defaults to (0 0 0)
    MeshData::Vertex vertex;
    // set normal for all vertices as (0 0 1) pointing outward from TextInput Actor.
    vertex.nZ = 1.0f;

    for(std::size_t v = 0; iter != endIter; ++iter,v+=4 )
    {
      // Add each quad geometry (a sub-selection) to the mesh data.

      // 0-----1
      // |\    |
      // | \ A |
      // |  \  |
      // | B \ |
      // |    \|
      // 2-----3

      QuadCoordinates& quad = *iter;
      // top-left (v+0)
      vertex.x = quad.min.x;
      vertex.y = quad.min.y;
      vertices.push_back( vertex );

      // top-right (v+1)
      vertex.x = quad.max.x;
      vertex.y = quad.min.y;
      vertices.push_back( vertex );

      // bottom-left (v+2)
      vertex.x = quad.min.x;
      vertex.y = quad.max.y;
      vertices.push_back( vertex );

      // bottom-right (v+3)
      vertex.x = quad.max.x;
      vertex.y = quad.max.y;
      vertices.push_back( vertex );

      // triangle A (3, 1, 0)
      faceIndices.push_back( v + 3 );
      faceIndices.push_back( v + 1 );
      faceIndices.push_back( v );

      // triangle B (0, 2, 3)
      faceIndices.push_back( v );
      faceIndices.push_back( v + 2 );
      faceIndices.push_back( v + 3 );
    }

    mMeshData.SetVertices( vertices );
    mMeshData.SetFaceIndices( faceIndices );

    mHighlightMesh.UpdateMeshData(mMeshData);
  }
}

Mesh TextHighlight::CreateHighLightMesh()
{
  mMeshData = MeshData( );
  mMeshData.SetHasNormals( true );

  mCustomMaterial = Material::New("CustomMaterial");
  mCustomMaterial.SetDiffuseColor( LIGHTBLUE );

  mMeshData.SetMaterial( mCustomMaterial );

  mHighlightMesh = Mesh::New( mMeshData );

  return mHighlightMesh;
}

void TextHighlight::HighlightInfo::AddQuad( float x1, float y1, float x2, float y2 )
{
  QuadCoordinates quad(x1, y1, x2, y2);
  mQuadList.push_back( quad );
}

void TextHighlight::HighlightInfo::Clamp2D(const Vector2& min, const Vector2& max)
{
  for(std::size_t i = 0;i < mQuadList.size(); i++)
  {
    QuadCoordinates& quad = mQuadList[i];

    quad.min.Clamp(min, max);
    quad.max.Clamp(min, max);
  }
}

} // Internal

} // namespace Toolkit

} // namespace Dali

