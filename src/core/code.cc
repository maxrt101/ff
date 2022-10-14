#include <ff/code.h>
#include <algorithm>
#include <cstdio>

std::string ff::opcodeToString(const Opcode op) {
  switch (op) {
    case OP_POP:            return "OP_POP";
    case OP_PULL_UP:        return "OP_PULL_UP";
    case OP_ROL:            return "OP_ROL";
    case OP_DUP:            return "OP_DUP";
    case OP_NULL:           return "OP_NULL";
    case OP_TRUE:           return "OP_TRUE";
    case OP_FALSE:          return "OP_FALSE";
    case OP_NEW:            return "OP_NEW";
    case OP_COPY:           return "OP_COPY";
    case OP_LOAD_CONSTANT:  return "OP_LOAD_CONSTANT";
    case OP_NEW_GLOBAL:     return "OP_NEW_GLOBAL";
    case OP_GET_GLOBAL:     return "OP_GET_GLOBAL";
    case OP_SET_GLOBAL:     return "OP_SET_GLOBAL";
    case OP_SET_GLOBAL_REF: return "OP_SET_GLOBAL_REF";
    case OP_GET_LOCAL:      return "OP_GET_LOCAL";
    case OP_SET_LOCAL:      return "OP_SET_LOCAL";
    case OP_SET_LOCAL_REF:  return "OP_SET_LOCAL_REF";
    case OP_GET_FIELD:      return "OP_GET_FIELD";
    case OP_SET_FIELD:      return "OP_SET_FIELD";
    case OP_SET_FIELD_REF:  return "OP_SET_FIELD_REF";
    case OP_GET_STATIC:     return "OP_GET_STATIC";
    case OP_JUMP:           return "OP_JUMP";
    case OP_JUMP_TRUE:      return "OP_JUMP_TRUE";
    case OP_JUMP_FALSE:     return "OP_JUMP_FALSE";
    case OP_LOOP:           return "OP_LOOP";
    case OP_CALL:           return "OP_CALL";
    case OP_CALL_MEMBER:    return "OP_CALL_MEMBER";
    case OP_RETURN:         return "OP_RETURN";
    case OP_CAST:           return "OP_CAST";
    case OP_PRINT:          return "OP_PRINT";
    case OP_ADD:            return "OP_ADD";
    case OP_SUB:            return "OP_SUB";
    case OP_MUL:            return "OP_MUL";
    case OP_DIV:            return "OP_DIV";
    case OP_MOD:            return "OP_MOD";
    case OP_EQ:             return "OP_EQ";
    case OP_NEQ:            return "OP_NEQ";
    case OP_LT:             return "OP_LT";
    case OP_GT:             return "OP_GT";
    case OP_LE:             return "OP_LE";
    case OP_GE:             return "OP_GE";
    case OP_AND:            return "OP_AND";
    case OP_OR:             return "OP_OR";
    case OP_NOT:            return "OP_NOT";
    case OP_NEG:            return "OP_NEG";
    case OP_INC:            return "OP_INC";
    case OP_DEC:            return "OP_DEC";
    case OP_BREAKPOINT:     return "OP_BREAKPOINT";
    default:                return "?";
  }
}

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
    if (m_lines[i].startOffset == offset) {return m_lines[i].line;
    } else if (m_lines[i].startOffset < offset) {
      if (i+1 == m_lines.size()) {
        return m_lines[i].line;
      }
    } else {return m_lines[i-1].line;
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
  Opcode op = (Opcode) read<uint8_t>();
  printf("%s%04zx | %s", prefix.c_str(), getReadIndex(), opcodeToString(op).c_str());
  switch (op) {
    case OP_PULL_UP:
      printf(" %u\n", read<uint16_t>());
      return;
    case OP_LOAD_CONSTANT:
      printf(" %u\n", read<uint32_t>());
      return;
    case OP_GET_LOCAL:
      printf(" %u\n", read<uint32_t>());
      return;
    case OP_SET_LOCAL:
      printf(" %u\n",read<uint32_t>());
      return;
    case OP_SET_LOCAL_REF:
      printf(" %u\n",read<uint32_t>());
      return;
    case OP_JUMP:
      printf(" %u\n", read<uint16_t>());
      return;
    case OP_JUMP_TRUE:
      printf(" %u\n", read<uint16_t>());
      return;
    case OP_JUMP_FALSE:
      printf(" %u\n", read<uint16_t>());
      return;
    case OP_LOOP:
      printf(" %u\n", read<uint16_t>());
      return;
    default:
      printf("\n");
      return;
  }
}
