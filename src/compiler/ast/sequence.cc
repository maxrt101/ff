#include <ff/ast/sequence.h>

ff::ast::Sequence::Sequence(const std::vector<Node*>& sequence)
  : Node(NTYPE_SEQUENCE), m_sequence(sequence) {}

std::vector<ff::ast::Node*> ff::ast::Sequence::getSequence() const {
  return m_sequence;
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
