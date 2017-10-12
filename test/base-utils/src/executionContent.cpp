#include "executionContent.h" 

#include <exception>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

#include <boost/core/swap.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>

#include "base-utils/yaml.h"

using std::cerr;
using std::endl;
using std::move;
using std::string;
using std::make_shared;
using std::shared_ptr;
using std::terminate;
using std::thread;

using boost::asio::buffer;
using boost::asio::io_service;
using boost::asio::local::stream_protocol;
using boost::filesystem::ofstream;
using boost::system::error_code;
using gsl::not_null;

using execHelper::test::baseUtils::ExecutionContentData;
using execHelper::test::baseUtils::ExecutionContentDataReply;
using execHelper::test::baseUtils::ReturnCode;
using execHelper::test::baseUtils::YamlReader;

namespace { 
    using ExecuteContentMessageCallback = std::function<ExecutionContentDataReply(const ExecutionContentData&)>;
    class ExecutionSession : public std::enable_shared_from_this<ExecutionSession> {
        public:
            explicit ExecutionSession(stream_protocol::socket socket, ExecuteContentMessageCallback callback) :
                m_callback(move(callback)),
                m_socket(move(socket))
            {
                ;
            }

            void start() {
                readStream();
            }

        private:
            void readStream() {
                auto self(shared_from_this());
                boost::asio::async_read(m_socket,
                    buffer(&m_data, sizeof(m_data)),
                    [this, self](error_code ec, std::size_t /*length*/) {
                        if(ec != nullptr) {
                            cerr << __FILE__ << ":" << __LINE__ <<  " " << this << " " << "Unexpected error occurred: " << ec << ": " << ec.message() << std::endl;
                            terminate();
                        }
                        auto reply = m_callback(m_data);
                        sendReply(reply);
                    });
            }

            void sendReply(const ExecutionContentDataReply& reply) {
                auto self(shared_from_this());
                boost::asio::async_write(m_socket,
                    buffer(&reply, sizeof(reply)),
                    [this, self](boost::system::error_code ec, std::size_t /*length*/) {
                        if(ec != nullptr) {
                            cerr << __FILE__ << ":" << __LINE__ << " " << this << " " << "Unexpected error occurred: " << ec << ": " << ec.message() << std::endl;
                            terminate();
                        }
                    });
            }

            ExecutionContentData m_data;
            ExecuteContentMessageCallback m_callback;
            stream_protocol::socket m_socket;
    };
} // namespace 

namespace execHelper {
    namespace test {
        namespace baseUtils {
            IoService::~IoService() noexcept {
                assert(m_thread.joinable());
                m_thread.join();
            }

            void IoService::start() noexcept {
                m_isRunning = true;
                m_service.run();
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
                    m_thread = std::thread([this]() {this->start();});
                }
            }

            boost::asio::io_service& IoService::get() noexcept {
                return m_service;
            }

            IoService* ExecutionContentServer::m_ioService = nullptr;

            ExecutionContentServer::ExecutionContentServer(ReturnCode returnCode) noexcept :
                m_returnCode(returnCode),
                m_file("exec-helper.unix-socket.%%%%"),
                m_endpoint(m_file.getPath().native()),
                m_socket(m_ioService->get()),
                m_acceptor(m_ioService->get(), m_endpoint)
            {
                init();
            }

            ExecutionContentServer::ExecutionContentServer(ExecutionContentServer&& other) noexcept :
                m_numberOfExecutions(other.m_numberOfExecutions),
                m_returnCode(other.m_returnCode),
                m_endpoint(move(other.m_endpoint)),
                m_socket(move(other.m_socket)),
                m_acceptor(move(other.m_acceptor))
            {
                ;
            }

            ExecutionContentServer::~ExecutionContentServer() noexcept {
                ::unlink(m_file.getPath().native().c_str());
            }

            ExecutionContentServer& ExecutionContentServer::operator=(ExecutionContentServer&& other) noexcept {
                swap(other);
                return *this;
            }

            void ExecutionContentServer::swap(ExecutionContentServer& other) noexcept {
                std::swap(m_numberOfExecutions, other.m_numberOfExecutions);
                std::swap(m_returnCode, other.m_returnCode);

                boost::swap(m_endpoint, other.m_endpoint);
                boost::swap(m_acceptor, other.m_acceptor);
            }

            void ExecutionContentServer::registerIoService(gsl::not_null<IoService*> ioService) noexcept {
                m_ioService = ioService;
            }

            void ExecutionContentServer::init() noexcept {
                accept();
            }

            void ExecutionContentServer::accept() noexcept {
                m_acceptor.async_accept(m_socket, [this](error_code ec) {
                    switch(ec.value()) {
                        case boost::system::errc::success:
                            {
                                ExecuteContentMessageCallback callback([this](const ExecutionContentData& data){ return addData(data);});
                                make_shared<ExecutionSession>(std::move(m_socket), callback)->start();
                                accept();
                            }
                            break;
                        case boost::system::errc::operation_canceled:
                            break;
                        default:
                            cerr << __FILE__ << ":" << __LINE__ << " " << "Unexpected error occurred: " << ec << ": " << ec.message() << endl;
                            terminate();
                            break;
                    }
                });
            }

            ExecutionContentServer::ConfigCommand ExecutionContentServer::getConfigCommand() const noexcept {
                return {"execution-content", m_endpoint.path()};    // Use the execution-content that is in your path
            }

            ExecutionContentDataReply ExecutionContentServer::addData(const ExecutionContentData& /*data*/) noexcept {
                ++m_numberOfExecutions; 
                return ExecutionContentDataReply(m_returnCode);
            }

            unsigned int ExecutionContentServer::getNumberOfExecutions() const noexcept {
                return m_numberOfExecutions;
            }

            void ExecutionContentServer::clear() noexcept {
                m_numberOfExecutions = 0;
            }


            ExecutionContentClient::ExecutionContentClient(const Path& file) noexcept :
                m_endpoint(file.native())
            {
                ;
            }

            ReturnCode ExecutionContentClient::addExecution() noexcept {
                io_service ioService;

                stream_protocol::socket socket(ioService);
                std::thread t([&ioService](){ ioService.run(); });

                error_code ec;
                socket.connect(m_endpoint, ec);
                if(ec != nullptr) {
                    cerr << __FILE__ << ":" << __LINE__ << " " << "Client: Unexpected error occurred: " << ec << ": " << ec.message() << endl;
                    return RUNTIME_ERROR;
                }
                ExecutionContentData data;
                boost::asio::write(socket, buffer(&data, sizeof(data)), ec);
                if(ec != nullptr) {
                    cerr << __FILE__ << ":" << __LINE__ << " " << "Client: Unexpected error occurred: " << ec << ": " << ec.message() << endl;
                    return RUNTIME_ERROR;
                }

                ExecutionContentDataReply reply(RUNTIME_ERROR);
                size_t replyLength = boost::asio::read(socket, buffer(&reply, sizeof(reply)), ec);
                if(ec != nullptr) {
                    cerr << __FILE__ << ":" << __LINE__ << " " << "Client: Unexpected error occurred: " << ec << ": " << ec.message() << endl;
                    return RUNTIME_ERROR;
                }
                if(replyLength != sizeof(reply)) {
                    cerr << __FILE__ << ":" << __LINE__ << " " << "Client: Mismatch in reply length detected." << endl;
                    return RUNTIME_ERROR;
                }
                socket.close();
                t.join();   // Note: since it will have no more work, io_service.run should automatically return and end the thread. So a join on the thread should suffice.
                return reply.returnCode;
            }
        } // namespace baseUtils
    } // namespace test
} // namespace execHelper
