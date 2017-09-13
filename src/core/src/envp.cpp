#include "envp.h"

#include <cstring>
#include <string>

#include <gsl/span>
#include <gsl/string_span>

#include "log/assertions.h"

using std::string;

using gsl::czstring;
using gsl::span;

namespace execHelper {
    namespace core {
        Envp::Envp(const EnvironmentCollection& env) noexcept
        {
            static const string DELIMITER("=");
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

        Envp::Envp(Envp&& other) noexcept {
            swap(other);
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

        Envp& Envp::operator=(Envp&& other) noexcept {
            swap(other);
            return *this;
        }

        void Envp::swap(Envp& other) noexcept {
            m_envp.swap(other.m_envp);
        }
        
        size_t Envp::size() const noexcept {
            ensures(!m_envp.empty());
            return m_envp.size() - 1U;
        }

        char** Envp::getEnvp() noexcept {
            return &m_envp.at(0);
        }

        const char* const * Envp::getEnvp() const noexcept {
            return &m_envp.at(0);
        }

        void Envp::deepCopy(const Envp& other) noexcept {
            for(const auto& otherElement : other.m_envp) {
                size_t length = strlen(otherElement);
                auto* newArg = new char[length];
                strncpy(newArg, otherElement, length);
                m_envp.emplace_back(newArg);
            }
        }

        std::ostream& operator<<(std::ostream& os, const Envp& envp) noexcept {
            const span<const czstring<>> envs(envp.getEnvp(), envp.size());
            bool firstIteration = true;
            for(const auto& env : envs) {
                if(!firstIteration) {
                    os << " ";
                } else {
                    firstIteration = false;
                }
                os << env;
            }
            return os;
        }
    } // namespace core
} // namespace execHelper
