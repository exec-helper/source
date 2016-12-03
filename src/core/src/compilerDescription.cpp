#include "compilerDescription.h"

#include "log/log.h"

namespace {
}

namespace execHelper {
    namespace core {
        CompilerDescription::CompilerDescription(const CompilerNames& compilerNames, const ModeNames& modeNames, const ArchitectureNames& architectureNames) :
            m_compilers(convertToCompilerCollection(compilerNames)),
            m_modes(convertToModeCollection(modeNames)),
            m_architectures(convertToArchitectureCollection(architectureNames))
        {

        }

        CompilerDescription::CompilerDescription(const CompilerCollection& compilers, const ModeCollection& modes, const ArchitectureCollection& architectures) :
            m_compilers(compilers),
            m_modes(modes),
            m_architectures(architectures)
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

        bool CompilerDescription::operator==(const CompilerDescription& other) const noexcept {
            return m_compilers == other.m_compilers && m_modes == other.m_modes;
        }

        bool CompilerDescription::operator!=(const CompilerDescription& other) const noexcept {
            return !(*this == other);
        }

        CompilerDescription::iterator CompilerDescription::begin() noexcept {
            return iterator(m_compilers.begin(), m_modes.begin(), m_architectures.begin(), m_compilers.end(), m_modes.end(), m_architectures.end());
        }

        CompilerDescription::const_iterator CompilerDescription::begin() const noexcept {
            return const_iterator(m_compilers.begin(), m_modes.begin(), m_architectures.begin(), m_compilers.end(), m_modes.end(), m_architectures.end());
        }

        CompilerDescription::iterator CompilerDescription::end() noexcept {
            return iterator(m_compilers.end(), m_modes.end(), m_architectures.end(), m_compilers.end(), m_modes.end(), m_architectures.end());
        }

        CompilerDescription::const_iterator CompilerDescription::end() const noexcept {
            return const_iterator(m_compilers.end(), m_modes.end(), m_architectures.end(), m_compilers.end(), m_modes.end(), m_architectures.end());
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
                if(modeName == "debug") {
                    result.push_back(Mode("debug"));
                } else if(modeName == "release") {
                    result.push_back(Mode("release"));
                } else {
                    LOG("Error: invalid mode name: '" << modeName << "'");
                }
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

        CompilerDescriptionElement::CompilerDescriptionElement(const Compiler& compiler, const Mode& mode, const Architecture& architecture) noexcept :
            m_compiler(compiler),
            m_mode(mode),
            m_architecture(architecture)
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
    }
}
