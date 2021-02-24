#ifndef DALI_TOOLKIT_WEB_FORM_REPOST_DECISION_H
#define DALI_TOOLKIT_WEB_FORM_REPOST_DECISION_H

/*
 * Copyright (c) 2021 Samsung Electronics Co., Ltd.
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
#include <memory>
#include <string>

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/dali-toolkit-common.h>

namespace Dali
{
class WebEngineFormRepostDecision;

namespace Toolkit
{
/**
 * @addtogroup dali_toolkit_controls_web_view
 * @{
 */

/**
 * @brief WebFormRepostDecision is a class for form repost decision of WebView.
 *
 *
 * For working WebFormRepostDecision, a Dali::WebEngineFormRepostDecision should be provided.
 *
 */
class DALI_TOOLKIT_API WebFormRepostDecision
{
public:
  /**
   * @brief Creates a WebFormRepostDecision.
   */
  WebFormRepostDecision(std::shared_ptr<Dali::WebEngineFormRepostDecision> decision);

  /**
   * @brief Destructor.
   */
  virtual ~WebFormRepostDecision() final;

  /**
   * @brief Reply the result about form repost decision.
   *
   * @param[in] allowed Whether allow form repost decision request or not
   */
  void Reply(bool allowed);

private:
  std::shared_ptr<Dali::WebEngineFormRepostDecision> mFormRepostDecision;
};

/**
 * @}
 */

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_WEB_FORM_REPOST_DECISION_H
