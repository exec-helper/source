#ifndef __EXECUTOR_INTERFACE_H__
#define __EXECUTOR_INTERFACE_H__

namespace execHelper {
    namespace core {
        class Task;
    }
}

namespace execHelper { namespace core {
    class ExecutorInterface {
        public:
            virtual bool execute(const Task& task) noexcept = 0;

        protected:
            ExecutorInterface() {};
    };
} }

#endif /* __EXECUTOR_INTERFACE_H__ */
