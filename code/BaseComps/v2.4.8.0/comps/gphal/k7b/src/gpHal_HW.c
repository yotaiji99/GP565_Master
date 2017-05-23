/*
 * Copyright (c) 2014, GreenPeak Technologies
 *
 * gphal_HW.c
 *
 *  The file gpHal.h contains functions to access HW registers.
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
 *     0!                         $Header: //depot/release/Embedded/BaseComps/v2.4.8.0/sw/comps/gphal/k7b/src/gpHal_HW.c#1 $
 *    M'   GreenPeak              $Change: 75614 $
 *   0'         Technologies      $DateTime: 2016/01/14 12:38:05 $
 *  F
 */

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

//GP hardware dependent register definitions
#include "gpHal.h"          //Containing all uC dependent implementations

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/
#define GP_COMPONENT_ID GP_COMPONENT_ID_GPHAL

/*****************************************************************************
 *                   Functional Macro Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Static Data Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Inline Function Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Static Function Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/

//-------------------------------------------------------------------------------------------------------
//  GENERAL FUNCTIONS
//-------------------------------------------------------------------------------------------------------

void gpHal_WriteReg(gpHal_Address_t Register, UInt8 Data)
{
    GP_HAL_WRITE_REG(Register, Data);
}

UInt8 gpHal_ReadReg(gpHal_Address_t Register)
{
    return(GP_HAL_READ_REG(Register));
}

void gpHal_ReadModifyWriteReg(gpHal_Address_t Register, UInt8 Mask, UInt8 Data)
{
    GP_HAL_READMODIFYWRITE_REG(Register,Mask,Data);
}

void gpHal_ReadRegs(gpHal_Address_t Address, void* pBuffer, UInt8 Length)
{
    GP_HAL_READ_BYTE_STREAM(Address, pBuffer, Length );
}

void gpHal_WriteRegs(gpHal_Address_t Address, void* pBuffer, UInt8 Length)
{
    GP_HAL_WRITE_BYTE_STREAM(Address, pBuffer, Length );
}
