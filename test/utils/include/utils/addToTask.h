#ifndef ADD_TO_TASK_INCLUDE
#define ADD_TO_TASK_INCLUDE

#include <functional>
#include <initializer_list>
#include <optional>
#include <string>
#include <utility>
#include <vector>

#include <gsl/pointers>

#include "base-utils/nonEmptyString.h"
#include "core/task.h"

namespace execHelper::test {
using AddToTaskFunction = std::function<core::TaskCollection(std::string)>;

inline void addToTask(const std::string& value, gsl::not_null<core::Task*> task,
                      AddToTaskFunction func) {
    task->append(func(value));
}

inline void addToTask(const NonEmptyString& value,
                      gsl::not_null<core::Task*> task, AddToTaskFunction func) {
    task->append(func(*value));
}

inline void addToTask(bool value, gsl::not_null<core::Task*> task,
                      AddToTaskFunction func) {
    if(value) {
        task->append(func("true"));
    } else {
        task->append(func("false"));
    }
}

inline void addToTask(const std::vector<std::string>& value,
                      gsl::not_null<core::Task*> task, AddToTaskFunction func) {
    std::for_each(
        value.begin(), value.end(),
        [&task, func](const auto& element) { task->append(func(element)); });
}

inline void addToTask(const std::pair<std::string, std::string>& value,
                      gsl::not_null<core::Task*> task, AddToTaskFunction func) {
    task->append(func(value.first + "=" + value.second));
}

template <typename T>
inline void addToTask(const std::optional<T>& value,
                      gsl::not_null<core::Task*> task, AddToTaskFunction func) {
    if(value) {
        addToTask(*value, task, func);
    }
}

template <typename T>
inline void addToTask(const std::optional<T>& value,
                      gsl::not_null<core::Task*> task, AddToTaskFunction func,
                      const T& defaultValue) {
    if(value) {
        addToTask(*value, task, func);
    } else {
        addToTask(defaultValue, task, func);
    }
}
} // namespace execHelper::test

#endif /* ADD_TO_TASK_INCLUDE */
