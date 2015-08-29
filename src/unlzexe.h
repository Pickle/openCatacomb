/* unlzexe ver 0.5 (PC-VAN UTJ44266 Kou )
*   UNLZEXE converts the compressed file by lzexe(ver.0.90,0.91) to the
*   UNcompressed executable one.
*
*   usage:  UNLZEXE packedfile[.EXE] [unpackedfile.EXE]
*/

#ifndef UNLZEXE_H_
#define UNLZEXE_H_

int unlzexe( const char *ipath, const char *opath );

#endif /* UNLZEXE_H_ */
