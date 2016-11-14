#ifndef __TARGET_H__
#define __TARGET_H__

#include <assert.h>
#include <string>
#include <vector>
#include <iterator>

#include "execHelperOptions.h"

namespace execHelper { namespace core {
    class TargetDescription {
        public:
            typedef std::string Target;
            typedef std::string RunTarget;
            typedef std::vector<Target> TargetCollection;
            typedef std::vector<RunTarget> RunTargetCollection;

            TargetDescription(const TargetCollection& targets, const RunTargetCollection& runTarget);

            const TargetCollection& getTargets() const noexcept;
            const RunTargetCollection& getRunTargets() const noexcept;

            // Iterator implementation
            template <typename IteratorType> 
            class TargetIterator : public std::iterator<
                        std::forward_iterator_tag,   // iterator_category
                        IteratorType,                      // value_type
                        IteratorType,                      // difference_type
                        IteratorType*,               // pointer
                        IteratorType                       // reference
                                      > {
                private:
                    IteratorType& m_targetDescription;
                    TargetDescription::TargetCollection::const_iterator m_targetIterator;
                    TargetDescription::RunTargetCollection::const_iterator m_runTargetIterator;

                public:
                    explicit TargetIterator(IteratorType& targetDescription, bool atEnd = false) :
                        m_targetDescription(targetDescription),
                        m_targetIterator(atEnd ? targetDescription.getTargets().end() : targetDescription.getTargets().begin()),
                        m_runTargetIterator(targetDescription.getRunTargets().begin())
                    {
                        ;
                    }

                    TargetIterator<IteratorType>& operator++() {
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

                    bool operator==(const TargetIterator<IteratorType>& other) const {
                        return (m_targetIterator == other.m_targetIterator && m_runTargetIterator == other.m_runTargetIterator);
                    }

                    bool operator!=(const TargetIterator<IteratorType>& other) const {
                        return !(*this == other);
                    }

                    IteratorType operator*() const {
                        return TargetDescription({*m_targetIterator}, {*m_runTargetIterator});
                    }
            };

            typedef TargetIterator<TargetDescription> iterator;
            typedef TargetIterator<const TargetDescription> const_iterator;

            iterator begin() noexcept;
            const_iterator begin() const noexcept;
            iterator end() noexcept;
            const_iterator end() const noexcept;

        private:
            TargetDescription() = delete;

            TargetCollection m_targets;
            RunTargetCollection m_runTargets;
    };
} }

#endif /* __TARGET_H__ */
