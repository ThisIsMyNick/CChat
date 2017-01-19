#include <openssl/aes.h>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <stdio.h>
#include <string.h>

#include "crypt.h"

typedef unsigned char aes_t;

void generate_bytes(aes_t *buf) {
    if (!RAND_bytes(buf, sizeof(buf))) {
        fprintf(stderr, "[!] Could not generate key");
    }
}

int encrypt(aes_t *plaintext, aes_t *key, aes_t *iv, aes_t *ciphertext) {
    EVP_CIPHER_CTX *ctx;
    int len = 0;
    int ciphertext_len = 0;
    int plaintext_len = strlen(plaintext);

    ctx = EVP_CIPHER_CTX_new();

    EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv);

    EVP_EncryptUpdate(ctx, ciphertext, &len, plaintext, plaintext_len);
    ciphertext_len = len;

    EVP_EncryptFinal_ex(ctx, ciphertext+len, &len);
    ciphertext_len += len;

    EVP_CIPHER_CTX_free(ctx);

    return ciphertext_len;
}

int decrypt(aes_t *ciphertext, int ciphertext_len, aes_t *key, aes_t *iv, aes_t *plaintext) {
    EVP_CIPHER_CTX *ctx;
    int len = 0;
    int plaintext_len = 0;

    ctx = EVP_CIPHER_CTX_new();

    EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv);

    EVP_DecryptUpdate(ctx, plaintext, &len, ciphertext, ciphertext_len);
    plaintext_len = len;

    int status;
    status = EVP_DecryptFinal_ex(ctx, plaintext + len, &len);
    EVP_CIPHER_CTX_free(ctx);

    if (status > 0) {
        plaintext_len += len;
        plaintext[plaintext_len] = 0;
        return plaintext_len;
    }
    return -1;
}
