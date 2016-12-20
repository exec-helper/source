#include "execHelperOptions.h"

#include <iostream>
#include <string>

#include <boost/program_options.hpp>

#include "log/log.h"
#include "yaml/yaml.h"
#include "config/settingsNode.h"
#include "compiler.h"
#include "mode.h"
#include "executorInterface.h"

using std::cout;
using std::endl;
using std::string;
using std::vector;
using std::shared_ptr;
using std::make_shared;

using boost::program_options::options_description;
using boost::program_options::variables_map;
using boost::program_options::store;
using boost::program_options::notify;
using boost::program_options::command_line_parser;
using boost::program_options::value;
using boost::program_options::positional_options_description;

using execHelper::core::CommandCollection;
using execHelper::core::TargetDescription;
using execHelper::core::CompilerDescription;
using execHelper::core::AnalyzeDescription;

using execHelper::config::SettingsNode;

using execHelper::yaml::Yaml;
using execHelper::yaml::YamlFile;

namespace {
    class OptionDescriptions {
        public:
            OptionDescriptions() {
                // Add the default options
                m_optionDescription.add_options()
                    ("help,h", "Produce help message")
                    ("verbose,v", "Set verbosity")
                    ("command,z", value<CommandCollection>()->multitoken(), "Set commands")
                    ("target,t", value<TargetDescription::TargetCollection>()->multitoken(), "Set targets")
                    ("run-target,r", value<TargetDescription::RunTargetCollection>()->multitoken(), "Set run targets")
                    ("compiler,c", value<CompilerDescription::CompilerNames>()->multitoken(), "Set compilers")
                    ("mode,m", value<CompilerDescription::ModeNames>()->multitoken(), "Set modes")
                    ("architecture,a", value<CompilerDescription::ArchitectureNames>()->multitoken(), "Set architecture")
                    ("distribution,d", value<CompilerDescription::DistributionNames>()->multitoken(), "Set distribution")
                    ("analyze,y", value<AnalyzeDescription::AnalyzeCollection>()->multitoken(), "Set analyze methods")
                    ("settings-file,s", value<string>(), "Set settings file")
                    ("single-threaded,u", "Set multithreaded")
                ;
            }

            options_description getOptionDescriptions() {
                return m_optionDescription;
            }

            variables_map getOptionsMap(int argc, const char* const* argv, bool allowUnregistered = false) {
                // Assign positional arguments
                positional_options_description positionalOptionsDesc;
                positionalOptionsDesc.add("command", -1);

                variables_map optionsMap;
                if(! allowUnregistered) {
                    store(command_line_parser(argc, argv).options(m_optionDescription).positional(positionalOptionsDesc).run(), optionsMap);
                } else {
                    store(command_line_parser(argc, argv).options(m_optionDescription).positional(positionalOptionsDesc).options(m_optionDescription).allow_unregistered().run(), optionsMap);
                }
                notify(optionsMap);
                return optionsMap;
            }

        private:
            options_description m_optionDescription;
    };

    OptionDescriptions optionsDescriptions;
}

namespace execHelper { namespace core {
    ExecHelperOptions::ExecHelperOptions() noexcept :
        m_help(false),
        m_verbose(false),
        m_singleThreaded(false),
        m_target(new TargetDescription({"all"}, {"all"})),
        m_compiler(new CompilerDescription({Compiler("gcc")}, {Mode("release")}, {Architecture("x64")}, {Distribution("arch-linux")})),
        m_analyze(new AnalyzeDescription({"cppcheck", "clang-static-analyzer"}))
    {
        ;
    }

    ExecHelperOptions::ExecHelperOptions(const ExecHelperOptions& other) noexcept :
        m_help(other.m_help), 
        m_verbose(other.m_verbose),
        m_singleThreaded(other.m_singleThreaded),
        m_target(new TargetDescription(other.getTarget().getTargets(), other.getTarget().getRunTargets())),
        m_compiler(new CompilerDescription(other.getCompiler().getCompilers(), other.getCompiler().getModes(), other.getCompiler().getArchitectures(), other.getCompiler().getDistributions())),
        m_analyze(new AnalyzeDescription(other.getAnalyzeMethods())),
        m_settings(other.m_settings),
        m_executor(other.m_executor)
    {
        ;
    }

    bool ExecHelperOptions::getVerbosity() const noexcept {
        return m_verbose;
    }

    bool ExecHelperOptions::getSingleThreaded() const noexcept {
        return m_singleThreaded;
    }

    const CommandCollection& ExecHelperOptions::getCommands() const noexcept {
        return m_commands;
    }

    const CompilerDescription& ExecHelperOptions::getCompiler() const noexcept {
        assert(m_compiler);
        return *m_compiler;
    }

    const TargetDescription& ExecHelperOptions::getTarget() const noexcept {
        assert(m_target);
        return *m_target;
    }

    const AnalyzeDescription& ExecHelperOptions::getAnalyzeMethods() const noexcept {
        assert(m_analyze);
        return *m_analyze;
    }

    string ExecHelperOptions::getSettingsFile(int argc, const char* const * argv) const noexcept {
        variables_map optionsMap = optionsDescriptions.getOptionsMap(argc, argv, true);
        if(optionsMap.count("settings-file")) {
            return optionsMap["settings-file"].as<string>();
        }
        return ".exec-helper";
    }

    bool ExecHelperOptions::parse(int argc, const char* const * argv) {
        variables_map optionsMap = optionsDescriptions.getOptionsMap(argc, argv);
        if(optionsMap.count("help")) {
            m_help = true;
            return true;
        }

        if(optionsMap.count("verbose")) {
            m_verbose = true;
        }

        if(optionsMap.count("single-threaded")) {
            m_singleThreaded = true;
        }

        if(optionsMap.count("command")) {
            m_commands.clear();
            m_commands = optionsMap["command"].as<CommandCollection>();
        }

        if(optionsMap.count("target")) {
            TargetDescription::TargetCollection targets = optionsMap["target"].as<TargetDescription::TargetCollection>();
            m_target.reset(new TargetDescription(targets, m_target->getRunTargets()));
        }

        if(optionsMap.count("run-target")) {
            TargetDescription::RunTargetCollection runTargets = optionsMap["run-target"].as<TargetDescription::RunTargetCollection>();
            m_target.reset(new TargetDescription(m_target->getTargets(), runTargets));
        }

        if(optionsMap.count("compiler")) {
            CompilerDescription::CompilerCollection compilers = CompilerDescription::convertToCompilerCollection(optionsMap["compiler"].as<CompilerDescription::CompilerNames>());
            m_compiler.reset(new CompilerDescription(compilers, m_compiler->getModes(), m_compiler->getArchitectures(), m_compiler->getDistributions()));
        }

        if(optionsMap.count("mode")) {
            CompilerDescription::ModeCollection modes = CompilerDescription::convertToModeCollection(optionsMap["mode"].as<CompilerDescription::ModeNames>());
            m_compiler.reset(new CompilerDescription(m_compiler->getCompilers(), modes, m_compiler->getArchitectures(), m_compiler->getDistributions()));
        }

        if(optionsMap.count("architecture")) {
            CompilerDescription::ArchitectureCollection architectures = CompilerDescription::convertToArchitectureCollection(optionsMap["architecture"].as<CompilerDescription::ArchitectureNames>());
            m_compiler.reset(new CompilerDescription(m_compiler->getCompilers(), m_compiler->getModes(), architectures, m_compiler->getDistributions()));
        }

        if(optionsMap.count("distribution")) {
            CompilerDescription::DistributionCollection distributions = CompilerDescription::convertToDistributionCollection(optionsMap["distribution"].as<CompilerDescription::DistributionNames>());
            m_compiler.reset(new CompilerDescription(m_compiler->getCompilers(), m_compiler->getModes(), m_compiler->getArchitectures(), distributions));
        }

        if(optionsMap.count("analyze")) {
            AnalyzeDescription::AnalyzeCollection analyzeMethods = optionsMap["analyze"].as<AnalyzeDescription::AnalyzeCollection>();
            m_analyze.reset(new AnalyzeDescription(analyzeMethods));
        }

        return true;
    }

    bool ExecHelperOptions::parseSettingsFile(const std::string& file) noexcept {
        YamlFile yamlFile;
        yamlFile.file = file;
        Yaml yaml(yamlFile);
        if(! yaml.getTree({}, m_settings)) {
            LOG("Could not get settings tree");
            return false;
        }

        if(m_settings.contains("default-targets")) {
            TargetDescription::TargetCollection targets = m_settings["default-targets"].toStringCollection();
            m_target.reset(new TargetDescription(targets, m_target->getRunTargets()));
        }

        if(m_settings.contains("default-run-targets")) {
            TargetDescription::RunTargetCollection runTargets = m_settings["default-run-targets"].toStringCollection();
            m_target.reset(new TargetDescription(m_target->getTargets(), runTargets));
        }

        if(m_settings.contains("default-compilers")) {
            CompilerDescription::CompilerCollection compilers = CompilerDescription::convertToCompilerCollection(m_settings["default-compilers"].toStringCollection());
            m_compiler.reset(new CompilerDescription(compilers, m_compiler->getModes(), m_compiler->getArchitectures(), m_compiler->getDistributions()));
        }

        if(m_settings.contains("default-modes")) {
            CompilerDescription::ModeCollection modes = CompilerDescription::convertToModeCollection(m_settings["default-modes"].toStringCollection());
            m_compiler.reset(new CompilerDescription(m_compiler->getCompilers(), modes, m_compiler->getArchitectures(), m_compiler->getDistributions()));
        }

        if(m_settings.contains("default-architectures")) {
            CompilerDescription::ArchitectureCollection architectures = CompilerDescription::convertToArchitectureCollection(m_settings["default-architectures"].toStringCollection());
            m_compiler.reset(new CompilerDescription(m_compiler->getCompilers(), m_compiler->getModes(), architectures, m_compiler->getDistributions()));
        }

        if(m_settings.contains("default-distributions")) {
            CompilerDescription::DistributionCollection distributions = CompilerDescription::convertToDistributionCollection(m_settings["default-distributions"].toStringCollection());
            m_compiler.reset(new CompilerDescription(m_compiler->getCompilers(), m_compiler->getModes(), m_compiler->getArchitectures(), distributions));
        }
        return true;
    }

    bool ExecHelperOptions::containsHelp() const noexcept {
        return m_help;
    }

    const config::SettingsNode& ExecHelperOptions::getSettings() const noexcept {
        return m_settings;
    }

    config::SettingsNode& ExecHelperOptions::getSettings(const std::string& key) noexcept {
        if(m_settings.contains(key)) {
            return m_settings[key]; 
        }
        return m_settings;
    }

    const config::SettingsNode& ExecHelperOptions::getSettings(const std::string& key) const noexcept {
        if(m_settings.contains(key)) {
            return m_settings[key]; 
        }
        return m_settings;
    }


    void ExecHelperOptions::setExecutor(ExecutorInterface* const executor) noexcept {
        m_executor = executor;
    }

    ExecutorInterface* ExecHelperOptions::getExecutor() const noexcept {
        return m_executor;
    }

    void ExecHelperOptions::printHelp() const noexcept {
        user_feedback(optionsDescriptions.getOptionDescriptions());
    }

    shared_ptr<Options> ExecHelperOptions::clone() const noexcept {
        return make_shared<ExecHelperOptions>(*this);
    }
} }
