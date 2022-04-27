#ifndef _FF_ANNOTATION_H_
#define _FF_ANNOTATION_H_ 1

#include <ff/ref.h>
#include <vector>
#include <string>

namespace ff {

struct TypeAnnotation {
  enum Type {
    TATYPE_DEFAULT,
    TATYPE_FUNCTION,
    TATYPE_UNION,
  };

  Type annotationType;
  std::string typeName;
  bool isInferred = false;

  TypeAnnotation(Type type, bool isInferred = false);
  TypeAnnotation(const std::string& typeName = "any", bool isInferred = false);
  virtual ~TypeAnnotation() = default;

  virtual bool operator==(const TypeAnnotation& rhs) const;
  virtual bool operator!=(const TypeAnnotation& rhs) const;

  virtual std::string toString() const;

  static Ref<TypeAnnotation> create(const std::string& typeName = "any", bool isInferred = false);

  static Ref<TypeAnnotation> any();
  static Ref<TypeAnnotation> nothing();
  static Ref<TypeAnnotation> type();
};

struct FunctionAnnotation : public TypeAnnotation {
  std::vector<Ref<TypeAnnotation>> arguments;
  Ref<TypeAnnotation> returnType;

  FunctionAnnotation(std::vector<Ref<TypeAnnotation>> arguments, Ref<TypeAnnotation> returnType, bool isInferred = false);
  ~FunctionAnnotation() = default;

  bool operator==(const TypeAnnotation& rhs) const override;
  std::string toString() const override;

  static Ref<FunctionAnnotation> create(std::vector<Ref<TypeAnnotation>> arguments, Ref<TypeAnnotation> returnType, bool isInferred = false);
};

struct UnionAnnotation : public TypeAnnotation {
  std::vector<Ref<TypeAnnotation>> types;

  UnionAnnotation(std::vector<Ref<TypeAnnotation>> types, bool isInferred = false);
  ~UnionAnnotation() = default;

  bool operator==(const TypeAnnotation& rhs) const override;
  std::string toString() const override;

  static Ref<UnionAnnotation> create(std::vector<Ref<TypeAnnotation>> types, bool isInferred = false);
};

/*class GenericAnnotation : public TypeAnnotation {
  // bounds, etc
};*/

} /* namespace ff */

#endif /* _FF_ANNOTATION_H_ */