#include <torch/optim/sgd.h>

#include <torch/csrc/autograd/variable.h>

#include <ATen/ATen.h>

#include <functional>

namespace torch {
namespace optim {
SGDOptions::SGDOptions(double learning_rate) : learning_rate_(learning_rate) {}

void SGD::step() {
  for (size_t i = 0; i < parameters_.size(); ++i) {
    auto& grad = parameters_.at(i).grad();
    auto& p = parameters_.at(i).data();

    if (!grad.defined()) {
      continue;
    }

    auto d_p = torch::Tensor(grad).data();
    if (options.weight_decay_ > 0) {
      d_p.add_(p, options.weight_decay_);
    }

    if (options.momentum_ != 0) {
      auto momentum = buffer_at(momentum_buffers_, i).data();
      if (iteration_ == 0) {
        momentum.mul_(options.momentum_).add_(d_p);
      } else {
        momentum.mul_(options.momentum_).add_(d_p, 1 - options.dampening_);
      }
      if (options.nesterov_) {
        d_p = d_p.add(momentum, options.momentum_);
      } else {
        d_p = momentum;
      }
    }

    p.add_(d_p, -options.learning_rate_);
  }
  iteration_ += 1;
}
} // namespace optim
} // namespace torch
