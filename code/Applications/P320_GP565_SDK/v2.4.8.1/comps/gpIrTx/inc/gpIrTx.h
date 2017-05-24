/*
* Copyright (c) 2011-2014, GreenPeak Technologies
*
*  The file gpIrTx.h contains the definitions for transmitting all sorts of IR codes
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
*     0!                         $Header: //depot/release/Embedded/Applications/P320_GP565_SDK/v2.4.8.1/comps/gpIrTx/inc/gpIrTx.h#1 $
*    M'   GreenPeak              $Change: 77946 $
*   0'         Technologies      $DateTime: 2016/03/03 15:31:03 $
*  F
*/

#ifndef _GP_IR_TX_H_
#define _GP_IR_TX_H_
#define GP_IR_TX_DIVERSITY_PREDEFINED
/*****************************************************************************
*                    Includes Definitions
*****************************************************************************/

#include <global.h>

#include "gpIrTx_Generic.h"

/**
 * @file gpIrTx.h
 *
 * @brief This file contains interface to the gpIrTx component
 *
 *  The application requiring the IR transmission support only needs to include this header file. Other header
 *  files will be included as necessary based on supported features.
 */

/**
 * @defgroup API_OVERVIEW General API overview
 *
 * This gpIrTx component contains routines that implement the transmission of IR protocols.
 *
 * The API of this component has the following parts:
 *  \li Initialization routines
 * \if PREDEFINED_DOC_ENABLED
 *  \li Routines that generate pre-defined IR protocols
 * \endif
 *  \li Routines that generate IR based on generic timing model
 *  \li Callback routines
 *  \li Serial API specific routines
 *
 * \if PREDEFINED_DOC_ENABLED
 * The routines implementing pre-defined IR protocols follow a simple SendRequest() model with a Confirm() callback.
 * The SendRequest() primitive takes a configuration structure with both general information such as minimum number of repeats
 * as well as protocol-specific information.
 * For repeated IR with non-fixed number of repeats, a special callback routine is provided which is called just
 * before the scheduled transmission of the repeat to query the application to continue with the transmission or stop.
 *
 * Following code example shows how to use the built-in NEC protocol to control a late-model Samsung TV:
 * \code
 * #include "gpIrTx.h"
 *
 * {
 *     gpIrTx_Config_t     config;
 *     gpIrTx_Config_Nec_t necConfig = { 0x07, gpIrTx_Necx2, 0x07 };
 *
 *     config.spec.nec         = necConfig;
 *     config.function         = 0x10;
 *     config.numberMinRepeats = 0;
 *     config.repeated         = true;
 *     gpIrTx_SendCommandRequestPredefined(gpIrTx_IrComponent_Nec, config);
 * }
 *
 * Bool gpIrTx_cbIsKeyPressed(void)
 * {
 *     return button_state == PRESSED;
 * }
 * \endcode
 * \endif
 *
 * The routines using the generic timing model to generate an IR protocol follow a simple SendRequest() model with a Confirm() callback.
 * The SendRequest() primitive expects a structure containing a byte array describing the IR signal to be generated. Optionally, a modifier
 * can be used to override the default repeat information in the IR signal description.
 *
 * If the IR signal is marked to be repeatable with a non-fixed number, a special callback routine is provided which is called
 * just before the scheduled transmission of the repeat to query the application to continue with the transmission or stop.
 *
 * Following code example shows a how to use the routines assuming a valid pattern is being used:
 * \code
 * #include "gpIrTx.h"
 *
 * {
 *     extern gpIrTx_TvIrDesc_t my_ir_pattern_description;
 *
 *     gpIrTx_SendCommandRequestGeneric(&my_ir_pattern_description, false, 0);
 * }
 *
 * Bool gpIrTx_cbIsKeyPressed(void)
 * {
 *     return button_state == PRESSED;
 * }
 * \endcode
 *
 * Note: when using the component using the serial API, the IsKeyPressed() callback is not available. The asynchronous routine
 * ClearKeyPressedFlag() is provided instead.
 */

/**
 * @defgroup COMMON_IRTX Initialization routines
 * @brief These routines are required for initialization and de-initialization of the gpIrTx component.
 */

/**
 * @defgroup PREDEFINED_IRTX Routines for predefined timing model
 * @brief These routines implement the IR transmission functionality for the available predefined IR protocols.
 */

/**
 * @defgroup GENERIC_IRTX Routines for generic timing model
 * @brief These routines implement the IR transmission functionality based on the generic timing model.
 */

/**
 * @defgroup CALLBACK_IRTX Callback routines
 * @brief Callback routines for the gpIrTx component.
 */

/**
 * @defgroup SERAPI_IRTX Serial API routines
 * @brief Specific routines for using the gpIrTx serial API option.
 *
 *        These functions are present only in the serial API version of the component.
 *        These functions are used by host application containing IrTx client and by
 *        device application containing IrTx server.
 */

/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/

/**
 * @name gpIrTx_Result_t
 */
//@{
/** @brief  Success */
#define gpIrTx_ResultSuccess           0
/** @brief  A different IR transmission is already in progress */
#define gpIrTx_ResultBusy              1
/** @brief  An invalid parameter has been supplied */
#define gpIrTx_ResultInvalidParameter  2
/** @typedef gpIrTx_Result_t
 *  @brief The gpIrTx_Result_t type defines all return values which can be generated by the routines of the component.
*/
#define gpIrTx_IrComponent_Nec        3

typedef UInt8 gpIrTx_Result_t;
//@}

/**
 * @name gpIrTx_ConfigMode_t
 */
//@{
/** @brief  GP mode as descirbed in GP_P215_AS_03407_Generic_IRDB_structures */
#define gpIrTx_ConfigModeGp           0
/** @brief  ZRC mode as descirbed in Annex B: Standard IR Database format of ZigBee RF4CE: ZRC Profile Specification Version 2.0 */
#define gpIrTx_ConfigModeZrc          1
/** @typedef gpIrTx_ConfigMode_t
 *  @brief The gpIrTx_ConfigMode_t type defines the bytestream gpIrTx_TvIrDesc_t used in the  gpIrTx_SendCommandRequestGeneric function will be intertpreted.
*/
typedef UInt8 gpIrTx_ConfigMode_t;
//@}


/** @typedef gpIrTx_CommandCode_t
 *  @brief gpIrTx_CommandCode_t is used to represent the IR payload.
*/
typedef UInt8 gpIrTx_CommandCode_t;

/**
 * @name gpIrTx_IrComponentType_t
 */
//@{
/// @typedef gpIrTx_IrComponentType_t
/// @brief The gpIrTx_IrComponentType_t type defines the type of predefined IR transmission.
typedef UInt8 gpIrTx_IrComponentType_t;
//@}




typedef struct {
/** device */    
    UInt8 device;
/** necType field contains nec type information packed in low 2 bits */    
    UInt8 necType;
/** subDevice */    
    UInt8 subDevice;
} gpIrTx_Config_Nec_t;


/** @typedef gpIrTx_Config_t
 *  @brief The gpIrTx_Config_t type is used with predefined IR protocols to describe information needed to calculate IR signal timing */
typedef struct {
/** function field describing command function to be sent to the device */
    gpIrTx_CommandCode_t function;
/** numberMinRepeats is number of repeats that will be generated as a minimum regardless of the state of the key press or signal request */
    UInt8                numberMinRepeats;
/** repeated is logical field describing if the signal is to be repeated or not */
    Bool                 repeated;
/** spec field is union of all of the supported predefined device descriptors */
    union {
        gpIrTx_Config_Nec_t      nec;
        UInt8 dummy;
    } spec;
} gpIrTx_Config_t;

/** @struct gpIrTx_TransmissionTimingData
 *  @brief The gpIrTx_TransmissionTimingData structure is used to describe the basic signaling element for the IR signal timing */
/** @typedef gpIrTx_TransmissionTimingData_t
 *  @brief The gpIrTx_TransmissionTimingData_t type is used to describe the basic signaling element for the IR signal timing */
typedef struct gpIrTx_TransmissionTimingData {
/** set (aka mark or make) describes the time the basic signaling element will excite the IR LED (continuous or modulated with carrier). Given as multiple of 4us. */
    UInt16 set;
/** clear (aka break) describes the time the basic signaling element will keep the IR LED excited. Given as multiple of 4us. */
    UInt16 clr;
} gpIrTx_TransmissionTimingData_t;

/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/

/**  @ingroup COMMON_IRTX
 *
 *  gpIrTx_Init function initializes resources used by the IRTX component.
 *  In case of the serial API usage of the component client (host side), this boils down to the initialization of the serial interface wrapper.
 */
void gpIrTx_Init(void);

/**  @ingroup GENERIC_IRTX
 *
 *  gpIrTx_SetConfigMode Setter function for the IrTx Config Mode.
 *
 *  @param mode The config mode to be set.
 */
void gpIrTx_SetConfigMode(gpIrTx_ConfigMode_t mode);

/**  @ingroup GENERIC_IRTX
 *
 *  gpIrTx_GetConfigMode Getter function for the IrTx Config Mode.
 *
 *  @return   The active config mode.
 */
gpIrTx_ConfigMode_t gpIrTx_GetConfigMode(void);

#if defined(GP_IR_TX_DIVERSITY_PREDEFINED)
/**  @ingroup PREDEFINED_IRTX
 *
 *  gpIrTx_PredefinedCommandBusy function returns if a predefined command is currently being executed
 *
 *  @return   busy      Returns true if a predefined IR command is being transmitted
 */
Bool gpIrTx_PredefinedCommandBusy(void);



/**  @ingroup PREDEFINED_IRTX
 *
 *  gpIrTx_SendCommandRequestPredefined function requests generation of one of the predefined propriatery IR commands.
 *
 *  This function doesn't exist on the serial API. It is implemented in the library only. It is included in embedded IrTx applications and in
 *  the device application where it is wrapped arround by gpIrTx_SendCommandRequestPredefinedAdapter.
 *  In the embedded mode the process of generating the desired IR signal is carried on as long as the application responds affirmatively to the
 *  gpIrTx_cbIsKeyPressed callback and gpIrTx_CancelRequestPredefined is not called.
 *
 *  @param    type      Specification of the desired predefined propriatery IR protocol
 *  @param    config    Configuration of the desired command that is to be generated over the specified protocol.
 */
void gpIrTx_SendCommandRequestPredefined(
                gpIrTx_IrComponentType_t type,
                gpIrTx_Config_t          config
            );

/**  @ingroup PREDEFINED_IRTX
 *
 *  gpIrTx_CancelRequestPredefined function cancels repeating of the ongoing predefined command.
 *
 *  This function behaives the same in both embedded and serial API mode. It attempts to cancel the repeating of the ongoing predefined command.
 *
 *  @return True if there was repeating ongoing to be stopped, false otherwise
 */
Bool gpIrTx_CancelRequestPredefined(void);

/**  @ingroup PREDEFINED_IRTX
 *
 *  gpIrTx_cbSendCommandConfirmPredefined callback informs application of the status of the predefined request.
 *
 *  This function is implemented by application and called by this module to pass information about the status of the
 *  last predefined request. It also confirms the requested function and info about repeat of the signal so the confirmation can be matched to the request.
 *
 *  @param  status      Success, busy or invalid parameter. Only success indicates that the request was accepted.
 *  @param  repeated    Information about repeat of the signal that is extracted from configuration of the request being confirmed
 *  @param  function    Information about function commanded that is extracted from configuration of the request being confirmed
 */
void gpIrTx_cbSendCommandConfirmPredefined(
                gpIrTx_Result_t      status,
                Bool                 repeated,
                gpIrTx_CommandCode_t function
            );

#endif /* defined(GP_IR_TX_DIVERSITY_PREDEFINED) */

/**  @ingroup GENERIC_IRTX
 *
 *  gpIrTx_GenericCommandBusy function returns if a predefined command is currently being executed
 *
 *  @return busy    Returns if a generic IR command is busy
 */
Bool gpIrTx_GenericCommandBusy(void);

/**  @ingroup GENERIC_IRTX
 *
 *  gpIrTx_CorrectTimingGeneric performs one-time in-place correction of the timing definitions in "Generic" mode
 *
 *  This function is used to optimize handling of the timing definitions of "Generic" mode that are only once loaded from ROM storage into the RAM storage.
 *  This function adjusts timing definitions of the IR signal in "Generic" mode to precompensate for the timing granularity error caused by the timer resolution.
 *  When adjustment has been applied, calls to gpIrTx_SendCommandRequestGeneric should have parameter expandTimingData set to false.
 *
 *  @param   code           Reference to the RAM storage containing full code definition
 */
void gpIrTx_CorrectTimingGeneric(UInt8* code);



/**  @ingroup GENERIC_IRTX
 *
 *  gpIrTx_SendCommandRequestGeneric function requests generation of a IR command defined with generic timing descriptor.
 *
 *  This function doesn't exist on the serial API. It is implemented in the library only. It is included in embedded IrTx applications and in
 *  the device application where it is wrapped arround by gpIrTx_SendCommandRequestGenericAdapter.
 *  In the embedded mode the process of generating the desired IR signal is carried on as long as the application responds affirmatively to the
 *  gpIrTx_cbIsKeyPressed callback.
 *
 *  @param    pDesc               Reference to the memory location containing description of the timing.
 *  @param    expandTimingData    Flag indicating that the timing data has to be adjusted for accuracy and correct time base. If needed, this operation has to be done only once if the timing data is reused. Adjustments are in-place so the storage for timing data has to be of RAM type.
 *  @param    commandFlagModifier The value of this parameter is used to modify the command flag of the signal timing descriptor.
 */
void gpIrTx_SendCommandRequestGeneric(
                gpIrTx_TvIrDesc_t* pDesc,
                Bool               expandTimingData,
                UInt8              commandFlagModifier
            );

/**  @ingroup GENERIC_IRTX
 *
 *  gpIrTx_cbSendCommandConfirmGeneric callback informs application of the status of the generic request.
 *
 *  This function is implemented by application and called by this module to pass information about the status of the
 *  last generic request. It also confirms the requested function and info about repeat of the signal so the confirmation can be matched to the request.
 *
 *  @param  status      Success, busy or invalid parameter. Only success indicates that the request was accepted.
 *  @param  pDesc       Reference to the memory location containing description of the timing used for IR signal generation. All of the modifications done by the execution of gpIrTx_cbSendCommandConfirmGeneric are visible here.
 */
void gpIrTx_cbSendCommandConfirmGeneric(
                gpIrTx_Result_t    status,
                gpIrTx_TvIrDesc_t* pDesc
            );

/**  @ingroup CALLBACK_IRTX
 *
 *  gpIrTx_cbIsKeyPressed callback function informs this module of the status of the "key press" and used for generating an IR repeat if the call to CommandRequest() was configured to do repeated transmission.
 *
 *  @return   True if next IR repeat needs to be generated, false otherwise.
 */
Bool gpIrTx_cbIsKeyPressed(void);


#endif //_GP_IR_TX_H_
