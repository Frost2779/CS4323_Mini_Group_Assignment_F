#ifndef UNTITLED_ENCRYPTION_H
#define UNTITLED_ENCRYPTION_H

/*
 * Encrypts a string with the given key.
 *
 * Paramters:
 *     str -> the string to encrypt
 *     key ->
 * Returns:
 *     A char pointer to the first element of the string
 */
char* encrypt(const char* str, int key);
/*
 * Decrypts and encrypted string using the given key.
 *
 * Paramters:
 *     length -> the desired length of the string
 * Returns:
 *     A char pointer to the first element of the string
 */
char* decrypt(const char* str, int key);

#endif
