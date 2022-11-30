/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include <sc-agents-common/utils/IteratorUtils.hpp>
#include <sc-agents-common/utils/AgentUtils.hpp>
#include <sc-agents-common/keynodes/coreKeynodes.hpp>

#include "keynodes/InferenceKeynodes.hpp"

#include "DirectInferenceAgent.hpp"

using namespace scAgentsCommon;

namespace inference
{
SC_AGENT_IMPLEMENTATION(DirectInferenceAgent)
{
  if (!edgeAddr.IsValid())
    return SC_RESULT_ERROR;

  ScAddr actionNode = ms_context->GetEdgeTarget(edgeAddr);
  if (!checkActionClass(actionNode))
    return SC_RESULT_OK;

  SC_LOG_DEBUG("DirectInferenceAgent started");

  ScAddr const targetStructure =
      utils::IteratorUtils::getAnyByOutRelation(ms_context.get(), actionNode, CoreKeynodes::rrel_1);
  ScAddr const formulasSet =
      utils::IteratorUtils::getAnyByOutRelation(ms_context.get(), actionNode, CoreKeynodes::rrel_2);
  ScAddr const inputStructure =
      utils::IteratorUtils::getAnyByOutRelation(ms_context.get(), actionNode, CoreKeynodes::rrel_3);
  ScAddr const rrel_4 = utils::IteratorUtils::getRoleRelation(ms_context.get(), 4);
  ScAddr outputStructure = utils::IteratorUtils::getAnyByOutRelation(ms_context.get(), actionNode, rrel_4);

  if (!targetStructure.IsValid() || !utils::IteratorUtils::getAnyFromSet(ms_context.get(), targetStructure).IsValid())
  {
    SC_LOG_WARNING("Target structure is not valid or empty.");
  }
  if (!formulasSet.IsValid() || !utils::IteratorUtils::getAnyFromSet(ms_context.get(), formulasSet).IsValid())
  {
    SC_LOG_ERROR("Formulas set is not valid or empty.");
    utils::AgentUtils::finishAgentWork(ms_context.get(), actionNode, false);
    return SC_RESULT_ERROR;
  }
  if (!inputStructure.IsValid() || !utils::IteratorUtils::getAnyFromSet(ms_context.get(), inputStructure).IsValid())
  {
    SC_LOG_ERROR("Input structure is not valid or empty.");
    utils::AgentUtils::finishAgentWork(ms_context.get(), actionNode, false);
    return SC_RESULT_ERROR;
  }
  if (!outputStructure.IsValid())
  {
    SC_LOG_WARNING("Output structure is not valid or empty, generate new");
    outputStructure = ms_context->CreateNode(ScType::NodeConstStruct);
  }

  ScAddrVector answerElements;
  this->inferenceManager = std::make_unique<DirectInferenceManager>(ms_context.get());
  ScAddr solutionNode;
  try
  {
    solutionNode =
        this->inferenceManager->applyInference(targetStructure, formulasSet, inputStructure, outputStructure);
  }
  catch (utils::ScException const & exception)
  {
    SC_LOG_ERROR(exception.Message());
    utils::AgentUtils::finishAgentWork(ms_context.get(), actionNode, false);
    return SC_RESULT_ERROR;
  }

  answerElements.push_back(solutionNode);
  utils::AgentUtils::finishAgentWork(ms_context.get(), actionNode, answerElements, true);
  SC_LOG_DEBUG("DirectInferenceAgent finished");
  return SC_RESULT_OK;
}

bool DirectInferenceAgent::checkActionClass(ScAddr const & actionNode)
{
  return ms_context->HelperCheckEdge(
      InferenceKeynodes::action_direct_inference, actionNode, ScType::EdgeAccessConstPosPerm);
}

}  // namespace inference
