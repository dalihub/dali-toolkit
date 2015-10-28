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
 * Additionaly it provides a scaling property to resize the contained actors @see Scaling.
 * @note The use of scaling property will override all constraints applied to actors.
 *
 * All actors added to an alignment are going to be set with the same anchor point and parent origin. And, if the scaling property is set to a value
 * different than ScaleNone, constraints as well.
 */
class DALI_IMPORT_API Alignment : public Control
{
public:
  /**
   * @brief Different types of alignment.
   */
  enum Type
  {
    HorizontalLeft   = 1,
    HorizontalCenter = 2,
    HorizontalRight  = 4,
    VerticalTop      = 8,
    VerticalCenter   = 16,
    VerticalBottom   = 32
  };

  /**
   * @brief Scaling determines how actors are scaled, to match the alignment's boundary.
   */
  enum Scaling
  {
    ScaleNone,             ///< The original size is kept.
    ScaleToFill,           ///< Scale added actors to fill alignment's boundary. Aspect ratio is not maintained.
    ScaleToFitKeepAspect,  ///< Scale added actors to fit within the alignment's boundary. Aspect ratio is maintained.
    ScaleToFillKeepAspect, ///< Scale added actors to fill the alignment's boundary. Aspect ratio is maintained, and the actor may exceed the alignment's boundary.
    ShrinkToFit,           ///< If added actors are larger than the alignment's boundary it will be shrunk down to fit. Aspect ratio is not maintained
    ShrinkToFitKeepAspect, ///< If added actors are larger than the alignment's boundary it will be shrunk down to fit. Aspect ratio is maintained
  };

  /**
   * @brief Structure describing the padding values.
   */
  struct Padding
  {
    /**
     * @brief Constructor
     */
    Padding()
    : left( 0.f ),
      right( 0.f ),
      top( 0.f ),
      bottom( 0.f )
    {
    }

    /**
     * @brief Constructor
     *
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
   * @brief Create an Alignment handle; this can be initialised with Alignment::New().
   *
   * Calling member functions with an uninitialised handle is not allowed.
   */
  Alignment();

  /**
   * @brief Creates an alignment control.
   *
   * @param [in] horizontal Specifies how to align actors horizontally. Could be HorizontalLeft, HorizontalCenter or HorizontalRight. By default HorizontalCenter.
   * @param [in] vertical Specifies how to align actors vertically. Could be VerticalTop, VerticalCenter or VerticalBottom. By default VerticalCenter.
   * @return A handle to the Alignment control.
   */
  static Alignment New( Type horizontal = HorizontalCenter, Type vertical = VerticalCenter );

  /**
   * @brief Copy constructor. Creates another handle that points to the same real object.
   *
   * @param[in] alignment Object to copy.
   */
  Alignment(const Alignment& alignment);

  /**
   * @brief Destructor
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   */
  ~Alignment();

  /**
   * @brief Downcast an Object handle to Alignment.
   *
   * If handle points to a Alignment the downcast produces valid
   * handle. If not the returned handle is left uninitialized.
   *
   * @param[in] handle Handle to an object
   * @return handle to a Alignment or an uninitialized handle
   */
  static Alignment DownCast( BaseHandle handle );

  /**
   * @brief Sets the new alignment. By default ( HorizontalCenter | VerticalCenter ).
   *
   * @note there should only be one horizontal and one vertical policy
   * @param [in] type The new alignment option.
   */
  void SetAlignmentType( Type type );

  /**
   * @brief Get the current alignment combined into a single value.
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
   * @return the alignment value.
   */
  Type GetAlignmentType() const;

  /**
   * @brief Sets how added actors scale to fit the alignment's boundary.
   *
   * @see Scaling.
   * @param[in] scaling The scaling property.
   */
  void SetScaling( Scaling scaling );

  /**
   * @brief Retrieves the scaling property.
   *
   * @see Scaling.
   * @return The scaling.
   */
  Scaling GetScaling() const;

  /**
   * @brief Set a padding value.
   *
   * @param [in] padding The left, right, top, bottom padding values.
   */
  void SetPadding( const Padding& padding );

  /**
   * @brief Get the padding values.
   *
   * @return The left, right, top, bottom padding values.
   */
  const Padding& GetPadding() const;

  /**
   * @brief Assignment operator.
   *
   * Changes this handle to point to another real object.
   * @param[in] alignment Object to copy
   * @return A reference to this
   */
  Alignment& operator=(const Alignment& alignment);

public: // Not intended for application developers

  /**
   * @brief Creates a handle using the Toolkit::Internal implementation.
   *
   * @param[in]  implementation  The Control implementation.
   */
  DALI_INTERNAL Alignment( Internal::Alignment& implementation );

  /**
   * @brief Allows the creation of this Control from an Internal::CustomActor pointer.
   *
   * @param[in]  internal  A pointer to the internal CustomActor.
   */
  explicit DALI_INTERNAL Alignment( Dali::Internal::CustomActor* internal );
};

/**
 * @}
 */
} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_LAYOUT_H__
