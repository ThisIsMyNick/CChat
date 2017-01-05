#include <openssl/aes.h>
#include <openssl/rand.h>
#include <stdio.h>
#include <string.h>

#include "crypt.h"

//#define DEBUG fprintf(stderr, "%s:%d", __FUNCTION__, __LINE__);

//Comment/uncomment the following for some C "fun"
//#define DEBUG  //Crashes
#define DEBUG printf("");  //Prevents crashes

typedef unsigned char aes_t;

void generate_bytes(aes_t *buf) {
    if (!RAND_bytes(buf, sizeof(buf))) {
        fprintf(stderr, "[!] Could not generate key");
    }
}

aes_t *encrypt(char *plaintext, aes_t *key, aes_t *iv) {
    DEBUG
    aes_t *ciphertext = (aes_t *) malloc(BUFFER_SIZE);
    DEBUG
    AES_KEY wctx;
    DEBUG
    AES_set_encrypt_key(key, KEY_SIZE * 8, &wctx);
    DEBUG
    AES_cbc_encrypt((aes_t *)plaintext, ciphertext, BUFFER_SIZE, &wctx, iv, AES_ENCRYPT);
    DEBUG
    memset(iv, 0x00, AES_BLOCK_SIZE);
    DEBUG
    return ciphertext;
}

char *decrypt(aes_t *ciphertext, aes_t *key, aes_t *iv) {
    aes_t *plaintext = (aes_t *) malloc(BUFFER_SIZE);
    AES_KEY wctx;
    AES_set_decrypt_key(key, KEY_SIZE * 8, &wctx);
    AES_cbc_encrypt(ciphertext, plaintext, BUFFER_SIZE, &wctx, iv, AES_DECRYPT);
    memset(iv, 0x00, AES_BLOCK_SIZE);
    return (char *) plaintext;
}
