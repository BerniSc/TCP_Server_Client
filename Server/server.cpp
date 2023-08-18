#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/enable_shared_from_this.hpp>
using namespace boost::asio;
using ip::tcp;

// Inheriting from shared_from_this ensures that the object is kept alive for every object that refers to this one
// Connection Handler for specific Socket Connection
class con_handler : public boost::enable_shared_from_this<con_handler> {
    private:
        // Socket that handles the Connection
        tcp::socket _socket;
        std::string message = "Hello From Server!";
        enum { 
            max_length = 1024
        };
        int counter = 0;
        char data[max_length];
    
    public:
        typedef boost::shared_ptr<con_handler> pointer;
        
        con_handler(boost::asio::io_service& io_service) : _socket(io_service) {
        
        }

        static pointer create(boost::asio::io_service& io_service) {
            return pointer(new con_handler(io_service));
        }
        
        tcp::socket& socket() {
            return _socket;
        }

        void start() {
            // Starts Asynchronous TCP Read on socket with Length max_length and store it in Data
            // Takes buffer (real Data and Max Length) and the Handler-Function (Bound to own Instance, placeholder in case of Error and number of Bytes transfered)
            _socket.async_read_some(boost::asio::buffer(data, max_length),
                boost::bind(&con_handler::handle_read, shared_from_this(), boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));

            // Starts Asynchronous TCP Write on socket with Length max_length and takes Data from var "message"
            // Takes buffer (real Data and Max Length) and the Handler-Function (Bound to own Instance, placeholder in case of Error and number of Bytes transfered)
            _socket.async_write_some(boost::asio::buffer(&message, max_length),
                boost::bind(&con_handler::handle_write, shared_from_this(), boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
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
	            std::cout << "Server sent message!" << std::endl;
                message += counter++;
            } else {
                std::cerr << "err (recv): " << err.message() << std::endl;
                _socket.close();
            }
        }
};

class Server {
    private:
        tcp::acceptor acceptor_;

        void start_accept() {
            // creates a socket
            con_handler::pointer connection = con_handler::create((boost::asio::io_context&)(acceptor_).get_executor().context());

            // initiates an asynchronous accept operation 
            // to wait for a new connection. 
            acceptor_.async_accept(connection->socket(), 
                boost::bind(&Server::handle_accept, this, connection, boost::asio::placeholders::error));
        }

    public:
        Server(boost::asio::io_service& io_service, const unsigned short int port): acceptor_(io_service, tcp::endpoint(tcp::v4(), port)) { 
            start_accept();
        }
	

        void handle_accept(con_handler::pointer connection, const boost::system::error_code& err) {
            if (!err) {
                connection->start();
            }
	        start_accept();
        }
};

int main(int argc, char *argv[]) {
    try {
        // Create IO-Service on Localhost
        boost::asio::io_service io_service;
        // Use IO_Service to open Server on Localhost on Port 1234   
        Server server(io_service, 1234);
        // Starts Thread continually running and Listening/Answering
        io_service.run();
    } catch(std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
    return 0;
}