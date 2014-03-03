#ifndef __DALI_ALIGNMENT_H__
#define __DALI_ALIGNMENT_H__

//
// Copyright (c) 2014 Samsung Electronics Co., Ltd.
//
// Licensed under the Flora License, Version 1.0 (the License);
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://floralicense.org/license/
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an AS IS BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

/**
 * @addtogroup CAPI_DALI_FRAMEWORK
 * @{
 */

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/controls/control.h>

namespace Dali DALI_IMPORT_API
{

namespace Toolkit
{

namespace Internal DALI_INTERNAL
{
class Alignment;
}

/**
 * Alignment is a container which provides an easy way to align other actors inside its boundary.
 *
 * Additionaly it provides a scaling property to resize the contained actors @see Scaling.
 * @note The use of scaling property will override all constraints applied to actors.
 *
 * All actors added to an alignment are going to be set with the same anchor point and parent origin. And, if the scaling property is set to a value
 * different than ScaleNone, constraints as well.
 */
class Alignment : public Control
{
public:
  /**
   * Different types of alignment.
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
   * Scaling determines how actors are scaled, to match the alignment's boundary.
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

  struct Padding
  {
    Padding()
    : left( 0.f ),
      right( 0.f ),
      top( 0.f ),
      bottom( 0.f )
    {
    }

    Padding( float l, float r, float t, float b )
    : left( l ),
      right( r ),
      top( t ),
      bottom( b )
    {
    }

    float left;
    float right;
    float top;
    float bottom;
  };

  /**
   * Create an Alignment handle; this can be initialised with Alignment::New()
   * Calling member functions with an uninitialised handle is not allowed.
   */
  Alignment();

  /**
   * Creates an alignment control.
   * @param [in] horizontal Specifies how to align actors horizontally. Could be HorizontalLeft, HorizontalCenter or HorizontalRight. By default HorizontalCenter.
   * @param [in] vertical Specifies how to align actors vertically. Could be VerticalTop, VerticalCenter or VerticalBottom. By default VerticalCenter.
   * @return A handle to the Alignment control.
   */
  static Alignment New( Type horizontal = HorizontalCenter, Type vertical = VerticalCenter );

  /**
   * Copy constructor. Creates another handle that points to the same real object.
   */
  Alignment(const Alignment& alignment);

  /**
   * Virtual destructor.
   * Dali::Object derived classes typically do not contain member data.
   */
  virtual ~Alignment();

  /**
   * Downcast an Object handle to Alignment. If handle points to a Alignment the
   * downcast produces valid handle. If not the returned handle is left uninitialized.
   * @param[in] handle Handle to an object
   * @return handle to a Alignment or an uninitialized handle
   */
  static Alignment DownCast( BaseHandle handle );

  /**
   * Sets the new alignment. By default ( HorizontalCenter | VerticalCenter ).
   * @param [in] type The new alignment option.
   */
  void SetAlignmentType( Type type );

  /**
   * Get the current alignment combined into a single value.
   * The values can be tested by using the & operator
   * and the desired flag. e.g. if (GetAlignmentType() & HorizontalCentre) ...
   * @return the alignment value.
   */
  Type GetAlignmentType() const;

  /**
   * Sets how added actors scale to fit the alignment's boundary.
   * @see Scaling.
   * @param[in] scaling The scaling property.
   */
  void SetScaling( Scaling scaling );

  /**
   * Retrieves the scaling property.
   * @see Scaling.
   * @return The scaling.
   */
  Scaling GetScaling() const;

  /**
   * Set a padding value.
   *
   * @param [in] padding The left, right, top, bottom padding values.
   */
  void SetPadding( const Padding& padding );

  /**
   * @return The left, right, top, bottom padding values.
   */
  const Padding& GetPadding() const;

  /**
   * Assignment operator. Changes this handle to point to another real object.
   */
  Alignment& operator=(const Alignment& alignment);

public: // Not intended for application developers

  /**
   * Creates a handle using the Toolkit::Internal implementation.
   * @param[in]  implementation  The Control implementation.
   */
  Alignment( Internal::Alignment& implementation );

  /**
   * Allows the creation of this Control from an Internal::CustomActor pointer.
   * @param[in]  internal  A pointer to the internal CustomActor.
   */
  Alignment( Dali::Internal::CustomActor* internal );
};

} // namespace Toolkit

} // namespace Dali

/**
 * @}
 */
#endif // __DALI_TOOLKIT_LAYOUT_H__
