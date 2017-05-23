#ifndef GP_WB_ENUM_H
#define GP_WB_ENUM_H

/***************************
 * layout: general_enum
 ***************************/
/* serial_itf_select  */
#define GP_WB_ENUM_SERIAL_ITF_SELECT_USE_SPI         0x0
#define GP_WB_ENUM_SERIAL_ITF_SELECT_USE_TWI         0x1
#define GP_WB_ENUM_SERIAL_ITF_SELECT_USE_UART        0x2
#define GP_WB_ENUM_SERIAL_ITF_SELECT_NO_INTERFACE    0x3

/* nvm_size  */
#define GP_WB_ENUM_NVM_SIZE_KB256    0x0
#define GP_WB_ENUM_NVM_SIZE_KB224    0x1
#define GP_WB_ENUM_NVM_SIZE_KB192    0x2
#define GP_WB_ENUM_NVM_SIZE_KB160    0x3
#define GP_WB_ENUM_NVM_SIZE_KB128    0x4
#define GP_WB_ENUM_NVM_SIZE_KB96     0x5
#define GP_WB_ENUM_NVM_SIZE_KB64     0x6
#define GP_WB_ENUM_NVM_SIZE_KB32     0x7

/* ram_size  */
#define GP_WB_ENUM_RAM_SIZE_KB16    0x0
#define GP_WB_ENUM_RAM_SIZE_KB12    0x1
#define GP_WB_ENUM_RAM_SIZE_KB8     0x2
#define GP_WB_ENUM_RAM_SIZE_KB4     0x3

/* pbm_utc_state  */
#define GP_WB_ENUM_PBM_UTC_STATE_PBM_FREE                0x0
#define GP_WB_ENUM_PBM_UTC_STATE_MC1_RMF_NORM            0x1
#define GP_WB_ENUM_PBM_UTC_STATE_GP_UNI_TX_FRM           0x2
#define GP_WB_ENUM_PBM_UTC_STATE_GP_UNI_TX_FRM_WAIT      0x3
#define GP_WB_ENUM_PBM_UTC_STATE_GP_UNI_TX_FRM_RESULT    0x4
#define GP_WB_ENUM_PBM_UTC_STATE_GP_UNI_TX_FRM_END       0x5
#define GP_WB_ENUM_PBM_UTC_STATE_GP_MC1_RMF_NORM_END     0x6

/* pbm_vq  */
#define GP_WB_ENUM_PBM_VQ_UNTIMED    0x1
#define GP_WB_ENUM_PBM_VQ_SCHED0     0x2
#define GP_WB_ENUM_PBM_VQ_SCHED1     0x3
#define GP_WB_ENUM_PBM_VQ_SCHED2     0x4
#define GP_WB_ENUM_PBM_VQ_SCHED3     0x5

/* pbm_result  */
#define GP_WB_ENUM_PBM_RESULT_SUCCESS                 0x00
#define GP_WB_ENUM_PBM_RESULT_BUSYRX                  0x01
#define GP_WB_ENUM_PBM_RESULT_BUSYTX                  0x02
#define GP_WB_ENUM_PBM_RESULT_FORCETRXOFF             0x03
#define GP_WB_ENUM_PBM_RESULT_IDLE                    0x04
#define GP_WB_ENUM_PBM_RESULT_INVALIDPARAMETER        0x05
#define GP_WB_ENUM_PBM_RESULT_RXON                    0x06
#define GP_WB_ENUM_PBM_RESULT_BUSY                    0x07
#define GP_WB_ENUM_PBM_RESULT_TRXOFF                  0x08
#define GP_WB_ENUM_PBM_RESULT_TXON                    0x09
#define GP_WB_ENUM_PBM_RESULT_UNSUPPORTEDATTRIBUTE    0x0A
#define GP_WB_ENUM_PBM_RESULT_LOCKLOSS                0xD3
#define GP_WB_ENUM_PBM_RESULT_NOLOCK                  0xD4
#define GP_WB_ENUM_PBM_RESULT_BEACONLOSS              0xE0
#define GP_WB_ENUM_PBM_RESULT_CHANNELACCESSFAILURE    0xE1
#define GP_WB_ENUM_PBM_RESULT_DENIED                  0xE2
#define GP_WB_ENUM_PBM_RESULT_DISABLETRXFAILURE       0xE3
#define GP_WB_ENUM_PBM_RESULT_FAILEDSECURITYCHECK     0xE4
#define GP_WB_ENUM_PBM_RESULT_FRAMETOOLONG            0xE5
#define GP_WB_ENUM_PBM_RESULT_INVALIDGTS              0xE6
#define GP_WB_ENUM_PBM_RESULT_INVALIDHANDLE           0xE7
#define GP_WB_ENUM_PBM_RESULT_NOACK                   0xE9
#define GP_WB_ENUM_PBM_RESULT_NOBEACON                0xEA
#define GP_WB_ENUM_PBM_RESULT_NODATA                  0xEB
#define GP_WB_ENUM_PBM_RESULT_NOSHORTADDRESS          0xEC
#define GP_WB_ENUM_PBM_RESULT_OUTOFCAP                0xED
#define GP_WB_ENUM_PBM_RESULT_PANIDCONFLICT           0xEE
#define GP_WB_ENUM_PBM_RESULT_REALIGNMENT             0xEF
#define GP_WB_ENUM_PBM_RESULT_TRANSACTIONEXPIRED      0xF0
#define GP_WB_ENUM_PBM_RESULT_TRANSACTIONOVERFLOW     0xF1
#define GP_WB_ENUM_PBM_RESULT_TXACTIVE                0xF2
#define GP_WB_ENUM_PBM_RESULT_UNAVAILABLEKEY          0xF3
#define GP_WB_ENUM_PBM_RESULT_RXDEFERRED              0xF5
#define GP_WB_ENUM_PBM_RESULT_SCH_TX_BUSY             0xD1

/* standby_state  */
#define GP_WB_ENUM_STANDBY_STATE_RESET                  0x0
#define GP_WB_ENUM_STANDBY_STATE_ACTIVE                 0x1
#define GP_WB_ENUM_STANDBY_STATE_READY_FOR_POWER_OFF    0x2
#define GP_WB_ENUM_STANDBY_STATE_SLEEP                  0x3

/* ext_clkfreq  */
#define GP_WB_ENUM_EXT_CLKFREQ_MHZ1     0x0
#define GP_WB_ENUM_EXT_CLKFREQ_MHZ2     0x1
#define GP_WB_ENUM_EXT_CLKFREQ_MHZ4     0x2
#define GP_WB_ENUM_EXT_CLKFREQ_MHZ8     0x3
#define GP_WB_ENUM_EXT_CLKFREQ_MHZ16    0x4

/* ssp_mode  */
#define GP_WB_ENUM_SSP_MODE_ENCRYPT    0x0
#define GP_WB_ENUM_SSP_MODE_DECRYPT    0x1
#define GP_WB_ENUM_SSP_MODE_AES        0x2

/* ssp_key_len  */
#define GP_WB_ENUM_SSP_KEY_LEN_KEY_128    0x0
#define GP_WB_ENUM_SSP_KEY_LEN_KEY_192    0x1
#define GP_WB_ENUM_SSP_KEY_LEN_KEY_256    0x2

/* watchdog_function  */
#define GP_WB_ENUM_WATCHDOG_FUNCTION_IDLE             0x0
#define GP_WB_ENUM_WATCHDOG_FUNCTION_GEN_INTERRUPT    0x1
#define GP_WB_ENUM_WATCHDOG_FUNCTION_SOFT_POR         0x2
#define GP_WB_ENUM_WATCHDOG_FUNCTION_RESET_INT_UC     0x3
#define GP_WB_ENUM_WATCHDOG_FUNCTION_RESET_EXT_UC     0x4

/* gpio_mode  */
#define GP_WB_ENUM_GPIO_MODE_FLOAT        0x0
#define GP_WB_ENUM_GPIO_MODE_PULLDOWN     0x1
#define GP_WB_ENUM_GPIO_MODE_PULLUP       0x2
#define GP_WB_ENUM_GPIO_MODE_BUSKEEPER    0x3

/* drive_strength  */
#define GP_WB_ENUM_DRIVE_STRENGTH_DRIVE_2DOT5MA    0x0
#define GP_WB_ENUM_DRIVE_STRENGTH_DRIVE_5MA        0x1
#define GP_WB_ENUM_DRIVE_STRENGTH_DRIVE_7DOT5MA    0x2
#define GP_WB_ENUM_DRIVE_STRENGTH_DRIVE_10MA       0x3

/* slew_rate  */
#define GP_WB_ENUM_SLEW_RATE_FAST    0x0
#define GP_WB_ENUM_SLEW_RATE_SLOW    0x1

/* gpio_port_sel  */
#define GP_WB_ENUM_GPIO_PORT_SEL_PORTA    0x0
#define GP_WB_ENUM_GPIO_PORT_SEL_PORTB    0x1
#define GP_WB_ENUM_GPIO_PORT_SEL_PORTC    0x2
#define GP_WB_ENUM_GPIO_PORT_SEL_PORTD    0x3

/* standby_mode  */
#define GP_WB_ENUM_STANDBY_MODE_RC_MODE             0x0
#define GP_WB_ENUM_STANDBY_MODE_XTAL_32KHZ_MODE     0x1
#define GP_WB_ENUM_STANDBY_MODE_XTAL_16MHZ_MODE     0x2
#define GP_WB_ENUM_STANDBY_MODE_FRING_16MHZ_MODE    0x3

/* wakeup_pin_mode  */
#define GP_WB_ENUM_WAKEUP_PIN_MODE_NO_EDGE         0x0
#define GP_WB_ENUM_WAKEUP_PIN_MODE_RISING_EDGE     0x1
#define GP_WB_ENUM_WAKEUP_PIN_MODE_FALLING_EDGE    0x2
#define GP_WB_ENUM_WAKEUP_PIN_MODE_BOTH_EDGES      0x3

/* por_reason  */
#define GP_WB_ENUM_POR_REASON_HW_POR                           0x0
#define GP_WB_ENUM_POR_REASON_SOFT_POR_BY_REGMAP               0x1
#define GP_WB_ENUM_POR_REASON_SOFT_POR_BY_VRR_BROWNOUT         0x2
#define GP_WB_ENUM_POR_REASON_SOFT_POR_BY_ISO_TX               0x3
#define GP_WB_ENUM_POR_REASON_SOFT_POR_BY_WATCHDOG             0x4
#define GP_WB_ENUM_POR_REASON_SOFT_POR_BY_ES                   0x5
#define GP_WB_ENUM_POR_REASON_POR_BY_VDDB_CUTOFF               0x6
#define GP_WB_ENUM_POR_REASON_POR_BY_VDDDIG_NOK                0x7
#define GP_WB_ENUM_POR_REASON_SOFT_POR_RAM_MW_INVALID          0x8
#define GP_WB_ENUM_POR_REASON_SOFT_POR_RAM_CRC_INVALID         0x9
#define GP_WB_ENUM_POR_REASON_SOFT_POR_FLASH_BL_CRC_INVALID    0xA
#define GP_WB_ENUM_POR_REASON_SOFT_POR_BOOTLOADER              0xB

/* clu_mode  */
#define GP_WB_ENUM_CLU_MODE_RESTORE    0x0
#define GP_WB_ENUM_CLU_MODE_BACKUP     0x1

/* parity  */
#define GP_WB_ENUM_PARITY_EVEN    0x0
#define GP_WB_ENUM_PARITY_ODD     0x1
#define GP_WB_ENUM_PARITY_OFF     0x2

/* ir_modulation_mode  */
#define GP_WB_ENUM_IR_MODULATION_MODE_MODULATION_OFF    0x0
#define GP_WB_ENUM_IR_MODULATION_MODE_REGISTER_BASED    0x1
#define GP_WB_ENUM_IR_MODULATION_MODE_PATTERN_BASED     0x2
#define GP_WB_ENUM_IR_MODULATION_MODE_TIME_BASED        0x3
#define GP_WB_ENUM_IR_MODULATION_MODE_EXT_BASED         0x4
#define GP_WB_ENUM_IR_MODULATION_MODE_EVENT_BASED       0x5

/* ir_time_unit  */
#define GP_WB_ENUM_IR_TIME_UNIT_TU_500NS    0x0
#define GP_WB_ENUM_IR_TIME_UNIT_TU_1US      0x1
#define GP_WB_ENUM_IR_TIME_UNIT_TU_2US      0x2
#define GP_WB_ENUM_IR_TIME_UNIT_TU_4US      0x3

/* generic_io_drive  */
#define GP_WB_ENUM_GENERIC_IO_DRIVE_PUSH_PULL     0x0
#define GP_WB_ENUM_GENERIC_IO_DRIVE_OPEN_DRAIN    0x1

/* generic_single_pinmap  */
#define GP_WB_ENUM_GENERIC_SINGLE_PINMAP_MAPPING_A     0x0
#define GP_WB_ENUM_GENERIC_SINGLE_PINMAP_NOT_MAPPED    0x1

/* generic_triple_pinmap  */
#define GP_WB_ENUM_GENERIC_TRIPLE_PINMAP_MAPPING_A     0x0
#define GP_WB_ENUM_GENERIC_TRIPLE_PINMAP_MAPPING_B     0x1
#define GP_WB_ENUM_GENERIC_TRIPLE_PINMAP_MAPPING_C     0x2
#define GP_WB_ENUM_GENERIC_TRIPLE_PINMAP_NOT_MAPPED    0x3

/* generic_seven_pinmap  */
#define GP_WB_ENUM_GENERIC_SEVEN_PINMAP_MAPPING_A     0x0
#define GP_WB_ENUM_GENERIC_SEVEN_PINMAP_MAPPING_B     0x1
#define GP_WB_ENUM_GENERIC_SEVEN_PINMAP_MAPPING_C     0x2
#define GP_WB_ENUM_GENERIC_SEVEN_PINMAP_MAPPING_D     0x3
#define GP_WB_ENUM_GENERIC_SEVEN_PINMAP_MAPPING_E     0x4
#define GP_WB_ENUM_GENERIC_SEVEN_PINMAP_MAPPING_F     0x5
#define GP_WB_ENUM_GENERIC_SEVEN_PINMAP_MAPPING_G     0x6
#define GP_WB_ENUM_GENERIC_SEVEN_PINMAP_NOT_MAPPED    0x7

/* qta_status  */
#define GP_WB_ENUM_QTA_STATUS_EMPTY        0x0
#define GP_WB_ENUM_QTA_STATUS_IDLE         0x1
#define GP_WB_ENUM_QTA_STATUS_TX           0x2
#define GP_WB_ENUM_QTA_STATUS_PURGE        0x3
#define GP_WB_ENUM_QTA_STATUS_CNF          0x4
#define GP_WB_ENUM_QTA_STATUS_CNF_RUN      0x5
#define GP_WB_ENUM_QTA_STATUS_ENTRY_SEL    0x6
#define GP_WB_ENUM_QTA_STATUS_DUMMY_7      0x7

/* qta_vq  */
#define GP_WB_ENUM_QTA_VQ_NONE       0x0
#define GP_WB_ENUM_QTA_VQ_UNTIMED    0x1
#define GP_WB_ENUM_QTA_VQ_SCH0       0x2
#define GP_WB_ENUM_QTA_VQ_SCH1       0x3
#define GP_WB_ENUM_QTA_VQ_SCH2       0x4
#define GP_WB_ENUM_QTA_VQ_SCH3       0x5
#define GP_WB_ENUM_QTA_VQ_DUMMY6     0x6
#define GP_WB_ENUM_QTA_VQ_DUMMY7     0x7

/* dma_func  */
#define GP_WB_ENUM_DMA_FUNC_CPY_PLAIN    0x0
#define GP_WB_ENUM_DMA_FUNC_CPY_XOR      0x1
#define GP_WB_ENUM_DMA_FUNC_CPY_AND      0x2
#define GP_WB_ENUM_DMA_FUNC_CPY_OR       0x3
#define GP_WB_ENUM_DMA_FUNC_CPY_ADD      0x4
#define GP_WB_ENUM_DMA_FUNC_CMP          0x5
#define GP_WB_ENUM_DMA_FUNC_MATCH        0x6
#define GP_WB_ENUM_DMA_FUNC_INC          0x7

/* phy_state_transition  */
#define GP_WB_ENUM_PHY_STATE_TRANSITION_OFF_TO_TX               0x0
#define GP_WB_ENUM_PHY_STATE_TRANSITION_OFF_TO_RX               0x1
#define GP_WB_ENUM_PHY_STATE_TRANSITION_TX_TO_RX                0x2
#define GP_WB_ENUM_PHY_STATE_TRANSITION_RX_TO_TX                0x3
#define GP_WB_ENUM_PHY_STATE_TRANSITION_TO_OFF                  0x4
#define GP_WB_ENUM_PHY_STATE_TRANSITION_TO_CAL                  0x5
#define GP_WB_ENUM_PHY_STATE_TRANSITION_GENERIC_TRANSITION_1    0x6
#define GP_WB_ENUM_PHY_STATE_TRANSITION_GENERIC_TRANSITION_2    0x7
#define GP_WB_ENUM_PHY_STATE_TRANSITION_SET_RX_ON               0x8

/* purge_status  */
#define GP_WB_ENUM_PURGE_STATUS_PURGE_SUCCESS     0x0
#define GP_WB_ENUM_PURGE_STATUS_PURGE_TOO_LATE    0x1

/* adc_trigger_mode  */
#define GP_WB_ENUM_ADC_TRIGGER_MODE_WHEN_RSSI_REQ            0x0
#define GP_WB_ENUM_ADC_TRIGGER_MODE_ALWAYS                   0x1
/* In this mode, adc measurement triggers are generated by the ES or the timer block */
#define GP_WB_ENUM_ADC_TRIGGER_MODE_WHEN_EXTERNAL_TRIGGER    0x2
/* the adc is never triggered */
#define GP_WB_ENUM_ADC_TRIGGER_MODE_NEVER                    0x3

/* adc_channel  */
#define GP_WB_ENUM_ADC_CHANNEL_RSSI         0x0
/* vdda/3 */
#define GP_WB_ENUM_ADC_CHANNEL_VDDA         0x1
/* Temperature sensor output voltage */
#define GP_WB_ENUM_ADC_CHANNEL_TEMP         0x2
/* reserved */
#define GP_WB_ENUM_ADC_CHANNEL_CHANNEL3     0x3
/* reserved */
#define GP_WB_ENUM_ADC_CHANNEL_CHANNEL4     0x4
/* reserved */
#define GP_WB_ENUM_ADC_CHANNEL_CHANNEL5     0x5
/* reserved */
#define GP_WB_ENUM_ADC_CHANNEL_CHANNEL6     0x6
/* reserved */
#define GP_WB_ENUM_ADC_CHANNEL_CHANNEL7     0x7
#define GP_WB_ENUM_ADC_CHANNEL_ANIO0        0x8
#define GP_WB_ENUM_ADC_CHANNEL_ANIO1        0x9
#define GP_WB_ENUM_ADC_CHANNEL_ANIO2        0xA
#define GP_WB_ENUM_ADC_CHANNEL_ANIO3        0xB
/* GPIO28 - tbd */
#define GP_WB_ENUM_ADC_CHANNEL_ADC_GPI0     0xC
/* GPIO29 - tbd */
#define GP_WB_ENUM_ADC_CHANNEL_ADC_GPI1     0xD
#define GP_WB_ENUM_ADC_CHANNEL_CHANNEL14    0xE
#define GP_WB_ENUM_ADC_CHANNEL_CHANNEL15    0xF

/* dma_trigger_src_select  */
#define GP_WB_ENUM_DMA_TRIGGER_SRC_SELECT_NO_TRIGGER_SRC             0x0
#define GP_WB_ENUM_DMA_TRIGGER_SRC_SELECT_SPI_SL_TX_NOT_FULL         0x1
#define GP_WB_ENUM_DMA_TRIGGER_SRC_SELECT_SPI_SL_RX_NOT_EMPTY        0x2
#define GP_WB_ENUM_DMA_TRIGGER_SRC_SELECT_SPI_M_TX_NOT_FULL          0x3
#define GP_WB_ENUM_DMA_TRIGGER_SRC_SELECT_SPI_M_RX_NOT_EMPTY         0x4
#define GP_WB_ENUM_DMA_TRIGGER_SRC_SELECT_UART_TX_NOT_FULL           0x5
#define GP_WB_ENUM_DMA_TRIGGER_SRC_SELECT_UART_RX_NOT_EMPTY          0x6
#define GP_WB_ENUM_DMA_TRIGGER_SRC_SELECT_PWM_TX_NOT_FULL            0x7
#define GP_WB_ENUM_DMA_TRIGGER_SRC_SELECT_PWM_TIMESTAMP_NOT_EMPTY    0x8
#define GP_WB_ENUM_DMA_TRIGGER_SRC_SELECT_ADC_FIFO_NOT_EMPTY         0x9

/* antsel_int  */
#define GP_WB_ENUM_ANTSEL_INT_USE_PORT_FROM_DESIGN    0x0
#define GP_WB_ENUM_ANTSEL_INT_USE_PORT_0              0x1
#define GP_WB_ENUM_ANTSEL_INT_USE_PORT_1              0x2

/* antsel_ext  */
#define GP_WB_ENUM_ANTSEL_EXT_USE_EXT_PORT_0                   0x0
#define GP_WB_ENUM_ANTSEL_EXT_USE_EXT_PORT_1                   0x1
#define GP_WB_ENUM_ANTSEL_EXT_USE_PORT_FROM_DESIGN_STRAIGHT    0x2
#define GP_WB_ENUM_ANTSEL_EXT_USE_PORT_FROM_DESIGN_INVERTED    0x3

/* err_status  */
#define GP_WB_ENUM_ERR_STATUS_NO_ERR           0x0
#define GP_WB_ENUM_ERR_STATUS_PROG_ERR         0x1
#define GP_WB_ENUM_ERR_STATUS_DATA_ERR         0x2
#define GP_WB_ENUM_ERR_STATUS_USER_PROG_ERR    0x3
#define GP_WB_ENUM_ERR_STATUS_USER_DATA_ERR    0x4
#define GP_WB_ENUM_ERR_STATUS_SIF_MMU_ERR      0x5

/***************************
 * layout: event_enum
 ***************************/
/* event_state  */
#define GP_WB_ENUM_EVENT_STATE_INVALID                              0x0
#define GP_WB_ENUM_EVENT_STATE_SCHEDULED                            0x1
#define GP_WB_ENUM_EVENT_STATE_SCHEDULED_FOR_IMMEDIATE_EXECUTION    0x2
#define GP_WB_ENUM_EVENT_STATE_RESCHEDULED                          0x3
#define GP_WB_ENUM_EVENT_STATE_DONE                                 0x4

/* event_result  */
#define GP_WB_ENUM_EVENT_RESULT_UNKNOWN              0x0
#define GP_WB_ENUM_EVENT_RESULT_EXECUTED_ON_TIME     0x1
#define GP_WB_ENUM_EVENT_RESULT_EXECUTED_TOO_LATE    0x2
#define GP_WB_ENUM_EVENT_RESULT_MISSED_TOO_LATE      0x3

/* event_type  */
#define GP_WB_ENUM_EVENT_TYPE_MAC_RX_ON0                      0x00
#define GP_WB_ENUM_EVENT_TYPE_MAC_RX_ON1                      0x01
#define GP_WB_ENUM_EVENT_TYPE_MAC_RX_ON2                      0x02
#define GP_WB_ENUM_EVENT_TYPE_MAC_RX_ON3                      0x03
#define GP_WB_ENUM_EVENT_TYPE_MAC_RX_OFF0                     0x04
#define GP_WB_ENUM_EVENT_TYPE_MAC_RX_OFF1                     0x05
#define GP_WB_ENUM_EVENT_TYPE_MAC_RX_OFF2                     0x06
#define GP_WB_ENUM_EVENT_TYPE_MAC_RX_OFF3                     0x07
#define GP_WB_ENUM_EVENT_TYPE_MAC_TX_QUEUE0                   0x08
#define GP_WB_ENUM_EVENT_TYPE_MAC_TX_QUEUE1                   0x09
#define GP_WB_ENUM_EVENT_TYPE_MAC_TX_QUEUE2                   0x0A
#define GP_WB_ENUM_EVENT_TYPE_MAC_TX_QUEUE3                   0x0B
#define GP_WB_ENUM_EVENT_TYPE_MAC_TX_UNTIMED_QUEUE_HALT       0x0C
#define GP_WB_ENUM_EVENT_TYPE_MAC_TX_UNTIMED_QUEUE_RESUME     0x0D
#define GP_WB_ENUM_EVENT_TYPE_ADC_START                       0x0E
#define GP_WB_ENUM_EVENT_TYPE_KEYSCAN_START                   0x0F
#define GP_WB_ENUM_EVENT_TYPE_RESET_INTERNAL_UC               0x10
#define GP_WB_ENUM_EVENT_TYPE_RESET_EXTERNAL_UC               0x11
#define GP_WB_ENUM_EVENT_TYPE_RESET_DEVICE                    0x12
#define GP_WB_ENUM_EVENT_TYPE_COEX_ITF_SET_REQUEST_EXT_A      0x20
#define GP_WB_ENUM_EVENT_TYPE_COEX_ITF_UNSET_REQUEST_EXT_A    0x21
#define GP_WB_ENUM_EVENT_TYPE_COEX_ITF_SET_REQUEST_EXT_B      0x22
#define GP_WB_ENUM_EVENT_TYPE_COEX_ITF_UNSET_REQUEST_EXT_B    0x23
#define GP_WB_ENUM_EVENT_TYPE_COEX_ITF_SET_REQUEST_INT        0x24
#define GP_WB_ENUM_EVENT_TYPE_COEX_ITF_UNSET_REQUEST_INT      0x25
#define GP_WB_ENUM_EVENT_TYPE_UPDATE_DITHER_SEED              0x30
#define GP_WB_ENUM_EVENT_TYPE_IR_MODULATION_ON                0x40
#define GP_WB_ENUM_EVENT_TYPE_IR_MODULATION_OFF               0x41
#define GP_WB_ENUM_EVENT_TYPE_IR_START                        0x42
#define GP_WB_ENUM_EVENT_TYPE_IR_SET_ALT_CARRIER              0x43
#define GP_WB_ENUM_EVENT_TYPE_IR_UNSET_ALT_CARRIER            0x44
#define GP_WB_ENUM_EVENT_TYPE_UNSET_PIN_0                     0x80
#define GP_WB_ENUM_EVENT_TYPE_SET_PIN_0                       0x81
#define GP_WB_ENUM_EVENT_TYPE_TOGGLE_PIN_0                    0x82
#define GP_WB_ENUM_EVENT_TYPE_PULSE_PIN_0                     0x83
#define GP_WB_ENUM_EVENT_TYPE_UNSET_PIN_1                     0x90
#define GP_WB_ENUM_EVENT_TYPE_SET_PIN_1                       0x91
#define GP_WB_ENUM_EVENT_TYPE_TOGGLE_PIN_1                    0x92
#define GP_WB_ENUM_EVENT_TYPE_PULSE_PIN_1                     0x93
#define GP_WB_ENUM_EVENT_TYPE_UNSET_PIN_2                     0xA0
#define GP_WB_ENUM_EVENT_TYPE_SET_PIN_2                       0xA1
#define GP_WB_ENUM_EVENT_TYPE_TOGGLE_PIN_2                    0xA2
#define GP_WB_ENUM_EVENT_TYPE_PULSE_PIN_2                     0xA3
#define GP_WB_ENUM_EVENT_TYPE_UNSET_PIN_3                     0xB0
#define GP_WB_ENUM_EVENT_TYPE_SET_PIN_3                       0xB1
#define GP_WB_ENUM_EVENT_TYPE_TOGGLE_PIN_3                    0xB2
#define GP_WB_ENUM_EVENT_TYPE_PULSE_PIN_3                     0xB3
#define GP_WB_ENUM_EVENT_TYPE_DUMMY                           0xFF

/***************************
 * layout: rib_enum
 ***************************/
/* simple_trc_state  */
#define GP_WB_ENUM_SIMPLE_TRC_STATE_IDLE                     0x0
#define GP_WB_ENUM_SIMPLE_TRC_STATE_CAL_RX                   0x1
#define GP_WB_ENUM_SIMPLE_TRC_STATE_RX_ON_IDLE               0x2
#define GP_WB_ENUM_SIMPLE_TRC_STATE_EARLY_DATA_IND_IDLE      0x3
#define GP_WB_ENUM_SIMPLE_TRC_STATE_DATA_IND_IDLE            0x4
#define GP_WB_ENUM_SIMPLE_TRC_STATE_ACK_TX_WARMUP_IDLE       0x5
#define GP_WB_ENUM_SIMPLE_TRC_STATE_ACK_TX_PREPARE_IDLE      0x6
#define GP_WB_ENUM_SIMPLE_TRC_STATE_ACK_TX_PHY_IDLE          0x7
#define GP_WB_ENUM_SIMPLE_TRC_STATE_RX_WINDOW                0x8
#define GP_WB_ENUM_SIMPLE_TRC_STATE_EARLY_DATA_IND_WINDOW    0x9
#define GP_WB_ENUM_SIMPLE_TRC_STATE_DATA_IND_WINDOW          0xA
#define GP_WB_ENUM_SIMPLE_TRC_STATE_ACK_TX_WARMUP_WINDOW     0xB
#define GP_WB_ENUM_SIMPLE_TRC_STATE_ACK_TX_PREPARE_WINDOW    0xC
#define GP_WB_ENUM_SIMPLE_TRC_STATE_ACK_TX_PHY_WINDOW        0xD
#define GP_WB_ENUM_SIMPLE_TRC_STATE_CAL_TX                   0xE
#define GP_WB_ENUM_SIMPLE_TRC_STATE_TX_FLOW_DECIDE           0xF
#define GP_WB_ENUM_SIMPLE_TRC_STATE_TX_PREPARE               0x10
#define GP_WB_ENUM_SIMPLE_TRC_STATE_TX_WARMUP                0x11
#define GP_WB_ENUM_SIMPLE_TRC_STATE_TX_PHY                   0x12
#define GP_WB_ENUM_SIMPLE_TRC_STATE_CSMA_CCA_PHASE_WARMUP    0x13
#define GP_WB_ENUM_SIMPLE_TRC_STATE_CSMA_BACKOFF_PHASE       0x14
#define GP_WB_ENUM_SIMPLE_TRC_STATE_CSMA_CCA_PHASE           0x15
#define GP_WB_ENUM_SIMPLE_TRC_STATE_ED_SCAN_WARMUP           0x16
#define GP_WB_ENUM_SIMPLE_TRC_STATE_ED_SCAN_EXECUTE          0x17
#define GP_WB_ENUM_SIMPLE_TRC_STATE_WAIT_FOR_ACK_PREPARE     0x18
#define GP_WB_ENUM_SIMPLE_TRC_STATE_WAIT_FOR_ACK_WARMUP      0x19
#define GP_WB_ENUM_SIMPLE_TRC_STATE_WAIT_FOR_ACK_START       0x1A
#define GP_WB_ENUM_SIMPLE_TRC_STATE_WAIT_FOR_ACK_DONE        0x1B
#define GP_WB_ENUM_SIMPLE_TRC_STATE_CAL_POST_TX_DECIDE       0x1C
#define GP_WB_ENUM_SIMPLE_TRC_STATE_CAL_POST_TX              0x1D
#define GP_WB_ENUM_SIMPLE_TRC_STATE_TX_DONE                  0x1E

/***************************
 * layout: phy_enum
 ***************************/
/* rx_drop_reason  */
#define GP_WB_ENUM_RX_DROP_REASON_NO_DROP                                0x0
#define GP_WB_ENUM_RX_DROP_REASON_WRONG_FRAME_TYPE                       0x1
#define GP_WB_ENUM_RX_DROP_REASON_WRONG_VERSION                          0x2
#define GP_WB_ENUM_RX_DROP_REASON_WRONG_SRC_PAN_ID                       0x3
#define GP_WB_ENUM_RX_DROP_REASON_WRONG_DST_PAN_ID                       0x4
#define GP_WB_ENUM_RX_DROP_REASON_WRONG_DST_ADDRESS                      0x5
#define GP_WB_ENUM_RX_DROP_REASON_WRONG_COMMAND                          0x6
#define GP_WB_ENUM_RX_DROP_REASON_WRONG_CRC                              0x7
#define GP_WB_ENUM_RX_DROP_REASON_WRONG_SRC_ADDRESSING_MODE              0x8
#define GP_WB_ENUM_RX_DROP_REASON_WRONG_DST_ADDRESSING_MODE              0x9
#define GP_WB_ENUM_RX_DROP_REASON_PACKET_ENDED_BEFORE_PROCESSING_DONE    0xA
#define GP_WB_ENUM_RX_DROP_REASON_NO_FREE_PBM                            0xB

/* fll_fsm  */
#define GP_WB_ENUM_FLL_FSM_IDLE                        0x0
#define GP_WB_ENUM_FLL_FSM_TX_COARSE_STEP_1            0x1
#define GP_WB_ENUM_FLL_FSM_TX_COARSE_STEP_2            0x2
#define GP_WB_ENUM_FLL_FSM_TX_COARSE_STEP_3            0x3
#define GP_WB_ENUM_FLL_FSM_TX_FINE_STEP_0_A            0x4
#define GP_WB_ENUM_FLL_FSM_TX_FINE_STEP_0_B            0x5
#define GP_WB_ENUM_FLL_FSM_TX_FINE_STEP_1              0x6
#define GP_WB_ENUM_FLL_FSM_TX_FINE_STEP_2              0x7
#define GP_WB_ENUM_FLL_FSM_RX_COARSE_STEP_0            0x8
#define GP_WB_ENUM_FLL_FSM_RX_FINE_STEP_1              0x9
#define GP_WB_ENUM_FLL_FSM_RX_FINE_STEP_2              0xA
#define GP_WB_ENUM_FLL_FSM_MI_STEP_0                   0xB
#define GP_WB_ENUM_FLL_FSM_MI_STEP_1_A                 0xC
#define GP_WB_ENUM_FLL_FSM_MI_STEP_1_B                 0xD
#define GP_WB_ENUM_FLL_FSM_LGC_A                       0xE
#define GP_WB_ENUM_FLL_FSM_LGC_B                       0xF
#define GP_WB_ENUM_FLL_FSM_LGC_C                       0x10
#define GP_WB_ENUM_FLL_FSM_DELAY_0                     0x11
#define GP_WB_ENUM_FLL_FSM_DELAY_1                     0x12
#define GP_WB_ENUM_FLL_FSM_MI_STEP_2_A                 0x13
#define GP_WB_ENUM_FLL_FSM_MI_STEP_2_B                 0x14
#define GP_WB_ENUM_FLL_FSM_LFF_INIT_TX                 0x15
#define GP_WB_ENUM_FLL_FSM_LFF_INIT_RX                 0x16
#define GP_WB_ENUM_FLL_FSM_FLL_SPEEDUP                 0x17
#define GP_WB_ENUM_FLL_FSM_FLL_CL                      0x18
#define GP_WB_ENUM_FLL_FSM_DONE                        0x19
#define GP_WB_ENUM_FLL_FSM_CHECK_VCO_FREQ              0x1A
#define GP_WB_ENUM_FLL_FSM_PREPARE_IDLE                0x1B
#define GP_WB_ENUM_FLL_FSM_PREPARE_TX_COARSE_STEP_1    0x1C
#define GP_WB_ENUM_FLL_FSM_PREPARE_TX_COARSE_STEP_2    0x1D
#define GP_WB_ENUM_FLL_FSM_PREPARE_TX_COARSE_STEP_3    0x1E
#define GP_WB_ENUM_FLL_FSM_PREPARE_TX_FINE_STEP_0_A    0x1F
#define GP_WB_ENUM_FLL_FSM_PREPARE_TX_FINE_STEP_0_B    0x20
#define GP_WB_ENUM_FLL_FSM_PREPARE_TX_FINE_STEP_1      0x21
#define GP_WB_ENUM_FLL_FSM_PREPARE_TX_FINE_STEP_2      0x22
#define GP_WB_ENUM_FLL_FSM_PREPARE_RX_COARSE_STEP_0    0x23
#define GP_WB_ENUM_FLL_FSM_PREPARE_RX_FINE_STEP_1      0x24
#define GP_WB_ENUM_FLL_FSM_PREPARE_RX_FINE_STEP_2      0x25
#define GP_WB_ENUM_FLL_FSM_PREPARE_MI_STEP_0           0x26
#define GP_WB_ENUM_FLL_FSM_PREPARE_MI_STEP_1_A         0x27
#define GP_WB_ENUM_FLL_FSM_PREPARE_MI_STEP_1_B         0x28
#define GP_WB_ENUM_FLL_FSM_PREPARE_LGC_A               0x29
#define GP_WB_ENUM_FLL_FSM_PREPARE_LGC_B               0x2A
#define GP_WB_ENUM_FLL_FSM_PREPARE_LGC_C               0x2B
#define GP_WB_ENUM_FLL_FSM_PREPARE_DELAY_0             0x2C
#define GP_WB_ENUM_FLL_FSM_PREPARE_DELAY_1             0x2D
#define GP_WB_ENUM_FLL_FSM_PREPARE_MI_STEP_2_A         0x2E
#define GP_WB_ENUM_FLL_FSM_PREPARE_MI_STEP_2_B         0x2F
#define GP_WB_ENUM_FLL_FSM_PREPARE_LFF_INIT_TX         0x30
#define GP_WB_ENUM_FLL_FSM_PREPARE_LFF_INIT_RX         0x31
#define GP_WB_ENUM_FLL_FSM_PREPARE_FLL_SPEEDUP         0x32
#define GP_WB_ENUM_FLL_FSM_PREPARE_FLL_CL              0x33
#define GP_WB_ENUM_FLL_FSM_PREPARE_DONE                0x34
#define GP_WB_ENUM_FLL_FSM_PREPARE_CHECK_VCO_FREQ      0x35

/* cp_charge_time  */
#define GP_WB_ENUM_CP_CHARGE_TIME_CP_CT_4_8_8      0x0
#define GP_WB_ENUM_CP_CHARGE_TIME_CP_CT_3_6_6      0x1
#define GP_WB_ENUM_CP_CHARGE_TIME_CP_CT_8_16_16    0x2
#define GP_WB_ENUM_CP_CHARGE_TIME_CP_CT_6_12_12    0x3

/* cp_sel  */
#define GP_WB_ENUM_CP_SEL_CP_7_5UA     0x0
#define GP_WB_ENUM_CP_SEL_CP_10UA      0x1
#define GP_WB_ENUM_CP_SEL_CP_3_75UA    0x2
#define GP_WB_ENUM_CP_SEL_CP_5UA       0x3

/* lff_int_coupling_factor  */
#define GP_WB_ENUM_LFF_INT_COUPLING_FACTOR_LFF_CF_64     0x0
#define GP_WB_ENUM_LFF_INT_COUPLING_FACTOR_LFF_CF_128    0x1
#define GP_WB_ENUM_LFF_INT_COUPLING_FACTOR_LFF_CF_256    0x2
#define GP_WB_ENUM_LFF_INT_COUPLING_FACTOR_LFF_CF_512    0x3

/* phy_state  */
#define GP_WB_ENUM_PHY_STATE_TRX_OFF    0x0
#define GP_WB_ENUM_PHY_STATE_RX_ON      0x1
#define GP_WB_ENUM_PHY_STATE_TX_ON      0x2
#define GP_WB_ENUM_PHY_STATE_CAL        0x3

/* rssi_offset_sel  */
#define GP_WB_ENUM_RSSI_OFFSET_SEL_RSSI_OFFSET_A    0x0
#define GP_WB_ENUM_RSSI_OFFSET_SEL_RSSI_OFFSET_B    0x1

/* receiver_mode  */
#define GP_WB_ENUM_RECEIVER_MODE_NORMAL       0x0
#define GP_WB_ENUM_RECEIVER_MODE_NORMAL_AD    0x1
#define GP_WB_ENUM_RECEIVER_MODE_LPL          0x2
#define GP_WB_ENUM_RECEIVER_MODE_LPL_AD       0x3
#define GP_WB_ENUM_RECEIVER_MODE_MCH          0x4
#define GP_WB_ENUM_RECEIVER_MODE_MCH_AD       0x5

/* RX_MANAGER_STATE  */
#define GP_WB_ENUM_RX_MANAGER_STATE_ENABLED            0x0
#define GP_WB_ENUM_RX_MANAGER_STATE_NEW_START          0x1
#define GP_WB_ENUM_RX_MANAGER_STATE_IDLE               0x2
#define GP_WB_ENUM_RX_MANAGER_STATE_OP_COR             0x3
#define GP_WB_ENUM_RX_MANAGER_STATE_WAIT_FOS_OFFSET    0x4
#define GP_WB_ENUM_RX_MANAGER_STATE_WAIT_SFD           0x5
#define GP_WB_ENUM_RX_MANAGER_STATE_RST_TRAINING       0x6
#define GP_WB_ENUM_RX_MANAGER_STATE_RST_FOS            0x7
#define GP_WB_ENUM_RX_MANAGER_STATE_RESTART_SYS        0x8
#define GP_WB_ENUM_RX_MANAGER_STATE_WFE_FRAME          0x9
#define GP_WB_ENUM_RX_MANAGER_STATE_PIP                0xA
#define GP_WB_ENUM_RX_MANAGER_STATE_PIP_REQ_FLUSH      0xB
#define GP_WB_ENUM_RX_MANAGER_STATE_RESTART_FOR_PIP    0xC
#define GP_WB_ENUM_RX_MANAGER_STATE_DUMMY_13           0xD
#define GP_WB_ENUM_RX_MANAGER_STATE_DUMMY_14           0xE
#define GP_WB_ENUM_RX_MANAGER_STATE_DUMMY_15           0xF

#endif //GP_WB_ENUM_H
