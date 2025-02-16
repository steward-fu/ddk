;Include file created by h2incX, Version C.01.00, 14/10/2022 22:00
;(C) 2018-today by Biterider, 2005-2009 by Japheth
;Source file: 'encode.h', last modified: 26/12/2019 20:24


ifndef WEBP_WEBP_ENCODE_INC_
  WEBP_WEBP_ENCODE_INC_ equ <>

  %include &WEBP_INC_PATH&\types.inc
  WEBP_ENCODER_ABI_VERSION equ 020fh
  WebPGetEncoderVersion proto WEBP_CALL_CONV
  WebPEncodeRGB proto WEBP_CALL_CONV :ptr uint8_t, :SDWORD, :SDWORD, :SDWORD, :REAL4, :ptr ptr uint8_t
  WebPEncodeBGR proto WEBP_CALL_CONV :ptr uint8_t, :SDWORD, :SDWORD, :SDWORD, :REAL4, :ptr ptr uint8_t
  WebPEncodeRGBA proto WEBP_CALL_CONV :ptr uint8_t, :SDWORD, :SDWORD, :SDWORD, :REAL4, :ptr ptr uint8_t
  WebPEncodeBGRA proto WEBP_CALL_CONV :ptr uint8_t, :SDWORD, :SDWORD, :SDWORD, :REAL4, :ptr ptr uint8_t
  WebPEncodeLosslessRGB proto WEBP_CALL_CONV :ptr uint8_t, :SDWORD, :SDWORD, :SDWORD, :ptr ptr uint8_t
  WebPEncodeLosslessBGR proto WEBP_CALL_CONV :ptr uint8_t, :SDWORD, :SDWORD, :SDWORD, :ptr ptr uint8_t
  WebPEncodeLosslessRGBA proto WEBP_CALL_CONV :ptr uint8_t, :SDWORD, :SDWORD, :SDWORD, :ptr ptr uint8_t
  WebPEncodeLosslessBGRA proto WEBP_CALL_CONV :ptr uint8_t, :SDWORD, :SDWORD, :SDWORD, :ptr ptr uint8_t
  WebPImageHint typedef SDWORD
  WEBP_HINT_DEFAULT equ 0
  WEBP_HINT_PICTURE equ 1
  WEBP_HINT_PHOTO equ 2
  WEBP_HINT_GRAPH equ 3
  WEBP_HINT_LAST equ 4

  WebPConfig struct
    lossless SDWORD ?
    quality REAL4 ?
    method_ SDWORD ?
    image_hint WebPImageHint ?
    target_size SDWORD ?
    target_PSNR REAL4 ?
    segments SDWORD ?
    sns_strength SDWORD ?
    filter_strength SDWORD ?
    filter_sharpness SDWORD ?
    filter_type SDWORD ?
    autofilter SDWORD ?
    alpha_compression SDWORD ?
    alpha_filtering SDWORD ?
    alpha_quality SDWORD ?
    pass SDWORD ?
    show_compressed SDWORD ?
    preprocessing SDWORD ?
    partitions SDWORD ?
    partition_limit SDWORD ?
    emulate_jpeg_size SDWORD ?
    thread_level SDWORD ?
    low_memory SDWORD ?
    near_lossless SDWORD ?
    exact SDWORD ?
    use_delta_palette SDWORD ?
    use_sharp_yuv SDWORD ?
    pad uint32_t 2 dup (?)
  WebPConfig ends
  WebPPreset typedef SDWORD
  WEBP_PRESET_DEFAULT equ 0
  WEBP_PRESET_PICTURE equ 1
  WEBP_PRESET_PHOTO equ 2
  WEBP_PRESET_DRAWING equ 3
  WEBP_PRESET_ICON equ 4
  WEBP_PRESET_TEXT equ 5

  WebPConfigInitInternal proto WEBP_CALL_CONV :ptr WebPConfig, :WebPPreset, :REAL4, :SDWORD
  WebPConfigInit proto WEBP_CALL_CONV :ptr WebPConfig
  WebPConfigPreset proto WEBP_CALL_CONV :ptr WebPConfig, :WebPPreset, :REAL4
  WebPConfigLosslessPreset proto WEBP_CALL_CONV :ptr WebPConfig, :SDWORD
  WebPValidateConfig proto WEBP_CALL_CONV :ptr WebPConfig
  WebPAuxStats struct
    coded_size SDWORD ?
    PSNR REAL4 5 dup (?)
    block_count SDWORD 3 dup (?)
    header_bytes SDWORD 2 dup (?)
    residual_bytes SDWORD 4 dup (?)
    segment_size SDWORD 4 dup (?)
    segment_quant SDWORD 4 dup (?)
    segment_level SDWORD 4 dup (?)
    alpha_data_size SDWORD ?
    layer_data_size SDWORD ?
    lossless_features uint32_t ?
    histogram_bits SDWORD ?
    transform_bits SDWORD ?
    cache_bits SDWORD ?
    palette_size SDWORD ?
    lossless_size SDWORD ?
    lossless_hdr_size SDWORD ?
    lossless_data_size SDWORD ?
    pad uint32_t 2 dup (?)
  WebPAuxStats ends
  TYPE_WebPWriterFunction typedef proto WEBP_CALL_CONV :ptr uint8_t, :size_t, :ptr WebPPicture
  WebPWriterFunction typedef ptr TYPE_WebPWriterFunction
  WebPMemoryWriter struct
    mem POINTER ?
    size_ size_t ?
    max_size size_t ?
    pad uint32_t 1 dup (?)
  WebPMemoryWriter ends
  WebPMemoryWriterInit proto WEBP_CALL_CONV :ptr WebPMemoryWriter
  WebPMemoryWriterClear proto WEBP_CALL_CONV :ptr WebPMemoryWriter
  WebPMemoryWrite proto WEBP_CALL_CONV :ptr uint8_t, :size_t, :ptr WebPPicture
  TYPE_WebPProgressHook typedef proto WEBP_CALL_CONV :SDWORD, :ptr WebPPicture
  WebPProgressHook typedef ptr TYPE_WebPProgressHook
  WebPEncCSP typedef SDWORD
  WEBP_YUV420 equ 0
  WEBP_YUV420A equ 4
  WEBP_CSP_UV_MASK equ 3
  WEBP_CSP_ALPHA_BIT equ 4

  WebPEncodingError typedef SDWORD
  VP8_ENC_OK equ 0
  VP8_ENC_ERROR_OUT_OF_MEMORY equ 1
  VP8_ENC_ERROR_BITSTREAM_OUT_OF_MEMORY equ 2
  VP8_ENC_ERROR_NULL_PARAMETER equ 3
  VP8_ENC_ERROR_INVALID_CONFIGURATION equ 4
  VP8_ENC_ERROR_BAD_DIMENSION equ 5
  VP8_ENC_ERROR_PARTITION0_OVERFLOW equ 6
  VP8_ENC_ERROR_PARTITION_OVERFLOW equ 7
  VP8_ENC_ERROR_BAD_WRITE equ 8
  VP8_ENC_ERROR_FILE_TOO_BIG equ 9
  VP8_ENC_ERROR_USER_ABORT equ 10
  VP8_ENC_ERROR_LAST equ 11

  WEBP_MAX_DIMENSION equ 16383
  WebPPicture struct
    use_argb SDWORD ?
    colorspace WebPEncCSP ?
    width_ SDWORD ?
    y POINTER ?
    y_stride SDWORD ?
    a POINTER ?
    a_stride SDWORD ?
    pad1 uint32_t 2 dup (?)
    argb POINTER ?
    argb_stride SDWORD ?
    pad2 uint32_t 3 dup (?)
    writer WebPWriterFunction ?
    custom_ptr POINTER ?
    extra_info_type SDWORD ?
    extra_info POINTER ?
    stats POINTER ?
    error_code WebPEncodingError ?
    progress_hook WebPProgressHook ?
    user_data POINTER ?
    pad3 uint32_t 3 dup (?)
    pad4 POINTER ?
    pad6 uint32_t 8 dup (?)
    memory_ POINTER ?
    memory_argb_ POINTER ?
    pad7 POINTER 2 dup (?)
  WebPPicture ends
  WebPPictureInitInternal proto WEBP_CALL_CONV :ptr WebPPicture, :SDWORD
  WebPPictureInit proto WEBP_CALL_CONV :ptr WebPPicture
  WebPPictureAlloc proto WEBP_CALL_CONV :ptr WebPPicture
  WebPPictureFree proto WEBP_CALL_CONV :ptr WebPPicture
  WebPPictureCopy proto WEBP_CALL_CONV :ptr WebPPicture, :ptr WebPPicture
  WebPPlaneDistortion proto WEBP_CALL_CONV :ptr uint8_t, :size_t, :ptr uint8_t, :size_t, :SDWORD, :SDWORD, :size_t, :SDWORD, :ptr REAL4, :ptr REAL4
  WebPPictureDistortion proto WEBP_CALL_CONV :ptr WebPPicture, :ptr WebPPicture, :SDWORD, :ptr REAL4
  WebPPictureCrop proto WEBP_CALL_CONV :ptr WebPPicture, :SDWORD, :SDWORD, :SDWORD, :SDWORD
  WebPPictureView proto WEBP_CALL_CONV :ptr WebPPicture, :SDWORD, :SDWORD, :SDWORD, :SDWORD, :ptr WebPPicture
  WebPPictureIsView proto WEBP_CALL_CONV :ptr WebPPicture
  WebPPictureRescale proto WEBP_CALL_CONV :ptr WebPPicture, :SDWORD, :SDWORD
  WebPPictureImportRGB proto WEBP_CALL_CONV :ptr WebPPicture, :ptr uint8_t, :SDWORD
  WebPPictureImportRGBA proto WEBP_CALL_CONV :ptr WebPPicture, :ptr uint8_t, :SDWORD
  WebPPictureImportRGBX proto WEBP_CALL_CONV :ptr WebPPicture, :ptr uint8_t, :SDWORD
  WebPPictureImportBGR proto WEBP_CALL_CONV :ptr WebPPicture, :ptr uint8_t, :SDWORD
  WebPPictureImportBGRA proto WEBP_CALL_CONV :ptr WebPPicture, :ptr uint8_t, :SDWORD
  WebPPictureImportBGRX proto WEBP_CALL_CONV :ptr WebPPicture, :ptr uint8_t, :SDWORD
  WebPPictureARGBToYUVA proto WEBP_CALL_CONV :ptr WebPPicture, :WebPEncCSP
  WebPPictureARGBToYUVADithered proto WEBP_CALL_CONV :ptr WebPPicture, :WebPEncCSP, :REAL4
  WebPPictureSharpARGBToYUVA proto WEBP_CALL_CONV :ptr WebPPicture
  WebPPictureSmartARGBToYUVA proto WEBP_CALL_CONV :ptr WebPPicture
  WebPPictureYUVAToARGB proto WEBP_CALL_CONV :ptr WebPPicture
  WebPCleanupTransparentArea proto WEBP_CALL_CONV :ptr WebPPicture
  WebPPictureHasTransparency proto WEBP_CALL_CONV :ptr WebPPicture
  WebPBlendAlpha proto WEBP_CALL_CONV :ptr WebPPicture, :uint32_t
  WebPEncode proto WEBP_CALL_CONV :ptr WebPConfig, :ptr WebPPicture
endif
 