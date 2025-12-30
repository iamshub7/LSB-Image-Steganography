//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ============================================================= * * * * * decode.h * * * * * ========================================================================= //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*

### USAGE OF decode.h FILE IN STEGANOGRAPHY PROJECT ?
--> DECODE.H IS THE HEADER FILE FOR THE DECODING PHASE OF THE STEGANOGRAPHY PROJECT. IT DECLARES THE DECODEINFO STRUCTURE, WHICH HOLDS ALL NECESSARY INFORMATION FOR 
    DECODING HIDDEN DATA FROM A STEGO IMAGE, SUCH AS FILE POINTERS, FILE NAMES, AND EXTRACTED FILE EXTENSION. IT ALSO CONTAINS FUNCTION PROTOTYPES FOR DECODING THE MAGIC 
    STRING, SECRET FILE EXTENSION, FILE DATA, AND HELPER FUNCTIONS LIKE DECODING A BYTE OR SIZE FROM LSBS, ENSURING THE DECODE.C FILE HAS ORGANIZED ACCESS TO ALL 
    DECODING LOGIC.

*/

// ==================================================================================================================================================================== //

#ifndef DECODE_H
#define DECODE_H

/* ======================================================================= INCLUDES ==================================================================================== */

#include <stdio.h>
#include "types.h"

#define MAX_FILE_SUFFIX 4

/* ======================================================================= STRUCTURE ================================================================================== */

/* Structure to store information required for decoding */
typedef struct _DecodeInfo
{
    /* Stego Image Info */
    char *stego_image_fname;
    FILE *fptr_stego_image;
    long size_stego_image;

    /* Output File Info */
    char *output_fname;
    FILE *fptr_output;
    char extn_secret_file[MAX_FILE_SUFFIX];

} DecodeInfo;

/* ================================================================= FUNCTION DEFINITIONS ============================================================================= */

/* Function Prototypes */
Status read_and_validate_decode_args(int argc, char *argv[], DecodeInfo *decInfo);
Status open_decode_files(DecodeInfo *decInfo);
Status decode_magic_string(DecodeInfo *decInfo);
Status decode_secret_file_extn(DecodeInfo *decInfo);
Status decode_secret_file_data(DecodeInfo *decInfo);
Status do_decoding(DecodeInfo *decInfo);

/* Helper functions */
char decode_byte_from_lsb(char *image_buffer);
int decode_size_from_lsb(FILE *fptr_stego_image);

#endif

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/* ==================================================================== END OF CODE ====================================================================================*/
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////