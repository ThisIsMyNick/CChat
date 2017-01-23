#define KEY_SIZE 32 // 256 bit key
#define IV_SIZE 16 // 128 bit iv
#define BUFFER_SIZE 1024

typedef unsigned char aes_t;

void generate_bytes(aes_t *buf);
int encrypt(char *plaintext, aes_t *key, aes_t *iv, aes_t *ciphertext);
int decrypt(aes_t *ciphertext, int ciphertext_len, aes_t *key, aes_t *iv, char *plaintext);
