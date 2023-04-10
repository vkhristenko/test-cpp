#include <grpcpp/grpcpp.h>

#include <iostream>
#include <memory>
#include <string>

#include "protos/grpc_dummy/dummy_0.grpc.pb.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;
using protos::grpc_dummy::DummyService;
using protos::grpc_dummy::HelloRequest;
using protos::grpc_dummy::HelloResponse;

// Implementation of the DummyService gRPC service
class DummyServiceImpl final : public DummyService::Service {
public:
    // Unary RPC method implementation
    Status SayHelloUnary(ServerContext* context, const HelloRequest* request,
                         HelloResponse* response) override {
        std::string message = "Hello, " + request->name();
        response->set_message(message);
        return Status::OK;
    }

    // Client streaming RPC method implementation
    Status SayHelloClientStreaming(ServerContext* context,
                                   grpc::ServerReader<HelloRequest>* reader,
                                   HelloResponse* response) override {
        std::string names;
        HelloRequest request;
        while (reader->Read(&request)) {
            names += request.name() + ", ";
        }
        if (!names.empty()) {
            names.erase(names.length() - 2);
            response->set_message("Hello, " + names);
        }
        return Status::OK;
    }

    // Server streaming RPC method implementation
    Status SayHelloServerStreaming(
        ServerContext* context, const HelloRequest* request,
        grpc::ServerWriter<HelloResponse>* writer) override {
        for (int i = 0; i < 3; i++) {
            std::string message = "Hello, " + request->name() + " (" +
                                  std::to_string(i + 1) + ")";
            HelloResponse response;
            response.set_message(message);
            writer->Write(response);
        }
        return Status::OK;
    }

    // Bidirectional streaming RPC method implementation
    Status SayHelloBidiStreaming(
        ServerContext* context,
        grpc::ServerReaderWriter<HelloResponse, HelloRequest>* stream)
        override {
        HelloRequest request;
        while (stream->Read(&request)) {
            std::string message = "Hello, " + request.name();
            HelloResponse response;
            response.set_message(message);
            stream->Write(response);
        }
        return Status::OK;
    }
};

void RunServer() {
    std::string server_address("0.0.0.0:50051");
    DummyServiceImpl service;

    ServerBuilder builder;
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    builder.RegisterService(&service);

    std::unique_ptr<Server> server(builder.BuildAndStart());
    std::cout << "Server listening on " << server_address << std::endl;
    server->Wait();
}

int main(int argc, char** argv) {
    RunServer();
    return 0;
}
