/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include "inference/template_manager_abstract.hpp"

#include <vector>

#include <sc-memory/sc_memory.hpp>

namespace inference
{
class TemplateManagerFixedArguments : public TemplateManagerAbstract
{
public:
  explicit TemplateManagerFixedArguments(ScMemoryContext * context);

  std::vector<ScTemplateParams> CreateTemplateParams(ScAddr const & scTemplate) override;
};
}  // namespace inference
