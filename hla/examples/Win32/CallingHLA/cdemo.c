/*
** A demonstration of how you can call HLA code
** that calls the HLA Standard Library from code
** that is not an HLA main program (in this case, it's
** a "C" program).
**
** Note: this program was compiled with Microsoft VC++
** using the following command lines:
**
**	c:>vcvars32
**  c:>hla -c hlafunc.hla
**  c:>cl cdemo.c hlafunc.obj
*/

#include <stdio.h>

extern void hlaFunc( int value );

int
main( void )
{
	printf( "Calling HLA code\n" );
	hlaFunc( 10 );
	printf( "Returned from HLA code\n" );
	
	return 0;

}
