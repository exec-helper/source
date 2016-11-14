#ifndef __TARGET_ITERATOR_H__
#define __TARGET_ITERATOR_H__

#include "target.h"

class execHelper::core::TargetDescription;
class execHelper::core::TargetDescription::TargetCollection;
class execHelper::core::TargetDescription::RunTargetCollection;

namespace execHelper { namespace core { namespace implementation {
    template <typename PointerType> 
    class TargetIterator : public std::forward_iterator_tag {
        private:
            PointerType m_targetDescription;
            TargetDescription::TargetCollection::const_iterator m_targetIterator;
            TargetDescription::RunTargetCollection::const_iterator m_runTargetIterator;

        public:
            explicit TargetIterator(PointerType targetDescription, bool atEnd = false) :
                m_targetDescription(targetDescription),
                m_targetIterator(atEnd ? targetDescription.getTargets().end() : targetDescription.getTargets().begin()),
                m_runTargetIterator(targetDescription.getRunTargets().begin()
            {
                ;
            }

            TargetIterator<PointerType>& operator++() {
                // The iterator is at its end if the m_targetIterator is at its own end
                if(m_targetIterator != m_targetDescription.getTargets().end()) {
                    assert(m_runTargetIterator != m_targetDescription.getRunTargets().end());   // The runTargetIterator can only be at the end if m_targetIterator is also at the end

                    ++m_runTargetIterator;
                    if(m_runTargetIterator == m_targetDescription.getRunTargets().end()) {
                        // Means we went to are at the end of the run targets
                        m_runTargetIterator = m_targetDescription.getRunTargets().begin();
                        ++m_targetIterator;
                    }
                }
                return *this;
            }
    };
} } }

#endif /* __TARGET_ITERATOR_H__ */
