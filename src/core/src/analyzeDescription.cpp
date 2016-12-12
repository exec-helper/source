#include "analyzeDescription.h"

namespace execHelper { namespace core {

    AnalyzeDescription::AnalyzeDescription(const std::initializer_list<Analyze>& analyzeList) :
        m_analyzeList(analyzeList)
    {
        ;
    }

    AnalyzeDescription::AnalyzeDescription(const AnalyzeCollection& analyzeList) :
        m_analyzeList(analyzeList)
    {
        ;
    }

    const AnalyzeDescription::AnalyzeCollection& AnalyzeDescription::getAnalyzeMethods() const noexcept {
        return m_analyzeList;
    }
} }
