export module swtl_test_helper_functions;

import std;
import swtl_vector;

export namespace swtl_test_helpers {

template <typename Type> auto generate_baseline_data() {
  using T = std::remove_cvref_t<Type>;

  return []() -> std::vector<T> {
    if constexpr (std::same_as<T, int>) {
      return {0, 1, 2, 3, 4, 5};
    } else if constexpr (std::same_as<T, double>) {
      return {0.0, 1.1, 2.2, 3.3, 4.4, 5.5};
    } else if constexpr (std::same_as<T, bool>) {
      return {true, false, false, false, true, true,
              true, false, true,  false, true};
    } else if constexpr (std::same_as<T, std::string>) {
      return {
          "one zero, zero zero one one, one zero one zero one",
          "three point one four one five nine two six five three five",
          "eighty-two eighty-two eighty-two, two hundred and forty-six total",
          "we've been trying to reach you about your car's extended warranty"};
    } else {
      throw std::invalid_argument("Missing conditional block to generate "
                                  "values for the passed in Vector type.");
    }
  }();
}

template <typename T> auto generate_populated_swtl_vector() {
  auto const data{generate_baseline_data<T>()};
  return swtl::Vector(data.begin(), data.end());
}

} // namespace swtl_test_helpers
