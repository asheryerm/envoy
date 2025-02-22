#pragma once

#include <memory>
#include <string>

#include "envoy/matcher/matcher.h"

#include "source/common/protobuf/utility.h"
#include "source/extensions/matching/input_matchers/cel_matcher/matcher.h"

#include "xds/type/matcher/v3/cel.pb.h"

namespace Envoy {
namespace Extensions {
namespace Matching {
namespace InputMatchers {
namespace CelMatcher {

class CelInputMatcherFactory : public ::Envoy::Matcher::InputMatcherFactory {
public:
  InputMatcherFactoryCb
  createInputMatcherFactoryCb(const Protobuf::Message& config,
                              Server::Configuration::ServerFactoryContext&) override {
    if (expr_builder_ == nullptr) {
      expr_builder_ = Extensions::Filters::Common::Expr::createBuilder(nullptr);
    }

    const auto& cel_matcher_config =
        dynamic_cast<const ::xds::type::matcher::v3::CelMatcher&>(config);
    CelMatcherSharedPtr cel_matcher =
        std::make_shared<::xds::type::matcher::v3::CelMatcher>(cel_matcher_config);

    return [cel_matcher = std::move(cel_matcher), this] {
      return std::make_unique<CelInputMatcher>(cel_matcher, *expr_builder_);
    };
  }

  ProtobufTypes::MessagePtr createEmptyConfigProto() override {
    return std::make_unique<CelMatcher>();
  }

  std::string name() const override { return "envoy.matching.matchers.cel_matcher"; }

private:
  // Expression builder must outlive the compiled expression.
  BuilderPtr expr_builder_;
};

} // namespace CelMatcher
} // namespace InputMatchers
} // namespace Matching
} // namespace Extensions
} // namespace Envoy
