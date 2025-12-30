//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////// * * * * * STEGANOGRAPHY * * * * * ///////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// ==================================================================================================================================================================== //
// PROJECT NAME : STEGANOGRAPHY
// NAME         : SHUBHAM S.
// 
//
// MODULE       : Adv. C - PROGRAMMING
// LANGUAGE     : C - PROGRAMMING
// ==================================================================================================================================================================== //

// ==================================================================== DESCRIPTION =================================================================================== //

/*
STEGANOGRAPHY IS THE TECHNIQUE OF CONCEALING SECRET DATA WITHIN A NORMAL-LOOKING CARRIER FILE, SUCH AS A BMP IMAGE, IN A WAY THAT MAKES THE PRESENCE OF THE DATA 
COMPLETELY UNNOTICEABLE. THIS PROJECT IMPLEMENTS IMAGE-BASED STEGANOGRAPHY USING ADVANCED C PROGRAMMING, WHERE A secret text file is embedded into a standard uncompressed 
BMP image file by modifying the least significant bits (LSBs) of pixel data. The encoding process starts by taking an original image like beautiful.bmp and hiding content 
from secret.txt inside it, generating a new image file stego.bmp or output.bmp. During encoding, details such as a magic string, secret file extension, file size, and 
actual content are embedded into the pixel data without visually altering the image. The decoding process reverses this operation by extracting hidden content from 
stego.bmp to regenerate the original secret file, such as output.txt. Required files include: a valid BMP image as source, a text file to hide, and optionally an output 
BMP or text file name. This tool demonstrates how information can be securely shared by hiding it inside media files, preserving both data privacy and visual integrity.
*/

// ==================================================================================================================================================================== //
// ==================================================== * * * * * MAIN FILE [test_encode.c] * * * * * ================================================================= //

/* ====================================================================== INCLUDES ==================================================================================== */

#include <stdio.h>   //Std Input/Output functions 
#include <string.h>  //Inbuilt String functions
#include "encode.h"  //Function declarations and structures for encoding logic
#include "types.h"   //Custom types like Status, OperationType
#include "common.h"  //Common definitions like OperationType enum
#include "decode.h"  //Function declarations and structures for decoding logic

/* ====================================================================== FUNCTION ==================================================================================== */

/*
First checks the type of operation user wants to execute (encode/decode)

argv -> Argument passed from command line (like "-e" or "-d")
-e >> endocing
-d >> decoding

*/

// Function to check if the operation is -e or -d
static OperationType check_operation_type(char *argv)
{
    // STEP 1: check if argv is "-e"
    if (strcmp(argv, "-e") == 0)
    {
        // STEP 2: if yes, return e_encode
        return e_encode;
    }
    // STEP 3: check if argv is "-d"
    else if (strcmp(argv, "-d") == 0)
    {
        // STEP 4: if yes, return e_decode
        return e_decode;
    }
    else
    {
        // STEP 5: neither -e nor -d, return unsupported
        return e_unsupported;
    }
}

/* ====================================================================== INT MAIN() ================================================================================== */

/*
Main entry point of the steganography application
It analyzes/reads the command line arguments, determines the operation type, and then calls either encoding or decoding logic

argc --> number of command line arguments
argv --> array of command line argument strings

return 0 for success, 1 for failure
*/

int main(int argc, char *argv[])
{
    // STEP 1: Validate argument count
    if (argc < 3)
    {
        printf("Usage:\n");
        printf("Encoding: ./steganography -e <input.bmp> <secret.txt> [output.bmp]\n");
        printf("Decoding: ./steganography -d <stego.bmp> [output.txt]\n");
        return 1;
    }

    // STEP 2: call the function to check operation type i.e. determine operation type (encode/decode)
    OperationType op_type = check_operation_type(argv[1]);

    /* ====================================================================== ENCODING MODE =========================================================================== */

    // STEP 3: if return value is e_encode, perform encoding
    if (op_type == e_encode)
    {
        printf("\033[0;33mENCODING MODE SELECTED\033[0m\n");  // Yellow text

        // STEP 4: validate and store input arguments in encInfo struct 
        EncodeInfo encInfo;

        // STEP 5: read and validate command-line arguments

        if (read_and_validate_encode_args(argv, &encInfo) == e_success)
        {
            printf("Reading and validation successful\n");

            // STEP 6: call the main do_encoding func
            if (do_encoding(&encInfo) == e_success)
            {
                printf("\033[0;32mEncoding completed successfully\033[0m\n");  // Green text
            }
            else
            {
                printf("\033[0;31mEncoding failed\033[0m\n");  // Red text
            }
        }
        else
        {
            printf("Validation of encoding arguments failed\n");
        }
    }

    /* ==================================================================== DECODING MODE ============================================================================= */

    else if (op_type == e_decode)
    {
        printf("\033[0;33mDECODING MODE SELECTED\033[0m\n");  // Yellow text

        DecodeInfo decInfo;

        if (read_and_validate_decode_args(argc, argv, &decInfo) == e_success)
        {
            printf("Arguments validated successfully for decoding\n");

            if (do_decoding(&decInfo) == e_success)
            {
                printf("\033[0;32mDecoding completed successfully\033[0m\n");  // Green text
            }
            else
            {
                printf("\033[0;31mDecoding failed\033[0m\n");  // Red text
            }
        }
        else
        {
            printf("Validation of decoding arguments failed\n");
        }
    }

    /* =================================================================== UNSUPPORTED MODE =========================================================================== */
    else
    {
        printf("Invalid option '%s'. Use -e for encoding or -d for decoding.\n", argv[1]);
        return 1;
    }
    return 0;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ========================================================================= END OF CODE ============================================================================== //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////