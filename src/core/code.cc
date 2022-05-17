#include <ff/code.h>
#include <algorithm>
#include <cstdio>

ff::Code::Code(const std::string& filename) : m_filename(filename) {}

size_t ff::Code::size() const {
  return m_code.size();
}

ff::Ref<ff::Object> ff::Code::getConstant(unsigned index) {
  return m_constants[index];
}

unsigned ff::Code::addConstant(Ref<Object> constant) {
  auto itr = std::find_if(m_constants.begin(), m_constants.end(), [&constant](Ref<Object> other) {
    return constant->equals(other);
  });
  if (itr == m_constants.end()) {
    m_constants.push_back(constant);
    return m_constants.size() - 1;
  }
  return itr - m_constants.begin();
}

std::vector<ff::Ref<ff::Object>>& ff::Code::getConstants() {
  return m_constants;
}

void ff::Code::addModule(const std::string& name, Ref<Object> module) {
  m_modules[name] = module;
}

bool ff::Code::hasModule(const std::string& name) {
  return m_modules.find(name) != m_modules.end();
}

ff::Ref<ff::Object> ff::Code::getModule(const std::string& name) {
  return m_modules[name];
}

std::map<std::string, ff::Ref<ff::Object>>& ff::Code::getModules() {
  return m_modules;
}

uint8_t& ff::Code::operator [](unsigned index) {
  return m_code[index];
}

uint8_t ff::Code::operator [](unsigned index) const {
  return m_code[index];
}

bool ff::Code::canRead() const {
  return m_readIndex < m_code.size();
}

void ff::Code::resetRead() {
  m_readIndex = 0;
}

size_t ff::Code::getReadIndex() const {
  return m_readIndex;
}

void ff::Code::setReadIndex(size_t readIndex) {
  m_readIndex = readIndex;
}

std::string ff::Code::getFilename() const {
  return m_filename;
}

int ff::Code::getLine(unsigned offset) const {
  for (int i = 0; i < m_lines.size(); i++) {
    if (m_lines[i].startOffset == offset) {
      return m_lines[i].line;
    } else if (m_lines[i].startOffset < offset) {
      if (i+1 == m_lines.size()) {
        return m_lines[i].line;
      }
    } else {
      return m_lines[i-1].line;
    }
  }

  return 0;
}

void ff::Code::pushInstruction(uint8_t op, int line) {
  m_code.push_back(op);

  if (line == -1) // Means the same line as the instruction before
    return;
  if (m_lines.size() == 0) {
    m_lines.push_back({m_code.size()-1, line});
  } else {
    if (m_lines[m_lines.size()-1].line != line) {
      m_lines.push_back({m_code.size()-1, line});
    }
  }
}

void ff::Code::disassemble(const std::string& prefix) {
  resetRead();
  while (canRead()) {
    disassembleInstruction(prefix);
  }
}

void ff::Code::disassembleInstruction(const std::string& prefix) {
  printf("%s%04zx | ", prefix.c_str(), getReadIndex());
  switch (read<uint8_t>()) {
    case OP_POP:
      printf("OP_POP\n");
      return;
    case OP_PULL_UP:
      printf("OP_PULL_UP %u\n", read<uint16_t>());
      return;
    case OP_ROL:
      printf("OP_ROL\n");
      return;
    case OP_DUP:
      printf("OP_DUP\n");
      return;
    case OP_NULL:
      printf("OP_NULL\n");
      return;
    case OP_TRUE:
      printf("OP_TRUE\n");
      return;
    case OP_FALSE:
      printf("OP_FALSE\n");
      return;
    case OP_NEW:
      printf("OP_NEW\n");
      return;
    case OP_COPY:
      printf("OP_COPY\n");
      return;
    case OP_LOAD_CONSTANT:
      printf("OP_LOAD_CONSTANT %u\n", read<uint32_t>());
      return;
    case OP_NEW_GLOBAL:
      printf("OP_NEW_GLOBAL\n");
      return;
    case OP_GET_GLOBAL:
      printf("OP_GET_GLOBAL\n");
      return;
    case OP_SET_GLOBAL:
      printf("OP_SET_GLOBAL\n");
      return;
    case OP_SET_GLOBAL_REF:
      printf("OP_SET_GLOBAL_REF\n");
      return;
    case OP_GET_LOCAL:
      printf("OP_GET_LOCAL %u\n", read<uint32_t>());
      return;
    case OP_SET_LOCAL:
      printf("OP_SET_LOCAL %u\n",read<uint32_t>());
      return;
    case OP_SET_LOCAL_REF:
      printf("OP_SET_LOCAL_REF %u\n",read<uint32_t>());
      return;
    case OP_GET_FIELD:
      printf("OP_GET_FIELD\n");
      return;
    case OP_SET_FIELD:
      printf("OP_SET_FIELD\n");
      return;
    case OP_SET_FIELD_REF:
      printf("OP_SET_FIELD_REF\n");
      return;
    case OP_GET_STATIC:
      printf("OP_GET_STATIC\n");
      return;
    case OP_JUMP:
      printf("OP_JUMP %u\n", read<uint16_t>());
      return;
    case OP_JUMP_TRUE:
      printf("OP_JUMP_TRUE %u\n", read<uint16_t>());
      return;
    case OP_JUMP_FALSE:
      printf("OP_JUMP_FALSE %u\n", read<uint16_t>());
      return;
    case OP_LOOP:
      printf("OP_LOOP %u\n", read<uint16_t>());
      return;
    case OP_CALL:
      printf("OP_CALL\n");
      return;
    case OP_CALL_MEMBER:
      printf("OP_CALL_MEMBER\n");
      return;
    case OP_RETURN:
      printf("OP_RETURN\n");
      return;
    case OP_CAST:
      printf("OP_CAST\n");
      return;
    case OP_PRINT:
      printf("OP_PRINT\n");
      return;
    case OP_ADD:
      printf("OP_ADD\n");
      return;
    case OP_SUB:
      printf("OP_SUB\n");
      return;
    case OP_MUL:
      printf("OP_MUL\n");
      return;
    case OP_DIV:
      printf("OP_DIV\n");
      return;
    case OP_MOD:
      printf("OP_MOD\n");
      return;
    case OP_EQ:
      printf("OP_EQ\n");
      return;
    case OP_NEQ:
      printf("OP_NEQ\n");
      return;
    case OP_LT:
      printf("OP_LT\n");
      return;
    case OP_GT:
      printf("OP_GT\n");
      return;
    case OP_LE:
      printf("OP_LE\n");
      return;
    case OP_GE:
      printf("OP_GE\n");
      return;
    case OP_AND:
      printf("OP_AND\n");
      return;
    case OP_OR:
      printf("OP_OR\n");
      return;
    case OP_NEG:
      printf("OP_NEG\n");
      return;
    case OP_INC:
      printf("OP_INC\n");
      return;
    case OP_DEC:
      printf("OP_DEC\n");
      return;
    case OP_NOT:
      printf("OP_NOT\n");
      return;
  }
}
