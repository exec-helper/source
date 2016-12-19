#ifndef __OPTIONS_STUB_H__
#define __OPTIONS_STUB_H__

#include <assert.h>

#include "core/options.h"
#include "core/targetDescription.h"
#include "core/compilerDescription.h"
#include "core/analyzeDescription.h"
#include "config/settingsNode.h"

namespace execHelper {
    namespace test {
        class OptionsStub : public core::Options {
            public:
                OptionsStub() :
                    Options(),
                    m_verbosity(false),
                    m_singleThreaded(false),
                    m_targets({}, {}),
                    m_compilers(core::CompilerDescription::CompilerCollection({}), {}, {}, {}),
                    m_analyze({})
                {
                    ;
                }

                virtual bool parse(int /*argc*/, const char* const * /*argv*/) override {
                    assert(false);  //  This function is currently not supported
                }

                virtual bool parseSettingsFile(const std::string& /*file*/) noexcept override {
                    assert(false);  //  This function is currently not supported
                }

                virtual bool getVerbosity() const noexcept override {
                    return m_verbosity;
                }

                virtual bool getSingleThreaded() const noexcept override {
                    return m_singleThreaded;
                }

                virtual const core::CommandCollection& getCommands() const noexcept override {
                    return m_commands;
                }

                virtual const core::TargetDescription& getTarget() const noexcept override {
                    return m_targets;
                }

                virtual const core::CompilerDescription& getCompiler() const noexcept override {
                    return m_compilers;
                }

                virtual const core::AnalyzeDescription& getAnalyzeMethods() const noexcept override {
                    return m_analyze;
                }

                virtual const config::SettingsNode& getSettings() const noexcept override {
                    return m_settings;
                }

                virtual config::SettingsNode& getSettings(const std::string& key) noexcept override {
                    if(m_settings.contains(key)) {
                        return m_settings[key];
                    }
                    return m_settings;
                }

                virtual const config::SettingsNode& getSettings(const std::string& key) const noexcept override {
                    if(m_settings.contains(key)) {
                        return m_settings[key];
                    }
                    return m_settings[key];
                }

                virtual bool containsHelp() const noexcept override {
                    return m_containsHelp;
                }

                virtual void setExecutor(core::ExecutorInterface* const executor) noexcept override {
                    m_executor = executor;
                }

                virtual core::ExecutorInterface* getExecutor() const noexcept override {
                    return m_executor;
                }
  
                virtual std::shared_ptr<Options> clone() const noexcept override {
                    return std::make_shared<OptionsStub>(*this);
                }

                bool m_verbosity;
                bool m_singleThreaded;
                core::CommandCollection m_commands;
                core::TargetDescription m_targets;
                core::CompilerDescription m_compilers;
                core::AnalyzeDescription m_analyze;
                config::SettingsNode m_settings;
                bool m_containsHelp;
                core::ExecutorInterface* m_executor;
        };
    }
}

#endif  /* __OPTIONS_STUB_H__ */
