#include <openssl/aes.h>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <stdio.h>
#include <string.h>

#include "crypt.h"

/*
 * Fills *buf* with random bytes
 */
void generate_bytes(aes_t *buf)
{
    if (!RAND_bytes(buf, sizeof(buf)))
        fprintf(stderr, "[!] Could not generate key");
}

/*
 * Uses AES encryption to encrypt *plaintext* into *ciphertext*.
 *
 * Params:
 * char *plaintext: The plaintext to be encrypted
 * aes_t *key: The key used for the encryption
 * aes_t *iv: The initialization vector for the encryption
 * aes_t *ciphertext: Buffer used to store the encrypted data
 *
 * Returns the length of the final encrypted ciphertext. Meant to be passed as a parameter to decrypt()
 */
int encrypt(char *plaintext, aes_t *key, aes_t *iv, aes_t *ciphertext)
{
    EVP_CIPHER_CTX *ctx;
    int len = 0;
    int ciphertext_len = 0;
    int plaintext_len = strlen(plaintext);

    // Initialize the encryption context as AES 256 CBC mode
    ctx = EVP_CIPHER_CTX_new();
    EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv);

    // Initial encryption, adding necessary padding
    EVP_EncryptUpdate(ctx, ciphertext, &len, (aes_t *)plaintext, plaintext_len);
    ciphertext_len = len;

    // Encrypts the final data
    EVP_EncryptFinal_ex(ctx, ciphertext+len, &len);
    ciphertext_len += len;

    EVP_CIPHER_CTX_free(ctx);

    return ciphertext_len;
}

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
int decrypt(aes_t *ciphertext, int ciphertext_len, aes_t *key, aes_t *iv, char *plaintext)
{
    EVP_CIPHER_CTX *ctx;
    int len = 0;
    int plaintext_len = 0;

    ctx = EVP_CIPHER_CTX_new();

    EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv);

    EVP_DecryptUpdate(ctx, (aes_t *)plaintext, &len, ciphertext, ciphertext_len);
    plaintext_len = len;

    int status;
    status = EVP_DecryptFinal_ex(ctx, (aes_t *)plaintext + len, &len);
    EVP_CIPHER_CTX_free(ctx);

    if (status > 0)
    {
        plaintext_len += len;
        plaintext[plaintext_len] = 0;
        return plaintext_len;
    }
    return -1;
}
