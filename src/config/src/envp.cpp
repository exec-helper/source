#include "envp.h"

#include <cstddef>
#include <cstring>
#include <iostream>
#include <span>
#include <string>
#include <string_view>

#include "log/assertions.h"
#include "logger.h"

using std::span;
using std::string;
using std::string_view;

using namespace std::literals;

namespace execHelper::config {
Envp::Envp(const EnvironmentCollection& env) noexcept {
    using namespace std::literals;

    static const auto DELIMITER = "="sv;
    m_envp.reserve(env.size() + 1);
    for(const auto& envVar : env) {
        string envVarString(envVar.first);
        envVarString.append(DELIMITER).append(envVar.second);

        try {
            auto* newVar = // NOLINT(cppcoreguidelines-owning-memory)
                new char[envVarString.size() + 1U];
            strncpy(newVar, envVarString.c_str(), envVarString.size() + 1U);
            newVar // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
                [envVarString.size()] = '\0';
            m_envp.emplace_back(newVar);
        } catch(const std::bad_alloc& e) {
            LOG(error) << e.what();
        }
    }
    m_envp.emplace_back(nullptr);
}

Envp::~Envp() noexcept { clear(); }
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

auto operator<<(std::ostream& out, const Envp& envp) noexcept -> std::ostream& {
    const span<const char* const> envs(envp.getEnvp(), envp.size());
    bool firstIteration = true;
    for(const auto& env : envs) {
        if(!firstIteration) {
            out << ", "sv;
        } else {
            firstIteration = false;
        }
        out << env;
    }
    return out;
}
} // namespace execHelper::config
