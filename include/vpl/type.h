#pragma once

#include <memory>
#include <ostream>

namespace vpl {

struct Type {
    using Ptr = std::shared_ptr<Type>;

    enum SimpleType {
        kTypeUndefined,
        kTypeInt,
        kTypeBool,
        kTypeDouble,
    };

    int num_of_ptrs_;
    bool is_reference_;
    SimpleType simple_type_;
    std::string name_;

    bool IsEquivalent(const Type::Ptr& other) const {
        return num_of_ptrs_ == other->num_of_ptrs_ && is_reference_ == other->is_reference_ &&
            simple_type_ == other->simple_type_;
    }

    Type(int num_of_ptrs, bool is_reference, SimpleType simple_type, const std::string& name)
        : num_of_ptrs_(num_of_ptrs), is_reference_(is_reference), simple_type_(simple_type), name_(name) {
    }

    Type() : Type(0, false, kTypeUndefined, "<UNDEFINED>") {}
    Type(SimpleType simple_type, const std::string& name) : Type(0, false, simple_type, name) {}

    Type::Ptr Copy() const {
        return std::make_shared<Type>(num_of_ptrs_, is_reference_, simple_type_, name_);
    }
};

struct NamedType {
    std::string name;
    Type::Ptr type;
};

static std::ostream& operator<<(std::ostream& out, const Type& type) {
    out << type.name_;
    for (int i = 0; i < type.num_of_ptrs_; ++i) {
        out << '*';
    }
    return out;
}

}  /* namespace vpl */
