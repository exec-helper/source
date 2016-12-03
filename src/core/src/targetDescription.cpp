#include "targetDescription.h"

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
            return iterator(m_targets.begin(), m_runTargets.begin(), m_targets.end(), m_runTargets.end());
        }

        TargetDescription::const_iterator TargetDescription::begin() const noexcept {
            return const_iterator(m_targets.begin(), m_runTargets.begin(), m_targets.end(), m_runTargets.end());
        }

        TargetDescription::iterator TargetDescription::end() noexcept {
            return iterator(m_targets.end(), m_runTargets.end(), m_targets.end(), m_runTargets.end());
        }

        TargetDescription::const_iterator TargetDescription::end() const noexcept {
            return const_iterator(m_targets.end(), m_runTargets.end(), m_targets.end(), m_runTargets.end());
        }

        TargetDescriptionElement::TargetDescriptionElement(const Target& target, const RunTarget& runTarget) :
            m_target(target),
            m_runTarget(runTarget)
        {
            ;
        }

        TargetDescriptionElement::Target TargetDescriptionElement::getTarget() const noexcept {
            return m_target;
        }

        TargetDescriptionElement::RunTarget TargetDescriptionElement::getRunTarget() const noexcept {
            return m_runTarget;
        }
 
    }
}
