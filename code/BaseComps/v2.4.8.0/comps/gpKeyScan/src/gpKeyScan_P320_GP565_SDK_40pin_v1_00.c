
/*
 *
 *   Key Scan for GP565 SDK 40 pin
 *
 *                ,               This software is owned by GreenPeak Technologies
 *                g               and protected under applicable copyright laws.
 *               ]&$              It is delivered under the terms of the license
 *               ;QW              and is intended and supplied for use solely and
 *               G##&             exclusively with products manufactured by
 *               N#&0,            GreenPeak Technologies.
 *              +Q*&##
 *              00#Q&&g
 *             ]M8  *&Q           THIS SOFTWARE IS PROVIDED IN AN "AS IS"
 *             #N'   Q0&          CONDITION. NO WARRANTIES, WHETHER EXPRESS,
 *            i0F j%  NN          IMPLIED OR STATUTORY, INCLUDING, BUT NOT
 *           ,&#  ##, "KA         LIMITED TO, IMPLIED WARRANTIES OF
 *           4N  NQ0N  0A         MERCHANTABILITY AND FITNESS FOR A
 *          2W',^^ `48  k#        PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 *         40f       ^6 [N        GREENPEAK TECHNOLOGIES B.V. SHALL NOT, IN ANY
 *        jB9         `, 0A       CIRCUMSTANCES, BE LIABLE FOR SPECIAL,
 *       ,&?             ]G       INCIDENTAL OR CONSEQUENTIAL DAMAGES,
 *      ,NF               48      FOR ANY REASON WHATSOEVER.
 *      EF                 @
 *     0!                         $Header: //depot/release/Embedded/BaseComps/v2.4.8.0/sw/comps/gpKeyScan/src/gpKeyScan_P320_GP565_SDK_40pin_v1_00.c#1 $
 *    M'   GreenPeak              $Change: 75614 $
 *   0'         Technologies      $DateTime: 2016/01/14 12:38:05 $
 *  F
 */
 
 
/*****************************************************************************
 *                    Definitions
 *****************************************************************************/

//Rows 
// Pin 31 - KEY_OUT2_a - GPIO10 
// Pin 32 - KEY_OUT3_a - GPIO11 
// Pin 33 - KEY_OUT4_a - GPIO12 
// Pin 34 - KEY_OUT5_a - GPIO13 
// Pin 35 - KEY_OUT6_a - GPIO14 
// Pin 36 - KEY_OUT7_a - GPIO15 
//Columns 
// Pin 22 - KEY_IN1_a - GPIO1 
// Pin 23 - KEY_IN2_a - GPIO2 
// Pin 24 - KEY_IN3_a - GPIO3 
// Pin 25 - KEY_IN4_a - GPIO4 
// Pin 26 - KEY_IN5_a - GPIO5 

#define HAL_KEY_MAPPING 0 //Mapping A

//Normal and debug masks
#define HAL_KEY_INPUT_MASK  (BM(1) | BM(2) | BM(3) | BM(4) | BM(5))
#define HAL_KEY_INPUT_MASK_SPI_ENABLED  (BM(1) | BM(2) | BM(3) | BM(4) | BM(5))
#define HAL_KEY_INPUT_MASK_UART_ENABLED (BM(1) | BM(2) | BM(3) | BM(4) | BM(5))

#define HAL_KEY_OUTPUT_MASK (BM(2) | BM(3) | BM(4) | BM(5) | BM(6) | BM(7))
#define HAL_KEY_OUTPUT_MASK_SPI_ENABLED  (BM(2) | BM(3) | BM(4) | BM(5) | BM(6) | BM(7))
#define HAL_KEY_OUTPUT_MASK_UART_ENABLED (BM(2) | BM(3) | BM(4) | BM(5) | BM(6) | BM(7))

//Used masks
#define KEY_INPUT_MASK  (HAL_KEY_INPUT_MASK  & (HAL_DEBUG_IS_SPI_ENABLED() ? (HAL_KEY_INPUT_MASK_SPI_ENABLED)  : 0xFF) & (HAL_DEBUG_IS_UART_ENABLED() ? HAL_KEY_INPUT_MASK_UART_ENABLED   : 0xFF))
#define KEY_OUTPUT_MASK (HAL_KEY_OUTPUT_MASK & (HAL_DEBUG_IS_SPI_ENABLED() ? (HAL_KEY_OUTPUT_MASK_SPI_ENABLED) : 0xFF) & (HAL_DEBUG_IS_UART_ENABLED() ? HAL_KEY_OUTPUT_MASK_UART_ENABLED  : 0xFF))

#define HAL_KEY_ROW_DRIVE_PERIOD             30UL //us

/*****************************************************************************
 *                    Static function definitions
 *****************************************************************************/

static inline void KeyScan_Init(void)
{
    GP_WB_WRITE_KEYPAD_SCAN_KEYP_SELECT_ALTERNATIVE_PIN_MAP(HAL_KEY_MAPPING);
    GP_WB_WRITE_KEYPAD_SCAN_ROW_DRIVE_PERIOD(HAL_KEY_ROW_DRIVE_PERIOD*4); //In 250ns

    /* Input mask */ 
    GP_WB_WRITE_KEYPAD_SCAN_MATRIX_SENSES(KEY_INPUT_MASK);
    /* Pull up inputs */ 
    //Disable SPI block ==> shared on  Pin 22 - KEY_IN1_a - GPIO1*/
    GP_WB_WRITE_MSI_SERIAL_ITF_SELECT(GP_WB_ENUM_SERIAL_ITF_SELECT_NO_INTERFACE); 
    GP_WB_WRITE_SPI_SL_PINMAP(GP_WB_ENUM_GENERIC_SINGLE_PINMAP_NOT_MAPPED);
    GP_WB_WRITE_IOB_GPIO_1_CFG(GP_WB_ENUM_GPIO_MODE_PULLUP);
    //Disable SPI block ==> shared on  Pin 23 - KEY_IN2_a - GPIO2*/
    GP_WB_WRITE_MSI_SERIAL_ITF_SELECT(GP_WB_ENUM_SERIAL_ITF_SELECT_NO_INTERFACE); 
    GP_WB_WRITE_SPI_SL_PINMAP(GP_WB_ENUM_GENERIC_SINGLE_PINMAP_NOT_MAPPED);
    GP_WB_WRITE_IOB_GPIO_2_CFG(GP_WB_ENUM_GPIO_MODE_PULLUP);
    //Disable SPI block ==> shared on  Pin 24 - KEY_IN3_a - GPIO3*/
    GP_WB_WRITE_MSI_SERIAL_ITF_SELECT(GP_WB_ENUM_SERIAL_ITF_SELECT_NO_INTERFACE); 
    GP_WB_WRITE_SPI_SL_PINMAP(GP_WB_ENUM_GENERIC_SINGLE_PINMAP_NOT_MAPPED);
    GP_WB_WRITE_IOB_GPIO_3_CFG(GP_WB_ENUM_GPIO_MODE_PULLUP);
    //Disable SPI block ==> shared on  Pin 25 - KEY_IN4_a - GPIO4*/
    GP_WB_WRITE_MSI_SERIAL_ITF_SELECT(GP_WB_ENUM_SERIAL_ITF_SELECT_NO_INTERFACE); 
    GP_WB_WRITE_SPI_SL_PINMAP(GP_WB_ENUM_GENERIC_SINGLE_PINMAP_NOT_MAPPED);
    GP_WB_WRITE_IOB_GPIO_4_CFG(GP_WB_ENUM_GPIO_MODE_PULLUP);
    GP_WB_WRITE_IOB_GPIO_5_CFG(GP_WB_ENUM_GPIO_MODE_PULLUP);

    /* Output mask */
    GP_WB_WRITE_KEYPAD_SCAN_MATRIX_SCANS(KEY_OUTPUT_MASK);
    /* Set outputs floating */
    GP_WB_WRITE_IOB_GPIO_10_CFG(GP_WB_ENUM_GPIO_MODE_FLOAT);
    GP_WB_WRITE_IOB_GPIO_11_CFG(GP_WB_ENUM_GPIO_MODE_FLOAT);
    GP_WB_WRITE_IOB_GPIO_12_CFG(GP_WB_ENUM_GPIO_MODE_FLOAT);
    GP_WB_WRITE_IOB_GPIO_13_CFG(GP_WB_ENUM_GPIO_MODE_FLOAT);
    GP_WB_WRITE_IOB_GPIO_14_CFG(GP_WB_ENUM_GPIO_MODE_FLOAT);
    GP_WB_WRITE_IOB_GPIO_15_CFG(GP_WB_ENUM_GPIO_MODE_FLOAT);
}
static inline void KeyScan_SetWakeupMode(UInt8 mode)
{
    GP_WB_WRITE_PMUD_WAKEUP_PIN_MODE_1(mode);
    GP_WB_WRITE_PMUD_WAKEUP_PIN_MODE_2(mode);
    GP_WB_WRITE_PMUD_WAKEUP_PIN_MODE_3(mode);
    GP_WB_WRITE_PMUD_WAKEUP_PIN_MODE_4(mode);
    GP_WB_WRITE_PMUD_WAKEUP_PIN_MODE_5(mode);
}

static inline UInt8 KeyScan_CompressRow(UInt8 row)
{
    UInt8 trans = 0x0;

    if(row & BM(1)) trans |= BM(0); //Pin 22 - KEY_IN1_a
    if(row & BM(2)) trans |= BM(1); //Pin 23 - KEY_IN2_a
    if(row & BM(3)) trans |= BM(2); //Pin 24 - KEY_IN3_a
    if(row & BM(4)) trans |= BM(3); //Pin 25 - KEY_IN4_a
    if(row & BM(5)) trans |= BM(4); //Pin 26 - KEY_IN5_a

    return trans;
}

static void KeyScan_GenerateScanIndication(UInt8* pStatus)
{
    //6 rows x 5 columns - 30 keys
    UInt8 compressed[4] = {0x0}; 
    UInt8 tempRow;

    tempRow = KeyScan_CompressRow(pStatus[2]); //5 columns
    compressed[0] |= tempRow & 0x1F; //bit 0-4
    tempRow = KeyScan_CompressRow(pStatus[3]); //5 columns
    compressed[0] |= (tempRow << 5) & 0xE0; //bit 5-9
    compressed[1] |= (tempRow >> 3) & 0x03;
    tempRow = KeyScan_CompressRow(pStatus[4]); //5 columns
    compressed[1] |= (tempRow << 2) & 0x7C; //bit 10-14
    tempRow = KeyScan_CompressRow(pStatus[5]); //5 columns
    compressed[1] |= (tempRow << 7) & 0x80; //bit 15-19
    compressed[2] |= (tempRow >> 1) & 0x0F;
    tempRow = KeyScan_CompressRow(pStatus[6]); //5 columns
    compressed[2] |= (tempRow << 4) & 0xF0; //bit 20-24
    compressed[3] |= (tempRow >> 4) & 0x01;
    tempRow = KeyScan_CompressRow(pStatus[7]); //5 columns
    compressed[3] |= (tempRow << 1) & 0x3E; //bit 25-29

    gpKeyScan_cbScanIndication(compressed);
}

static Bool KeyScan_WasIllegal(void)
{
    return false; //no overlapping pins between TWI and keyscan
}

