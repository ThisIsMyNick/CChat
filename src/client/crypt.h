#define KEY_SIZE 32
#define BUFFER_SIZE 1024

typedef unsigned char aes_t;

void generate_bytes(aes_t *buf);
aes_t *encrypt(char *plaintext, aes_t *key, aes_t *iv);
char *decrypt(aes_t *ciphertext, aes_t *key, aes_t *iv);
aes_t *to_unsigned(char *buf);
char *to_signed(aes_t *buf);
