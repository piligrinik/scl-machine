/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "ImplicationExpressionNode.hpp"

ImplicationExpressionNode::ImplicationExpressionNode(OperandsVector & operands)
{
  for (auto & operand : operands)
    this->operands.emplace_back(std::move(operand));
}

ImplicationExpressionNode::ImplicationExpressionNode(
    ScMemoryContext * context,
    OperatorLogicExpressionNode::OperandsVector & operands)
  : ImplicationExpressionNode(operands)
{
  this->context = context;
}

LogicExpressionResult ImplicationExpressionNode::check(ScTemplateParams & params) const
{
  if (operands.size() != 2)
  {
    SC_LOG_ERROR("Implication should have 2 operands but it has " + to_string(operands.size()));
    return {false, false, {}, ScAddr()};
  }
  LogicExpressionResult premiseResult = operands[0]->check(params);
  LogicExpressionResult conclusionResult = operands[1]->check(params);
}

/**
 * @brief Search premise and get it's replacements. Generate conclusion with found replacements
 * @param result is a LogicFormulaResult{bool: value, value: isGenerated, Replacements: replacements}
 * @return result from param
 */
LogicFormulaResult ImplicationExpressionNode::compute(LogicFormulaResult & result) const
{
  LogicExpressionNode * premiseAtom = operands[0].get();
  premiseAtom->setArgumentVector(argumentVector);

  LogicExpressionNode * conclusionAtom = operands[1].get();
  conclusionAtom->setGenerateOnlyFirst(generateOnlyFirst);
  conclusionAtom->setArgumentVector(argumentVector);

  // Compute premise formula, get replacements with found constructions
  LogicFormulaResult premiseResult = premiseAtom->compute(result);

  // Generate conclusion using computed premise replacements
  LogicFormulaResult conclusionResult = conclusionAtom->generate(premiseResult.replacements);

  // Implication value (a -> b) is equal to ((!a) || b)
  result.value = !premiseResult.value || conclusionResult.value;
  result.isGenerated = conclusionResult.isGenerated;
  if (conclusionResult.value)
  {
    result.replacements =
          ReplacementsUtils::intersectReplacements(premiseResult.replacements, conclusionResult.replacements);
  }

  return result;
}
