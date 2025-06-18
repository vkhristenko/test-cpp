#include <grpcpp/grpcpp.h>

#include <iostream>
#include <memory>
#include <string>

#include "protos/grpc_dummy/dummy_0.grpc.pb.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
using protos::grpc_dummy::DummyService;
using protos::grpc_dummy::HelloRequest;
using protos::grpc_dummy::HelloResponse;

class DummyClient {
public:
    DummyClient(std::shared_ptr<Channel> channel)
        : stub_(DummyService::NewStub(channel)) {}

    // Unary RPC method
    std::string SayHelloUnary(const std::string& name) {
        HelloRequest request;
        request.set_name(name);
        HelloResponse response;
        ClientContext context;
        Status status = stub_->SayHelloUnary(&context, request, &response);
        if (status.ok()) {
            return response.message();
        } else {
            std::cout << "Error: " << status.error_code() << ": "
                      << status.error_message() << std::endl;
            return "RPC failed";
        }
    }

    // Client streaming RPC method
    std::string SayHelloClientStreaming(const std::vector<std::string>& names) {
        ClientContext context;
        HelloResponse response;

        std::unique_ptr<grpc::ClientWriter<HelloRequest>> writer(
            stub_->SayHelloClientStreaming(&context, &response));
        for (const auto& name : names) {
            HelloRequest request;
            request.set_name(name);
            if (!writer->Write(request)) {
                break;
            }
        }
        writer->WritesDone();
        Status status = writer->Finish();
        if (status.ok()) {
            return response.message();
        } else {
            std::cout << "Error: " << status.error_code() << ": "
                      << status.error_message() << std::endl;
            return "RPC failed";
        }
    }

    // Server streaming RPC method
    void SayHelloServerStreaming(const std::string& name) {
        HelloRequest request;
        request.set_name(name);
        ClientContext context;
        std::unique_ptr<grpc::ClientReader<HelloResponse>> reader(
            stub_->SayHelloServerStreaming(&context, request));
        HelloResponse response;
        while (reader->Read(&response)) {
            std::cout << response.message() << std::endl;
        }
        Status status = reader->Finish();
        if (!status.ok()) {
            std::cout << "Error: " << status.error_code() << ": "
                      << status.error_message() << std::endl;
        }
    }

    // Bidirectional streaming RPC method
    void SayHelloBidiStreaming(const std::vector<std::string>& names) {
        ClientContext context;
        std::shared_ptr<grpc::ClientReaderWriter<HelloRequest, HelloResponse>>
            stream(stub_->SayHelloBidiStreaming(&context));
        for (auto const& name : names) {
            HelloRequest request;
            request.set_name(name);
            stream->Write(request);

            HelloResponse response;
            stream->Read(&response);
            std::cout << response.message() << std::endl;
        }
        stream->WritesDone();
        Status status = stream->Finish();
        if (!status.ok()) {
            std::cout << "Error: " << status.error_code() << ": "
                      << status.error_message() << std::endl;
        }
    }

private:
    std::unique_ptr<DummyService::Stub> stub_;
};

int main(int argc, char** argv) {
    DummyClient client(grpc::CreateChannel("localhost:50051",
                                           grpc::InsecureChannelCredentials()));

    // Unary RPC
    std::string name = "Alice";
    std::string response = client.SayHelloUnary(name);
    std::cout << "Unary response: " << response << std::endl;

    // Client streaming RPC
    std::vector<std::string> names = {"Bob", "Charlie", "Dave"};
    response = client.SayHelloClientStreaming(names);
    std::cout << "Client streaming response: " << response << std::endl;

    // Server streaming RPC
    name = "Eve";
    std::cout << "Server streaming response:" << std::endl;
    client.SayHelloServerStreaming(name);

    // Bidirectional streaming RPC
    names = {"Frank", "Grace", "Henry"};
    std::cout << "Bidirectional streaming response:" << std::endl;
    client.SayHelloBidiStreaming(names);

    return 0;
}
