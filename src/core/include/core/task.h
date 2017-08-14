#ifndef __TASK_H__
#define __TASK_H__

#include <map>
#include <string>
#include <vector>

#include <boost/filesystem.hpp>

#include "config/path.h"

namespace execHelper {
    namespace core {
        typedef std::vector<std::string> TaskCollection;
        typedef std::map<std::string, std::string> EnvironmentCollection;
        typedef std::pair<std::string, std::string> EnvironmentValue;

        /**
         * \brief Represents a task to execute
         */
        class Task {
            public:
                /**
                 * Create a task
                 *
                 * \param subtasks  The task subdivided in separate arguments
                 * \param workingDirectory  The working directory for the task
                 */
                Task(const std::initializer_list<std::string>& subtasks = {}, const config::Path& workingDirectory = boost::filesystem::current_path()) noexcept;

                /**
                 * Returns the task
                 *
                 * \returns An ordered collection of all the arguments that are contained in the task
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
                const EnvironmentCollection& getEnvironment() const noexcept;

                /**
                 * Sets the working directory of the task
                 *
                 * \param workingDirectory  The new working directory to set
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
                 * \param taskPart     The part to add to the task
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
                 * Set the environment of the task
                 *
                 * \param env  The environment to set for the task
                 * \returns True    If the new environment was successfully set
                 *          False   Otherwise
                 */
                bool setEnvironment(const EnvironmentCollection& env) noexcept;

                /*! @copydoc(const EnvirontmentCollection&)
                 */
                bool setEnvironment(EnvironmentCollection&& env) noexcept;

                /**
                 * Add or replace an additional value to the environment of the task.
                 * If the new key already exists in the environment, the existing value for the key is overwritten.
                 *
                 * \param newValue The new value to add or replace to the task
                 * \returns True    If the new value was successfully appended to the task
                 *          False   Otherwise
                 */
                bool appendToEnvironment(EnvironmentValue&& newValue) noexcept;

                /*! @copydoc appendToEnvironment(EnvironmentValue&&)
                 */
                bool appendToEnvironment(EnvironmentCollection&& newValue) noexcept;

                /**
                 * Checks whether other instance equals this instance of the object
                 *
                 * \param other The other instance to compare with
                 * \returns True    If the other instance is equal to this instance of the object
                 *          False   Otherwise
                 */
                bool operator==(const Task& other) const noexcept;

                /**
                 * Checks whether other instance does not equal this instance of the object
                 *
                 * \param other The other instance to compare with
                 * \returns ! \ref operator==(const Task&) const
                 */
                bool operator!=(const Task& other) const noexcept;

            private:
                TaskCollection m_task;
                EnvironmentCollection m_env;
                config::Path m_workingDirectory;
        };
        
        /**
         * Adds the details of the Task object to the given stream
         *
         * \param os    The stream to add the details to
         * \param task  The task to add the details from
         * \returns The given stream expanded with the details of the given task
         */
        std::ostream& operator<<( std::ostream& os, const Task& task ) noexcept;
    }
}
#endif /* __TASK_H__ */
