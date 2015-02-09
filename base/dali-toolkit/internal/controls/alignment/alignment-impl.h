#ifndef __DALI_TOOLKIT_INTERNAL_ALIGNMENT_H__
#define __DALI_TOOLKIT_INTERNAL_ALIGNMENT_H__

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
#include <dali-toolkit/public-api/controls/control-impl.h>
#include <dali-toolkit/public-api/controls/alignment/alignment.h>

namespace Dali
{

namespace Toolkit
{

class Alignment;

namespace Internal
{

/**
 * Alignment is a control to position and resize actors inside other container actors.
 * @see Dali::Toolkit::Alignment for more details.
 */
class Alignment : public Control
{
public:

  /**
   * Create an initialized Alignment.
   * @param type Type of alignment.
   * @return A handle to a newly allocated Dali resource.
   */
  static Toolkit::Alignment New( Toolkit::Alignment::Type horizontal, Toolkit::Alignment::Type vertical );

  /**
   * @copydoc Dali::Toolkit::Alignment::SetAlignmentType()
   */
  void SetAlignmentType( Toolkit::Alignment::Type type );

  /**
   * @copydoc Dali::Toolkit::Alignment::GetAlignmentType()
   */
  Toolkit::Alignment::Type GetAlignmentType() const;

  /**
   * @copydoc Dali::Toolkit::Alignment::SetScaling()
   */
  void SetScaling( Toolkit::Alignment::Scaling scaling );

  /**
   * @copydoc Dali::Toolkit::Alignment::GetScaling()
   */
  Toolkit::Alignment::Scaling GetScaling() const;

  /**
   * @copydoc Dali::Toolkit::Alignment::SetPadding()
   */
  void SetPadding( const Toolkit::Alignment::Padding& padding );

  /**
   * @copydoc Dali::Toolkit::Alignment::GetPadding()
   */
  const Toolkit::Alignment::Padding& GetPadding() const;


private: // From Control

  /**
   * @copydoc Control::OnRelayout()
   */
  virtual void OnRelayout( const Vector2& size, ActorSizeContainer& container );

private:

  /**
   * Constructor.
   * It initializes Alignment members.
   */
  Alignment( Toolkit::Alignment::Type horizontal, Toolkit::Alignment::Type vertical );

  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~Alignment();

private:

  // Undefined
  Alignment(const Alignment&);
  Alignment& operator=(const Alignment&);

private:
  Toolkit::Alignment::Type    mHorizontal; ///< Type of alignment.
  Toolkit::Alignment::Type    mVertical;   ///< Type of alignment.
  Toolkit::Alignment::Scaling mScaling;    ///< Stores the geometry scaling.
  Toolkit::Alignment::Padding mPadding;    ///< Stores the padding values.
};

} // namespace Internal


// Helpers for public-api forwarding methods

inline Toolkit::Internal::Alignment& GetImpl( Toolkit::Alignment& alignment )
{
  DALI_ASSERT_ALWAYS( alignment );

  Dali::RefObject& handle = alignment.GetImplementation();

  return static_cast<Toolkit::Internal::Alignment&>( handle );
}

inline const Toolkit::Internal::Alignment& GetImpl( const Toolkit::Alignment& alignment )
{
  DALI_ASSERT_ALWAYS( alignment );

  const Dali::RefObject& handle = alignment.GetImplementation();

  return static_cast<const Toolkit::Internal::Alignment&>( handle );
}

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_INTERNAL_ALIGNMENT_H__
