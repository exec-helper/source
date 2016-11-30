#ifndef __COMPILER_DESCRIPTION_H__
#define __COMPILER_DESCRIPTION_H__

#include <vector>
#include <iterator>

#include "permutationIterator.h"
#include "compiler.h"
#include "mode.h"

namespace execHelper { 
    namespace core {
        class CompilerDescription {
            public:
                typedef std::vector<std::string> CompilerNames;
                typedef std::vector<std::string> ModeNames;
                typedef std::vector<std::string> ArchitectureNames;

                typedef std::vector<Compiler> CompilerCollection;
                typedef std::vector<Mode> ModeCollection;

                typedef PermutationIterator<CompilerDescription, CompilerCollection, ModeCollection> iterator;
                typedef PermutationIterator<const CompilerDescription, CompilerCollection, ModeCollection> const_iterator;

                CompilerDescription(const CompilerNames& compilerNames, const ModeNames& modeNames);
                CompilerDescription(const CompilerCollection& compilers, const ModeCollection& modes);

                const CompilerCollection& getCompilers() const noexcept;
                const ModeCollection& getModes() const noexcept;

                bool operator==(const CompilerDescription& other) const noexcept;
                bool operator!=(const CompilerDescription& other) const noexcept;

                iterator begin() noexcept;
                const_iterator begin() const noexcept;
                iterator end() noexcept;
                const_iterator end() const noexcept;

                static CompilerCollection convertToCompilerCollection(const CompilerNames& compilers) noexcept;
                static ModeCollection convertToModeCollection(const ModeNames& compilers) noexcept;

            private:
                CompilerDescription() = delete;

                CompilerCollection m_compilers;
                ModeCollection m_modes;
        };
    } 
}

#endif /* __COMPILER_DESCRIPTION_H__ */
