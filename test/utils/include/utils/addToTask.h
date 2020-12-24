#ifndef ADD_TO_TASK_INCLUDE
#define ADD_TO_TASK_INCLUDE

#include <filesystem>
#include <functional>
#include <initializer_list>
#include <map>
#include <optional>
#include <string>
#include <string_view>
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

inline void addToTask(const std::string_view& value, gsl::not_null<core::Task*> task,
                      AddToTaskFunction func) {
    task->append(func(std::string(value)));
}

inline void addToTask(bool value, gsl::not_null<core::Task*> task,
                      AddToTaskFunction func) {
    if(value) {
        task->append(func("true"));
    } else {
        task->append(func("false"));
    }
}

inline void addToTask(uint16_t value, gsl::not_null<core::Task*> task,
                      AddToTaskFunction func) {
    addToTask(std::to_string(value), task, std::move(func));
}

inline void addToTask(uint32_t value, gsl::not_null<core::Task*> task,
                      AddToTaskFunction func) {
    addToTask(std::to_string(value), task, std::move(func));
}

inline void addToTask(const std::pair<std::string, std::string>& value,
                      gsl::not_null<core::Task*> task, AddToTaskFunction func) {
    task->append(func(value.first + "=" + value.second));
}

inline void addToTask(const std::filesystem::path& value,
                        gsl::not_null<core::Task*> task, AddToTaskFunction func = [](const std::filesystem::path& value) -> execHelper::core::TaskCollection { return { value.string() }; } ) {
    task->append(func(value));
}

template<typename T>
inline void addToTask(const std::vector<T>& value,
                      gsl::not_null<core::Task*> task, AddToTaskFunction func) {
    std::for_each(
        value.begin(), value.end(),
        [&task, func](const auto& element) {
            addToTask(element, task, func);
        });
}

template<typename K, typename V>
inline void addToTask(const std::map<K, V>& value,
                      gsl::not_null<core::Task*> task, std::function<core::TaskCollection(const K&, const V&)> func) {
    core::TaskCollection collection;
    collection.reserve(value.size());

    std::for_each(value.begin(), value.end(), [&task, func](const auto& element) {
        task->append(func(element.first, element.second));
    });
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
