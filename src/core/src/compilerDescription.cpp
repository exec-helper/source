#include "compilerDescription.h"

#include "log/log.h"

namespace {
}

namespace execHelper {
    namespace core {
        CompilerDescription::CompilerDescription(const CompilerNames& compilerNames, const ModeNames& modeNames) :
            m_compilers(convertToCompilerCollection(compilerNames)),
            m_modes(convertToModeCollection(modeNames))
        {

        }

        CompilerDescription::CompilerDescription(const CompilerCollection& compilers, const ModeCollection& modes) :
            m_compilers(compilers),
            m_modes(modes)
        {
            ;
        }

        const CompilerDescription::CompilerCollection& CompilerDescription::getCompilers() const noexcept {
            return m_compilers;
        }

        const CompilerDescription::ModeCollection& CompilerDescription::getModes() const noexcept {
            return m_modes;
        }

        bool CompilerDescription::operator==(const CompilerDescription& other) const noexcept {
            return m_compilers == other.m_compilers && m_modes == other.m_modes;
        }

        bool CompilerDescription::operator!=(const CompilerDescription& other) const noexcept {
            return !(*this == other);
        }

        CompilerDescription::iterator CompilerDescription::begin() noexcept {
            return permutationBeginIterator(*this, m_compilers, m_modes);
        }

        CompilerDescription::const_iterator CompilerDescription::begin() const noexcept {
            return permutationBeginIterator(*this, m_compilers, m_modes);
        }

        CompilerDescription::iterator CompilerDescription::end() noexcept {
            return permutationEndIterator(*this, m_compilers, m_modes);
        }

        CompilerDescription::const_iterator CompilerDescription::end() const noexcept {
            return permutationEndIterator(*this, m_compilers, m_modes);
        }

        CompilerDescription::CompilerCollection CompilerDescription::convertToCompilerCollection(const CompilerNames& compilers) noexcept {
            CompilerCollection result;
            for(const auto& compilerName : compilers) {
                if(compilerName == "gcc") {
                    result.push_back(Gcc());
                } else if(compilerName == "clang") {
                    result.push_back(Clang());
                } else {
                    LOG("Error: invalid compiler name: '" << compilerName << "'");
                }
            }
            return result;
        }

        CompilerDescription::ModeCollection CompilerDescription::convertToModeCollection(const ModeNames& modes) noexcept {
            ModeCollection result;
            for(const auto& modeName : modes) {
                if(modeName == "debug") {
                    result.push_back(Debug());
                } else if(modeName == "release") {
                    result.push_back(Release());
                } else {
                    LOG("Error: invalid mode name: '" << modeName << "'");
                }
            }
            return result;
        }
    }
}
