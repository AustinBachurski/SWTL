export module swtl_test_helper_functions;

import std;

export namespace swtl_test_helpers {

template <typename T> auto generate_baseline_data() -> std::vector<T> {
  return []() {
    if constexpr (std::same_as<T, int>) {
      return VectorType{0, 1, 2, 3, 4, 5};
    } else if constexpr (std::same_as<T, double>) {
      return VectorType{0.0, 1.1, 2.2, 3.3, 4.4, 5.5};
    } else if constexpr (std::same_as<T, bool>) {
      return VectorType{true, false, false, false, true, true,
                        true, false, true,  false, true};
    } else if constexpr (std::same_as<T, std::string>) {
      return VectorType{
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

template <typename VectorType> auto generate_populated_vector() {
  using T = typename std::remove_const_t<VectorType>::value_type;
  auto const data{generate_baseline_data<T>()};
  return VectorType(data.begin(), data.end());
}

} // namespace swtl_test_helpers
