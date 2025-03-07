/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include <vector>

#include <sc-memory/sc_memory.hpp>

#include "inference/template_manager_abstract.hpp"

namespace inference
{
class TemplateManager : public TemplateManagerAbstract
{
public:
  explicit TemplateManager(ScMemoryContext * ms_context);

  std::vector<ScTemplateParams> CreateTemplateParams(ScAddr const & scTemplate) override;
};
}  // namespace inference
