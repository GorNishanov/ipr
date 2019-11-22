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
   struct builder {
      Printer pp{std::cout};

      impl::Lexicon fac;
      impl::Translation_unit tu{fac};
      impl::Region* active_region = tu.global_region();
      File_index file_index = fac.get_fileindex(fac.get_string("<source>"));

      void build_forward_declaration_of_S();
      void build_definition_of_S();

      builder() {
         build_forward_declaration_of_S();
         build_definition_of_S();
      }
   };
} // namespace
//  1 template <typename T>
//  2 struct S; -- ; is at pos <2:9>
void builder::build_forward_declaration_of_S() {
   auto* mapping = fac.make_mapping(*active_region);
   auto* T = fac.make_parameter(fac.get_identifier("T"), fac.typename_type(), *mapping);
   auto& templ = fac.get_template(mapping->parameters.type(), fac.class_type());
   mapping->constraint = &templ;
   pp << "template-type: " << templ << " " << templ.node_id << "\n";

   auto* S = active_region->declare_primary_map(fac.get_identifier("S"), templ);
   S->init = mapping;
   S->args.push_back(T);
   S->src_locus = ipr::Source_location{{Line_number{1}, Column_number{1}}, file_index};
   pp << *S << "\n";
}

//  4 template <typename T>
//  5 struct S {
//  6   template <typename U>
//  7   struct Q;
//  8 };
void builder::build_definition_of_S() {
   auto* mapping = fac.make_mapping(*active_region);
   auto* T = fac.make_parameter(fac.get_identifier("T"), fac.typename_type(), *mapping);
   auto& templ = fac.get_template(mapping->parameters.type(), fac.class_type());
   mapping->constraint = &templ;
   pp << "template-type: " << templ << " " << templ.node_id << "\n";

   auto* S = active_region->declare_primary_map(fac.get_identifier("S"), templ);
   S->init = mapping;
   S->args.push_back(T);
   S->src_locus = ipr::Source_location{{Line_number{4}, Column_number{1}}, file_index};

   auto* body = fac.make_class(mapping->params());
   mapping->body = body;

   auto& n = *fac.make_id_expr(*S);
   auto& a = S->args;

   body->id = &fac.get_template_id(n, a);
   pp << *S << "\n";
}

int main() {
   std::cout << "Hi\n";
   builder b;
}
