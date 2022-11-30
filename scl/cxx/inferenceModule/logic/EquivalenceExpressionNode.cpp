/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "EquivalenceExpressionNode.hpp"

EquivalenceExpressionNode::EquivalenceExpressionNode(OperandsVector & operands)
{
  for (auto & operand : operands)
    this->operands.emplace_back(std::move(operand));
}

EquivalenceExpressionNode::EquivalenceExpressionNode(
    ScMemoryContext * context,
    OperatorLogicExpressionNode::OperandsVector & operands)
  : EquivalenceExpressionNode(operands)
{
  this->context = context;
}

// TODO(MksmOrlov): Need to implement
LogicExpressionResult EquivalenceExpressionNode::check(ScTemplateParams & params) const
{
  return {};
}

LogicFormulaResult EquivalenceExpressionNode::compute(LogicFormulaResult & result) const
{
  vector<LogicFormulaResult> subFormulaResults;
  result.value = false;
  FormulaClassifier formulaClassifier(context);

  vector<TemplateExpressionNode *> formulasWithoutConstants;
  vector<TemplateExpressionNode *> formulasToGenerate;
  for (auto const & operand : operands)
  {
    auto atom = dynamic_cast<TemplateExpressionNode *>(operand.get());
    if (atom)
    {
      if (!formulaClassifier.isFormulaWithConst(atom->getFormulaTemplate()))
      {
        SC_LOG_DEBUG("Found formula without constants in equivalence");
        formulasWithoutConstants.push_back(atom);
        continue;
      }
      if (formulaClassifier.isFormulaToGenerate(atom->getFormulaTemplate()))
      {
        SC_LOG_DEBUG("Found formula to generate in equivalence");
        formulasToGenerate.push_back(atom);
        continue;
      }
    }
    subFormulaResults.push_back(operand->compute(result));
  }
  SC_LOG_DEBUG("Processed " + to_string(subFormulaResults.size()) + " formulas in equivalence");
  if (subFormulaResults.empty())
  {
    SC_LOG_ERROR("All sub formulas in equivalence are either don't have constants or supposed to be generated");
    throw std::exception();
  }

  if (!formulasWithoutConstants.empty())
  {
    SC_LOG_DEBUG("Processing formula without constants");
    auto formulaWithoutConstants = formulasWithoutConstants[0];
    subFormulaResults.push_back(formulaWithoutConstants->find(subFormulaResults[0].replacements));
  }

  if (!formulasToGenerate.empty())
  {
    SC_LOG_DEBUG("Processing formula to generate");
    auto formulaToGenerate = formulasToGenerate[0];
    subFormulaResults.push_back(formulaToGenerate->generate(subFormulaResults[0].replacements));
  }
  result.value = subFormulaResults[0].value == subFormulaResults[1].value;
  if (result.value)
    result.replacements =
        ReplacementsUtils::intersectReplacements(subFormulaResults[0].replacements, subFormulaResults[1].replacements);
  return result;

  auto leftAtom = dynamic_cast<TemplateExpressionNode *>(operands[0].get());
  bool isLeftGenerated = (leftAtom) && formulaClassifier.isFormulaToGenerate(leftAtom->getFormulaTemplate());

  auto rightAtom = dynamic_cast<TemplateExpressionNode *>(operands[1].get());
  bool isRightGenerated = (rightAtom) && formulaClassifier.isFormulaToGenerate(rightAtom->getFormulaTemplate());

  bool leftHasConstants = (leftAtom) && formulaClassifier.isFormulaWithConst(leftAtom->getFormulaTemplate());
  bool rightHasConstants = (rightAtom) && formulaClassifier.isFormulaWithConst(rightAtom->getFormulaTemplate());

  SC_LOG_DEBUG("Left has constants = " + to_string(leftHasConstants));
  SC_LOG_DEBUG("Right has constants = " + to_string(rightHasConstants));

  LogicFormulaResult leftResult;
  LogicFormulaResult rightResult;

  if (!isLeftGenerated)
  {
    SC_LOG_DEBUG("*** Left part of equivalence shouldn't be generated");
    leftResult = operands[0]->compute(result);
    rightResult = (isRightGenerated ? rightAtom->generate(leftResult.replacements) : operands[1]->compute(result));
  }
  else
  {
    if (isRightGenerated)
    {
      SC_LOG_DEBUG("*** Right part should be generated");
      return {true, {}};
    }
    else
    {
      SC_LOG_DEBUG("*** Right part shouldn't be generated");
      rightResult = operands[1]->compute(result);
      leftResult = leftAtom->generate(rightResult.replacements);
    }
  }

  result.value = leftResult.value == rightResult.value;
  if (rightResult.value)
    result.replacements = ReplacementsUtils::intersectReplacements(leftResult.replacements, rightResult.replacements);
  return result;
}
