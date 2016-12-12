#ifndef __ANALYZE_DESCRIPTION_H__
#define __ANALYZE_DESCRIPTION_H__

#include <vector>
#include <string>

namespace execHelper {
    namespace core {
        class AnalyzeDescription {
            public:
                typedef std::string Analyze;
                typedef std::vector<Analyze> AnalyzeCollection;

                AnalyzeDescription(const std::initializer_list<Analyze>& analyzeList);
                AnalyzeDescription(const AnalyzeCollection& analyzeList);

                const AnalyzeCollection& getAnalyzeMethods() const noexcept;

            private:
                AnalyzeCollection m_analyzeList;
        };
    }
}

#endif  /* __ANALYZE_DESCRIPTION_H__ */
