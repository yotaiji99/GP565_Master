/*****************************************************************************
 *                    Check enumrations compatibility
 *****************************************************************************/
#include "gpMacCore.h"

#define CHECK_ENUM_EQUAL(a,b) GP_COMPILE_TIME_VERIFY(((int) (a)) == ((int) (b)))
//MAC results
CHECK_ENUM_EQUAL(gpHal_ResultSuccess              , gpMacCore_ResultSuccess);
CHECK_ENUM_EQUAL(gpHal_ResultInvalidHandle        , gpMacCore_ResultInvalidHandle);
CHECK_ENUM_EQUAL(gpHal_ResultCCAFailure           , gpMacCore_ResultChannelAccessFailure);
CHECK_ENUM_EQUAL(gpHal_ResultNoAck                , gpMacCore_ResultNoAck);
