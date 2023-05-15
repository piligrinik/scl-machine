/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include "sc-memory/sc_memory.hpp"
#include "sc-memory/sc_addr.hpp"

#include "searcher/TemplateSearcherAbstract.hpp"
#include "manager/SolutionTreeManager.hpp"
#include "manager/TemplateManager.hpp"
#include "logic/LogicExpressionNode.hpp"
#include "inferenceConfig/InferenceConfig.hpp"

namespace inference
{
using ScAddrQueue = std::queue<ScAddr>;

/// Class to control formulas iteration strategy: when to stop applying rules and how to iterate over them
class FormulasIterationStrategyAbstract
{
public:
  explicit FormulasIterationStrategyAbstract(ScMemoryContext * context);

  virtual ~FormulasIterationStrategyAbstract() = default;

  void setTemplateSearcher(std::shared_ptr<TemplateSearcherAbstract> searcher);
  void setTemplateManager(std::shared_ptr<TemplateManagerAbstract> manager);
  void setSolutionTreeManager(std::shared_ptr<SolutionTreeManagerAbstract> manager);

  std::shared_ptr<SolutionTreeManagerAbstract> getSolutionTreeManager();

  virtual bool applyIterationStrategy(InferenceParamsConfig const & inferenceParamsConfig) = 0;

  // TODO: Need to implement common logic of inference rules (e.g. modus ponens)
  LogicFormulaResult useFormula(
        ScAddr const & formula,
        ScAddr const & outputStructure);

  void fillFormulaFixedArgumentsIdentifiers(ScAddr const & formula, ScAddr const & firstFixedArgument) const;

  void formTemplateManagerFixedArguments(ScAddr const & formula, ScAddr const & firstFixedArgument);
  void resetTemplateManager(std::shared_ptr<TemplateManagerAbstract> otherTemplateManager);

  vector<ScAddrQueue> createFormulasQueuesListByPriority(ScAddr const & formulasSet);

  ScAddrQueue createQueue(ScAddr const & set);

protected:
  ScMemoryContext * context;

  std::shared_ptr<TemplateManagerAbstract> templateManager;
  std::shared_ptr<TemplateSearcherAbstract> templateSearcher;
  std::shared_ptr<SolutionTreeManagerAbstract> solutionTreeManager;

  std::unordered_set<ScAddr, ScAddrHashFunc<::size_t>> outputStructureElements;
};
} // inference
