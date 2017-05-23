/*
 * Copyright (c) 2015, GreenPeak Technologies
 * 
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
 *     0!                         $Header: //depot/release/Embedded/BaseComps/v2.4.8.0/sw/comps/gpPd/src/gpPd_common.c#1 $
 *    M'   GreenPeak              $Change: 75614 $
 *   0'         Technologies      $DateTime: 2016/01/14 12:38:05 $
 *  F
 */

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Static Function Definitions
 *****************************************************************************/
 
INLINE static void Pd_AppendWithUpdate(gpPd_Loh_t *pPdLoh, UInt8 length, UInt8 const *pData)
{
    if (pPdLoh && length && pData)
    {
        if (length == 1)
        {
            Pd_WriteByte(pPdLoh->handle, pPdLoh->offset + pPdLoh->length, *pData);
        }
        else
        {
            Pd_WriteByteStream(pPdLoh->handle, pPdLoh->offset + pPdLoh->length, length, (UInt8*)pData);
        }
        pPdLoh->length += length;
    }
}
INLINE static void Pd_PrependWithUpdate(gpPd_Loh_t *pPdLoh, UInt8 length, UInt8 const *pData)
{
    if (pPdLoh && length && pData)
    {
        pPdLoh->offset -= length;
        if (length == 1)
        {
            Pd_WriteByte(pPdLoh->handle, pPdLoh->offset, *pData);
        }
        else
        {
            Pd_WriteByteStream(pPdLoh->handle, pPdLoh->offset, length, (UInt8*)pData);
        }
        pPdLoh->length += length;
    }
}

INLINE static void Pd_ReadWithUpdate(gpPd_Loh_t *pPdLoh, UInt8 length, UInt8 *pData)
{
    if (pPdLoh && length && pData)
    {
        if (length == 1)
        {
            *pData = Pd_ReadByte(pPdLoh->handle, pPdLoh->offset);
        }
        else
        {
            Pd_ReadByteStream(pPdLoh->handle, pPdLoh->offset, length, pData);
        }
        pPdLoh->offset += length;
        pPdLoh->length -= length;
    }
}
