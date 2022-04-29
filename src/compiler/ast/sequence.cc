#include <ff/ast/sequence.h>

ff::ast::Sequence::Sequence(const std::vector<Node*>& sequence)
  : Node(NTYPE_SEQUENCE), m_sequence(sequence) {}

std::vector<ff::ast::Node*> ff::ast::Sequence::getSequence() const {
  return m_sequence;
}

bool ff::ast::Sequence::hasFunctionCalls() const {
  for (int i = 0; i < m_sequence.size(); i++) {
    if (m_sequence[i]->getType() == NTYPE_CALL) {
      return false;
    }
  }
  return true;
}

std::string ff::ast::Sequence::toString() const {
  std::string result;

  for (int i = 0; i < m_sequence.size(); i++) {
    result += m_sequence[i]->toString();
    if (i+1 < m_sequence.size()) {
      result += ".";
    }
  }
  return result;
}
