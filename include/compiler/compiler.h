#ifndef _FF_COMPILER_COMPILER_H_
#define _FF_COMPILER_COMPILER_H_ 1

#include <ff/compiler/type_annotation.h>
#include <ff/compiler/ast_annotation.h>
#include <ff/errors.h>
#include <ff/types.h>
#include <ff/code.h>
#include <ff/ref.h>
#include <ff/ast.h>
#include <mrt/dynamic_library.h>
#include <exception>
#include <string>
#include <vector>
#include <map>

#define FF_IMPORT_PATH_ENV_VAR "FF_IMPORT_PATH"

namespace ff {

class Compiler {
 public:
  struct Variable {
    std::string name;
    Ref<TypeAnnotation> type;
    bool isConst = false;
    std::map<std::string, Variable> fields;

    Variable() = default;
    Variable(const std::string& name, Ref<TypeAnnotation> type, bool isConst, const std::map<std::string, Variable>& fields);

    static Variable fromObject(const std::string& name, Ref<Object> object);
  };

  struct TypeInfo {
    Ref<TypeAnnotation> type = TypeAnnotation::any();
    Variable* var = nullptr;
  };

  struct ModuleInfo {
    std::string name;
    Ref<Module> module;
    Variable var;
    std::vector<ModuleInfo> imports;
  };

  enum ScopeType {
    SCOPE_FUNCTION,
    SCOPE_BLOCK,
  };

  struct Scope {
    ScopeType type;
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
  std::vector<Scope> m_scopes;                        // Stack of scopes
  std::vector<LoopRecord> m_loops;                    // Stack of loops
  std::map<std::string, Variable> m_globalVariables;
  std::vector<std::string> m_modules;                 // Stack for module declarations
  std::vector<std::string> m_imports;                 // List of imported modules, to prevent reimports and circular dependencies
  std::string m_thisModuleName;                       // Current module
  std::string m_parentModuleName;                     // Module that imports this module (assuming that the module is compiled)

 public:
  Compiler();

  Ref<Code> compile(const std::string& filename, ast::Node* node);

  void setParentModule(const std::string& parentModule);
  void setThisModule(const std::string& thisModule);

  std::map<std::string, Variable>& getGlobals();
  std::vector<std::string>& getImports();

 private:
  Ref<Code>& getCode();
  std::vector<Variable>& getLocals();
  bool localExists(const std::string& name);
  std::vector<Variable>::iterator findLocal(const std::string& name);
  bool isTopScope() const;

  /* Returns generated value type */
  Ref<TypeAnnotation> evalNode(ast::Node* node, bool copyValue = true, bool isModule = false, bool saveToVariable = true);

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
  void patchRemoteJump(int offset, int jump);
  void emitLoop(int loopStart);

  Ref<TypeAnnotation> resolveVariable(const std::string& name, Opcode local = OP_GET_LOCAL, Opcode global = OP_GET_GLOBAL, bool checkIsConst = false);
  Ref<TypeAnnotation> getVariableType(const std::string& name);
  Ref<TypeAnnotation> defineLocal(Variable var, int line = 0, ast::Node* value = nullptr, bool copyValue = true);
  TypeInfo resolveCurrentModule();

  std::vector<Function::Argument> parseArgs(ast::VarDeclList* args);
  void defineArgs(ast::VarDeclList* args);

  /* DEBUG */
  void printScopes();
  void printScope(int i, const std::string& prefix);
  void printGlobals();

  /* Nodes */
  Ref<TypeAnnotation> identifier(ast::Node* node, bool copyValue = true);
  Ref<TypeAnnotation> sequence(ast::Node* node, bool copyValue = true);
  Ref<TypeAnnotation> binaryExpr(ast::Node* node);
  Ref<TypeAnnotation> unaryExpr(ast::Node* node);
  Ref<TypeAnnotation> fndecl(ast::Node* node, bool isModule = false, bool saveToVariable = true);
  Ref<TypeAnnotation> classdecl(ast::Node* node, bool isModule = false);
  Ref<TypeAnnotation> vardecl(ast::Node* node, bool copyValue = true, bool isModule = false);
  Ref<TypeAnnotation> assignment(ast::Node* node, bool copyValue = true);
  Ref<TypeAnnotation> cast(ast::Node* node, bool copyValue = true);
  Ref<TypeAnnotation> ref(ast::Node* node);
  Ref<TypeAnnotation> newexpr(ast::Node* node);
  Ref<TypeAnnotation> call(ast::Node* node, bool topLevelCallee = false, TypeInfo typeInfo = {TypeAnnotation::any(), nullptr}, bool explicitSelf = false);
  Ref<TypeAnnotation> callMember(const std::string& memberName, const std::vector<ast::Node*>& args, bool isReturnValueExpected, bool explicitSelf, Ref<TypeAnnotation> type);
  Ref<TypeAnnotation> lambda(ast::Node* node);
  Ref<TypeAnnotation> dict(ast::Node* node);
  Ref<TypeAnnotation> vector(ast::Node* node);
  void returnCall(ast::Node* node);
  void block(ast::Node* node);
  void ifstmt(ast::Node* node);
  void loopstmt(ast::Node* node);
  void whilestmt(ast::Node* node);
  void forstmt(ast::Node* node);
  void import(ast::Node* node, bool isModule);
  void module(ast::Node* node, bool isModule);

  TypeInfo evalSequenceStart(ast::Node* node);
  TypeInfo evalSequenceElement(TypeInfo info, ast::Node* node, bool& isCopyable);
};

Compiler::ModuleInfo loadModule(const std::string& name, const std::string& filename, const std::string& parentModule);
Compiler::ModuleInfo loadNativeModule(const std::string& name, const std::string& filename);
Ref<Code> compile(const std::string& src, const std::string& filename = "<input>");

} /* namespace ff */

#endif /* _FF_COMPILER_COMPILER_H_ */