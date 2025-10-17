#include "calculator.grpc.pb.h"
#include "helloworld.grpc.pb.h"
#include <grpcpp/grpcpp.h>

#include <iostream>
#include <memory>
#include <random>
#include <string>

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;

using helloworld::Greeter;
using helloworld::HelloReply;
using helloworld::HelloRequest;

using calculator::AddRequest;
using calculator::AddResponse;
using calculator::Calculator;

class GreeterClient {
public:
  explicit GreeterClient(std::shared_ptr<Channel> channel)
      : stub_(Greeter::NewStub(channel)) {}

  std::string SayHello(const std::string &user) {
    HelloRequest request;
    request.set_name(user);
    HelloReply reply;
    ClientContext context;

    Status status = stub_->SayHello(&context, request, &reply);
    if (status.ok())
      return reply.message();

    std::cerr << "Greeter RPC failed: " << status.error_code() << " - "
              << status.error_message() << std::endl;
    return "RPC failed";
  }

private:
  std::unique_ptr<Greeter::Stub> stub_;
};

class CalculatorClient {
public:
  explicit CalculatorClient(std::shared_ptr<Channel> channel)
      : stub_(Calculator::NewStub(channel)) {}

  int Add(int a, int b) {
    AddRequest request;
    request.set_a(a);
    request.set_b(b);
    AddResponse reply;
    ClientContext context;

    Status status = stub_->Add(&context, request, &reply);
    if (status.ok())
      return reply.result();

    std::cerr << "Calculator RPC failed: " << status.error_code() << " - "
              << status.error_message() << std::endl;
    return 0;
  }

private:
  std::unique_ptr<Calculator::Stub> stub_;
};

int main(int argc, char **argv) {
  std::string greeter_target = "localhost:50051";
  std::string calc_target = "localhost:50052"; // separate port for calculator

  // Generate an interesting name for this request
  std::random_device rd;
  std::mt19937 gen(rd());
  std::vector<std::string> names = {"🌟Alice", "🚀Bob", "🔥Charlie", "🎉Dana"};
  std::uniform_int_distribution<> dis(0, names.size() - 1);
  std::string user = names[dis(gen)]; // random name

  if (argc > 1)
    user = argv[1]; // optional override from command line

  GreeterClient greeter(
      grpc::CreateChannel(greeter_target, grpc::InsecureChannelCredentials()));
  CalculatorClient calculator(
      grpc::CreateChannel(calc_target, grpc::InsecureChannelCredentials()));

  std::cout << "Greeter received: " << greeter.SayHello(user) << std::endl;

  std::uniform_int_distribution<> disNum(1, 100);
  int a = disNum(gen), b = disNum(gen);
  std::cout << "Calculator: " << a << " + " << b << " = "
            << calculator.Add(a, b) << std::endl;

  return 0;
}
