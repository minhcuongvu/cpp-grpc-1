#include "helloworld.grpc.pb.h"
#include <grpcpp/grpcpp.h>

#include <iostream>
#include <memory>
#include <string>

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
using helloworld::Greeter;
using helloworld::HelloReply;
using helloworld::HelloRequest;

class GreeterClient {
public:
  explicit GreeterClient(std::shared_ptr<Channel> channel)
      : stub_(Greeter::NewStub(channel)) {}

  std::string SayHello(const std::string &user) {
    // Prepare the request
    HelloRequest request;
    request.set_name(user);

    // Prepare the response and context
    HelloReply reply;
    ClientContext context;

    // Perform the RPC
    Status status = stub_->SayHello(&context, request, &reply);

    // Check the status
    if (status.ok()) {
      return reply.message();
    } else {
      std::cerr << "RPC failed: " << status.error_code() << " - "
                << status.error_message() << std::endl;
      return "RPC failed";
    }
  }

private:
  std::unique_ptr<Greeter::Stub> stub_;
};

int main(int argc, char **argv) {
  std::string target_str = "localhost:50051";
  std::string user = "world";

  if (argc > 1) {
    user = argv[1];
  }

  // Create a gRPC channel (no TLS, insecure)
  GreeterClient greeter(
      grpc::CreateChannel(target_str, grpc::InsecureChannelCredentials()));

  std::string reply = greeter.SayHello(user);
  std::cout << "Greeter received: " << reply << std::endl;

  return 0;
}
