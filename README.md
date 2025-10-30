### Description

This project is a reimagining of a previous school project using C++, where I enhanced UDP to be reliable by implementing TCP-like features, with the goal of gaining a deeper understanding of network communications.

### How to run

To run the project we will need 2 different terminals, 1 for client and 1 for server.

We will need to compile for both client and server using these commands.

Server:
```
g++ -std=c++17 server.cpp -o server
```

Client:
```
g++ -std=c++17 client.cpp -o client
```

Now we can run the program starting with the server then the client by running this command

Server:
```
./server
```

Client:
```
./client
```
