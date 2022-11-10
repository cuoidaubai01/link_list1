#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "error.h"
#include "intel_hex.h"
/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define MAX_SIZE 521u
/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*******************************************************************************
 * Code
 ******************************************************************************/
/* run this program using the console pauser or add your own getch, system("pause") or input loop */
int main(int argc, char *argv[])
{
    /* poiter file */
    FILE *f;
    /* variable  check return of function hand_a_record*/
    bool ret = false;
    /* variable to count number of line*/
    uint8_t lineCount = 0;
    /*array buffer to store a line  */
    uint8_t buffer[MAX_SIZE];
    /* struct to store information record */
    intelHex_t recordInfor;
    /* array to store error code */
    errorCode_enum_t errorCode[5];

    printf("%s", errorCode);
    f = fopen(argv[1],"r+");
 //   f = fopen("intel_hex_1.txt", "r+");
    if (f == NULL)
    {
        printf("can't open file!");
    }
    else
    {
        printf("%-8s%-8s%-15s%-60s%-20s\n", "Line", "Type", "Address", "Data", "State");
        printf("------------------------------------------------------");
        printf("------------------------------------------------------\n");
        while (fgets(buffer, MAX_SIZE, (FILE *)f) != NULL)
        {
            lineCount++;

            ret = intel_hex_handle_a_record(buffer, &recordInfor, errorCode);
            if (ret == true)
            {
                intel_hex_show_record(buffer, &recordInfor, lineCount);
            }
            else
            {
                intel_hex_show_error(buffer, errorCode, lineCount);
            }
        }
    }
    fclose(f);
    return 0;
}
