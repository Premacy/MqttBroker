#include <iostream>
#include <sstream>
#include <map>
#include <functional>
#include <boost/asio.hpp>
#include <json.hpp>

#include "MqttProtocol/MqttProtocolManager.h"
/*
namespace Utils{
    std::vector<std::string> SplitByChar(const std::string& str, char delim){
        std::vector<std::string> result;
        std::istream stream(str.c_str());

        std::string buff;
        while(std::getline(stream, buff, delim)){
            result.push_back(std::move(buff));
        }

        return result;
    }
};

namespace Bytes{

};

struct command_t{
    using param_name = std::string;
    using param_value = std::string;
    std::map<param_name, param_value>;
};

void usage(){
    std::cout << "Usage: " << std::endl;
    std::cout << "-p port" << std::endl;
    std::cout << "-h host" << std::endl;
    std::cout << "-help app usage" << std::endl;
    std::cout << "-d daemonize process" << std::endl;
}

command_t parse_command(const std::string& command_str){
    command_t command;

    return command_t;
}

class Terminal{
public:
    void run(){
        std::string command;
        while(true){
            std::getline(cin, command);
            
        }
    }
};
*/

using boost::asio::ip::tcp;

class tcp_connection
  : public std::enable_shared_from_this<tcp_connection>
{
public:
  using pointer =  std::shared_ptr<tcp_connection>;

   tcp_connection(boost::asio::io_context& io_context)
    : socket_(io_context)
  {
  }

  tcp::socket& socket()
  {
    return socket_;
  }

  void start()
  {}

  void write(std::string& message_){
    boost::asio::async_write(socket_, boost::asio::buffer(message_),
        [](const boost::system::error_code& ec,
           size_t sz)
        {
            if(ec){
                std::cout << "Error write in socket" << std::endl;
            }else{
                std::cout << "Succsessfull write " << std::endl;
            }
        });
  }

  void read(std::string& message_){
    boost::asio::async_read(socket_, boost::asio::buffer(message_),
        [](const boost::system::error_code& ec,
           size_t sz)
        {

        });
  }

private:
  tcp::socket socket_;
};

class MqttBroker
{
    /*using topic = Topic;
    using topic_name = std::string;*/

public:
  MqttBroker(boost::asio::io_context& io_context, std::string ip, int port)
    : io_context_(io_context),
      acceptor_(io_context, tcp::endpoint(tcp::v4(), port))
  {
    start_server();
  }

private:
  void start_server()
  { 
    std::cout << "Start server..." << std::endl;
    accept_connection();
  }

  void accept_connection(){
    tcp_connection::pointer new_connection =
      std::make_shared<tcp_connection>(io_context_);

    acceptor_.async_accept(new_connection->socket(),
        std::bind(&MqttBroker::handle_accept, this, new_connection,
          boost::asio::placeholders::error));
  }

  void handle_accept(tcp_connection::pointer new_connection,
      const boost::system::error_code& error)
  {
    if (!error)
    {
        std::cout << "Start new connection" << std::endl;
        new_connection->start();
    }

    accept_connection();
  }

  boost::asio::io_context& io_context_;
  tcp::acceptor acceptor_;

  std::vector<tcp_connection> connections;
  /*std::vector<topic> topics;
  std::vector<message> messages;*/
};

int main(int arc, char* argv[]){
    Json json;
    auto ptree = json.ReadJson("config.json");

    std::string ip = ptree.get<std::string>("network.ip");
    int port = ptree.get<int>("network.port");

    boost::asio::io_context io_context;
    MqttBroker broker(io_context, ip, port);

    io_context.run();

    return 0;
}
