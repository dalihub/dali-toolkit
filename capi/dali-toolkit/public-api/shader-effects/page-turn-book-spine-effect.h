#ifndef __DALI_PAGE_TURN_BOOK_SPINE_EFFECT_H__
#define __DALI_PAGE_TURN_BOOK_SPINE_EFFECT_H__

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
#include <dali/dali.h>

namespace Dali DALI_IMPORT_API
{

namespace Toolkit
{
/**
 * It is an assisting effect of PageTurnEffect to display book spine on STATIC pages, and also flip the image horizontally when needed.
 * When the page is turned over in landscape, call SetIsBackImageVisible(true), this effect can display the back image correctly after the imageActor been rotated 180 degrees.
 * To display the pages visually consistent with its turning state, please set the uniforms with the same values as the PageTurnEffect
 **/
class PageTurnBookSpineEffect : public ShaderEffect
{
public:
  /**
   * Create an uninitialized PageTurnBookSpineEffect; this can be initialized with PageTurnBookSpineEffect::New()
   * Calling member functions with an uninitialized Dali::Object is not allowed.
   */
  PageTurnBookSpineEffect();

  /**
   * Virtual destructor.
   */
  virtual ~PageTurnBookSpineEffect();

  /**
   * Create an initialized PageTurnBookSpineEffect.
   * @return A handle to a newly allocated Dali resource.
   */
  static PageTurnBookSpineEffect New();

  /**
   * Set whether the current page is with its backside visible
   * Need to pass the parameter as true for the page which is turned over but still visible in Landscape
   * @param [in] isBackVisible True for page with its backside upwards
   */
  void SetIsBackImageVisible( bool isBackVisible );

  /**
   * Set the page width of the PageTurnBookSpineEffect.
   * @param [in] pageWidth The width of the page size.
   */
  void SetPageWidth( float pageWidth );

  /**
   * Set the width of shadow to be pageSize * shadowWidth
   * this shadow appears at the edges of the actor which is not visible on static pages
   * @param [in] shadowWidth The width for the simulated shadow
   */
  void SetShadowWidth( float shadowWidth );

  /**
   * Set the spine shadow parameter
   * The two parameters are the major&minor radius (in pixels) to form an ellipse shape
   * The top-left quarter of this ellipse is used to calculate spine normal for simulating shadow
   * @param [in] spineShadowParameter The major&minor ellipse radius for the simulated spine shadow
   */
  void SetSpineShadowParameter( const Vector2& spineShadowParameter );


private:// Helper for New()
  PageTurnBookSpineEffect( ShaderEffect handle );

}; // End of PageTurnBookSpineEffect class

} // namespace Toolkit

} // namespace Dali

/**
 * @}
 */
#endif /* __DALI_PAGE_TURN_BOOK_SPINE_EFFECT_H__ */
