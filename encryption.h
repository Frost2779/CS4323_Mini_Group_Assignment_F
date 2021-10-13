#ifndef ENCRYPTION_H
#define ENCRYPTION_H

/*
 * Encrypts a string with the given key.
 *
 * Parameters:
 *     str -> the string to encrypt
 *     key -> the key to encrypt the string with
 * Returns:
 *     A char pointer to the first element of the string
 */
char* encrypt(const char* str, int key);

/*
 * Decrypts and encrypted string using the given key.
 *
 * Parameters:
 *     str -> the string to decrypt
 *     key -> the key to decrypt the string with
 * Returns:
 *     A char pointer to the first element of the string
 */
char* decrypt(const char* str, int key);

#endif //ENCRYPTION_H
