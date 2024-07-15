# Tcp Sockets {#examples-core-tcp-sockets}

@brief Using TCP sockets for communication between a server and a client.

@see Full source code [here](https://github.com/GameDevTecnico/cubos/tree/main/core/samples/net/tcp).

This example demonstrates how to set up a TCP server and client, exchanging data, using the **Cubos** engine.

The TCP sockets are wrapped around a @ref cubos::core::net::TcpListener and @ref cubos::core::net::TcpStream classes for a better API and simpler usage.

First, we need the network dependencies:

@snippet net/tcp/main.cpp Import headers

We'll also define some dummy constants for demonstration purposes:

@snippet net/tcp/main.cpp Define constants

Now, let's launch the "TCP server", which will be a @ref cubos::core::net::TcpListener waiting for a new connection, and quit after receveing a message from client via @ref cubos::core::net::TcpStream .

@snippet net/tcp/main.cpp Server function

Server is up and waiting, so let's create the client which will send a message to the server.

@snippet net/tcp/main.cpp Client function

To finish off, let's launch them in separate threads, because we are using blocking methods (such as `accept`).

@snippet net/tcp/main.cpp Main function

Output:

```
[20:36:53.918] [tcp_listener.cpp:67 listen] info: TCP listener socket '3' at address "127.0.0.1" with port '8080'
[20:36:53.919] [main.cpp:28 runServer] debug: Server is listening
[20:36:54.918] [tcp_listener.cpp:80 accept] info: Connecting stream to socket
[20:36:54.919] [tcp_stream.cpp:45 connect] info: New TCP stream at address "127.0.0.1" and port '8080'
[20:36:54.919] [tcp_stream.cpp:54 inner] info: New TCP stream at socket 4
[20:36:54.919] [tcp_stream.cpp:136 write] info: Sent TCP message
[20:36:54.919] [tcp_stream.cpp:111 read] info: Incoming TCP message
[20:36:54.920] [main.cpp:36 runServer] info: Received message: "Hello Cubos!" with size 12
[20:36:54.920] [tcp_stream.cpp:62 disconnect] warn: Closing TCP stream socket: '5'
[20:36:54.920] [tcp_listener.cpp:89 close] warn: Closing TCP listener socket: '3'
[20:36:54.921] [tcp_stream.cpp:62 disconnect] warn: Closing TCP stream socket: '4'
```