ifndef	_VSD_H
_VSD_H	EQU	1
Begin_Service_Table VSD, VxD
VSD_Service  VSD_Get_Version, LOCAL
VSD_Service  VSD_Bell, LOCAL
VSD_Service  VSD_SoundOn, LOCAL
VSD_Service  VSD_TakeSoundPort, LOCAL
VSD_Service  _VSD_Play_A_Tone, LOCAL
End_Service_Table VSD, VxD
endif
