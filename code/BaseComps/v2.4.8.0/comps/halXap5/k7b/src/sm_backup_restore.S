#include "gpHal_kx_regprop_basic.h"
#include "gpHal_kx_mm.h"

   .global $sm_backup_restore_default
$sm_backup_restore_default:
   mov.i    %r2, #(GP_MM_RAM_RETENTION_START)
   /*Custom restore of watchdog control. Needs to be done at the very beginning while
     The change window is still open */
   clu.ds #(GP_WB_WATCHDOG_ENABLE_ADDRESS), %r2, %r2
   clu.ds #(GP_WB_WATCHDOG_TIMEOUT_ADDRESS), %r2, %r2
   clu.ds #(GP_WB_WATCHDOG_TIMEOUT_ADDRESS+1), %r2, %r2
    /*include board specific save/restore routine*/
#include "gpBsp_sm_restore_k7b.S"
   pop.ret  {}, #0
