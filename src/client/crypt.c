#include <string.h>
#include <stdio.h>
#include <openssl/rand.h>
#include <openssl/aes.h>

#include "crypt.h"

typedef unsigned char aes_t;

void generate_key(aes_t *buf) {
    if (!RAND_bytes(buf, sizeof(buf))) {
        fprintf(stderr, "[!] Could not generate key");
    }
}

aes_t *encrypt(aes_t *plaintext, aes_t *key) {
    aes_t *ciphertext = (aes_t *) malloc(BUFFER_SIZE);
    AES_KEY wctx;
    AES_set_encrypt_key(key, 128, &wctx);
    AES_encrypt(plaintext, ciphertext, &wctx);
    return ciphertext;
}

aes_t *decrypt(aes_t *ciphertext, aes_t *key) {
    aes_t *plaintext = (aes_t *) malloc(BUFFER_SIZE);
    AES_KEY wctx;
    AES_set_decrypt_key(key, 128, &wctx);
    AES_decrypt(ciphertext, plaintext, &wctx);
    return plaintext;
}
