#include <ff/ast.h>
#include <ff/types.h>
#include <mrt/container_utils.h>
#include <cstdio>
#include <string>

static void _printTree(ff::ast::Node* node, const std::string& prefix = "", bool flag = false) {
  using namespace ff::ast;
  if (!node) return;
  switch (node->getType()) {
    case NTYPE_FLOAT_LITERAL:
      printf("%f", node->as<FloatLiteral>()->getValue());
      break;
    case NTYPE_INTEGER_LITERAL:
      printf("%d", node->as<IntegerLiteral>()->getValue());
      break;
    case NTYPE_STRING_LITERAL:
      printf("\"%s\"", node->as<StringLiteral>()->getValue().c_str());
      break;
    case NTYPE_IDENTIFIER:
      printf("%s", node->as<Identifier>()->getValue().c_str());
      break;
    case NTYPE_GROUP_EXPR:
      printf("(");
      _printTree(node->as<Group>()->getValue());
      printf(")");
      break;
    case NTYPE_UNARY_EXPR:
      printf(" %s", node->as<Unary>()->getOperator().str.c_str());
      _printTree(node->as<Unary>()->getValue());
      break;
    case NTYPE_BINARY_EXPR:
      _printTree(node->as<Binary>()->getLeft());
      printf(" %s ", node->as<Binary>()->getOperator().str.c_str());
      _printTree(node->as<Binary>()->getRight());
      break;
    case NTYPE_SEQUENCE:
      for (int i = 0; i < node->as<Sequence>()->getSequence().size(); i++) {
        _printTree(node->as<Sequence>()->getSequence()[i]);
        if (i + 1 < node->as<Sequence>()->getSequence().size()) putchar('.');
      }
      break;
    case NTYPE_IMPORT: {
      Import* imp = node->as<Import>();
      printf("import ");
      if (imp->getImports().size() == 1) {
        printf("\"%s\";", imp->getImports().front().c_str());
      } else {
        printf("{");
        for (int i = 0; i < imp->getImports().size(); i++) {
          printf("\"%s\"", imp->getImports()[i].c_str());
          if (i + 1 < imp->getImports().size()) printf(", ");
        }
        printf("}");
      }
      break;
    }
    case NTYPE_MODULE: {
      Module* mod = node->as<Module>();
      printf("module %s ", mod->getName().c_str());
      _printTree(mod->getBody(), prefix);
      break;
    }
    case NTYPE_CLASS: {
      Class* class_ = node->as<Class>();
      printf("class %s {\n", class_->getName().str.c_str());
      for (auto& field : class_->getFields()) {
        printf("%s  ", prefix.c_str());
        if (field.isStatic) {
          printf("static ");
        }
        if (field.isConst) {
          printf("const ");
        }
        printf("%s: %s", field.name.str.c_str(), field.type->toString().c_str());
        if (field.value) {
          printf(" = ");
          _printTree(field.value, prefix);
        }
        printf(";\n");
      }
      for (auto& method : class_->getMethods()) {
        printf("%s  ", prefix.c_str());
        if (method.isStatic) {
          printf("static ");
        }
        _printTree(method.fn, prefix + "  ");
        printf("\n");
      }
      printf("%s}", prefix.c_str());
      break;
    }
    case NTYPE_NEW: {
      New* new_ = node->as<New>();
      printf("new ");
      _printTree(new_->getClass(), "");
      printf("()");
      break;
    }
    case NTYPE_FUNCTION: {
      Function* fn = node->as<Function>();
      printf("fn %s(", fn->getName().str.c_str());
      _printTree(fn->getArgs());
      printf("): %s -> ", fn->getFunctionType().asRefTo<ff::FunctionAnnotation>()->returnType->toString().c_str());
      _printTree(fn->getBody(), prefix, true);
      if (fn->getBody()->getType() != NTYPE_BLOCK) {
        printf(";");
      }
      break;
    }
    case NTYPE_LAMBDA: {
      Lambda* lambda = node->as<Lambda>();
      printf("fn(");
      _printTree(lambda->getArgs());
      printf("): %s -> ", lambda->getFunctionType().asRefTo<ff::FunctionAnnotation>()->returnType->toString().c_str());
      _printTree(lambda->getBody(), "  ", true);
      break;
    }
    case NTYPE_VAR_DECL: {
      VarDecl* var = node->as<VarDecl>();
      if (flag) printf("%s ", var->getConst() ? "const" : "var");
      printf("%s: %s", var->getName().str.c_str(), var->getVarType()->toString().c_str());
      if (var->getValue()) {
        printf(" = ");
        _printTree(var->getValue(), prefix + "  ");
      }
      break;
    }
    case NTYPE_VAR_DECL_LIST: {
      VarDeclList* list = node->as<VarDeclList>();
      for (auto& var : list->getList()) {
        _printTree(var, "", false);
        printf(", ");
      }
      printf("\b\b  \b\b");
      break;
    }
    case NTYPE_CALL: {
      Call* call = node->as<Call>();
      _printTree(call->getCallee());
      printf("(");
      for (int i = 0; i < call->getArgs().size(); i++) {
        _printTree(call->getArgs()[i]);
        if (i + 1 < call->getArgs().size()) printf(", ");
      }
      printf(")");
      break;
    }
    case NTYPE_ASSIGNMENT: {
      Assignment* ass = node->as<Assignment>();
      _printTree(ass->getAssignee());
      if (ass->getIsRefAssignment()) {
        printf(" := ");
      } else {
        printf(" = ");
      }
      _printTree(ass->getValue());
      break;
    }
    case NTYPE_RETURN: {
      printf("return ");
      _printTree(node->as<Return>()->getValue());
      break;
    }
    case NTYPE_BLOCK: {
      printf("{\n");
      for (auto& bodyNode : node->as<Block>()->getBody()) {
        printf("%s", (prefix + "  ").c_str());
        _printTree(bodyNode, prefix + "  ", true);
        if (!mrt::isIn(bodyNode->getType(), NTYPE_FUNCTION, NTYPE_CLASS, NTYPE_BLOCK, NTYPE_IF, NTYPE_FOR, NTYPE_FOREACH, NTYPE_WHILE, NTYPE_MODULE, NTYPE_IMPORT)) {
          printf(";");
        }
        printf("\n");
      }
      printf("%s}", prefix.c_str());
      break;
    }
    case NTYPE_DICT: {
      printf("{");
      int count = 0;
      auto fields = node->as<Dict>()->getFields();
      for (auto& p : fields) {
        printf("%s -> ", p.first.c_str());
        _printTree(p.second, prefix, true);
        if (count + 1 < fields.size()) printf(", ");
        count++;
      }
      printf("}");
      break;
    }
    case NTYPE_VECTOR: {
      printf("{");
      auto elements = node->as<Vector>()->getElements();
      for (int i = 0; i < elements.size(); i++) {
        _printTree(elements[i], prefix, true);
        if (i + 1 < elements.size()) {
          printf(", ");
        }
      }
      printf("}");
      break;
    }
    case NTYPE_CAST_EXPR: {
      Cast* cast = node->as<Cast>();
      _printTree(cast->getValue());
      printf(" as %s ", cast->getCastType()->toString().c_str());
      break;
    }
    case NTYPE_REF: {
      printf("ref ");
      _printTree(node->as<Ref>()->getValue());
      break;
    }
    case NTYPE_PRINT: {
      printf("print ");
      _printTree(node->as<Print>()->getValue());
      break;
    }
    case NTYPE_NULL: {
      printf("null");
      break;
    }
    case NTYPE_TRUE: {
      printf("true");
      break;
    }
    case NTYPE_FALSE: {
      printf("false");
      break;
    }
    case NTYPE_IF: {
      If* if_ = node->as<If>();
      printf("if (");
      _printTree(if_->getCondition());
      printf(") ");
      _printTree(if_->getBody(), prefix);
      if (if_->getElseBody()) {
        printf(" else ");
        _printTree(if_->getElseBody(), prefix);
      }
      break;
    }
    case NTYPE_FOR: {
      For* for_ = node->as<For>();
      printf("for (");
      if (for_->getInit()->getType() == NTYPE_VAR_DECL) {
        printf("var ");
      }
      _printTree(for_->getInit());
      printf("; ");
      _printTree(for_->getCondition());
      printf("; ");
      _printTree(for_->getIncrement());
      printf(")");
      _printTree(for_->getBody(), prefix);
      break;
    }
    case NTYPE_FOREACH: {
      ForEach* foreach = node->as<ForEach>();
      printf("for ");
      _printTree(foreach->getLoopVariable());
      printf(" in ");
      _printTree(foreach->getIterable());
      printf(" ");
      _printTree(foreach->getBody(), prefix);
      break;
    }
    case NTYPE_WHILE: {
      While* while_ = node->as<While>();
      printf("while (");
      _printTree(while_->getCondition());
      printf(") ");
      _printTree(while_->getBody(), prefix);
      break;
    }
    case NTYPE_LOOP: {
      Loop* loop_ = node->as<Loop>();
      printf("loop ");
      _printTree(loop_->getBody(), prefix);
      break;
    }
    case NTYPE_CONTINUE: {
      printf("continue");
      break;
    }
    case NTYPE_BREAK: {
      printf("break");
      break;
    }
    case NTYPE_BREAKPOINT: {
      printf("breakpoint");
      break;
    }
  }
}

void ff::ast::printTree(Node* node) {
  _printTree(node, "", true);
  printf("\n");
}

void ff::ast::unwrapCode(ff::Ref<ff::Code> code, const std::string& prefix) {
  code->disassemble(prefix + "| ");

  int i = 0;
  for (ff::Ref<ff::Object>& obj : code->getConstants()) {
    printf("%s+ constant#%d: %s = %s\n", prefix.c_str(), i, (obj->isInstance() ? obj.as<ff::Instance>()->getType()->toString().c_str() : "type"), obj->toString().c_str());
    if (obj->isInstance() && obj.as<ff::Instance>()->getType() == ff::FunctionType::getInstance().asRefTo<ff::Type>()) {
      printf("%s \\\n", prefix.c_str());
      unwrapCode(obj.as<ff::Function>()->code, prefix + "  ");
    }
    i++;
  }
}

void ff::ast::deleteTree(Node* node) {
  if (!node) return;
  switch (node->getType()) {
    case NTYPE_GROUP_EXPR:
      deleteTree(node->as<Group>()->getValue());
      break;
    case NTYPE_UNARY_EXPR:
      deleteTree(node->as<Unary>()->getValue());
      break;
    case NTYPE_BINARY_EXPR:
      deleteTree(node->as<Binary>()->getLeft());
      deleteTree(node->as<Binary>()->getRight());
      break;
    case NTYPE_SEQUENCE:
      for (int i = 0; i < node->as<Sequence>()->getSequence().size(); i++) {
        deleteTree(node->as<Sequence>()->getSequence()[i]);
      }
      break;
    case NTYPE_MODULE: {
      deleteTree(node->as<Module>()->getBody());
      break;
    }
    case NTYPE_CLASS: {
      Class* class_ = node->as<Class>();
      for (auto& field : class_->getFields()) {
        if (field.value) {
          deleteTree(field.value);
        }
      }
      for (auto& method : class_->getMethods()) {
        deleteTree(method.fn);
      }
      break;
    }
    case NTYPE_NEW: {
      deleteTree(node->as<New>()->getClass());
      break;
    }
    case NTYPE_FUNCTION: {
      Function* fn = node->as<Function>();
      deleteTree(fn->getArgs());
      deleteTree(fn->getBody());
      break;
    }
    case NTYPE_LAMBDA: {
      Lambda* lambda = node->as<Lambda>();
      deleteTree(lambda->getArgs());
      deleteTree(lambda->getBody());
      break;
    }
    case NTYPE_VAR_DECL: {
      VarDecl* var = node->as<VarDecl>();
      if (var->getValue()) {
        deleteTree(var->getValue());
      }
      break;
    }
    case NTYPE_VAR_DECL_LIST: {
      VarDeclList* list = node->as<VarDeclList>();
      for (auto& var : list->getList()) {
        deleteTree(var);
      }
      break;
    }
    case NTYPE_CALL: {
      Call* call = node->as<Call>();
      deleteTree(call->getCallee());
      for (int i = 0; i < call->getArgs().size(); i++) {
        deleteTree(call->getArgs()[i]);
      }
      break;
    }
    case NTYPE_ASSIGNMENT: {
      Assignment* ass = node->as<Assignment>();
      deleteTree(ass->getAssignee());
      deleteTree(ass->getValue());
      break;
    }
    case NTYPE_RETURN: {
      deleteTree(node->as<Return>()->getValue());
      break;
    }
    case NTYPE_BLOCK: {
      for (auto& bodyNode : node->as<Block>()->getBody()) {
        deleteTree(bodyNode);
      }
      break;
    }
    case NTYPE_DICT: {
      auto fields = node->as<Dict>()->getFields();
      for (auto& p : fields) {
        deleteTree(p.second);
      }
      break;
    }
    case NTYPE_VECTOR: {
      auto elements = node->as<Vector>()->getElements();
      for (int i = 0; i < elements.size(); i++) {
        deleteTree(elements[i]);
      }
      break;
    }
    case NTYPE_CAST_EXPR: {
      Cast* cast = node->as<Cast>();
      deleteTree(cast->getValue());
      break;
    }
    case NTYPE_REF: {
      deleteTree(node->as<Ref>()->getValue());
      break;
    }
    case NTYPE_PRINT: {
      deleteTree(node->as<Print>()->getValue());
      break;
    }
    case NTYPE_IF: {
      If* if_ = node->as<If>();
      deleteTree(if_->getCondition());
      deleteTree(if_->getBody());
      if (if_->getElseBody()) {
        deleteTree(if_->getElseBody());
      }
      break;
    }
    case NTYPE_FOR: {
      For* for_ = node->as<For>();
      deleteTree(for_->getInit());
      deleteTree(for_->getCondition());
      deleteTree(for_->getIncrement());
      deleteTree(for_->getBody());
      break;
    }
    case NTYPE_FOREACH: {
      ForEach* foreach = node->as<ForEach>();
      deleteTree(foreach->getLoopVariable());
      deleteTree(foreach->getIterable());
      deleteTree(foreach->getBody());
      break;
    }
    case NTYPE_WHILE: {
      While* while_ = node->as<While>();
      deleteTree(while_->getCondition());
      deleteTree(while_->getBody());
      break;
    }
    case NTYPE_LOOP: {
      Loop* loop_ = node->as<Loop>();
      deleteTree(loop_->getBody());
      break;
    }
    case NTYPE_FLOAT_LITERAL:
    case NTYPE_INTEGER_LITERAL:
    case NTYPE_STRING_LITERAL:
    case NTYPE_IDENTIFIER:
    case NTYPE_IMPORT:
    case NTYPE_NULL:
    case NTYPE_TRUE:
    case NTYPE_FALSE:
    case NTYPE_CONTINUE:
    case NTYPE_BREAK:
    case NTYPE_BREAKPOINT:
    default:
      break;
  }

  delete node;
}
