#ifndef __COMPILER_STUB_H__
#define __COMPILER_STUB_H__

#include "core/compiler.h"

namespace execHelper {
    namespace core {
        namespace test {
            class CompilerStub : public Compiler {
                public:
                    CompilerStub() :
                      Compiler("compilerStub")
                    {
                        ;
                    }  
            };
        }
    }
}

#endif /* __COMPILER_STUB_H__ */
