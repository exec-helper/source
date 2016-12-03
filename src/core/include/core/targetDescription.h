#ifndef __TARGET_H__
#define __TARGET_H__

#include <assert.h>
#include <string>
#include <vector>

#include "permutationIterator.h"

namespace execHelper { 
    namespace core {
        class TargetDescriptionElement {
            public:
                typedef std::string Target;
                typedef std::string RunTarget;

                TargetDescriptionElement(const Target& target, const RunTarget& runTarget);

                Target getTarget() const noexcept;
                RunTarget getRunTarget() const noexcept;
              
            private:
                const Target m_target;
                const RunTarget m_runTarget;
        };

        class TargetDescription {
            public:
                typedef TargetDescriptionElement::Target Target;
                typedef TargetDescriptionElement::RunTarget RunTarget;
                typedef std::vector<Target> TargetCollection;
                typedef std::vector<RunTarget> RunTargetCollection;

                typedef PermutationIterator<TargetDescriptionElement, TargetCollection, RunTargetCollection> iterator;
                typedef PermutationIterator<const TargetDescriptionElement, const TargetCollection, const RunTargetCollection> const_iterator;

                TargetDescription(const TargetCollection& targets, const RunTargetCollection& runTarget);

                const TargetCollection& getTargets() const noexcept;
                const RunTargetCollection& getRunTargets() const noexcept;

                bool operator==(const TargetDescription& other) const noexcept;
                bool operator!=(const TargetDescription& other) const noexcept;

                iterator begin() noexcept;
                const_iterator begin() const noexcept;
                iterator end() noexcept;
                const_iterator end() const noexcept;

            private:
                TargetDescription() = delete;

                TargetCollection m_targets;
                RunTargetCollection m_runTargets;
        };
    } 
}

#endif /* __TARGET_H__ */
