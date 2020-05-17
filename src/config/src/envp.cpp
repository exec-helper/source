#include "envp.h"

#include <cstring>
#include <iostream>
#include <string>
#include <string_view>

#include <gsl/gsl>

#include "log/assertions.h"

using std::string;

using gsl::czstring;
using gsl::owner;
using gsl::span;

using namespace std::literals;

namespace execHelper::config {
Envp::Envp(const EnvironmentCollection& env) noexcept {
    static const string DELIMITER("=");
    m_envp.reserve(env.size() + 1);
    for(const auto& envVar : env) {
        string envVarString(envVar.first);
        envVarString.append(DELIMITER).append(envVar.second);

        auto* newVar = // NOLINT(cppcoreguidelines-owning-memory)
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

auto Envp::operator=(const Envp& other) noexcept -> Envp& {
    if(this != &other) {
        m_envp.reserve(other.m_envp.size());
        deepCopy(other);
    }
    return *this;
}

auto Envp::operator=(Envp&& other) noexcept -> Envp& {
    swap(other);
    return *this;
}

void Envp::swap(Envp& other) noexcept { m_envp.swap(other.m_envp); }

auto Envp::size() const noexcept -> size_t {
    ensures(!m_envp.empty());
    return m_envp.size() - 1U;
}

auto Envp::getEnvp() noexcept -> char** { return &m_envp.at(0); }

auto Envp::getEnvp() const noexcept -> const char* const* {
    return &m_envp.at(0);
}

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
        auto* newArg = // NOLINT(cppcoreguidelines-owning-memory)
            new char[length];
        strncpy(newArg, otherElement, length);
        m_envp.emplace_back(newArg);
    }
    m_envp.emplace_back(nullptr);
}

auto operator<<(std::ostream& os, const Envp& envp) noexcept -> std::ostream& {
    const span<const czstring<>> envs(envp.getEnvp(), envp.size());
    bool firstIteration = true;
    for(const auto& env : envs) {
        if(!firstIteration) {
            os << ", "sv;
        } else {
            firstIteration = false;
        }
        os << env;
    }
    return os;
}
} // namespace execHelper::config
