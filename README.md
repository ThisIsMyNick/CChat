# CChat
By Nobel Gautam and James Wang

## Requirements
Dependencies can be installed by running:

`$ sudo apt-get install libncurses-dev libssl-dev`

## Running
Running "make" will compile the project and create a "cchat" binary in the root of the repository.

`$ make`

Then to start a server:

`$ ./cchat -n servername`

To start a client:

`$ ./cchat -n clientname -s serverip`

For example, running a local server and client can be done as such:

`$ ./cchat -n servo`

Then on another terminal

`$ ./cchat -n cliento -c 127.0.0.1`
