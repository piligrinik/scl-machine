/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include "inference/inference_manager_abstract.hpp"

namespace inference
{
class InferenceManagerFactory
{
public:
  static std::unique_ptr<InferenceManagerAbstract> ConstructDirectInferenceManagerAll(
      ScMemoryContext * context,
      InferenceConfig const & inferenceFlowConfig);

  static std::unique_ptr<InferenceManagerAbstract> ConstructDirectInferenceManagerTarget(
      ScMemoryContext * context,
      InferenceConfig const & inferenceFlowConfig);
};
}  // namespace inference
