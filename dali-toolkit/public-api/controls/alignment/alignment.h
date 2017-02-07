#ifndef __DALI_ALIGNMENT_H__
#define __DALI_ALIGNMENT_H__

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

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/controls/control.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal DALI_INTERNAL
{
class Alignment;
}
/**
 * @addtogroup dali_toolkit_controls_alignment
 * @{
 */

/**
 * @brief Alignment is a container which provides an easy way to align other actors inside its boundary.
 *
 * Additionally, it provides a scaling property to resize the contained actors @see Scaling.
 * @SINCE_1_0.0
 * @note The use of scaling property will override all constraints applied to actors.
 *
 * All actors added to an alignment are going to be set with the same anchor point and parent origin. And if the scaling property is set to a value
 * different than ScaleNone, constraints as well.
 */
class DALI_IMPORT_API Alignment : public Control
{
public:
  /**
   * @brief Enumeration for different types of alignment.
   * @SINCE_1_0.0
   */
  enum Type
  {
    HorizontalLeft   = 1, ///< Horizontal left alignment @SINCE_1_0.0
    HorizontalCenter = 2, ///< Horizontal center alignment @SINCE_1_0.0
    HorizontalRight  = 4, ///< Horizontal right alignment @SINCE_1_0.0
    VerticalTop      = 8, ///< Vertical top alignment @SINCE_1_0.0
    VerticalCenter   = 16, ///< Vertical center alignment @SINCE_1_0.0
    VerticalBottom   = 32 ///< Vertical bottom alignment @SINCE_1_0.0
  };

  /**
   * @brief Scaling determines how actors are scaled to match the alignment's boundary.
   * @SINCE_1_0.0
   */
  enum Scaling
  {
    ScaleNone,             ///< The original size is kept. @SINCE_1_0.0
    ScaleToFill,           ///< Scale added actors to fill alignment's boundary. Aspect ratio is not maintained. @SINCE_1_0.0
    ScaleToFitKeepAspect,  ///< Scale added actors to fit within the alignment's boundary. Aspect ratio is maintained. @SINCE_1_0.0
    ScaleToFillKeepAspect, ///< Scale added actors to fill the alignment's boundary. Aspect ratio is maintained, and the actor may exceed the alignment's boundary. @SINCE_1_0.0
    ShrinkToFit,           ///< If added actors are larger than the alignment's boundary it will be shrunk down to fit. Aspect ratio is not maintained @SINCE_1_0.0
    ShrinkToFitKeepAspect, ///< If added actors are larger than the alignment's boundary it will be shrunk down to fit. Aspect ratio is maintained @SINCE_1_0.0
  };

  /**
   * @brief Structure describing the padding values.
   * @SINCE_1_0.0
   */
  struct Padding
  {
    /**
     * @brief Constructor.
     * @SINCE_1_0.0
     */
    Padding()
    : left( 0.f ),
      right( 0.f ),
      top( 0.f ),
      bottom( 0.f )
    {
    }

    /**
     * @brief Constructor.
     *
     * @SINCE_1_0.0
     * @param[in] l Left padding
     * @param[in] r Right padding
     * @param[in] t Top padding
     * @param[in] b Bottom padding
     */
    Padding( float l, float r, float t, float b )
    : left( l ),
      right( r ),
      top( t ),
      bottom( b )
    {
    }

    float left;  ///< The left padding
    float right; ///< The right padding
    float top;   ///< The top padding
    float bottom; ///< The bottom padding
  };

  /**
   * @brief Creates an Alignment handle; this can be initialized with Alignment::New().
   *
   * Calling member functions with an uninitialized handle is not allowed.
   * @SINCE_1_0.0
   */
  Alignment();

  /**
   * @brief Creates an alignment control.
   *
   * @SINCE_1_0.0
   * @param[in] horizontal Specifies how to align actors horizontally. Could be HorizontalLeft, HorizontalCenter or HorizontalRight. By default, HorizontalCenter
   * @param[in] vertical Specifies how to align actors vertically. Could be VerticalTop, VerticalCenter or VerticalBottom. By default, VerticalCenter
   * @return A handle to the Alignment control
   */
  static Alignment New( Type horizontal = HorizontalCenter, Type vertical = VerticalCenter );

  /**
   * @brief Copy constructor. Creates another handle that points to the same real object.
   *
   * @SINCE_1_0.0
   * @param[in] alignment Object to copy
   */
  Alignment(const Alignment& alignment);

  /**
   * @brief Destructor.
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   * @SINCE_1_0.0
   */
  ~Alignment();

  /**
   * @brief Downcasts a handle to Alignment handle.
   *
   * If handle points to an Alignment, the downcast produces valid handle.
   * If not, the returned handle is left uninitialized.
   *
   * @SINCE_1_0.0
   * @param[in] handle Handle to an object
   * @return A handle to a Alignment or an uninitialized handle
   */
  static Alignment DownCast( BaseHandle handle );

  /**
   * @brief Sets the new alignment. By default, ( HorizontalCenter | VerticalCenter ).
   *
   * @SINCE_1_0.0
   * @param[in] type The new alignment option
   * @note There should only be one horizontal and one vertical policy.
   */
  void SetAlignmentType( Type type );

  /**
   * @brief Gets the current alignment combined into a single value.
   *
   * The values can be tested by using the & operator and the desired
   * flag. e.g.
   * @code
   *   if (GetAlignmentType() & HorizontalCentre)
   *   {
   *     ...
   *   }
   * @endcode
   *
   * @SINCE_1_0.0
   * @return the alignment value
   */
  Type GetAlignmentType() const;

  /**
   * @brief Sets how added actors scale to fit the alignment's boundary.
   *
   * @SINCE_1_0.0
   * @param[in] scaling The scaling property
   * @see Scaling.
   */
  void SetScaling( Scaling scaling );

  /**
   * @brief Retrieves the scaling property.
   *
   * @SINCE_1_0.0
   * @return The scaling
   * @see Scaling.
   */
  Scaling GetScaling() const;

  /**
   * @brief Sets a padding value.
   *
   * @SINCE_1_0.0
   * @param[in] padding The left, right, top, bottom padding values
   */
  void SetPadding( const Padding& padding );

  /**
   * @brief Gets the padding values.
   *
   * @SINCE_1_0.0
   * @return The left, right, top, bottom padding values
   */
  const Padding& GetPadding() const;

  /**
   * @brief Assignment operator.
   *
   * Changes this handle to point to another real object.
   * @SINCE_1_0.0
   * @param[in] alignment Object to copy
   * @return A reference to this
   */
  Alignment& operator=(const Alignment& alignment);

public: // Not intended for application developers

  /**
   * @internal
   * @brief Creates a handle using the Toolkit::Internal implementation.
   *
   * @SINCE_1_0.0
   * @param[in] implementation The Control implementation
   */
  DALI_INTERNAL Alignment( Internal::Alignment& implementation );

  /**
   * @internal
   * @brief Allows the creation of this Control from an Internal::CustomActor pointer.
   *
   * @SINCE_1_0.0
   * @param[in] internal A pointer to the internal CustomActor
   */
  explicit DALI_INTERNAL Alignment( Dali::Internal::CustomActor* internal );
};

/**
 * @}
 */
} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_LAYOUT_H__
