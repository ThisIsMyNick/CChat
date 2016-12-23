#define BUFFER_SIZE 1024
#define KEY_SIZE 16

typedef unsigned char aes_t;

void generate_key(aes_t *buf);
aes_t *encrypt(aes_t *plaintext, aes_t *key);
aes_t *decrypt(aes_t *ciphertext, aes_t *key);
aes_t *to_unsigned(char *buf);
char *to_signed(aes_t *buf);
