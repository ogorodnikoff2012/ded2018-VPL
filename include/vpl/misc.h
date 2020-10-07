#pragma once

#include "object.h"

namespace vpl {

template <class T>
const T* As(const Object* obj) {
    return dynamic_cast<const T*>(obj);
}

template <class T>
T* As(Object* obj) {
    return dynamic_cast<T*>(obj);
}

template <class T>
bool Is(const Object* obj) {
    return As<T>(obj) != nullptr;
}

}  /* namespace vpl */
