#include <iostream>
#include <ipr/io>
#include <ipr/impl>

using namespace ipr;

struct Sema;
struct Parameter_list_builder {
    Sema& sema;
    impl::Mapping& mapping;

    impl::Parameter* add_parameter(const ipr::Name& n, const ipr::Type& t);
    impl::Mapping& commit_template(const ipr::Type& t);
};

struct Sema : impl::Lexicon {
    impl::Translation_unit tu;
    impl::Region* active_region;

    Sema() : tu(*this), active_region(tu.global_region()) {}

    Parameter_list_builder act_on_Parameter_list_start() {
        return {*this, *make_mapping(*active_region)};
    }
};

impl::Parameter* Parameter_list_builder::add_parameter(const ipr::Name& n, const ipr::Type& t) {
    return sema.make_parameter(n, t, mapping);
}

impl::Mapping& Parameter_list_builder::commit_template(const ipr::Type& t) {
    auto& templ = sema.get_template(mapping.parameters.type(), t);
    mapping.constraint = &templ;
    return mapping;
}

/*
  template <typename T> struct S;

  S: <T: typename> struct;
  */

int main() {
    Printer pp(std::cout);
    Sema sema;

    auto bld = sema.act_on_Parameter_list_start();
    bld.add_parameter(sema.get_identifier("T"), sema.typename_type());
    auto& mapping = bld.commit_template(sema.class_type());

    auto *body = sema.make_class(mapping.params());
    mapping.body = body;

    auto *X = sema.active_region->declare_primary_map(
        sema.get_identifier("X"),
        dynamic_cast<ipr::Template const&>(mapping.type()));
    X->init = &mapping;

    for (auto& p: mapping.params())
        X->args.push_back(&p.name());

    pp << "Hello, world\n";

    pp << *X << "\n";
}
