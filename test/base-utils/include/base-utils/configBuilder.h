#ifndef CONFIG_BUILDER_INCLUDE
#define CONFIG_BUILDER_INCLUDE

#include "executionContent.h"
#include "testCommand.h"

namespace execHelper {
namespace test {
namespace baseUtils {
class ConfigBuilder {
  private:
    class ConfigBuilderRAII {
      public:
        explicit ConfigBuilderRAII(Commands& commands) noexcept;
        ConfigBuilderRAII(const ConfigBuilderRAII& other) = default;
        ConfigBuilderRAII(ConfigBuilderRAII&& other) = default;
        ~ConfigBuilderRAII();

        ConfigBuilderRAII& operator=(const ConfigBuilderRAII& other) = default;
        ConfigBuilderRAII& operator=(ConfigBuilderRAII&& other) =
            default; // NOLINT(misc-noexcept-move-constructor)

      private:
        Commands& m_commands;
    };

  public:
    ConfigBuilder();

    Commands::const_iterator begin() const noexcept;
    Commands::const_iterator end() const noexcept;

    void add(const TestCommand& command) noexcept;
    void add(TestCommand&& command) noexcept;
    void write(gsl::not_null<YamlWriter*> yaml) noexcept;

    ConfigBuilderRAII startIteration() noexcept;

  private:
    // Note: the io service needs to be created before and destroyed after the commands in this config builder
    IoService m_ioService;
    Commands m_commands;
};
} // namespace baseUtils
} // namespace test
} // namespace execHelper

#endif /* CONFIG_BUILDER_INCLUDE */
