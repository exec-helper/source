#ifndef __OPTION_DESCRIPTIONS_H__
#define __OPTION_DESCRIPTIONS_H__

#include <memory>
#include <optional>
#include <span>
#include <string_view>
#include <type_traits>
#include <vector>

#include <boost/program_options.hpp>

#include "log/assertions.h"

#include "log/log.h"
#include "settingsNode.h"
#include "variablesMap.h"

namespace execHelper::config {
using ArgumentOption = std::string;
using ArgumentOptions = std::vector<ArgumentOption>;
using Args = std::span<const char* const>;

namespace detail {
/**
 * Converts the given value to the associated settings value
 *
 * \param[in] value     The value to convert
 * \returns The converted value
 */
template <typename T> inline auto toSetting([[maybe_unused]] T value) noexcept {
    static_assert(std::is_void_v<T>,
                  "T was not specialized for the correct type");
    return SettingsValue();
}

/*! \overload toSetting<T>(T)
 */
inline auto toSetting(bool value) noexcept {
    if(value) {
        return "1";
    }
    return "0";
}

/*! \overload toSetting<T>(T)
 */
inline auto toSetting(const std::string& value) noexcept { return value; }

/*! \overload toSetting<T>(T)
 */
inline auto toSetting(const std::vector<std::string>& value) noexcept {
    return value;
}

/**
 * \brief Construction for getting a command line argument that fits the expected type
 */
template <typename T> struct TypeValue {
    /**
     * Returns a program_options value that converts to the given type
     *
     * \returns A boost::program_options value that can contain and convert a command line argument to the given type
     */
    [[nodiscard]] static inline auto value() noexcept
        -> const boost::program_options::value_semantic* {
        return boost::program_options::value<T>();
    }
};

/**
 * \brief Construction for getting a command line argument that fits the expected type. Specialization for bool.
 */
template <> struct TypeValue<bool> {
    /*! @copydoc TypeValue<T>::value()
     */
    [[nodiscard]] static inline auto value() noexcept
        -> const boost::program_options::value_semantic* {
        return boost::program_options::bool_switch();
    }
};

/**
 * \brief Construction for getting a command line argument that fits the expected type. Specialization for std::vector<T>.
 */
template <typename T> struct TypeValue<std::vector<T>> {
    /*! @copydoc TypeValue<T>()::value()
     */
    [[nodiscard]] static inline auto value() noexcept
        -> boost::program_options::value_semantic* {
        return boost::program_options::value<std::vector<T>>()->multitoken();
    }
};
} // namespace detail

/**
 * \brief Interface for determining the option characteristics associated with
 * an argument option
 */
class OptionInterface {
  public:
    virtual ~OptionInterface() = default;

    /**
     * Returns the id for identifying the specified option throughout the system
     *
     * \returns The unique id associated with this option
     */
    [[nodiscard]] virtual auto getId() const noexcept -> std::string = 0;

    /**
     * Returns the argument options for this option, in addition to the one
     * returned by getId().
     *
     * \returns A collection of additional option keys associated with this
     * option
     */
    [[nodiscard]] virtual auto getArgumentOptions() const noexcept
        -> const ArgumentOptions& = 0;

    /**
     * Returns the explanation associated with this option
     *
     * \returns The explanation
     */
    [[nodiscard]] virtual auto getExplanation() const noexcept
        -> const std::string& = 0;

    /**
     * Extract the value(s) associated with this option from the optionsMap
     *
     * \param[out] variablesMap The variables map to write to
     * \param[in] optionsMap    The options map to take the value(s) from
     * \throws boost::bad_any_cast  If the given option value can not be parsed to the given option type
     */
    virtual void
    toMap(config::VariablesMap& variablesMap,
          const boost::program_options::variables_map& optionsMap) const = 0;

    /**
     * Return the associated type value of this options in order to be able to
     * properly parse it
     *
     * \returns The implementation-specific semantic to be used by the
     * optionparser.
     */
    [[nodiscard]] virtual auto getTypeValue() const noexcept
        -> const boost::program_options::value_semantic* = 0;

  protected:
    OptionInterface() = default;

    /**
     * Copy constructor
     *
     * \param[in] other The object to copy from
     */
    OptionInterface(const OptionInterface& other) = default;

    /**
     * Move constructor
     *
     * \param[in] other The object to move from
     */
    OptionInterface(OptionInterface&& other) = default;

    /**
     * Copy assignment operator
     *
     * \param[in] other The object to assign from
     * \returns A reference to this object
     */
    auto operator=(const OptionInterface& other) -> OptionInterface& = default;

    /**
     * move assignment operator
     *
     * \param[in] other the object to move assign from
     * \returns a reference to this object
     */
    auto operator=(OptionInterface&& other) -> OptionInterface& = default;
};

/**
 * \brief   Class for generic OptionInterface parent functions
 */
template <typename T> class Option : public OptionInterface {
  public:
    /**
     * \param[in] identifyingOption The unique string to identify this option
     * with \param[in] argumentOptions   A collection of additional,
     * non-identifying options \param[in] explanation       The explanation
     * associated with this option
     */
    Option(std::string identifyingOption, ArgumentOptions argumentOptions,
           std::string explanation)
        : m_identifyingOption(std::move(identifyingOption)),
          m_argumentOptions(std::move(argumentOptions)),
          m_explanation(std::move(explanation)) {
        expectsMessage(argumentOptions.size() <= 1U,
                       "Currently we support only one additional, "
                       "non-identifying option: the associated short option");
    }

    /**
     * Copy constructor
     *
     * \param[in] other The object to copy from
     */
    Option(const Option& other) = default;

    /**
     * Move constructor
     *
     * \param[in] other The object to move from
     */
    Option(Option&& other) noexcept = default;

    ~Option() override = default;

    /**
     * Copy assignment operator
     *
     * \param[in] other The object to assign from
     * \returns A reference to this object
     */
    auto operator=(const Option& other) -> Option& = default;

    /**
     * move assignment operator
     *
     * \param[in] other the object to move assign from
     * \returns a reference to this object
     */
    auto operator=(Option&& other) noexcept -> Option& = default;

    [[nodiscard]] auto getId() const noexcept -> std::string override {
        return m_identifyingOption;
    }

    [[nodiscard]] auto getArgumentOptions() const noexcept
        -> const ArgumentOptions& override {
        return m_argumentOptions;
    }

    [[nodiscard]] auto getExplanation() const noexcept
        -> const std::string& override {
        return m_explanation;
    }

    auto toMap(config::VariablesMap& variablesMap,
               const boost::program_options::variables_map& optionsMap) const
        -> void override {
        auto value = optionsMap[m_identifyingOption].template as<T>();
        auto settings = detail::toSetting(value);
        if(!variablesMap.replace(m_identifyingOption, settings)) {
            throw(std::invalid_argument(
                std::string("Could not set ").append(m_identifyingOption)));
        }
    }

    [[nodiscard]] auto getTypeValue() const noexcept
        -> const boost::program_options::value_semantic* override {
        return detail::TypeValue<T>::value();
    }

  private:
    std::string m_identifyingOption; //!< The identifying option associated with
                                     //!< this option
    ArgumentOptions m_argumentOptions; //!< Additional non-identifying options
                                       //!< associated with this option
    std::string m_explanation;         //!< The associated explanation
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
     */
    void setPositionalArgument(const OptionInterface& option) noexcept;

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
    [[nodiscard]] auto
    getOptionsMap(config::VariablesMap& variablesMap, const Args& args,
                  bool allowUnregistered = false) const noexcept -> bool;

    /**
     * Returns all the option keys
     *
     * \returns A list of option keys
     */
    [[nodiscard]] auto getOptionKeys() const noexcept
        -> const std::vector<std::string>& {
        return m_optionKeys;
    }

  private:
    /**
     * Maps option values to settings
     *
     * \param[out] variablesMap     The variables map to add the settings to
     * \param[in] optionsMap        The options mapping
     * \throws std::invalid_argument    If an option value can not be mapped to the corresponding setting
     */
    void toMap(config::VariablesMap& variablesMap,
               const boost::program_options::variables_map& optionsMap) const;

    boost::program_options::options_description m_optionDescription;

    std::vector<std::string> m_optionKeys;
    std::map<std::string, std::unique_ptr<OptionInterface>> m_options;
    std::optional<std::string> m_positional;
};
} // namespace execHelper::config

#endif /* __OPTION_DESCRIPTIONS_H__ */
