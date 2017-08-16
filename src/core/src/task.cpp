#include "task.h"

#include <algorithm>
#include <iostream>
#include <numeric>
#include <ostream>
#include <utility>

using std::accumulate;
using std::back_inserter;
using std::endl;
using std::initializer_list;
using std::move;
using std::ostream;
using std::string;
using std::vector;

using execHelper::config::Path;

namespace {
    inline string implodeVector(const vector<string>& toImplode, const string& delimiter = string(" ")) {
        string result;
        return accumulate(toImplode.begin(), toImplode.end(), string(),
                [&delimiter](std::string& a, const std::string& b) {
					if(a.empty()) {
                        return string(b);
					}
                    return string(a + delimiter + b);
				});
    }
}

namespace execHelper { namespace core {

    Task::Task(const initializer_list<string>& subtasks, Path workingDirectory) noexcept :
        m_task(subtasks),
        m_workingDirectory(move(workingDirectory))
    {
        ;
    }

    const execHelper::core::TaskCollection& Task::getTask() const noexcept {
        return m_task;
    }

    string Task::toString() const {
        return implodeVector(m_task);
    }

    const EnvironmentCollection& Task::getEnvironment() const noexcept {
        return m_env;
    }

    void Task::setWorkingDirectory(const Path& workingDirectory) noexcept {
        m_workingDirectory = workingDirectory;
    }

    const Path& Task::getWorkingDirectory() const noexcept {
        return m_workingDirectory;
    }


    bool Task::append(const string& taskPart) noexcept {
        m_task.push_back(taskPart);
        return true;
    }

    bool Task::append(string&& taskPart) noexcept {
        m_task.push_back(move(taskPart));
        return true;
    }

    bool Task::append(const TaskCollection& taskPart) noexcept {
        m_task.reserve(m_task.size() + taskPart.size());
        m_task.insert(std::end(m_task), std::begin(taskPart), std::end(taskPart)); 
        return true;
    }

    bool Task::append(TaskCollection&& taskPart) noexcept {
        m_task.reserve(m_task.size() + taskPart.size());
        move(taskPart.begin(), taskPart.end(), back_inserter(m_task));
        return true;
    }

    bool Task::setEnvironment(const EnvironmentCollection& env) noexcept {
        m_env = env;
        return true;
    }

    bool Task::setEnvironment(EnvironmentCollection&& env) noexcept {
        m_env = move(env);
        return true;
    }

    bool Task::appendToEnvironment(EnvironmentValue&& newValue) noexcept {
        if(m_env.count(newValue.first) > 0U) {
            m_env.erase(newValue.first);
        }
        m_env.emplace(newValue);
        return true;
    }

    bool Task::appendToEnvironment(EnvironmentCollection&& newValue) noexcept {
        for(auto&& value : newValue) {
            appendToEnvironment(value);
        }
        return true;
    }

    bool Task::operator==(const Task& other) const noexcept {
        return (m_task == other.m_task && m_workingDirectory == other.m_workingDirectory && m_env == other.m_env);
    }

    bool Task::operator!=(const Task& other) const noexcept {
        return !(*this == other);
    }

    ostream& operator<<(ostream& os, const Task& task) noexcept {
        TaskCollection subtasks = task.getTask();
        EnvironmentCollection environment = task.getEnvironment();
        os << string("Task {");

        os << string("Environment(") << environment.size() << "): {";
        for(const auto& envVar : environment) {
            os << string(" ") << envVar.first << ": " << envVar.second << ";";
        }
        os << string("} ");

        os << string("Command(") << subtasks.size() << "): {";
        for(const auto& subTask : subtasks) {
            os << string(" ") << subTask;
        }
        os << string("} ");
        os << string("Working-dir: {") << task.getWorkingDirectory().native() << "}";
        os << string("}");
        os << endl;
        return os;
    }
} }
