#include <stdio.h>
#include "../set_1.h"
#include <stdlib.h>
#include <string.h>

// Break repeating-key XOR
// It is officially on, now.
// This challenge isn't conceptually hard, but it involves actual error-prone coding. The other challenges in this set are there to bring you up to speed. This one is there to qualify you. If you can do this one, you're probably just fine up to Set 6.
// There's a file here (txt/challenge_6). It's been base64'd after being encrypted with repeating-key XOR.
// Decrypt it.
// Here's how:
//     Let KEYSIZE be the guessed length of the key; try values from 2 to (say) 40.
//     Write a function to compute the edit distance/Hamming distance between two strings. The Hamming distance is just the number of differing bits. The distance between:
//     this is a test
//     and
//     wokka wokka!!!
//     is 37. Make sure your code agrees before you proceed.
//     For each KEYSIZE, take the first KEYSIZE worth of bytes, and the second KEYSIZE worth of bytes, and find the edit distance between them. Normalize this result by dividing by KEYSIZE.
//     The KEYSIZE with the smallest normalized edit distance is probably the key. You could proceed perhaps with the smallest 2-3 KEYSIZE values. Or take 4 KEYSIZE blocks instead of 2 and average the distances.
//     Now that you probably know the KEYSIZE: break the ciphertext into blocks of KEYSIZE length.
//     Now transpose the blocks: make a block that is the first byte of every block, and a block that is the second byte of every block, and so on.
//     Solve each block as if it was single-character XOR. You already have code to do this.
//     For each block, the single-byte XOR key that produces the best looking histogram is the repeating-key XOR key byte for that block. Put them together and you have the key.
// This code is going to turn out to be surprisingly useful later on. Breaking repeating-key XOR ("Vigenere") statistically is obviously an academic exercise, a "Crypto 101" thing. But more people "know how" to break it than can actually break it, and a similar technique breaks something much more important.
// No, that's not a mistake.
// We get more tech support questions for this challenge than any of the other ones. We promise, there aren't any blatant errors in this text. In particular: the "wokka wokka!!!" edit distance really is 37.

const int indexbase[256] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 62, 63, 62, 62, 63, 52, 53, 54, 55,
                            56, 57, 58, 59, 60, 61, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 4, 5, 6,
                            7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 0,
                            0, 0, 0, 63, 0, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
                            41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51};

int main(void)
{
    // const char *test_1 = "this is a test";
    // const char *test_2 = "wokka wokka!!!";
    // size_t len = strlen(test_1);
    // unsigned int *bytes_1 = text_to_bytes(test_1);
    // unsigned int *bytes_2 = text_to_bytes(test_2);

    // int test = hamming_distance(bytes_1, bytes_2, len);
    // printf("%d", test);

    FILE *fptr;
    char line[256];

    fptr = fopen("./txt/challenge_6.txt", "r");
    if (fptr == NULL)
    {
        printf("Cannot open file \n");
        exit(0);
    }

    while (fgets(line, sizeof(line), fptr))
    {
        size_t len = strlen(line);
        printf("\nIn Base64: %s", line);

        printf("\nIn Hex: %s\n", base64_to_hex(line));
        unsigned int *one = str_to_base64bytes(line);
        unsigned int *two = base64bytes_to_hexbytes(one, len);
        for (int i = 0; i < len * 3 / 4; i++)
        {
            printf("%x-", two[i]);
        }
        // break;
    }

    fclose(fptr);
    return 0;
}