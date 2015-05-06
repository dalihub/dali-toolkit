#ifndef __DALI_TOOLKIT_INTERNAL_SCROLL_VIEW_PAGE_PATH_EFFECT_H__
#define __DALI_TOOLKIT_INTERNAL_SCROLL_VIEW_PAGE_PATH_EFFECT_H__

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

// EXTERNAL INCLUDES
#include <dali/public-api/animation/linear-constrainer.h>
#include <dali/public-api/animation/path-constrainer.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/controls/scrollable/scroll-view/scroll-view-effect-impl.h>
#include <dali-toolkit/public-api/controls/scrollable/scroll-view/scroll-view-page-path-effect.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

/**
 * @copydoc Toolkit::ScrollViewPagePathEffect
 */
class ScrollViewPagePathEffect : public ScrollViewEffect
{

public:

  /**
   * Constructor
   * @param[in] path Pages will follow this path
   * @param[in] forward Vector in page local space which will be aligned with tangent of the path
   * @param[in] inputPropertyIndex index of the property in the scrollview used to drivce the path
   * @param[in] pageSize size of a page in the scrollview
   * @param[in] pageCount total number of pages in the scrollview
   */
  ScrollViewPagePathEffect(Path path, const Vector3& forward, Dali::Property::Index inputPropertyIndex, const Vector3& pageSize, unsigned int pageCount );

public:

  /**
   * @copydoc ScrollViewEffect::ApplyToActor
   */
  void ApplyToPage( Actor child, unsigned int pageOrder );

public:

  /**
   * @copydoc ScrollViewEffect::OnAttach
   */
  virtual void OnAttach( Toolkit::ScrollView& scrollView );

  /**
   * @copydoc ScrollViewEffect::OnDetach
   */
  virtual void OnDetach( Toolkit::ScrollView& scrollView );

protected:

  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~ScrollViewPagePathEffect();

private:

  Vector3               mPageSize;            ///< The logical page size for the 3D effect.
  PathConstrainer       mPathConstrainer;     ///< PathConstrainer used to constraint position and orientation
  LinearConstrainer     mLinearConstrainer;   ///< LinearConstrainer used to constraint opacity
  Dali::Property::Index mInputPropertyIndex;  ///< Index of the property in the scrollview used as the parameter for the path
  unsigned int          mPageCount;           ///< Total number of pages (Needed for wrapping)
};

} // namespace Internal

// Helpers for public-api forwarding methods

inline Internal::ScrollViewPagePathEffect& GetImpl(Dali::Toolkit::ScrollViewPagePathEffect& obj)
{
  DALI_ASSERT_ALWAYS(obj);

  Dali::RefObject& handle = obj.GetBaseObject();

  return static_cast<Internal::ScrollViewPagePathEffect&>(handle);
}

inline const Internal::ScrollViewPagePathEffect& GetImpl(const Dali::Toolkit::ScrollViewPagePathEffect& obj)
{
  DALI_ASSERT_ALWAYS(obj);

  const Dali::RefObject& handle = obj.GetBaseObject();

  return static_cast<const Internal::ScrollViewPagePathEffect&>(handle);
}

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_INTERNAL_SCROLL_VIEW_PAGE_CAROUSEL_EFFECT_H__
