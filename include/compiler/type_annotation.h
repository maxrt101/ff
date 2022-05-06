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
  bool isRef = false;

  explicit TypeAnnotation(Type type, bool isInferred = false, bool isRef = false);
  TypeAnnotation(const std::string& typeName = "any", bool isInferred = false, bool isRef = false);
  virtual ~TypeAnnotation() = default;

  virtual bool operator==(const TypeAnnotation& rhs) const;
  virtual bool operator!=(const TypeAnnotation& rhs) const;

  virtual std::string toString() const;
  virtual Ref<TypeAnnotation> copy();

  static Ref<TypeAnnotation> create(const std::string& typeName = "any", bool isInferred = false, bool isRef = false);

  static Ref<TypeAnnotation> any();
  static Ref<TypeAnnotation> nothing();
  static Ref<TypeAnnotation> type();
};

struct FunctionAnnotation : public TypeAnnotation {
  std::vector<Ref<TypeAnnotation>> arguments;
  Ref<TypeAnnotation> returnType;

  FunctionAnnotation(const std::vector<Ref<TypeAnnotation>>& arguments, Ref<TypeAnnotation> returnType, bool isInferred = false);
  ~FunctionAnnotation() = default;

  bool operator==(const TypeAnnotation& rhs) const override;
  std::string toString() const override;
  Ref<TypeAnnotation> copy() override;

  static Ref<FunctionAnnotation> create(const std::vector<Ref<TypeAnnotation>>& arguments, Ref<TypeAnnotation> returnType, bool isInferred = false);
};

struct UnionAnnotation : public TypeAnnotation {
  std::vector<Ref<TypeAnnotation>> types;

  explicit UnionAnnotation(const std::vector<Ref<TypeAnnotation>>& types, bool isInferred = false);
  ~UnionAnnotation() = default;

  bool operator==(const TypeAnnotation& rhs) const override;
  std::string toString() const override;
  Ref<TypeAnnotation> copy() override;

  static Ref<UnionAnnotation> create(const std::vector<Ref<TypeAnnotation>>& types, bool isInferred = false);
};

/*class GenericAnnotation : public TypeAnnotation {
  // bounds, etc
};*/

} /* namespace ff */

#endif /* _FF_ANNOTATION_H_ */