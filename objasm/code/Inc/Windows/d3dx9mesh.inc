;Include file created by h2incX, Version B.01.01, 15/11/2018 21:48
;(C) 2018-today by Biterider, 2005-2009 by Japheth
;Source file: '\ObjAsm\Projects\h2IncX\header\d3dx9mesh.h', last modified: 28/08/2017 14:00
;Creation parameters: 

include d3dx9.inc
ifndef __D3DX9MESH_H__
  __D3DX9MESH_H__ equ <>
  DEFINE_GUID IID_ID3DXBaseMesh, 7ed943ddh, 52e8h, 40b5h, 0a8h, 0d8h, 76h, 68h, 5ch, 40h, 63h, 30h
  DEFINE_GUID IID_ID3DXMesh, 4020e5c2h, 1403h, 4929h, 88h, 3fh, 0e2h, 0e8h, 49h, 0fah, 0c1h, 95h
  DEFINE_GUID IID_ID3DXPMesh, 8875769ah, 0d579h, 4088h, 0aah, 0ebh, 53h, 4dh, 1ah, 0d8h, 4eh, 96h
  DEFINE_GUID IID_ID3DXSPMesh, 667ea4c7h, 0f1cdh, 4386h, 0b5h, 23h, 7ch, 2h, 90h, 0b8h, 3ch, 0c5h
  DEFINE_GUID IID_ID3DXSkinInfo, 11eaa540h, 0f9a6h, 4d49h, 0aeh, 6ah, 0e1h, 92h, 21h, 0f7h, 0ch, 0c4h
  DEFINE_GUID IID_ID3DXPatchMesh, 3ce6cc22h, 0dbf2h, 44f4h, 89h, 4dh, 0f9h, 0c3h, 4ah, 33h, 71h, 39h
  D3DXPATCHMESHTYPE typedef SDWORD
  D3DXPATCHMESH_RECT equ 001h
  D3DXPATCHMESH_TRI equ 002h
  D3DXPATCHMESH_NPATCH equ 003h
  D3DXPATCHMESH_FORCE_DWORD equ 7fffffffh
  _D3DXPATCHMESHTYPE typedef D3DXPATCHMESHTYPE
  
  _D3DXMESH typedef SDWORD
  D3DXMESH_32BIT equ 001h
  D3DXMESH_DONOTCLIP equ 002h
  D3DXMESH_POINTS equ 004h
  D3DXMESH_RTPATCHES equ 008h
  D3DXMESH_NPATCHES equ 4000h
  D3DXMESH_VB_SYSTEMMEM equ 010h
  D3DXMESH_VB_MANAGED equ 020h
  D3DXMESH_VB_WRITEONLY equ 040h
  D3DXMESH_VB_DYNAMIC equ 080h
  D3DXMESH_VB_SOFTWAREPROCESSING equ 8000h
  D3DXMESH_IB_SYSTEMMEM equ 100h
  D3DXMESH_IB_MANAGED equ 200h
  D3DXMESH_IB_WRITEONLY equ 400h
  D3DXMESH_IB_DYNAMIC equ 800h
  D3DXMESH_IB_SOFTWAREPROCESSING equ 10000h
  D3DXMESH_VB_SHARE equ 1000h
  D3DXMESH_USEHWONLY equ 2000h
  D3DXMESH_SYSTEMMEM equ 110h
  D3DXMESH_MANAGED equ 220h
  D3DXMESH_WRITEONLY equ 440h
  D3DXMESH_DYNAMIC equ 880h
  D3DXMESH_SOFTWAREPROCESSING equ 18000h
  
  _D3DXPATCHMESH typedef SDWORD
  D3DXPATCHMESH_DEFAULT equ 000
  
  _D3DXMESHSIMP typedef SDWORD
  D3DXMESHSIMP_VERTEX equ 1h
  D3DXMESHSIMP_FACE equ 2h
  
  D3DXCLEANTYPE typedef SDWORD
  D3DXCLEAN_BACKFACING equ 00000001h
  D3DXCLEAN_BOWTIES equ 00000002h
  D3DXCLEAN_SKINNING equ D3DXCLEAN_BACKFACING
  D3DXCLEAN_OPTIMIZATION equ D3DXCLEAN_BACKFACING
  D3DXCLEAN_SIMPLIFICATION equ D3DXCLEAN_BACKFACING or D3DXCLEAN_BOWTIES
  _D3DXCLEANTYPE typedef D3DXCLEANTYPE
  
  _MAX_FVF_DECL_SIZE typedef SDWORD
  MAX_FVF_DECL_SIZE equ MAXD3DDECLLENGTH + 1
  
  D3DXTANGENT typedef SDWORD
  D3DXTANGENT_WRAP_U equ 01h
  D3DXTANGENT_WRAP_V equ 02h
  D3DXTANGENT_WRAP_UV equ 03h
  D3DXTANGENT_DONT_NORMALIZE_PARTIALS equ 04h
  D3DXTANGENT_DONT_ORTHOGONALIZE equ 08h
  D3DXTANGENT_ORTHOGONALIZE_FROM_V equ 010h
  D3DXTANGENT_ORTHOGONALIZE_FROM_U equ 020h
  D3DXTANGENT_WEIGHT_BY_AREA equ 040h
  D3DXTANGENT_WEIGHT_EQUAL equ 080h
  D3DXTANGENT_WIND_CW equ 0100h
  D3DXTANGENT_CALCULATE_NORMALS equ 0200h
  D3DXTANGENT_GENERATE_IN_PLACE equ 0400h
  _D3DXTANGENT typedef D3DXTANGENT
  
  D3DXIMT typedef SDWORD
  D3DXIMT_WRAP_U equ 01h
  D3DXIMT_WRAP_V equ 02h
  D3DXIMT_WRAP_UV equ 03h
  _D3DXIMT typedef D3DXIMT
  
  D3DXUVATLAS typedef SDWORD
  D3DXUVATLAS_DEFAULT equ 00h
  D3DXUVATLAS_GEODESIC_FAST equ 01h
  D3DXUVATLAS_GEODESIC_QUALITY equ 02h
  _D3DXUVATLAS typedef D3DXUVATLAS
  
  LPD3DXBASEMESH typedef ptr ID3DXBaseMesh
  
  LPD3DXMESH typedef ptr ID3DXMesh
  
  LPD3DXPMESH typedef ptr ID3DXPMesh
  
  LPD3DXSPMESH typedef ptr ID3DXSPMesh
  
  LPD3DXSKININFO typedef ptr ID3DXSkinInfo
  
  LPD3DXPATCHMESH typedef ptr ID3DXPatchMesh
  
  LPD3DXTEXTUREGUTTERHELPER typedef ptr ID3DXTextureGutterHelper
  
  LPD3DXPRTBUFFER typedef ptr ID3DXPRTBuffer
  
  D3DXATTRIBUTERANGE struct
    AttribId DWORD ?
    FaceStart DWORD ?
    FaceCount DWORD ?
    VertexStart DWORD ?
    VertexCount DWORD ?
  D3DXATTRIBUTERANGE ends
  _D3DXATTRIBUTERANGE typedef D3DXATTRIBUTERANGE
  LPD3DXATTRIBUTERANGE typedef ptr D3DXATTRIBUTERANGE
  D3DXMATERIAL struct
    MatD3D D3DMATERIAL9 <>
    pTextureFilename LPSTR ?
  D3DXMATERIAL ends
  _D3DXMATERIAL typedef D3DXMATERIAL
  LPD3DXMATERIAL typedef ptr D3DXMATERIAL
  D3DXEFFECTDEFAULTTYPE typedef SDWORD
  D3DXEDT_STRING equ 1h
  D3DXEDT_FLOATS equ 2h
  D3DXEDT_DWORD equ 3h
  D3DXEDT_FORCEDWORD equ 7fffffffh
  _D3DXEFFECTDEFAULTTYPE typedef D3DXEFFECTDEFAULTTYPE
  
  D3DXEFFECTDEFAULT struct
    pParamName LPSTR ?
    Type_ D3DXEFFECTDEFAULTTYPE ?
    NumBytes DWORD ?
    pValue LPVOID ?
  D3DXEFFECTDEFAULT ends
  _D3DXEFFECTDEFAULT typedef D3DXEFFECTDEFAULT
  LPD3DXEFFECTDEFAULT typedef ptr D3DXEFFECTDEFAULT
  
  D3DXEFFECTINSTANCE struct
    pEffectFilename LPSTR ?
    NumDefaults DWORD ?
    pDefaults LPD3DXEFFECTDEFAULT ?
  D3DXEFFECTINSTANCE ends
  _D3DXEFFECTINSTANCE typedef D3DXEFFECTINSTANCE
  LPD3DXEFFECTINSTANCE typedef ptr D3DXEFFECTINSTANCE
  
  D3DXATTRIBUTEWEIGHTS struct
    Position FLOAT ?
    Boundary FLOAT ?
    Normal FLOAT ?
    Diffuse FLOAT ?
    Specular FLOAT ?
    Texcoord FLOAT 8 dup (?)
    Tangent FLOAT ?
    Binormal FLOAT ?
  D3DXATTRIBUTEWEIGHTS ends
  _D3DXATTRIBUTEWEIGHTS typedef D3DXATTRIBUTEWEIGHTS
  LPD3DXATTRIBUTEWEIGHTS typedef ptr D3DXATTRIBUTEWEIGHTS
  
  _D3DXWELDEPSILONSFLAGS typedef SDWORD
  D3DXWELDEPSILONS_WELDALL equ 1h
  D3DXWELDEPSILONS_WELDPARTIALMATCHES equ 2h
  D3DXWELDEPSILONS_DONOTREMOVEVERTICES equ 4h
  D3DXWELDEPSILONS_DONOTSPLIT equ 8h
  
  D3DXWELDEPSILONS struct
    Position FLOAT ?
    BlendWeights FLOAT ?
    Normal FLOAT ?
    PSize FLOAT ?
    Specular FLOAT ?
    Diffuse FLOAT ?
    Texcoord FLOAT 8 dup (?)
    Tangent FLOAT ?
    Binormal FLOAT ?
    TessFactor FLOAT ?
  D3DXWELDEPSILONS ends
  _D3DXWELDEPSILONS typedef D3DXWELDEPSILONS
  LPD3DXWELDEPSILONS typedef ptr D3DXWELDEPSILONS
  INTERFACE equ <ID3DXBaseMesh>
  DECLARE_INTERFACE_ ID3DXBaseMesh, IUnknown
    STD_METHOD QueryInterface, :THIS_, :ptr LPVOID
    STD_METHOD AddRef, :THIS_
    STD_METHOD Release, :THIS_
    STD_METHOD DrawSubset, :THIS_
    STD_METHOD GetNumFaces, :THIS_
    STD_METHOD GetNumVertices, :THIS_
    STD_METHOD GetFVF, :THIS_
    STD_METHOD GetDeclaration, :ptr THIS_
    STD_METHOD GetNumBytesPerVertex, :THIS_
    STD_METHOD GetOptions, :THIS_
    STD_METHOD GetDevice, :ptr THIS_
    STD_METHOD CloneMeshFVF, :THIS_, :DWORD, :LPDIRECT3DDEVICE9, :ptr LPD3DXMESH
    STD_METHOD CloneMesh, :THIS_, :ptr D3DVERTEXELEMENT9, :LPDIRECT3DDEVICE9, :ptr LPD3DXMESH
    STD_METHOD GetVertexBuffer, :ptr THIS_
    STD_METHOD GetIndexBuffer, :ptr THIS_
    STD_METHOD LockVertexBuffer, :THIS_, :ptr LPVOID
    STD_METHOD UnlockVertexBuffer, :THIS_
    STD_METHOD LockIndexBuffer, :THIS_, :ptr LPVOID
    STD_METHOD UnlockIndexBuffer, :THIS_
    STD_METHOD GetAttributeTable, :ptr THIS_, :ptr DWORD
    STD_METHOD ConvertPointRepsToAdjacency, :ptr THIS_, :ptr DWORD
    STD_METHOD ConvertAdjacencyToPointReps, :ptr THIS_, :ptr DWORD
    STD_METHOD GenerateAdjacency, :THIS_, :ptr DWORD
    STD_METHOD UpdateSemantics, :ptr THIS_
  END_DECLARE_INTERFACE_
  INTERFACE equ <ID3DXMesh>
  DECLARE_INTERFACE_ ID3DXMesh, ID3DXBaseMesh
    STD_METHOD QueryInterface, :THIS_, :ptr LPVOID
    STD_METHOD AddRef, :THIS_
    STD_METHOD Release, :THIS_
    STD_METHOD DrawSubset, :THIS_
    STD_METHOD GetNumFaces, :THIS_
    STD_METHOD GetNumVertices, :THIS_
    STD_METHOD GetFVF, :THIS_
    STD_METHOD GetDeclaration, :ptr THIS_
    STD_METHOD GetNumBytesPerVertex, :THIS_
    STD_METHOD GetOptions, :THIS_
    STD_METHOD GetDevice, :ptr THIS_
    STD_METHOD CloneMeshFVF, :THIS_, :DWORD, :LPDIRECT3DDEVICE9, :ptr LPD3DXMESH
    STD_METHOD CloneMesh, :THIS_, :ptr D3DVERTEXELEMENT9, :LPDIRECT3DDEVICE9, :ptr LPD3DXMESH
    STD_METHOD GetVertexBuffer, :ptr THIS_
    STD_METHOD GetIndexBuffer, :ptr THIS_
    STD_METHOD LockVertexBuffer, :THIS_, :ptr LPVOID
    STD_METHOD UnlockVertexBuffer, :THIS_
    STD_METHOD LockIndexBuffer, :THIS_, :ptr LPVOID
    STD_METHOD UnlockIndexBuffer, :THIS_
    STD_METHOD GetAttributeTable, :ptr THIS_, :ptr DWORD
    STD_METHOD ConvertPointRepsToAdjacency, :ptr THIS_, :ptr DWORD
    STD_METHOD ConvertAdjacencyToPointReps, :ptr THIS_, :ptr DWORD
    STD_METHOD GenerateAdjacency, :THIS_, :ptr DWORD
    STD_METHOD UpdateSemantics, :ptr THIS_
    STD_METHOD LockAttributeBuffer, :THIS_, :ptr ptr DWORD
    STD_METHOD UnlockAttributeBuffer, :THIS_
    STD_METHOD Optimize, :THIS_, :ptr DWORD, :ptr DWORD, :ptr DWORD, :ptr LPD3DXBUFFER, :ptr LPD3DXMESH
    STD_METHOD OptimizeInplace, :THIS_, :ptr DWORD, :ptr DWORD, :ptr DWORD, :ptr LPD3DXBUFFER
    STD_METHOD SetAttributeTable, :ptr THIS_, :DWORD
  END_DECLARE_INTERFACE_
  INTERFACE equ <ID3DXPMesh>
  DECLARE_INTERFACE_ ID3DXPMesh, ID3DXBaseMesh
    STD_METHOD QueryInterface, :THIS_, :ptr LPVOID
    STD_METHOD AddRef, :THIS_
    STD_METHOD Release, :THIS_
    STD_METHOD DrawSubset, :THIS_
    STD_METHOD GetNumFaces, :THIS_
    STD_METHOD GetNumVertices, :THIS_
    STD_METHOD GetFVF, :THIS_
    STD_METHOD GetDeclaration, :ptr THIS_
    STD_METHOD GetNumBytesPerVertex, :THIS_
    STD_METHOD GetOptions, :THIS_
    STD_METHOD GetDevice, :ptr THIS_
    STD_METHOD CloneMeshFVF, :THIS_, :DWORD, :LPDIRECT3DDEVICE9, :ptr LPD3DXMESH
    STD_METHOD CloneMesh, :THIS_, :ptr D3DVERTEXELEMENT9, :LPDIRECT3DDEVICE9, :ptr LPD3DXMESH
    STD_METHOD GetVertexBuffer, :ptr THIS_
    STD_METHOD GetIndexBuffer, :ptr THIS_
    STD_METHOD LockVertexBuffer, :THIS_, :ptr LPVOID
    STD_METHOD UnlockVertexBuffer, :THIS_
    STD_METHOD LockIndexBuffer, :THIS_, :ptr LPVOID
    STD_METHOD UnlockIndexBuffer, :THIS_
    STD_METHOD GetAttributeTable, :ptr THIS_, :ptr DWORD
    STD_METHOD ConvertPointRepsToAdjacency, :ptr THIS_, :ptr DWORD
    STD_METHOD ConvertAdjacencyToPointReps, :ptr THIS_, :ptr DWORD
    STD_METHOD GenerateAdjacency, :THIS_, :ptr DWORD
    STD_METHOD UpdateSemantics, :ptr THIS_
    STD_METHOD ClonePMeshFVF, :THIS_, :DWORD, :LPDIRECT3DDEVICE9, :ptr LPD3DXPMESH
    STD_METHOD ClonePMesh, :THIS_, :ptr D3DVERTEXELEMENT9, :LPDIRECT3DDEVICE9, :ptr LPD3DXPMESH
    STD_METHOD SetNumFaces, :THIS_
    STD_METHOD SetNumVertices, :THIS_
    STD_METHOD GetMaxFaces, :THIS_
    STD_METHOD GetMinFaces, :THIS_
    STD_METHOD GetMaxVertices, :THIS_
    STD_METHOD GetMinVertices, :THIS_
    STD_METHOD Save, :ptr THIS_, :ptr D3DXMATERIAL, :ptr D3DXEFFECTINSTANCE, :DWORD
    STD_METHOD Optimize, :THIS_, :ptr DWORD, :ptr DWORD, :ptr LPD3DXBUFFER, :ptr LPD3DXMESH
    STD_METHOD OptimizeBaseLOD, :THIS_, :ptr DWORD
    STD_METHOD TrimByFaces, :THIS_, :DWORD, :ptr DWORD, :ptr DWORD
    STD_METHOD TrimByVertices, :THIS_, :DWORD, :ptr DWORD, :ptr DWORD
    STD_METHOD GetAdjacency, :ptr THIS_
    STD_METHOD GenerateVertexHistory, :ptr THIS_
  END_DECLARE_INTERFACE_
  INTERFACE equ <ID3DXSPMesh>
  DECLARE_INTERFACE_ ID3DXSPMesh, IUnknown
    STD_METHOD QueryInterface, :THIS_, :ptr LPVOID
    STD_METHOD AddRef, :THIS_
    STD_METHOD Release, :THIS_
    STD_METHOD GetNumFaces, :THIS_
    STD_METHOD GetNumVertices, :THIS_
    STD_METHOD GetFVF, :THIS_
    STD_METHOD GetDeclaration, :ptr THIS_
    STD_METHOD GetOptions, :THIS_
    STD_METHOD GetDevice, :ptr THIS_
    STD_METHOD CloneMeshFVF, :THIS_, :DWORD, :LPDIRECT3DDEVICE9, :ptr DWORD, :ptr DWORD, :ptr LPD3DXMESH
    STD_METHOD CloneMesh, :THIS_, :ptr D3DVERTEXELEMENT9, :LPDIRECT3DDEVICE9, :ptr DWORD, :ptr DWORD, :ptr LPD3DXMESH
    STD_METHOD ClonePMeshFVF, :THIS_, :DWORD, :LPDIRECT3DDEVICE9, :ptr DWORD, :ptr FLOAT, :ptr LPD3DXPMESH
    STD_METHOD ClonePMesh, :THIS_, :ptr D3DVERTEXELEMENT9, :LPDIRECT3DDEVICE9, :ptr DWORD, :ptr FLOAT, :ptr LPD3DXPMESH
    STD_METHOD ReduceFaces, :THIS_
    STD_METHOD ReduceVertices, :THIS_
    STD_METHOD GetMaxFaces, :THIS_
    STD_METHOD GetMaxVertices, :THIS_
    STD_METHOD GetVertexAttributeWeights, :THIS_
    STD_METHOD GetVertexWeights, :ptr THIS_
  END_DECLARE_INTERFACE_
  UNUSED16 equ 0ffffh
  UNUSED32 equ 0ffffffffh
  _D3DXMESHOPT typedef SDWORD
  D3DXMESHOPT_COMPACT equ 01000000h
  D3DXMESHOPT_ATTRSORT equ 02000000h
  D3DXMESHOPT_VERTEXCACHE equ 04000000h
  D3DXMESHOPT_STRIPREORDER equ 08000000h
  D3DXMESHOPT_IGNOREVERTS equ 10000000h
  D3DXMESHOPT_DONOTSPLIT equ 20000000h
  D3DXMESHOPT_DEVICEINDEPENDENT equ 00400000h
  
  D3DXBONECOMBINATION struct
    AttribId DWORD ?
    FaceStart DWORD ?
    FaceCount DWORD ?
    VertexStart DWORD ?
    VertexCount DWORD ?
    BoneId POINTER ?
  D3DXBONECOMBINATION ends
  _D3DXBONECOMBINATION typedef D3DXBONECOMBINATION
  LPD3DXBONECOMBINATION typedef ptr D3DXBONECOMBINATION
  
  D3DXPATCHINFO struct
    PatchType D3DXPATCHMESHTYPE ?
    Degree D3DDEGREETYPE ?
    Basis D3DBASISTYPE ?
  D3DXPATCHINFO ends
  _D3DXPATCHINFO typedef D3DXPATCHINFO
  LPD3DXPATCHINFO typedef ptr D3DXPATCHINFO
  
  INTERFACE equ <ID3DXPatchMesh>
  DECLARE_INTERFACE_ ID3DXPatchMesh, IUnknown
    STD_METHOD QueryInterface, :THIS_, :ptr LPVOID
    STD_METHOD AddRef, :THIS_
    STD_METHOD Release, :THIS_
    STD_METHOD GetNumPatches, :THIS_
    STD_METHOD GetNumVertices, :THIS_
    STD_METHOD GetDeclaration, :ptr THIS_
    STD_METHOD GetControlVerticesPerPatch, :THIS_
    STD_METHOD GetOptions, :THIS_
    STD_METHOD GetDevice, :ptr THIS_
    STD_METHOD GetPatchInfo, :THIS_
    STD_METHOD GetVertexBuffer, :ptr THIS_
    STD_METHOD GetIndexBuffer, :ptr THIS_
    STD_METHOD LockVertexBuffer, :THIS_, :ptr LPVOID
    STD_METHOD UnlockVertexBuffer, :THIS_
    STD_METHOD LockIndexBuffer, :THIS_, :ptr LPVOID
    STD_METHOD UnlockIndexBuffer, :THIS_
    STD_METHOD LockAttributeBuffer, :THIS_, :ptr ptr DWORD
    STD_METHOD UnlockAttributeBuffer, :THIS_
    STD_METHOD GetTessSize, :THIS_, :DWORD, :ptr DWORD, :ptr DWORD
    STD_METHOD GenerateAdjacency, :THIS_
    STD_METHOD CloneMesh, :THIS_, :ptr D3DVERTEXELEMENT9, :ptr LPD3DXPATCHMESH
    STD_METHOD Optimize, :THIS_
    STD_METHOD SetDisplaceParam, :THIS_, :D3DTEXTUREFILTERTYPE, :D3DTEXTUREFILTERTYPE, :D3DTEXTUREFILTERTYPE, :D3DTEXTUREADDRESS, :DWORD
    STD_METHOD GetDisplaceParam, :ptr THIS_, :ptr D3DTEXTUREFILTERTYPE, :ptr D3DTEXTUREFILTERTYPE, :ptr D3DTEXTUREFILTERTYPE, :ptr D3DTEXTUREADDRESS, :ptr DWORD
    STD_METHOD Tessellate, :THIS_, :LPD3DXMESH
    STD_METHOD TessellateAdaptive, :ptr THIS_, :DWORD, :DWORD, :LPD3DXMESH
  END_DECLARE_INTERFACE_
  INTERFACE equ <ID3DXSkinInfo>
  DECLARE_INTERFACE_ ID3DXSkinInfo, IUnknown
    STD_METHOD QueryInterface, :THIS_, :ptr LPVOID
    STD_METHOD AddRef, :THIS_
    STD_METHOD Release, :THIS_
    STD_METHOD SetBoneInfluence, :THIS_, :DWORD, :ptr DWORD, :ptr FLOAT
    STD_METHOD SetBoneVertexInfluence, :THIS_, :DWORD, :REAL4
    STD_METHOD GetNumBoneInfluences, :THIS_
    STD_METHOD GetBoneInfluence, :THIS_, :ptr DWORD, :ptr FLOAT
    STD_METHOD GetBoneVertexInfluence, :THIS_, :DWORD, :ptr REAL4, :ptr DWORD
    STD_METHOD GetMaxVertexInfluences, :ptr THIS_
    STD_METHOD GetNumBones, :THIS_
    STD_METHOD FindBoneVertexInfluenceIndex, :THIS_, :DWORD, :ptr DWORD
    STD_METHOD GetMaxFaceInfluences, :THIS_, :DWORD, :ptr DWORD
    STD_METHOD SetMinBoneInfluence, :THIS_
    STD_METHOD GetMinBoneInfluence, :THIS_
    STD_METHOD SetBoneName, :THIS_, :LPCSTR
    STD_METHOD GetBoneName, :THIS_
    STD_METHOD SetBoneOffsetMatrix, :THIS_, :ptr D3DXMATRIX
    STD_METHOD GetBoneOffsetMatrix, :THIS_
    STD_METHOD Clone, :ptr THIS_
    STD_METHOD Remap, :THIS_, :ptr DWORD
    STD_METHOD SetFVF, :THIS_
    STD_METHOD SetDeclaration, :ptr THIS_
    STD_METHOD GetFVF, :THIS_
    STD_METHOD GetDeclaration, :ptr THIS_
    STD_METHOD UpdateSkinnedMesh, :ptr THIS_, :ptr D3DXMATRIX, :LPCVOID, :PVOID
    STD_METHOD ConvertToBlendedMesh, :THIS_, :DWORD, :ptr DWORD, :LPDWORD, :ptr DWORD, :ptr LPD3DXBUFFER, :ptr DWORD, :ptr DWORD, :ptr LPD3DXBUFFER, :ptr LPD3DXMESH
    STD_METHOD ConvertToIndexedBlendedMesh, :THIS_, :DWORD, :DWORD, :ptr DWORD, :LPDWORD, :ptr DWORD, :ptr LPD3DXBUFFER, :ptr DWORD, :ptr DWORD, :ptr LPD3DXBUFFER, :ptr LPD3DXMESH
  END_DECLARE_INTERFACE_
  ifdef __cplusplus
    ;[...]
  endif
  D3DXCreateMesh proto WIN_STD_CALL_CONV :DWORD, :DWORD, :DWORD, :ptr D3DVERTEXELEMENT9, :LPDIRECT3DDEVICE9, :ptr LPD3DXMESH
  D3DXCreateMeshFVF proto WIN_STD_CALL_CONV :DWORD, :DWORD, :DWORD, :DWORD, :LPDIRECT3DDEVICE9, :ptr LPD3DXMESH
  D3DXCreateSPMesh proto WIN_STD_CALL_CONV :LPD3DXMESH, :ptr DWORD, :ptr D3DXATTRIBUTEWEIGHTS, :ptr FLOAT, :ptr LPD3DXSPMESH
  D3DXCleanMesh proto WIN_STD_CALL_CONV :D3DXCLEANTYPE, :LPD3DXMESH, :ptr DWORD, :ptr LPD3DXMESH, :ptr DWORD, :ptr LPD3DXBUFFER
  D3DXValidMesh proto WIN_STD_CALL_CONV :LPD3DXMESH, :ptr DWORD, :ptr LPD3DXBUFFER
  D3DXGeneratePMesh proto WIN_STD_CALL_CONV :LPD3DXMESH, :ptr DWORD, :ptr D3DXATTRIBUTEWEIGHTS, :ptr FLOAT, :DWORD, :DWORD, :ptr LPD3DXPMESH
  D3DXSimplifyMesh proto WIN_STD_CALL_CONV :LPD3DXMESH, :ptr DWORD, :ptr D3DXATTRIBUTEWEIGHTS, :ptr FLOAT, :DWORD, :DWORD, :ptr LPD3DXMESH
  D3DXComputeBoundingSphere proto WIN_STD_CALL_CONV :ptr D3DXVECTOR3, :DWORD, :DWORD, :ptr D3DXVECTOR3, :ptr FLOAT
  D3DXComputeBoundingBox proto WIN_STD_CALL_CONV :ptr D3DXVECTOR3, :DWORD, :DWORD, :ptr D3DXVECTOR3, :ptr D3DXVECTOR3
  D3DXComputeNormals proto WIN_STD_CALL_CONV :LPD3DXBASEMESH, :ptr DWORD
  D3DXCreateBuffer proto WIN_STD_CALL_CONV :DWORD, :ptr LPD3DXBUFFER
  D3DXLoadMeshFromXA proto WIN_STD_CALL_CONV :LPCSTR, :DWORD, :LPDIRECT3DDEVICE9, :ptr LPD3DXBUFFER, :ptr LPD3DXBUFFER, :ptr LPD3DXBUFFER, :ptr DWORD, :ptr LPD3DXMESH
  D3DXLoadMeshFromXW proto WIN_STD_CALL_CONV :LPCWSTR, :DWORD, :LPDIRECT3DDEVICE9, :ptr LPD3DXBUFFER, :ptr LPD3DXBUFFER, :ptr LPD3DXBUFFER, :ptr DWORD, :ptr LPD3DXMESH
  ifdef UNICODE
    D3DXLoadMeshFromX equ <D3DXLoadMeshFromXW>
  else
    D3DXLoadMeshFromX equ <D3DXLoadMeshFromXA>
  endif
  D3DXLoadMeshFromXInMemory proto WIN_STD_CALL_CONV :LPCVOID, :DWORD, :DWORD, :LPDIRECT3DDEVICE9, :ptr LPD3DXBUFFER, :ptr LPD3DXBUFFER, :ptr LPD3DXBUFFER, :ptr DWORD, :ptr LPD3DXMESH
  D3DXLoadMeshFromXResource proto WIN_STD_CALL_CONV :HMODULE, :LPCSTR, :LPCSTR, :DWORD, :LPDIRECT3DDEVICE9, :ptr LPD3DXBUFFER, :ptr LPD3DXBUFFER, :ptr LPD3DXBUFFER, :ptr DWORD, :ptr LPD3DXMESH
  D3DXSaveMeshToXA proto WIN_STD_CALL_CONV :LPCSTR, :LPD3DXMESH, :ptr DWORD, :ptr D3DXMATERIAL, :ptr D3DXEFFECTINSTANCE, :DWORD, :DWORD
  D3DXSaveMeshToXW proto WIN_STD_CALL_CONV :LPCWSTR, :LPD3DXMESH, :ptr DWORD, :ptr D3DXMATERIAL, :ptr D3DXEFFECTINSTANCE, :DWORD, :DWORD
  ifdef UNICODE
    D3DXSaveMeshToX equ <D3DXSaveMeshToXW>
  else
    D3DXSaveMeshToX equ <D3DXSaveMeshToXA>
  endif
  D3DXCreatePMeshFromStream proto WIN_STD_CALL_CONV :ptr IStream, :DWORD, :LPDIRECT3DDEVICE9, :ptr LPD3DXBUFFER, :ptr LPD3DXBUFFER, :ptr DWORD, :ptr LPD3DXPMESH
  D3DXCreateSkinInfo proto WIN_STD_CALL_CONV :DWORD, :ptr D3DVERTEXELEMENT9, :DWORD, :ptr LPD3DXSKININFO
  D3DXCreateSkinInfoFVF proto WIN_STD_CALL_CONV :DWORD, :DWORD, :DWORD, :ptr LPD3DXSKININFO
  ifdef __cplusplus
    ;[...]
  endif
  D3DXLoadMeshFromXof proto WIN_STD_CALL_CONV :LPD3DXFILEDATA, :DWORD, :LPDIRECT3DDEVICE9, :ptr LPD3DXBUFFER, :ptr LPD3DXBUFFER, :ptr LPD3DXBUFFER, :ptr DWORD, :ptr LPD3DXMESH
  D3DXLoadSkinMeshFromXof proto WIN_STD_CALL_CONV :LPD3DXFILEDATA, :DWORD, :LPDIRECT3DDEVICE9, :ptr LPD3DXBUFFER, :ptr LPD3DXBUFFER, :ptr LPD3DXBUFFER, :ptr DWORD, :ptr LPD3DXSKININFO, :ptr LPD3DXMESH
  D3DXCreateSkinInfoFromBlendedMesh proto WIN_STD_CALL_CONV :LPD3DXBASEMESH, :DWORD, :ptr D3DXBONECOMBINATION, :ptr LPD3DXSKININFO
  D3DXTessellateNPatches proto WIN_STD_CALL_CONV :LPD3DXMESH, :ptr DWORD, :FLOAT, :BOOL, :ptr LPD3DXMESH, :ptr LPD3DXBUFFER
  D3DXGenerateOutputDecl proto WIN_STD_CALL_CONV :ptr D3DVERTEXELEMENT9, :ptr D3DVERTEXELEMENT9
  D3DXLoadPatchMeshFromXof proto WIN_STD_CALL_CONV :LPD3DXFILEDATA, :DWORD, :LPDIRECT3DDEVICE9, :ptr LPD3DXBUFFER, :ptr LPD3DXBUFFER, :PDWORD, :ptr LPD3DXPATCHMESH
  D3DXRectPatchSize proto WIN_STD_CALL_CONV :ptr FLOAT, :ptr DWORD, :ptr DWORD
  D3DXTriPatchSize proto WIN_STD_CALL_CONV :ptr FLOAT, :ptr DWORD, :ptr DWORD
  D3DXTessellateRectPatch proto WIN_STD_CALL_CONV :LPDIRECT3DVERTEXBUFFER9, :ptr FLOAT, :ptr D3DVERTEXELEMENT9, :ptr D3DRECTPATCH_INFO, :LPD3DXMESH
  D3DXTessellateTriPatch proto WIN_STD_CALL_CONV :LPDIRECT3DVERTEXBUFFER9, :ptr FLOAT, :ptr D3DVERTEXELEMENT9, :ptr D3DTRIPATCH_INFO, :LPD3DXMESH
  D3DXCreateNPatchMesh proto WIN_STD_CALL_CONV :LPD3DXMESH, :ptr LPD3DXPATCHMESH
  D3DXCreatePatchMesh proto WIN_STD_CALL_CONV :ptr D3DXPATCHINFO, :DWORD, :DWORD, :DWORD, :ptr D3DVERTEXELEMENT9, :LPDIRECT3DDEVICE9, :ptr LPD3DXPATCHMESH
  D3DXValidPatchMesh proto WIN_STD_CALL_CONV :LPD3DXPATCHMESH, :ptr DWORD, :ptr DWORD, :ptr LPD3DXBUFFER
  D3DXGetFVFVertexSize proto WIN_STD_CALL_CONV :DWORD
  D3DXGetDeclVertexSize proto WIN_STD_CALL_CONV :ptr D3DVERTEXELEMENT9, :DWORD
  D3DXGetDeclLength proto WIN_STD_CALL_CONV :ptr D3DVERTEXELEMENT9
  D3DXDeclaratorFromFVF proto WIN_STD_CALL_CONV :DWORD, :ptr D3DVERTEXELEMENT9
  D3DXFVFFromDeclarator proto WIN_STD_CALL_CONV :ptr D3DVERTEXELEMENT9, :ptr DWORD
  D3DXWeldVertices proto WIN_STD_CALL_CONV :LPD3DXMESH, :DWORD, :ptr D3DXWELDEPSILONS, :ptr DWORD, :ptr DWORD, :ptr DWORD, :ptr LPD3DXBUFFER
  D3DXINTERSECTINFO struct
    FaceIndex DWORD ?
    U FLOAT ?
    V FLOAT ?
    Dist FLOAT ?
  D3DXINTERSECTINFO ends
  _D3DXINTERSECTINFO typedef D3DXINTERSECTINFO
  LPD3DXINTERSECTINFO typedef ptr D3DXINTERSECTINFO
  
  D3DXIntersect proto WIN_STD_CALL_CONV :LPD3DXBASEMESH, :ptr D3DXVECTOR3, :ptr D3DXVECTOR3, :ptr BOOL, :ptr DWORD, :ptr FLOAT, :ptr FLOAT, :ptr FLOAT, :ptr LPD3DXBUFFER, :ptr DWORD
  D3DXIntersectSubset proto WIN_STD_CALL_CONV :LPD3DXBASEMESH, :DWORD, :ptr D3DXVECTOR3, :ptr D3DXVECTOR3, :ptr BOOL, :ptr DWORD, :ptr FLOAT, :ptr FLOAT, :ptr FLOAT, :ptr LPD3DXBUFFER, :ptr DWORD
  D3DXSplitMesh proto WIN_STD_CALL_CONV :LPD3DXMESH, :ptr DWORD, :DWORD, :DWORD, :ptr DWORD, :ptr LPD3DXBUFFER, :ptr LPD3DXBUFFER, :ptr LPD3DXBUFFER, :ptr LPD3DXBUFFER
  D3DXIntersectTri proto WIN_STD_CALL_CONV :ptr D3DXVECTOR3, :ptr D3DXVECTOR3, :ptr D3DXVECTOR3, :ptr D3DXVECTOR3, :ptr D3DXVECTOR3, :ptr FLOAT, :ptr FLOAT, :ptr FLOAT
  D3DXSphereBoundProbe proto WIN_STD_CALL_CONV :ptr D3DXVECTOR3, :FLOAT, :ptr D3DXVECTOR3, :ptr D3DXVECTOR3
  D3DXBoxBoundProbe proto WIN_STD_CALL_CONV :ptr D3DXVECTOR3, :ptr D3DXVECTOR3, :ptr D3DXVECTOR3, :ptr D3DXVECTOR3
  D3DXComputeTangentFrame proto WIN_STD_CALL_CONV :ptr ID3DXMesh, :DWORD
  D3DXComputeTangentFrameEx proto WIN_STD_CALL_CONV :ptr ID3DXMesh, :DWORD, :DWORD, :DWORD, :DWORD, :DWORD, :DWORD, :DWORD, :DWORD, :DWORD, :ptr DWORD, :FLOAT, :FLOAT, :FLOAT, :ptr ptr ID3DXMesh, :ptr ptr ID3DXBuffer
  D3DXComputeTangent proto WIN_STD_CALL_CONV :LPD3DXMESH, :DWORD, :DWORD, :DWORD, :DWORD, :ptr DWORD
  TYPE_LPD3DXUVATLASCB typedef proto WIN_STD_CALL_CONV :FLOAT, :LPVOID
  LPD3DXUVATLASCB typedef ptr TYPE_LPD3DXUVATLASCB
  D3DXUVAtlasCreate proto WIN_STD_CALL_CONV :LPD3DXMESH, :UINT, :FLOAT, :UINT, :UINT, :FLOAT, :DWORD, :ptr DWORD, :ptr DWORD, :ptr FLOAT, :LPD3DXUVATLASCB, :FLOAT, :LPVOID, :DWORD, :ptr LPD3DXMESH, :ptr LPD3DXBUFFER, :ptr LPD3DXBUFFER, :ptr FLOAT, :ptr UINT
  D3DXUVAtlasPartition proto WIN_STD_CALL_CONV :LPD3DXMESH, :UINT, :FLOAT, :DWORD, :ptr DWORD, :ptr DWORD, :ptr FLOAT, :LPD3DXUVATLASCB, :FLOAT, :LPVOID, :DWORD, :ptr LPD3DXMESH, :ptr LPD3DXBUFFER, :ptr LPD3DXBUFFER, :ptr LPD3DXBUFFER, :ptr FLOAT, :ptr UINT
  D3DXUVAtlasPack proto WIN_STD_CALL_CONV :ptr ID3DXMesh, :UINT, :UINT, :FLOAT, :DWORD, :ptr DWORD, :LPD3DXUVATLASCB, :FLOAT, :LPVOID, :DWORD, :LPD3DXBUFFER
  TYPE_LPD3DXIMTSIGNALCALLBACK typedef proto WIN_STD_CALL_CONV :ptr D3DXVECTOR2, :UINT, :UINT, :ptr VOID, :ptr FLOAT
  LPD3DXIMTSIGNALCALLBACK typedef ptr TYPE_LPD3DXIMTSIGNALCALLBACK
  D3DXComputeIMTFromPerVertexSignal proto WIN_STD_CALL_CONV :LPD3DXMESH, :ptr FLOAT, :UINT, :UINT, :DWORD, :LPD3DXUVATLASCB, :LPVOID, :ptr LPD3DXBUFFER
  D3DXComputeIMTFromSignal proto WIN_STD_CALL_CONV :LPD3DXMESH, :DWORD, :UINT, :FLOAT, :DWORD, :LPD3DXIMTSIGNALCALLBACK, :ptr VOID, :LPD3DXUVATLASCB, :LPVOID, :ptr LPD3DXBUFFER
  D3DXComputeIMTFromTexture proto WIN_STD_CALL_CONV :LPD3DXMESH, :LPDIRECT3DTEXTURE9, :DWORD, :DWORD, :LPD3DXUVATLASCB, :LPVOID, :ptr LPD3DXBUFFER
  D3DXComputeIMTFromPerTexelSignal proto WIN_STD_CALL_CONV :LPD3DXMESH, :DWORD, :ptr FLOAT, :UINT, :UINT, :UINT, :UINT, :DWORD, :LPD3DXUVATLASCB, :LPVOID, :ptr LPD3DXBUFFER
  D3DXConvertMeshSubsetToSingleStrip proto WIN_STD_CALL_CONV :LPD3DXBASEMESH, :DWORD, :DWORD, :ptr LPDIRECT3DINDEXBUFFER9, :ptr DWORD
  D3DXConvertMeshSubsetToStrips proto WIN_STD_CALL_CONV :LPD3DXBASEMESH, :DWORD, :DWORD, :ptr LPDIRECT3DINDEXBUFFER9, :ptr DWORD, :ptr LPD3DXBUFFER, :ptr DWORD
  D3DXOptimizeFaces proto WIN_STD_CALL_CONV :LPCVOID, :UINT, :UINT, :BOOL, :ptr DWORD
  D3DXOptimizeVertices proto WIN_STD_CALL_CONV :LPCVOID, :UINT, :UINT, :BOOL, :ptr DWORD
  ifdef __cplusplus
    ;[...]
  endif
  D3DXSHCOMPRESSQUALITYTYPE typedef SDWORD
  D3DXSHCQUAL_FASTLOWQUALITY equ 1
  D3DXSHCQUAL_SLOWHIGHQUALITY equ 2
  D3DXSHCQUAL_FORCE_DWORD equ 7fffffffh
  _D3DXSHCOMPRESSQUALITYTYPE typedef D3DXSHCOMPRESSQUALITYTYPE
  
  D3DXSHGPUSIMOPT typedef SDWORD
  D3DXSHGPUSIMOPT_SHADOWRES256 equ 1
  D3DXSHGPUSIMOPT_SHADOWRES512 equ 0
  D3DXSHGPUSIMOPT_SHADOWRES1024 equ 2
  D3DXSHGPUSIMOPT_SHADOWRES2048 equ 3
  D3DXSHGPUSIMOPT_HIGHQUALITY equ 4
  D3DXSHGPUSIMOPT_FORCE_DWORD equ 7fffffffh
  _D3DXSHGPUSIMOPT typedef D3DXSHGPUSIMOPT
  
  D3DXSHMATERIAL struct
    Diffuse D3DCOLORVALUE <>
    bMirror BOOL ?
    bSubSurf BOOL ?
    RelativeIndexOfRefraction FLOAT ?
    Absorption D3DCOLORVALUE <>
    ReducedScattering D3DCOLORVALUE <>
  D3DXSHMATERIAL ends
  _D3DXSHMATERIAL typedef D3DXSHMATERIAL
  D3DXSHPRTSPLITMESHVERTDATA struct
    uVertRemap UINT ?
    uSubCluster UINT ?
    ucVertStatus UCHAR ?
  D3DXSHPRTSPLITMESHVERTDATA ends
  _D3DXSHPRTSPLITMESHVERTDATA typedef D3DXSHPRTSPLITMESHVERTDATA
  D3DXSHPRTSPLITMESHCLUSTERDATA struct
    uVertStart UINT ?
    uVertLength UINT ?
    uFaceStart UINT ?
    uFaceLength UINT ?
    uClusterStart UINT ?
    uClusterLength UINT ?
  D3DXSHPRTSPLITMESHCLUSTERDATA ends
  _D3DXSHPRTSPLITMESHCLUSTERDATA typedef D3DXSHPRTSPLITMESHCLUSTERDATA
  TYPE_LPD3DXSHPRTSIMCB typedef proto WIN_STD_CALL_CONV :REAL4, :LPVOID
  LPD3DXSHPRTSIMCB typedef ptr TYPE_LPD3DXSHPRTSIMCB
  DEFINE_GUID IID_ID3DXPRTBuffer, 0f1827e47h, 0a8h, 49cdh, 90h, 8ch, 9dh, 11h, 95h, 5fh, 87h, 28h
  DEFINE_GUID IID_ID3DXPRTCompBuffer, 0a758d465h, 0fe8dh, 45adh, 9ch, 0f0h, 0d0h, 1eh, 56h, 26h, 6ah, 7h
  DEFINE_GUID IID_ID3DXTextureGutterHelper, 838f01ech, 9729h, 4527h, 0aah, 0dbh, 0dfh, 70h, 0adh, 0e7h, 0feh, 0a9h
  DEFINE_GUID IID_ID3DXPRTEngine, 683a4278h, 0cd5fh, 4d24h, 90h, 0adh, 0c4h, 0e1h, 0b6h, 85h, 5dh, 53h
  ;ID3DXTextureGutterHelper typedef ID3DXTextureGutterHelper
  
  ;ID3DXPRTBuffer typedef ID3DXPRTBuffer
  
  INTERFACE equ <ID3DXPRTBuffer>
  DECLARE_INTERFACE_ ID3DXPRTBuffer, IUnknown
    STD_METHOD QueryInterface, :THIS_, :ptr LPVOID
    STD_METHOD AddRef, :THIS_
    STD_METHOD Release, :THIS_
    STD_METHOD GetNumSamples, :THIS_
    STD_METHOD GetNumCoeffs, :THIS_
    STD_METHOD GetNumChannels, :THIS_
    STD_METHOD IsTexture, :THIS_
    STD_METHOD GetWidth, :THIS_
    STD_METHOD GetHeight, :THIS_
    STD_METHOD Resize, :THIS_
    STD_METHOD LockBuffer, :THIS_, :UINT, :ptr ptr FLOAT
    STD_METHOD UnlockBuffer, :THIS_
    STD_METHOD ScaleBuffer, :THIS_
    STD_METHOD AddBuffer, :THIS_
    STD_METHOD AttachGH, :THIS_
    STD_METHOD ReleaseGH, :THIS_
    STD_METHOD EvalGH, :THIS_
    STD_METHOD ExtractTexture, :THIS_, :UINT, :UINT, :LPDIRECT3DTEXTURE9
    STD_METHOD ExtractToMesh, :THIS_, :D3DDECLUSAGE, :UINT, :LPD3DXMESH
  END_DECLARE_INTERFACE_
  ;ID3DXPRTCompBuffer typedef ID3DXPRTCompBuffer
  
  LPD3DXPRTCOMPBUFFER typedef ptr ID3DXPRTCompBuffer
  
  INTERFACE equ <ID3DXPRTCompBuffer>
  DECLARE_INTERFACE_ ID3DXPRTCompBuffer, IUnknown
    STD_METHOD QueryInterface, :THIS_, :ptr LPVOID
    STD_METHOD AddRef, :THIS_
    STD_METHOD Release, :THIS_
    STD_METHOD GetNumSamples, :THIS_
    STD_METHOD GetNumCoeffs, :THIS_
    STD_METHOD GetNumChannels, :THIS_
    STD_METHOD IsTexture, :THIS_
    STD_METHOD GetWidth, :THIS_
    STD_METHOD GetHeight, :THIS_
    STD_METHOD GetNumClusters, :THIS_
    STD_METHOD GetNumPCA, :THIS_
    STD_METHOD NormalizeData, :THIS_
    STD_METHOD ExtractBasis, :THIS_, :ptr FLOAT
    STD_METHOD ExtractClusterIDs, :ptr THIS_
    STD_METHOD ExtractPCA, :THIS_, :UINT, :ptr FLOAT
    STD_METHOD ExtractTexture, :THIS_, :UINT, :LPDIRECT3DTEXTURE9
    STD_METHOD ExtractToMesh, :THIS_, :D3DDECLUSAGE, :UINT, :LPD3DXMESH
  END_DECLARE_INTERFACE_
  INTERFACE equ <ID3DXTextureGutterHelper>
  DECLARE_INTERFACE_ ID3DXTextureGutterHelper, IUnknown
    STD_METHOD QueryInterface, :THIS_, :ptr LPVOID
    STD_METHOD AddRef, :THIS_
    STD_METHOD Release, :THIS_
    STD_METHOD GetWidth, :THIS_
    STD_METHOD GetHeight, :THIS_
    STD_METHOD ApplyGuttersFloat, :ptr THIS_, :UINT, :UINT, :UINT
    STD_METHOD ApplyGuttersTex, :THIS_
    STD_METHOD ApplyGuttersPRT, :THIS_
    STD_METHOD ResampleTex, :THIS_, :LPD3DXMESH, :D3DDECLUSAGE, :UINT, :LPDIRECT3DTEXTURE9
    STD_METHOD GetFaceMap, :ptr THIS_
    STD_METHOD GetBaryMap, :ptr THIS_
    STD_METHOD GetTexelMap, :ptr THIS_
    STD_METHOD GetGutterMap, :ptr THIS_
    STD_METHOD SetFaceMap, :ptr THIS_
    STD_METHOD SetBaryMap, :ptr THIS_
    STD_METHOD SetTexelMap, :ptr THIS_
    STD_METHOD SetGutterMap, :ptr THIS_
  END_DECLARE_INTERFACE_
  ;ID3DXPRTEngine typedef ID3DXPRTEngine
  
  LPD3DXPRTENGINE typedef ptr ID3DXPRTEngine
  
  INTERFACE equ <ID3DXPRTEngine>
  DECLARE_INTERFACE_ ID3DXPRTEngine, IUnknown
    STD_METHOD QueryInterface, :THIS_, :ptr LPVOID
    STD_METHOD AddRef, :THIS_
    STD_METHOD Release, :THIS_
    STD_METHOD SetMeshMaterials, :ptr ptr THIS_, :UINT, :UINT, :BOOL, :FLOAT
    STD_METHOD SetPerVertexAlbedo, :ptr THIS_, :UINT, :UINT
    STD_METHOD SetPerTexelAlbedo, :THIS_, :UINT, :LPD3DXTEXTUREGUTTERHELPER
    STD_METHOD GetVertexAlbedo, :ptr THIS_, :UINT
    STD_METHOD SetPerTexelNormal, :THIS_
    STD_METHOD ExtractPerVertexAlbedo, :THIS_, :D3DDECLUSAGE, :UINT
    STD_METHOD ResampleBuffer, :THIS_, :LPD3DXPRTBUFFER
    STD_METHOD GetAdaptedMesh, :THIS_, :ptr UINT, :ptr UINT, :ptr FLOAT, :ptr LPD3DXMESH
    STD_METHOD GetNumVerts, :THIS_
    STD_METHOD GetNumFaces, :THIS_
    STD_METHOD SetMinMaxIntersection, :THIS_, :FLOAT
    STD_METHOD RobustMeshRefine, :THIS_, :UINT
    STD_METHOD SetSamplingInfo, :THIS_, :BOOL, :BOOL, :BOOL, :FLOAT
    STD_METHOD ComputeDirectLightingSH, :THIS_, :LPD3DXPRTBUFFER
    STD_METHOD ComputeDirectLightingSHAdaptive, :THIS_, :FLOAT, :FLOAT, :UINT, :LPD3DXPRTBUFFER
    STD_METHOD ComputeDirectLightingSHGPU, :THIS_, :UINT, :UINT, :FLOAT, :FLOAT, :LPD3DXPRTBUFFER
    STD_METHOD ComputeSS, :THIS_, :LPD3DXPRTBUFFER, :LPD3DXPRTBUFFER
    STD_METHOD ComputeSSAdaptive, :THIS_, :FLOAT, :FLOAT, :UINT, :LPD3DXPRTBUFFER, :LPD3DXPRTBUFFER
    STD_METHOD ComputeBounce, :THIS_, :LPD3DXPRTBUFFER, :LPD3DXPRTBUFFER
    STD_METHOD ComputeBounceAdaptive, :THIS_, :FLOAT, :FLOAT, :UINT, :LPD3DXPRTBUFFER, :LPD3DXPRTBUFFER
    STD_METHOD ComputeVolumeSamplesDirectSH, :THIS_, :UINT, :UINT, :ptr D3DXVECTOR3, :LPD3DXPRTBUFFER
    STD_METHOD ComputeVolumeSamples, :THIS_, :UINT, :UINT, :ptr D3DXVECTOR3, :LPD3DXPRTBUFFER
    STD_METHOD ComputeSurfSamplesDirectSH, :THIS_, :UINT, :ptr D3DXVECTOR3, :ptr D3DXVECTOR3, :LPD3DXPRTBUFFER
    STD_METHOD ComputeSurfSamplesBounce, :THIS_, :UINT, :ptr D3DXVECTOR3, :ptr D3DXVECTOR3, :LPD3DXPRTBUFFER, :LPD3DXPRTBUFFER
    STD_METHOD FreeSSData, :THIS_
    STD_METHOD FreeBounceData, :THIS_
    STD_METHOD ComputeLDPRTCoeffs, :THIS_, :UINT, :ptr D3DXVECTOR3, :LPD3DXPRTBUFFER
    STD_METHOD ScaleMeshChunk, :THIS_, :FLOAT, :LPD3DXPRTBUFFER
    STD_METHOD MultiplyAlbedo, :THIS_
    STD_METHOD SetCallBack, :THIS_, :FLOAT, :LPVOID
    STD_METHOD ShadowRayIntersects, :ptr THIS_, :ptr D3DXVECTOR3
    STD_METHOD ClosestRayIntersects, :ptr THIS_, :ptr D3DXVECTOR3, :ptr DWORD, :ptr FLOAT, :ptr FLOAT, :ptr FLOAT
  END_DECLARE_INTERFACE_
  ifdef __cplusplus
    ;[...]
  endif
  D3DXCreatePRTBuffer proto WIN_STD_CALL_CONV :UINT, :UINT, :UINT, :ptr LPD3DXPRTBUFFER
  D3DXCreatePRTBufferTex proto WIN_STD_CALL_CONV :UINT, :UINT, :UINT, :UINT, :ptr LPD3DXPRTBUFFER
  D3DXLoadPRTBufferFromFileA proto WIN_STD_CALL_CONV :LPCSTR, :ptr LPD3DXPRTBUFFER
  D3DXLoadPRTBufferFromFileW proto WIN_STD_CALL_CONV :LPCWSTR, :ptr LPD3DXPRTBUFFER
  ifdef UNICODE
    D3DXLoadPRTBufferFromFile equ <D3DXLoadPRTBufferFromFileW>
  else
    D3DXLoadPRTBufferFromFile equ <D3DXLoadPRTBufferFromFileA>
  endif
  D3DXSavePRTBufferToFileA proto WIN_STD_CALL_CONV :LPCSTR, :LPD3DXPRTBUFFER
  D3DXSavePRTBufferToFileW proto WIN_STD_CALL_CONV :LPCWSTR, :LPD3DXPRTBUFFER
  ifdef UNICODE
    D3DXSavePRTBufferToFile equ <D3DXSavePRTBufferToFileW>
  else
    D3DXSavePRTBufferToFile equ <D3DXSavePRTBufferToFileA>
  endif
  D3DXLoadPRTCompBufferFromFileA proto WIN_STD_CALL_CONV :LPCSTR, :ptr LPD3DXPRTCOMPBUFFER
  D3DXLoadPRTCompBufferFromFileW proto WIN_STD_CALL_CONV :LPCWSTR, :ptr LPD3DXPRTCOMPBUFFER
  ifdef UNICODE
    D3DXLoadPRTCompBufferFromFile equ <D3DXLoadPRTCompBufferFromFileW>
  else
    D3DXLoadPRTCompBufferFromFile equ <D3DXLoadPRTCompBufferFromFileA>
  endif
  D3DXSavePRTCompBufferToFileA proto WIN_STD_CALL_CONV :LPCSTR, :LPD3DXPRTCOMPBUFFER
  D3DXSavePRTCompBufferToFileW proto WIN_STD_CALL_CONV :LPCWSTR, :LPD3DXPRTCOMPBUFFER
  ifdef UNICODE
    D3DXSavePRTCompBufferToFile equ <D3DXSavePRTCompBufferToFileW>
  else
    D3DXSavePRTCompBufferToFile equ <D3DXSavePRTCompBufferToFileA>
  endif
  D3DXCreatePRTCompBuffer proto WIN_STD_CALL_CONV :D3DXSHCOMPRESSQUALITYTYPE, :UINT, :UINT, :LPD3DXSHPRTSIMCB, :LPVOID, :LPD3DXPRTBUFFER, :ptr LPD3DXPRTCOMPBUFFER
  D3DXCreateTextureGutterHelper proto WIN_STD_CALL_CONV :UINT, :UINT, :LPD3DXMESH, :FLOAT, :ptr LPD3DXTEXTUREGUTTERHELPER
  D3DXCreatePRTEngine proto WIN_STD_CALL_CONV :LPD3DXMESH, :ptr DWORD, :BOOL, :LPD3DXMESH, :ptr LPD3DXPRTENGINE
  D3DXConcatenateMeshes proto WIN_STD_CALL_CONV :ptr LPD3DXMESH, :UINT, :DWORD, :ptr D3DXMATRIX, :ptr D3DXMATRIX, :ptr D3DVERTEXELEMENT9, :LPDIRECT3DDEVICE9, :ptr LPD3DXMESH
  D3DXSHPRTCompSuperCluster proto WIN_STD_CALL_CONV :ptr UINT, :LPD3DXMESH, :UINT, :UINT, :ptr UINT, :ptr UINT
  D3DXSHPRTCompSplitMeshSC proto WIN_STD_CALL_CONV :ptr UINT, :UINT, :UINT, :ptr UINT, :UINT, :LPVOID, :BOOL, :UINT, :ptr LPD3DXBUFFER, :ptr UINT, :BOOL, :ptr LPD3DXBUFFER, :ptr LPD3DXBUFFER, :ptr UINT, :ptr UINT, :ptr D3DXSHPRTSPLITMESHCLUSTERDATA
  ifdef __cplusplus
    ;[...]
  endif
  DEFINE_GUID DXFILEOBJ_XSkinMeshHeader, 3cf169ceh, 0ff7ch, 44abh, 93h, 0c0h, 0f7h, 8fh, 62h, 0d1h, 72h, 0e2h
  DEFINE_GUID DXFILEOBJ_VertexDuplicationIndices, 0b8d65549h, 0d7c9h, 4995h, 89h, 0cfh, 53h, 0a9h, 0a8h, 0b0h, 31h, 0e3h
  DEFINE_GUID DXFILEOBJ_FaceAdjacency, 0a64c844ah, 0e282h, 4756h, 8bh, 80h, 25h, 0ch, 0deh, 4h, 39h, 8ch
  DEFINE_GUID DXFILEOBJ_SkinWeights, 6f0d123bh, 0bad2h, 4167h, 0a0h, 0d0h, 80h, 22h, 4fh, 25h, 0fah, 0bbh
  DEFINE_GUID DXFILEOBJ_Patch, 0a3eb5d44h, 0fc22h, 429dh, 9ah, 0fbh, 32h, 21h, 0cbh, 97h, 19h, 0a6h
  DEFINE_GUID DXFILEOBJ_PatchMesh, 0d02c95cch, 0edbah, 4305h, 9bh, 5dh, 18h, 20h, 0d7h, 70h, 4bh, 0bfh
  DEFINE_GUID DXFILEOBJ_PatchMesh9, 0b9ec94e1h, 0b9a6h, 4251h, 0bah, 18h, 94h, 89h, 3fh, 2h, 0c0h, 0eah
  DEFINE_GUID DXFILEOBJ_PMInfo, 0b6c3e656h, 0ec8bh, 4b92h, 9bh, 62h, 68h, 16h, 59h, 52h, 29h, 47h
  DEFINE_GUID DXFILEOBJ_PMAttributeRange, 917e0427h, 0c61eh, 4a14h, 9ch, 64h, 0afh, 0e6h, 5fh, 9eh, 98h, 44h
  DEFINE_GUID DXFILEOBJ_PMVSplitRecord, 574ccc14h, 0f0b3h, 4333h, 82h, 2dh, 93h, 0e8h, 0a8h, 0a0h, 8eh, 4ch
  DEFINE_GUID DXFILEOBJ_FVFData, 0b6e70a0eh, 8ef9h, 4e83h, 94h, 0adh, 0ech, 0c8h, 0b0h, 0c0h, 48h, 97h
  DEFINE_GUID DXFILEOBJ_VertexElement, 0f752461ch, 1e23h, 48f6h, 0b9h, 0f8h, 83h, 50h, 85h, 0fh, 33h, 6fh
  DEFINE_GUID DXFILEOBJ_DeclData, 0bf22e553h, 292ch, 4781h, 9fh, 0eah, 62h, 0bdh, 55h, 4bh, 0ddh, 93h
  DEFINE_GUID DXFILEOBJ_EffectFloats, 0f1cfe2b3h, 0de3h, 4e28h, 0afh, 0a1h, 15h, 5ah, 75h, 0ah, 28h, 2dh
  DEFINE_GUID DXFILEOBJ_EffectString, 0d55b097eh, 0bdb6h, 4c52h, 0b0h, 3dh, 60h, 51h, 0c8h, 9dh, 0eh, 42h
  DEFINE_GUID DXFILEOBJ_EffectDWord, 622c0ed0h, 956eh, 4da9h, 90h, 8ah, 2ah, 0f9h, 4fh, 3ch, 0e7h, 16h
  DEFINE_GUID DXFILEOBJ_EffectParamFloats, 3014b9a0h, 62f5h, 478ch, 9bh, 86h, 0e4h, 0ach, 9fh, 4eh, 41h, 8bh
  DEFINE_GUID DXFILEOBJ_EffectParamString, 1dbc4c88h, 94c1h, 46eeh, 90h, 76h, 2ch, 28h, 81h, 8ch, 94h, 81h
  DEFINE_GUID DXFILEOBJ_EffectParamDWord, 0e13963bch, 0ae51h, 4c5dh, 0b0h, 0fh, 0cfh, 0a3h, 0a9h, 0d9h, 7ch, 0e5h
  DEFINE_GUID DXFILEOBJ_EffectInstance, 0e331f7e4h, 559h, 4cc2h, 8eh, 99h, 1ch, 0ech, 16h, 57h, 92h, 8fh
  DEFINE_GUID DXFILEOBJ_AnimTicksPerSecond, 9e415a43h, 7ba6h, 4a73h, 87h, 43h, 0b7h, 3dh, 47h, 0e8h, 84h, 76h
  DEFINE_GUID DXFILEOBJ_CompressedAnimationSet, 7f9b00b3h, 0f125h, 4890h, 87h, 6eh, 1ch, 42h, 0bfh, 69h, 7ch, 4dh
  XFILECOMPRESSEDANIMATIONSET struct
    CompressedBlockSize DWORD ?
    TicksPerSec FLOAT ?
    PlaybackType DWORD ?
    BufferLength DWORD ?
  XFILECOMPRESSEDANIMATIONSET ends
  _XFILECOMPRESSEDANIMATIONSET typedef XFILECOMPRESSEDANIMATIONSET
endif

