#include "task.h"

#include <utility>
#include <numeric>
#include <algorithm>
#include <ostream>

using std::string;
using std::vector;
using std::accumulate;
using std::move;
using std::back_inserter;
using std::initializer_list;
using std::ostream;

namespace {
    inline string implodeVector(const vector<string>& toImplode, string delimiter = string(" ")) {
        string result;
        return accumulate(toImplode.begin(), toImplode.end(), string(),
                [&delimiter](std::string& a, const std::string& b) {
					if(a.empty()) {
                        return string(b);
					} else {
                        return string(a + delimiter + b);
                    }
				});
    }
}

namespace execHelper { namespace core {

    Task::Task() {
        ;
    }

    Task::Task(const initializer_list<string>& subtasks) :
        m_task(subtasks)
    {
        ;
    }

    const execHelper::core::TaskCollection& Task::getTask() const noexcept {
        return m_task;
    }

    string Task::toString() const {
        return implodeVector(m_task);
    }

    bool Task::append(const string& taskPart) noexcept {
        m_task.push_back(taskPart);
        return true;
    }

    bool Task::append(const string&& taskPart) noexcept {
        m_task.push_back(move(taskPart));
        return true;
    }

    bool Task::append(const TaskCollection& taskPart) noexcept {
        m_task.reserve(m_task.size() + taskPart.size());
        m_task.insert(std::end(m_task), std::begin(taskPart), std::end(taskPart)); 
        return true;
    }

    bool Task::append(const TaskCollection&& taskPart) noexcept {
        m_task.reserve(m_task.size() + taskPart.size());
        move(taskPart.begin(), taskPart.end(), back_inserter(m_task));
        return true;
    }

    bool Task::operator==(const Task& other) const noexcept {
        return m_task == other.m_task;
    }

    bool Task::operator!=(const Task& other) const noexcept {
        return !(*this == other);
    }

    std::ostream& operator<<( std::ostream& os, const Task& task ) noexcept {
        os << std::string( "Task: {");

        for(const auto& subTask : task.getTask()) {
            os << std::string(" ") << subTask;
        }
        os << std::string(" }");
        return os;
    }
} }
