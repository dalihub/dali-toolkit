#ifndef DALI_TOOLKIT_TEXT_ASYNC_TEXT_LOADER_IMPL_H
#define DALI_TOOLKIT_TEXT_ASYNC_TEXT_LOADER_IMPL_H

/*
 * Copyright (c) 2024 Samsung Electronics Co., Ltd.
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
#include <dali/devel-api/threading/mutex.h>
#include <dali/public-api/common/dali-vector.h>
#include <dali/public-api/object/base-object.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/text/async-text/async-text-loader.h>

#include <dali-toolkit/internal/text/glyph-metrics-helper.h>
#include <dali-toolkit/internal/text/layouts/layout-engine.h>
#include <dali-toolkit/internal/text/layouts/layout-parameters.h>
#include <dali-toolkit/internal/text/rendering/text-typesetter.h>
#include <dali-toolkit/internal/text/text-model.h>

namespace Dali
{
namespace Toolkit
{
namespace Text
{
namespace Internal
{

/**
 * Implementation of the AsyncTextLoader
 */
class AsyncTextLoader : public BaseObject
{
public:
  /**
   * Constructor
   */
  AsyncTextLoader();

  /**
   * Destructor
   */
  ~AsyncTextLoader();

  /**
   * @copydoc Dali::AsyncTextLoader::ClearModule()
   */
  void ClearModule();

  /**
   * @copydoc Dali::AsyncTextLoader::SetModuleClearNeeded()
   */
  void SetModuleClearNeeded(bool clear);

  /**
   * @copydoc Dali::AsyncTextLoader::IsModuleClearNeeded()
   */
  bool IsModuleClearNeeded();


  // Worker thread
  /**
   * @copydoc Dali::AsyncTextLoader::RenderText()
   */
  AsyncTextRenderInfo RenderText(AsyncTextParameters& parameters);

  /**
   * @copydoc Dali::AsyncTextLoader::RenderTextFit()
   */
  AsyncTextRenderInfo RenderTextFit(AsyncTextParameters& parameters);

  /**
   * @copydoc Dali::AsyncTextLoader::RenderAutoScroll()
   */
  AsyncTextRenderInfo RenderAutoScroll(AsyncTextParameters& parameters);

  /**
   * @copydoc Dali::AsyncTextLoader::GetNaturalSize()
   */
  AsyncTextRenderInfo GetNaturalSize(AsyncTextParameters& parameters);

  /**
   * @copydoc Dali::AsyncTextLoader::GetHeightForWidth()
   */
  AsyncTextRenderInfo GetHeightForWidth(AsyncTextParameters& parameters);

private:
  // Worker thread
  /**
   * @brief Initializes internal fields.
   *
   * @param[in] parameters All options required to render text.
   */
  void Initialize();

  /**
   * @brief Clear completely data of the text model.
   */
  void ClearTextModelData();

  /**
   * @brief Update text model to render.
   *
   * @param[in] parameters All options required to render text.
   */
  void Update(AsyncTextParameters& parameters);

  /**
   * @brief Layout the updated text model to render.
   *
   * @param[in] parameters All options required to render text.
   * @param[out] updated true if the text has been laid-out. false means the given width is too small to layout even a single character.
   *
   * @return The size of the text after it has been laid-out.
   */
  Size Layout(AsyncTextParameters& parameters, bool& updated);

  /**
   * @brief Off screend render the updated text model to render.
   *
   * @param[in] parameters All options required to render text.
   */
  AsyncTextRenderInfo Render(AsyncTextParameters& parameters);

  /**
   * @brief Compute natural size of text.
   *
   * @param[in] parameters All options required to compute size of text.
   *
   * @return The natural size of text.
   */
  Size ComputeNaturalSize(AsyncTextParameters& parameters);

  /**
   * @brief Compute height for width of text.
   *
   * @param[in] parameters All options required to compute height of text.
   * @param[in] width The width of text to compute.
   * @param[in] layoutOnly If there is no need to Initialize/Update, only the Layout is performed.
   *
   * @return The height for width of text.
   */
  float ComputeHeightForWidth(AsyncTextParameters& parameters, float width, bool layoutOnly);

  /**
   * @brief Check if the text fits.
   *
   * @param[in] parameters Parameters of the text to check text fit.
   * @param[in] pointSize The point size of the text to check text fit.
   * @param[in] allowedSize The size of the layout to check text fit.
   *
   * @return True if the size of layout performed with parameters information fits, otherwise false.
   */
  bool CheckForTextFit(AsyncTextParameters& parameters, float pointSize, const Size& allowedSize);

private:
  /**
   * private method
   */

private:
  // Undefined copy constructor.
  AsyncTextLoader(const AsyncTextLoader&);

  // Undefined assignment constructor.
  AsyncTextLoader& operator=(const AsyncTextLoader&);

private:
  /**
   * private field
   */
  Text::AsyncTextModule mModule;

  Text::ModelPtr        mTextModel;
  MetricsPtr            mMetrics;
  Text::Layout::Engine  mLayoutEngine;
  Text::TypesetterPtr   mTypesetter;

  Length                mNumberOfCharacters;
  bool                  mFitActualEllipsis      : 1; // Used to store actual ellipses during TextFit calculations. Do not use it in other sections.
  bool                  mIsTextDirectionRTL     : 1; // The direction of the first line after layout completion.
  bool                  mIsTextMirrored         : 1;
  bool                  mModuleClearNeeded      : 1;

  Mutex                 mMutex;
}; // class AsyncTextLoader

inline bool compareByPointSize(DevelTextLabel::FitOption& lhs, DevelTextLabel::FitOption& rhs)
{
  return lhs.GetPointSize() < rhs.GetPointSize();
}

} // namespace Internal

inline static Internal::AsyncTextLoader& GetImplementation(AsyncTextLoader& asyncTextLoader)
{
  DALI_ASSERT_ALWAYS(asyncTextLoader && "async text loader handle is empty");
  BaseObject& handle = asyncTextLoader.GetBaseObject();
  return static_cast<Internal::AsyncTextLoader&>(handle);
}

inline static const Internal::AsyncTextLoader& GetImplementation(const AsyncTextLoader& asyncTextLoader)
{
  DALI_ASSERT_ALWAYS(asyncTextLoader && "async text loader handle is empty");
  const BaseObject& handle = asyncTextLoader.GetBaseObject();
  return static_cast<const Internal::AsyncTextLoader&>(handle);
}

} // namespace Text

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_TEXT_ASYNC_TEXT_LOADER_IMPL_H
