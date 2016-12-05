#ifndef __DISTRIBUTION_H__
#define __DISTRIBUTION_H__

#include <string>

namespace execHelper {
    namespace core {
        class Distribution {
            public:
                Distribution(const std::string& distributionName) :
                    m_distribution(distributionName)
                {
                    ;
                }

                virtual const std::string& getDistribution() const noexcept {
                    return m_distribution;
                }

                virtual bool operator==(const Distribution& other) const noexcept {
                    return m_distribution == other.m_distribution;
                }

                virtual bool operator!=(const Distribution& other) const noexcept {
                    return !(*this == other);
                }

            private:
                std::string m_distribution;
        };
    }
}

#endif /* __DISTRIBUTION_H__ */
