#ifndef __TASK_H__
#define __TASK_H__

#include <vector>
#include <string>

namespace execHelper {
    namespace core {
        typedef std::vector<std::string> TaskCollection;

        class Task {
            public:
                const TaskCollection& getTask() const noexcept;
                std::string toString() const;
                
                Task();
                Task(const std::initializer_list<std::string>& subtasks);

                bool append(const std::string& taskPart) noexcept;
                bool append(const std::string&& taskPart) noexcept;
                bool append(const TaskCollection& taskPart) noexcept;
                bool append(const TaskCollection&& taskPart) noexcept;

                bool operator==(const Task& other) const noexcept;
                bool operator!=(const Task& other) const noexcept;

                //std::ostream& operator<<(std::ostream& os) const noexcept;

            private:
                TaskCollection m_task;
        };
        
        std::ostream& operator<<( std::ostream& os, const Task& task ) noexcept;
    }
}
#endif /* __TASK_H__ */
