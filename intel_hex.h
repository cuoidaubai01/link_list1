#ifndef _INTEL_HEX_H_
#define _INTEL_HEX_H_

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define MAX_SIZE_DATA 510u
/*******************************************************************************
 * Prototypes
 ******************************************************************************/
typedef struct intel_hex_t
{
    uint8_t recordType;
    uint32_t address;
    uint8_t data[MAX_SIZE_DATA];
    uint8_t byteCount;
} intelHex_t;

typedef enum stateCheck
{
    FIRST_CHARACTER = 0,
    CHECK_CHARACTER = 1,
    CHECK_BYTE_COUNT = 2,
    STORE_ADDRESS = 3,
    CHECK_RECORD_TYPE = 4,
    STORE_DATA = 5,
    CHECK_SUM = 6,
    DONE = 7,
} state_check_enum_t;

typedef enum errorCode
{
    ERROR_FIRST_CHARACTER = 0,
    ERROR_CHARACTER = 1,
    ERROR_BYTE_COUNT = 2,
    ERROR_RECORD_TYPE = 3,
    ERROR_CHECK_SUM = 4,
} errorCode_enum_t;
/*******************************************************************************
 * API
 ******************************************************************************/
/*!
 * @brief               :handle a line.
 * @param buffer        :buffer to check.
 * @param recordInfor   :struct store information(recordtype,address,data..).
 * @param errorCode     :array store error code.
 * @return              :bool.
 */
bool intel_hex_handle_a_record(uint8_t *buffer, intelHex_t *recordInfor, errorCode_enum_t *errorCode);

/**
 *
 * @brief               :print a line.
 * @param buffer        :buffer to print.
 * @param recordInfor   :struct store information(recordtype,address,data..).
 * @param lineCount     :number of line.
 * @return              :bool.
 */
void intel_hex_show_record(uint8_t *buffer, intelHex_t *recordInfor, uint8_t lineCount);

/**
 *
 * @brief               :print a line.
 * @param buffer        :buffer to print.
 * @param errorCode     :array error code.
 * @param lineCount     :number of line.
 * @return              :bool.
 */
void intel_hex_show_error(uint8_t *buffer, errorCode_enum_t *errorCode, uint8_t lineCount);

#endif /* _INTEL_HEX_H_ */
