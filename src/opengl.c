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

#include "opengl.h"
#include "system.h"
#include "util.h"
#if defined(USE_GL2)
#include "shader.h"
#endif

uint16_t screen_x, screen_y, screen_w, screen_h;
font_t font;
tex_t textiles[graphmode_total];
tex_t texpics[graphmode_total];
tex_t screen_title;
tex_t screen_end;
GLfloat *matMVP;
GLfloat matRender[16];
GLfloat matScreen[16];

int8_t shader_texture;
GLuint shader_pos;
GLuint shader_tex;
GLuint shader_mvp;

#if defined(USE_FBO)
SDL_Rect render_rect;
boolean fbo_enabled = False;
fbo_t fbo;
#endif

#if defined(WIN32)
/* OpenGL Version 2.0 API */
#if defined(USE_GL2)
PFNGLDELETESHADERPROC               glDeleteShader              = NULL;
PFNGLDELETEPROGRAMPROC              glDeleteProgram             = NULL;
PFNGLDETACHSHADERPROC               glDetachShader              = NULL;
PFNGLUSEPROGRAMPROC                 glUseProgram                = NULL;
PFNGLCREATESHADERPROC               glCreateShader              = NULL;
PFNGLSHADERSOURCEPROC               glShaderSource              = NULL;
PFNGLCOMPILESHADERPROC              glCompileShader             = NULL;
PFNGLGETSHADERIVPROC                glGetShaderiv               = NULL;
PFNGLCREATEPROGRAMPROC              glCreateProgram             = NULL;
PFNGLATTACHSHADERPROC               glAttachShader              = NULL;
PFNGLLINKPROGRAMPROC                glLinkProgram               = NULL;
PFNGLGETPROGRAMIVPROC               glGetProgramiv              = NULL;
PFNGLGETACTIVEATTRIBPROC            glGetActiveAttrib           = NULL;
PFNGLGETATTRIBLOCATIONPROC          glGetAttribLocation         = NULL;
PFNGLGETACTIVEUNIFORMPROC           glGetActiveUniform          = NULL;
PFNGLGETUNIFORMLOCATIONPROC         glGetUniformLocation        = NULL;
PFNGLGETSHADERINFOLOGPROC           glGetShaderInfoLog          = NULL;
PFNGLGETPROGRAMINFOLOGPROC          glGetProgramInfoLog         = NULL;
PFNGLDISABLEVERTEXATTRIBARRAYPROC   glDisableVertexAttribArray  = NULL;
PFNGLENABLEVERTEXATTRIBARRAYPROC    glEnableVertexAttribArray   = NULL;
PFNGLVERTEXATTRIBPOINTERPROC        glVertexAttribPointer       = NULL;
PFNGLUNIFORM1IPROC                  glUniform1i                 = NULL;
PFNGLUNIFORM1FVPROC                 glUniform1fv                = NULL;
PFNGLUNIFORM3FVPROC                 glUniform3fv                = NULL;
PFNGLUNIFORM4FVPROC                 glUniform4fv                = NULL;
PFNGLUNIFORMMATRIX3FVPROC           glUniformMatrix3fv          = NULL;
PFNGLUNIFORMMATRIX4FVPROC           glUniformMatrix4fv          = NULL;
#endif
/* FBO API */
#if defined(USE_FBO)
PFNGLBINDFRAMEBUFFEREXTPROC         glBindFramebuffer           = NULL;
PFNGLDELETEFRAMEBUFFERSEXTPROC      glDeleteFramebuffers        = NULL;
PFNGLGENFRAMEBUFFERSEXTPROC         glGenFramebuffers           = NULL;
PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC  glCheckFramebufferStatus    = NULL;
PFNGLFRAMEBUFFERTEXTURE2DEXTPROC    glFramebufferTexture2D      = NULL;
PFNGLGENRENDERBUFFERSEXTPROC        glGenRenderbuffers          = NULL;
PFNGLBINDRENDERBUFFEREXTPROC        glBindRenderbuffer          = NULL;
PFNGLRENDERBUFFERSTORAGEEXTPROC     glRenderbufferStorage       = NULL;
PFNGLDELETERENDERBUFFERSEXTPROC     glDeleteRenderbuffers       = NULL;
#endif /* defined(USE_FBO) */

#if defined(USE_GL1) || defined(USE_GL2)
/* GL_ARB_texture_compression */
PFNGLCOMPRESSEDTEXIMAGE2DARBPROC    glCompressedTexImage2D      = NULL;
PFNGLGETCOMPRESSEDTEXIMAGEARBPROC   glGetCompressedTexImageARB  = NULL;
#endif
#endif /* defined(WIN32) */

int8_t OGL_Init( uint16_t in_w, uint16_t in_h, uint16_t out_w, uint16_t out_h )
{
    const char *output;

    uint8_t i;

    /* Default values for textures */
    for (i=0; i<graphmode_total; i++)
    {
        textiles[i].id = 0;
        texpics[i].id = 0;
    }
    screen_title.id = 0;
    screen_end.id = 0;
    font.texture.id = 0;

#if defined(WIN32)
    LoadGLFunctions();
#endif

    /* OpenGL Information */
    output = (char*)glGetString( GL_VENDOR );
    printf( "[OPENGL] GL_VENDOR: %s\n", output );
    output = (char*)glGetString( GL_RENDERER );
    printf( "[OPENGL] GL_RENDERER: %s\n", output );
    output = (char*)glGetString( GL_VERSION );
    printf( "[OPENGL] GL_VERSION: %s\n", output );
#if defined(USE_GL2)
    output = (char*)glGetString( GL_SHADING_LANGUAGE_VERSION );
    printf( "[OPENGL] GL_SHADING_LANGUAGE_VERSION: %s\n", output );
#endif
    output = (char*)glGetString( GL_EXTENSIONS );
    printf( "[OPENGL] GL_EXTENSIONS: %s\n", output );

    screen_x = (out_w - in_w)/2;
    screen_y = (out_h - in_h)/2;
    screen_w = in_w;
    screen_h = in_h;

#if defined(USE_FBO)
    OGL_OpenFBO( in_w, in_h, out_w, out_h );

    if (fbo_enabled == True)
    {
        screen_x = 0;
        screen_y = 0;
        screen_w = out_w;
        screen_h = out_h;
    }
#endif

    OrthoMatrix( &matScreen[0], 0, screen_w, screen_h, 0, 0, 1 );

    glClearColor( 0,0,0,1 );
    glDisable( GL_BLEND );
    glDisable( GL_DEPTH_TEST );
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
    glViewport( screen_x, screen_y, screen_w, screen_h );
#if !defined(USE_GL2)
    glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE );
    glEnable( GL_TEXTURE_2D );
    glColor4f( 0,0,0,1 );
#endif

#if defined(USE_GL2)
    shader_texture = ShaderLoad( SHADER_VERT, SHADER_FRAG );

    if (shader_texture >= 0)
    {
        shader_pos = GetAttribute( shader_texture, "a_Position" );
        shader_tex = GetAttribute( shader_texture, "a_Texcoord0" );
        shader_mvp = GetUniform( shader_texture, "u_MVPMatrix" );

        ShaderUse( shader_texture );
    }
    else
    {
        printf( "[OPENGL] OGL_Init: Failed to compile shaders\n" );
        return 1;
    }
#endif

#if defined(USE_FBO)
    OGL_SelectBuffer( True );
#else
    #if defined(USE_GL2)
        matMVP = &matScreen[0];
    #else
        glMatrixMode( GL_MODELVIEW );
        glLoadIdentity();
        glMatrixMode( GL_PROJECTION );
        glLoadMatrixf( &matScreen[0] );
    #endif
#endif

    font.w = 7;
    font.h = 9;
    if (LoadBMP( "font.bmp", &font.texture ) == False)
    {
        printf( "ERROR Could not load font.bmp\n" );
        return 1;
    }

    OGL_CheckErrors( __FILE__, __LINE__ );

    return 0;
}

GLuint OGL_OpenTexture( uint16_t width, uint16_t height, uint8_t bpp, void* data )
{
    GLuint texture;
    GLuint iformat = ((bpp == 4) ? GL_RGBA : GL_RGB);
    GLuint oformat = ((bpp == 4) ? GL_BGRA : GL_BGR);

    if (bpp == 3 && oformat == GL_RGB)
    {
        SwapColors( data, height, width*bpp );
    }

    glGenTextures( 1, &texture );
    glBindTexture( GL_TEXTURE_2D, texture );

    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );

    glTexImage2D( GL_TEXTURE_2D, 0, iformat, width, height
                               , 0, oformat, GL_UNSIGNED_BYTE, data );

    OGL_CheckErrors( __FILE__, __LINE__ );

    return texture;
}

void OrthoMatrix( GLfloat *m, GLfloat l, GLfloat r, GLfloat b, GLfloat t, GLfloat n, GLfloat f )
{
    GLfloat r_l = r - l;
    GLfloat t_b = t - b;
    GLfloat f_n = f - n;
    GLfloat tx = - ((r + l) / (r - l));
    GLfloat ty = - ((t + b) / (t - b));
    GLfloat tz = - ((f + n) / (f - n));

    m[0] = 2.0f / r_l;
    m[1] = 0.0f;
    m[2] = 0.0f;
    m[3] = 0.0f;

    m[4] = 0.0f;
    m[5] = 2.0f / t_b;
    m[6] = 0.0f;
    m[7] = 0.0f;

    m[8] = 0.0f;
    m[9] = 0.0f;
    m[10] = -2.0f / f_n;
    m[11] = 0.0f;

    m[12] = tx;
    m[13] = ty;
    m[14] = tz;
    m[15] = 1.0f;
}

void OGL_CheckErrors( const char *file, uint32_t line )
{
    const char *msg = NULL;
    GLenum err;

    err = glGetError();
    if (err != GL_NO_ERROR)
    {
        switch (err)
        {
            case GL_INVALID_ENUM:       msg = "GL_INVALID_ENUM"; break;
            case GL_INVALID_VALUE:      msg = "GL_INVALID_VALUE"; break;
            case GL_INVALID_OPERATION:  msg = "GL_INVALID_OPERATION"; break;
            case GL_OUT_OF_MEMORY:      msg = "GL_OUT_OF_MEMORY"; break;
#if !defined(USE_GLES2)
            case GL_STACK_OVERFLOW:     msg = "GL_STACK_OVERFLOW"; break;
            case GL_STACK_UNDERFLOW:    msg = "GL_STACK_UNDERFLOW"; break;
#endif
#if !defined(USE_GLES1) && !defined(USE_GLES2)
            case GL_TABLE_TOO_LARGE:    msg = "GL_TABLE_TOO_LARGE"; break;
#endif
            default: msg = "UNKNOWN"; break;
        }

        printf( "[OPENGL] Error: %s (%4X) at %s line %d\n", msg, err, file, line );
    }
}

void OGL_Close( void )
{
    if (font.texture.id != 0) {
        glDeleteTextures( 1, &font.texture.id );
        font.texture.id = 0;
    }

#if defined(USE_FBO)
    OGL_CloseFBO();
#endif

#if defined(USE_GL2)
    ShaderClose();
#endif
}

void OGL_DrawImage( tex_t *texture, boolean flip, SDL_Rect *dst, SDL_Rect *src )
{
    vertex_t vtx[8];
    GLfloat x,y,w,h;
    GLfloat tx,ty,tw,th;

    /* Pos Coords */
    if (dst == NULL)
    {
        x = y = 0;
        w = texture->w;
        h = texture->h;
    }
    else
    {
        x = dst->x;
        y = dst->y;
        w = dst->w;
        h = dst->h;
    }

    if (w <= 0)
        w = src->w;
    if (h <= 0)
        h = src->h;

    /* x,y   A[===]B x+w,y      */
    /*        [ \ ]             */
    /*        [  \]             */
    /* x,y+h D[===]C x+w,y+h    */

    vtx[0].pos[0] = x;   vtx[0].pos[1] = y;   /* A */
    vtx[1].pos[0] = x+w; vtx[1].pos[1] = y;   /* B */
    vtx[2].pos[0] = x+w; vtx[2].pos[1] = y+h; /* C */
    vtx[3].pos[0] = x;   vtx[3].pos[1] = y+h; /* D */

    /* Tex Coords */
    if (src == NULL)
    {
        tx = ty = 0;
        tw = th = 1;
    }
    else
    {
        tx = (float)src->x/(float)texture->w;
        ty = (float)src->y/(float)texture->h;
        tw = (float)src->w/(float)texture->w;
        th = (float)src->h/(float)texture->h;
    }

    /* 0,1  A[===]B 1,1 */
    /*       [ \ ]      */
    /*       [  \]      */
    /* 0,0  D[===]C 1,0 */

    if (flip == True)
    {
        vtx[0].tex[0]  = tx;         vtx[0].tex[1]  = ty+th;  /* A */
        vtx[1].tex[0]  = tx+tw;      vtx[1].tex[1]  = ty+th;  /* B */
        vtx[2].tex[0]  = tx+tw;      vtx[2].tex[1]  = ty;     /* C */
        vtx[3].tex[0]  = tx;         vtx[3].tex[1]  = ty;     /* D */
    }
    else
    {
        vtx[0].tex[0]  = tx;         vtx[0].tex[1]  = ty;     /* D */
        vtx[1].tex[0]  = tx+tw;      vtx[1].tex[1]  = ty;     /* C */
        vtx[2].tex[0]  = tx+tw;      vtx[2].tex[1]  = ty+th;  /* B */
        vtx[3].tex[0]  = tx;         vtx[3].tex[1]  = ty+th;  /* A */
    }

    OGL_DrawTextures( GL_TRIANGLE_FAN, texture->id, 4, &vtx[0], NULL );

#if defined(DEBUG)
    OGL_CheckErrors( __FILE__, __LINE__ );
#endif
}

void OGL_DrawString( font_t *f, float scale, uint32_t x, uint32_t y, const char *string, ... )
{
    vertex_t *vtx = NULL;
    GLushort* idx = NULL;
    GLfloat w, h;
    GLfloat tx, ty, tw, th;
    uint32_t i, v, t, d;
    uint32_t xx, size;
    uint8_t rows, cols, ch;
    va_list fmtargs;
    char buffer[4096];

    va_start( fmtargs, string );
    vsnprintf( buffer, sizeof(buffer)-1, string, fmtargs );
    va_end( fmtargs );

    size = MIN( strlen(buffer), STRSIZE );

    if (size > 0)
    {
        rows = f->texture.h / f->h;
        cols = f->texture.w / f->w;

        vtx = malloc( sizeof(vertex_t) * VERTICES * size );
        idx = malloc( sizeof(GLushort) * INDICES * size );

        tw = (float)f->w / (float)f->texture.w;
        th = (float)f->h / (float)f->texture.h;

        xx = x;
        for( i=v=t=d=0; i<size; i++)
        {
            ch = buffer[i]-' ';
            tx = (float)(ch % cols)*tw;
            ty = 1.0f -(float)(rows-(ch / cols))*th;

            /* x,y   A[===]B x+w,y      */
            /*        [ \ ]             */
            /*        [  \]             */
            /* x,y+h D[===]C x+w,y+h    */

            w = (float)xx + ((float)f->w*scale);
            h = (float)y  + ((float)f->h*scale);

            vtx[v+0].pos[0]  = xx;     vtx[v+0].pos[1]  = y;      /* A */
            vtx[v+1].pos[0]  = w;      vtx[v+1].pos[1]  = y;      /* B */
            vtx[v+2].pos[0]  = w;      vtx[v+2].pos[1]  = h;      /* C */
            vtx[v+3].pos[0]  = xx;     vtx[v+3].pos[1]  = h;      /* D */

            /* 0,1  A[===]B 1,1 */
            /*       [ \ ]      */
            /*       [  \]      */
            /* 0,0  D[===]C 1,0 */

            vtx[v+0].tex[0]  = tx;     vtx[v+0].tex[1]  = ty;     /* D */
            vtx[v+1].tex[0]  = tx+tw;  vtx[v+1].tex[1]  = ty;     /* C */
            vtx[v+2].tex[0]  = tx+tw;  vtx[v+2].tex[1]  = ty+th;  /* B */
            vtx[v+3].tex[0]  = tx;     vtx[v+3].tex[1]  = ty+th;  /* A */

            idx[d+0] = v+0;     /* A */
            idx[d+1] = v+2;     /* C */
            idx[d+2] = v+1;     /* B */
            idx[d+3] = v+0;     /* A */
            idx[d+4] = v+3;     /* D */
            idx[d+5] = v+2;     /* C */

            v += VERTICES;
            d += INDICES;

            xx += (float)f->w*scale;
        }

        OGL_DrawTextures( GL_TRIANGLES, f->texture.id, size*INDICES, &vtx[0], &idx[0] );

        free( vtx );
        free( idx );
    }
}

void OGL_DrawTextures( GLenum mode, GLuint id, GLsizei count, vertex_t *vtx, GLushort *idx )
{
#if defined(USE_GL2)
    glEnableVertexAttribArray( shader_pos );
    glEnableVertexAttribArray( shader_tex );

    glVertexAttribPointer( shader_pos, 2, GL_SHORT, GL_FALSE, STRIDE_VTX, vtx->pos );
    glVertexAttribPointer( shader_tex, 2, GL_FLOAT, GL_FALSE, STRIDE_VTX, vtx->tex );
    glUniformMatrix4fv( shader_mvp, 1, GL_FALSE, matMVP );
#else /* GL1 */
    glEnableClientState( GL_VERTEX_ARRAY );
    glEnableClientState( GL_TEXTURE_COORD_ARRAY );

    glVertexPointer( 2, GL_SHORT, STRIDE_VTX, vtx->pos );
    glTexCoordPointer( 2, GL_FLOAT, STRIDE_VTX, vtx->tex );
#endif
    glBindTexture( GL_TEXTURE_2D, id );

    if (idx == NULL) {
        glDrawArrays( mode, 0, count );
    } else {
        glDrawElements( mode, count, GL_UNSIGNED_SHORT, idx );
    }

#if defined(USE_GL2)
    glDisableVertexAttribArray( shader_tex );
    glDisableVertexAttribArray( shader_pos );
#else /* GL1 */
    glDisableClientState( GL_TEXTURE_COORD_ARRAY );
    glDisableClientState( GL_VERTEX_ARRAY );
#endif
}

#if defined(USE_FBO)

boolean OGL_OpenFBO( uint16_t w_in, uint16_t h_in, uint16_t w_out, uint16_t h_out )
{
    GLenum status;

    fbo_enabled = False;

#if defined(WIN32)
    if (glBindFramebuffer && glDeleteFramebuffers && glGenFramebuffers && glCheckFramebufferStatus && glFramebufferTexture2D &&
        glGenRenderbuffers && glBindRenderbuffer && glRenderbufferStorage && glDeleteRenderbuffers)
    {
        printf( "[OPENGL] FBO supported\n" );
    }
    else
    {
        printf( "[OPENGL] FBO is NOT supported\n" );
        return fbo_enabled;
    }
#endif

    fbo.texture.w = w_in;
    fbo.texture.h = h_in;

    /* Select the best 8:5 resolution */
    render_rect.w = h_out*(8.0/5.0);
    render_rect.h = w_out/(8.0/5.0);
    if (render_rect.w>render_rect.w)
    {
        render_rect.w = w_out;
    }
    else
    {
        render_rect.h = h_out;
    }
    render_rect.x = MAX(0, w_out - render_rect.w)/2;
    render_rect.y = MAX(0, h_out - render_rect.h)/2;

    /* Create a texture to be the color buffer */
    glGenTextures( 1, &fbo.texture.id );
    glBindTexture( GL_TEXTURE_2D, fbo.texture.id );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, config.filtermode );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, config.filtermode );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, fbo.texture.w, fbo.texture.h, 0, GL_RGB, GL_UNSIGNED_SHORT_5_6_5, NULL );
    /* Create the framebuffer reference */
    glGenFramebuffers( 1, &fbo.framebuffer );
    glBindFramebuffer( GL_FRAMEBUFFER, fbo.framebuffer );
    /* Attach the texture to the framebuffer */
    glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fbo.texture.id, 0);

    /* Verify the framebuffer is valid */
    status = glCheckFramebufferStatus( GL_FRAMEBUFFER );
    switch(status)
    {
        case GL_FRAMEBUFFER_COMPLETE:
            fbo_enabled = True;
            printf( "[OPENGL] GL_FRAMEBUFFER_COMPLETE: created successfully resolution: %dx%d\n", fbo.texture.w, fbo.texture.h );
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
            printf( "[OPENGL] GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT: Not all framebuffer attachment points are framebuffer attachment complete.\n" );
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS:
            printf( "[OPENGL] GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS: Not all attached images have the same width and height.\n" );
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
            printf( "[OPENGL] GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT: No images are attached to the framebuffer.\n" );
            break;
        case GL_FRAMEBUFFER_UNSUPPORTED:
            printf( "[OPENGL] GL_FRAMEBUFFER_UNSUPPORTED: The combination of internal formats of the attached images violates an implementation-dependent set of restrictions.\n" );
            break;
        default:
            printf( "[OPENGL] FBO failed\n" );
            break;
    }

    OGL_CheckErrors( __FILE__, __LINE__ );

    OrthoMatrix( &matRender[0], 0, fbo.texture.w, fbo.texture.h, 0, -1, 1 );

    glBindFramebuffer( GL_FRAMEBUFFER, 0 );

    return fbo_enabled;
}

void OGL_CloseFBO( void )
{
    if (fbo_enabled == True)
    {
        if (fbo.texture.id != 0) {
            glDeleteTextures( 1, &fbo.texture.id );
            fbo.texture.id = 0;
        }

        glBindFramebuffer( GL_FRAMEBUFFER, 0 );
        if (fbo.framebuffer != 0) {
            glDeleteFramebuffers( 1, &fbo.framebuffer );
            fbo.framebuffer = 0;
        }
    }
}

void OGL_SelectBuffer( boolean active )
{
    if (fbo_enabled == True && active == True)
    {
        glBindFramebuffer( GL_FRAMEBUFFER, fbo.framebuffer );
        glViewport( 0, 0, fbo.texture.w, fbo.texture.h );

#if defined(USE_GL2)
        matMVP = &matRender[0];
#else
        glMatrixMode( GL_PROJECTION );
        glLoadMatrixf( &matRender[0] );
#endif
    }
    else
    {
        glBindFramebuffer( GL_FRAMEBUFFER, 0 );
        glViewport( screen_x, screen_y, screen_w, screen_h );

#if defined(USE_GL2)
        matMVP = &matScreen[0];
#else
        glMatrixMode( GL_PROJECTION );
        glLoadMatrixf( &matScreen[0] );
#endif
    }

#if defined(DEBUG)
    OGL_CheckErrors( __FILE__, __LINE__ );
#endif
}

void OGL_RenderBuffer( void )
{
    if (fbo_enabled == True)
    {
        OGL_DrawImage( &fbo.texture, True, &render_rect, NULL );
    }
}

#endif /* USE_FBO */


#if defined(WIN32)
#define LOAD_OPENGL_PROC(T,N)                                       \
{                                                                   \
    N = (T)wglGetProcAddress(#N);                                   \
    if (N == NULL) {                                                \
        printf( "ERROR Video: OpenGL function is NULL:" #N "\n" );  \
    }                                                               \
}

uint8_t LoadGLFunctions( void )
{
#if defined(USE_GL2)
    LOAD_OPENGL_PROC( PFNGLDELETESHADERPROC,                glDeleteShader );
    LOAD_OPENGL_PROC( PFNGLDELETEPROGRAMPROC,               glDeleteProgram );
    LOAD_OPENGL_PROC( PFNGLDETACHSHADERPROC,                glDetachShader );
    LOAD_OPENGL_PROC( PFNGLUSEPROGRAMPROC,                  glUseProgram );
    LOAD_OPENGL_PROC( PFNGLCREATESHADERPROC,                glCreateShader );
    LOAD_OPENGL_PROC( PFNGLSHADERSOURCEPROC,                glShaderSource );
    LOAD_OPENGL_PROC( PFNGLCOMPILESHADERPROC,               glCompileShader );
    LOAD_OPENGL_PROC( PFNGLGETSHADERIVPROC,                 glGetShaderiv );
    LOAD_OPENGL_PROC( PFNGLCREATEPROGRAMPROC,               glCreateProgram );
    LOAD_OPENGL_PROC( PFNGLATTACHSHADERPROC,                glAttachShader );
    LOAD_OPENGL_PROC( PFNGLLINKPROGRAMPROC,                 glLinkProgram );
    LOAD_OPENGL_PROC( PFNGLGETPROGRAMIVPROC,                glGetProgramiv );
    LOAD_OPENGL_PROC( PFNGLGETACTIVEATTRIBPROC,             glGetActiveAttrib );
    LOAD_OPENGL_PROC( PFNGLGETATTRIBLOCATIONPROC,           glGetAttribLocation );
    LOAD_OPENGL_PROC( PFNGLGETACTIVEUNIFORMPROC,            glGetActiveUniform );
    LOAD_OPENGL_PROC( PFNGLGETUNIFORMLOCATIONPROC,          glGetUniformLocation );
    LOAD_OPENGL_PROC( PFNGLGETSHADERINFOLOGPROC,            glGetShaderInfoLog );
    LOAD_OPENGL_PROC( PFNGLGETPROGRAMINFOLOGPROC,           glGetProgramInfoLog );
    LOAD_OPENGL_PROC( PFNGLDISABLEVERTEXATTRIBARRAYPROC,    glDisableVertexAttribArray );
    LOAD_OPENGL_PROC( PFNGLENABLEVERTEXATTRIBARRAYPROC,     glEnableVertexAttribArray );
    LOAD_OPENGL_PROC( PFNGLVERTEXATTRIBPOINTERPROC,         glVertexAttribPointer );
    LOAD_OPENGL_PROC( PFNGLUNIFORM1IPROC,                   glUniform1i );
    LOAD_OPENGL_PROC( PFNGLUNIFORM1FVPROC,                  glUniform1fv );
    LOAD_OPENGL_PROC( PFNGLUNIFORM3FVPROC,                  glUniform3fv );
    LOAD_OPENGL_PROC( PFNGLUNIFORM4FVPROC,                  glUniform4fv );
    LOAD_OPENGL_PROC( PFNGLUNIFORMMATRIX3FVPROC,            glUniformMatrix3fv );
    LOAD_OPENGL_PROC( PFNGLUNIFORMMATRIX4FVPROC,            glUniformMatrix4fv );
#endif
    /* FBO API */
#if defined(USE_FBO)
    LOAD_OPENGL_PROC( PFNGLBINDFRAMEBUFFEREXTPROC,          glBindFramebuffer );
    LOAD_OPENGL_PROC( PFNGLDELETEFRAMEBUFFERSEXTPROC,       glDeleteFramebuffers );
    LOAD_OPENGL_PROC( PFNGLGENFRAMEBUFFERSEXTPROC,          glGenFramebuffers );
    LOAD_OPENGL_PROC( PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC,   glCheckFramebufferStatus );
    LOAD_OPENGL_PROC( PFNGLFRAMEBUFFERTEXTURE2DEXTPROC,     glFramebufferTexture2D );
    LOAD_OPENGL_PROC( PFNGLGENRENDERBUFFERSEXTPROC,         glGenRenderbuffers );
    LOAD_OPENGL_PROC( PFNGLBINDRENDERBUFFEREXTPROC,         glBindRenderbuffer );
    LOAD_OPENGL_PROC( PFNGLRENDERBUFFERSTORAGEEXTPROC,      glRenderbufferStorage );
    LOAD_OPENGL_PROC( PFNGLDELETERENDERBUFFERSEXTPROC,      glDeleteRenderbuffers );
#endif /* defined(USE_FBO) */

#if defined(USE_GL1) || defined(USE_GL2)
    LOAD_OPENGL_PROC( PFNGLCOMPRESSEDTEXIMAGE2DARBPROC,     glCompressedTexImage2D );
    LOAD_OPENGL_PROC( PFNGLGETCOMPRESSEDTEXIMAGEARBPROC,    glGetCompressedTexImageARB );
#endif

    return 0;
}

#endif /* defined(WIN32) */
