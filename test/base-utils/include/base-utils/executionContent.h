#ifndef EXECUTION_CONTENT_INCLUDE
#define EXECUTION_CONTENT_INCLUDE

#include <atomic>
#include <string>
#include <thread>

#include <boost/asio.hpp>
#include <boost/serialization/vector.hpp>
#include <gsl/gsl>

#include "base-utils/commandUtils.h"
#include "base-utils/execution.h"
#include "base-utils/tmpFile.h"

namespace execHelper {
namespace test {
namespace baseUtils {
class IoService {
  public:
    IoService() = default;
    IoService(const IoService& other) = delete;
    IoService(IoService&& other) = default;
    ~IoService() noexcept;

    IoService& operator=(const IoService& other) = delete;
    IoService& operator=(IoService&& other) = default;

    void start() noexcept;
    void stop() noexcept;
    void run() noexcept;

    boost::asio::io_service& get() noexcept;

  private:
    boost::asio::io_service m_service;
    std::atomic<bool> m_isRunning = {false};
    std::thread m_thread;
};

struct ExecutionContentData {
    std::vector<std::string> args;

    template <typename Archive>
    void serialize(Archive& ar, const unsigned int /*version*/) {
        ar& args;
    }
};

struct ExecutionContentDataReply {
    ReturnCode returnCode;

    ExecutionContentDataReply(ReturnCode returnCode) noexcept
        : returnCode(returnCode) {
        ;
    }
};

class ExecutionContentServer {
  public:
    using ConfigCommand = std::vector<std::string>;

    ExecutionContentServer(ReturnCode returnCode) noexcept;
    ~ExecutionContentServer() noexcept;

    ExecutionContentServer(const ExecutionContentServer& other) = delete;
    ExecutionContentServer(ExecutionContentServer&& other) noexcept;

    ExecutionContentServer&
    operator=(const ExecutionContentServer& other) = delete;
    ExecutionContentServer& operator=(ExecutionContentServer&& other) noexcept;

    void swap(ExecutionContentServer& other) noexcept;

    ConfigCommand getConfigCommand() const noexcept;
    unsigned int getNumberOfExecutions() const noexcept;
    const std::vector<ExecutionContentData>& getReceivedData() const noexcept;

    void clear() noexcept;

    static void registerIoService(gsl::not_null<IoService*> ioService) noexcept;

  private:
    /**
     * Opens the acceptor explicitly rather than using the appropriate constructor for it. This is to work around
     * the fact that the appropriate constructor will throw, causing an exception to leak out of the interface
     *
     * \throws boost::system::system_error  If the acceptor can not be opened
     */
    void openAcceptor();
    void init() noexcept;
    void accept() noexcept;
    ExecutionContentDataReply addData(ExecutionContentData data) noexcept;

    std::vector<ExecutionContentData> m_receivedData;
    uint32_t m_numberOfExecutions = {0};
    ReturnCode m_returnCode = {SUCCESS};
    TmpFile m_file;
    boost::asio::local::stream_protocol::endpoint m_endpoint;
    boost::asio::local::stream_protocol::socket m_socket;
    boost::asio::local::stream_protocol::acceptor m_acceptor;

    static IoService* m_ioService;
};
using ExecutionContent = ExecutionContentServer;

class ExecutionContentClient {
  public:
    ExecutionContentClient(const Path& file);

    ReturnCode addExecution(const ExecutionContentData& data);

  private:
    boost::asio::local::stream_protocol::endpoint m_endpoint;
};
} // namespace baseUtils
} // namespace test
} // namespace execHelper

#endif /* EXECUTION_CONTENT_INCLUDE */
