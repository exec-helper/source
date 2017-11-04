#include "argv.h"

#include <cstring>

#include <gsl/span>
#include <gsl/string_span>

#include "log/assertions.h"

using std::string;
using std::vector;

using gsl::czstring;
using gsl::span;

namespace execHelper {
namespace config {
Argv::Argv(int argc, const char* const* const argv) noexcept {
    span<const czstring<>> spanArgv(argv, argc);
    m_argv.reserve(argc + 1U);
    for(const auto& arg : spanArgv) {
        const auto argLength = strnlen(arg, 256U) + 1U;
        auto newArg = new char[argLength];
        strncpy(newArg, arg, argLength);
        m_argv.emplace_back(newArg);
    }
    m_argv.emplace_back(nullptr);
}

Argv::Argv(const vector<string>& task) noexcept {
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

Argv::Argv(Argv&& other) noexcept { swap(other); }

Argv::~Argv() noexcept { clear(); }

void Argv::deepCopy(const Argv& other) noexcept {
    clear();
    for(const auto& otherElement : other.m_argv) {
        if(otherElement == nullptr) {
            break;
        }
        size_t length = strlen(otherElement) + 1U;
        auto* newArg = new char[length];
        strncpy(newArg, otherElement, length);
        m_argv.emplace_back(newArg);
    }
    m_argv.emplace_back(nullptr);
}

Argv& Argv::operator=(const Argv& other) noexcept {
    m_argv.reserve(other.m_argv.size());
    deepCopy(other);
    return *this;
}

Argv& Argv::operator=(Argv&& other) noexcept {
    swap(other);
    return *this;
}

bool Argv::operator==(const Argv& other) const noexcept {
    return m_argv == other.m_argv;
}

bool Argv::operator!=(const Argv& other) const noexcept {
    return !(*this == other);
}

char* Argv::operator[](size_t index) const noexcept {
    if(index >=
       m_argv.size() -
           1U) { // Accessing the last nullptr element is considered an error
        return nullptr;
    }
    return m_argv[index];
}

void Argv::swap(Argv& other) noexcept { m_argv.swap(other.m_argv); }

void Argv::clear() noexcept {
    for(const auto& arg : m_argv) {
        delete[] arg;
    }
    m_argv.clear();
}

size_t Argv::getArgc() const noexcept { return m_argv.size() - 1U; }

char** Argv::getArgv() noexcept { return &m_argv.at(0); }

const char* const* Argv::getArgv() const noexcept { return &m_argv.at(0); }

std::ostream& operator<<(std::ostream& os, const Argv& argv) noexcept {
    const span<const czstring<>> args(argv.getArgv(), argv.getArgc());
    bool firstIteration = true;
    for(const auto& arg : args) {
        if(!firstIteration) {
            os << ", ";
        } else {
            firstIteration = false;
        }
        os << arg;
    }
    return os;
}
} // namespace config
} // namespace execHelper
