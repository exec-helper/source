#ifndef __COMPILER_DESCRIPTION_H__
#define __COMPILER_DESCRIPTION_H__

#include <vector>
#include <iterator>

#include "permutationIterator.h"
#include "compiler.h"
#include "mode.h"
#include "architecture.h"
#include "distribution.h"

namespace execHelper { 
    namespace core {
        class CompilerDescriptionElement {
            public:
                CompilerDescriptionElement(const Compiler& compiler, const Mode& mode, const Architecture& architecture, const Distribution& distribution) noexcept;

                Compiler getCompiler() const noexcept;
                Mode getMode() const noexcept;
                Architecture getArchitecture() const noexcept;
                Distribution getDistribution() const noexcept;

            private:
                const Compiler m_compiler;
                const Mode m_mode;
                const Architecture m_architecture;
                const Distribution m_distribution;
        };

        class CompilerDescription {
            public:
                typedef std::vector<std::string> CompilerNames;
                typedef std::vector<std::string> ModeNames;
                typedef std::vector<std::string> ArchitectureNames;
                typedef std::vector<std::string> DistributionNames;

                typedef std::vector<Compiler> CompilerCollection;
                typedef std::vector<Mode> ModeCollection;
                typedef std::vector<Architecture> ArchitectureCollection;
                typedef std::vector<Distribution> DistributionCollection;

                typedef PermutationIterator<CompilerDescriptionElement, CompilerCollection, ModeCollection, ArchitectureCollection, DistributionCollection> iterator;
                typedef PermutationIterator<const CompilerDescriptionElement, const CompilerCollection, const ModeCollection, const ArchitectureCollection, const DistributionCollection> const_iterator;

                CompilerDescription(const CompilerNames& compilerNames, const ModeNames& modeNames, const ArchitectureNames& architectures, const DistributionNames& distributions);
                CompilerDescription(const CompilerCollection& compilers, const ModeCollection& modes, const ArchitectureCollection& architectures, const DistributionCollection& distributions);

                const CompilerCollection& getCompilers() const noexcept;
                const ModeCollection& getModes() const noexcept;
                const ArchitectureCollection& getArchitectures() const noexcept;
                const DistributionCollection& getDistributions() const noexcept;

                bool operator==(const CompilerDescription& other) const noexcept;
                bool operator!=(const CompilerDescription& other) const noexcept;

                iterator begin() noexcept;
                const_iterator begin() const noexcept;
                iterator end() noexcept;
                const_iterator end() const noexcept;

                static CompilerCollection convertToCompilerCollection(const CompilerNames& compilers) noexcept;
                static ModeCollection convertToModeCollection(const ModeNames& compilers) noexcept;
                static ArchitectureCollection convertToArchitectureCollection(const ArchitectureNames& architectures) noexcept;
                static DistributionCollection convertToDistributionCollection(const DistributionNames& distributions) noexcept;

            private:
                CompilerDescription() = delete;

                CompilerCollection m_compilers;
                ModeCollection m_modes;
                ArchitectureCollection m_architectures;
                DistributionCollection m_distributions;
        };
    } 
}

#endif /* __COMPILER_DESCRIPTION_H__ */
