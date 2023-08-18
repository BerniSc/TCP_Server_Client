#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <thread>
using namespace boost::asio;
using ip::tcp;

std::string get_current_message() {
    static int counter = 0;
    std::string s = "Message ";
    s += std::to_string(counter++);
    return s;
}

// Inheriting from shared_from_this ensures that the object is kept alive for every object that refers to this one
// Connection Handler for specific Socket Connection
class con_handler_tcp : public boost::enable_shared_from_this<con_handler_tcp> {
    public:
        typedef boost::shared_ptr<con_handler_tcp> pointer;

        static pointer create(boost::asio::io_context& io_context) {
            return pointer(new con_handler_tcp(io_context));
        }

        tcp::socket& socket() {
            return _socket;
        }

        void start() {
            // Starts Asynchronous TCP Read on socket with Length max_length and store it in Data
            // Takes buffer (real Data and Max Length) and the Handler-Function (Bound to own Instance, placeholder in case of Error and number of Bytes transfered)
            _socket.async_read_some(boost::asio::buffer(data, max_length),
                boost::bind(&con_handler_tcp::handle_read, shared_from_this(), boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));

            // Starts Asynchronous TCP Write on socket with Length max_length and takes Data from var "message"
            // Takes buffer (real Data and Max Length) and the Handler-Function (Bound to own Instance, placeholder in case of Error and number of Bytes transfered)
            message = get_current_message();
            std::cout << message << std::endl;
            _socket.async_write_some(boost::asio::buffer(message, 2048),
                boost::bind(&con_handler_tcp::handle_write, shared_from_this(), boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
        }

    private:
        // Socket that handles the Connection
        tcp::socket _socket;
        std::string message = "Hello From Server!";

        enum { 
            max_length = 1024
        };
        char data[max_length];

        con_handler_tcp(boost::asio::io_context& io_context) : _socket(io_context) {
        
        }

        void handle_read(const boost::system::error_code& err, size_t bytes_transferred) {
            if(!err) {
                std::cout << "Server recieved: ";
                std::cout << data << std::endl;
            } else {
                std::cerr << "err (recv): " << err.message() << std::endl;
                _socket.close();
            }
        }
        
        void handle_write(const boost::system::error_code& err, size_t bytes_transferred) {
            if(!err) {
	            std::cout << "Server sent message: " << message << "!" << std::endl;
            } else {
                std::cerr << "err (recv): " << err.message() << std::endl;
                _socket.close();
            }
        }
};

class TCP_Server {
    public:
        TCP_Server(boost::asio::io_context& io_context, const unsigned short int port) : 
            _io_context(io_context),
            _acceptor(_io_context, tcp::endpoint(boost::asio::ip::address::from_string("127.0.0.1"), port)) {
            // _acceptor(_io_context, tcp::endpoint(tcp::v4(), port)) {
            // Start Accepting Messages on TCP Socket with IP 127.0.0.1 with Port port
            start_accept();
        }
    
    private:
        boost::asio::io_context& _io_context;
        tcp::acceptor _acceptor;

        void start_accept() {
            // creates a socket
            con_handler_tcp::pointer connection = con_handler_tcp::create(_io_context);

            // initiates an asynchronous accept operation 
            // to wait for a new connection. 
            _acceptor.async_accept(connection->socket(), 
                boost::bind(&TCP_Server::handle_accept, this, connection, boost::asio::placeholders::error));
        }
	
        void handle_accept(con_handler_tcp::pointer connection, const boost::system::error_code& err) {
            if (!err) {
                connection->start();
            }
	        start_accept();
        }
};

void start_my_tcp_server(const unsigned short int port) {
    // Create IO-Service on Localhost
    boost::asio::io_context io_context;
    // Use IO_Service to open Server on Localhost on Port 1234   
    TCP_Server tcpServer(io_context, 1234);
    // Starts Thread continually running and Listening/Answering
    io_context.run();
}

int main(int argc, char *argv[]) {
    try {
        std::thread server_thread(start_my_tcp_server, 1234);

        for(int i = 0; i < 100; i++) {
            std::cout << "Main Programm now running at Stage " << i << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        }

        server_thread.join();
    } catch(std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
    return 0;
}