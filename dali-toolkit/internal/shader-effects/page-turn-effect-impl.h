#ifndef __DALI_TOOLKIT_INTERNAL_PAGE_TURN_EFFECT_H__
#define __DALI_TOOLKIT_INTERNAL_PAGE_TURN_EFFECT_H__

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

// EXTERNAL INCLUDES
#include <dali/public-api/animation/constraint.h>

// INTERNAL INCLUDES
#include <dali-toolkit/devel-api/shader-effects/page-turn-effect.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

/**
 * PageTurnEffect implementation class
 */
class PageTurnEffect : public ShaderEffect::Extension
{
public:

  PageTurnEffect();
  /**
   * Virtual destructor.
   */
  virtual ~PageTurnEffect();

  /**
   * @copydoc Dali::Toolkit::PageTurnEffect::New
   */
  static Toolkit::PageTurnEffect CreateShaderEffect( bool enableBlending );

  /**
   * @copydoc Dali::Toolkit::PageTurnEffect::SetPageSize
   */
  void SetPageSize(const Vector2& pageSize);

  /**
   * @copydoc Dali::Toolkit::PageTurnEffect::SetOriginalCenter
   */
  void SetOriginalCenter(const Vector2& originalCenter);

  /**
   * @copydoc Dali::Toolkit::PageTurnEffect::SetCurrentCenter
   */
  void SetCurrentCenter(const Vector2& currentCenter);

  /**
   * @copydoc Dali::Toolkit::PageTurnEffect::SetIsTurningBack
   */
  void SetIsTurningBack(bool isTurningBack);

  /**
   * @copydoc Dali::Toolkit::PageTurnEffect::SetShadowWidth
   */
  void SetShadowWidth(float shadowWidth);

  /**
   *@copydoc Dali::Toolkit::PageTurnEffect::SetSpineShadowParameter
   */
  void SetSpineShadowParameter(const Vector2& spineShadowParameter);

  /**
   * The internal constraint uses the OriginalCenter property and the CurrentCenter Property
   * to update the variety of common parameters which are with the same value for all the vertices.
   * Note: For each actor, the constraints are applied in the same order as the calls to Actor::ApplyConstraint().
   * So if there are other contraints applied to the OriginalCenter or CurrentCenter while when using this effect,
   * call this method to get the internal constraints and re-apply it afterwards.
   */
  void ApplyInternalConstraint();

  /**
   * @copydoc Dali::Toolkit::PageTurnEffect::GetPageSizePropertyName
   */
  const std::string& GetPageSizePropertyName() const;

  /**
  * @copydoc Dali::Toolkit::PageTurnEffect::GetOriginalCenterPropertyName
  */
  const std::string& GetOriginalCenterPropertyName() const;

  /**
   * @copydoc Dali::Toolkit::PageTurnEffect::GetCurrentCenterPropertyName
   */
  const std::string& GetCurrentCenterPropertyName() const;

private:

  void Initialize( ShaderEffect shaderEffect );

private:
  ShaderEffect mShaderEffect;

  Property::Index mOriginalCenterPropertyIndex;
  Property::Index mCurrentCenterPropertyIndex;

private:
  //undefined copy constructor.
  PageTurnEffect( const PageTurnEffect& );

  //Undefined assignment operator.
  PageTurnEffect& operator=( const PageTurnEffect& );

};

} // namespace Internal

//Helpers for public-api forwarding methods
inline Toolkit::Internal::PageTurnEffect& GetImpl( Toolkit::PageTurnEffect& effect )
{
  DALI_ASSERT_ALWAYS( effect );
  return static_cast<Internal::PageTurnEffect&>( effect.GetExtension() );
}

inline const Toolkit::Internal::PageTurnEffect& GetImpl( const Toolkit::PageTurnEffect& effect )
{
  DALI_ASSERT_ALWAYS( effect );
  return static_cast<const Internal::PageTurnEffect&>( effect.GetExtension() );
}

} // namespace Toolkit

} // namespace Dali

#endif /* __DALI_TOOLKIT_INTERNAL_PAGE_TURN_EFFECT_H__*/
