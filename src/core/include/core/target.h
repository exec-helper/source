#ifndef __TARGET_H__
#define __TARGET_H__

#include <assert.h>
#include <string>
#include <vector>

#include "permutationIterator.h"

namespace execHelper { 
    namespace core {
        class TargetDescription {
            public:
                typedef std::string Target;
                typedef std::string RunTarget;
                typedef std::vector<Target> TargetCollection;
                typedef std::vector<RunTarget> RunTargetCollection;

                typedef PermutationIterator<TargetDescription, TargetCollection, RunTargetCollection> iterator;
                typedef PermutationIterator<const TargetDescription, TargetCollection, RunTargetCollection> const_iterator;

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
