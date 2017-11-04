#ifndef __MODE_STUB_H__
#define __MODE_STUB_H__

#include "core/mode.h"

namespace execHelper {
namespace core {
namespace test {
class ModeStub : public Mode {
  public:
    ModeStub() : Mode("modeStub") { ; }
};
} // namespace test
} // namespace core
} // namespace execHelper

#endif /* __MODE_STUB_H__ */
