#include "executionContent.h"

#include <fstream>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

#include <boost/archive/archive_exception.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/core/swap.hpp>

#include "base-utils/yaml.h"

using std::cerr;
using std::endl;
using std::make_shared;
using std::move;
using std::string;
using std::terminate;
using std::thread;

using boost::asio::buffer;
using boost::asio::io_service;
using boost::asio::socket_base;
using boost::asio::local::stream_protocol;
using boost::system::error_code;
using boost::system::system_error;
using gsl::not_null;

using execHelper::test::baseUtils::ExecutionContentData;
using execHelper::test::baseUtils::ExecutionContentDataReply;
using execHelper::test::baseUtils::ReturnCode;
using execHelper::test::baseUtils::YamlReader;

namespace {
constexpr uint8_t HEADER_LENGTH = 8;
using ExecuteContentMessageCallback =
    std::function<ExecutionContentDataReply(const ExecutionContentData&)>;
class ExecutionSession : public std::enable_shared_from_this<ExecutionSession> {
  public:
    explicit ExecutionSession(stream_protocol::socket socket,
                              ExecuteContentMessageCallback callback)
        : m_callback(move(callback)), m_socket(move(socket)) {
        ;
    }

    void start() { readHeader(); }

  private:
    void readHeader() noexcept {
        auto self(shared_from_this());

        boost::asio::async_read(
            m_socket, buffer(&messageLength, HEADER_LENGTH),
            [this, self](error_code ec, std::size_t length) {
                if(ec) {
                    cerr << __FILE__ << ":" << __LINE__ << " " << this << " "
                         << "Unexpected error occurred: " << ec << ": "
                         << ec.message() << std::endl;
                    terminate();
                }
                if(length != HEADER_LENGTH) {
                    cerr << __FILE__ << ":" << __LINE__ << " " << this << " "
                         << "Partial message received. Received length = "
                         << length
                         << " expected length = " << int(HEADER_LENGTH)
                         << std::endl;
                    terminate();
                }

                std::istringstream iss(
                    std::string(messageLength, HEADER_LENGTH));
                std::size_t headerLength = 0;
                if(!(iss >> std::hex >> headerLength)) {
                    cerr << __FILE__ << ":" << __LINE__ << " " << this << " "
                         << "Could not stream data to length" << endl;
                    terminate();
                }

                readMessage(headerLength);
            });
    }

    void readMessage(uint32_t length) noexcept {
        auto self(shared_from_this());

        m_data.resize(length);
        boost::asio::async_read(
            m_socket, buffer(m_data),
            [this, self](error_code ec, std::size_t length) {
                if(ec) {
                    cerr << __FILE__ << ":" << __LINE__ << " " << this << " "
                         << "Unexpected error occurred: " << ec << ": "
                         << ec.message() << std::endl;
                    terminate();
                }

                // Deserialize the received data
                std::string inbound_data(&m_data[0], length);

                ExecutionContentData content;
                std::istringstream deserialized_stream(inbound_data);
                try {
                    boost::archive::text_iarchive deserialized(
                        deserialized_stream);
                    deserialized >> content;
                } catch(const boost::archive::archive_exception& e) {
                    cerr << __FILE__ << ":" << __LINE__ << " " << this << " "
                         << e.what() << endl;
                    terminate();
                }

                // Process data
                auto reply = m_callback(content);
                sendReply(reply);
            });
    }

    void sendReply(const ExecutionContentDataReply& reply) {
        auto self(shared_from_this());

        boost::asio::async_write(
            m_socket, buffer(&reply, sizeof(reply)),
            [this, self](boost::system::error_code ec, std::size_t /*length*/) {
                if(ec) {
                    cerr << __FILE__ << ":" << __LINE__ << " " << this << " "
                         << "Unexpected error occurred: " << ec << ": "
                         << ec.message() << std::endl;
                    terminate();
                }
                m_socket.shutdown(
                    boost::asio::local::stream_protocol::socket::shutdown_both);
                m_socket.close();
            });
    }

    char messageLength[HEADER_LENGTH];
    std::vector<char> m_data;
    ExecuteContentMessageCallback m_callback;
    stream_protocol::socket m_socket;
};
} // namespace

namespace execHelper {
namespace test {
namespace baseUtils {
IoService::~IoService() noexcept { stop(); }

void IoService::start() noexcept {
    m_isRunning = true;
    error_code ec;
    m_service.run(ec);
    if(ec) {
        cerr << "Received an unexpected system error: " << ec << ":"
             << ec.message() << endl;
        assert(false);
    }
    m_service.reset();
    m_isRunning = false;
}

void IoService::stop() noexcept {
    if(m_isRunning) {
        m_service.stop();
    }
    assert(m_thread.joinable());
    m_thread.join();
}

void IoService::run() noexcept {
    if(!m_isRunning) {
        m_thread = std::thread([this]() { this->start(); });
    }
}

boost::asio::io_service& IoService::get() noexcept { return m_service; }

IoService* ExecutionContentServer::m_ioService = nullptr;

ExecutionContentServer::ExecutionContentServer(ReturnCode returnCode) noexcept
    : m_returnCode(returnCode),
      m_file("exec-helper.unix-socket.%%%%%%%%"),
      m_endpoint(m_file.getPath().string()),
      m_socket(m_ioService->get()),
      m_acceptor(m_ioService->get()) {
    try {
        // Explicitly open the acceptor in the constructor body: exceptions will leak out of the constructor otherwise
        openAcceptor();
    } catch(const system_error& e) {
        cerr << "Unexpected exception caught: '" << e.what()
             << "'. The execution content server will not work" << endl;
        assert(false);
    }
    init();
}

ExecutionContentServer::ExecutionContentServer(
    ExecutionContentServer&& other) noexcept
    : m_numberOfExecutions(other.m_numberOfExecutions),
      m_returnCode(other.m_returnCode),
      m_endpoint(move(other.m_endpoint)),
      m_socket(move(other.m_socket)),
      m_acceptor(other.m_acceptor.get_io_service()) {
    try {
        openAcceptor();
    } catch(const system_error& e) {
        cerr << "Unexpected exception caught: " << e.what() << endl;
        assert(false);
    }
}

ExecutionContentServer::~ExecutionContentServer() noexcept {
    m_acceptor.close();
    ::unlink(m_file.getPath().native().c_str());
}

void ExecutionContentServer::openAcceptor() {
    m_acceptor.open(m_endpoint.protocol());
    m_acceptor.set_option(socket_base::reuse_address(true));
    m_acceptor.bind(m_endpoint);
    m_acceptor.listen(socket_base::max_connections);
}

ExecutionContentServer& ExecutionContentServer::
operator=(ExecutionContentServer&& other) noexcept {
    swap(other);
    return *this;
}

void ExecutionContentServer::swap(ExecutionContentServer& other) noexcept {
    std::swap(m_numberOfExecutions, other.m_numberOfExecutions);
    std::swap(m_returnCode, other.m_returnCode);

    try {
        boost::swap(m_endpoint, other.m_endpoint);
        boost::swap(m_acceptor, other.m_acceptor);
    } catch(const system_error& e) {
        cerr << "Unexpected exception caught: " << e.what() << endl;
        assert(false);
    }
}

void ExecutionContentServer::registerIoService(
    gsl::not_null<IoService*> ioService) noexcept {
    m_ioService = ioService;
}

void ExecutionContentServer::init() noexcept { accept(); }

void ExecutionContentServer::accept() noexcept {
    m_acceptor.async_accept(m_socket, [this](error_code ec) {
        switch(ec.value()) {
        case boost::system::errc::success: {
            ExecuteContentMessageCallback callback(
                [this](const ExecutionContentData& data) {
                    return addData(data);
                });
            make_shared<ExecutionSession>(std::move(m_socket), callback)
                ->start();
            accept();
        } break;
        case boost::system::errc::operation_canceled:
            break;
        default:
            cerr << __FILE__ << ":" << __LINE__ << " "
                 << "Unexpected error occurred: " << ec << ": " << ec.message()
                 << endl;
            terminate();
            break;
        }
    });
}

ExecutionContentServer::ConfigCommand
ExecutionContentServer::getConfigCommand() const noexcept {
    return {
        "execution-content",
        m_endpoint.path()}; // Use the execution-content that is in your path
}

ExecutionContentDataReply
ExecutionContentServer::addData(ExecutionContentData data) noexcept {
    m_receivedData.emplace_back(data);
    ++m_numberOfExecutions;
    return {m_returnCode};
}

const std::vector<ExecutionContentData>&
ExecutionContentServer::getReceivedData() const noexcept {
    return m_receivedData;
}

unsigned int ExecutionContentServer::getNumberOfExecutions() const noexcept {
    return m_numberOfExecutions;
}

void ExecutionContentServer::clear() noexcept { m_numberOfExecutions = 0; }

ExecutionContentClient::ExecutionContentClient(const Path& file)
    : m_endpoint(file.native()) {
    ;
}

ReturnCode
ExecutionContentClient::addExecution(const ExecutionContentData& data) {
    io_service ioService;

    stream_protocol::socket socket(ioService);
    std::thread t([&ioService]() { ioService.run(); });

    error_code ec;
    socket.connect(m_endpoint, ec);
    if(ec) {
        cerr << __FILE__ << ":" << __LINE__ << " "
             << "Client: Unexpected error occurred: " << ec << ": "
             << ec.message() << endl;
        return RUNTIME_ERROR;
    }

    // Serialize the struct before sending
    std::ostringstream serialized_stream;
    try {
        boost::archive::text_oarchive serialized(serialized_stream);
        serialized << data;
    } catch(const boost::archive::archive_exception& e) {
        cerr << __FILE__ << ":" << __LINE__ << e.what() << endl;
        return RUNTIME_ERROR;
    }
    auto outboundData = serialized_stream.str();

    std::ostringstream header_stream;
    header_stream << std::setw(HEADER_LENGTH) << std::hex
                  << outboundData.size();
    auto headerData = header_stream.str();
    if(!header_stream || header_stream.str().size() != HEADER_LENGTH) {
        cerr << __FILE__ << ":" << __LINE__
             << "Could not properly construct header stream" << endl;
        terminate();
    }

    std::vector<boost::asio::const_buffer> buffers;
    buffers.emplace_back(buffer(headerData));
    buffers.emplace_back(buffer(outboundData));

    boost::asio::write(socket, buffers, ec);
    if(ec) {
        cerr << __FILE__ << ":" << __LINE__ << " "
             << "Client: Unexpected error occurred: " << ec << ": "
             << ec.message() << endl;
        return RUNTIME_ERROR;
    }

    ExecutionContentDataReply reply(RUNTIME_ERROR);
    size_t replyLength =
        boost::asio::read(socket, buffer(&reply, sizeof(reply)), ec);
    if(ec) {
        cerr << __FILE__ << ":" << __LINE__ << " "
             << "Client: Unexpected error occurred: " << ec << ": "
             << ec.message() << endl;
        return RUNTIME_ERROR;
    }
    if(replyLength != sizeof(reply)) {
        cerr << __FILE__ << ":" << __LINE__ << " "
             << "Client: Mismatch in reply length detected." << endl;
        return RUNTIME_ERROR;
    }
    socket.close();
    t.join(); // Note: since it will have no more work, io_service.run should automatically return and end the thread. So a join on the thread should suffice.
    return reply.returnCode;
}
} // namespace baseUtils
} // namespace test
} // namespace execHelper
