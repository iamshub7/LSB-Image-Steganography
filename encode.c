//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ============================================================= * * * * * encode.c * * * * * ========================================================================= //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*

### USAGE OF encode.c in STEGANOGRAPHY PROJCET ?
--> THIS PROGRAM EXPLAINS BASIC CONCEPTS OF POINTERS AND ARRAYS IN C. IT SHOWS HOW POINTERS ARE INITIALIZED, USED WITH ARRAYS, AND HOW POINTER ARITHMETIC WORKS. USEFUL 
    FOR BEGINNERS TO UNDERSTAND HOW ARRAYS AND POINTERS ARE CLOSELY RELATED.

*/

/* ======================================================================= INCLUDES =================================================================================== */

#include <stdio.h>   //Std inbuilt functions
#include "encode.h"  //Encoding function declarations and struct
#include "types.h"   //Custom types like Status and Operation Type
#include "common.h"  //Magic string macro used for encoding check

/* ================================================================= FUNCTION DEFINITIONS ============================================================================= */

/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */

uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;
    // Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width (an int) from BMP Header
    fread(&width, sizeof(int), 1, fptr_image);
    printf("width = %u\n", width);

    // Read the height (an int) from BMP Header
    fread(&height, sizeof(int), 1, fptr_image);
    printf("height = %u\n", height);

    // Each pixel = 3 bytes (RGB), return total image capacity in bytes
    return width * height * 3;
}

/* 
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */

Status open_files(EncodeInfo *encInfo)
{
    // Open Src Image file for reading
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");
    // Do Error handling
    if (encInfo->fptr_src_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);

        return e_failure;
    }

    // Open Secret file for reading
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
    // Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);

        return e_failure;
    }

    // Open Stego Image file for writing
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");
    // Do Error handling
    if (encInfo->fptr_stego_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);

        return e_failure;
    }

    // No failure return e_success
    return e_success;
}

//Validates command line arguments and fills encInfo structure
Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
    //FOR SOURCE FILE

    //STEP 1 : Check if argv[2] is  .bmp file or not, if YES GOTO STEP 2, if NOT GOTO STEP 3
    if (strstr(argv[2], ".bmp") != NULL)
    {
        //STEP 2 : Store the src_image name in encInfo->src_image_fname (storing src image filename address here[i.e. a char pointer])
        encInfo->src_image_fname = argv[2];
    }
    else
    {
        //STEP 3 : Print error msg like (please pass .bmp file) and return e_failure
        printf("Error: Source image must be a .bmp file.\n");
        return e_failure;
    }

    //FOR SECRET FILE 
    //STEP 4 : Check if the argv[3] is .txt file, if YES GOTO STEP 5, if NOT GOTO STEP 6
    if (strstr(argv[3], ".txt") != NULL)
    {
        //STEP 5 : Store the secret_file name in secret_fname and store the extension in extn_sec_file
        encInfo->secret_fname = argv[3];

        // Extract extension from filename and copy into extn_secret_file
        char *extn = strrchr(argv[3], '.'); // points to ".txt"
        if (extn != NULL)
        {
            strcpy(encInfo->extn_secret_file, extn + 1); // skip the dot
        }
        else
        {
            printf("Error: Failed to extract extension from secret file.\n");
            return e_failure;
        }
    }
    else
    {
        //STEP 6 : Print error msg and return e_failure (no need to continue)
        printf("Error: Secret file must be a .txt file.\n");
        return e_failure;
    }

    //FOR OPTIONAL FILE
    //STEP 7 : Check if argv[4] is passed or NOT, if YES GOTO STEP 8, if NO, GOTO STEP 11
    if (argv[4] != NULL)
    {
        //STEP 8 : Check the file is .bmp or NOT, if YES GOTO STEP 9, if NOT GOTO STEP 10
        if (strstr(argv[4], ".bmp") != NULL)
        {
            //STEP 9 :  Store the file name in stego_image_fname
            encInfo->stego_image_fname = argv[4];
        }
        else
        {
            //STEP 10 : Print error msg and return e_failure
            printf("Error: Stego image file must be a .bmp file.\n");
            return e_failure;
        }
    }
    else
    {
        //STEP 11 : Print the msg and store the default filename[stego.bmp] in a stego_image_fname
        printf("Info: Output stego image not specified. Defaulting to stego.bmp\n");
        encInfo->stego_image_fname = "stego.bmp";
    }
    //STEP 12 : Return e_success
    return e_success;
}

//Get size of file in bytes
uint get_file_size(FILE *fptr)
{
    // find size of secret file 

    // STEP 1 : Move the file pointer to the end of the file using fseek
    fseek(fptr, 0, SEEK_END);

    // STEP 2 : Use ftell to get the position (i.e. total file size in bytes)
    uint size = ftell(fptr);

    // Optional: Reset file pointer back to beginning (in case needed again)
    rewind(fptr);

    // Return the calculated file size
    return size;
}

//Check if the image has enough capacity to store secret data
Status check_capacity(EncodeInfo *encInfo)
{
    // Get the total number of bytes available in the image for encoding
    uint image_capacity = get_image_size_for_bmp(encInfo->fptr_src_image);

    // Get the size of the secret file (in bytes) and store it in the struct
    encInfo -> size_secret_file = get_file_size(encInfo->fptr_secret);

    // Check if the image has enough capacity to store:
    // - MAGIC STRING length in bits
    // - Extension size (32 bits)
    // - Extension characters in bits
    // - Secret file size (32 bits)
    // - Actual secret data in bits

    //Required space = header + magic string + extn size + extn data + file size + secret
    if(image_capacity > 54 + (strlen(MAGIC_STRING) * 8) + 32 + strlen(encInfo -> extn_secret_file) * 8 + 32 + (encInfo -> size_secret_file * 8))
    {
        //if enough capacity
        return e_success;
    }
    else
    {
        //if not enough capacity
        return e_failure;
    }
}

//Embed a byte into 8 bytes of image using LSB technique
Status encode_byte_to_lsb(char data, char *image_buffer)
{
    // Encode the data byte into LSBs of the 8 bytes
    for (int i = 0; i < 8; i++)
    {
        // Clear the LSB and set it to the corresponding bit of data
        image_buffer[i] = (image_buffer[i] & 0xFE) | ((data >> (7 - i)) & 1);
    }
    return e_success;
}

//To encode extn size and file size call this func [also change iteration as of for 32 i.e. size]
Status encode_size_to_lsb(int data, char *image_buffer)
{
    // Loop through 32 bits of the integer (from MSB to LSB)
    for (int i = 0; i < 32; i++)
    {
        // STEP 1 : Extract the i-th bit from MSB side
        char bit = (data >> (31 - i)) & 1;

        // STEP 2 : Clear the LSB of current image byte
        image_buffer[i] = image_buffer[i] & 0xFE;

        // STEP 3 : Set the LSB to the extracted bit
        image_buffer[i] = image_buffer[i] | bit;
    }
    return e_success;
}

//Encode the predefined magic string
Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo)
{
   return encode_data_to_image(magic_string, strlen(magic_string), encInfo->fptr_src_image, encInfo->fptr_stego_image);    
}

// common func used for magic string, extn, file data
Status encode_data_to_image(const char *data, int size, FILE *fptr_src_image, FILE *fptr_stego_image)
{
    char image_buffer[8];

    // Loop through each byte of data to encode
    for (int i = 0; i < size; i++)
    {
        // STEP 1 : read 8 bytes from the source image
        fread(image_buffer, sizeof(char), 8, fptr_src_image);

        // STEP 2 : encode 1 byte of secret data into these 8 bytes
        encode_byte_to_lsb(data[i], image_buffer );

        // STEP 3 : write the modified 8 bytes to the stego image
        fwrite(image_buffer, sizeof(char), 8, fptr_stego_image);
    }
    return e_success;
}

//Copy original 54 byte BMP header to stego image 
Status copy_bmp_header(FILE *src, FILE *dest)
{
    // Move file pointer of source image to the beginning
    rewind(src); 
    
    char buffer[54];  // BMP header size is always 54 bytes

    // Read 54 bytes from the source BMP into the buffer
    fread(buffer, 54, 1, src);  // Read once, 54 bytes

    // Write the same 54 bytes to the destination (stego image)
    fwrite(buffer, 54, 1, dest);

    return e_success;
}

//Encode the extension length (3 for txt, etc) into 32 bits
Status encode_secret_extn_size(long extn_size, EncodeInfo *encInfo)
{
    char image_buffer[32];

    // Read 32 bytes from the source image
    fread(image_buffer, sizeof(char), 32, encInfo->fptr_src_image);

    // Call encode_size_to_lsb to embed the extension size into image_buffer
    encode_size_to_lsb(extn_size, image_buffer);

    // Write the modified 32 bytes into the stego image
    fwrite(image_buffer, sizeof(char), 32, encInfo->fptr_stego_image);

    return e_success;
}

//Encode actual secret file size into 32 bits
Status encode_secret_file_size(long file_size, EncodeInfo *encInfo)
{
    char image_buffer[32];

    // Read 32 bytes from the source image
    fread(image_buffer, sizeof(char), 32, encInfo->fptr_src_image);

    // Call encode_size_to_lsb to embed the secret file size
    encode_size_to_lsb(file_size, image_buffer);

    // Write the modified 32 bytes into the stego image
    fwrite(image_buffer, sizeof(char), 32, encInfo->fptr_stego_image);

    return e_success;
}


// generic func
Status encode_secret_file_extn(const char *extn, EncodeInfo *encInfo)
{
    return encode_data_to_image(extn, strlen(extn), encInfo->fptr_src_image, encInfo->fptr_stego_image);
}

//Read entire secret file and encode its content
Status encode_secret_file_data(EncodeInfo *encInfo)
{
    char ch;
    while (fread(&ch, 1, 1, encInfo->fptr_secret))
    {
        char image_buffer[8];
        
        // Read 8 bytes from source image
        fread(image_buffer, sizeof(char), 8, encInfo->fptr_src_image);
        
        // Encode the byte into the buffer
        if (encode_byte_to_lsb(ch, image_buffer) == e_failure)
        {
            return e_failure;
        }
        
        // Write the modified buffer to stego image
        fwrite(image_buffer, sizeof(char), 8, encInfo->fptr_stego_image);
    }
    return e_success;
}

//Copy rest of the image (after encoding) as is
Status copy_remaining_img_data(FILE * src, FILE * dest)
{
    char ch;

    // Copy remaining bytes from source image to destination image
    while(fread(&ch, 1, 1, src))
    {
        fwrite(&ch, 1, 1, dest);
    }
    return e_success;
}


//Master func to perform complete encoding process
Status do_encoding(EncodeInfo *encInfo)
{
    // Step 1: Open files
    if (open_files(encInfo) == e_failure)
    {
        printf("Error: Unable to open required files.\n");
        return e_failure;
    }
    else
    {
        printf("Files opened successfully.\n");
    }

    // Step 2: Check capacity of image
    if (check_capacity(encInfo) == e_failure)
    {
        printf("Error: Insufficient image capacity.\n");
        return e_failure;
    }
    else
    {
        printf("Image has sufficient capacity.\n");
    }

    // Step 3: Copy BMP header from source to stego image
    if (copy_bmp_header(encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_failure)
    {
        printf("Error: Failed to copy BMP header.\n");
        return e_failure;
    }

    // Step 4: Encode magic string
    if (encode_magic_string(MAGIC_STRING, encInfo) == e_failure)
    {
        printf("Error: Failed to encode magic string.\n");
        return e_failure;
    }

    // Step 5: Encode secret file extension size
    if (encode_secret_extn_size(strlen(encInfo->extn_secret_file), encInfo) == e_failure)
    {
        printf("Error: Failed to encode secret file extension size.\n");
        return e_failure;
    }

    // Step 6: Encode secret file extension
    if (encode_data_to_image(encInfo->extn_secret_file, strlen(encInfo->extn_secret_file),
                            encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_failure)
    {
        printf("Error: Failed to encode secret file extension.\n");
        return e_failure;
    }

    // Step 7: Encode size of secret file
    if (encode_secret_file_size(encInfo->size_secret_file, encInfo) == e_failure)
    {
        printf("Error: Failed to encode secret file size.\n");
        return e_failure;
    }

    // Step 8: Encode the actual secret file content
    if (encode_secret_file_data(encInfo) == e_failure)
    {
        printf("Error: Failed to encode secret file data.\n");
        return e_failure;
    }

    // Step 9: Copy remaining image data
    if (copy_remaining_img_data(encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_failure)
    {
        printf("Error: Failed to copy remaining image data.\n"); 
        return e_failure;
    }

    printf("Encoding completed successfully.\n");
    return e_success;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/* ========================================================================= END OF CODE ============================================================================== */
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////