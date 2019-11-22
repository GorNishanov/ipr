#include <iostream>
#include <ipr/impl>
#include <ipr/io>

using namespace ipr;

#if 0
template<auto N>
struct X {
    operator int() const { return 0; }
};

int a(int) { return 1; }

template<typename T>
auto g(T t) {
    constexpr auto n = sizeof sizeof t - sizeof(decltype(0));
    X<n> x;
    return a(x);
}

template<auto n>
int a(X<n>) { return 2; }

int main() {
    return g(9) + g(1.0);
}
#endif

struct value_dep_builder {
   impl::Lexicon fac;
   impl::Translation_unit tu{fac};
   Printer pp{std::cout};

   ipr::Decl* X{};
   ipr::Decl* a_fn{};
   ipr::Decl* a_tmpl{};

   //    template<auto N> struct X {
   //        operator int() const { return 0; }
   //    };
   //
   impl::Class* build_X() {
      // wny not global_region->make_mapping?
      auto* mapping = fac.make_mapping(*tu.global_region());
      auto* N = fac.make_parameter(fac.get_identifier("N"), fac.get_auto(), *mapping);
      // should be done at commit. Since once we got the type of paramters,
      // we should not be adding more.
      auto& templ = fac.get_template(mapping->parameters.type(), fac.class_type());
      mapping->constraint = &templ;

      auto* X = tu.global_region()->declare_primary_map(fac.get_identifier("X"), templ);
      X->init = mapping;
      auto* args = fac.make_expr_list();
      args->push_back(N);
      this->X = X;

      auto* body = fac.make_class(mapping->params());
      mapping->body = body;

      auto& n = *fac.make_id_expr(*X);
      auto& a = *args;

      body->id = &fac.get_template_id(n, a);
      X_add_conversion_to_int(body);

      return body;
   }
   void X_add_conversion_to_int(impl::Class* owner) {
      //        operator int() const { return 0; }
      //    operator int: (this:
      auto& int_type = fac.int_type();
      auto* mapping = fac.make_mapping(owner->region());
      auto& const_class = fac.get_qualified(Type_qualifier::Const, *owner);
      auto& const_ptr = fac.get_pointer(const_class);
      auto* this_param
          = fac.make_parameter(fac.get_identifier("this"), const_ptr, *mapping);
      auto& fn_type = fac.get_function(mapping->parameters.type(), int_type);
      mapping->constraint = &fn_type;

      auto* int_conv = owner->declare_fun(fac.get_conversion(fac.int_type()), fn_type);
      int_conv->init = mapping;

      auto* body = fac.make_block(mapping->parameters, int_type);
      mapping->body = body;

      auto& zero = fac.get_literal(int_type, "0");
      body->add_stmt(fac.make_return(zero));
   }

   //  int a(int) { return 1; }

   void build_a_int() {
      auto& int_type = fac.int_type();
      auto* owner = tu.global_region();

      auto* mapping = fac.make_mapping(*owner);
      fac.make_parameter(fac.get_identifier(""), int_type, *mapping);
      auto& fn_type = fac.get_function(mapping->parameters.type(), int_type);
      mapping->constraint = &fn_type;

      auto* a_fn = owner->declare_fun(fac.get_identifier("a"), fn_type);
      a_fn->init = mapping;

      auto* body = fac.make_block(mapping->parameters, int_type);
      mapping->body = body;

      auto& one = fac.get_literal(int_type, "1");
      body->add_stmt(fac.make_return(one));

      this->a_fn = a_fn;
   }

   // template<typename T>
   // auto g(T t) {
   //     constexpr auto n = sizeof sizeof t - sizeof(decltype(0));
   //     X<n> x;
   //     return a(x);
   // }

   // ipr=impl: FIXME: make_parameter should be on mapping.

   void build_g_T() {
      auto* owner = tu.global_region();

      auto* mapping = fac.make_mapping(*owner);
      auto* T
          = fac.make_parameter(fac.get_identifier("T"), fac.typename_type(), *mapping);
      auto& template_ty
          = fac.get_template(mapping->parameters.type(), fac.typename_type());
      mapping->constraint = &template_ty;

      auto* mapping2 = fac.impl::expr_factory::make_mapping(
          mapping->parameters, fac.typename_type(), 1);
      auto* t
          = fac.make_parameter(fac.get_identifier("t"), fac.get_as_type(*T), *mapping2);
      auto& fn_ty = fac.get_function(mapping2->parameters.type(), fac.get_auto());
      mapping2->constraint = &fn_ty;

      auto* X
          = tu.global_region()->declare_primary_map(fac.get_identifier("g"), template_ty);
      X->init = mapping;
      //X->args.push_back(T);

      mapping->body = mapping2;

      auto* body = fac.make_block(mapping2->parameters, fn_ty.target());
      mapping2->body = body;

      auto* n = build_n_decl(*body, *t);
      body->add_stmt(n);
      auto* x = build_x_decl(*body, *n);
      body->add_stmt(x);
      // build f(x) TODO: conv operator
      auto* args = fac.make_expr_list();
      args->push_back(fac.make_id_expr(*x));
      auto* call = fac.make_call(*fac.make_id_expr(*a_fn), *args);
      body->add_stmt(fac.make_return(*call));
   }

   //     constexpr auto n = sizeof sizeof t - sizeof(decltype(0));
   ipr::Decl const* build_n_decl(impl::Block& body, ipr::Decl const& t) {
      auto* x = fac.make_sizeof(
          *fac.make_sizeof(*fac.make_id_expr(t))); // FIXME: compute sizes and types
      auto& dclt = fac.get_decltype(fac.get_literal(fac.int_type(), "0"));
      auto* y = fac.make_sizeof(dclt);
      auto* init = fac.make_minus(*x, *y);

      auto* n = body.region.declare_var(fac.get_identifier("n"), fac.get_auto());
      n->init = init;

      return n;
   }

   //     X<n> x; // x: X<n> -- need to request specialization.
   ipr::Decl const* build_x_decl(impl::Block& body, ipr::Decl const& n) {
      // build X<n>
      //impl::Expr_list args; // BUG. Why it is allowed?
      auto* args = fac.make_expr_list();
      args->push_back(fac.make_id_expr(n));
      auto& tmpl_id = fac.get_template_id(*fac.make_id_expr(*X), *args);
      // FIXME: Need to do specialization.
      auto& specialization = fac.get_as_type(tmpl_id);
      auto* x = body.region.declare_var(fac.get_identifier("x"), specialization);
      // TODO: a constructor call.
      return x;
   }

   value_dep_builder() {
      build_X();
      build_a_int();
      build_g_T();
      pp << tu;
   }
};

int main() {
   value_dep_builder b;
   b.pp << "Starting work on value dep example 3\n";
}

#if 0
TranslationUnitDecl
|-ClassTemplateDecl <line:1:1, line:4:1> line:2:8 X
| |-NonTypeTemplateParmDecl <line:1:10, col:15> col:15 'auto' depth 0 index 0 N
| |-CXXRecordDecl <line:2:1, line:4:1> line:2:8 struct X definition
| | |-DefinitionData empty aggregate standard_layout trivially_copyable pod trivial literal has_constexpr_non_copy_move_ctor can_const_default_init
| | | |-DefaultConstructor exists trivial constexpr needs_implicit defaulted_is_constexpr
| | | |-CopyConstructor simple trivial has_const_param needs_implicit implicit_has_const_param
| | | |-MoveConstructor exists simple trivial needs_implicit
| | | |-CopyAssignment trivial has_const_param needs_implicit implicit_has_const_param
| | | |-MoveAssignment exists simple trivial needs_implicit
| | | `-Destructor simple irrelevant trivial needs_implicit
| | |-CXXRecordDecl <col:1, col:8> col:8 implicit struct X
| | `-CXXConversionDecl <line:3:5, col:38> col:5 operator int 'int () const'
| |   `-CompoundStmt <col:26, col:38>
| |     `-ReturnStmt <col:28, col:35>
| |       `-IntegerLiteral <col:35> 'int' 0
| `-ClassTemplateSpecializationDecl <line:1:1, line:4:1> line:2:8 struct X definition
|   |-DefinitionData pass_in_registers empty aggregate standard_layout trivially_copyable pod trivial literal has_constexpr_non_copy_move_ctor can_const_default_init
|   | |-DefaultConstructor exists trivial constexpr defaulted_is_constexpr
|   | |-CopyConstructor simple trivial has_const_param implicit_has_const_param
|   | |-MoveConstructor exists simple trivial
|   | |-CopyAssignment trivial has_const_param needs_implicit implicit_has_const_param
|   | |-MoveAssignment exists simple trivial needs_implicit
|   | `-Destructor simple irrelevant trivial needs_implicit
|   |-TemplateArgument integral 4
|   |-CXXRecordDecl prev 0x5650bf95ad28 <col:1, col:8> col:8 implicit struct X
|   |-CXXConversionDecl <line:3:5, col:38> col:5 used operator int 'int () const'
|   | `-CompoundStmt <col:26, col:38>
|   |   `-ReturnStmt <col:28, col:35>
|   |     `-IntegerLiteral <col:35> 'int' 0
|   |-CXXConstructorDecl <line:2:8> col:8 implicit used constexpr X 'void () noexcept' inline default trivial
|   | `-CompoundStmt <col:8>
|   |-CXXConstructorDecl <col:8> col:8 implicit constexpr X 'void (const X<4> &)' inline default trivial noexcept-unevaluated 0x5650bf95b290
|   | `-ParmVarDecl <col:8> col:8 'const X<4> &'
|   `-CXXConstructorDecl <col:8> col:8 implicit constexpr X 'void (X<4> &&)' inline default trivial noexcept-unevaluated 0x5650bf95b450
|     `-ParmVarDecl <col:8> col:8 'X<4> &&'
|-FunctionDecl <line:6:1, col:24> col:5 used a 'int (int)'
| |-ParmVarDecl <col:7> col:10 'int'
| `-CompoundStmt <col:12, col:24>
|   `-ReturnStmt <col:14, col:21>
|     `-IntegerLiteral <col:21> 'int' 1
|-FunctionTemplateDecl <line:8:1, line:13:1> line:9:6 g
| |-TemplateTypeParmDecl <line:8:10, col:19> col:19 referenced typename depth 0 index 0 T
| |-FunctionDecl <line:9:1, line:13:1> line:9:6 g 'auto (T)'
| | |-ParmVarDecl <col:8, col:10> col:10 referenced t 'T'
| | `-CompoundStmt <col:13, line:13:1>
| |   |-DeclStmt <line:10:5, col:61>
| |   | `-VarDecl <col:5, col:60> col:20 referenced n 'const unsigned long':'const unsigned long' constexpr cinit
| |   |   `-BinaryOperator <col:24, col:60> 'unsigned long' '-'
| |   |     |-UnaryExprOrTypeTraitExpr <col:24, col:38> 'unsigned long' sizeof
| |   |     | `-UnaryExprOrTypeTraitExpr <col:31, col:38> 'unsigned long' sizeof
| |   |     |   `-DeclRefExpr <col:38> 'T' lvalue ParmVar 0x5650bf92b5a8 't' 'T' non_odr_use_unevaluated
| |   |     `-UnaryExprOrTypeTraitExpr <col:42, col:60> 'unsigned long' sizeof 'decltype(0)':'int'
| |   |-DeclStmt <line:11:5, col:11>
| |   | `-VarDecl <col:5, col:10> col:10 referenced x 'X<n>':'X<4>' callinit
| |   |   `-CXXConstructExpr <col:10> 'X<n>':'X<4>' 'void () noexcept'
| |   `-ReturnStmt <line:12:5, col:15>
| |     `-CallExpr <col:12, col:15> 'int'
| |       |-ImplicitCastExpr <col:12> 'int (*)(int)' <FunctionToPointerDecay>
| |       | `-DeclRefExpr <col:12> 'int (int)' lvalue Function 0x5650bf92b358 'a' 'int (int)'
| |       `-ImplicitCastExpr <col:14> 'int' <UserDefinedConversion>
| |         `-CXXMemberCallExpr <col:14> 'int'
| |           `-MemberExpr <col:14> '<bound member function type>' .operator int 0x5650bf95b058
| |             `-ImplicitCastExpr <col:14> 'const X<4>' lvalue <NoOp>
| |               `-DeclRefExpr <col:14> 'X<n>':'X<4>' lvalue Var 0x5650bf95aee8 'x' 'X<n>':'X<4>'
| |-FunctionDecl <line:9:1, line:13:1> line:9:6 used g 'int (int)'
| | |-TemplateArgument type 'int'
| | |-ParmVarDecl <col:8, col:10> col:10 referenced t 'int':'int'
| | `-CompoundStmt <col:13, line:13:1>
| |   |-DeclStmt <line:10:5, col:61>
| |   | `-VarDecl <col:5, col:60> col:20 referenced n 'const unsigned long':'const unsigned long' constexpr cinit
| |   |   `-BinaryOperator <col:24, col:60> 'unsigned long' '-'
| |   |     |-UnaryExprOrTypeTraitExpr <col:24, col:38> 'unsigned long' sizeof
| |   |     | `-UnaryExprOrTypeTraitExpr <col:31, col:38> 'unsigned long' sizeof
| |   |     |   `-DeclRefExpr <col:38> 'int':'int' lvalue ParmVar 0x5650bf95c8f0 't' 'int':'int' non_odr_use_unevaluated
| |   |     `-UnaryExprOrTypeTraitExpr <col:42, col:60> 'unsigned long' sizeof 'decltype(0)':'int'
| |   |-DeclStmt <line:11:5, col:11>
| |   | `-VarDecl <col:5, col:10> col:10 used x 'X<n>':'X<4>' callinit
| |   |   `-CXXConstructExpr <col:10> 'X<n>':'X<4>' 'void () noexcept'
| |   `-ReturnStmt <line:12:5, col:15>
| |     `-CallExpr <col:12, col:15> 'int'
| |       |-ImplicitCastExpr <col:12> 'int (*)(int)' <FunctionToPointerDecay>
| |       | `-DeclRefExpr <col:12> 'int (int)' lvalue Function 0x5650bf92b358 'a' 'int (int)'
| |       `-ImplicitCastExpr <col:14> 'int' <UserDefinedConversion>
| |         `-CXXMemberCallExpr <col:14> 'int'
| |           `-MemberExpr <col:14> '<bound member function type>' .operator int 0x5650bf95b058
| |             `-ImplicitCastExpr <col:14> 'const X<4>' lvalue <NoOp>
| |               `-DeclRefExpr <col:14> 'X<n>':'X<4>' lvalue Var 0x5650bf95cd08 'x' 'X<n>':'X<4>'
| `-FunctionDecl <line:9:1, line:13:1> line:9:6 used g 'int (double)'
|   |-TemplateArgument type 'double'
|   |-ParmVarDecl <col:8, col:10> col:10 referenced t 'double':'double'
|   `-CompoundStmt <col:13, line:13:1>
|     |-DeclStmt <line:10:5, col:61>
|     | `-VarDecl <col:5, col:60> col:20 referenced n 'const unsigned long':'const unsigned long' constexpr cinit
|     |   `-BinaryOperator <col:24, col:60> 'unsigned long' '-'
|     |     |-UnaryExprOrTypeTraitExpr <col:24, col:38> 'unsigned long' sizeof
|     |     | `-UnaryExprOrTypeTraitExpr <col:31, col:38> 'unsigned long' sizeof
|     |     |   `-DeclRefExpr <col:38> 'double':'double' lvalue ParmVar 0x5650bf95d160 't' 'double':'double' non_odr_use_unevaluated
|     |     `-UnaryExprOrTypeTraitExpr <col:42, col:60> 'unsigned long' sizeof 'decltype(0)':'int'
|     |-DeclStmt <line:11:5, col:11>
|     | `-VarDecl <col:5, col:10> col:10 used x 'X<n>':'X<4>' callinit
|     |   `-CXXConstructExpr <col:10> 'X<n>':'X<4>' 'void () noexcept'
|     `-ReturnStmt <line:12:5, col:15>
|       `-CallExpr <col:12, col:15> 'int'
|         |-ImplicitCastExpr <col:12> 'int (*)(int)' <FunctionToPointerDecay>
|         | `-DeclRefExpr <col:12> 'int (int)' lvalue Function 0x5650bf92b358 'a' 'int (int)'
|         `-ImplicitCastExpr <col:14> 'int' <UserDefinedConversion>
|           `-CXXMemberCallExpr <col:14> 'int'
|             `-MemberExpr <col:14> '<bound member function type>' .operator int 0x5650bf95b058
|               `-ImplicitCastExpr <col:14> 'const X<4>' lvalue <NoOp>
|                 `-DeclRefExpr <col:14> 'X<n>':'X<4>' lvalue Var 0x5650bf95e088 'x' 'X<n>':'X<4>'
|-FunctionTemplateDecl <line:15:1, line:16:25> col:5 a
| |-NonTypeTemplateParmDecl <line:15:10, col:15> col:15 referenced 'auto' depth 0 index 0 n
| `-FunctionDecl <line:16:1, col:25> col:5 a 'int (X<n>)'
|   |-ParmVarDecl <col:7, col:10> col:11 'X<n>'
|   `-CompoundStmt <col:13, col:25>
|     `-ReturnStmt <col:15, col:22>
|       `-IntegerLiteral <col:22> 'int' 2
`-FunctionDecl <line:18:1, line:20:1> line:18:5 main 'int ()'
  `-CompoundStmt <col:12, line:20:1>
    `-ReturnStmt <line:19:5, col:24>
      `-BinaryOperator <col:12, col:24> 'int' '+'
        |-CallExpr <col:12, col:15> 'int':'int'
        | |-ImplicitCastExpr <col:12> 'int (*)(int)' <FunctionToPointerDecay>
        | | `-DeclRefExpr <col:12> 'int (int)' lvalue Function 0x5650bf95c9f0 'g' 'int (int)' (FunctionTemplate 0x5650bf92b768 'g')
        | `-IntegerLiteral <col:14> 'int' 9
        `-CallExpr <col:19, col:24> 'int':'int'
          |-ImplicitCastExpr <col:19> 'int (*)(double)' <FunctionToPointerDecay>
          | `-DeclRefExpr <col:19> 'int (double)' lvalue Function 0x5650bf95d260 'g' 'int (double)' (FunctionTemplate 0x5650bf92b768 'g')
          `-FloatingLiteral <col:21> 'double' 1.000000e+00
#endif
