#include <vpl/function.h>
#include <vpl/misc.h>
#include <macros.h>

namespace vpl {

void Function::AddBlock(Object::Ptr& obj) {
    REQUIRE(Is<Block>(obj.get()));
    SetBody(std::shared_ptr<Block>(As<Block>(obj.release())));
}

}
