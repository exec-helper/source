#ifndef __TASK_H__
#define __TASK_H__

#include <vector>
#include <string>

namespace execHelper {
    namespace core {
        typedef std::vector<std::string> TaskCollection;

        class Task {
            public:
                const TaskCollection& getTask() noexcept;
                std::string toString();

                bool append(const std::string& taskPart) noexcept;
                bool append(const std::string&& taskPart) noexcept;
                bool append(const TaskCollection& taskPart) noexcept;
                bool append(const TaskCollection&& taskPart) noexcept;

            private:
                TaskCollection m_task;
        };
    }
}
#endif /* __TASK_H__ */
