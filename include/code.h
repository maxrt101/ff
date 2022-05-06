#ifndef _FF_CODE_H_
#define _FF_CODE_H_ 1

#include <ff/object.h>
#include <ff/ref.h>
#include <ff/abi.h>
#include <cstdint>
#include <cstddef>
#include <cstdio>
#include <string>
#include <vector>
#include <map>

namespace ff {

enum Opcode {
  OP_POP,
  OP_PULL_UP,
  OP_ROL, // Deprecated
  OP_DUP,
  OP_NULL,
  OP_TRUE,
  OP_FALSE,
  OP_NEW,
  OP_COPY,
  OP_LOAD_CONSTANT,
  OP_NEW_GLOBAL,
  OP_GET_GLOBAL,
  OP_SET_GLOBAL,
  OP_SET_GLOBAL_REF,
  OP_GET_LOCAL,
  OP_SET_LOCAL,
  OP_SET_LOCAL_REF,
  OP_MAKECONST,
  OP_GET_FIELD,
  OP_SET_FIELD,
  OP_SET_FIELD_REF,
  OP_GET_STATIC,
  OP_JUMP,
  OP_JUMP_TRUE,
  OP_JUMP_FALSE,
  OP_LOOP,
  OP_CALL,
  OP_CALL_MEMBER,
  OP_RETURN,
  OP_CAST,
  OP_PRINT,
  OP_ADD,
  OP_SUB,
  OP_MUL,
  OP_DIV,
  OP_EQ,
  OP_NEQ,
  OP_LT,
  OP_GT,
  OP_LE,
  OP_GE,
  OP_AND,
  OP_OR,
  OP_NEG,
  OP_NOT,
  OP_INC,
  OP_DEC,
  OP_BREAKPOINT,
};

class Code {
 private:
  struct LineInfo {
    size_t startOffset;
    int line;
  };

 private:
  std::vector<uint8_t> m_code;
  std::vector<Ref<Object>> m_constants;
  std::vector<LineInfo> m_lines;
  // std::vector<Local> m_locals;
  std::map<std::string, Ref<Object>> m_modules;

  std::string m_filename;
  size_t m_readIndex = 0;

 public:
  Code(const std::string& filename);
  Code(const Code&) = delete;
  Code(Code&&) = default;
  ~Code() = default;

  size_t size() const;

  Ref<Object> getConstant(unsigned index);
  unsigned addConstant(Ref<Object> constant);
  std::vector<Ref<Object>>& getConstants();

  void addModule(const std::string& name, Ref<Object> module);
  bool hasModule(const std::string& name);
  Ref<Object> getModule(const std::string& name);
  std::map<std::string, Ref<Object>>& getModules();

  uint8_t& operator [](unsigned index);
  uint8_t operator [](unsigned index) const;

  bool canRead() const;
  void resetRead();
  size_t getReadIndex() const;
  void setReadIndex(size_t readIndex);

  std::string getFilename() const;
  int getLine(unsigned offset) const;
  void pushInstruction(uint8_t op, int line = -1);

  void disassemble(const std::string& prefix = "");
  void disassembleInstruction(const std::string& prefix = "");

  template <typename T>
  inline T read() = delete;

  template <typename T>
  inline void push(T value) = delete;
};


// Code::read

template <>
inline uint8_t Code::read<uint8_t>() {
  return m_code[m_readIndex++];
}

template <>
inline int8_t Code::read<int8_t>() {
  return m_code[m_readIndex++];
}

template <>
inline int16_t Code::read<int16_t>() {
  abi::N32 n;
  n.u8[0] = m_code[m_readIndex++];
  n.u8[1] = m_code[m_readIndex++];
  return n.i16[0];
}

template <>
inline uint16_t Code::read<uint16_t>() {
  abi::N32 n;
  n.u8[0] = m_code[m_readIndex++];
  n.u8[1] = m_code[m_readIndex++];
  return n.u16[0];
}

template <>
inline int32_t Code::read<int32_t>() {
  abi::N32 n;
  n.u8[0] = m_code[m_readIndex++];
  n.u8[1] = m_code[m_readIndex++];
  n.u8[2] = m_code[m_readIndex++];
  n.u8[3] = m_code[m_readIndex++];
  return n.i32;
}

template <>
inline uint32_t Code::read<uint32_t>() {
  abi::N32 n;
  n.u8[0] = m_code[m_readIndex++];
  n.u8[1] = m_code[m_readIndex++];
  n.u8[2] = m_code[m_readIndex++];
  n.u8[3] = m_code[m_readIndex++];
  return n.u32;
}


// Code::push

template <>
inline void Code::push<int8_t>(int8_t value) {
  m_code.push_back(value);
}

template <>
inline void Code::push<uint8_t>(uint8_t value) {
  m_code.push_back(value);
}

template <>
inline void Code::push<int16_t>(int16_t value) {
  abi::N32 n;
  n.i16[0] = value;
  m_code.push_back(n.i8[0]);
  m_code.push_back(n.i8[1]);
}

template <>
inline void Code::push<uint16_t>(uint16_t value) {
  abi::N32 n;
  n.u16[0] = value;
  m_code.push_back(n.i8[0]);
  m_code.push_back(n.i8[1]);
}

template <>
inline void Code::push<int32_t>(int32_t value) {
  abi::N32 n;
  n.i32 = value;
  m_code.push_back(n.i8[0]);
  m_code.push_back(n.i8[1]);
  m_code.push_back(n.i8[2]);
  m_code.push_back(n.i8[3]);
}

template <>
inline void Code::push<uint32_t>(uint32_t value) {
  abi::N32 n;
  n.u32 = value;
  m_code.push_back(n.i8[0]);
  m_code.push_back(n.i8[1]);
  m_code.push_back(n.i8[2]);
  m_code.push_back(n.i8[3]);
}


} /* namespace ff */

#endif /* _FF_CODE_H_ */