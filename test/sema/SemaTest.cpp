#include <iostream>
#include <ipr/io>
#include <ipr/impl>

using namespace ipr;

struct Sema;
struct Parameter_list_builder {
    Sema& sema;
    impl::Mapping& mapping;

    impl::Parameter* add_parameter(const ipr::Name& n, const ipr::Type& t) {
        return sema.make_parameter(n, t, mapping);
    }
};

struct Sema : impl::Lexicon {
    impl::Translation_unit tu;
    impl::Region* active_region;

    Sema() : tu(*this), active_region(tu.global_region()) {}

    Parameter_list_builder act_on_Parameter_list_start() {
        return {*this, *make_mapping(*active_region)};
    }
};

int main() {
    Printer pp(std::cout);
    Sema sema;
    pp << "Hello, world\n";
}