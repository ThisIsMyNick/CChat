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

`$ ./cchat -n clientname -c serverip`

For example, running a local server and client can be done as such:

`$ ./cchat -n servo`

Then on another terminal

`$ ./cchat -n cliento -c 127.0.0.1`

If you want to quit chatting, use the `/quit` command. Running `/quit` as the client will exit the program, but
running it as the server will terminate the existing connection and continue listening for more connections.

## Bugs
* After the 13th message, the screen temporarily clears itself until another message is receieved
* Multi-line messages work fine until they have to scroll past the screen. Since messages are not actually
broken up into separate lines, it takes a little bit for the message to scroll off the screen.

## Documentation
Documentation is taken directly from the source. Run `./generate_documentation.sh` to generate the documentation.
src/client/client.c:
```C
/*
 * Sends over the AES key and initialization vector to the server
 * to set up end-to-end encryption
 *
 * Params:
 * int sv_fd: File descriptor of the server to share the keys with
 */
static void exchange_keys(int sv_fd);

/*
 * Share client name with the server, and receive the server's name
 *
 * Params:
 * int cl_fd: File descriptor of the server to share the names with
 */
static void share_names(int cl_fd);

/*
 * Connects to the server at the address sv_nameaddr and shares keys and names
 *
 * Params:
 * char sv_nameaddr[64]: The address of the server, represented as a string
 *
 * Returns the file descriptor for the server socket
 */
static int sock_setup(char sv_nameaddr[64]);

/*
 * Worker function that constantly listens for a message from the server
 * Meant to be run in a thread
 *
 * Params:
 * void *args: Struct containing the arguments server file descriptor and the list of messages
 */
static void *input(void *args);

/*
 * Runs the client loop
 *
 * Params:
 * char sv_nameaddr[64]: Server address represented as a string
 * char nick[64]: The client nickname
 */
void client(char sv_nameaddr[64], char nick[64]);

```
src/client/crypt.c:
```C
/*
 * Fills *buf* with random bytes
 */
void generate_bytes(aes_t *buf);

/*
 * Uses AES encryption to encrypt *plaintext* into *ciphertext*.
 *
 * Params:
 * char *plaintext: The plaintext to be encrypted
 * aes_t *key: The key used for the encryption
 * aes_t *iv: The initialization vector for the encryption
 * aes_t *ciphertext: Buffer used to store the encrypted data
 *
 * Returns the length of the final encrypted ciphertext. Meant to be passed as a parameter to decrypt();
 */
int encrypt(char *plaintext, aes_t *key, aes_t *iv, aes_t *ciphertext);

/*
 * Uses AES decryption to decrypt *ciphertext* into *plaintext*.
 *
 * Params:
 * aes_t *ciphertext: The ciphertext to be decrypted
 * aes_t *key: The key that was used for the encryption
 * aes_t *iv: The initialization vector that was used for encryption
 * aes_t *plaintext: Buffer used to store the decrypted data
 *
 * Returns the length of the plaintext if the decryption was successful, -1 otherwise
 */
int decrypt(aes_t *ciphertext, int ciphertext_len, aes_t *key, aes_t *iv, char *plaintext);

```
src/client/main.c:
```C
/*
 * Prints the help string
 */
void help_exit();

/*
 * Top-level function that gets run by the users. Parses arguments and runs
 * the server or client accordingly.
 */
int main(int argc, char *argv[]);

```
src/client/message.c:
```C
/*
 * Add a message to the list of messages
 *
 * msg *m - Message list to modify (must be malloc'd);
 * int *m_size - Capacity of message list
 * int *m_curr - Current size of message list
 * char *user - Username of content author
 * char *content - Message content
 */
void add_msg(msgs_data *d, char *user, char *content);

/*
 * Frees the msgs_data struct and everything in it
 */
void free_msgs(msgs_data *d);

```
src/client/notify.c:
```C
/*
 * Displays a notification using notify-send and xprop
 * Only tested on Ubuntu
 */
void notify(msgs_data *d);

```
src/client/server.c:
```C
/*
 * Reads the AES key and initialization vector from the client
 * to set up end-to-end encryption
 */
static void exchange_keys(int cl_fd);

/*
 * Share server name with the client, and receive the client's name
 */
static void share_names(int cl_fd);

/*
 * Worker function that constantly listens for a message from a client
 * Meant to be run in a thread
 */
static void *input(void *args);

/*
 * Handles the server loop for a specific client
 *
 * Params:
 * int cl_fd: File descriptor of the client to be served
 */
void serve(int cl_fd);

/*
 * Runs the server loop
 *
 * Params:
 * char nick[64]: Server nickname
 */
void server(char nick[64]);

```
src/client/window.c:
```C
/*
 * Render the title bar
 */
void title_bar();

/*
 * Draw messages onto the screen
 *
 * Params:
 * msg *msg_list: List of messages to be drawn
 * int length: Number of messages to draw
 * int display_to: Index of the last message to draw
 */
void draw_messages(msg *msg_list, int length, int display_to);

/*
 * Get the maximum x coordinate of the screen
 */
int get_max_x();

/*
 * Draw the input prompt
 */
void draw_prompt();

/*
 * Clear the screen of all messages
 */
void clear_msgs();

/*
 * Update the screen, redrawing messages and the prompt
 */
void update_window(msgs_data *d);

/*
 * Get input from the prompt window
 *
 * Params:
 * char *dst: Char array used to store the input
 */
void get_input(char *dst);

/*
 * Creates the initial display of the window
 */
void init_window();

/*
 * Closes the screen and removes both the main and input windows
 */
void close_window();

```
