#include "target.h"

namespace execHelper {
    namespace core {
        TargetDescription::TargetDescription(const TargetCollection& targets, const RunTargetCollection& runTargetDescription) :
            m_targets(targets),
            m_runTargets(runTargetDescription)
        {
            ;
        }

        const execHelper::core::TargetDescription::TargetCollection& TargetDescription::getTargets() const noexcept {
            return m_targets;
        }
        
        const execHelper::core::TargetDescription::RunTargetCollection& TargetDescription::getRunTargets() const noexcept {
            return m_runTargets;
        }

        bool TargetDescription::operator==(const TargetDescription& other) const noexcept {
            return m_targets == other.m_targets && m_runTargets == other.m_runTargets;
        }

        bool TargetDescription::operator!=(const TargetDescription& other) const noexcept {
            return !(*this == other);
        }

        TargetDescription::iterator TargetDescription::begin() noexcept {
            return permutationBeginIterator(*this, m_targets, m_runTargets);
        }

        TargetDescription::const_iterator TargetDescription::begin() const noexcept {
            return permutationBeginIterator(*this, m_targets, m_runTargets);
        }

        TargetDescription::iterator TargetDescription::end() noexcept {
            return permutationEndIterator(*this, m_targets, m_runTargets);
        }

        TargetDescription::const_iterator TargetDescription::end() const noexcept {
            return permutationEndIterator(*this, m_targets, m_runTargets);
        }
    }
}
