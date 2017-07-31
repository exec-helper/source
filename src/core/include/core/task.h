#ifndef __TASK_H__
#define __TASK_H__

#include <map>
#include <string>
#include <vector>

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
                 * Create a task
                 *
                 * \param env   The environment to apply to the task
                 */
                Task(EnvironmentCollection env = EnvironmentCollection()) noexcept;

                /**
                 * \copybrief Task(EnvironmentCollection)
                 *
                 * \param subtasks  The task subdivided in separate arguments
                 * \copydetails Task(EnvironmentCollection)
                 */
                Task(const std::initializer_list<std::string>& subtasks, EnvironmentCollection env = EnvironmentCollection()) noexcept;

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

                /**
                 * Append an additional value to the environment of the task
                 *
                 * \param newValue The new value to add to the task
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
