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

#if defined(USE_GL2)

#include "shader.h"
#include "opengl.h"
#include "util.h"

uint8_t pc = 0;
program_t *programs;

void ShaderClose( void )
{
    uint32_t i, j;

    for (i=0; i<pc; i++)
    {
        for (j=0; j<programs[i].attributes.count; j++)
        {
            free( programs[i].attributes.items[j].name );
        }
        free( programs[i].attributes.items );

        for (j=0; j<programs[i].uniforms.count; j++)
        {
            free( programs[i].uniforms.items[j].name );
        }
        free( programs[i].uniforms.items );

        if (programs[i].id != GL_INVALID_VALUE) {
            glDeleteProgram( programs[i].id );
        }
    }

    free( programs );
}

int8_t ShaderLoad( const char *path_vertex, const char *path_frag )
{
    program_t *reprog = NULL;

    pc++;
    reprog = realloc( programs, sizeof(program_t) * pc);
    if (reprog == NULL) {
        return -1;
    }

    programs = reprog;
    programs[pc-1].attributes.count = 0;
    programs[pc-1].attributes.items = NULL;
    programs[pc-1].uniforms.count = 0;
    programs[pc-1].uniforms.items = NULL;

    if (LoadShader( GL_VERTEX_SHADER, path_vertex, &programs[pc-1].shaders[0] ))
        return -1;

    if (LoadShader( GL_FRAGMENT_SHADER, path_frag, &programs[pc-1].shaders[1] ))
        return -1;

    if (CreateProgram( &programs[pc-1] ))
        return -1;

    return pc-1;
}

void ShaderUse( uint8_t index )
{
    if (index < pc)
    {
        glUseProgram( programs[index].id );
    }
}

int8_t LoadShader( GLenum type, const char *path, shader_t *shader )
{
    printf( "[SHADER] Compiling %s\n", path );

    shader->type = type;
    shader->path = path;
    shader->name = CompileShader( shader->type, shader->path );

    if (shader->name == GL_INVALID_VALUE) {
        return 1;
    }
    else {
        printf( "[SHADER] Compiled %s\n", path );
    }

    return 0;
}

GLuint CompileShader( GLenum type, const char *path )
{
    GLint       status = 0;
    GLuint      shader = 0;
    const char *shadertype = NULL;
    uint8_t    *source = NULL;
    uint32_t    size = 0;

    size = LoadDataFromFile( path, &source, 0, 0 );
    printf( "[SHADER] Loaded shader source: %d bytes\n", size );

#if defined(USE_GLES2)
    uint8_t     length = 0;
    uint8_t    *temp = NULL;
    const char *precision = "precision mediump float;\n";

    length = strlen(precision);
    temp = malloc(size+length+1);

    memcpy( temp, precision, length );
    memcpy( temp+length, source, size );
    temp[size+length] = '\0';

    free( source );
    source = temp;
#endif

    if (source != NULL)
    {
#if defined(DEBUG)
        printf( "[SHADER] Shader Source Begin\n%s\nShader Source End\n", source );
#endif

        shader = glCreateShader( type );
        glShaderSource( shader, 1, (const char**)&source, NULL );
        glCompileShader( shader );

        free( source );

        glGetShaderiv( shader, GL_COMPILE_STATUS, &status );
        if (status == GL_FALSE)
        {
            switch(type)
            {
                case GL_VERTEX_SHADER:      shadertype = "vertex"; break;
                case GL_FRAGMENT_SHADER:    shadertype = "fragment"; break;
                default :                   shadertype = "unknown"; break;
            }

            printf( "[SHADER] ERROR: Compile failure in %s shader: %s\n", shadertype, path );
            PrintLog( SHADER, shader );
            return GL_INVALID_VALUE;
        }
    }
    else
    {
        printf( "[SHADER] ERROR: File read failure in %d shader: %s\n", type, path );
        return GL_INVALID_VALUE;
    }

    return shader;
}

int8_t CreateProgram( program_t *program )
{
    GLint status;

    program->id = glCreateProgram();

    glAttachShader( program->id, program->shaders[0].name );
    glAttachShader( program->id, program->shaders[1].name );
    glLinkProgram( program->id );

    glGetProgramiv( program->id, GL_LINK_STATUS, &status );
    if (status == GL_FALSE)
    {
        printf( "[SHADER] ERROR: Linker failure\n" );
        PrintLog( program->id, PROGRAM );
        program->id = GL_INVALID_VALUE;
        return 1 ;
    }

    FindAttributes( program->id, &program->attributes );
    FindUniforms( program->id, &program->uniforms );

    /* Release unused shader objects */
    glDetachShader( program->id, program->shaders[0].name );
    glDetachShader( program->id, program->shaders[1].name );
    glDeleteShader( program->shaders[0].name );
    glDeleteShader( program->shaders[1].name );
    program->shaders[0].name = GL_INVALID_VALUE;
    program->shaders[1].name = GL_INVALID_VALUE;

    return 0;
}

void FindAttributes( GLuint program, interface_t *attributes )
{
    GLuint index;
    GLint numAttributes;
    GLint maxAttributeLen;
    char *attributeName = NULL;

    glGetProgramiv( program, GL_ACTIVE_ATTRIBUTES, &numAttributes );
    glGetProgramiv( program, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &maxAttributeLen );
    attributeName = malloc(maxAttributeLen);

    attributes->count = numAttributes;
    attributes->items = malloc( sizeof(interface_t) * attributes->count );

    for (index=0; index < attributes->count; index++)
    {
        GLint size;
        GLenum type;

        glGetActiveAttrib( program, index, maxAttributeLen, NULL, &size, &type, attributeName );
        attributes->items[index].loc = glGetAttribLocation( program, attributeName );
        attributes->items[index].name = malloc( maxAttributeLen );
        strncpy( attributes->items[index].name, attributeName, maxAttributeLen );
    }

    free( attributeName );
}

void FindUniforms( GLuint program, interface_t *uniforms )
{
    GLuint index;
    GLint numUniforms;
    GLint maxUniformLen;
    char *uniformName;

    glGetProgramiv( program, GL_ACTIVE_UNIFORMS, &numUniforms );
    glGetProgramiv( program, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxUniformLen );
    uniformName = malloc(maxUniformLen);

    uniforms->count = numUniforms;
    uniforms->items = malloc( sizeof(interface_t) * uniforms->count );

    for (index=0; index < uniforms->count; index++)
    {
        GLint size;
        GLenum type;

        glGetActiveUniform( program, index, maxUniformLen, NULL, &size, &type, uniformName );
        uniforms->items[index].loc  = glGetUniformLocation( program, uniformName );
        uniforms->items[index].name = malloc( maxUniformLen );
        strncpy( uniforms->items[index].name, uniformName, maxUniformLen );
    }

    free( uniformName );
}

GLint GetAttribute( uint8_t index, const char *attribute )
{
    uint32_t i;

    for (i=0; i<programs[index].attributes.count; i++)
    {
        if (strcmp( programs[index].attributes.items[i].name, attribute) == 0)
        {
            return programs[index].attributes.items[i].loc;
        }
    }

    printf( "[SHADER] ERROR: Could not locate attribute out of %d: %s\n", programs[index].attributes.count, attribute );

    return GL_INVALID_VALUE;
}

GLint GetUniform( uint8_t index, const char *uniform )
{
    uint32_t i;

    for (i=0; i<programs[index].uniforms.count; i++)
    {
        if (strcmp( programs[index].uniforms.items[i].name, uniform) == 0)
        {
            return programs[index].uniforms.items[i].loc;
        }
    }

    printf( "[SHADER] ERROR: Could not locate uniform out of %d: : %s\n", programs[index].uniforms.count, uniform );

    return GL_INVALID_VALUE;
}

void PrintLog( uint8_t type, GLuint id )
{
    GLint loglength;
    char *shaderlog;

    if (CHECK_FLAG(type, SHADER))
    {
        glGetShaderiv( id, GL_INFO_LOG_LENGTH, &loglength );

        shaderlog = malloc(loglength+1);
        glGetShaderInfoLog( id, loglength, NULL, shaderlog );
        printf( "[SHADER]: Shader log:\n %s", shaderlog );
        free( shaderlog );
    }

    if (CHECK_FLAG(type, PROGRAM))
    {
        glGetProgramiv( id, GL_INFO_LOG_LENGTH, &loglength );

        shaderlog = malloc(loglength + 1);
        glGetProgramInfoLog( id, loglength, NULL, shaderlog );
        printf( "[SHADER]: Program shaderlog:\n %s", shaderlog );
        free( shaderlog );
    }
}

#endif
