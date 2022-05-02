#include <ff/ast.h>
#include <mrt/container_utils.h>
#include <string>

static void _printTree(ff::ast::Node* node, std::string prefix = "", bool flag = false) {
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
    case NTYPE_FUNCTION: {
      Function* fn = node->as<Function>();
      printf("fn %s(", fn->getName().str.c_str());
      _printTree(fn->getArgs());
      printf("): %s -> ", fn->getFunctionType().asRefTo<ff::FunctionAnnotation>()->returnType->toString().c_str());
      _printTree(fn->getBody(), "  ", true);
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
        _printTree(var->getValue());
      }
      break;
    }
    case NTYPE_VAR_DECL_LIST: {
      VarDeclList* list = node->as<VarDeclList>();
      for (auto& var : list->getList()) {
        _printTree(var);
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
      printf(" = ");
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
        _printTree(bodyNode, prefix + "  ", flag);
        if (!mrt::isIn(bodyNode->getType(), NTYPE_FUNCTION, NTYPE_BLOCK, NTYPE_IF, NTYPE_FOR, NTYPE_FOREACH, NTYPE_WHILE)) {
          printf(";");
        }
        printf("\n");
      }
      printf("%s}", prefix.c_str());
      break;
    }
    case NTYPE_EXPR_LIST_EXPR: {
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
  _printTree(node, "", false);
  printf("\n");
}
