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

        execHelper::core::TargetDescription::iterator TargetDescription::begin() noexcept {
            return iterator(*this);
        }

        execHelper::core::TargetDescription::const_iterator TargetDescription::begin() const noexcept {
            return const_iterator(*this);
        }

        execHelper::core::TargetDescription::iterator TargetDescription::end() noexcept {
            return iterator(*this, true);
        }

        execHelper::core::TargetDescription::const_iterator TargetDescription::end() const noexcept {
            return const_iterator(*this, true);
        }
    }
}
