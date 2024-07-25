ifndef _VDMAD_H_
_VDMAD_H_	EQU	1
ifndef  Not_VxD
Begin_Service_Table VDMAD, VxD
VDMAD_Service VDMAD_Get_Version, LOCAL
VDMAD_Service VDMAD_Virtualize_Channel, LOCAL
VDMAD_Service VDMAD_Get_Region_Info, LOCAL
VDMAD_Service VDMAD_Set_Region_Info, LOCAL
VDMAD_Service VDMAD_Get_Virt_State, LOCAL
VDMAD_Service VDMAD_Set_Virt_State, LOCAL
VDMAD_Service VDMAD_Set_Phys_State, LOCAL
VDMAD_Service VDMAD_Mask_Channel, LOCAL
VDMAD_Service VDMAD_UnMask_Channel, LOCAL
VDMAD_Service VDMAD_Lock_DMA_Region, LOCAL
VDMAD_Service VDMAD_Unlock_DMA_Region, LOCAL
VDMAD_Service VDMAD_Scatter_Lock, LOCAL
VDMAD_Service VDMAD_Scatter_Unlock, LOCAL
VDMAD_Service VDMAD_Reserve_Buffer_Space, LOCAL
VDMAD_Service VDMAD_Request_Buffer, LOCAL
VDMAD_Service VDMAD_Release_Buffer, LOCAL
VDMAD_Service VDMAD_Copy_To_Buffer, LOCAL
VDMAD_Service VDMAD_Copy_From_Buffer, LOCAL
VDMAD_Service VDMAD_Default_Handler, LOCAL
VDMAD_Service VDMAD_Disable_Translation, LOCAL
VDMAD_Service VDMAD_Enable_Translation, LOCAL
VDMAD_Service VDMAD_Get_EISA_Adr_Mode, LOCAL
VDMAD_Service VDMAD_Set_EISA_Adr_Mode, LOCAL
VDMAD_Service VDMAD_Unlock_DMA_Region_No_Dirty, LOCAL
VDMAD_Service VDMAD_Phys_Mask_Channel, LOCAL
VDMAD_Service VDMAD_Phys_Unmask_Channel, LOCAL
VDMAD_Service VDMAD_Unvirtualize_Channel, LOCAL
VDMAD_Service VDMAD_Set_IO_Address
VDMAD_Service VDMAD_Get_Phys_Count, LOCAL
VDMAD_Service VDMAD_Get_Phys_Status, LOCAL
VDMAD_Service VDMAD_Get_Max_Phys_Page, LOCAL
VDMAD_Service VDMAD_Set_Channel_Callbacks, LOCAL
VDMAD_Service VDMAD_Get_Virt_Count, LOCAL
VDMAD_Service VDMAD_Set_Virt_Count, LOCAL
VDMAD_Service VDMAD_Get_Virt_Address, LOCAL
VDMAD_Service VDMAD_Set_Virt_Address, LOCAL
End_Service_Table VDMAD, VxD
Function_Mask	EQU	0F0H
Channel_Mask	EQU	07H
DMA_type_verify	EQU	00H
DMA_type_write	EQU	04H
DMA_type_read	EQU	08H
DMA_AutoInit	EQU	10H
DMA_AdrDec	EQU	20H
DMA_demand_mode	EQU	00H
DMA_single_mode	EQU	40H
DMA_block_mode	EQU	80H
DMA_cascade	EQU	0C0H
DMA_mode_mask	EQU	0C0H
DMA_chan_sel	EQU	03H
DMA_masked	EQU	01H
DMA_masked_bit	EQU	0
DMA_requested	EQU	02H
DMA_req_bit	EQU	1
Programmed_IO	EQU	01H
PS2_AutoInit	EQU	02H
Transfer_Data	EQU	04H
Write_Mem	EQU	08H
_16_bit_xfer	EQU	40H
_16_bit_xfer_bit	EQU	6
endif

DMA_Descriptor_Struc	STRUC
DDS_size	DD	?
DDS_linear	DD	?
DDS_seg	DW	?
DDS_bufferID	DW	?
DDS_physical	DD	?
DMA_Descriptor_Struc	ENDS
DDS_sel equ word ptr [DDS_seg]


Extended_DDS_Struc	STRUC
EDDS_size	DD	?
EDDS_linear	DD	?
EDDS_seg	DW	?
EDDS_reserved	DW	?
DDS_avail	DW	?
DDS_used	DW	?
Extended_DDS_Struc	ENDS
DMA_Buf_Copy	EQU	00000002H
DMA_Buf_Copy_bit	EQU	1
DMA_No_Alloc_Buf	EQU	00000004H
DMA_No_Alloc_Buf_bit	EQU	2
DMA_Align_64K	EQU	00000010H
DMA_Align_64K_bit	EQU	4
DMA_Align_128K	EQU	00000020H
DMA_Align_128K_bit	EQU	5
DMA_Get_PgTable	EQU	00000040H
DMA_Get_PgTable_bit	EQU	6
DMA_Allow_NPs	EQU	00000080H
DMA_Allow_NPs_bit	EQU	7
DMA_Not_Contiguous	EQU	01H
DMA_Not_Aligned	EQU	02H
DMA_Lock_Failed	EQU	03H
DMA_No_Buffer	EQU	04H
DMA_Buffer_Too_Small	EQU	05H
DMA_Buffer_In_Use	EQU	06H
DMA_Invalid_Region	EQU	07H
DMA_Region_Not_Locked	EQU	08H
DMA_Table_Too_Small	EQU	09H
DMA_Invalid_Buffer	EQU	0AH
DMA_Copy_Out_Range	EQU	0BH
DMA_Invalid_Channel	EQU	0CH
DMA_Disable_Cnt_Overflow	EQU	0DH
DMA_Disable_Cnt_Underflow	EQU	0EH
DMA_Func_Not_Supported	EQU	0FH
DMA_NonZero_Reserved_Flags	EQU	10H
DMA_SL_Get_PgTable	EQU	000000001H
DMA_SL_Get_PgTable_bit	EQU	0
DMA_SL_Allow_NPs	EQU	000000002H
DMA_SL_Allow_NPs_bit	EQU	1
DMA_SL_Dont_Dirty	EQU	000000004H
DMA_SL_Dont_Dirty_bit	EQU	2
VDMAD_Callbacks_ReadNotify	EQU	00000001H
VDMAD_Callbacks_AllUnmaskedWritesNotify	EQU	00000002H
VDMAD_Callbacks_EDXhasportinfo	EQU	00000004H
VDMAD_Callbacks_CntrlrStatus	EQU	00000008H
VDMAD_ForcedCallbacks	EQU	00008000H
VDMAD_Callbacks_Valid	EQU	0000800fH
VDMAD_Callback_ReadCount	EQU	00000001H
VDMAD_Callback_ReadPage	EQU	00000002H
VDMAD_Callback_ReadBase	EQU	00000003H
endif
