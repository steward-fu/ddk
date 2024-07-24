ifndef __DDKMMINI_INCLUDED__
__DDKMMINI_INCLUDED__	EQU	1
MINIPROC STRUC  
	DD	?
MINIPROC ENDS

_DDMINIVDDTABLE	STRUC
dwMiniVDDContext	DWORD	<>
vddGetIRQInfo	DD	?
vddIsOurIRQ	DD	?
vddEnableIRQ	DD	?
vddSkipNextField	DD	?
vddBobNextField	DD	?
vddSetState	DD	?
vddLock	DD	?
vddFlipOverlay	DD	?
vddFlipVideoPort	DD	?
vddGetPolarity	DD	?
vddSetSkipPattern	DD	?
vddGetCurrentAutoflip	DD	?
vddGetPreviousAutoflip	DD	?
vddTransfer	DD	?
vddGetTransferStatus	DD	?
_DDMINIVDDTABLE	ENDS
LPDDMINIVDDTABLE STRUC  
	DD	?
LPDDMINIVDDTABLE ENDS

_DDSURFACEDATA	STRUC
dwSize	DWORD	<>
ddsCaps	DWORD	<>
dwSurfaceOffset	DWORD	<>
fpLockPtr	DWORD	<>
dwWidth	DWORD	<>
dwHeight	DWORD	<>
lPitch	DD	?
dwOverlayFlags	DWORD	<>
dwOverlayOffset	DWORD	<>
dwOverlaySrcWidth	DWORD	<>
dwOverlaySrcHeight	DWORD	<>
dwOverlayDestWidth	DWORD	<>
dwOverlayDestHeight	DWORD	<>
dwVideoPortId	DWORD	<>
dwPhysicalPageTable	DWORD	<>
pPageTable	DWORD	<>
cPages	DWORD	<>
dwFormatFlags	DWORD	<>
dwFormatFourCC	DWORD	<>
dwFormatBitCount	DWORD	<>
dwRBitMask	DWORD	<>
dwGBitMask	DWORD	<>
dwBBitMask	DWORD	<>
dwSurfInternalFlags	DWORD	<>
dwIndex	DWORD	<>
dwRefCnt	DWORD	<>
dwDriverReserved1	DWORD	<>
dwDriverReserved2	DWORD	<>
dwDriverReserved3	DWORD	<>
_DDSURFACEDATA	ENDS
LPDDSURFACEDATA STRUC  
	DD	?
LPDDSURFACEDATA ENDS

DDVIDEOPORTDATA	STRUC
dwSize	DWORD	<>
dwVideoPortId	DWORD	<>
dwVPFlags	DWORD	<>
dwOriginOffset	DWORD	<>
dwHeight	DWORD	<>
dwVBIHeight	DWORD	<>
dwDriverReserved1	DWORD	<>
dwDriverReserved2	DWORD	<>
dwDriverReserved3	DWORD	<>
DDVIDEOPORTDATA	ENDS
LPDDVIDEOPORTDATA STRUC  
	DD	?
LPDDVIDEOPORTDATA ENDS

_DDGETIRQINFO	STRUC
dwSize	DWORD	<>
dwFlags	DWORD	<>
dwIRQNum	DWORD	<>
_DDGETIRQINFO	ENDS
IRQINFO_HANDLED	EQU	01H
IRQINFO_NOTHANDLED	EQU	02H

_DDENABLEIRQINFO	STRUC
dwSize	DWORD	<>
dwIRQSources	DWORD	<>
dwLine	DWORD	<>
IRQCallback	DWORD	<>
dwContext	DWORD	<>
_DDENABLEIRQINFO	ENDS

_DDFLIPVIDEOPORTINFO	STRUC
dwSize	DWORD	<>
lpVideoPortData	DWORD	<>
lpCurrentSurface	DWORD	<>
lpTargetSurface	DWORD	<>
dwFlipVPFlags	DWORD	<>
_DDFLIPVIDEOPORTINFO	ENDS

_DDFLIPOVERLAYINFO	STRUC
dwSize	DWORD	<>
lpCurrentSurface	DWORD	<>
lpTargetSurface	DWORD	<>
dwFlags	DWORD	<>
_DDFLIPOVERLAYINFO	ENDS

_DDSTATEININFO	STRUC
dwSize	DWORD	<>
lpSurfaceData	DWORD	<>
_DDSTATEININFO	ENDS

_DDSTATEOUTINFO	STRUC
dwSize	DWORD	<>
dwSoftwareAutoflip	DWORD	<>
dwSurfaceIndex	DWORD	<>
_DDSTATEOUTINFO	ENDS

_DDPOLARITYININFO	STRUC
dwSize	DWORD	<>
lpVideoPortData	DWORD	<>
_DDPOLARITYININFO	ENDS

_DDPOLARITYOUTINFO	STRUC
dwSize	DWORD	<>
bPolarity	DWORD	<>
_DDPOLARITYOUTINFO	ENDS

_DDLOCKININFO	STRUC
dwSize	DWORD	<>
lpSurfaceData	DWORD	<>
_DDLOCKININFO	ENDS

_DDLOCKOUTINFO	STRUC
dwSize	DWORD	<>
dwSurfacePtr	DWORD	<>
_DDLOCKOUTINFO	ENDS

_DDBOBINFO	STRUC
dwSize	DWORD	<>
lpSurface	DWORD	<>
_DDBOBINFO	ENDS

_DDSKIPINFO	STRUC
dwSize	DWORD	<>
lpVideoPortData	DWORD	<>
dwSkipFlags	DWORD	<>
_DDSKIPINFO	ENDS

_DDSETSKIPINFO	STRUC
dwSize	DWORD	<>
lpVideoPortData	DWORD	<>
dwPattern	DWORD	<>
dwPatternSize	DWORD	<>
_DDSETSKIPINFO	ENDS

_DDGETAUTOFLIPININFO	STRUC
dwSize	DWORD	<>
lpVideoPortData	DWORD	<>
_DDGETAUTOFLIPININFO	ENDS

_DDGETAUTOFLIPOUTINFO	STRUC
dwSize	DWORD	<>
dwSurfaceIndex	DWORD	<>
_DDGETAUTOFLIPOUTINFO	ENDS

_DDTRANSFERINFO	STRUC
dwSize	DWORD	<>
hSrcSurface	DWORD	<>
SrcLeft	DWORD	<>
SrcRight	DWORD	<>
SrcTop	DWORD	<>
SrcBottom	DWORD	<>
hDestSurface	DWORD	<>
DestLeft	DWORD	<>
DestRight	DWORD	<>
DestTop	DWORD	<>
DestBottom	DWORD	<>
_DDTRANSFERINFO	ENDS

_DDGETTRANSFERSTATUSINFO	STRUC
dwSize	DWORD	<>
hSurface	DWORD	<>
_DDGETTRANSFERSTATUSINFO	ENDS
DDIRQ_DISPLAY_VSYNC	EQU	00000001H
DDIRQ_RESERVED1	EQU	00000002H
DDIRQ_VPORT0_VSYNC	EQU	00000004H
DDIRQ_VPORT0_LINE	EQU	00000008H
DDIRQ_VPORT1_VSYNC	EQU	00000010H
DDIRQ_VPORT1_LINE	EQU	00000020H
DDIRQ_VPORT2_VSYNC	EQU	00000040H
DDIRQ_VPORT2_LINE	EQU	00000080H
DDIRQ_VPORT3_VSYNC	EQU	00000100H
DDIRQ_VPORT3_LINE	EQU	00000200H
DDIRQ_VPORT4_VSYNC	EQU	00000400H
DDIRQ_VPORT4_LINE	EQU	00000800H
DDIRQ_VPORT5_VSYNC	EQU	00001000H
DDIRQ_VPORT5_LINE	EQU	00002000H
DDIRQ_VPORT6_VSYNC	EQU	00004000H
DDIRQ_VPORT6_LINE	EQU	00008000H
DDIRQ_VPORT7_VSYNC	EQU	00010000H
DDIRQ_VPORT7_LINE	EQU	00020000H
DDIRQ_VPORT8_VSYNC	EQU	00040000H
DDIRQ_VPORT8_LINE	EQU	00080000H
DDIRQ_VPORT9_VSYNC	EQU	00010000H
DDIRQ_VPORT9_LINE	EQU	00020000H
DDSKIP_SKIPNEXT	EQU	1
DDSKIP_ENABLENEXT	EQU	2
DDVPFLIP_VIDEO	EQU	00000001H
DDVPFLIP_VBI	EQU	00000002H
MAX_DDKM_DEVICES	EQU	9
DDKMVP_CREATE	EQU	0001H
DDKMVP_RELEASE	EQU	0002H
DDKMVP_UPDATE	EQU	0004H
DDKMVP_ON	EQU	0008H
DDKMVP_AUTOFLIP	EQU	0010H
DDKMVP_BOB	EQU	0020H
DDKMVP_NOIRQ	EQU	0040H
DDKMVP_NOSKIP	EQU	0080H
DDKMDF_IN_USE	EQU	00000001H
DDKMDF_CAN_BOB_INTERLEAVED	EQU	00000002H
DDKMDF_CAN_BOB_NONINTERLEAVED	EQU	00000004H
DDKMDF_NOSTATE	EQU	00000008H
DDKMDF_TRANSITION	EQU	00000010H
ACTION_SKIP	EQU	0001H
ACTION_RESTORE_SKIP	EQU	0002H
ACTION_BOB	EQU	0004H
ACTION_FLIP	EQU	0008H
ACTION_STATE	EQU	0010H
STATUS_SKIPPED_LAST	EQU	0001H
STATUS_FLIP_NEXT	EQU	0002H
STATUS_FIRST_SKIP	EQU	0004H
DDKMSF_STATE_SET	EQU	00000001H
DDKMSF_TRANSFER	EQU	00000002H
MINIPROC STRUC  
	DD	?
MINIPROC ENDS
EVENTPROC STRUC  
	DD	?
EVENTPROC ENDS

_KMEVENTNODE	STRUC
dwEvents	DWORD	<>
pfnCallback	DD	?
dwParam1	DWORD	<>
dwParam2	DWORD	<>
dwContext	DWORD	<>
lpNext	DD	?
_KMEVENTNODE	ENDS
LPKMEVENTNODE STRUC  
	DD	?
LPKMEVENTNODE ENDS

_KMVPEDATA	STRUC
ddvpData	DDVIDEOPORTDATA	<>
dwInternalFlags	DWORD	<>
dwNumAutoflip	DWORD	<>
dwSurfaces	DWORD	10 DUP (<>)
dwVBISurfaces	DWORD	10 DUP (<>)
dwIRQCnt_VPSYNC	DWORD	<>
dwIRQCnt_VPLine	DWORD	<>
dwIRQLine	DWORD	<>
dwCurrentField	DWORD	<>
dwSkipPattern	DWORD	<>
dwSkipPatternSize	DWORD	<>
dwSkipStartField	DWORD	<>
dwSkipBit	DWORD	<>
dwActions	DWORD	<>
dwStatus	DWORD	<>
dwCurrentBuffer	DWORD	<>
dwNewState	DWORD	<>
dwStateStartField	DWORD	<>
dwRefCnt	DWORD	<>
_KMVPEDATA	ENDS
LPKMVPEDATA STRUC  
	DD	?
LPKMVPEDATA ENDS

_KMSTATEDATA	STRUC
dwDeviceFlags	DWORD	<>
dwContext	DWORD	<>
dwListHandle	DWORD	<>
lpVideoPort	DD	?
dwHigh	DWORD	<>
dwLow	DWORD	<>
dwMaxVideoPorts	DWORD	<>
dwNumVPInUse	DWORD	<>
dwIRQHandle	DWORD	<>
dwIRQFlags	DWORD	<>
dwIRQCnt_VSYNC	DWORD	<>
dwEventFlags	DWORD	<>
dwIRQEvents	DWORD	<>
dwRefCnt	DWORD	<>
lpEventList	DD	?
pfnGetIRQInfo	DD	?
pfnIsOurIRQ	DD	?
pfnEnableIRQ	DD	?
pfnSkipNextField	DD	?
pfnBobNextField	DD	?
pfnSetState	DD	?
pfnLock	DD	?
pfnFlipOverlay	DD	?
pfnFlipVideoPort	DD	?
pfnGetPolarity	DD	?
pfnSetSkipPattern	DD	?
pfnGetCurrentAutoflip	DD	?
pfnGetPreviousAutoflip	DD	?
pfnTransfer	DD	?
pfnGetTransferStatus	DD	?
_KMSTATEDATA	ENDS
LPKMSTATEDATA STRUC  
	DD	?
LPKMSTATEDATA ENDS
endif