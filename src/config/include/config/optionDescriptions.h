#ifndef __OPTION_DESCRIPTIONS_H__
#define __OPTION_DESCRIPTIONS_H__

#include <memory>
#include <optional>
#include <vector>

#include <boost/program_options.hpp>
#include <gsl/span>
#include <gsl/string_span>

#include "log/assertions.h"

#include "log/log.h"
#include "variablesMap.h"

namespace execHelper::config {
class Argv;
} // namespace execHelper::config

namespace execHelper::config {
using ArgumentOption = std::string;
using ArgumentOptions = std::vector<ArgumentOption>;
using Args = gsl::span<const gsl::czstring<>>;

/**
 * \brief Interface for determining the option characteristics associated with
 * an argument option
 */
class OptionInterface {
  public:
    virtual ~OptionInterface() {}

    /**
     * Returns the id for identifying the specified option throughout the system
     *
     * \returns The unique id associated with this option
     */
    [[nodiscard]] virtual auto getId() const noexcept -> std::string = 0;

    /**
     * Extract the value(s) associated with this option from the optionsMap
     *
     * \param[out] variablesMap The variables map to write to
     * \param[in] optionsMap    The options map to take the value(s) from
     * \returns True    if the associated value(s) were successfully extracted
     *          False   otherwise
     */
    virtual auto toMap(config::VariablesMap& variablesMap,
                       const boost::program_options::variables_map& optionsMap)
        const noexcept -> bool = 0;

    /**
     * Return the associated type value of this options in order to be able to
     * properly parse it
     *
     * \returns The implementation-specific semantic to be used by the
     * optionparser.
     */
    virtual auto getTypeValue() const noexcept
        -> const boost::program_options::value_semantic* = 0;

  protected:
    OptionInterface() {}
};

/**
 * \brief   Class for generic OptionInterface parent functions
 */
class OptionBase : public OptionInterface {
  public:
    virtual ~OptionBase() {}

    [[nodiscard]] auto getId() const noexcept -> std::string override {
        return m_identifyingOption;
    }

    /**
     * Returns the argument options for this option, in addition to the one
     * returned by getId().
     *
     * \returns A collection of additional option keys associated with this
     * option
     */
    virtual auto getArgumentOptions() const noexcept -> const ArgumentOptions& {
        return m_argumentOptions;
    }

    /**
     * Returns the explanation associated with this option
     *
     * \returns The explanation
     */
    virtual auto getExplanation() const noexcept -> const std::string& {
        return m_explanation;
    }

    auto toMap(config::VariablesMap& /*variablesMap*/,
               const boost::program_options::variables_map& /*optionsMap*/)
        const noexcept -> bool override {
        ensuresMessage(false, "We should not get here: all children should "
                              "implement this function");
        return false;
    }

    auto getTypeValue() const noexcept
        -> const boost::program_options::value_semantic* override {
        ensuresMessage(false, "We should not get here: all children should "
                              "implement this function");
        return boost::program_options::value<char>();
    }

  protected:
    /**
     * \param[in] identifyingOption The unique string to identify this option
     * with \param[in] argumentOptions   A collection of additional,
     * non-identifying options \param[in] explanation       The explanation
     * associated with this option
     */
    OptionBase(const std::string& identifyingOption,
               const ArgumentOptions& argumentOptions,
               const std::string& explanation)
        : OptionInterface(),
          m_identifyingOption(identifyingOption),
          m_argumentOptions(argumentOptions),
          m_explanation(explanation) {
        expectsMessage(argumentOptions.size() <= 1U,
                       "Currently we support only one additional, "
                       "non-identifying option: the associated short option");
    }

    std::string m_identifyingOption; //!< The identifying option associated with
                                     //!< this option
    ArgumentOptions m_argumentOptions; //!< Additional non-identifying options
                                       //!< associated with this option
    std::string m_explanation;         //!< The associated explanation
};
typedef std::vector<OptionBase> OptionCollection;

/**
 * \brief Class implementing an argument option that fits an option value of
 * type T
 */
template <typename T> class Option : public OptionBase {
  public:
    /*! @copydoc OptionBase::OptionBase(const std::string&, const ArgumentOptions&, const std::string&)
     */
    Option(const std::string& identifyingOption,
           const ArgumentOptions& argumentOptions,
           const std::string& explanation)
        : OptionBase(identifyingOption, argumentOptions, explanation) {
        ;
    }

    virtual ~Option() {}

    virtual auto toMap(config::VariablesMap& variablesMap,
                       const boost::program_options::variables_map& optionsMap)
        const noexcept -> bool override {
        try {
            return variablesMap.replace(
                m_identifyingOption,
                optionsMap[m_identifyingOption].template as<T>());
        } catch(const boost::bad_any_cast& e) {
            user_feedback_error("Bad_any_cast exception caught: " << e.what());
            return false;
        }
    }

    virtual auto getTypeValue() const noexcept
        -> const boost::program_options::value_semantic* override {
        return boost::program_options::value<T>();
    }
};

/**
 * \brief Class implementing a flag
 */
template <> class Option<bool> : public OptionBase {
  public:
    /*! @copydoc OptionBase::OptionBase(const std::string&, const ArgumentOptions&, const std::string&)
     */
    Option(const std::string& identifyingOption,
           const ArgumentOptions& argumentOptions,
           const std::string& explanation)
        : OptionBase(identifyingOption, argumentOptions, explanation) {
        ;
    }

    virtual ~Option() {}

    virtual auto toMap(config::VariablesMap& variablesMap,
                       const boost::program_options::variables_map& optionsMap)
        const noexcept -> bool override {
        if(optionsMap.count(m_identifyingOption) > 0) {
            try {
                if(optionsMap[m_identifyingOption].as<bool>()) {
                    return variablesMap.replace(m_identifyingOption, "1");
                }
            } catch(const boost::bad_any_cast& e) {
                user_feedback_error(
                    "Bad_any_cast exception caught: " << e.what());
                return false;
            }
        }
        return variablesMap.replace(m_identifyingOption, "0");
    }

    virtual auto getTypeValue() const noexcept
        -> const boost::program_options::value_semantic* override {
        return boost::program_options::bool_switch();
    }
};

/**
 * \brief   Partial implementation an argument option that fits an option value
 * of type vector<T>
 */
template <typename T> class Option<std::vector<T>> : public OptionBase {
  public:
    /*! @copydoc OptionBase::OptionBase(const std::string&, const ArgumentOptions&, const std::string&)
     */
    Option(const std::string& identifyingOption,
           const ArgumentOptions& argumentOptions,
           const std::string& explanation)
        : OptionBase(identifyingOption, argumentOptions, explanation) {
        ;
    }

    virtual ~Option() {}

    virtual auto toMap(config::VariablesMap& variablesMap,
                       const boost::program_options::variables_map& optionsMap)
        const noexcept -> bool override {
        try {
            return variablesMap.replace(
                m_identifyingOption,
                optionsMap[m_identifyingOption].template as<std::vector<T>>());
        } catch(const boost::bad_any_cast& e) {
            user_feedback_error("Bad_any_cast exception caught: " << e.what());
            return false;
        }
    }

    virtual auto getTypeValue() const noexcept
        -> boost::program_options::value_semantic* override {
        return boost::program_options::value<std::vector<T>>()->multitoken();
    }
};

/**
 * \brief Represents the descriptions of the options. This description is used
 * for interpreting e.g. command line arguments.
 */
class OptionDescriptions {
  public:
    OptionDescriptions() noexcept;

    /**
     * Returns the currently registered option descriptions
     *
     * \returns A collection of the registered option descriptions
     */
    [[nodiscard]] auto getOptionDescriptions() const noexcept
        -> boost::program_options::options_description;

    /**
     * Add an option description
     *
     * \param[in] option        The option do add
     */
    template <typename T> void addOption(const Option<T>& option) noexcept {
        auto id = option.getId();

        m_optionKeys.emplace_back("--" + id);
        std::string option_code(id);
        for(const auto& argumentOption : option.getArgumentOptions()) {
            option_code.append(",");
            option_code.append(argumentOption);
            if(argumentOption.size() == 1U) {
                m_optionKeys.emplace_back("-" + argumentOption);
            } else {
                m_optionKeys.emplace_back("--" + argumentOption);
            }
        }

        m_optionDescription.add_options()(option_code.c_str(),
                                          option.getTypeValue(),
                                          option.getExplanation().c_str());
        m_options.emplace(std::pair<std::string, std::unique_ptr<Option<T>>>(
            id, std::make_unique<Option<T>>(option)));
    }

    /**
     * Set the given option as the positional argument (meaning that if no
     * associated option is specified for an argument on the command line, it is
     * assumed to belong to this specific option.
     *
     * \param[in] option  The option to use
     * \returns true    if the positional argument was successfully set
     *          false   otherwise
     */
    auto setPositionalArgument(const OptionInterface& option) noexcept -> bool;

    /**
     * Returns a map containing the parsed option descriptions for the given
     * command line arguments
     *
     * \param[out] variablesMap  The variables map to add the values to
     * \param[in] args  A collection of input arguments
     * \param[in] allowUnregistered Whether to allow options in args that are
     * not described in this option description \returns True    if the options
     * map was successfully constructed False   otherwise
     */
    auto getOptionsMap(config::VariablesMap& variablesMap, const Args& args,
                       bool allowUnregistered = false) const noexcept -> bool;

    /**
     * Returns all the option keys
     *
     * \returns A list of option keys
     */
    auto getOptionKeys() const noexcept -> const std::vector<std::string>& {
        return m_optionKeys;
    }

  private:
    void toMap(
        config::VariablesMap& variablesMap,
        const boost::program_options::variables_map& optionsMap) const noexcept;

    boost::program_options::options_description m_optionDescription;

    std::vector<std::string> m_optionKeys;
    std::map<std::string, std::unique_ptr<OptionBase>> m_options;
    std::optional<std::string> m_positional;
};
} // namespace execHelper::config

#endif /* __OPTION_DESCRIPTIONS_H__ */
