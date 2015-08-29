/**
 *
 *  openCatacomb
 *  Copyright (C) 2015 Scott R. Smith
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in
 *  all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 *  THE SOFTWARE.
 *
 */

#ifndef SHADER_H_INCLUDED
#define SHADER_H_INCLUDED

#include "types.h"

#define CHECK_FLAG(X,Y) ((X & Y) == Y)

enum {
    SHADER  =0x01,
    PROGRAM =0x02
};

typedef struct PAIR_T {
    GLuint loc;
    char *name;
} pair_t;

typedef struct INTERFACE_T {
    pair_t *items;
    uint32_t count;
} interface_t;

typedef struct SHADER_T {
    const char *path;
    GLenum  type;
    GLuint  name;
} shader_t;

typedef struct PROGRAM_T {
    GLuint id;
    shader_t shaders[2];
    interface_t attributes;
    interface_t uniforms;
} program_t;

void ShaderClose( void );
int8_t ShaderLoad( const char *path_vertex, const char *path_frag );
void ShaderUse( uint8_t index );

int8_t LoadShader( GLenum type, const char *path, shader_t *shader );
GLuint CompileShader( GLenum type, const char *path );
int8_t CreateProgram( program_t *program );
void FindAttributes( GLuint program, interface_t *attributes );
void FindUniforms( GLuint program, interface_t *uniforms );
GLint GetAttribute( uint8_t index, const char *attribute );
GLint GetUniform( uint8_t index, const char *uniform );
void PrintLog( uint8_t type, GLuint id );
uint32_t LoadToMemory( const char *filename, uint8_t **buffer );

#endif /* SHADER_H_INCLUDED */
