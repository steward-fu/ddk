'' FreeBASIC binding for gsl-1.16
''
'' based on the C header files:
''   sort/gsl_sort_vector_char.h
''
''   Copyright (C) 1996, 1997, 1998, 1999, 2000, 2007 Thomas Walter, Brian Gough
''
''   This program is free software; you can redistribute it and/or modify
''   it under the terms of the GNU General Public License as published by
''   the Free Software Foundation; either version 3 of the License, or (at
''   your option) any later version.
''
''   This program is distributed in the hope that it will be useful, but
''   WITHOUT ANY WARRANTY; without even the implied warranty of
''   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
''   General Public License for more details.
''
''   You should have received a copy of the GNU General Public License
''   along with this program; if not, write to the Free Software
''   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
''
'' translated to FreeBASIC by:
''   FreeBASIC development team

#pragma once

#include once "crt/stdlib.bi"
#include once "gsl/gsl_errno.bi"
#include once "gsl/gsl_permutation.bi"
#include once "gsl/gsl_vector_char.bi"

extern "C"

#define __GSL_SORT_VECTOR_CHAR_H__
declare sub gsl_sort_vector_char(byval v as gsl_vector_char ptr)
declare sub gsl_sort_vector2_char(byval v1 as gsl_vector_char ptr, byval v2 as gsl_vector_char ptr)
declare function gsl_sort_vector_char_index(byval p as gsl_permutation ptr, byval v as const gsl_vector_char ptr) as long
declare function gsl_sort_vector_char_smallest(byval dest as byte ptr, byval k as const uinteger, byval v as const gsl_vector_char ptr) as long
declare function gsl_sort_vector_char_largest(byval dest as byte ptr, byval k as const uinteger, byval v as const gsl_vector_char ptr) as long
declare function gsl_sort_vector_char_smallest_index(byval p as uinteger ptr, byval k as const uinteger, byval v as const gsl_vector_char ptr) as long
declare function gsl_sort_vector_char_largest_index(byval p as uinteger ptr, byval k as const uinteger, byval v as const gsl_vector_char ptr) as long

end extern
