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

                bool append(const std::string& taskPart) noexcept;
                bool append(const std::string&& taskPart) noexcept;
                bool append(const TaskCollection& taskPart) noexcept;
                bool append(const TaskCollection&& taskPart) noexcept;

                bool operator==(const Task& other) const noexcept;
                bool operator!=(const Task& other) const noexcept;

            private:
                TaskCollection m_task;
        };
    }
}
#endif /* __TASK_H__ */
