//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ============================================================= * * * * * decode.c * * * * * ========================================================================= //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*

### USAGE OF decode.c FILE IN STEGANOGRAPHY PROJECT ?
--> THE DECODE.C FILE IS RESPONSIBLE FOR EXTRACTING HIDDEN SECRET DATA FROM A STEGO BMP IMAGE USING LSB (LEAST SIGNIFICANT BIT) DECODING. IT VALIDATES INPUT ARGUMENTS, 
    OPENS REQUIRED FILES, VERIFIES THE PRESENCE OF A MAGIC STRING, DECODES THE SECRET FILE’S EXTENSION AND CONTENT BIT-BY-BIT FROM IMAGE PIXELS, AND WRITES THE RECOVERED 
    DATA INTO A NEW OUTPUT FILE. THIS FORMS THE CORE LOGIC OF THE DECODING PHASE IN THE STEGANOGRAPHY PROJECT.

*/

/* ====================================================================== INCLUDES ==================================================================================== */

#include <stdio.h>     // Std inbuilt functions 
#include <stdlib.h>    // Std library files
#include <string.h>    // Inbuilt string functions
#include "types.h"     // Custom files like status, operation Type
#include "decode.h"    // Decode func declarations and decodeinfo struct
#include "common.h"    // Contains MAGIC_STRING macro

/* ================================================================= FUNCTION DEFINITIONS ============================================================================= */

/* Function to determine operation type*/
static OperationType check_operation_type(char *argv)
{
    if(strcmp(argv, "-d") == 0)
    {
        return e_decode;
    }
    else
    {
        return e_unsupported;
    }
}

/* Read and validate decode arguments from command line */
Status read_and_validate_decode_args(int argc, char *argv[], DecodeInfo *decInfo)
{
    // Check minimum number of arguments
    if (argc < 4)
    {
        printf("ERROR! Insufficient arguments for decoding.\n");
        printf("Usage: ./program -d stego_image.bmp output_file [original_stego.bmp]\n");
        return e_failure;
    }

    // Check for .bmp (stego image)
    if(strstr(argv[2], ".bmp") != NULL)
    {
        decInfo->stego_image_fname = argv[2];
    }
    else
    {
        printf("ERROR! Stego image must be a .bmp file\n");
        return e_failure;
    }

    // Set output filename (argv[3])
    decInfo->output_fname = argv[3];
    
    // Test if we can create the output file
    FILE *fp = fopen(decInfo->output_fname, "w");
    if (fp == NULL)
    {
        printf("Error: Cannot create output file %s\n", decInfo->output_fname);
        return e_failure;
    }
    fclose(fp);

    // Get secret file extension from output filename (not stego image)
    char *dot = strrchr(decInfo->output_fname, '.');
    if (dot != NULL)
    {
        strncpy(decInfo->extn_secret_file, dot, MAX_FILE_SUFFIX - 1);
        decInfo->extn_secret_file[MAX_FILE_SUFFIX - 1] = '\0'; // Ensure null termination
    }
    else
    {
        printf("Warning: Output file has no extension, using default .txt\n");
        strcpy(decInfo->extn_secret_file, ".txt"); // Default fallback
    }

    // Get stego image file size
    fp = fopen(decInfo->stego_image_fname, "rb");
    if (fp == NULL)
    {
        printf("Error: Cannot open stego image file %s\n", decInfo->stego_image_fname);
        return e_failure;
    }
    fseek(fp, 0, SEEK_END);
    decInfo->size_stego_image = ftell(fp);
    fclose(fp);

    // Validate minimum file size (BMP header + some data)
    if (decInfo->size_stego_image < 54)
    {
        printf("Error: Stego image file is too small to be a valid BMP\n");
        return e_failure;
    }

    return e_success;
}

/* Open files */
// Open stego image for reading and output file for writing
Status open_decode_files(DecodeInfo *decInfo)
{
    // Open stego image in binary read mode
    decInfo->fptr_stego_image = fopen(decInfo->stego_image_fname, "rb");
    if (decInfo->fptr_stego_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Cannot open stego image file %s\n", decInfo->stego_image_fname);
        return e_failure;
    }

    // Open output file in write mode
    decInfo->fptr_output = fopen(decInfo->output_fname, "w");
    if (decInfo->fptr_output == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Cannot open output file %s\n", decInfo->output_fname);
        fclose(decInfo->fptr_stego_image); // Clean up already opened file
        return e_failure;
    }

    return e_success;
}

/* Decode one byte from 8 LSBs of image data */
char decode_byte_from_lsb(char *image_buffer)
{
    char data = 0;
    for (int i = 0; i < 8; i++)
    {
        data = (data << 1) | (image_buffer[i] & 1);
    }
    return data;
}

/* Decode magic string and verify */
Status decode_magic_string(DecodeInfo *decInfo)
{
    char image_buffer[8];
    char magic_str[3]; // 2 characters + null

    // Decode 2 bytes (16 bits)
    for (int i = 0; i < 2; i++)
    {
        if (fread(image_buffer, sizeof(char), 8, decInfo->fptr_stego_image) != 8)
        {
            printf("ERROR! Cannot read enough data for magic string\n");
            return e_failure;
        }
        magic_str[i] = decode_byte_from_lsb(image_buffer);
    }
    magic_str[2] = '\0'; // NULL terminate

    // Compare decoded magic string with predefined one 
    if (strcmp(magic_str, MAGIC_STRING) == 0)
    {
        return e_success;
    }
    else
    {
        printf("ERROR! Magic string mismatch. Expected: %s, Got: %s\n", MAGIC_STRING, magic_str);
        return e_failure;
    }
}

/* Decode 4 bytes (32 bits) from image and return as integer */
int decode_size_from_lsb(FILE *fptr_stego_image)
{
    char image_buffer[32];
    if (fread(image_buffer, sizeof(char), 32, fptr_stego_image) != 32)
    {
        printf("ERROR! Cannot read size data from image\n");
        return -1;
    }

    int size = 0;
    // Extract 32 LSBs and form an integer
    for (int i = 0; i < 32; i++)
    {
        size = (size << 1) | (image_buffer[i] & 1);
    }
    return size;
}

/* Decode extension from image */
Status decode_secret_file_extn(DecodeInfo *decInfo)
{
    // Get extension length first
    int extn_size = decode_size_from_lsb(decInfo->fptr_stego_image);
    if (extn_size <= 0 || extn_size >= MAX_FILE_SUFFIX)
    {
        printf("ERROR! Invalid extension size: %d\n", extn_size);
        return e_failure;
    }

    char image_buffer[8];

    // Decode each character of extension
    for (int i = 0; i < extn_size; i++)
    {
        if (fread(image_buffer, sizeof(char), 8, decInfo->fptr_stego_image) != 8)
        {
            printf("ERROR! Cannot read extension data from image\n");
            return e_failure;
        }
        decInfo->extn_secret_file[i] = decode_byte_from_lsb(image_buffer);
    }
    // NULL terminate
    decInfo->extn_secret_file[extn_size] = '\0';

    printf("Decoded file extension: %s\n", decInfo->extn_secret_file);
    return e_success;
}

/* Decode secret file data */
Status decode_secret_file_data(DecodeInfo *decInfo)
{
    // Get file size
    int file_size = decode_size_from_lsb(decInfo->fptr_stego_image);
    if (file_size <= 0)
    {
        printf("ERROR! Invalid file size: %d\n", file_size);
        return e_failure;
    }

    printf("Decoding file of size: %d bytes\n", file_size);
    
    char image_buffer[8];

    // Decode each byte of secret and write to output
    for (int i = 0; i < file_size; i++)
    {
        if (fread(image_buffer, sizeof(char), 8, decInfo->fptr_stego_image) != 8)
        {
            printf("ERROR! Cannot read secret data from image at byte %d\n", i);
            return e_failure;
        }
        char ch = decode_byte_from_lsb(image_buffer);
        fputc(ch, decInfo->fptr_output); // Write character to output file
    }

    return e_success;
}

/* Do Decoding */
Status do_decoding(DecodeInfo *decInfo)
{
    printf("Starting decoding process...\n");

    // Open stego image and output file
    if (open_decode_files(decInfo) == e_failure)
    {
        printf("ERROR! Failed to open files\n");
        return e_failure;
    }

    // Skip BMP header (54 bytes)
    if (fseek(decInfo->fptr_stego_image, 54, SEEK_SET) != 0)
    {
        printf("ERROR! Cannot seek to data section in BMP file\n");
        fclose(decInfo->fptr_stego_image);
        fclose(decInfo->fptr_output);
        return e_failure;
    }

    // Validate magic string
    printf("Validating magic string...\n");
    if (decode_magic_string(decInfo) == e_failure)
    {
        printf("ERROR! Magic string validation failed. Not a valid stego image.\n");
        fclose(decInfo->fptr_stego_image);
        fclose(decInfo->fptr_output);
        return e_failure;
    }
    printf("Magic string validated successfully.\n");

    // Decode extension
    printf("Decoding file extension...\n");
    if (decode_secret_file_extn(decInfo) == e_failure)
    {
        printf("ERROR! Failed to decode secret file extension.\n");
        fclose(decInfo->fptr_stego_image);
        fclose(decInfo->fptr_output);
        return e_failure;
    }

    // Decode and write secret data to output file
    printf("Decoding secret file data...\n");
    if (decode_secret_file_data(decInfo) == e_failure)
    {
        printf("ERROR! Failed to decode secret file data.\n");
        fclose(decInfo->fptr_stego_image);
        fclose(decInfo->fptr_output);
        return e_failure;
    }

    // Close files
    fclose(decInfo->fptr_stego_image);
    fclose(decInfo->fptr_output);

    // SUCCESS message
    printf("Decoding completed successfully. Data written to: %s\n", decInfo->output_fname);
    return e_success;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/* ============================================================================= END OF CODE ===========================================================================*/
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////