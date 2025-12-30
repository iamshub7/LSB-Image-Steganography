//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// =============================================================== * * * * * types.h * * * * * ======================================================================== //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*

### USAGE OF types.h FILE IN STEGANOGRAPHY PROJECT ?
->> THIS HEADER FILE DEFINES USER-DEFINED DATA TYPES USED THROUGHOUT THE STEGANOGRAPHY PROJECT. IT INCLUDES ENUMS FOR FUNCTION STATUS AND OPERATION MODES (ENCODE/DECODE),
    ENSURING BETTER READABILITY, TYPE SAFETY, AND MODULAR DESIGN.

*/

// ==================================================================================================================================================================== //

#ifndef TYPES_H
#define TYPES_H

/* User defined types */
typedef unsigned int uint;

/* Status will be used in fn. return type */
typedef enum
{
    e_success, // Function executed successfully
    e_failure  // Function encountered an error
} Status;

//enum to represent the operation mode selected by user
typedef enum
{
    e_encode,       //return 0 , Encoding operation ->> (-e)
    e_decode,       //return 1 , Decoding operation ->> (-d)
    e_unsupported   //return 2 //Invalid operation (neither -e or -d)
} OperationType;

#endif

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/* ========================================================================= END OF CODE ============================================================================== */
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////