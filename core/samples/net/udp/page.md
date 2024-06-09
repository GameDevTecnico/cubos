# Udp Sockets {#examples-core-net-udp-sockets}

@brief Using UDP sockets for communication between a server and a client.

@see Full source code [here](https://github.com/GameDevTecnico/cubos/tree/main/core/samples/net/udp).

This example demonstrates how to set up a UDP server and client using the **Cubos** engine.

First, we need the network dependencies

@snippet net/udp/main.cpp Import header

We'll also define some dummy constants for demonstration purposes

@snippet net/udp/main.cpp Define constants

Now, let's launch the UDP server, which will wait for a message and stop once it receives it

@snippet net/udp/main.cpp Server function

Server is up, so let's create the UDP client which will send a message to the server

@snippet net/udp/main.cpp Client function

Pretty easy huh? To finish off, let's launch them in separate threads

@snippet net/udp/main.cpp Main function

Output:

```
[15:14:06.503] [main.cpp:25 runServer] info: Server is running and waiting for messages at port 8080
[15:14:07.503] [main.cpp:60 runClient] info: Message sent to server
[15:14:07.503] [main.cpp:34 runServer] info: Received message: "Hello, I'm a Cubos UDP client!" with size 30
```