#include "compilerDescription.h"

namespace execHelper {
    namespace core {
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
    }
}
