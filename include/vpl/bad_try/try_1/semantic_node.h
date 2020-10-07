#pragma once

#include <memory>

namespace vpl {

class SemanticNode {
public:
    virtual ~SemanticNode() = default;
private:
};

template <class T>
const T* As(const std::unique_ptr<SemanticNode>& node) {
    return dynamic_cast<const T*>(node.get());
}

template <class T>
T* As(std::unique_ptr<SemanticNode>& node) {
    return dynamic_cast<T*>(node.get());
}

template <class T>
bool Is(const std::unique_ptr<SemanticNode>& node) {
    return As<T>(node) != nullptr;
}

}  /* namespace vpl */
