AEP_PrivateLen	EQU	32
AEP_INITIALIZE	EQU	0
AEP_SYSTEM_CRIT_SHUTDOWN	EQU	1
AEP_BOOT_COMPLETE	EQU	2
AEP_CONFIG_DCB	EQU	3
AEP_UNCONFIG_DCB	EQU	4
AEP_IOP_TIMEOUT	EQU	5
AEP_DEVICE_INQUIRY	EQU	6
AEP_HALF_SEC	EQU	7
AEP_1_SEC	EQU	8
AEP_2_SECS	EQU	9
AEP_4_SECS	EQU	10
AEP_DBG_DOT_CMD	EQU	11
AEP_ASSOCIATE_DCB	EQU	12
AEP_REAL_MODE_HANDOFF	EQU	13
AEP_SYSTEM_SHUTDOWN	EQU	14
AEP_UNINITIALIZE	EQU	15
AEP_DCB_LOCK	EQU	16
AEP_MOUNT_NOTIFY	EQU	17
AEP_CREATE_VRP	EQU	18
AEP_DESTROY_VRP	EQU	19
AEP_REFRESH_DRIVE	EQU	20
AEP_PEND_UNCONFIG_DCB	EQU	21
AEP_1E_VEC_UPDATE	EQU	22
AEP_CHANGE_RPM	EQU	23
AEP_QUERY_RPM	EQU	24
AEP_CONFIG_3MODE	EQU	25
AEP_SYSTEM_REBOOT	EQU	26
AEP_QUERY_CD_SPINDOWN	EQU	27
AEP_FUNC_POWER_MESSAGE	EQU	28
AEP_FUNC_ACPI_REQUEST	EQU	29
AEP_MAX_FUNC	EQU	29

AEPHDR	STRUC
AEP_func	DW	?
AEP_result	DW	?
AEP_ddb	DD	?
AEP_lgn	DB	?
AEP_align	DB	3 DUP (?)
AEPHDR	ENDS
AEP_SUCCESS	EQU	0
AEP_DEREGISTER	EQU	1
AEP_FAILURE	EQU	-1

AEP_bi_init	STRUC
AEP_bi_i_hdr	DB	SIZE AEPHDR DUP (?)
AEP_bi_reference_data	DD	?
AEP_bi_flags	DB	?
AEP_bi_i_max_target	DB	?
AEP_bi_i_max_lun	DB	?
AEP_bi_i_dcb	DD	?
AEP_bi_i_hdevnode	DD	?
AEP_bi_i_regkey	DD	?
AEP_bi_i_align	DB	1 DUP (?)
AEP_bi_init	ENDS
AEP_BI_FL_SCSI_SCAN_DOWN	EQU	01H
AEP_BI_FL_SEND_CONFIG_AGAIN	EQU	02H

AEP_inquiry_device	STRUC
AEP_i_d_hdr	DB	SIZE AEPHDR DUP (?)
AEP_i_d_dcb	DD	?
AEP_inquiry_device	ENDS
AEP_NO_INQ_DATA	EQU	1
AEP_NO_MORE_DEVICES	EQU	2

AEP_bi_uninit	STRUC
AEP_bi_u_hdr	DB	SIZE AEPHDR DUP (?)
AEP_bi_uninit	ENDS

AEP_boot_done	STRUC
AEP_b_d_hdr	DB	SIZE AEPHDR DUP (?)
AEP_boot_done	ENDS

AEP_dcb_config	STRUC
AEP_d_c_hdr	DB	SIZE AEPHDR DUP (?)
AEP_d_c_dcb	DD	?
AEP_dcb_config	ENDS

AEP_dcb_unconfig	STRUC
AEP_d_u_hdr	DB	SIZE AEPHDR DUP (?)
AEP_d_u_dcb	DD	?
AEP_dcb_unconfig	ENDS

AEP_dcb_unconfig_pend	STRUC
AEP_d_u_p_hdr	DB	SIZE AEPHDR DUP (?)
AEP_d_u_p_dcb	DD	?
AEP_dcb_unconfig_pend	ENDS

AEP_iop_timeout_occurred	STRUC
AEP_i_t_o_hdr	DB	SIZE AEPHDR DUP (?)
AEP_i_t_o_iop	DD	?
AEP_iop_timeout_occurred	ENDS

AEP_dot_command	STRUC
AEP_i_t_o_hdr	DB	SIZE AEPHDR DUP (?)
AEP_d_c_flags	DB	?
AEP_d_c_pkeyword	DD	?
AEP_d_c_key_len	DB	?
AEP_d_c_num_1	DD	?
AEP_d_c_num_2	DD	?
AEP_d_c_num_3	DD	?
AEP_d_c_reserved	DW	?
AEP_dot_command	ENDS
AEP_D_C_NUM_1_PRES	EQU	01H
AEP_D_C_NUM_2_PRES	EQU	02H
AEP_D_C_NUM_3_PRES	EQU	04H
MAX_DOT_CMD_KEYWORD_SIZE	EQU	16

AEP_sys_reboot	STRUC
AEP_s_r_hdr	DB	SIZE AEPHDR DUP (?)
AEP_sys_reboot	ENDS

AEP_sys_shutdown	STRUC
AEP_s_s_hdr	DB	SIZE AEPHDR DUP (?)
AEP_sys_shutdown	ENDS

AEP_sys_crit_shutdown	STRUC
AEP_s_c_s_hdr	DB	SIZE AEPHDR DUP (?)
AEP_sys_crit_shutdown	ENDS

AEP_assoc_dcb	STRUC
AEP_a_d_hdr	DB	SIZE AEPHDR DUP (?)
AEP_a_d_pdcb	DD	?
AEP_a_d_drives	DD	?
AEP_assoc_dcb	ENDS

AEP_rm_handoff	STRUC
AEP_r_m_h_hdr	DB	SIZE AEPHDR DUP (?)
AEP_rm_handoff	ENDS

AEP_lock_dcb	STRUC
AEP_d_l_hdr	DB	SIZE AEPHDR DUP (?)
AEP_d_l_pdcb	DD	?
AEP_d_l_drives	DD	?
AEP_d_l_designtr	DB	?
AEP_d_l_align	DB	3 DUP (?)
AEP_lock_dcb	ENDS

AEP_mnt_notify	STRUC
AEP_m_n_hdr	DB	SIZE AEPHDR DUP (?)
AEP_m_n_pvrp	DD	?
AEP_m_n_drivemap	DD	?
AEP_m_n_drive	DD	?
AEP_m_n_effective_drive	DD	?
AEP_m_n_actual_drive	DD	?
AEP_mnt_notify	ENDS
AEP_M_N_SUCCESS	EQU	AEP_SUCCESS

AEP_vrp_create_destroy	STRUC
AEP_v_cd_hdr	DB	SIZE AEPHDR DUP (?)
AEP_v_cd_pvrp	DD	?
AEP_v_cd_drive	DD	?
AEP_vrp_create_destroy	ENDS

AEP_drive_refresh	STRUC
AEP_d_r_hdr	DB	SIZE AEPHDR DUP (?)
AEP_d_r_drive	DD	?
AEP_drive_refresh	ENDS

AEP_update_1e_vec	STRUC
AEP_u_1e_hdr	DB	SIZE AEPHDR DUP (?)
AEP_update_1e_vec	ENDS

AEP_rpm_change	STRUC
AEP_rc_hdr	DB	SIZE AEPHDR DUP (?)
AEP_rc_speed	DW	?
AEP_rc_dcb	DD	?
AEP_rpm_change	ENDS
AEP_RC_360_RPM	EQU	0001H
AEP_RC_300_RPM	EQU	0002H

AEP_rpm_inquiry	STRUC
AEP_ri_hdr	DB	SIZE AEPHDR DUP (?)
AEP_ri_speed	DW	?
AEP_ri_dcb	DD	?
AEP_rpm_inquiry	ENDS

AEP_3mode_config	STRUC
AEP_3c_hdr	DB	SIZE AEPHDR DUP (?)
AEP_3c_flag	DW	?
AEP_3c_wait	DW	?
AEP_3c_dcb	DD	?
AEP_3mode_config	ENDS

AEP_QUERY_CD_SPIN	STRUC
AEP_qcds_hdr	DB	SIZE AEPHDR DUP (?)
AEP_qcds_result	DD	?
AEP_QUERY_CD_SPIN	ENDS

_AEP_POWER_MESSAGE	STRUC
AEP_hdr	DB	SIZE AEPHDR DUP (?)
ulSubFunction	DD	?
devNode	DD	?
_AEP_POWER_MESSAGE	ENDS

_AEP_ACPI_REQUEST	STRUC
AEP_hdr	DB	SIZE AEPHDR DUP (?)
aep_acpi_ulSubFunction	DD	?
aep_acpi_pDdb	DD	?
aep_acpi_pDcb	DD	?
aep_acpi_ulInputLength	DD	?
aep_acpi_pbInputBuffer	DD	?
aep_acpi_ulOutputLength	DD	?
aep_acpi_pbOutputBuffer	DD	?
_AEP_ACPI_REQUEST	ENDS
AEP_SUBF_ACPI_SET_TASK_FILES	EQU	(1)
