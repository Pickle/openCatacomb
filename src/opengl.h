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

#ifndef OPENGL_H_INCLUDED
#define OPENGL_H_INCLUDED

#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define VERTICES    4
#define INDICES     6

#if defined(USE_GL2)
    #define SHADER_VERT "shaders/shader_texture.vert"
    #define SHADER_FRAG "shaders/shader_texture.frag"
#endif
#if defined(PANDORA)
    #define GL_BGR  GL_RGB
#else
    #if defined(USE_GLES2) || defined(USE_GLES1)
        #define GL_BGR  GL_RGB
        #define GL_BGRA GL_BGRA_EXT
    #endif
#endif

typedef struct VERTEX_T {
    GLshort pos[2];
    GLfloat tex[2];
} vertex_t;

#define STRIDE_VTX  (sizeof(vertex_t))

extern font_t font;
extern tex_t textiles[graphmode_total];
extern tex_t texpics[graphmode_total];
extern tex_t screen_title;
extern tex_t screen_end;

int8_t OGL_Init( uint16_t in_w, uint16_t in_h, uint16_t out_w, uint16_t out_h );
GLuint OGL_OpenTexture( uint16_t width, uint16_t height, uint8_t bpp, void *data );
void OrthoMatrix( GLfloat *m, GLfloat l, GLfloat r, GLfloat b, GLfloat t, GLfloat n, GLfloat f );
void OGL_CheckErrors( const char *file, uint32_t line );
void OGL_Close( void );
void OGL_DrawImage( tex_t *texture, boolean flip, SDL_Rect *dst, SDL_Rect *src );
void OGL_DrawString( font_t *f, float scale, uint32_t x, uint32_t y, const char *string, ... );
void OGL_DrawTextures( GLenum mode, GLuint id, GLsizei count, vertex_t *vtx, GLushort *idx );

#if defined(USE_FBO)
boolean OGL_OpenFBO( uint16_t w_in, uint16_t h_in, uint16_t w_out, uint16_t h_out );
void OGL_CloseFBO( void );
void OGL_SelectBuffer( boolean active );
void OGL_RenderBuffer( void );
#endif /* USE_FBO */

#if defined(WIN32)
uint8_t LoadGLFunctions( void );
#endif

#if defined(USE_FBO) && !defined(USE_GLES2)
#define GL_FRAMEBUFFER                                  GL_FRAMEBUFFER_EXT
#define GL_FRAMEBUFFER_COMPLETE                         GL_FRAMEBUFFER_COMPLETE_EXT
#define GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT            GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT
#define GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS            GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT
#define GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT    GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT
#define GL_FRAMEBUFFER_UNSUPPORTED                      GL_FRAMEBUFFER_UNSUPPORTED_EXT
#define GL_COLOR_ATTACHMENT0                            GL_COLOR_ATTACHMENT0_EXT
#define glBindFramebuffer                               glBindFramebufferEXT
#define glDeleteFramebuffers                            glDeleteFramebuffersEXT
#define glGenFramebuffers                               glGenFramebuffersEXT
#define glCheckFramebufferStatus                        glCheckFramebufferStatusEXT
#define glFramebufferTexture2D                          glFramebufferTexture2DEXT
#define glGenRenderbuffers                              glGenRenderbuffersEXT
#define glBindRenderbuffer                              glBindRenderbufferEXT
#define glRenderbufferStorage                           glRenderbufferStorageEXT
#define glDeleteRenderbuffers                           glDeleteRenderbuffersEXT
#endif

#if defined(WIN32)
#if defined(USE_GL2)
/* OpenGL Version 2.0 API */
extern PFNGLDELETESHADERPROC                glDeleteShader;
extern PFNGLDELETEPROGRAMPROC               glDeleteProgram;
extern PFNGLDETACHSHADERPROC                glDetachShader;
extern PFNGLUSEPROGRAMPROC                  glUseProgram;
extern PFNGLCREATESHADERPROC                glCreateShader;
extern PFNGLSHADERSOURCEPROC                glShaderSource;
extern PFNGLCOMPILESHADERPROC               glCompileShader;
extern PFNGLGETSHADERIVPROC                 glGetShaderiv;
extern PFNGLCREATEPROGRAMPROC               glCreateProgram;
extern PFNGLATTACHSHADERPROC                glAttachShader;
extern PFNGLLINKPROGRAMPROC                 glLinkProgram;
extern PFNGLGETPROGRAMIVPROC                glGetProgramiv;
extern PFNGLGETACTIVEATTRIBPROC             glGetActiveAttrib;
extern PFNGLGETATTRIBLOCATIONPROC           glGetAttribLocation;
extern PFNGLGETACTIVEUNIFORMPROC            glGetActiveUniform;
extern PFNGLGETUNIFORMLOCATIONPROC          glGetUniformLocation;
extern PFNGLGETSHADERINFOLOGPROC            glGetShaderInfoLog;
extern PFNGLGETPROGRAMINFOLOGPROC           glGetProgramInfoLog;
extern PFNGLDISABLEVERTEXATTRIBARRAYPROC    glDisableVertexAttribArray;
extern PFNGLENABLEVERTEXATTRIBARRAYPROC     glEnableVertexAttribArray;
extern PFNGLVERTEXATTRIBPOINTERPROC         glVertexAttribPointer;
extern PFNGLUNIFORM1IPROC                   glUniform1i;
extern PFNGLUNIFORM1FVPROC                  glUniform1fv;
extern PFNGLUNIFORM3FVPROC                  glUniform3fv;
extern PFNGLUNIFORM4FVPROC                  glUniform4fv;
extern PFNGLUNIFORMMATRIX3FVPROC            glUniformMatrix3fv;
extern PFNGLUNIFORMMATRIX4FVPROC            glUniformMatrix4fv;
#endif
/* FBO API */
#if defined(USE_FBO)
extern PFNGLBINDFRAMEBUFFEREXTPROC          glBindFramebuffer;
extern PFNGLDELETEFRAMEBUFFERSEXTPROC       glDeleteFramebuffers;
extern PFNGLGENFRAMEBUFFERSEXTPROC          glGenFramebuffers;
extern PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC   glCheckFramebufferStatus;
extern PFNGLFRAMEBUFFERTEXTURE2DEXTPROC     glFramebufferTexture2D;
extern PFNGLGENRENDERBUFFERSEXTPROC         glGenRenderbuffers;
extern PFNGLBINDRENDERBUFFEREXTPROC         glBindRenderbuffer;
extern PFNGLRENDERBUFFERSTORAGEEXTPROC      glRenderbufferStorage;
extern PFNGLDELETERENDERBUFFERSEXTPROC      glDeleteRenderbuffers;
#endif /* defined(USE_FBO) */

#if defined(USE_GL1) || defined(USE_GL2)
/* GL_ARB_texture_compression */
extern PFNGLCOMPRESSEDTEXIMAGE2DARBPROC     glCompressedTexImage2D;
extern PFNGLGETCOMPRESSEDTEXIMAGEARBPROC    glGetCompressedTexImageARB;
#endif
#endif /* WIN32 */

#ifdef __cplusplus
}
#endif

#endif /* OPENGL_H_INCLUDED */
