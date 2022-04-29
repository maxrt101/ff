#ifndef _FF_COMPILER_COMPILER_H_
#define _FF_COMPILER_COMPILER_H_ 1

#include <ff/compiler/type_annotation.h>
#include <ff/errors.h>
#include <ff/types.h>
#include <ff/code.h>
#include <ff/ref.h>
#include <ff/ast.h>
#include <exception>
#include <string>
#include <vector>
#include <map>

namespace ff {

class Compiler {
 private:
  struct Variable {
    std::string name;
    Ref<TypeAnnotation> type;
    bool isConst = false;
    std::map<std::string, Variable> fields;
    // bool isInitialized = false;

    Variable() = default;
    Variable(const std::string& name, Ref<TypeAnnotation> type, bool isConst, std::map<std::string, Variable> fields);

    static Variable fromObject(const std::string& name, Ref<Object> object);
  };

  struct TypeInfo {
    Ref<TypeAnnotation> type;
    Variable* var;
  };

  struct Scope {
    Ref<Code> code;
    std::vector<Variable> localVariables;
    int prevLocalsLength = 0;
    bool isFunctionScope = false;
    Ref<TypeAnnotation> returnType = TypeAnnotation::any();
    std::vector<Ref<TypeAnnotation>> returnStatements;
  };

  struct LoopRecord {
    std::vector<int> continue_jumps;
    std::vector<int> break_jumps;
  };

 private:
  std::string m_filename;
  ast::Node* m_rootNode;
  Ref<Code> m_rootCode;
  std::vector<Scope> m_scopes;
  std::vector<LoopRecord> m_loops;

  std::map<std::string, Variable> m_globalVariables;

  bool m_hadError = false;

 public:
  Compiler();

  Ref<Code> compile(const std::string& filename, ast::Node* node);

 private:
  Ref<Code>& getCode();
  std::vector<Variable>& getLocals();
  bool localExists(const std::string& name);
  std::vector<Variable>::iterator findLocal(const std::string& name);
  bool isTopScope() const;

  /* Returns generated value type */
  Ref<TypeAnnotation> evalNode(ast::Node* node);

  void beginScope();
  void beginFunctionScope(Ref<TypeAnnotation> returnType);
  Scope endScope();

  void beginBlock();
  void endBlock();

  void beginLoop();
  void endLoop();
  LoopRecord& getLoop();

  void emitConstant(Ref<Object> obj);
  void emitCall(const std::string& callee);
  uint16_t emitJump(Opcode op);
  void patchJump(int offset);
  void patchRemoteJump(int jumpOpLocation, int offset);
  void emitLoop(int loopStart);

  Ref<TypeAnnotation> resolveVariable(const std::string& name, Opcode local = OP_GET_LOCAL, Opcode global = OP_GET_GLOBAL);
  Ref<TypeAnnotation> getVariableType(const std::string& name);
  Ref<TypeAnnotation> defineLocal(Variable var, int line = 0, ast::Node* value = nullptr);

  std::vector<Function::Argument> parseArgs(ast::VarDeclList* args);
  void defineArgs(ast::VarDeclList* args);

  /* DEBUG */
  void printScopes();
  void printScope(int i, std::string prefix);
  void printGlobals();

  /* Nodes */
  Ref<TypeAnnotation> identifier(ast::Node* node);
  Ref<TypeAnnotation> sequence(ast::Node* node);
  Ref<TypeAnnotation> binaryExpr(ast::Node* node);
  Ref<TypeAnnotation> unaryExpr(ast::Node* node);
  Ref<TypeAnnotation> fndecl(ast::Node* node);
  Ref<TypeAnnotation> vardecl(ast::Node* node);
  Ref<TypeAnnotation> assignment(ast::Node* node);
  Ref<TypeAnnotation> cast(ast::Node* node);
  Ref<TypeAnnotation> call(ast::Node* node, bool topLevelCallee = false, TypeInfo typeInfo = {TypeAnnotation::any(), nullptr});
  void returnCall(ast::Node* node);
  void block(ast::Node* node);
  void ifstmt(ast::Node* node);
  void loopstmt(ast::Node* node);
  void whilestmt(ast::Node* node);

  TypeInfo evalSequenceStart(ast::Node* node);
  TypeInfo evalSequenceElement(TypeInfo info, ast::Node* node);
};

Ref<Code> compile(const std::string& src, const std::string& filename = "<input>");

} /* namespace ff */

#endif /* _FF_COMPILER_COMPILER_H_ */