#include <iostream>
#include <ipr/impl>
#include <ipr/io>

//  1 template <typename T>
//  2 struct S;
//  3
//  4 template <typename T>
//  5 struct S {
//  6   template <typename U>
//  7   struct Q;
//  8 };
//  9
// 10 template <typename T>
// 11 template <typename U>
// 12 struct S<T>::Q {};
// 13
// 14 template <>
// 15 template <typename U>
// 16 struct S<int>::Q {};
// 17
// 18 template <>
// 19 template <>
// 20 struct S<int>::Q<int> {};

using namespace ipr;

namespace {
	struct builder
	{
		Printer pp{std::cout};

		impl::Lexicon fac;
		impl::Translation_unit tu {fac};
		impl::Region *active_region = tu.global_region();

		void build_forward_declaration_of_S();
	};
}
//  1 template <typename T>
//  2 struct S;

void builder::build_forward_declaration_of_S() {

}

int main() {
	std::cout << "Hi\n";
	builder b;
}
