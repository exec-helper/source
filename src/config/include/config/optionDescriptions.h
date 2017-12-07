#ifndef __OPTION_DESCRIPTIONS_H__
#define __OPTION_DESCRIPTIONS_H__

#include <memory>
#include <vector>

#include <boost/optional/optional.hpp>
#include <boost/program_options.hpp>

#include "log/assertions.h"

#include "logger.h"
#include "variablesMap.h"

namespace execHelper {
namespace config {
class Argv;
} // namespace config
} // namespace execHelper

namespace execHelper {
namespace config {
typedef std::string ArgumentOption;
typedef std::vector<ArgumentOption> ArgumentOptions;

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
    virtual std::string getId() const noexcept = 0;

    /**
     * Extract the value(s) associated with this option from the optionsMap
     *
     * \param[out] variablesMap The variables map to write to
     * \param[in] optionsMap    The options map to take the value(s) from
     * \returns True    if the associated value(s) were successfully extracted
     *          False   otherwise
     */
    virtual bool
    toMap(config::VariablesMap& variablesMap,
          const boost::program_options::variables_map& optionsMap) const
        noexcept = 0;

    /**
     * Return the associated type value of this options in order to be able to
     * properly parse it
     *
     * \returns The implementation-specific semantic to be used by the
     * optionparser.
     */
    virtual const boost::program_options::value_semantic* getTypeValue() const
        noexcept = 0;

  protected:
    OptionInterface() {}
};

/**
 * \brief   Class for generic OptionInterface parent functions
 */
class OptionBase : public OptionInterface {
  public:
    virtual ~OptionBase() {}

    std::string getId() const noexcept override { return m_identifyingOption; }

    /**
     * Returns the argument options for this option, in addition to the one
     * returned by getId().
     *
     * \returns A collection of additional option keys associated with this
     * option
     */
    virtual const ArgumentOptions& getArgumentOptions() const noexcept {
        return m_argumentOptions;
    }

    /**
     * Returns the explanation associated with this option
     *
     * \returns The explanation
     */
    virtual const std::string& getExplanation() const noexcept {
        return m_explanation;
    }

    bool
    toMap(config::VariablesMap& /*variablesMap*/,
          const boost::program_options::variables_map& /*optionsMap*/) const
        noexcept override {
        ensuresMessage(false, "We should not get here: all children should "
                              "implement this function");
        return false;
    }

    const boost::program_options::value_semantic* getTypeValue() const
        noexcept override {
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

    virtual bool
    toMap(config::VariablesMap& variablesMap,
          const boost::program_options::variables_map& optionsMap) const
        noexcept override {
        try {
            return variablesMap.replace(
                m_identifyingOption,
                optionsMap[m_identifyingOption].template as<T>());
        } catch(const boost::bad_any_cast& e) {
            LOG(warning) << "Bad_any_cast exception caught: " << e.what();
            return false;
        }
    }

    virtual const boost::program_options::value_semantic* getTypeValue() const
        noexcept override {
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

    virtual bool
    toMap(config::VariablesMap& variablesMap,
          const boost::program_options::variables_map& optionsMap) const
        noexcept override {
        if(optionsMap.count(m_identifyingOption) > 0) {
            try {
                if(optionsMap[m_identifyingOption].as<bool>()) {
                    return variablesMap.replace(m_identifyingOption, "1");
                }
            } catch(const boost::bad_any_cast& e) {
                LOG(warning) << "Bad_any_cast exception caught: " << e.what();
                return false;
            }
        }
        return variablesMap.replace(m_identifyingOption, "0");
    }

    virtual const boost::program_options::value_semantic* getTypeValue() const
        noexcept override {
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

    virtual bool
    toMap(config::VariablesMap& variablesMap,
          const boost::program_options::variables_map& optionsMap) const
        noexcept override {
        try {
            return variablesMap.replace(
                m_identifyingOption,
                optionsMap[m_identifyingOption].template as<std::vector<T>>());
        } catch(const boost::bad_any_cast& e) {
            LOG(warning) << "Bad_any_cast exception caught: " << e.what();
            return false;
        }
    }

    virtual const boost::program_options::value_semantic* getTypeValue() const
        noexcept override {
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
    boost::program_options::options_description getOptionDescriptions() const
        noexcept;

    /**
     * Add an option description
     *
     * \param[in] option        The option do add
     */
    template <typename T> void addOption(const Option<T>& option) noexcept {
        auto id = option.getId();
        std::string option_code(id);
        for(const auto& argumentOption : option.getArgumentOptions()) {
            option_code.append(",");
            option_code.append(argumentOption);
        }

        m_descriptions(option_code.c_str(), option.getTypeValue(),
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
    bool setPositionalArgument(const OptionInterface& option) noexcept;

    /**
     * Returns a map containing the parsed option descriptions for the given
     * command line arguments
     *
     * \param[out] variablesMap  The variables map to add the values to
     * \param[in] argv  A collection of input arguments
     * \param[in] allowUnregistered Whether to allow options in argv that are
     * not described in this option description \returns True    if the options
     * map was successfully constructed False   otherwise
     */
    bool getOptionsMap(config::VariablesMap& variablesMap, const Argv& argv,
                       bool allowUnregistered = false) const noexcept;

  private:
    void toMap(config::VariablesMap& variablesMap,
               const boost::program_options::variables_map& optionsMap) const
        noexcept;

    boost::program_options::options_description m_optionDescription;
    boost::program_options::options_description_easy_init m_descriptions;

    std::map<std::string, std::unique_ptr<OptionBase>> m_options;
    boost::optional<std::string> m_positional;
};
} // namespace config
} // namespace execHelper

#endif /* __OPTION_DESCRIPTIONS_H__ */
