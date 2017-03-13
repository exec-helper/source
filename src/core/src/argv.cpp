#include "argv.h"

#include <cstring>

namespace execHelper { namespace core {
    Argv::Argv(const TaskCollection& task) noexcept {
        m_argv.reserve(task.size() + 1);

        for(const auto& arg : task) {
            auto newArg = new char[arg.size() + 1U];
            strncpy(newArg, arg.c_str(), arg.size() + 1U);
            m_argv.emplace_back(newArg);
        }
        m_argv.emplace_back(nullptr);
    }

    Argv::Argv(const Argv& other) noexcept {
        m_argv.reserve(other.m_argv.size());
        deepCopy(other);
    }

    Argv::~Argv() noexcept {
        for(char* arg : m_argv) {
            delete[] arg;
        }
    }

    void Argv::deepCopy(const Argv& other) noexcept {
        for(const auto& otherElement : other.m_argv) {
            size_t length = strlen(otherElement);
            auto* newArg = new char[length];
            strncpy(newArg, otherElement, length);
            m_argv.emplace_back(newArg);
        }
    }

    Argv& Argv::operator=(const Argv& other) noexcept {
        m_argv.reserve(other.m_argv.size());
        deepCopy(other);
        return *this;
    }

    char* Argv::operator[](size_t index) const noexcept {
        if(index >= m_argv.size()) {
            return nullptr; 
        }
        return m_argv[index];
    }

    size_t Argv::getArgc() const noexcept {
        return m_argv.size() - 1U;
    }

    char** Argv::getArgv() noexcept {
        return &m_argv[0];
    }

} /* core */ } /* execHelper */
