#include "task.h"

#include <algorithm>
#include <filesystem>
#include <iostream>
#include <numeric>
#include <ostream>
#include <utility>

#include "logger.h"

using std::accumulate;
using std::back_inserter;
using std::endl;
using std::make_pair;
using std::move;
using std::ostream;
using std::string;
using std::vector;

using execHelper::config::EnvironmentCollection;
using execHelper::config::EnvironmentValue;
using execHelper::config::Path;

namespace filesystem = std::filesystem;

namespace {
inline auto implodeVector(const vector<string>& toImplode,
                          const string& delimiter) -> string {
    string result;
    return accumulate(
        toImplode.begin(), toImplode.end(), string(),
        [&delimiter](const std::string& lhs, const std::string& rhs) {
            if(lhs.empty()) {
                return string(rhs);
            }
            return string(lhs).append(delimiter).append(rhs);
        });
}

inline auto implodeVector(const vector<string>& toImplode) -> string {
    return implodeVector(toImplode, " ");
}
} // namespace

namespace execHelper::core {
// cppcheck-suppress passedByValue symbolName=subtasks
Task::Task(std::vector<std::string> subtasks,
           // cppcheck-suppress passedByValue symbolName=environment
           config::EnvironmentCollection environment,
           config::Path workingDirectory) noexcept
    : m_task(std::move(subtasks)),
      m_env(std::move(environment)),
      m_workingDirectory(std::move(workingDirectory)) {
    setWorkingDirectory(m_workingDirectory);
}

auto Task::getTask() const noexcept -> const execHelper::core::TaskCollection& {
    return m_task;
}

auto Task::toString() const -> string { return implodeVector(m_task); }

auto Task::getEnvironment() const noexcept -> const EnvironmentCollection& {
    return m_env;
}

void Task::setWorkingDirectory(const Path& workingDirectory) noexcept {
    LOG(trace) << "Changing working directory of task to " << workingDirectory;
    m_workingDirectory = workingDirectory;

    // Set the PWD environment variable to the working directory
    appendToEnvironment(
        make_pair("PWD", filesystem::absolute(m_workingDirectory).string()));
}

auto Task::getWorkingDirectory() const noexcept -> const Path& {
    return m_workingDirectory;
}

auto Task::append(const string& taskPart) noexcept -> bool {
    m_task.push_back(taskPart);
    return true;
}

auto Task::append(string&& taskPart) noexcept -> bool {
    m_task.push_back(move(taskPart));
    return true;
}

auto Task::append(const TaskCollection& taskPart) noexcept -> bool {
    m_task.reserve(m_task.size() + taskPart.size());
    m_task.insert(std::end(m_task), std::begin(taskPart), std::end(taskPart));
    return true;
}

auto Task::append(TaskCollection&& taskPart) noexcept -> bool {
    m_task.reserve(m_task.size() + taskPart.size());
    move(taskPart.begin(), taskPart.end(), back_inserter(m_task));
    return true;
}

auto Task::setEnvironment(const EnvironmentCollection& env) noexcept -> bool {
    m_env = env;
    setWorkingDirectory(m_workingDirectory);
    return true;
}

auto Task::setEnvironment(EnvironmentCollection&& env) noexcept -> bool {
    m_env = move(env);
    setWorkingDirectory(m_workingDirectory);
    return true;
}

auto Task::appendToEnvironment(EnvironmentValue&& newValue) noexcept -> bool {
    if(m_env.contains(newValue.first)) {
        m_env.erase(newValue.first);
    }
    m_env.emplace(newValue);
    return true;
}

auto Task::appendToEnvironment(EnvironmentCollection&& newValue) noexcept
    -> bool {
    for(auto&& value : newValue) {
        appendToEnvironment(value);
    }
    return true;
}

auto Task::addPatterns(const config::Patterns& patterns) noexcept -> void {
    for(const auto& pattern : patterns) {
        if(count(m_patterns.begin(), m_patterns.end(), pattern) == 0) {
            m_patterns.emplace_back(pattern);
        }
    }
}

auto Task::getPatterns() const noexcept -> const config::Patterns& {
    return m_patterns;
}

auto Task::operator==(const Task& other) const noexcept -> bool {
    return (m_task == other.m_task &&
            m_workingDirectory == other.m_workingDirectory &&
            m_env == other.m_env);
}

auto Task::operator!=(const Task& other) const noexcept -> bool {
    return !(*this == other);
}

auto operator<<(ostream& stream, const Task& task) noexcept -> ostream& {
    TaskCollection subtasks = task.getTask();
    EnvironmentCollection environment = task.getEnvironment();
    stream << string("Task {");

    stream << string("Environment(") << environment.size() << "): {";
    for(const auto& envVar : environment) {
        stream << string(" ") << envVar.first << ": " << envVar.second << ";";
    }
    stream << string("} ");

    stream << string("Command(") << subtasks.size() << "): {";
    for(const auto& subTask : subtasks) {
        stream << string(" ") << subTask;
    }
    stream << string("} ");
    stream << string("Working-dir: {") << task.getWorkingDirectory().string()
           << "}";
    stream << string("}");
    stream << endl;
    return stream;
}
} // namespace execHelper::core
