#ifndef __TASK_H__
#define __TASK_H__

#include <filesystem>
#include <map>
#include <string>
#include <vector>

#include "config/environment.h"
#include "config/path.h"
#include "config/pattern.h"

namespace execHelper::core {
using TaskCollection = std::vector<std::string>;

/**
 * \brief Represents a task to execute
 */
class Task {
  public:
    /**
     * Create a task
     *
     */
    Task() noexcept : Task(std::vector<std::string>()) {}

    /**
     * Create a task
     *
     * \param[in] subtasks  The task subdivided in separate arguments
     */
    explicit Task(const std::initializer_list<std::string>& subtasks) noexcept
        : Task(std::vector<std::string>(subtasks)) {}

    /**
     * Create a task
     *
     * \param[in] subtasks  The task subdivided in separate arguments
     */
    explicit Task(const std::vector<std::string>& subtasks) noexcept
        : Task(subtasks, {}, std::filesystem::current_path()) {}

    /**
     * Create a task
     *
     * \param[in] subtasks  The dask subdivided in separate arguments
     * \param[in] environment   The initial environment to use for the task
     * \param[in] workingDirectory  The working directory from which to execute the task
     */
    Task(std::vector<std::string> subtasks,
         config::EnvironmentCollection environment,
         config::Path workingDirectory) noexcept;

    /**
     * Returns the task
     *
     * \returns An ordered collection of all the arguments that are contained in
     * the task
     */
    const TaskCollection& getTask() const noexcept;

    /**
     * Returns the task as a string
     *
     * \returns The task as a string
     */
    std::string toString() const;

    /**
     * Returns the environment in which to execute the task
     *
     * \returns A collection of the environment
     */
    const config::EnvironmentCollection& getEnvironment() const noexcept;

    /**
     * Sets the working directory of the task
     *
     * \param[in] workingDirectory  The new working directory to set
     */
    void setWorkingDirectory(const config::Path& workingDirectory) noexcept;

    /**
     * Gets the working directory of the task
     *
     * \returns The working directory associated with this task
     */
    const config::Path& getWorkingDirectory() const noexcept;

    /**
     * Append to this task
     *
     * \param[in] taskPart     The part to add to the task
     * \returns True    If the part was successfully added to the task
     *          False   Otherwise
     */
    bool append(const std::string& taskPart) noexcept;

    /*! @copydoc append(const std::string&)
     */
    bool append(std::string&& taskPart) noexcept;

    /*! @copydoc append(const std::string&)
     */
    bool append(const TaskCollection& taskPart) noexcept;

    /*! @copydoc append(const std::string&)
     */
    bool append(TaskCollection&& taskPart) noexcept;

    /**
     * Set the environment of the task. Replaces the existing environment for the task.
     *
     * \param[in] env  The environment to set for the task
     * \returns True    If the new environment was successfully set
     *          False   Otherwise
     */
    bool setEnvironment(const config::EnvironmentCollection& env) noexcept;

    /*! @copydoc setEnvironment(const config::EnvironmentCollection&)
     */
    bool setEnvironment(config::EnvironmentCollection&& env) noexcept;

    /**
     * Add or replace an additional value to the environment of the task.
     * If the new key already exists in the environment, the existing value for
     * the key is overwritten.
     *
     * \param[in] newValue The new value to add or replace to the task
     * \returns True    If the new value was successfully appended to the task
     *          False   Otherwise
     */
    bool appendToEnvironment(config::EnvironmentValue&& newValue) noexcept;

    /*! @copydoc appendToEnvironment(config::EnvironmentValue&&)
     */
    bool appendToEnvironment(config::EnvironmentCollection&& newValue) noexcept;

    /**
     * Checks whether other instance equals this instance of the object
     *
     * \param[in] other The other instance to compare with
     * \returns True    If the other instance is equal to this instance of the
     * object False   Otherwise
     */
    bool operator==(const Task& other) const noexcept;

    /**
     * Checks whether other instance does not equal this instance of the object
     *
     * \param[in] other The other instance to compare with
     * \returns ! \ref operator==(const Task&) const
     */
    bool operator!=(const Task& other) const noexcept;

    /**
     * Add relevant patterns to the context of this task
     *
     * \param[in] patterns  The patterns to add to this task.
     */
    void addPatterns(const config::Patterns& patterns) noexcept;

    /**
     * Get the patterns for this tasks context
     *
     * \returns The patterns registered for this tasks context
     */
    [[nodiscard]] auto getPatterns() const noexcept -> const config::Patterns&;

  private:
    TaskCollection m_task;
    config::EnvironmentCollection m_env;
    config::Path m_workingDirectory;
    config::Patterns m_patterns = {};
};
using Tasks = std::vector<Task>;

/**
 * Adds the details of the Task object to the given stream
 *
 * \param[in] stream    The stream to add the details to
 * \param[in] task  The task to add the details from
 * \returns The given stream expanded with the details of the given task
 */
auto operator<<(std::ostream& stream, const Task& task) noexcept
    -> std::ostream&;
} // namespace execHelper::core
#endif /* __TASK_H__ */
