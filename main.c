#include <3ds.h>
#include <stdio.h>
#include <stdint.h>
#include "sha256.h"
#include "miner.h"

static void print_hex(const uint8_t *data, size_t len)
{
    for (size_t i = 0; i < len; i++)
        printf("%02x", data[i]);

    printf("\n");
}

int main(void)
{
    gfxInitDefault();
    consoleInit(GFX_TOP, NULL);

    /*
     * Bitcoin Genesis Block header.
     *
     * This is the serialized 80-byte header exactly as it
     * should be passed to SHA-256.
     */
    uint8_t header[80] = {
    /* Version */
    0x01, 0x00, 0x00, 0x00,

    /* Previous block hash */
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,

    /* Merkle root */
    0x3b, 0xa3, 0xed, 0xfd,
    0x7a, 0x7b, 0x12, 0xb2,
    0x7a, 0xc7, 0x2c, 0x3e,
    0x67, 0x76, 0x8f, 0x61,
    0x7f, 0xc8, 0x1b, 0xc3,
    0x88, 0x8a, 0x51, 0x32,
    0x3a, 0x9f, 0xb8, 0xaa,
    0x4b, 0x1e, 0x5e, 0x4a,

    /* Timestamp */
    0x29, 0xab, 0x5f, 0x49,

    /* nBits */
    0xff, 0xff, 0x00, 0x1d,

    /* Nonce */
    0x1d, 0xac, 0x2b, 0x7c
    };

    uint8_t hash1[32];
    uint8_t hash2[32];

    printf("Bitcoin SHA-256d Test\n\n");

    printf("Header (%zu bytes):\n", sizeof(header));
    print_hex(header, sizeof(header));

    /*
     * First SHA-256
     */
    sha256(header, sizeof(header), hash1);

    printf("\nFirst SHA-256:\n");
    print_hex(hash1, sizeof(hash1));

    /*
     * Second SHA-256
     */
    sha256(hash1, sizeof(hash1), hash2);

    printf("\nSHA-256d (raw):\n");
    print_hex(hash2, sizeof(hash2));

    /*
     * Bitcoin displays block hashes with their bytes reversed.
     */
    printf("\nBitcoin block hash:\n");

    for (int i = 31; i >= 0; i--)
        printf("%02x", hash2[i]);

    printf("\n");

    printf("\nExpected:\n");
    printf("000000000019d6689c085ae165831e934ff763ae46a2a6c172b3f1b60a8ce26f\n");

    printf("\nPress A to start mining benchmark.\n");
    printf("Press START to exit.");

    while (aptMainLoop())
    {
        hidScanInput();

        u32 kDown = hidKeysDown();

        if (kDown & KEY_START)
            break;

        if (kDown & KEY_A)
        {
            /* miner_run() overwrites the header's nonce (and possibly
             * timestamp) fields, but that's fine - we don't need the
             * original values again after this point. */
            miner_run(header);

            consoleClear();
            printf("Press A to start mining benchmark.\n");
            printf("Press START to exit.");
        }

        gfxFlushBuffers();
        gfxSwapBuffers();
        gspWaitForVBlank();
    }

    gfxExit();

    return 0;
}
