//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ============================================================= * * * * * encode.h * * * * * ========================================================================= //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*

### USAGE OF encode.h FILE IN STEGANOGRAPHY PROJECT ?
--> THIS HEADER FILE DEFINES STRUCTURES AND FUNCTION PROTOTYPES NEEDED FOR ENCODING DATA INTO AN IMAGE USING LSB (LEAST SIGNIFICANT BIT) STEGANOGRAPHY. IT CONTAINS 
    MACROS, STRUCTURE DEFINITIONS, AND DECLARATIONS TO HANDLE SOURCE IMAGE, SECRET FILE, AND OUTPUT STEGO IMAGE. USEFUL FOR UNDERSTANDING HOW SECRET INFORMATION IS 
    HIDDEN INSIDE IMAGE FILES BY MANIPULATING IMAGE BYTES IN A CONTROLLED MANNER. DESIGNED FOR MODULARITY AND CLARITY IN IMPLEMENTING ENCODING LOGIC.

*/

// ==================================================================================================================================================================== //

#ifndef ENCODE_H
#define ENCODE_H

/* ====================================================================== INCLUDES ==================================================================================== */

#include<stdio.h>   //Inbuilt STD operations
#include "types.h"  // Contains user defined types
#include<string.h>  //string inbuilt func

/* ========================================================================== */
/* 
 * File: encode.h
 * Purpose: This header file contains structure definitions and function
 *          prototypes required for encoding secret data into an image using
 *          LSB (Least Significant Bit) steganography technique.
 */
/* ========================================================================== */

/* 
 * Structure to store information required for
 * encoding secret file to source Image
 * Info about output and intermediate data is
 * also stored
 */

/* ===================================================================== MACROS ======================================================================================= */

#define MAX_SECRET_BUF_SIZE 1
#define MAX_IMAGE_BUF_SIZE (MAX_SECRET_BUF_SIZE * 8)
#define MAX_FILE_SUFFIX 4

/* =============================================================== STRUCTURE DEFINITION =============================================================================== */

/*
 * Struct: EncodeInfo
 * Purpose: Stores all the metadata and file pointers required for encoding process
 * Members:
 *  - Source image info
 *  - Secret text file info
 *  - Output stego image info
 */
typedef struct _EncodeInfo
{
    /* --------------- Source Image info --------------- */
    char *src_image_fname; //Store address of src image filename
    FILE *fptr_src_image; //File pointer to src image

    //uint image_capacity;
    //uint bits_per_pixel;
    //char image_data[MAX_IMAGE_BUF_SIZE];

    /* --------------- Secret File Info --------------- */
    char *secret_fname; //Store address of secret filename
    FILE *fptr_secret; //Store address of secret file
    char extn_secret_file[MAX_FILE_SUFFIX]; //Store extension of secret file
    //char secret_data[MAX_SECRET_BUF_SIZE];
    long size_secret_file; //Size of the secret file in bytes

    /* --------------- Stego Image Info --------------- */
    char *stego_image_fname; //Pointer to output stego image filename
    FILE *fptr_stego_image; //File pointer to write the stego image

} EncodeInfo;

/* ================================================================= FUNCTION PROTOTYPES ============================================================================== */

/* --------------------- Encoding function prototype -------------------------*/

/* Check operation type */
//static OperationType check_operation_type(char *argv); //check user passing -e or -d

/* Read and validate Encode args from argv */
Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo);

/* Perform the encoding */
Status do_encoding(EncodeInfo *encInfo);

/* Get File pointers for i/p and o/p files */
Status open_files(EncodeInfo *encInfo);

/* check capacity */
Status check_capacity(EncodeInfo *encInfo);

/* Get image size */
uint get_image_size_for_bmp(FILE *fptr_image);

/* Get file size */
uint get_file_size(FILE *fptr);

/* Copy bmp image header */
Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image);

/* Store Magic String */
Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo);

/* Encode secret file extenstion */
Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo);

/* Encode secret file size */
Status encode_secret_file_size(long file_size, EncodeInfo *encInfo);

//func of extn size
Status encode_secret_extn_size(long extn_size, EncodeInfo *encInfo);

/* Encode secret file data*/
Status encode_secret_file_data(EncodeInfo *encInfo);

/* Encode function, which does the real encoding */
Status encode_data_to_image(const char *data, int size, FILE *fptr_src_image, FILE *fptr_stego_image);

/* Encode a byte into LSB of image data array */
Status encode_byte_to_lsb(char data, char *image_buffer);

Status encode_size_to_lsb(int data, char *image_buffer);

/* Copy remaining image bytes from src to stego image after encoding */
Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest);

#endif

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/* ============================================================================= END OF CODE ===========================================================================*/
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////