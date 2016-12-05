#include "compilerDescription.h"

#include "log/log.h"

namespace {
}

namespace execHelper {
    namespace core {
        CompilerDescription::CompilerDescription(const CompilerNames& compilerNames, const ModeNames& modeNames, const ArchitectureNames& architectureNames, const DistributionNames& distributions) :
            m_compilers(convertToCompilerCollection(compilerNames)),
            m_modes(convertToModeCollection(modeNames)),
            m_architectures(convertToArchitectureCollection(architectureNames)),
            m_distributions(convertToDistributionCollection(distributions))
        {
            ;
        }

        CompilerDescription::CompilerDescription(const CompilerCollection& compilers, const ModeCollection& modes, const ArchitectureCollection& architectures, const DistributionCollection& distributions) :
            m_compilers(compilers),
            m_modes(modes),
            m_architectures(architectures),
            m_distributions(distributions)
        {
            ;
        }

        const CompilerDescription::CompilerCollection& CompilerDescription::getCompilers() const noexcept {
            return m_compilers;
        }

        const CompilerDescription::ModeCollection& CompilerDescription::getModes() const noexcept {
            return m_modes;
        }

        const CompilerDescription::ArchitectureCollection& CompilerDescription::getArchitectures() const noexcept {
            return m_architectures;
        }

        const CompilerDescription::DistributionCollection& CompilerDescription::getDistributions() const noexcept {
            return m_distributions;
        }

        bool CompilerDescription::operator==(const CompilerDescription& other) const noexcept {
            return m_compilers == other.m_compilers && m_modes == other.m_modes && m_architectures == other.m_architectures && m_distributions == other.m_distributions;
        }

        bool CompilerDescription::operator!=(const CompilerDescription& other) const noexcept {
            return !(*this == other);
        }

        CompilerDescription::iterator CompilerDescription::begin() noexcept {
            return iterator(m_compilers.begin(), m_modes.begin(), m_architectures.begin(), m_distributions.begin(), m_compilers.end(), m_modes.end(), m_architectures.end(), m_distributions.end());
        }

        CompilerDescription::const_iterator CompilerDescription::begin() const noexcept {
            return const_iterator(m_compilers.begin(), m_modes.begin(), m_architectures.begin(), m_distributions.begin(), m_compilers.end(), m_modes.end(), m_architectures.end(), m_distributions.end());
        }

        CompilerDescription::iterator CompilerDescription::end() noexcept {
            return iterator(m_compilers.end(), m_modes.end(), m_architectures.end(), m_distributions.end(), m_compilers.end(), m_modes.end(), m_architectures.end(), m_distributions.end());
        }

        CompilerDescription::const_iterator CompilerDescription::end() const noexcept {
            return const_iterator(m_compilers.end(), m_modes.end(), m_architectures.end(), m_distributions.end(), m_compilers.end(), m_modes.end(), m_architectures.end(), m_distributions.end());
        }

        CompilerDescription::CompilerCollection CompilerDescription::convertToCompilerCollection(const CompilerNames& compilers) noexcept {
            CompilerCollection result;
            for(const auto& compilerName : compilers) {
                result.push_back(Compiler(compilerName));
            }
            return result;
        }

        CompilerDescription::ModeCollection CompilerDescription::convertToModeCollection(const ModeNames& modes) noexcept {
            ModeCollection result;
            for(const auto& modeName : modes) {
                result.push_back(Mode(modeName));
            }
            return result;
        }

        CompilerDescription::ArchitectureCollection CompilerDescription::convertToArchitectureCollection(const ArchitectureNames& architectures) noexcept {
            ArchitectureCollection result;
            for(const auto& architecture : architectures) {
                result.push_back(Architecture(architecture));
            }
            return result;
        }
        
        CompilerDescription::DistributionCollection CompilerDescription::convertToDistributionCollection(const DistributionNames& distributions) noexcept {
            DistributionCollection result;
            for(const auto& distribution : distributions) {
                result.push_back(distribution);
            }
            return result;
        }

        CompilerDescriptionElement::CompilerDescriptionElement(const Compiler& compiler, const Mode& mode, const Architecture& architecture, const Distribution& distribution) noexcept :
            m_compiler(compiler),
            m_mode(mode),
            m_architecture(architecture),
            m_distribution(distribution)
        {
            ;
        }

        Compiler CompilerDescriptionElement::getCompiler() const noexcept {
            return m_compiler;
        }

        Mode CompilerDescriptionElement::getMode() const noexcept {
            return m_mode;
        }

        Architecture CompilerDescriptionElement::getArchitecture() const noexcept {
            return m_architecture;
        }

        Distribution CompilerDescriptionElement::getDistribution() const noexcept {
            return m_distribution;
        }
    }
}
