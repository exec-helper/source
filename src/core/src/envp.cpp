#include "envp.h"

#include <cstring>
#include <string>

using std::string;

namespace {
    const string DELIMITER("=");
}

namespace execHelper {
    namespace core {
        Envp::Envp(const EnvironmentCollection& env) noexcept
        {
            m_envp.reserve(env.size() + 1);
            for(const auto& envVar : env) {
                auto newVar = new char[envVar.first.size() + DELIMITER.size() + envVar.second.size() + 1U];
                strncpy(newVar, envVar.first.c_str(), envVar.first.size());
                strncpy(&newVar[envVar.first.size()], DELIMITER.c_str(), DELIMITER.size());
                strncpy(&newVar[envVar.first.size() + DELIMITER.size()], envVar.second.c_str(), envVar.second.size() + 1U);
                m_envp.emplace_back(newVar);
            }
            m_envp.emplace_back(nullptr);
        }

        Envp::Envp(const Envp& other) noexcept {
            m_envp.reserve(other.m_envp.size());
            deepCopy(other);
        }

        Envp::~Envp() noexcept {
            for(char* arg : m_envp) {
                delete[] arg;
            }
        }

        Envp& Envp::operator=(const Envp& other) noexcept {
            m_envp.reserve(other.m_envp.size());
            deepCopy(other);
            return *this;
        }

        char** Envp::getEnvp() noexcept {
            return &m_envp[0];
        }

        void Envp::deepCopy(const Envp& other) noexcept {
            for(const auto& otherElement : other.m_envp) {
                size_t length = strlen(otherElement);
                auto* newArg = new char[length];
                strncpy(newArg, otherElement, length);
                m_envp.emplace_back(newArg);
            }
        }
    } // core
} // execHelper
