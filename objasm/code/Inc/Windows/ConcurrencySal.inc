;Include file created by h2incX, Version B.01.01, 02/09/2018 16:44
;(C) 2018-today by Biterider, 2005-2009 by Japheth
;Source file: '\ObjAsm\Projects\h2IncX\header\ConcurrencySal.h', last modified: 19/04/2018 10:43
;Creation parameters: 

ifndef CONCURRENCYSAL_H
  CONCURRENCYSAL_H equ <>
  ifdef __cplusplus
    ;[...]
  endif
  if  not Defined(__midl) and Defined(_PREFAST_) and _MSC_VER ge 1000 and  not Defined(_SDV_)
    ;[...]
  else
    ifndef _Interlocked_operand_
      _Interlocked_operand_ equ <>
    endif
    _Guarded_by_ macro lock_
      exitm <>
    endm
    _Write_guarded_by_ macro lock_
      exitm <>
    endm
    _Interlocked_ equ <>
    _Requires_lock_held_ macro lock_
      exitm <>
    endm
    _Requires_exclusive_lock_held_ macro lock_
      exitm <>
    endm
    _Requires_shared_lock_held_ macro lock_
      exitm <>
    endm
    _Requires_lock_not_held_ macro lock_
      exitm <>
    endm
    _Requires_no_locks_held_ equ <>
    _Acquires_lock_ macro lock_
      exitm <>
    endm
    _Acquires_exclusive_lock_ macro lock_
      exitm <>
    endm
    _Acquires_shared_lock_ macro lock_
      exitm <>
    endm
    _Releases_lock_ macro lock_
      exitm <>
    endm
    _Releases_exclusive_lock_ macro lock_
      exitm <>
    endm
    _Releases_shared_lock_ macro lock_
      exitm <>
    endm
    _Acquires_nonreentrant_lock_ macro lock_
      exitm <>
    endm
    _Releases_nonreentrant_lock_ macro lock_
      exitm <>
    endm
    _Post_same_lock_ macro lock1, lock2
      exitm <>
    endm
    _Internal_set_lock_count_ macro lock_, count
      exitm <>
    endm
    _Create_lock_level_ macro level
      exitm <>
    endm
    _Has_lock_level_ macro level
      exitm <>
    endm
    _Internal_lock_level_order_ macro a, b
      exitm <>
    endm
    _Csalcat1_ macro x, y
      exitm <>
    endm
    _Csalcat2_ macro x, y
      exitm <>
    endm
    _Lock_level_order_ macro a, b
      exitm <>
    endm
    _No_competing_thread_ equ <>
    _Analysis_assume_lock_acquired_ macro lock_
      exitm <>
    endm
    _Analysis_assume_lock_released_ macro lock_
      exitm <>
    endm
    _Analysis_assume_lock_held_ macro lock_
      exitm <>
    endm
    _Analysis_assume_lock_not_held_ macro lock_
      exitm <>
    endm
    _Analysis_assume_same_lock_ macro lock1, lock2
      exitm <>
    endm
    _Function_ignore_lock_checking_ macro lock_
      exitm <>
    endm
    _Analysis_suppress_lock_checking_ macro lock_
      exitm <>
    endm
    _Benign_race_begin_ equ <__pragma (warning (push))>
    _Benign_race_end_ equ <__pragma (warning (pop))>
    _No_competing_thread_begin_ equ <__pragma (warning (push))>
    _No_competing_thread_end_ equ <__pragma (warning (pop))>
    _Has_lock_kind_ macro kind
      exitm <>
    endm
    __guarded_by macro lock_
      exitm <>
    endm
    __write_guarded_by macro lock_
      exitm <>
    endm
    __interlocked equ <>
    __requires_lock_held macro lock_
      exitm <>
    endm
    __requires_exclusive_lock_held macro lock_
      exitm <>
    endm
    __requires_shared_lock_held macro lock_
      exitm <>
    endm
    __requires_lock_not_held macro lock_
      exitm <>
    endm
    __requires_no_locks_held equ <>
    __acquires_lock macro lock_
      exitm <>
    endm
    __acquires_exclusive_lock macro lock_
      exitm <>
    endm
    __acquires_shared_lock macro lock_
      exitm <>
    endm
    __releases_lock macro lock_
      exitm <>
    endm
    __releases_exclusive_lock macro lock_
      exitm <>
    endm
    __releases_shared_lock macro lock_
      exitm <>
    endm
    __has_lock_property macro kind
      exitm <>
    endm
    __declare_lock_level macro level
      exitm <>
    endm
    __has_lock_level macro level
      exitm <>
    endm
    __internal_lock_level_order macro a, b
      exitm <>
    endm
    CSALCAT1 macro x, y
      exitm <>
    endm
    CSALCAT2 macro x, y
      exitm <>
    endm
    __lock_level_order macro a, b
      exitm <>
    endm
    __no_competing_thread equ <>
    __analysis_assume_lock_acquired macro lock_
      exitm <>
    endm
    __analysis_assume_lock_released macro lock_
      exitm <>
    endm
    __function_ignore_lock_checking macro lock_
      exitm <>
    endm
    __analysis_suppress_lock_checking macro lock_
      exitm <>
    endm
    BENIGN_RACE_BEGIN equ <__pragma (warning (push))>
    BENIGN_RACE_END equ <__pragma (warning (pop))>
    NO_COMPETING_THREAD_BEGIN equ <__pragma (warning (push))>
    NO_COMPETING_THREAD_END equ <__pragma (warning (pop))>
  endif
  ifdef __cplusplus
    ;[...]
  endif
endif

