#include "envp.h"

#include <cstring>
#include <string>

#include <gsl/pointers>
#include <gsl/span>
#include <gsl/string_span>

#include "log/assertions.h"

using std::string;

using gsl::czstring;
using gsl::owner;
using gsl::span;

namespace execHelper {
namespace config {
Envp::Envp(const EnvironmentCollection& env) noexcept {
    static const string DELIMITER("=");
    m_envp.reserve(env.size() + 1);
    for(const auto& envVar : env) {
        string envVarString(envVar.first);
        envVarString.append(DELIMITER).append(envVar.second);

        auto newVar = // NOLINT(cppcoreguidelines-owning-memory)
            new char[envVarString.size() + 1U];
        strncpy(newVar, envVarString.c_str(), envVarString.size() + 1U);
        m_envp.emplace_back(newVar);
    }
    m_envp.emplace_back(nullptr);
}

Envp::Envp(const Envp& other) noexcept {
    m_envp.reserve(other.m_envp.size());
    deepCopy(other);
}

Envp::Envp(Envp&& other) noexcept { swap(other); }

Envp::~Envp() noexcept { clear(); }

Envp& Envp::operator=(const Envp& other) noexcept {
    m_envp.reserve(other.m_envp.size());
    deepCopy(other);
    return *this;
}

Envp& Envp::operator=(Envp&& other) noexcept {
    swap(other);
    return *this;
}

void Envp::swap(Envp& other) noexcept { m_envp.swap(other.m_envp); }

size_t Envp::size() const noexcept {
    ensures(!m_envp.empty());
    return m_envp.size() - 1U;
}

char** Envp::getEnvp() noexcept { return &m_envp.at(0); }

const char* const* Envp::getEnvp() const noexcept { return &m_envp.at(0); }

void Envp::clear() noexcept {
    for(const auto& arg : m_envp) {
        delete[] arg; // NOLINT(cppcoreguidelines-owning-memory)
    }
    m_envp.clear();
}

void Envp::deepCopy(const Envp& other) noexcept {
    clear(); // Clear the current content first
    for(const auto& otherElement : other.m_envp) {
        if(otherElement == nullptr) {
            break;
        }
        size_t length = strlen(otherElement) + 1U;
        auto newArg = // NOLINT(cppcoreguidelines-owning-memory)
            new char[length];
        strncpy(newArg, otherElement, length);
        m_envp.emplace_back(newArg);
    }
    m_envp.emplace_back(nullptr);
}

std::ostream& operator<<(std::ostream& os, const Envp& envp) noexcept {
    const span<const czstring<>> envs(envp.getEnvp(), envp.size());
    bool firstIteration = true;
    for(const auto& env : envs) {
        if(!firstIteration) {
            os << ", ";
        } else {
            firstIteration = false;
        }
        os << env;
    }
    return os;
}
} // namespace config
} // namespace execHelper
