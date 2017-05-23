
/*
 *
 *   SM restore for GP565 SDK 40 pin
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
 *     0!                         $Header:  $
 *    M'   GreenPeak              $Change:  $
 *   0'         Technologies      $DateTime:  $
 *  F
 */
 
 
/* plme */
    clu.ds #(0x0002),   %r2, %r2 //cca_ed_thresh
/* rx */
    clu.ds #(0x0080),   %r2, %r2 //rx_antsel_int|rx_antsel_ext
    clu.ds #(0x0083),   %r2, %r2 //ant_use|en_pip|lqi_range_switch
    clu.ds #(0x0084),   %r2, %r2 //fll_freq_comp_sign|use_bbprx_duty_cycling|rx_mode
/* tx */
    clu.ds #(0x00C5),   %r2, %r2 //en_inv_msk|en_cont_mod|en_tx_pa_on|en_tx_noise|bypass_tx_delay
    clu.ds #(0x00C8),   %r2, %r2 //txma_var_delay
/* trx */
    clu.ds #(0x010A),   %r2, %r2 //antsw_pinmap|antswn_pinmap|mode_ctrl_0_pinmap|mode_ctrl_1_pinmap|mode_ctrl_2_pinmap|mode_ctrl_3_pinmap
    clu.ds #(0x0112),   %r2, %r2 //fll_cl_err_gain|fll_msk_delay
    clu.ds #(0x011C),   %r2, %r2 //fll_lock_enable|fll_lff_bypass|fll_fsm_go_nxt_state_en|fll_fsm_prepare_states_dis|fll_cl_start_lff_clr_and_en|fll_insert_delay_state
/* gpio */
    clu.ds #(0x0201),   %r2, %r2 //gpio0_direction|gpio1_direction|gpio2_direction|gpio3_direction|gpio4_direction|gpio5_direction|gpio6_direction|gpio7_direction
    clu.ds #(0x0202),   %r2, %r2 //gpio8_direction|gpio9_direction|gpio10_direction|gpio11_direction|gpio12_direction|gpio13_direction|gpio14_direction|gpio15_direction
    clu.ds #(0x0203),   %r2, %r2 //gpio16_direction|gpio17_direction|gpio18_direction|gpio19_direction|gpio20_direction|gpio21_direction|gpio22_direction|gpio23_direction
    clu.ds #(0x0204),   %r2, %r2 //gpio24_direction|gpio25_direction|gpio26_direction|gpio27_direction|gpio28_direction|gpio29_direction
    clu.ds #(0x0205),   %r2, %r2 //gpio0_output_value|gpio1_output_value|gpio2_output_value|gpio3_output_value|gpio4_output_value|gpio5_output_value|gpio6_output_value|gpio7_output_value
    clu.ds #(0x0206),   %r2, %r2 //gpio8_output_value|gpio9_output_value|gpio10_output_value|gpio11_output_value|gpio12_output_value|gpio13_output_value|gpio14_output_value|gpio15_output_value
    clu.ds #(0x0207),   %r2, %r2 //gpio16_output_value|gpio17_output_value|gpio18_output_value|gpio19_output_value|gpio20_output_value|gpio21_output_value|gpio22_output_value|gpio23_output_value
    clu.ds #(0x0208),   %r2, %r2 //gpio24_output_value|gpio25_output_value|gpio26_output_value|gpio27_output_value|gpio28_output_value|gpio29_output_value
/* adcif */
    clu.ds #(0x0240),   %r2, %r2 //trigger_mode|nbr_of_rejected_conversions|nbr_of_slots_in_cycle
    clu.ds #(0x0241),   %r2, %r2 //slot_a_channel|slot_a_post_to_rssi|slot_a_post_to_buffer_a|slot_a_post_to_buffer_b|slot_a_post_to_fifo
    clu.ds #(0x0242),   %r2, %r2 //slot_b_channel|slot_b_post_to_rssi|slot_b_post_to_buffer_a|slot_b_post_to_buffer_b|slot_b_post_to_fifo
    clu.ds #(0x0243),   %r2, %r2 //slot_c_channel|slot_c_post_to_rssi|slot_c_post_to_buffer_a|slot_c_post_to_buffer_b|slot_c_post_to_fifo
    clu.ds #(0x0244),   %r2, %r2 //slot_d_channel|slot_d_post_to_rssi|slot_d_post_to_buffer_a|slot_d_post_to_buffer_b|slot_d_post_to_fifo
/* uart */
    clu.ds #(0x0284),   %r2, %r2 //baud_rate
    clu.ds #(0x0284+1), %r2, %r2 //baud_rate_1
    clu.ds #(0x0286),   %r2, %r2 //data_bits|parity|stop_bits|use_sync_bit|rx_enable|bit_reverse_reply
    clu.ds #(0x0286+1), %r2, %r2 //config_1
    clu.ds #(0x0288),   %r2, %r2 //rx_map|tx_map
/* keypad_scan */
    clu.ds #(0x02C0),   %r2, %r2 //keypad_enable|sensitive_during_active|prolonged_row_drive|keyp_select_alternative_pin_map|sso_spread_enable|enable_clk_keypad_by_uc
    clu.ds #(0x02C1),   %r2, %r2 //matrix_scans
    clu.ds #(0x02C2),   %r2, %r2 //matrix_senses
    clu.ds #(0x02C3),   %r2, %r2 //active_columns_charge_period
    clu.ds #(0x02C4),   %r2, %r2 //passive_columns_charge_period
    clu.ds #(0x02C5),   %r2, %r2 //row_drive_period
    clu.ds #(0x02CC),   %r2, %r2 //results_stable_counter
    clu.ds #(0x02CD),   %r2, %r2 //number_of_keys|single_key_scan|single_key_sense
    clu.ds #(0x02D0),   %r2, %r2 //key_vector_2
    clu.ds #(0x02D1),   %r2, %r2 //key_vector_3
    clu.ds #(0x02D2),   %r2, %r2 //key_vector_4
    clu.ds #(0x02D3),   %r2, %r2 //key_vector_5
    clu.ds #(0x02D4),   %r2, %r2 //key_vector_6
    clu.ds #(0x02D5),   %r2, %r2 //key_vector_7
    clu.ds #(0x02D8),   %r2, %r2 //retention_unmasked_keypad_interrupt|retention_results_updated|retention_results_changed|retention_results_valid
/* ir */
    clu.ds #(0x02E0),   %r2, %r2 //prescale_div
    clu.ds #(0x02E1),   %r2, %r2 //threshold
    clu.ds #(0x02E2),   %r2, %r2 //wrap
    clu.ds #(0x02E3),   %r2, %r2 //alt_threshold
    clu.ds #(0x02E4),   %r2, %r2 //alt_wrap
    clu.ds #(0x02E5),   %r2, %r2 //carrier_tu
    clu.ds #(0x02E6),   %r2, %r2 //timer_tu
    clu.ds #(0x02E7),   %r2, %r2 //modulation_mode|align_on_start|modulation_latch_on_start|modulation|repeat_sequence
    clu.ds #(0x02E8),   %r2, %r2 //latch_ram_sequence_start_ptr_and_len_on_start|latch_ram_sequence_start_ptr_and_len_on_repeat|toggle_alt_carrier_config_on_index_match|sensitive_for_es_start
    clu.ds #(0x02EA),   %r2, %r2 //ram_sequence_start_ptr
    clu.ds #(0x02EA+1), %r2, %r2 //ram_sequence_start_ptr_1
    clu.ds #(0x02EC),   %r2, %r2 //ram_sequence_len
    clu.ds #(0x02F3),   %r2, %r2 //output_pinmap|input_pinmap|output_drive|output_invert|input_invert
/* watchdog */
    clu.ds #(0x0315),   %r2, %r2 //freeze_when_uc_asleep|freeze_when_uc_break_point
/* led */
    clu.ds #(0x0320),   %r2, %r2 //prescale_div|use_short_time_base
    clu.ds #(0x0325),   %r2, %r2 //threshold_2
    clu.ds #(0x0326),   %r2, %r2 //threshold_3
    clu.ds #(0x0329),   %r2, %r2 //led2_output_pinmap|led2_output_drive|led2_output_invert|led2_allow_gotosleep_when_on
    clu.ds #(0x032A),   %r2, %r2 //led3_output_pinmap|led3_output_drive|led3_output_invert|led3_allow_gotosleep_when_on
/* rib */
    clu.ds #(0x0400),   %r2, %r2 //rx_pbm_ptr|rx_pbm_ptr_valid
    clu.ds #(0x040A),   %r2, %r2 //bypass_tx_delay|disable_radio_when_no_free_pbm|restart_packet_det_when_pip|disable_trc_clk_gating
    clu.ds #(0x040D),   %r2, %r2 //rx_on_when_idle_ch0|rx_on_when_idle_ch|rx_on_when_idle_ch1|rx_on_when_idle_ch2|rx_on_when_idle_ch3|rx_on_when_idle_ch4|rx_on_when_idle_ch5
    clu.ds #(0x040E),   %r2, %r2 //ack_request_disable|en_data_req_for_every_frame
    clu.ds #(0x040F),   %r2, %r2 //rx_fcs_check_on
    clu.ds #(0x041C),   %r2, %r2 //rx_on_req_en|packet_rx_req_en|ack_tx_req_en|es_req_req_en
    clu.ds #(0x041D),   %r2, %r2 //rx_on_prio|packet_rx_prio|ack_tx_prio|es_req_prio
    clu.ds #(0x041E),   %r2, %r2 //delayed_start|csma_ca_hold|treat_as_cca_fail|packet_disable_pa|ack_skip|ack_disable_pa
    clu.ds #(0x0425),   %r2, %r2 //etoex_data_0
    clu.ds #(0x0426),   %r2, %r2 //etoex_data_1
    clu.ds #(0x0427),   %r2, %r2 //etoex_data_2
    clu.ds #(0x0428),   %r2, %r2 //etoex_data_3
    clu.ds #(0x0429),   %r2, %r2 //etoex_data_4
    clu.ds #(0x042A),   %r2, %r2 //etoex_data_5
    clu.ds #(0x042B),   %r2, %r2 //etoex_data_6
    clu.ds #(0x042C),   %r2, %r2 //etoex_data_7
    clu.ds #(0x042D),   %r2, %r2 //etoex_rptr
    clu.ds #(0x042E),   %r2, %r2 //etoex_wrtr
    clu.ds #(0x0438),   %r2, %r2 //dst_pan_id_table_size
/* qta */
    clu.ds #(0x0480),   %r2, %r2 //pbm_ptr_0|status_0|persistance_0|valid_0
    clu.ds #(0x0481),   %r2, %r2 //pbm_ptr_1|status_1|persistance_1|valid_1
    clu.ds #(0x0482),   %r2, %r2 //pbm_ptr_2|status_2|persistance_2|valid_2
    clu.ds #(0x0483),   %r2, %r2 //pbm_ptr_3|status_3|persistance_3|valid_3
    clu.ds #(0x0484),   %r2, %r2 //pbm_ptr_4|status_4|persistance_4|valid_4
    clu.ds #(0x0485),   %r2, %r2 //pbm_ptr_5|status_5|persistance_5|valid_5
    clu.ds #(0x0486),   %r2, %r2 //pbm_ptr_6|status_6|persistance_6|valid_6
    clu.ds #(0x0487),   %r2, %r2 //pbm_ptr_7|status_7|persistance_7|valid_7
    clu.ds #(0x0488),   %r2, %r2 //vq_0
    clu.ds #(0x0489),   %r2, %r2 //vq_1
    clu.ds #(0x048A),   %r2, %r2 //vq_2
    clu.ds #(0x048B),   %r2, %r2 //vq_3
    clu.ds #(0x048C),   %r2, %r2 //vq_4
    clu.ds #(0x048D),   %r2, %r2 //vq_5
    clu.ds #(0x048E),   %r2, %r2 //vq_6
    clu.ds #(0x048F),   %r2, %r2 //vq_7
    clu.ds #(0x0490),   %r2, %r2 //retry_cnt
    clu.ds #(0x0498),   %r2, %r2 //ret_unmasked_qta_sch0_empty|ret_unmasked_qta_sch1_empty|ret_unmasked_qta_sch2_empty|ret_unmasked_qta_sch3_empty
/* prg */
    clu.ds #(0x04A0),   %r2, %r2 //enable_clk_prg_by_uc|enable_random_seed|enable_random_source
    clu.ds #(0x04A2),   %r2, %r2 //random_seed
    clu.ds #(0x04A2+1), %r2, %r2 //random_seed_1
/* msi */
    clu.ds #(0x0504),   %r2, %r2 //serial_itf_select
/* int_ctrl */
    clu.ds #(0x0548),   %r2, %r2 //mask_ext_es_interrupt|mask_ext_trc_interrupt|mask_ext_qta_interrupt|mask_ext_macfilt_interrupt|mask_ext_stbc_interrupt|mask_ext_mri_interrupt|mask_ext_gpio_interrupt|mask_ext_twims_interrupt|mask_ext_ssp_interrupt|mask_ext_phy_interrupt|mask_ext_rci_interrupt|mask_ext_uart_interrupt|mask_ext_kps_interrupt|mask_ext_spim_interrupt|mask_ext_ir_interrupt|mask_ext_watchdog_interrupt|mask_ext_ipcint2ext_interrupt|mask_ext_pwm_interrupt|mask_ext_dma_interrupt|mask_ext_adcif_interrupt
    clu.ds #(0x0548+1), %r2, %r2 //mask_ext_interrupts_1
    clu.ds #(0x0548+2), %r2, %r2 //mask_ext_interrupts_2
    clu.ds #(0x0550),   %r2, %r2 //mask_int_es_interrupt|mask_int_trc_interrupt|mask_int_qta_interrupt|mask_int_macfilt_interrupt|mask_int_stbc_interrupt|mask_int_mri_interrupt|mask_int_gpio_interrupt|mask_int_twims_interrupt|mask_int_ssp_interrupt|mask_int_phy_interrupt|mask_int_rci_interrupt|mask_int_uart_interrupt|mask_int_kps_interrupt|mask_int_spim_interrupt|mask_int_ir_interrupt|mask_int_watchdog_interrupt|mask_int_ipcext2int_interrupt|mask_int_pwm_interrupt|mask_int_dma_interrupt|mask_int_spisl_interrupt|mask_int_i2csl_interrupt|mask_int_adcif_interrupt
    clu.ds #(0x0550+1), %r2, %r2 //mask_int_interrupts_1
    clu.ds #(0x0550+2), %r2, %r2 //mask_int_interrupts_2
    clu.ds #(0x0554),   %r2, %r2 //mask_seq_macfilt_interrupt
    clu.ds #(0x0556),   %r2, %r2 //mask_es_event_interrupt
    clu.ds #(0x0556+1), %r2, %r2 //mask_es_event_interrupt_1
    clu.ds #(0x0558),   %r2, %r2 //mask_es_symbol_counter_valid_interrupt|mask_es_relative_event_interrupt|mask_es_external_event_interrupt|mask_dma_src_almost_empty_interrupt|mask_dma_dest_almost_full_interrupt|mask_dma_cpy_err_interrupt|mask_dma_src_underrun_interrupt|mask_dma_dest_overflow_interrupt
    clu.ds #(0x0559),   %r2, %r2 //mask_qta_sch0_empty_interrupt|mask_qta_sch1_empty_interrupt|mask_qta_sch2_empty_interrupt|mask_qta_sch3_empty_interrupt|mask_trc_generic_0_interrupt|mask_trc_generic_1_interrupt|mask_trc_generic_2_interrupt|mask_trc_generic_3_interrupt
    clu.ds #(0x055A),   %r2, %r2 //mask_trc_pbm_full_interrupt|mask_trc_tx_started_interrupt|mask_trc_tx_ack_started_interrupt|mask_trc_phy_transition_not_done_interrupt|mask_macfilt_level_trigger_interrupt
    clu.ds #(0x055B),   %r2, %r2 //mask_stbc_pord_interrupt|mask_stbc_active_interrupt|mask_stbc_iso_tx_interrupt|mask_stbc_vrr_brownout_interrupt|mask_stbc_vlt_status_interrupt|mask_mri_write_done_interrupt|mask_ssp_done_interrupt
    clu.ds #(0x055C),   %r2, %r2 //mask_twims_done_interrupt|mask_twims_arb_lost_interrupt|mask_twims_clk_sync_interrupt|mask_phy_fll_vco_measure_done_interrupt|mask_phy_fll_out_of_lock_interrupt|mask_phy_fll_cp_out_of_range_interrupt|mask_kps_int_interrupt
    clu.ds #(0x055D),   %r2, %r2 //mask_rci_data_ind_interrupt|mask_rci_data_cnf_0_interrupt|mask_rci_data_cnf_1_interrupt|mask_rci_data_cnf_2_interrupt|mask_rci_data_cnf_3_interrupt|mask_watchdog_timer_expired_interrupt
    clu.ds #(0x055E),   %r2, %r2 //mask_ir_sequence_start_interrupt|mask_ir_sequence_repeat_interrupt|mask_ir_sequence_done_interrupt|mask_ir_index_match_interrupt
    clu.ds #(0x055F),   %r2, %r2 //mask_ipcint2ext_flag_interrupt|mask_ipcint2ext_flag_a_interrupt|mask_ipcint2ext_flag_b_interrupt|mask_ipcint2ext_flag_c_interrupt|mask_ipcint2ext_flag_d_interrupt|mask_ipcext2int_flag_interrupt|mask_ipcext2int_flag_a_interrupt|mask_ipcext2int_flag_b_interrupt|mask_ipcext2int_flag_c_interrupt|mask_ipcext2int_flag_d_interrupt
    clu.ds #(0x0560),   %r2, %r2 //mask_gpio_exti0_interrupt|mask_gpio_exti1_interrupt|mask_gpio_exti2_interrupt|mask_gpio_exti3_interrupt|mask_gpio_exti4_interrupt|mask_gpio_exti5_interrupt|mask_gpio_exti6_interrupt|mask_gpio_exti7_interrupt
    clu.ds #(0x0561),   %r2, %r2 //mask_spim_not_busy_interrupt|mask_spim_tx_not_full_interrupt|mask_spim_rx_not_empty_interrupt|mask_spim_rx_overrun_interrupt|mask_spisl_tx_not_full_interrupt|mask_spisl_rx_not_empty_interrupt|mask_spisl_tx_underrun_interrupt|mask_spisl_rx_overrun_interrupt
    clu.ds #(0x0562),   %r2, %r2 //mask_pwm_main_counter_wrap_interrupt|mask_pwm_carrier_counter_wrap_interrupt|mask_pwm_next_threshold_underrun_interrupt|mask_pwm_next_threshold_not_full_interrupt|mask_pwm_pwm0_threshold_match_interrupt|mask_pwm_pwm1_threshold_match_interrupt|mask_pwm_pwm2_threshold_match_interrupt|mask_pwm_pwm3_threshold_match_interrupt
    clu.ds #(0x0563),   %r2, %r2 //mask_pwm_timestamp_overrun_interrupt|mask_pwm_timestamp_not_empty_interrupt|mask_i2csl_tx_not_full_interrupt|mask_i2csl_rx_not_empty_interrupt|mask_i2csl_tx_stretch_interrupt|mask_i2csl_start_interrupt|mask_i2csl_stop_interrupt|mask_i2csl_slad_interrupt
    clu.ds #(0x0564),   %r2, %r2 //mask_adcif_buffer_a_updated_interrupt|mask_adcif_buffer_b_updated_interrupt|mask_adcif_fifo_not_empty_interrupt|mask_adcif_fifo_overrun_interrupt|mask_adcif_cycle_done_interrupt|mask_adcif_overvoltage_interrupt
    clu.ds #(0x0565),   %r2, %r2 //mask_uart_tx_not_busy_interrupt|mask_uart_tx_not_full_interrupt|mask_uart_rx_not_empty_interrupt|mask_uart_rx_overrun_interrupt|mask_uart_rx_parity_error_interrupt|mask_uart_rx_framing_error_interrupt
/* es */
    clu.ds #(0x0580),   %r2, %r2 //enable_oscillator_benchmark|force_init_time_reference_done|gray_counter_stable_on_falling_edge|disable_automatic_reference_point_update|reconversion_on_updates|disable_extrapolation|disable_conversion
    clu.ds #(0x0581),   %r2, %r2 //fast_sync_on_start|enable_sample_conversion_by_gpio_extern
    clu.ds #(0x0596),   %r2, %r2 //startup_symbol_time
    clu.ds #(0x0596+1), %r2, %r2 //startup_symbol_time_1
    clu.ds #(0x0596+2), %r2, %r2 //startup_symbol_time_2
    clu.ds #(0x0596+3), %r2, %r2 //startup_symbol_time_3
    clu.ds #(0x059A),   %r2, %r2 //long_startup_symbol_time
    clu.ds #(0x059A+1), %r2, %r2 //long_startup_symbol_time_1
    clu.ds #(0x059A+2), %r2, %r2 //long_startup_symbol_time_2
    clu.ds #(0x059A+3), %r2, %r2 //long_startup_symbol_time_3
    clu.ds #(0x059E),   %r2, %r2 //binary_gray_counter_reference
    clu.ds #(0x059E+1), %r2, %r2 //binary_gray_counter_reference_1
    clu.ds #(0x059E+2), %r2, %r2 //binary_gray_counter_reference_2
    clu.ds #(0x059E+3), %r2, %r2 //binary_gray_counter_reference_3
    clu.ds #(0x05A2),   %r2, %r2 //symbol_counter_reference
    clu.ds #(0x05A2+1), %r2, %r2 //symbol_counter_reference_1
    clu.ds #(0x05A2+2), %r2, %r2 //symbol_counter_reference_2
    clu.ds #(0x05A2+3), %r2, %r2 //symbol_counter_reference_3
    clu.ds #(0x05A6),   %r2, %r2 //calibration_factor
    clu.ds #(0x05A6+1), %r2, %r2 //calibration_factor_1
    clu.ds #(0x05A6+2), %r2, %r2 //calibration_factor_2
    clu.ds #(0x05A6+3), %r2, %r2 //calibration_factor_3
    clu.ds #(0x05AA),   %r2, %r2 //phase_compensation
    clu.ds #(0x05AA+1), %r2, %r2 //phase_compensation_1
    clu.ds #(0x05AA+2), %r2, %r2 //phase_compensation_2
    clu.ds #(0x05AA+3), %r2, %r2 //phase_compensation_3
    clu.ds #(0x05AE),   %r2, %r2 //enable_clk_time_reference_base_a_by_uc|enable_clk_time_reference_base_b_by_uc|enable_clk_time_reference_io_by_uc|enable_clk_time_reference_oscillator_benchmark_by_uc
    clu.ds #(0x05B0),   %r2, %r2 //valid_events
    clu.ds #(0x05B0+1), %r2, %r2 //valid_events_1
    clu.ds #(0x05B2),   %r2, %r2 //buffer_time
    clu.ds #(0x05B2+1), %r2, %r2 //buffer_time_1
    clu.ds #(0x05B4),   %r2, %r2 //priority_time
    clu.ds #(0x05B4+1), %r2, %r2 //priority_time_1
    clu.ds #(0x05B6),   %r2, %r2 //guard_time_sel
    clu.ds #(0x05B7),   %r2, %r2 //overwrite_execution_time_on_final_execution|relative_event_freeze_countdown|external_event_valid
    clu.ds #(0x05B8),   %r2, %r2 //external_event_type_to_be_executed
    clu.ds #(0x05C2),   %r2, %r2 //dither_seed
    clu.ds #(0x05C2+1), %r2, %r2 //dither_seed_1
    clu.ds #(0x05C4),   %r2, %r2 //dither_backoff_exponent_mask
    clu.ds #(0x05C6),   %r2, %r2 //enable_clk_event_handler_base_a_by_uc|enable_clk_event_handler_base_b_by_uc|enable_clk_event_handler_external_event_by_uc
    clu.ds #(0x05D0),   %r2, %r2 //retention_unmasked_symbol_counter_valid_interrupt
    clu.ds #(0x05D1),   %r2, %r2 //retention_unmasked_relative_event_interrupt|retention_unmasked_external_event_interrupt
    clu.ds #(0x05D2),   %r2, %r2 //retention_unmasked_event_interrupts
    clu.ds #(0x05D2+1), %r2, %r2 //retention_unmasked_event_interrupts_1
/* pmud */
    clu.ds #(0x0600),   %r2, %r2 //stby_mode|xosc_rdy_override|vdddig_rdy_override|disable_vmt_st|disable_vddb_cutoff|vmt_dis|vmt_throttle
/* iob */
    clu.ds #(0x0640),   %r2, %r2 //mcu_intoutn_pinmap|mcu_clk_pinmap|mcu_rstn_pinmap|fring_pinmap|digital_test_bus_from_ana_pinmap|adc_debug_pinmap
    clu.ds #(0x0641),   %r2, %r2 //dbg0_sel
    clu.ds #(0x0647),   %r2, %r2 //gpio_dbg_en
    clu.ds #(0x0648),   %r2, %r2 //gpio_0_cfg|gpio_1_cfg|gpio_2_cfg|gpio_3_cfg|gpio_4_cfg|gpio_5_cfg|gpio_6_cfg|gpio_7_cfg|gpio_8_cfg|gpio_9_cfg|gpio_10_cfg|gpio_11_cfg|gpio_12_cfg|gpio_13_cfg|gpio_14_cfg|gpio_15_cfg|gpio_16_cfg|gpio_17_cfg|gpio_18_cfg|gpio_19_cfg|gpio_20_cfg|gpio_21_cfg|gpio_22_cfg|gpio_23_cfg|gpio_24_cfg|gpio_25_cfg|gpio_26_cfg|gpio_27_cfg|gpio_28_cfg|gpio_29_cfg
    clu.ds #(0x0648+1), %r2, %r2 //gpio_pin_config_a_1
    clu.ds #(0x0648+2), %r2, %r2 //gpio_pin_config_a_2
    clu.ds #(0x0648+3), %r2, %r2 //gpio_pin_config_a_3
    clu.ds #(0x0648+4), %r2, %r2 //gpio_pin_config_a_4
    clu.ds #(0x0648+5), %r2, %r2 //gpio_pin_config_a_5
    clu.ds #(0x0648+6), %r2, %r2 //gpio_pin_config_a_6
    clu.ds #(0x0648+7), %r2, %r2 //gpio_pin_config_a_7
    clu.ds #(0x0650),   %r2, %r2 //gpio_0_3_slew_rate|gpio_0_3_schmitt_trigger|gpio_0_3_drive_strength|gpio_4_7_slew_rate|gpio_4_7_schmitt_trigger|gpio_4_7_drive_strength|gpio_8_11_slew_rate|gpio_8_11_schmitt_trigger|gpio_8_11_drive_strength|gpio_12_15_slew_rate|gpio_12_15_schmitt_trigger|gpio_12_15_drive_strength|gpio_16_19_slew_rate|gpio_16_19_schmitt_trigger|gpio_16_19_drive_strength|gpio_20_23_slew_rate|gpio_20_23_schmitt_trigger|gpio_20_23_drive_strength|gpio_24_27_slew_rate|gpio_24_27_schmitt_trigger|gpio_24_27_drive_strength|gpio_28_29_slew_rate|gpio_28_29_schmitt_trigger|gpio_28_29_drive_strength
    clu.ds #(0x0650+1), %r2, %r2 //gpio_pin_config_b_1
    clu.ds #(0x0650+2), %r2, %r2 //gpio_pin_config_b_2
    clu.ds #(0x0650+3), %r2, %r2 //gpio_pin_config_b_3
    clu.ds #(0x0654),   %r2, %r2 //gpio_0_pulsecfg|gpio_1_pulsecfg|gpio_2_pulsecfg|gpio_3_pulsecfg|gpio_4_pulsecfg|gpio_5_pulsecfg|gpio_6_pulsecfg|gpio_7_pulsecfg|gpio_8_pulsecfg|gpio_9_pulsecfg|gpio_10_pulsecfg|gpio_11_pulsecfg
    clu.ds #(0x0654+1), %r2, %r2 //gpio_pin_config_c_1
/* xap5_gp */
    clu.ds #(0x0684),   %r2, %r2 //sif_clk_mapping|sif_loadb_mapping
    clu.ds #(0x0685),   %r2, %r2 //sif_miso_mapping|sif_mosi_mapping
/* mm */
    clu.ds #(0x0700),   %r2, %r2 //pbm_opts_base_address
    clu.ds #(0x0700+1), %r2, %r2 //pbm_opts_base_address_1
    clu.ds #(0x0702),   %r2, %r2 //pbm_0_data_base_address
    clu.ds #(0x0702+1), %r2, %r2 //pbm_0_data_base_address_1
    clu.ds #(0x0704),   %r2, %r2 //pbm_1_data_base_address
    clu.ds #(0x0704+1), %r2, %r2 //pbm_1_data_base_address_1
    clu.ds #(0x0706),   %r2, %r2 //pbm_2_data_base_address
    clu.ds #(0x0706+1), %r2, %r2 //pbm_2_data_base_address_1
    clu.ds #(0x0708),   %r2, %r2 //pbm_3_data_base_address
    clu.ds #(0x0708+1), %r2, %r2 //pbm_3_data_base_address_1
    clu.ds #(0x070A),   %r2, %r2 //pbm_4_data_base_address
    clu.ds #(0x070A+1), %r2, %r2 //pbm_4_data_base_address_1
    clu.ds #(0x070C),   %r2, %r2 //pbm_5_data_base_address
    clu.ds #(0x070C+1), %r2, %r2 //pbm_5_data_base_address_1
    clu.ds #(0x070E),   %r2, %r2 //pbm_6_data_base_address
    clu.ds #(0x070E+1), %r2, %r2 //pbm_6_data_base_address_1
    clu.ds #(0x0710),   %r2, %r2 //pbm_7_data_base_address
    clu.ds #(0x0710+1), %r2, %r2 //pbm_7_data_base_address_1
    clu.ds #(0x0712),   %r2, %r2 //event_base_address
    clu.ds #(0x0712+1), %r2, %r2 //event_base_address_1
    clu.ds #(0x0714),   %r2, %r2 //gen_window_0_base_address
    clu.ds #(0x0714+1), %r2, %r2 //gen_window_0_base_address_1
    clu.ds #(0x0716),   %r2, %r2 //gen_window_1_base_address
    clu.ds #(0x0716+1), %r2, %r2 //gen_window_1_base_address_1
    clu.ds #(0x0718),   %r2, %r2 //gen_window_2_base_address
    clu.ds #(0x0718+1), %r2, %r2 //gen_window_2_base_address_1
    clu.ds #(0x071A),   %r2, %r2 //gen_window_3_base_address
    clu.ds #(0x071A+1), %r2, %r2 //gen_window_3_base_address_1
    clu.ds #(0x071C),   %r2, %r2 //circ_window_0_base_address
    clu.ds #(0x071C+1), %r2, %r2 //circ_window_0_base_address_1
    clu.ds #(0x071E),   %r2, %r2 //gen_window_var_0_base_address
    clu.ds #(0x071E+1), %r2, %r2 //gen_window_var_0_base_address_1
    clu.ds #(0x0720),   %r2, %r2 //gen_window_var_0_len
    clu.ds #(0x0720+1), %r2, %r2 //gen_window_var_0_len_1
    clu.ds #(0x0722),   %r2, %r2 //gen_window_var_1_base_address
    clu.ds #(0x0722+1), %r2, %r2 //gen_window_var_1_base_address_1
    clu.ds #(0x0724),   %r2, %r2 //gen_window_var_1_len
    clu.ds #(0x0724+1), %r2, %r2 //gen_window_var_1_len_1
/* pbm_adm */
    clu.ds #(0x0780),   %r2, %r2 //pbm_entry_uc_mask
    clu.ds #(0x0781),   %r2, %r2 //pbm_entry_rxmac_mask
    clu.ds #(0x0784),   %r2, %r2 //pbm_entry_claimed
/* standby */
    clu.ds #(0x0802),   %r2, %r2 //crc_enable|mw_enable|wb_backup_needed|vlt_status_compare_value
    clu.ds #(0x0818),   %r2, %r2 //rtgts_threshold
    clu.ds #(0x081D),   %r2, %r2 //rtgts_sw
    clu.ds #(0x081E),   %r2, %r2 //wait_for_nvm_dpdown|keep_nvm_on_during_active
    clu.ds #(0x0825),   %r2, %r2 //soft_por_on_iso_tx_detect|soft_por_on_vrr_brownout
