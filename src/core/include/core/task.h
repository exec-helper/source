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

        class Task {
            public:
                const TaskCollection& getTask() const noexcept;
                std::string toString() const;

                const EnvironmentCollection& getEnvironment() const noexcept;
                
                Task(EnvironmentCollection env = EnvironmentCollection()) noexcept;
                Task(const std::initializer_list<std::string>& subtasks, EnvironmentCollection env = EnvironmentCollection()) noexcept;

                bool append(const std::string& taskPart) noexcept;
                bool append(std::string&& taskPart) noexcept;
                bool append(const TaskCollection& taskPart) noexcept;
                bool append(TaskCollection&& taskPart) noexcept;

                bool setEnvironment(const EnvironmentCollection& env) noexcept;
                bool appendToEnvironment(EnvironmentValue&& newValue) noexcept;
                bool appendToEnvironment(EnvironmentCollection&& newValues) noexcept;

                bool operator==(const Task& other) const noexcept;
                bool operator!=(const Task& other) const noexcept;

            private:
                TaskCollection m_task;
                EnvironmentCollection m_env;
        };
        
        std::ostream& operator<<( std::ostream& os, const Task& task ) noexcept;
    }
}
#endif /* __TASK_H__ */
