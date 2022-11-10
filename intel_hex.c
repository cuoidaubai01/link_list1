#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "intel_hex.h"
#include "error.h"
#include "check.h"
/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/
static uint32_t addressBase;
static uint8_t previousRecordType;
static bool endOfFile = false;
/*******************************************************************************
 * Code
 ******************************************************************************/

bool intel_hex_handle_a_record(uint8_t *buffer, intelHex_t *recordInfor,
                                             errorCode_enum_t *errorCode)
{
    /* variable to return  */
    bool checkRecord = true;
    /* variable to count */
    uint16_t i;
    /* variable to store checksum */
    uint32_t checkSum = 0;
    /* variable to count error */
    uint8_t countError = 0;
    /*  variable to check state */
    state_check_enum_t state = FIRST_CHARACTER;

    /* reset array errorcode */
    for (i = 0; i < 5; i++)
    {
        errorCode[i] = 0xff;
    }
    /* reset array store data */
    for (i = 0; i < MAX_SIZE_DATA; i++)
    {
        recordInfor->data[i] = 0;
    }
    while (state != DONE && endOfFile != true)
    {
        switch (state)
        {
        /* check ':' */
        case FIRST_CHARACTER:
            if (check_first_character(buffer) == false)
            {
                errorCode[countError] = ERROR_FIRST_CHARACTER;
                countError++;
                state = DONE;
                checkRecord = false;
            }
            else
            {
                state = CHECK_CHARACTER;
            }

            break;
        /* check character 0-9 , A-F */
        case CHECK_CHARACTER:
            if (check_character(buffer) == false)
            {
                errorCode[countError] = ERROR_CHARACTER;
                countError++;
                state = DONE;
                checkRecord = false;
            }
            else
            {
                state = CHECK_BYTE_COUNT;
            }

            break;
        /* check byte count */
        case CHECK_BYTE_COUNT:
            recordInfor->byteCount = convert_char_to_hex(&buffer[1], 2);
            if (check_byte_count(buffer, recordInfor->byteCount) == false)
            {
                errorCode[countError] = ERROR_BYTE_COUNT;
                countError++;
                state = DONE;
                checkRecord = false;
            }
            else
            {
                state = STORE_ADDRESS;
            }
            break;
        /* store address to struct */
        case STORE_ADDRESS:
            recordInfor->address = convert_char_to_hex(&buffer[3], 4);
            state = CHECK_RECORD_TYPE;
            break;
            /* check record type and change address base */
        case CHECK_RECORD_TYPE:
            recordInfor->recordType = convert_char_to_hex(&buffer[7], 2);
            if (check_record_type(recordInfor->recordType) == false)
            {
                errorCode[countError] = ERROR_RECORD_TYPE;
                countError++;
                checkRecord = false;
            }
            switch (recordInfor->recordType)
            {
            case 00:
            /* recordtype is data */
            {

                if (previousRecordType == 2)
                {
                    recordInfor->address = addressBase << 8 | recordInfor->address;
                }
                else if (previousRecordType == 4)
                {
                    recordInfor->address = addressBase << 16 | recordInfor->address;
                }
            }
            break;
            case 01:
                /*recordtype is end of file */
                endOfFile = true;
                break;
            case 02:
               /* change previousRecordType =02 for following recordtype  */
                previousRecordType = 02;
                addressBase = convert_char_to_hex(&buffer[9], 4);
                break;
            case 03:
                /* record type = 03 rarely used*/
                break;
            case 04:
                /* change previousRecordType =04 for following recordtype  */
                previousRecordType = 04;
                addressBase = convert_char_to_hex(&buffer[9], 4);
                break;
            case 05:
                /* record type = 05  rarely used*/
                break;
            }
            state = STORE_DATA;
            break;
        case STORE_DATA:
            /*store data to struct.data  */
            for (i = 0; i < (recordInfor->byteCount) * 2; i++)
            {
                recordInfor->data[i] = buffer[i + 9];
            }
            state = CHECK_SUM;
            break;
        case CHECK_SUM:
            /*check Sum  */
            if (check_sum(buffer, recordInfor->byteCount) == false)
            {
                errorCode[countError] = ERROR_CHECK_SUM;
                countError++;
                checkRecord = false;
            }
            state = DONE;
            break;
        default:
            break;
        }
    }

    return checkRecord;
}

void intel_hex_show_record(uint8_t *buffer, intelHex_t *recordInfor, uint8_t lineCount)
{
    uint16_t i;

    if (endOfFile == true)
    {
        /* print end of file */
        printf("\n%-8d", lineCount);
        printf("%-8d", recordInfor->recordType);
        printf("%-15x", recordInfor->address);
        for (i = 0; buffer[i] != '\n'; i++)
        {
            printf("%c", buffer[i]);
        }
        printf("%-39c", ' ');
        printf("End of File ");
    }
    else
    {
        /* print information record if correct */
        printf("\n%-8d", lineCount);
        printf("%-8d", recordInfor->recordType);
        printf("%-15x", recordInfor->address);
        printf("%-50s", recordInfor->data);
        printf("%s", "correct\n");
    }
}

void intel_hex_show_error(uint8_t *buffer, errorCode_enum_t *errorCode, uint8_t lineCount)
{
    uint16_t i;
    /* print record and show error if incorrect */
    printf("\n%-31d", lineCount);
    for (i = 0; buffer[i] != '\0'; i++)
    {
        if (buffer[i] == '\n')
        {
            buffer[i] = '\0';
        }
    }
    printf("%-50s", buffer);
    for (i = 0; i < 5; i++)
    {
        switch (errorCode[i])
        {
        case ERROR_FIRST_CHARACTER:
            printf("Error ':' ");
            break;
        case ERROR_CHARACTER:
            printf("Error char  ");
            break;
        case ERROR_BYTE_COUNT:
            printf("Error byte count ");
            break;
        case ERROR_RECORD_TYPE:
            printf("Error record type ");
            break;
        case ERROR_CHECK_SUM:
            printf("Error check sum ");
            break;
        }
    }
    printf("\n");
}
