/**
 * SPDX-License-Identifier: (WTFPL OR CC0-1.0) AND Apache-2.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/glad/gl.h"

#include "../../../../lv_conf_internal.h"

#if LV_USE_GLFW

#ifndef GLAD_IMPL_UTIL_C_
    #define GLAD_IMPL_UTIL_C_

    #ifdef _MSC_VER
        #define GLAD_IMPL_UTIL_SSCANF sscanf_s
    #else
        #define GLAD_IMPL_UTIL_SSCANF sscanf
    #endif

#endif /* GLAD_IMPL_UTIL_C_ */

#ifdef __cplusplus
extern "C" {
#endif



int GLAD_GL_VERSION_1_0 = 0;
int GLAD_GL_VERSION_1_1 = 0;
int GLAD_GL_VERSION_1_2 = 0;
int GLAD_GL_VERSION_1_3 = 0;
int GLAD_GL_VERSION_1_4 = 0;
int GLAD_GL_VERSION_1_5 = 0;
int GLAD_GL_VERSION_2_0 = 0;
int GLAD_GL_VERSION_2_1 = 0;
int GLAD_GL_VERSION_3_0 = 0;

int GLAD_GL_EXT_framebuffer_object = 0;



PFNGLACCUMPROC glad_glAccum = NULL;
PFNGLACTIVETEXTUREPROC glad_glActiveTexture = NULL;
PFNGLALPHAFUNCPROC glad_glAlphaFunc = NULL;
PFNGLARETEXTURESRESIDENTPROC glad_glAreTexturesResident = NULL;
PFNGLARRAYELEMENTPROC glad_glArrayElement = NULL;
PFNGLATTACHSHADERPROC glad_glAttachShader = NULL;
PFNGLBEGINPROC glad_glBegin = NULL;
PFNGLBEGINQUERYPROC glad_glBeginQuery = NULL;
PFNGLBINDATTRIBLOCATIONPROC glad_glBindAttribLocation = NULL;
PFNGLBINDBUFFERPROC glad_glBindBuffer = NULL;
PFNGLBINDFRAMEBUFFEREXTPROC glad_glBindFramebufferEXT = NULL;
PFNGLBINDRENDERBUFFEREXTPROC glad_glBindRenderbufferEXT = NULL;
PFNGLBINDTEXTUREPROC glad_glBindTexture = NULL;
PFNGLBITMAPPROC glad_glBitmap = NULL;
PFNGLBLENDCOLORPROC glad_glBlendColor = NULL;
PFNGLBLENDEQUATIONPROC glad_glBlendEquation = NULL;
PFNGLBLENDEQUATIONSEPARATEPROC glad_glBlendEquationSeparate = NULL;
PFNGLBLENDFUNCPROC glad_glBlendFunc = NULL;
PFNGLBLENDFUNCSEPARATEPROC glad_glBlendFuncSeparate = NULL;
PFNGLBUFFERDATAPROC glad_glBufferData = NULL;
PFNGLBUFFERSUBDATAPROC glad_glBufferSubData = NULL;
PFNGLCALLLISTPROC glad_glCallList = NULL;
PFNGLCALLLISTSPROC glad_glCallLists = NULL;
PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC glad_glCheckFramebufferStatusEXT = NULL;
PFNGLCLEARPROC glad_glClear = NULL;
PFNGLCLEARACCUMPROC glad_glClearAccum = NULL;
PFNGLCLEARCOLORPROC glad_glClearColor = NULL;
PFNGLCLEARDEPTHPROC glad_glClearDepth = NULL;
PFNGLCLEARINDEXPROC glad_glClearIndex = NULL;
PFNGLCLEARSTENCILPROC glad_glClearStencil = NULL;
PFNGLCLIENTACTIVETEXTUREPROC glad_glClientActiveTexture = NULL;
PFNGLCLIPPLANEPROC glad_glClipPlane = NULL;
PFNGLCOLOR3BPROC glad_glColor3b = NULL;
PFNGLCOLOR3BVPROC glad_glColor3bv = NULL;
PFNGLCOLOR3DPROC glad_glColor3d = NULL;
PFNGLCOLOR3DVPROC glad_glColor3dv = NULL;
PFNGLCOLOR3FPROC glad_glColor3f = NULL;
PFNGLCOLOR3FVPROC glad_glColor3fv = NULL;
PFNGLCOLOR3IPROC glad_glColor3i = NULL;
PFNGLCOLOR3IVPROC glad_glColor3iv = NULL;
PFNGLCOLOR3SPROC glad_glColor3s = NULL;
PFNGLCOLOR3SVPROC glad_glColor3sv = NULL;
PFNGLCOLOR3UBPROC glad_glColor3ub = NULL;
PFNGLCOLOR3UBVPROC glad_glColor3ubv = NULL;
PFNGLCOLOR3UIPROC glad_glColor3ui = NULL;
PFNGLCOLOR3UIVPROC glad_glColor3uiv = NULL;
PFNGLCOLOR3USPROC glad_glColor3us = NULL;
PFNGLCOLOR3USVPROC glad_glColor3usv = NULL;
PFNGLCOLOR4BPROC glad_glColor4b = NULL;
PFNGLCOLOR4BVPROC glad_glColor4bv = NULL;
PFNGLCOLOR4DPROC glad_glColor4d = NULL;
PFNGLCOLOR4DVPROC glad_glColor4dv = NULL;
PFNGLCOLOR4FPROC glad_glColor4f = NULL;
PFNGLCOLOR4FVPROC glad_glColor4fv = NULL;
PFNGLCOLOR4IPROC glad_glColor4i = NULL;
PFNGLCOLOR4IVPROC glad_glColor4iv = NULL;
PFNGLCOLOR4SPROC glad_glColor4s = NULL;
PFNGLCOLOR4SVPROC glad_glColor4sv = NULL;
PFNGLCOLOR4UBPROC glad_glColor4ub = NULL;
PFNGLCOLOR4UBVPROC glad_glColor4ubv = NULL;
PFNGLCOLOR4UIPROC glad_glColor4ui = NULL;
PFNGLCOLOR4UIVPROC glad_glColor4uiv = NULL;
PFNGLCOLOR4USPROC glad_glColor4us = NULL;
PFNGLCOLOR4USVPROC glad_glColor4usv = NULL;
PFNGLCOLORMASKPROC glad_glColorMask = NULL;
PFNGLCOLORMATERIALPROC glad_glColorMaterial = NULL;
PFNGLCOLORPOINTERPROC glad_glColorPointer = NULL;
PFNGLCOMPILESHADERPROC glad_glCompileShader = NULL;
PFNGLCOMPRESSEDTEXIMAGE1DPROC glad_glCompressedTexImage1D = NULL;
PFNGLCOMPRESSEDTEXIMAGE2DPROC glad_glCompressedTexImage2D = NULL;
PFNGLCOMPRESSEDTEXIMAGE3DPROC glad_glCompressedTexImage3D = NULL;
PFNGLCOMPRESSEDTEXSUBIMAGE1DPROC glad_glCompressedTexSubImage1D = NULL;
PFNGLCOMPRESSEDTEXSUBIMAGE2DPROC glad_glCompressedTexSubImage2D = NULL;
PFNGLCOMPRESSEDTEXSUBIMAGE3DPROC glad_glCompressedTexSubImage3D = NULL;
PFNGLCOPYPIXELSPROC glad_glCopyPixels = NULL;
PFNGLCOPYTEXIMAGE1DPROC glad_glCopyTexImage1D = NULL;
PFNGLCOPYTEXIMAGE2DPROC glad_glCopyTexImage2D = NULL;
PFNGLCOPYTEXSUBIMAGE1DPROC glad_glCopyTexSubImage1D = NULL;
PFNGLCOPYTEXSUBIMAGE2DPROC glad_glCopyTexSubImage2D = NULL;
PFNGLCOPYTEXSUBIMAGE3DPROC glad_glCopyTexSubImage3D = NULL;
PFNGLCREATEPROGRAMPROC glad_glCreateProgram = NULL;
PFNGLCREATESHADERPROC glad_glCreateShader = NULL;
PFNGLCULLFACEPROC glad_glCullFace = NULL;
PFNGLDELETEBUFFERSPROC glad_glDeleteBuffers = NULL;
PFNGLDELETEFRAMEBUFFERSEXTPROC glad_glDeleteFramebuffersEXT = NULL;
PFNGLDELETELISTSPROC glad_glDeleteLists = NULL;
PFNGLDELETEPROGRAMPROC glad_glDeleteProgram = NULL;
PFNGLDELETEQUERIESPROC glad_glDeleteQueries = NULL;
PFNGLDELETERENDERBUFFERSEXTPROC glad_glDeleteRenderbuffersEXT = NULL;
PFNGLDELETESHADERPROC glad_glDeleteShader = NULL;
PFNGLDELETETEXTURESPROC glad_glDeleteTextures = NULL;
PFNGLDEPTHFUNCPROC glad_glDepthFunc = NULL;
PFNGLDEPTHMASKPROC glad_glDepthMask = NULL;
PFNGLDEPTHRANGEPROC glad_glDepthRange = NULL;
PFNGLDETACHSHADERPROC glad_glDetachShader = NULL;
PFNGLDISABLEPROC glad_glDisable = NULL;
PFNGLDISABLECLIENTSTATEPROC glad_glDisableClientState = NULL;
PFNGLDISABLEVERTEXATTRIBARRAYPROC glad_glDisableVertexAttribArray = NULL;
PFNGLDRAWARRAYSPROC glad_glDrawArrays = NULL;
PFNGLDRAWBUFFERPROC glad_glDrawBuffer = NULL;
PFNGLDRAWBUFFERSPROC glad_glDrawBuffers = NULL;
PFNGLDRAWELEMENTSPROC glad_glDrawElements = NULL;
PFNGLDRAWPIXELSPROC glad_glDrawPixels = NULL;
PFNGLDRAWRANGEELEMENTSPROC glad_glDrawRangeElements = NULL;
PFNGLEDGEFLAGPROC glad_glEdgeFlag = NULL;
PFNGLEDGEFLAGPOINTERPROC glad_glEdgeFlagPointer = NULL;
PFNGLEDGEFLAGVPROC glad_glEdgeFlagv = NULL;
PFNGLENABLEPROC glad_glEnable = NULL;
PFNGLENABLECLIENTSTATEPROC glad_glEnableClientState = NULL;
PFNGLENABLEVERTEXATTRIBARRAYPROC glad_glEnableVertexAttribArray = NULL;
PFNGLENDPROC glad_glEnd = NULL;
PFNGLENDLISTPROC glad_glEndList = NULL;
PFNGLENDQUERYPROC glad_glEndQuery = NULL;
PFNGLEVALCOORD1DPROC glad_glEvalCoord1d = NULL;
PFNGLEVALCOORD1DVPROC glad_glEvalCoord1dv = NULL;
PFNGLEVALCOORD1FPROC glad_glEvalCoord1f = NULL;
PFNGLEVALCOORD1FVPROC glad_glEvalCoord1fv = NULL;
PFNGLEVALCOORD2DPROC glad_glEvalCoord2d = NULL;
PFNGLEVALCOORD2DVPROC glad_glEvalCoord2dv = NULL;
PFNGLEVALCOORD2FPROC glad_glEvalCoord2f = NULL;
PFNGLEVALCOORD2FVPROC glad_glEvalCoord2fv = NULL;
PFNGLEVALMESH1PROC glad_glEvalMesh1 = NULL;
PFNGLEVALMESH2PROC glad_glEvalMesh2 = NULL;
PFNGLEVALPOINT1PROC glad_glEvalPoint1 = NULL;
PFNGLEVALPOINT2PROC glad_glEvalPoint2 = NULL;
PFNGLFEEDBACKBUFFERPROC glad_glFeedbackBuffer = NULL;
PFNGLFINISHPROC glad_glFinish = NULL;
PFNGLFLUSHPROC glad_glFlush = NULL;
PFNGLFOGCOORDPOINTERPROC glad_glFogCoordPointer = NULL;
PFNGLFOGCOORDDPROC glad_glFogCoordd = NULL;
PFNGLFOGCOORDDVPROC glad_glFogCoorddv = NULL;
PFNGLFOGCOORDFPROC glad_glFogCoordf = NULL;
PFNGLFOGCOORDFVPROC glad_glFogCoordfv = NULL;
PFNGLFOGFPROC glad_glFogf = NULL;
PFNGLFOGFVPROC glad_glFogfv = NULL;
PFNGLFOGIPROC glad_glFogi = NULL;
PFNGLFOGIVPROC glad_glFogiv = NULL;
PFNGLFRAMEBUFFERRENDERBUFFEREXTPROC glad_glFramebufferRenderbufferEXT = NULL;
PFNGLFRAMEBUFFERTEXTURE1DEXTPROC glad_glFramebufferTexture1DEXT = NULL;
PFNGLFRAMEBUFFERTEXTURE2DEXTPROC glad_glFramebufferTexture2DEXT = NULL;
PFNGLFRAMEBUFFERTEXTURE3DEXTPROC glad_glFramebufferTexture3DEXT = NULL;
PFNGLFRONTFACEPROC glad_glFrontFace = NULL;
PFNGLFRUSTUMPROC glad_glFrustum = NULL;
PFNGLGENBUFFERSPROC glad_glGenBuffers = NULL;
PFNGLGENFRAMEBUFFERSEXTPROC glad_glGenFramebuffersEXT = NULL;
PFNGLGENLISTSPROC glad_glGenLists = NULL;
PFNGLGENQUERIESPROC glad_glGenQueries = NULL;
PFNGLGENRENDERBUFFERSEXTPROC glad_glGenRenderbuffersEXT = NULL;
PFNGLGENTEXTURESPROC glad_glGenTextures = NULL;
PFNGLGENERATEMIPMAPEXTPROC glad_glGenerateMipmapEXT = NULL;
PFNGLGETACTIVEATTRIBPROC glad_glGetActiveAttrib = NULL;
PFNGLGETACTIVEUNIFORMPROC glad_glGetActiveUniform = NULL;
PFNGLGETATTACHEDSHADERSPROC glad_glGetAttachedShaders = NULL;
PFNGLGETATTRIBLOCATIONPROC glad_glGetAttribLocation = NULL;
PFNGLGETBOOLEANVPROC glad_glGetBooleanv = NULL;
PFNGLGETBUFFERPARAMETERIVPROC glad_glGetBufferParameteriv = NULL;
PFNGLGETBUFFERPOINTERVPROC glad_glGetBufferPointerv = NULL;
PFNGLGETBUFFERSUBDATAPROC glad_glGetBufferSubData = NULL;
PFNGLGETCLIPPLANEPROC glad_glGetClipPlane = NULL;
PFNGLGETCOMPRESSEDTEXIMAGEPROC glad_glGetCompressedTexImage = NULL;
PFNGLGETDOUBLEVPROC glad_glGetDoublev = NULL;
PFNGLGETERRORPROC glad_glGetError = NULL;
PFNGLGETFLOATVPROC glad_glGetFloatv = NULL;
PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVEXTPROC glad_glGetFramebufferAttachmentParameterivEXT = NULL;
PFNGLGETINTEGERVPROC glad_glGetIntegerv = NULL;
PFNGLGETLIGHTFVPROC glad_glGetLightfv = NULL;
PFNGLGETLIGHTIVPROC glad_glGetLightiv = NULL;
PFNGLGETMAPDVPROC glad_glGetMapdv = NULL;
PFNGLGETMAPFVPROC glad_glGetMapfv = NULL;
PFNGLGETMAPIVPROC glad_glGetMapiv = NULL;
PFNGLGETMATERIALFVPROC glad_glGetMaterialfv = NULL;
PFNGLGETMATERIALIVPROC glad_glGetMaterialiv = NULL;
PFNGLGETPIXELMAPFVPROC glad_glGetPixelMapfv = NULL;
PFNGLGETPIXELMAPUIVPROC glad_glGetPixelMapuiv = NULL;
PFNGLGETPIXELMAPUSVPROC glad_glGetPixelMapusv = NULL;
PFNGLGETPOINTERVPROC glad_glGetPointerv = NULL;
PFNGLGETPOLYGONSTIPPLEPROC glad_glGetPolygonStipple = NULL;
PFNGLGETPROGRAMINFOLOGPROC glad_glGetProgramInfoLog = NULL;
PFNGLGETPROGRAMIVPROC glad_glGetProgramiv = NULL;
PFNGLGETQUERYOBJECTIVPROC glad_glGetQueryObjectiv = NULL;
PFNGLGETQUERYOBJECTUIVPROC glad_glGetQueryObjectuiv = NULL;
PFNGLGETQUERYIVPROC glad_glGetQueryiv = NULL;
PFNGLGETRENDERBUFFERPARAMETERIVEXTPROC glad_glGetRenderbufferParameterivEXT = NULL;
PFNGLGETSHADERINFOLOGPROC glad_glGetShaderInfoLog = NULL;
PFNGLGETSHADERSOURCEPROC glad_glGetShaderSource = NULL;
PFNGLGETSHADERIVPROC glad_glGetShaderiv = NULL;
PFNGLGETSTRINGPROC glad_glGetString = NULL;
PFNGLGETTEXENVFVPROC glad_glGetTexEnvfv = NULL;
PFNGLGETTEXENVIVPROC glad_glGetTexEnviv = NULL;
PFNGLGETTEXGENDVPROC glad_glGetTexGendv = NULL;
PFNGLGETTEXGENFVPROC glad_glGetTexGenfv = NULL;
PFNGLGETTEXGENIVPROC glad_glGetTexGeniv = NULL;
PFNGLGETTEXIMAGEPROC glad_glGetTexImage = NULL;
PFNGLGETTEXLEVELPARAMETERFVPROC glad_glGetTexLevelParameterfv = NULL;
PFNGLGETTEXLEVELPARAMETERIVPROC glad_glGetTexLevelParameteriv = NULL;
PFNGLGETTEXPARAMETERFVPROC glad_glGetTexParameterfv = NULL;
PFNGLGETTEXPARAMETERIVPROC glad_glGetTexParameteriv = NULL;
PFNGLGETUNIFORMLOCATIONPROC glad_glGetUniformLocation = NULL;
PFNGLGETUNIFORMFVPROC glad_glGetUniformfv = NULL;
PFNGLGETUNIFORMIVPROC glad_glGetUniformiv = NULL;
PFNGLGETVERTEXATTRIBPOINTERVPROC glad_glGetVertexAttribPointerv = NULL;
PFNGLGETVERTEXATTRIBDVPROC glad_glGetVertexAttribdv = NULL;
PFNGLGETVERTEXATTRIBFVPROC glad_glGetVertexAttribfv = NULL;
PFNGLGETVERTEXATTRIBIVPROC glad_glGetVertexAttribiv = NULL;
PFNGLHINTPROC glad_glHint = NULL;
PFNGLINDEXMASKPROC glad_glIndexMask = NULL;
PFNGLINDEXPOINTERPROC glad_glIndexPointer = NULL;
PFNGLINDEXDPROC glad_glIndexd = NULL;
PFNGLINDEXDVPROC glad_glIndexdv = NULL;
PFNGLINDEXFPROC glad_glIndexf = NULL;
PFNGLINDEXFVPROC glad_glIndexfv = NULL;
PFNGLINDEXIPROC glad_glIndexi = NULL;
PFNGLINDEXIVPROC glad_glIndexiv = NULL;
PFNGLINDEXSPROC glad_glIndexs = NULL;
PFNGLINDEXSVPROC glad_glIndexsv = NULL;
PFNGLINDEXUBPROC glad_glIndexub = NULL;
PFNGLINDEXUBVPROC glad_glIndexubv = NULL;
PFNGLINITNAMESPROC glad_glInitNames = NULL;
PFNGLINTERLEAVEDARRAYSPROC glad_glInterleavedArrays = NULL;
PFNGLISBUFFERPROC glad_glIsBuffer = NULL;
PFNGLISENABLEDPROC glad_glIsEnabled = NULL;
PFNGLISFRAMEBUFFEREXTPROC glad_glIsFramebufferEXT = NULL;
PFNGLISLISTPROC glad_glIsList = NULL;
PFNGLISPROGRAMPROC glad_glIsProgram = NULL;
PFNGLISQUERYPROC glad_glIsQuery = NULL;
PFNGLISRENDERBUFFEREXTPROC glad_glIsRenderbufferEXT = NULL;
PFNGLISSHADERPROC glad_glIsShader = NULL;
PFNGLISTEXTUREPROC glad_glIsTexture = NULL;
PFNGLLIGHTMODELFPROC glad_glLightModelf = NULL;
PFNGLLIGHTMODELFVPROC glad_glLightModelfv = NULL;
PFNGLLIGHTMODELIPROC glad_glLightModeli = NULL;
PFNGLLIGHTMODELIVPROC glad_glLightModeliv = NULL;
PFNGLLIGHTFPROC glad_glLightf = NULL;
PFNGLLIGHTFVPROC glad_glLightfv = NULL;
PFNGLLIGHTIPROC glad_glLighti = NULL;
PFNGLLIGHTIVPROC glad_glLightiv = NULL;
PFNGLLINESTIPPLEPROC glad_glLineStipple = NULL;
PFNGLLINEWIDTHPROC glad_glLineWidth = NULL;
PFNGLLINKPROGRAMPROC glad_glLinkProgram = NULL;
PFNGLLISTBASEPROC glad_glListBase = NULL;
PFNGLLOADIDENTITYPROC glad_glLoadIdentity = NULL;
PFNGLLOADMATRIXDPROC glad_glLoadMatrixd = NULL;
PFNGLLOADMATRIXFPROC glad_glLoadMatrixf = NULL;
PFNGLLOADNAMEPROC glad_glLoadName = NULL;
PFNGLLOADTRANSPOSEMATRIXDPROC glad_glLoadTransposeMatrixd = NULL;
PFNGLLOADTRANSPOSEMATRIXFPROC glad_glLoadTransposeMatrixf = NULL;
PFNGLLOGICOPPROC glad_glLogicOp = NULL;
PFNGLMAP1DPROC glad_glMap1d = NULL;
PFNGLMAP1FPROC glad_glMap1f = NULL;
PFNGLMAP2DPROC glad_glMap2d = NULL;
PFNGLMAP2FPROC glad_glMap2f = NULL;
PFNGLMAPBUFFERPROC glad_glMapBuffer = NULL;
PFNGLMAPGRID1DPROC glad_glMapGrid1d = NULL;
PFNGLMAPGRID1FPROC glad_glMapGrid1f = NULL;
PFNGLMAPGRID2DPROC glad_glMapGrid2d = NULL;
PFNGLMAPGRID2FPROC glad_glMapGrid2f = NULL;
PFNGLMATERIALFPROC glad_glMaterialf = NULL;
PFNGLMATERIALFVPROC glad_glMaterialfv = NULL;
PFNGLMATERIALIPROC glad_glMateriali = NULL;
PFNGLMATERIALIVPROC glad_glMaterialiv = NULL;
PFNGLMATRIXMODEPROC glad_glMatrixMode = NULL;
PFNGLMULTMATRIXDPROC glad_glMultMatrixd = NULL;
PFNGLMULTMATRIXFPROC glad_glMultMatrixf = NULL;
PFNGLMULTTRANSPOSEMATRIXDPROC glad_glMultTransposeMatrixd = NULL;
PFNGLMULTTRANSPOSEMATRIXFPROC glad_glMultTransposeMatrixf = NULL;
PFNGLMULTIDRAWARRAYSPROC glad_glMultiDrawArrays = NULL;
PFNGLMULTIDRAWELEMENTSPROC glad_glMultiDrawElements = NULL;
PFNGLMULTITEXCOORD1DPROC glad_glMultiTexCoord1d = NULL;
PFNGLMULTITEXCOORD1DVPROC glad_glMultiTexCoord1dv = NULL;
PFNGLMULTITEXCOORD1FPROC glad_glMultiTexCoord1f = NULL;
PFNGLMULTITEXCOORD1FVPROC glad_glMultiTexCoord1fv = NULL;
PFNGLMULTITEXCOORD1IPROC glad_glMultiTexCoord1i = NULL;
PFNGLMULTITEXCOORD1IVPROC glad_glMultiTexCoord1iv = NULL;
PFNGLMULTITEXCOORD1SPROC glad_glMultiTexCoord1s = NULL;
PFNGLMULTITEXCOORD1SVPROC glad_glMultiTexCoord1sv = NULL;
PFNGLMULTITEXCOORD2DPROC glad_glMultiTexCoord2d = NULL;
PFNGLMULTITEXCOORD2DVPROC glad_glMultiTexCoord2dv = NULL;
PFNGLMULTITEXCOORD2FPROC glad_glMultiTexCoord2f = NULL;
PFNGLMULTITEXCOORD2FVPROC glad_glMultiTexCoord2fv = NULL;
PFNGLMULTITEXCOORD2IPROC glad_glMultiTexCoord2i = NULL;
PFNGLMULTITEXCOORD2IVPROC glad_glMultiTexCoord2iv = NULL;
PFNGLMULTITEXCOORD2SPROC glad_glMultiTexCoord2s = NULL;
PFNGLMULTITEXCOORD2SVPROC glad_glMultiTexCoord2sv = NULL;
PFNGLMULTITEXCOORD3DPROC glad_glMultiTexCoord3d = NULL;
PFNGLMULTITEXCOORD3DVPROC glad_glMultiTexCoord3dv = NULL;
PFNGLMULTITEXCOORD3FPROC glad_glMultiTexCoord3f = NULL;
PFNGLMULTITEXCOORD3FVPROC glad_glMultiTexCoord3fv = NULL;
PFNGLMULTITEXCOORD3IPROC glad_glMultiTexCoord3i = NULL;
PFNGLMULTITEXCOORD3IVPROC glad_glMultiTexCoord3iv = NULL;
PFNGLMULTITEXCOORD3SPROC glad_glMultiTexCoord3s = NULL;
PFNGLMULTITEXCOORD3SVPROC glad_glMultiTexCoord3sv = NULL;
PFNGLMULTITEXCOORD4DPROC glad_glMultiTexCoord4d = NULL;
PFNGLMULTITEXCOORD4DVPROC glad_glMultiTexCoord4dv = NULL;
PFNGLMULTITEXCOORD4FPROC glad_glMultiTexCoord4f = NULL;
PFNGLMULTITEXCOORD4FVPROC glad_glMultiTexCoord4fv = NULL;
PFNGLMULTITEXCOORD4IPROC glad_glMultiTexCoord4i = NULL;
PFNGLMULTITEXCOORD4IVPROC glad_glMultiTexCoord4iv = NULL;
PFNGLMULTITEXCOORD4SPROC glad_glMultiTexCoord4s = NULL;
PFNGLMULTITEXCOORD4SVPROC glad_glMultiTexCoord4sv = NULL;
PFNGLNEWLISTPROC glad_glNewList = NULL;
PFNGLNORMAL3BPROC glad_glNormal3b = NULL;
PFNGLNORMAL3BVPROC glad_glNormal3bv = NULL;
PFNGLNORMAL3DPROC glad_glNormal3d = NULL;
PFNGLNORMAL3DVPROC glad_glNormal3dv = NULL;
PFNGLNORMAL3FPROC glad_glNormal3f = NULL;
PFNGLNORMAL3FVPROC glad_glNormal3fv = NULL;
PFNGLNORMAL3IPROC glad_glNormal3i = NULL;
PFNGLNORMAL3IVPROC glad_glNormal3iv = NULL;
PFNGLNORMAL3SPROC glad_glNormal3s = NULL;
PFNGLNORMAL3SVPROC glad_glNormal3sv = NULL;
PFNGLNORMALPOINTERPROC glad_glNormalPointer = NULL;
PFNGLORTHOPROC glad_glOrtho = NULL;
PFNGLPASSTHROUGHPROC glad_glPassThrough = NULL;
PFNGLPIXELMAPFVPROC glad_glPixelMapfv = NULL;
PFNGLPIXELMAPUIVPROC glad_glPixelMapuiv = NULL;
PFNGLPIXELMAPUSVPROC glad_glPixelMapusv = NULL;
PFNGLPIXELSTOREFPROC glad_glPixelStoref = NULL;
PFNGLPIXELSTOREIPROC glad_glPixelStorei = NULL;
PFNGLPIXELTRANSFERFPROC glad_glPixelTransferf = NULL;
PFNGLPIXELTRANSFERIPROC glad_glPixelTransferi = NULL;
PFNGLPIXELZOOMPROC glad_glPixelZoom = NULL;
PFNGLPOINTPARAMETERFPROC glad_glPointParameterf = NULL;
PFNGLPOINTPARAMETERFVPROC glad_glPointParameterfv = NULL;
PFNGLPOINTPARAMETERIPROC glad_glPointParameteri = NULL;
PFNGLPOINTPARAMETERIVPROC glad_glPointParameteriv = NULL;
PFNGLPOINTSIZEPROC glad_glPointSize = NULL;
PFNGLPOLYGONMODEPROC glad_glPolygonMode = NULL;
PFNGLPOLYGONOFFSETPROC glad_glPolygonOffset = NULL;
PFNGLPOLYGONSTIPPLEPROC glad_glPolygonStipple = NULL;
PFNGLPOPATTRIBPROC glad_glPopAttrib = NULL;
PFNGLPOPCLIENTATTRIBPROC glad_glPopClientAttrib = NULL;
PFNGLPOPMATRIXPROC glad_glPopMatrix = NULL;
PFNGLPOPNAMEPROC glad_glPopName = NULL;
PFNGLPRIORITIZETEXTURESPROC glad_glPrioritizeTextures = NULL;
PFNGLPUSHATTRIBPROC glad_glPushAttrib = NULL;
PFNGLPUSHCLIENTATTRIBPROC glad_glPushClientAttrib = NULL;
PFNGLPUSHMATRIXPROC glad_glPushMatrix = NULL;
PFNGLPUSHNAMEPROC glad_glPushName = NULL;
PFNGLRASTERPOS2DPROC glad_glRasterPos2d = NULL;
PFNGLRASTERPOS2DVPROC glad_glRasterPos2dv = NULL;
PFNGLRASTERPOS2FPROC glad_glRasterPos2f = NULL;
PFNGLRASTERPOS2FVPROC glad_glRasterPos2fv = NULL;
PFNGLRASTERPOS2IPROC glad_glRasterPos2i = NULL;
PFNGLRASTERPOS2IVPROC glad_glRasterPos2iv = NULL;
PFNGLRASTERPOS2SPROC glad_glRasterPos2s = NULL;
PFNGLRASTERPOS2SVPROC glad_glRasterPos2sv = NULL;
PFNGLRASTERPOS3DPROC glad_glRasterPos3d = NULL;
PFNGLRASTERPOS3DVPROC glad_glRasterPos3dv = NULL;
PFNGLRASTERPOS3FPROC glad_glRasterPos3f = NULL;
PFNGLRASTERPOS3FVPROC glad_glRasterPos3fv = NULL;
PFNGLRASTERPOS3IPROC glad_glRasterPos3i = NULL;
PFNGLRASTERPOS3IVPROC glad_glRasterPos3iv = NULL;
PFNGLRASTERPOS3SPROC glad_glRasterPos3s = NULL;
PFNGLRASTERPOS3SVPROC glad_glRasterPos3sv = NULL;
PFNGLRASTERPOS4DPROC glad_glRasterPos4d = NULL;
PFNGLRASTERPOS4DVPROC glad_glRasterPos4dv = NULL;
PFNGLRASTERPOS4FPROC glad_glRasterPos4f = NULL;
PFNGLRASTERPOS4FVPROC glad_glRasterPos4fv = NULL;
PFNGLRASTERPOS4IPROC glad_glRasterPos4i = NULL;
PFNGLRASTERPOS4IVPROC glad_glRasterPos4iv = NULL;
PFNGLRASTERPOS4SPROC glad_glRasterPos4s = NULL;
PFNGLRASTERPOS4SVPROC glad_glRasterPos4sv = NULL;
PFNGLREADBUFFERPROC glad_glReadBuffer = NULL;
PFNGLREADPIXELSPROC glad_glReadPixels = NULL;
PFNGLRECTDPROC glad_glRectd = NULL;
PFNGLRECTDVPROC glad_glRectdv = NULL;
PFNGLRECTFPROC glad_glRectf = NULL;
PFNGLRECTFVPROC glad_glRectfv = NULL;
PFNGLRECTIPROC glad_glRecti = NULL;
PFNGLRECTIVPROC glad_glRectiv = NULL;
PFNGLRECTSPROC glad_glRects = NULL;
PFNGLRECTSVPROC glad_glRectsv = NULL;
PFNGLRENDERMODEPROC glad_glRenderMode = NULL;
PFNGLRENDERBUFFERSTORAGEEXTPROC glad_glRenderbufferStorageEXT = NULL;
PFNGLROTATEDPROC glad_glRotated = NULL;
PFNGLROTATEFPROC glad_glRotatef = NULL;
PFNGLSAMPLECOVERAGEPROC glad_glSampleCoverage = NULL;
PFNGLSCALEDPROC glad_glScaled = NULL;
PFNGLSCALEFPROC glad_glScalef = NULL;
PFNGLSCISSORPROC glad_glScissor = NULL;
PFNGLSECONDARYCOLOR3BPROC glad_glSecondaryColor3b = NULL;
PFNGLSECONDARYCOLOR3BVPROC glad_glSecondaryColor3bv = NULL;
PFNGLSECONDARYCOLOR3DPROC glad_glSecondaryColor3d = NULL;
PFNGLSECONDARYCOLOR3DVPROC glad_glSecondaryColor3dv = NULL;
PFNGLSECONDARYCOLOR3FPROC glad_glSecondaryColor3f = NULL;
PFNGLSECONDARYCOLOR3FVPROC glad_glSecondaryColor3fv = NULL;
PFNGLSECONDARYCOLOR3IPROC glad_glSecondaryColor3i = NULL;
PFNGLSECONDARYCOLOR3IVPROC glad_glSecondaryColor3iv = NULL;
PFNGLSECONDARYCOLOR3SPROC glad_glSecondaryColor3s = NULL;
PFNGLSECONDARYCOLOR3SVPROC glad_glSecondaryColor3sv = NULL;
PFNGLSECONDARYCOLOR3UBPROC glad_glSecondaryColor3ub = NULL;
PFNGLSECONDARYCOLOR3UBVPROC glad_glSecondaryColor3ubv = NULL;
PFNGLSECONDARYCOLOR3UIPROC glad_glSecondaryColor3ui = NULL;
PFNGLSECONDARYCOLOR3UIVPROC glad_glSecondaryColor3uiv = NULL;
PFNGLSECONDARYCOLOR3USPROC glad_glSecondaryColor3us = NULL;
PFNGLSECONDARYCOLOR3USVPROC glad_glSecondaryColor3usv = NULL;
PFNGLSECONDARYCOLORPOINTERPROC glad_glSecondaryColorPointer = NULL;
PFNGLSELECTBUFFERPROC glad_glSelectBuffer = NULL;
PFNGLSHADEMODELPROC glad_glShadeModel = NULL;
PFNGLSHADERSOURCEPROC glad_glShaderSource = NULL;
PFNGLSTENCILFUNCPROC glad_glStencilFunc = NULL;
PFNGLSTENCILFUNCSEPARATEPROC glad_glStencilFuncSeparate = NULL;
PFNGLSTENCILMASKPROC glad_glStencilMask = NULL;
PFNGLSTENCILMASKSEPARATEPROC glad_glStencilMaskSeparate = NULL;
PFNGLSTENCILOPPROC glad_glStencilOp = NULL;
PFNGLSTENCILOPSEPARATEPROC glad_glStencilOpSeparate = NULL;
PFNGLTEXCOORD1DPROC glad_glTexCoord1d = NULL;
PFNGLTEXCOORD1DVPROC glad_glTexCoord1dv = NULL;
PFNGLTEXCOORD1FPROC glad_glTexCoord1f = NULL;
PFNGLTEXCOORD1FVPROC glad_glTexCoord1fv = NULL;
PFNGLTEXCOORD1IPROC glad_glTexCoord1i = NULL;
PFNGLTEXCOORD1IVPROC glad_glTexCoord1iv = NULL;
PFNGLTEXCOORD1SPROC glad_glTexCoord1s = NULL;
PFNGLTEXCOORD1SVPROC glad_glTexCoord1sv = NULL;
PFNGLTEXCOORD2DPROC glad_glTexCoord2d = NULL;
PFNGLTEXCOORD2DVPROC glad_glTexCoord2dv = NULL;
PFNGLTEXCOORD2FPROC glad_glTexCoord2f = NULL;
PFNGLTEXCOORD2FVPROC glad_glTexCoord2fv = NULL;
PFNGLTEXCOORD2IPROC glad_glTexCoord2i = NULL;
PFNGLTEXCOORD2IVPROC glad_glTexCoord2iv = NULL;
PFNGLTEXCOORD2SPROC glad_glTexCoord2s = NULL;
PFNGLTEXCOORD2SVPROC glad_glTexCoord2sv = NULL;
PFNGLTEXCOORD3DPROC glad_glTexCoord3d = NULL;
PFNGLTEXCOORD3DVPROC glad_glTexCoord3dv = NULL;
PFNGLTEXCOORD3FPROC glad_glTexCoord3f = NULL;
PFNGLTEXCOORD3FVPROC glad_glTexCoord3fv = NULL;
PFNGLTEXCOORD3IPROC glad_glTexCoord3i = NULL;
PFNGLTEXCOORD3IVPROC glad_glTexCoord3iv = NULL;
PFNGLTEXCOORD3SPROC glad_glTexCoord3s = NULL;
PFNGLTEXCOORD3SVPROC glad_glTexCoord3sv = NULL;
PFNGLTEXCOORD4DPROC glad_glTexCoord4d = NULL;
PFNGLTEXCOORD4DVPROC glad_glTexCoord4dv = NULL;
PFNGLTEXCOORD4FPROC glad_glTexCoord4f = NULL;
PFNGLTEXCOORD4FVPROC glad_glTexCoord4fv = NULL;
PFNGLTEXCOORD4IPROC glad_glTexCoord4i = NULL;
PFNGLTEXCOORD4IVPROC glad_glTexCoord4iv = NULL;
PFNGLTEXCOORD4SPROC glad_glTexCoord4s = NULL;
PFNGLTEXCOORD4SVPROC glad_glTexCoord4sv = NULL;
PFNGLTEXCOORDPOINTERPROC glad_glTexCoordPointer = NULL;
PFNGLTEXENVFPROC glad_glTexEnvf = NULL;
PFNGLTEXENVFVPROC glad_glTexEnvfv = NULL;
PFNGLTEXENVIPROC glad_glTexEnvi = NULL;
PFNGLTEXENVIVPROC glad_glTexEnviv = NULL;
PFNGLTEXGENDPROC glad_glTexGend = NULL;
PFNGLTEXGENDVPROC glad_glTexGendv = NULL;
PFNGLTEXGENFPROC glad_glTexGenf = NULL;
PFNGLTEXGENFVPROC glad_glTexGenfv = NULL;
PFNGLTEXGENIPROC glad_glTexGeni = NULL;
PFNGLTEXGENIVPROC glad_glTexGeniv = NULL;
PFNGLTEXIMAGE1DPROC glad_glTexImage1D = NULL;
PFNGLTEXIMAGE2DPROC glad_glTexImage2D = NULL;
PFNGLTEXIMAGE3DPROC glad_glTexImage3D = NULL;
PFNGLTEXPARAMETERFPROC glad_glTexParameterf = NULL;
PFNGLTEXPARAMETERFVPROC glad_glTexParameterfv = NULL;
PFNGLTEXPARAMETERIPROC glad_glTexParameteri = NULL;
PFNGLTEXPARAMETERIVPROC glad_glTexParameteriv = NULL;
PFNGLTEXSUBIMAGE1DPROC glad_glTexSubImage1D = NULL;
PFNGLTEXSUBIMAGE2DPROC glad_glTexSubImage2D = NULL;
PFNGLTEXSUBIMAGE3DPROC glad_glTexSubImage3D = NULL;
PFNGLTRANSLATEDPROC glad_glTranslated = NULL;
PFNGLTRANSLATEFPROC glad_glTranslatef = NULL;
PFNGLUNIFORM1FPROC glad_glUniform1f = NULL;
PFNGLUNIFORM1FVPROC glad_glUniform1fv = NULL;
PFNGLUNIFORM1IPROC glad_glUniform1i = NULL;
PFNGLUNIFORM1IVPROC glad_glUniform1iv = NULL;
PFNGLUNIFORM2FPROC glad_glUniform2f = NULL;
PFNGLUNIFORM2FVPROC glad_glUniform2fv = NULL;
PFNGLUNIFORM2IPROC glad_glUniform2i = NULL;
PFNGLUNIFORM2IVPROC glad_glUniform2iv = NULL;
PFNGLUNIFORM3FPROC glad_glUniform3f = NULL;
PFNGLUNIFORM3FVPROC glad_glUniform3fv = NULL;
PFNGLUNIFORM3IPROC glad_glUniform3i = NULL;
PFNGLUNIFORM3IVPROC glad_glUniform3iv = NULL;
PFNGLUNIFORM4FPROC glad_glUniform4f = NULL;
PFNGLUNIFORM4FVPROC glad_glUniform4fv = NULL;
PFNGLUNIFORM4IPROC glad_glUniform4i = NULL;
PFNGLUNIFORM4IVPROC glad_glUniform4iv = NULL;
PFNGLUNIFORMMATRIX2FVPROC glad_glUniformMatrix2fv = NULL;
PFNGLUNIFORMMATRIX2X3FVPROC glad_glUniformMatrix2x3fv = NULL;
PFNGLUNIFORMMATRIX2X4FVPROC glad_glUniformMatrix2x4fv = NULL;
PFNGLUNIFORMMATRIX3FVPROC glad_glUniformMatrix3fv = NULL;
PFNGLUNIFORMMATRIX3X2FVPROC glad_glUniformMatrix3x2fv = NULL;
PFNGLUNIFORMMATRIX3X4FVPROC glad_glUniformMatrix3x4fv = NULL;
PFNGLUNIFORMMATRIX4FVPROC glad_glUniformMatrix4fv = NULL;
PFNGLUNIFORMMATRIX4X2FVPROC glad_glUniformMatrix4x2fv = NULL;
PFNGLUNIFORMMATRIX4X3FVPROC glad_glUniformMatrix4x3fv = NULL;
PFNGLUNMAPBUFFERPROC glad_glUnmapBuffer = NULL;
PFNGLUSEPROGRAMPROC glad_glUseProgram = NULL;
PFNGLVALIDATEPROGRAMPROC glad_glValidateProgram = NULL;
PFNGLVERTEX2DPROC glad_glVertex2d = NULL;
PFNGLVERTEX2DVPROC glad_glVertex2dv = NULL;
PFNGLVERTEX2FPROC glad_glVertex2f = NULL;
PFNGLVERTEX2FVPROC glad_glVertex2fv = NULL;
PFNGLVERTEX2IPROC glad_glVertex2i = NULL;
PFNGLVERTEX2IVPROC glad_glVertex2iv = NULL;
PFNGLVERTEX2SPROC glad_glVertex2s = NULL;
PFNGLVERTEX2SVPROC glad_glVertex2sv = NULL;
PFNGLVERTEX3DPROC glad_glVertex3d = NULL;
PFNGLVERTEX3DVPROC glad_glVertex3dv = NULL;
PFNGLVERTEX3FPROC glad_glVertex3f = NULL;
PFNGLVERTEX3FVPROC glad_glVertex3fv = NULL;
PFNGLVERTEX3IPROC glad_glVertex3i = NULL;
PFNGLVERTEX3IVPROC glad_glVertex3iv = NULL;
PFNGLVERTEX3SPROC glad_glVertex3s = NULL;
PFNGLVERTEX3SVPROC glad_glVertex3sv = NULL;
PFNGLVERTEX4DPROC glad_glVertex4d = NULL;
PFNGLVERTEX4DVPROC glad_glVertex4dv = NULL;
PFNGLVERTEX4FPROC glad_glVertex4f = NULL;
PFNGLVERTEX4FVPROC glad_glVertex4fv = NULL;
PFNGLVERTEX4IPROC glad_glVertex4i = NULL;
PFNGLVERTEX4IVPROC glad_glVertex4iv = NULL;
PFNGLVERTEX4SPROC glad_glVertex4s = NULL;
PFNGLVERTEX4SVPROC glad_glVertex4sv = NULL;
PFNGLVERTEXATTRIB1DPROC glad_glVertexAttrib1d = NULL;
PFNGLVERTEXATTRIB1DVPROC glad_glVertexAttrib1dv = NULL;
PFNGLVERTEXATTRIB1FPROC glad_glVertexAttrib1f = NULL;
PFNGLVERTEXATTRIB1FVPROC glad_glVertexAttrib1fv = NULL;
PFNGLVERTEXATTRIB1SPROC glad_glVertexAttrib1s = NULL;
PFNGLVERTEXATTRIB1SVPROC glad_glVertexAttrib1sv = NULL;
PFNGLVERTEXATTRIB2DPROC glad_glVertexAttrib2d = NULL;
PFNGLVERTEXATTRIB2DVPROC glad_glVertexAttrib2dv = NULL;
PFNGLVERTEXATTRIB2FPROC glad_glVertexAttrib2f = NULL;
PFNGLVERTEXATTRIB2FVPROC glad_glVertexAttrib2fv = NULL;
PFNGLVERTEXATTRIB2SPROC glad_glVertexAttrib2s = NULL;
PFNGLVERTEXATTRIB2SVPROC glad_glVertexAttrib2sv = NULL;
PFNGLVERTEXATTRIB3DPROC glad_glVertexAttrib3d = NULL;
PFNGLVERTEXATTRIB3DVPROC glad_glVertexAttrib3dv = NULL;
PFNGLVERTEXATTRIB3FPROC glad_glVertexAttrib3f = NULL;
PFNGLVERTEXATTRIB3FVPROC glad_glVertexAttrib3fv = NULL;
PFNGLVERTEXATTRIB3SPROC glad_glVertexAttrib3s = NULL;
PFNGLVERTEXATTRIB3SVPROC glad_glVertexAttrib3sv = NULL;
PFNGLVERTEXATTRIB4NBVPROC glad_glVertexAttrib4Nbv = NULL;
PFNGLVERTEXATTRIB4NIVPROC glad_glVertexAttrib4Niv = NULL;
PFNGLVERTEXATTRIB4NSVPROC glad_glVertexAttrib4Nsv = NULL;
PFNGLVERTEXATTRIB4NUBPROC glad_glVertexAttrib4Nub = NULL;
PFNGLVERTEXATTRIB4NUBVPROC glad_glVertexAttrib4Nubv = NULL;
PFNGLVERTEXATTRIB4NUIVPROC glad_glVertexAttrib4Nuiv = NULL;
PFNGLVERTEXATTRIB4NUSVPROC glad_glVertexAttrib4Nusv = NULL;
PFNGLVERTEXATTRIB4BVPROC glad_glVertexAttrib4bv = NULL;
PFNGLVERTEXATTRIB4DPROC glad_glVertexAttrib4d = NULL;
PFNGLVERTEXATTRIB4DVPROC glad_glVertexAttrib4dv = NULL;
PFNGLVERTEXATTRIB4FPROC glad_glVertexAttrib4f = NULL;
PFNGLVERTEXATTRIB4FVPROC glad_glVertexAttrib4fv = NULL;
PFNGLVERTEXATTRIB4IVPROC glad_glVertexAttrib4iv = NULL;
PFNGLVERTEXATTRIB4SPROC glad_glVertexAttrib4s = NULL;
PFNGLVERTEXATTRIB4SVPROC glad_glVertexAttrib4sv = NULL;
PFNGLVERTEXATTRIB4UBVPROC glad_glVertexAttrib4ubv = NULL;
PFNGLVERTEXATTRIB4UIVPROC glad_glVertexAttrib4uiv = NULL;
PFNGLVERTEXATTRIB4USVPROC glad_glVertexAttrib4usv = NULL;
PFNGLVERTEXATTRIBPOINTERPROC glad_glVertexAttribPointer = NULL;
PFNGLVERTEXPOINTERPROC glad_glVertexPointer = NULL;
PFNGLVIEWPORTPROC glad_glViewport = NULL;
PFNGLWINDOWPOS2DPROC glad_glWindowPos2d = NULL;
PFNGLWINDOWPOS2DVPROC glad_glWindowPos2dv = NULL;
PFNGLWINDOWPOS2FPROC glad_glWindowPos2f = NULL;
PFNGLWINDOWPOS2FVPROC glad_glWindowPos2fv = NULL;
PFNGLWINDOWPOS2IPROC glad_glWindowPos2i = NULL;
PFNGLWINDOWPOS2IVPROC glad_glWindowPos2iv = NULL;
PFNGLWINDOWPOS2SPROC glad_glWindowPos2s = NULL;
PFNGLWINDOWPOS2SVPROC glad_glWindowPos2sv = NULL;
PFNGLWINDOWPOS3DPROC glad_glWindowPos3d = NULL;
PFNGLWINDOWPOS3DVPROC glad_glWindowPos3dv = NULL;
PFNGLWINDOWPOS3FPROC glad_glWindowPos3f = NULL;
PFNGLWINDOWPOS3FVPROC glad_glWindowPos3fv = NULL;
PFNGLWINDOWPOS3IPROC glad_glWindowPos3i = NULL;
PFNGLWINDOWPOS3IVPROC glad_glWindowPos3iv = NULL;
PFNGLWINDOWPOS3SPROC glad_glWindowPos3s = NULL;
PFNGLWINDOWPOS3SVPROC glad_glWindowPos3sv = NULL;

PFNGLBINDVERTEXARRAYPROC glad_glBindVertexArray = NULL;
PFNGLDELETEVERTEXARRAYSPROC glad_glDeleteVertexArrays = NULL;
PFNGLGENVERTEXARRAYSPROC glad_glGenVertexArrays = NULL;
PFNGLISVERTEXARRAYPROC glad_glIsVertexArray = NULL;


static void glad_gl_load_GL_VERSION_1_0(GLADuserptrloadfunc load, void * userptr)
{
    if(!GLAD_GL_VERSION_1_0) return;
    glad_glAccum = (PFNGLACCUMPROC) load(userptr, "glAccum");
    glad_glAlphaFunc = (PFNGLALPHAFUNCPROC) load(userptr, "glAlphaFunc");
    glad_glBegin = (PFNGLBEGINPROC) load(userptr, "glBegin");
    glad_glBitmap = (PFNGLBITMAPPROC) load(userptr, "glBitmap");
    glad_glBlendFunc = (PFNGLBLENDFUNCPROC) load(userptr, "glBlendFunc");
    glad_glCallList = (PFNGLCALLLISTPROC) load(userptr, "glCallList");
    glad_glCallLists = (PFNGLCALLLISTSPROC) load(userptr, "glCallLists");
    glad_glClear = (PFNGLCLEARPROC) load(userptr, "glClear");
    glad_glClearAccum = (PFNGLCLEARACCUMPROC) load(userptr, "glClearAccum");
    glad_glClearColor = (PFNGLCLEARCOLORPROC) load(userptr, "glClearColor");
    glad_glClearDepth = (PFNGLCLEARDEPTHPROC) load(userptr, "glClearDepth");
    glad_glClearIndex = (PFNGLCLEARINDEXPROC) load(userptr, "glClearIndex");
    glad_glClearStencil = (PFNGLCLEARSTENCILPROC) load(userptr, "glClearStencil");
    glad_glClipPlane = (PFNGLCLIPPLANEPROC) load(userptr, "glClipPlane");
    glad_glColor3b = (PFNGLCOLOR3BPROC) load(userptr, "glColor3b");
    glad_glColor3bv = (PFNGLCOLOR3BVPROC) load(userptr, "glColor3bv");
    glad_glColor3d = (PFNGLCOLOR3DPROC) load(userptr, "glColor3d");
    glad_glColor3dv = (PFNGLCOLOR3DVPROC) load(userptr, "glColor3dv");
    glad_glColor3f = (PFNGLCOLOR3FPROC) load(userptr, "glColor3f");
    glad_glColor3fv = (PFNGLCOLOR3FVPROC) load(userptr, "glColor3fv");
    glad_glColor3i = (PFNGLCOLOR3IPROC) load(userptr, "glColor3i");
    glad_glColor3iv = (PFNGLCOLOR3IVPROC) load(userptr, "glColor3iv");
    glad_glColor3s = (PFNGLCOLOR3SPROC) load(userptr, "glColor3s");
    glad_glColor3sv = (PFNGLCOLOR3SVPROC) load(userptr, "glColor3sv");
    glad_glColor3ub = (PFNGLCOLOR3UBPROC) load(userptr, "glColor3ub");
    glad_glColor3ubv = (PFNGLCOLOR3UBVPROC) load(userptr, "glColor3ubv");
    glad_glColor3ui = (PFNGLCOLOR3UIPROC) load(userptr, "glColor3ui");
    glad_glColor3uiv = (PFNGLCOLOR3UIVPROC) load(userptr, "glColor3uiv");
    glad_glColor3us = (PFNGLCOLOR3USPROC) load(userptr, "glColor3us");
    glad_glColor3usv = (PFNGLCOLOR3USVPROC) load(userptr, "glColor3usv");
    glad_glColor4b = (PFNGLCOLOR4BPROC) load(userptr, "glColor4b");
    glad_glColor4bv = (PFNGLCOLOR4BVPROC) load(userptr, "glColor4bv");
    glad_glColor4d = (PFNGLCOLOR4DPROC) load(userptr, "glColor4d");
    glad_glColor4dv = (PFNGLCOLOR4DVPROC) load(userptr, "glColor4dv");
    glad_glColor4f = (PFNGLCOLOR4FPROC) load(userptr, "glColor4f");
    glad_glColor4fv = (PFNGLCOLOR4FVPROC) load(userptr, "glColor4fv");
    glad_glColor4i = (PFNGLCOLOR4IPROC) load(userptr, "glColor4i");
    glad_glColor4iv = (PFNGLCOLOR4IVPROC) load(userptr, "glColor4iv");
    glad_glColor4s = (PFNGLCOLOR4SPROC) load(userptr, "glColor4s");
    glad_glColor4sv = (PFNGLCOLOR4SVPROC) load(userptr, "glColor4sv");
    glad_glColor4ub = (PFNGLCOLOR4UBPROC) load(userptr, "glColor4ub");
    glad_glColor4ubv = (PFNGLCOLOR4UBVPROC) load(userptr, "glColor4ubv");
    glad_glColor4ui = (PFNGLCOLOR4UIPROC) load(userptr, "glColor4ui");
    glad_glColor4uiv = (PFNGLCOLOR4UIVPROC) load(userptr, "glColor4uiv");
    glad_glColor4us = (PFNGLCOLOR4USPROC) load(userptr, "glColor4us");
    glad_glColor4usv = (PFNGLCOLOR4USVPROC) load(userptr, "glColor4usv");
    glad_glColorMask = (PFNGLCOLORMASKPROC) load(userptr, "glColorMask");
    glad_glColorMaterial = (PFNGLCOLORMATERIALPROC) load(userptr, "glColorMaterial");
    glad_glCopyPixels = (PFNGLCOPYPIXELSPROC) load(userptr, "glCopyPixels");
    glad_glCullFace = (PFNGLCULLFACEPROC) load(userptr, "glCullFace");
    glad_glDeleteLists = (PFNGLDELETELISTSPROC) load(userptr, "glDeleteLists");
    glad_glDepthFunc = (PFNGLDEPTHFUNCPROC) load(userptr, "glDepthFunc");
    glad_glDepthMask = (PFNGLDEPTHMASKPROC) load(userptr, "glDepthMask");
    glad_glDepthRange = (PFNGLDEPTHRANGEPROC) load(userptr, "glDepthRange");
    glad_glDisable = (PFNGLDISABLEPROC) load(userptr, "glDisable");
    glad_glDrawBuffer = (PFNGLDRAWBUFFERPROC) load(userptr, "glDrawBuffer");
    glad_glDrawPixels = (PFNGLDRAWPIXELSPROC) load(userptr, "glDrawPixels");
    glad_glEdgeFlag = (PFNGLEDGEFLAGPROC) load(userptr, "glEdgeFlag");
    glad_glEdgeFlagv = (PFNGLEDGEFLAGVPROC) load(userptr, "glEdgeFlagv");
    glad_glEnable = (PFNGLENABLEPROC) load(userptr, "glEnable");
    glad_glEnd = (PFNGLENDPROC) load(userptr, "glEnd");
    glad_glEndList = (PFNGLENDLISTPROC) load(userptr, "glEndList");
    glad_glEvalCoord1d = (PFNGLEVALCOORD1DPROC) load(userptr, "glEvalCoord1d");
    glad_glEvalCoord1dv = (PFNGLEVALCOORD1DVPROC) load(userptr, "glEvalCoord1dv");
    glad_glEvalCoord1f = (PFNGLEVALCOORD1FPROC) load(userptr, "glEvalCoord1f");
    glad_glEvalCoord1fv = (PFNGLEVALCOORD1FVPROC) load(userptr, "glEvalCoord1fv");
    glad_glEvalCoord2d = (PFNGLEVALCOORD2DPROC) load(userptr, "glEvalCoord2d");
    glad_glEvalCoord2dv = (PFNGLEVALCOORD2DVPROC) load(userptr, "glEvalCoord2dv");
    glad_glEvalCoord2f = (PFNGLEVALCOORD2FPROC) load(userptr, "glEvalCoord2f");
    glad_glEvalCoord2fv = (PFNGLEVALCOORD2FVPROC) load(userptr, "glEvalCoord2fv");
    glad_glEvalMesh1 = (PFNGLEVALMESH1PROC) load(userptr, "glEvalMesh1");
    glad_glEvalMesh2 = (PFNGLEVALMESH2PROC) load(userptr, "glEvalMesh2");
    glad_glEvalPoint1 = (PFNGLEVALPOINT1PROC) load(userptr, "glEvalPoint1");
    glad_glEvalPoint2 = (PFNGLEVALPOINT2PROC) load(userptr, "glEvalPoint2");
    glad_glFeedbackBuffer = (PFNGLFEEDBACKBUFFERPROC) load(userptr, "glFeedbackBuffer");
    glad_glFinish = (PFNGLFINISHPROC) load(userptr, "glFinish");
    glad_glFlush = (PFNGLFLUSHPROC) load(userptr, "glFlush");
    glad_glFogf = (PFNGLFOGFPROC) load(userptr, "glFogf");
    glad_glFogfv = (PFNGLFOGFVPROC) load(userptr, "glFogfv");
    glad_glFogi = (PFNGLFOGIPROC) load(userptr, "glFogi");
    glad_glFogiv = (PFNGLFOGIVPROC) load(userptr, "glFogiv");
    glad_glFrontFace = (PFNGLFRONTFACEPROC) load(userptr, "glFrontFace");
    glad_glFrustum = (PFNGLFRUSTUMPROC) load(userptr, "glFrustum");
    glad_glGenLists = (PFNGLGENLISTSPROC) load(userptr, "glGenLists");
    glad_glGetBooleanv = (PFNGLGETBOOLEANVPROC) load(userptr, "glGetBooleanv");
    glad_glGetClipPlane = (PFNGLGETCLIPPLANEPROC) load(userptr, "glGetClipPlane");
    glad_glGetDoublev = (PFNGLGETDOUBLEVPROC) load(userptr, "glGetDoublev");
    glad_glGetError = (PFNGLGETERRORPROC) load(userptr, "glGetError");
    glad_glGetFloatv = (PFNGLGETFLOATVPROC) load(userptr, "glGetFloatv");
    glad_glGetIntegerv = (PFNGLGETINTEGERVPROC) load(userptr, "glGetIntegerv");
    glad_glGetLightfv = (PFNGLGETLIGHTFVPROC) load(userptr, "glGetLightfv");
    glad_glGetLightiv = (PFNGLGETLIGHTIVPROC) load(userptr, "glGetLightiv");
    glad_glGetMapdv = (PFNGLGETMAPDVPROC) load(userptr, "glGetMapdv");
    glad_glGetMapfv = (PFNGLGETMAPFVPROC) load(userptr, "glGetMapfv");
    glad_glGetMapiv = (PFNGLGETMAPIVPROC) load(userptr, "glGetMapiv");
    glad_glGetMaterialfv = (PFNGLGETMATERIALFVPROC) load(userptr, "glGetMaterialfv");
    glad_glGetMaterialiv = (PFNGLGETMATERIALIVPROC) load(userptr, "glGetMaterialiv");
    glad_glGetPixelMapfv = (PFNGLGETPIXELMAPFVPROC) load(userptr, "glGetPixelMapfv");
    glad_glGetPixelMapuiv = (PFNGLGETPIXELMAPUIVPROC) load(userptr, "glGetPixelMapuiv");
    glad_glGetPixelMapusv = (PFNGLGETPIXELMAPUSVPROC) load(userptr, "glGetPixelMapusv");
    glad_glGetPolygonStipple = (PFNGLGETPOLYGONSTIPPLEPROC) load(userptr, "glGetPolygonStipple");
    glad_glGetString = (PFNGLGETSTRINGPROC) load(userptr, "glGetString");
    glad_glGetTexEnvfv = (PFNGLGETTEXENVFVPROC) load(userptr, "glGetTexEnvfv");
    glad_glGetTexEnviv = (PFNGLGETTEXENVIVPROC) load(userptr, "glGetTexEnviv");
    glad_glGetTexGendv = (PFNGLGETTEXGENDVPROC) load(userptr, "glGetTexGendv");
    glad_glGetTexGenfv = (PFNGLGETTEXGENFVPROC) load(userptr, "glGetTexGenfv");
    glad_glGetTexGeniv = (PFNGLGETTEXGENIVPROC) load(userptr, "glGetTexGeniv");
    glad_glGetTexImage = (PFNGLGETTEXIMAGEPROC) load(userptr, "glGetTexImage");
    glad_glGetTexLevelParameterfv = (PFNGLGETTEXLEVELPARAMETERFVPROC) load(userptr, "glGetTexLevelParameterfv");
    glad_glGetTexLevelParameteriv = (PFNGLGETTEXLEVELPARAMETERIVPROC) load(userptr, "glGetTexLevelParameteriv");
    glad_glGetTexParameterfv = (PFNGLGETTEXPARAMETERFVPROC) load(userptr, "glGetTexParameterfv");
    glad_glGetTexParameteriv = (PFNGLGETTEXPARAMETERIVPROC) load(userptr, "glGetTexParameteriv");
    glad_glHint = (PFNGLHINTPROC) load(userptr, "glHint");
    glad_glIndexMask = (PFNGLINDEXMASKPROC) load(userptr, "glIndexMask");
    glad_glIndexd = (PFNGLINDEXDPROC) load(userptr, "glIndexd");
    glad_glIndexdv = (PFNGLINDEXDVPROC) load(userptr, "glIndexdv");
    glad_glIndexf = (PFNGLINDEXFPROC) load(userptr, "glIndexf");
    glad_glIndexfv = (PFNGLINDEXFVPROC) load(userptr, "glIndexfv");
    glad_glIndexi = (PFNGLINDEXIPROC) load(userptr, "glIndexi");
    glad_glIndexiv = (PFNGLINDEXIVPROC) load(userptr, "glIndexiv");
    glad_glIndexs = (PFNGLINDEXSPROC) load(userptr, "glIndexs");
    glad_glIndexsv = (PFNGLINDEXSVPROC) load(userptr, "glIndexsv");
    glad_glInitNames = (PFNGLINITNAMESPROC) load(userptr, "glInitNames");
    glad_glIsEnabled = (PFNGLISENABLEDPROC) load(userptr, "glIsEnabled");
    glad_glIsList = (PFNGLISLISTPROC) load(userptr, "glIsList");
    glad_glLightModelf = (PFNGLLIGHTMODELFPROC) load(userptr, "glLightModelf");
    glad_glLightModelfv = (PFNGLLIGHTMODELFVPROC) load(userptr, "glLightModelfv");
    glad_glLightModeli = (PFNGLLIGHTMODELIPROC) load(userptr, "glLightModeli");
    glad_glLightModeliv = (PFNGLLIGHTMODELIVPROC) load(userptr, "glLightModeliv");
    glad_glLightf = (PFNGLLIGHTFPROC) load(userptr, "glLightf");
    glad_glLightfv = (PFNGLLIGHTFVPROC) load(userptr, "glLightfv");
    glad_glLighti = (PFNGLLIGHTIPROC) load(userptr, "glLighti");
    glad_glLightiv = (PFNGLLIGHTIVPROC) load(userptr, "glLightiv");
    glad_glLineStipple = (PFNGLLINESTIPPLEPROC) load(userptr, "glLineStipple");
    glad_glLineWidth = (PFNGLLINEWIDTHPROC) load(userptr, "glLineWidth");
    glad_glListBase = (PFNGLLISTBASEPROC) load(userptr, "glListBase");
    glad_glLoadIdentity = (PFNGLLOADIDENTITYPROC) load(userptr, "glLoadIdentity");
    glad_glLoadMatrixd = (PFNGLLOADMATRIXDPROC) load(userptr, "glLoadMatrixd");
    glad_glLoadMatrixf = (PFNGLLOADMATRIXFPROC) load(userptr, "glLoadMatrixf");
    glad_glLoadName = (PFNGLLOADNAMEPROC) load(userptr, "glLoadName");
    glad_glLogicOp = (PFNGLLOGICOPPROC) load(userptr, "glLogicOp");
    glad_glMap1d = (PFNGLMAP1DPROC) load(userptr, "glMap1d");
    glad_glMap1f = (PFNGLMAP1FPROC) load(userptr, "glMap1f");
    glad_glMap2d = (PFNGLMAP2DPROC) load(userptr, "glMap2d");
    glad_glMap2f = (PFNGLMAP2FPROC) load(userptr, "glMap2f");
    glad_glMapGrid1d = (PFNGLMAPGRID1DPROC) load(userptr, "glMapGrid1d");
    glad_glMapGrid1f = (PFNGLMAPGRID1FPROC) load(userptr, "glMapGrid1f");
    glad_glMapGrid2d = (PFNGLMAPGRID2DPROC) load(userptr, "glMapGrid2d");
    glad_glMapGrid2f = (PFNGLMAPGRID2FPROC) load(userptr, "glMapGrid2f");
    glad_glMaterialf = (PFNGLMATERIALFPROC) load(userptr, "glMaterialf");
    glad_glMaterialfv = (PFNGLMATERIALFVPROC) load(userptr, "glMaterialfv");
    glad_glMateriali = (PFNGLMATERIALIPROC) load(userptr, "glMateriali");
    glad_glMaterialiv = (PFNGLMATERIALIVPROC) load(userptr, "glMaterialiv");
    glad_glMatrixMode = (PFNGLMATRIXMODEPROC) load(userptr, "glMatrixMode");
    glad_glMultMatrixd = (PFNGLMULTMATRIXDPROC) load(userptr, "glMultMatrixd");
    glad_glMultMatrixf = (PFNGLMULTMATRIXFPROC) load(userptr, "glMultMatrixf");
    glad_glNewList = (PFNGLNEWLISTPROC) load(userptr, "glNewList");
    glad_glNormal3b = (PFNGLNORMAL3BPROC) load(userptr, "glNormal3b");
    glad_glNormal3bv = (PFNGLNORMAL3BVPROC) load(userptr, "glNormal3bv");
    glad_glNormal3d = (PFNGLNORMAL3DPROC) load(userptr, "glNormal3d");
    glad_glNormal3dv = (PFNGLNORMAL3DVPROC) load(userptr, "glNormal3dv");
    glad_glNormal3f = (PFNGLNORMAL3FPROC) load(userptr, "glNormal3f");
    glad_glNormal3fv = (PFNGLNORMAL3FVPROC) load(userptr, "glNormal3fv");
    glad_glNormal3i = (PFNGLNORMAL3IPROC) load(userptr, "glNormal3i");
    glad_glNormal3iv = (PFNGLNORMAL3IVPROC) load(userptr, "glNormal3iv");
    glad_glNormal3s = (PFNGLNORMAL3SPROC) load(userptr, "glNormal3s");
    glad_glNormal3sv = (PFNGLNORMAL3SVPROC) load(userptr, "glNormal3sv");
    glad_glOrtho = (PFNGLORTHOPROC) load(userptr, "glOrtho");
    glad_glPassThrough = (PFNGLPASSTHROUGHPROC) load(userptr, "glPassThrough");
    glad_glPixelMapfv = (PFNGLPIXELMAPFVPROC) load(userptr, "glPixelMapfv");
    glad_glPixelMapuiv = (PFNGLPIXELMAPUIVPROC) load(userptr, "glPixelMapuiv");
    glad_glPixelMapusv = (PFNGLPIXELMAPUSVPROC) load(userptr, "glPixelMapusv");
    glad_glPixelStoref = (PFNGLPIXELSTOREFPROC) load(userptr, "glPixelStoref");
    glad_glPixelStorei = (PFNGLPIXELSTOREIPROC) load(userptr, "glPixelStorei");
    glad_glPixelTransferf = (PFNGLPIXELTRANSFERFPROC) load(userptr, "glPixelTransferf");
    glad_glPixelTransferi = (PFNGLPIXELTRANSFERIPROC) load(userptr, "glPixelTransferi");
    glad_glPixelZoom = (PFNGLPIXELZOOMPROC) load(userptr, "glPixelZoom");
    glad_glPointSize = (PFNGLPOINTSIZEPROC) load(userptr, "glPointSize");
    glad_glPolygonMode = (PFNGLPOLYGONMODEPROC) load(userptr, "glPolygonMode");
    glad_glPolygonStipple = (PFNGLPOLYGONSTIPPLEPROC) load(userptr, "glPolygonStipple");
    glad_glPopAttrib = (PFNGLPOPATTRIBPROC) load(userptr, "glPopAttrib");
    glad_glPopMatrix = (PFNGLPOPMATRIXPROC) load(userptr, "glPopMatrix");
    glad_glPopName = (PFNGLPOPNAMEPROC) load(userptr, "glPopName");
    glad_glPushAttrib = (PFNGLPUSHATTRIBPROC) load(userptr, "glPushAttrib");
    glad_glPushMatrix = (PFNGLPUSHMATRIXPROC) load(userptr, "glPushMatrix");
    glad_glPushName = (PFNGLPUSHNAMEPROC) load(userptr, "glPushName");
    glad_glRasterPos2d = (PFNGLRASTERPOS2DPROC) load(userptr, "glRasterPos2d");
    glad_glRasterPos2dv = (PFNGLRASTERPOS2DVPROC) load(userptr, "glRasterPos2dv");
    glad_glRasterPos2f = (PFNGLRASTERPOS2FPROC) load(userptr, "glRasterPos2f");
    glad_glRasterPos2fv = (PFNGLRASTERPOS2FVPROC) load(userptr, "glRasterPos2fv");
    glad_glRasterPos2i = (PFNGLRASTERPOS2IPROC) load(userptr, "glRasterPos2i");
    glad_glRasterPos2iv = (PFNGLRASTERPOS2IVPROC) load(userptr, "glRasterPos2iv");
    glad_glRasterPos2s = (PFNGLRASTERPOS2SPROC) load(userptr, "glRasterPos2s");
    glad_glRasterPos2sv = (PFNGLRASTERPOS2SVPROC) load(userptr, "glRasterPos2sv");
    glad_glRasterPos3d = (PFNGLRASTERPOS3DPROC) load(userptr, "glRasterPos3d");
    glad_glRasterPos3dv = (PFNGLRASTERPOS3DVPROC) load(userptr, "glRasterPos3dv");
    glad_glRasterPos3f = (PFNGLRASTERPOS3FPROC) load(userptr, "glRasterPos3f");
    glad_glRasterPos3fv = (PFNGLRASTERPOS3FVPROC) load(userptr, "glRasterPos3fv");
    glad_glRasterPos3i = (PFNGLRASTERPOS3IPROC) load(userptr, "glRasterPos3i");
    glad_glRasterPos3iv = (PFNGLRASTERPOS3IVPROC) load(userptr, "glRasterPos3iv");
    glad_glRasterPos3s = (PFNGLRASTERPOS3SPROC) load(userptr, "glRasterPos3s");
    glad_glRasterPos3sv = (PFNGLRASTERPOS3SVPROC) load(userptr, "glRasterPos3sv");
    glad_glRasterPos4d = (PFNGLRASTERPOS4DPROC) load(userptr, "glRasterPos4d");
    glad_glRasterPos4dv = (PFNGLRASTERPOS4DVPROC) load(userptr, "glRasterPos4dv");
    glad_glRasterPos4f = (PFNGLRASTERPOS4FPROC) load(userptr, "glRasterPos4f");
    glad_glRasterPos4fv = (PFNGLRASTERPOS4FVPROC) load(userptr, "glRasterPos4fv");
    glad_glRasterPos4i = (PFNGLRASTERPOS4IPROC) load(userptr, "glRasterPos4i");
    glad_glRasterPos4iv = (PFNGLRASTERPOS4IVPROC) load(userptr, "glRasterPos4iv");
    glad_glRasterPos4s = (PFNGLRASTERPOS4SPROC) load(userptr, "glRasterPos4s");
    glad_glRasterPos4sv = (PFNGLRASTERPOS4SVPROC) load(userptr, "glRasterPos4sv");
    glad_glReadBuffer = (PFNGLREADBUFFERPROC) load(userptr, "glReadBuffer");
    glad_glReadPixels = (PFNGLREADPIXELSPROC) load(userptr, "glReadPixels");
    glad_glRectd = (PFNGLRECTDPROC) load(userptr, "glRectd");
    glad_glRectdv = (PFNGLRECTDVPROC) load(userptr, "glRectdv");
    glad_glRectf = (PFNGLRECTFPROC) load(userptr, "glRectf");
    glad_glRectfv = (PFNGLRECTFVPROC) load(userptr, "glRectfv");
    glad_glRecti = (PFNGLRECTIPROC) load(userptr, "glRecti");
    glad_glRectiv = (PFNGLRECTIVPROC) load(userptr, "glRectiv");
    glad_glRects = (PFNGLRECTSPROC) load(userptr, "glRects");
    glad_glRectsv = (PFNGLRECTSVPROC) load(userptr, "glRectsv");
    glad_glRenderMode = (PFNGLRENDERMODEPROC) load(userptr, "glRenderMode");
    glad_glRotated = (PFNGLROTATEDPROC) load(userptr, "glRotated");
    glad_glRotatef = (PFNGLROTATEFPROC) load(userptr, "glRotatef");
    glad_glScaled = (PFNGLSCALEDPROC) load(userptr, "glScaled");
    glad_glScalef = (PFNGLSCALEFPROC) load(userptr, "glScalef");
    glad_glScissor = (PFNGLSCISSORPROC) load(userptr, "glScissor");
    glad_glSelectBuffer = (PFNGLSELECTBUFFERPROC) load(userptr, "glSelectBuffer");
    glad_glShadeModel = (PFNGLSHADEMODELPROC) load(userptr, "glShadeModel");
    glad_glStencilFunc = (PFNGLSTENCILFUNCPROC) load(userptr, "glStencilFunc");
    glad_glStencilMask = (PFNGLSTENCILMASKPROC) load(userptr, "glStencilMask");
    glad_glStencilOp = (PFNGLSTENCILOPPROC) load(userptr, "glStencilOp");
    glad_glTexCoord1d = (PFNGLTEXCOORD1DPROC) load(userptr, "glTexCoord1d");
    glad_glTexCoord1dv = (PFNGLTEXCOORD1DVPROC) load(userptr, "glTexCoord1dv");
    glad_glTexCoord1f = (PFNGLTEXCOORD1FPROC) load(userptr, "glTexCoord1f");
    glad_glTexCoord1fv = (PFNGLTEXCOORD1FVPROC) load(userptr, "glTexCoord1fv");
    glad_glTexCoord1i = (PFNGLTEXCOORD1IPROC) load(userptr, "glTexCoord1i");
    glad_glTexCoord1iv = (PFNGLTEXCOORD1IVPROC) load(userptr, "glTexCoord1iv");
    glad_glTexCoord1s = (PFNGLTEXCOORD1SPROC) load(userptr, "glTexCoord1s");
    glad_glTexCoord1sv = (PFNGLTEXCOORD1SVPROC) load(userptr, "glTexCoord1sv");
    glad_glTexCoord2d = (PFNGLTEXCOORD2DPROC) load(userptr, "glTexCoord2d");
    glad_glTexCoord2dv = (PFNGLTEXCOORD2DVPROC) load(userptr, "glTexCoord2dv");
    glad_glTexCoord2f = (PFNGLTEXCOORD2FPROC) load(userptr, "glTexCoord2f");
    glad_glTexCoord2fv = (PFNGLTEXCOORD2FVPROC) load(userptr, "glTexCoord2fv");
    glad_glTexCoord2i = (PFNGLTEXCOORD2IPROC) load(userptr, "glTexCoord2i");
    glad_glTexCoord2iv = (PFNGLTEXCOORD2IVPROC) load(userptr, "glTexCoord2iv");
    glad_glTexCoord2s = (PFNGLTEXCOORD2SPROC) load(userptr, "glTexCoord2s");
    glad_glTexCoord2sv = (PFNGLTEXCOORD2SVPROC) load(userptr, "glTexCoord2sv");
    glad_glTexCoord3d = (PFNGLTEXCOORD3DPROC) load(userptr, "glTexCoord3d");
    glad_glTexCoord3dv = (PFNGLTEXCOORD3DVPROC) load(userptr, "glTexCoord3dv");
    glad_glTexCoord3f = (PFNGLTEXCOORD3FPROC) load(userptr, "glTexCoord3f");
    glad_glTexCoord3fv = (PFNGLTEXCOORD3FVPROC) load(userptr, "glTexCoord3fv");
    glad_glTexCoord3i = (PFNGLTEXCOORD3IPROC) load(userptr, "glTexCoord3i");
    glad_glTexCoord3iv = (PFNGLTEXCOORD3IVPROC) load(userptr, "glTexCoord3iv");
    glad_glTexCoord3s = (PFNGLTEXCOORD3SPROC) load(userptr, "glTexCoord3s");
    glad_glTexCoord3sv = (PFNGLTEXCOORD3SVPROC) load(userptr, "glTexCoord3sv");
    glad_glTexCoord4d = (PFNGLTEXCOORD4DPROC) load(userptr, "glTexCoord4d");
    glad_glTexCoord4dv = (PFNGLTEXCOORD4DVPROC) load(userptr, "glTexCoord4dv");
    glad_glTexCoord4f = (PFNGLTEXCOORD4FPROC) load(userptr, "glTexCoord4f");
    glad_glTexCoord4fv = (PFNGLTEXCOORD4FVPROC) load(userptr, "glTexCoord4fv");
    glad_glTexCoord4i = (PFNGLTEXCOORD4IPROC) load(userptr, "glTexCoord4i");
    glad_glTexCoord4iv = (PFNGLTEXCOORD4IVPROC) load(userptr, "glTexCoord4iv");
    glad_glTexCoord4s = (PFNGLTEXCOORD4SPROC) load(userptr, "glTexCoord4s");
    glad_glTexCoord4sv = (PFNGLTEXCOORD4SVPROC) load(userptr, "glTexCoord4sv");
    glad_glTexEnvf = (PFNGLTEXENVFPROC) load(userptr, "glTexEnvf");
    glad_glTexEnvfv = (PFNGLTEXENVFVPROC) load(userptr, "glTexEnvfv");
    glad_glTexEnvi = (PFNGLTEXENVIPROC) load(userptr, "glTexEnvi");
    glad_glTexEnviv = (PFNGLTEXENVIVPROC) load(userptr, "glTexEnviv");
    glad_glTexGend = (PFNGLTEXGENDPROC) load(userptr, "glTexGend");
    glad_glTexGendv = (PFNGLTEXGENDVPROC) load(userptr, "glTexGendv");
    glad_glTexGenf = (PFNGLTEXGENFPROC) load(userptr, "glTexGenf");
    glad_glTexGenfv = (PFNGLTEXGENFVPROC) load(userptr, "glTexGenfv");
    glad_glTexGeni = (PFNGLTEXGENIPROC) load(userptr, "glTexGeni");
    glad_glTexGeniv = (PFNGLTEXGENIVPROC) load(userptr, "glTexGeniv");
    glad_glTexImage1D = (PFNGLTEXIMAGE1DPROC) load(userptr, "glTexImage1D");
    glad_glTexImage2D = (PFNGLTEXIMAGE2DPROC) load(userptr, "glTexImage2D");
    glad_glTexParameterf = (PFNGLTEXPARAMETERFPROC) load(userptr, "glTexParameterf");
    glad_glTexParameterfv = (PFNGLTEXPARAMETERFVPROC) load(userptr, "glTexParameterfv");
    glad_glTexParameteri = (PFNGLTEXPARAMETERIPROC) load(userptr, "glTexParameteri");
    glad_glTexParameteriv = (PFNGLTEXPARAMETERIVPROC) load(userptr, "glTexParameteriv");
    glad_glTranslated = (PFNGLTRANSLATEDPROC) load(userptr, "glTranslated");
    glad_glTranslatef = (PFNGLTRANSLATEFPROC) load(userptr, "glTranslatef");
    glad_glVertex2d = (PFNGLVERTEX2DPROC) load(userptr, "glVertex2d");
    glad_glVertex2dv = (PFNGLVERTEX2DVPROC) load(userptr, "glVertex2dv");
    glad_glVertex2f = (PFNGLVERTEX2FPROC) load(userptr, "glVertex2f");
    glad_glVertex2fv = (PFNGLVERTEX2FVPROC) load(userptr, "glVertex2fv");
    glad_glVertex2i = (PFNGLVERTEX2IPROC) load(userptr, "glVertex2i");
    glad_glVertex2iv = (PFNGLVERTEX2IVPROC) load(userptr, "glVertex2iv");
    glad_glVertex2s = (PFNGLVERTEX2SPROC) load(userptr, "glVertex2s");
    glad_glVertex2sv = (PFNGLVERTEX2SVPROC) load(userptr, "glVertex2sv");
    glad_glVertex3d = (PFNGLVERTEX3DPROC) load(userptr, "glVertex3d");
    glad_glVertex3dv = (PFNGLVERTEX3DVPROC) load(userptr, "glVertex3dv");
    glad_glVertex3f = (PFNGLVERTEX3FPROC) load(userptr, "glVertex3f");
    glad_glVertex3fv = (PFNGLVERTEX3FVPROC) load(userptr, "glVertex3fv");
    glad_glVertex3i = (PFNGLVERTEX3IPROC) load(userptr, "glVertex3i");
    glad_glVertex3iv = (PFNGLVERTEX3IVPROC) load(userptr, "glVertex3iv");
    glad_glVertex3s = (PFNGLVERTEX3SPROC) load(userptr, "glVertex3s");
    glad_glVertex3sv = (PFNGLVERTEX3SVPROC) load(userptr, "glVertex3sv");
    glad_glVertex4d = (PFNGLVERTEX4DPROC) load(userptr, "glVertex4d");
    glad_glVertex4dv = (PFNGLVERTEX4DVPROC) load(userptr, "glVertex4dv");
    glad_glVertex4f = (PFNGLVERTEX4FPROC) load(userptr, "glVertex4f");
    glad_glVertex4fv = (PFNGLVERTEX4FVPROC) load(userptr, "glVertex4fv");
    glad_glVertex4i = (PFNGLVERTEX4IPROC) load(userptr, "glVertex4i");
    glad_glVertex4iv = (PFNGLVERTEX4IVPROC) load(userptr, "glVertex4iv");
    glad_glVertex4s = (PFNGLVERTEX4SPROC) load(userptr, "glVertex4s");
    glad_glVertex4sv = (PFNGLVERTEX4SVPROC) load(userptr, "glVertex4sv");
    glad_glViewport = (PFNGLVIEWPORTPROC) load(userptr, "glViewport");
}
static void glad_gl_load_GL_VERSION_1_1(GLADuserptrloadfunc load, void * userptr)
{
    if(!GLAD_GL_VERSION_1_1) return;
    glad_glAreTexturesResident = (PFNGLARETEXTURESRESIDENTPROC) load(userptr, "glAreTexturesResident");
    glad_glArrayElement = (PFNGLARRAYELEMENTPROC) load(userptr, "glArrayElement");
    glad_glBindTexture = (PFNGLBINDTEXTUREPROC) load(userptr, "glBindTexture");
    glad_glColorPointer = (PFNGLCOLORPOINTERPROC) load(userptr, "glColorPointer");
    glad_glCopyTexImage1D = (PFNGLCOPYTEXIMAGE1DPROC) load(userptr, "glCopyTexImage1D");
    glad_glCopyTexImage2D = (PFNGLCOPYTEXIMAGE2DPROC) load(userptr, "glCopyTexImage2D");
    glad_glCopyTexSubImage1D = (PFNGLCOPYTEXSUBIMAGE1DPROC) load(userptr, "glCopyTexSubImage1D");
    glad_glCopyTexSubImage2D = (PFNGLCOPYTEXSUBIMAGE2DPROC) load(userptr, "glCopyTexSubImage2D");
    glad_glDeleteTextures = (PFNGLDELETETEXTURESPROC) load(userptr, "glDeleteTextures");
    glad_glDisableClientState = (PFNGLDISABLECLIENTSTATEPROC) load(userptr, "glDisableClientState");
    glad_glDrawArrays = (PFNGLDRAWARRAYSPROC) load(userptr, "glDrawArrays");
    glad_glDrawElements = (PFNGLDRAWELEMENTSPROC) load(userptr, "glDrawElements");
    glad_glEdgeFlagPointer = (PFNGLEDGEFLAGPOINTERPROC) load(userptr, "glEdgeFlagPointer");
    glad_glEnableClientState = (PFNGLENABLECLIENTSTATEPROC) load(userptr, "glEnableClientState");
    glad_glGenTextures = (PFNGLGENTEXTURESPROC) load(userptr, "glGenTextures");
    glad_glGetPointerv = (PFNGLGETPOINTERVPROC) load(userptr, "glGetPointerv");
    glad_glIndexPointer = (PFNGLINDEXPOINTERPROC) load(userptr, "glIndexPointer");
    glad_glIndexub = (PFNGLINDEXUBPROC) load(userptr, "glIndexub");
    glad_glIndexubv = (PFNGLINDEXUBVPROC) load(userptr, "glIndexubv");
    glad_glInterleavedArrays = (PFNGLINTERLEAVEDARRAYSPROC) load(userptr, "glInterleavedArrays");
    glad_glIsTexture = (PFNGLISTEXTUREPROC) load(userptr, "glIsTexture");
    glad_glNormalPointer = (PFNGLNORMALPOINTERPROC) load(userptr, "glNormalPointer");
    glad_glPolygonOffset = (PFNGLPOLYGONOFFSETPROC) load(userptr, "glPolygonOffset");
    glad_glPopClientAttrib = (PFNGLPOPCLIENTATTRIBPROC) load(userptr, "glPopClientAttrib");
    glad_glPrioritizeTextures = (PFNGLPRIORITIZETEXTURESPROC) load(userptr, "glPrioritizeTextures");
    glad_glPushClientAttrib = (PFNGLPUSHCLIENTATTRIBPROC) load(userptr, "glPushClientAttrib");
    glad_glTexCoordPointer = (PFNGLTEXCOORDPOINTERPROC) load(userptr, "glTexCoordPointer");
    glad_glTexSubImage1D = (PFNGLTEXSUBIMAGE1DPROC) load(userptr, "glTexSubImage1D");
    glad_glTexSubImage2D = (PFNGLTEXSUBIMAGE2DPROC) load(userptr, "glTexSubImage2D");
    glad_glVertexPointer = (PFNGLVERTEXPOINTERPROC) load(userptr, "glVertexPointer");
}
static void glad_gl_load_GL_VERSION_1_2(GLADuserptrloadfunc load, void * userptr)
{
    if(!GLAD_GL_VERSION_1_2) return;
    glad_glCopyTexSubImage3D = (PFNGLCOPYTEXSUBIMAGE3DPROC) load(userptr, "glCopyTexSubImage3D");
    glad_glDrawRangeElements = (PFNGLDRAWRANGEELEMENTSPROC) load(userptr, "glDrawRangeElements");
    glad_glTexImage3D = (PFNGLTEXIMAGE3DPROC) load(userptr, "glTexImage3D");
    glad_glTexSubImage3D = (PFNGLTEXSUBIMAGE3DPROC) load(userptr, "glTexSubImage3D");
}
static void glad_gl_load_GL_VERSION_1_3(GLADuserptrloadfunc load, void * userptr)
{
    if(!GLAD_GL_VERSION_1_3) return;
    glad_glActiveTexture = (PFNGLACTIVETEXTUREPROC) load(userptr, "glActiveTexture");
    glad_glClientActiveTexture = (PFNGLCLIENTACTIVETEXTUREPROC) load(userptr, "glClientActiveTexture");
    glad_glCompressedTexImage1D = (PFNGLCOMPRESSEDTEXIMAGE1DPROC) load(userptr, "glCompressedTexImage1D");
    glad_glCompressedTexImage2D = (PFNGLCOMPRESSEDTEXIMAGE2DPROC) load(userptr, "glCompressedTexImage2D");
    glad_glCompressedTexImage3D = (PFNGLCOMPRESSEDTEXIMAGE3DPROC) load(userptr, "glCompressedTexImage3D");
    glad_glCompressedTexSubImage1D = (PFNGLCOMPRESSEDTEXSUBIMAGE1DPROC) load(userptr, "glCompressedTexSubImage1D");
    glad_glCompressedTexSubImage2D = (PFNGLCOMPRESSEDTEXSUBIMAGE2DPROC) load(userptr, "glCompressedTexSubImage2D");
    glad_glCompressedTexSubImage3D = (PFNGLCOMPRESSEDTEXSUBIMAGE3DPROC) load(userptr, "glCompressedTexSubImage3D");
    glad_glGetCompressedTexImage = (PFNGLGETCOMPRESSEDTEXIMAGEPROC) load(userptr, "glGetCompressedTexImage");
    glad_glLoadTransposeMatrixd = (PFNGLLOADTRANSPOSEMATRIXDPROC) load(userptr, "glLoadTransposeMatrixd");
    glad_glLoadTransposeMatrixf = (PFNGLLOADTRANSPOSEMATRIXFPROC) load(userptr, "glLoadTransposeMatrixf");
    glad_glMultTransposeMatrixd = (PFNGLMULTTRANSPOSEMATRIXDPROC) load(userptr, "glMultTransposeMatrixd");
    glad_glMultTransposeMatrixf = (PFNGLMULTTRANSPOSEMATRIXFPROC) load(userptr, "glMultTransposeMatrixf");
    glad_glMultiTexCoord1d = (PFNGLMULTITEXCOORD1DPROC) load(userptr, "glMultiTexCoord1d");
    glad_glMultiTexCoord1dv = (PFNGLMULTITEXCOORD1DVPROC) load(userptr, "glMultiTexCoord1dv");
    glad_glMultiTexCoord1f = (PFNGLMULTITEXCOORD1FPROC) load(userptr, "glMultiTexCoord1f");
    glad_glMultiTexCoord1fv = (PFNGLMULTITEXCOORD1FVPROC) load(userptr, "glMultiTexCoord1fv");
    glad_glMultiTexCoord1i = (PFNGLMULTITEXCOORD1IPROC) load(userptr, "glMultiTexCoord1i");
    glad_glMultiTexCoord1iv = (PFNGLMULTITEXCOORD1IVPROC) load(userptr, "glMultiTexCoord1iv");
    glad_glMultiTexCoord1s = (PFNGLMULTITEXCOORD1SPROC) load(userptr, "glMultiTexCoord1s");
    glad_glMultiTexCoord1sv = (PFNGLMULTITEXCOORD1SVPROC) load(userptr, "glMultiTexCoord1sv");
    glad_glMultiTexCoord2d = (PFNGLMULTITEXCOORD2DPROC) load(userptr, "glMultiTexCoord2d");
    glad_glMultiTexCoord2dv = (PFNGLMULTITEXCOORD2DVPROC) load(userptr, "glMultiTexCoord2dv");
    glad_glMultiTexCoord2f = (PFNGLMULTITEXCOORD2FPROC) load(userptr, "glMultiTexCoord2f");
    glad_glMultiTexCoord2fv = (PFNGLMULTITEXCOORD2FVPROC) load(userptr, "glMultiTexCoord2fv");
    glad_glMultiTexCoord2i = (PFNGLMULTITEXCOORD2IPROC) load(userptr, "glMultiTexCoord2i");
    glad_glMultiTexCoord2iv = (PFNGLMULTITEXCOORD2IVPROC) load(userptr, "glMultiTexCoord2iv");
    glad_glMultiTexCoord2s = (PFNGLMULTITEXCOORD2SPROC) load(userptr, "glMultiTexCoord2s");
    glad_glMultiTexCoord2sv = (PFNGLMULTITEXCOORD2SVPROC) load(userptr, "glMultiTexCoord2sv");
    glad_glMultiTexCoord3d = (PFNGLMULTITEXCOORD3DPROC) load(userptr, "glMultiTexCoord3d");
    glad_glMultiTexCoord3dv = (PFNGLMULTITEXCOORD3DVPROC) load(userptr, "glMultiTexCoord3dv");
    glad_glMultiTexCoord3f = (PFNGLMULTITEXCOORD3FPROC) load(userptr, "glMultiTexCoord3f");
    glad_glMultiTexCoord3fv = (PFNGLMULTITEXCOORD3FVPROC) load(userptr, "glMultiTexCoord3fv");
    glad_glMultiTexCoord3i = (PFNGLMULTITEXCOORD3IPROC) load(userptr, "glMultiTexCoord3i");
    glad_glMultiTexCoord3iv = (PFNGLMULTITEXCOORD3IVPROC) load(userptr, "glMultiTexCoord3iv");
    glad_glMultiTexCoord3s = (PFNGLMULTITEXCOORD3SPROC) load(userptr, "glMultiTexCoord3s");
    glad_glMultiTexCoord3sv = (PFNGLMULTITEXCOORD3SVPROC) load(userptr, "glMultiTexCoord3sv");
    glad_glMultiTexCoord4d = (PFNGLMULTITEXCOORD4DPROC) load(userptr, "glMultiTexCoord4d");
    glad_glMultiTexCoord4dv = (PFNGLMULTITEXCOORD4DVPROC) load(userptr, "glMultiTexCoord4dv");
    glad_glMultiTexCoord4f = (PFNGLMULTITEXCOORD4FPROC) load(userptr, "glMultiTexCoord4f");
    glad_glMultiTexCoord4fv = (PFNGLMULTITEXCOORD4FVPROC) load(userptr, "glMultiTexCoord4fv");
    glad_glMultiTexCoord4i = (PFNGLMULTITEXCOORD4IPROC) load(userptr, "glMultiTexCoord4i");
    glad_glMultiTexCoord4iv = (PFNGLMULTITEXCOORD4IVPROC) load(userptr, "glMultiTexCoord4iv");
    glad_glMultiTexCoord4s = (PFNGLMULTITEXCOORD4SPROC) load(userptr, "glMultiTexCoord4s");
    glad_glMultiTexCoord4sv = (PFNGLMULTITEXCOORD4SVPROC) load(userptr, "glMultiTexCoord4sv");
    glad_glSampleCoverage = (PFNGLSAMPLECOVERAGEPROC) load(userptr, "glSampleCoverage");
}
static void glad_gl_load_GL_VERSION_1_4(GLADuserptrloadfunc load, void * userptr)
{
    if(!GLAD_GL_VERSION_1_4) return;
    glad_glBlendColor = (PFNGLBLENDCOLORPROC) load(userptr, "glBlendColor");
    glad_glBlendEquation = (PFNGLBLENDEQUATIONPROC) load(userptr, "glBlendEquation");
    glad_glBlendFuncSeparate = (PFNGLBLENDFUNCSEPARATEPROC) load(userptr, "glBlendFuncSeparate");
    glad_glFogCoordPointer = (PFNGLFOGCOORDPOINTERPROC) load(userptr, "glFogCoordPointer");
    glad_glFogCoordd = (PFNGLFOGCOORDDPROC) load(userptr, "glFogCoordd");
    glad_glFogCoorddv = (PFNGLFOGCOORDDVPROC) load(userptr, "glFogCoorddv");
    glad_glFogCoordf = (PFNGLFOGCOORDFPROC) load(userptr, "glFogCoordf");
    glad_glFogCoordfv = (PFNGLFOGCOORDFVPROC) load(userptr, "glFogCoordfv");
    glad_glMultiDrawArrays = (PFNGLMULTIDRAWARRAYSPROC) load(userptr, "glMultiDrawArrays");
    glad_glMultiDrawElements = (PFNGLMULTIDRAWELEMENTSPROC) load(userptr, "glMultiDrawElements");
    glad_glPointParameterf = (PFNGLPOINTPARAMETERFPROC) load(userptr, "glPointParameterf");
    glad_glPointParameterfv = (PFNGLPOINTPARAMETERFVPROC) load(userptr, "glPointParameterfv");
    glad_glPointParameteri = (PFNGLPOINTPARAMETERIPROC) load(userptr, "glPointParameteri");
    glad_glPointParameteriv = (PFNGLPOINTPARAMETERIVPROC) load(userptr, "glPointParameteriv");
    glad_glSecondaryColor3b = (PFNGLSECONDARYCOLOR3BPROC) load(userptr, "glSecondaryColor3b");
    glad_glSecondaryColor3bv = (PFNGLSECONDARYCOLOR3BVPROC) load(userptr, "glSecondaryColor3bv");
    glad_glSecondaryColor3d = (PFNGLSECONDARYCOLOR3DPROC) load(userptr, "glSecondaryColor3d");
    glad_glSecondaryColor3dv = (PFNGLSECONDARYCOLOR3DVPROC) load(userptr, "glSecondaryColor3dv");
    glad_glSecondaryColor3f = (PFNGLSECONDARYCOLOR3FPROC) load(userptr, "glSecondaryColor3f");
    glad_glSecondaryColor3fv = (PFNGLSECONDARYCOLOR3FVPROC) load(userptr, "glSecondaryColor3fv");
    glad_glSecondaryColor3i = (PFNGLSECONDARYCOLOR3IPROC) load(userptr, "glSecondaryColor3i");
    glad_glSecondaryColor3iv = (PFNGLSECONDARYCOLOR3IVPROC) load(userptr, "glSecondaryColor3iv");
    glad_glSecondaryColor3s = (PFNGLSECONDARYCOLOR3SPROC) load(userptr, "glSecondaryColor3s");
    glad_glSecondaryColor3sv = (PFNGLSECONDARYCOLOR3SVPROC) load(userptr, "glSecondaryColor3sv");
    glad_glSecondaryColor3ub = (PFNGLSECONDARYCOLOR3UBPROC) load(userptr, "glSecondaryColor3ub");
    glad_glSecondaryColor3ubv = (PFNGLSECONDARYCOLOR3UBVPROC) load(userptr, "glSecondaryColor3ubv");
    glad_glSecondaryColor3ui = (PFNGLSECONDARYCOLOR3UIPROC) load(userptr, "glSecondaryColor3ui");
    glad_glSecondaryColor3uiv = (PFNGLSECONDARYCOLOR3UIVPROC) load(userptr, "glSecondaryColor3uiv");
    glad_glSecondaryColor3us = (PFNGLSECONDARYCOLOR3USPROC) load(userptr, "glSecondaryColor3us");
    glad_glSecondaryColor3usv = (PFNGLSECONDARYCOLOR3USVPROC) load(userptr, "glSecondaryColor3usv");
    glad_glSecondaryColorPointer = (PFNGLSECONDARYCOLORPOINTERPROC) load(userptr, "glSecondaryColorPointer");
    glad_glWindowPos2d = (PFNGLWINDOWPOS2DPROC) load(userptr, "glWindowPos2d");
    glad_glWindowPos2dv = (PFNGLWINDOWPOS2DVPROC) load(userptr, "glWindowPos2dv");
    glad_glWindowPos2f = (PFNGLWINDOWPOS2FPROC) load(userptr, "glWindowPos2f");
    glad_glWindowPos2fv = (PFNGLWINDOWPOS2FVPROC) load(userptr, "glWindowPos2fv");
    glad_glWindowPos2i = (PFNGLWINDOWPOS2IPROC) load(userptr, "glWindowPos2i");
    glad_glWindowPos2iv = (PFNGLWINDOWPOS2IVPROC) load(userptr, "glWindowPos2iv");
    glad_glWindowPos2s = (PFNGLWINDOWPOS2SPROC) load(userptr, "glWindowPos2s");
    glad_glWindowPos2sv = (PFNGLWINDOWPOS2SVPROC) load(userptr, "glWindowPos2sv");
    glad_glWindowPos3d = (PFNGLWINDOWPOS3DPROC) load(userptr, "glWindowPos3d");
    glad_glWindowPos3dv = (PFNGLWINDOWPOS3DVPROC) load(userptr, "glWindowPos3dv");
    glad_glWindowPos3f = (PFNGLWINDOWPOS3FPROC) load(userptr, "glWindowPos3f");
    glad_glWindowPos3fv = (PFNGLWINDOWPOS3FVPROC) load(userptr, "glWindowPos3fv");
    glad_glWindowPos3i = (PFNGLWINDOWPOS3IPROC) load(userptr, "glWindowPos3i");
    glad_glWindowPos3iv = (PFNGLWINDOWPOS3IVPROC) load(userptr, "glWindowPos3iv");
    glad_glWindowPos3s = (PFNGLWINDOWPOS3SPROC) load(userptr, "glWindowPos3s");
    glad_glWindowPos3sv = (PFNGLWINDOWPOS3SVPROC) load(userptr, "glWindowPos3sv");
}
static void glad_gl_load_GL_VERSION_1_5(GLADuserptrloadfunc load, void * userptr)
{
    if(!GLAD_GL_VERSION_1_5) return;
    glad_glBeginQuery = (PFNGLBEGINQUERYPROC) load(userptr, "glBeginQuery");
    glad_glBindBuffer = (PFNGLBINDBUFFERPROC) load(userptr, "glBindBuffer");
    glad_glBufferData = (PFNGLBUFFERDATAPROC) load(userptr, "glBufferData");
    glad_glBufferSubData = (PFNGLBUFFERSUBDATAPROC) load(userptr, "glBufferSubData");
    glad_glDeleteBuffers = (PFNGLDELETEBUFFERSPROC) load(userptr, "glDeleteBuffers");
    glad_glDeleteQueries = (PFNGLDELETEQUERIESPROC) load(userptr, "glDeleteQueries");
    glad_glEndQuery = (PFNGLENDQUERYPROC) load(userptr, "glEndQuery");
    glad_glGenBuffers = (PFNGLGENBUFFERSPROC) load(userptr, "glGenBuffers");
    glad_glGenQueries = (PFNGLGENQUERIESPROC) load(userptr, "glGenQueries");
    glad_glGetBufferParameteriv = (PFNGLGETBUFFERPARAMETERIVPROC) load(userptr, "glGetBufferParameteriv");
    glad_glGetBufferPointerv = (PFNGLGETBUFFERPOINTERVPROC) load(userptr, "glGetBufferPointerv");
    glad_glGetBufferSubData = (PFNGLGETBUFFERSUBDATAPROC) load(userptr, "glGetBufferSubData");
    glad_glGetQueryObjectiv = (PFNGLGETQUERYOBJECTIVPROC) load(userptr, "glGetQueryObjectiv");
    glad_glGetQueryObjectuiv = (PFNGLGETQUERYOBJECTUIVPROC) load(userptr, "glGetQueryObjectuiv");
    glad_glGetQueryiv = (PFNGLGETQUERYIVPROC) load(userptr, "glGetQueryiv");
    glad_glIsBuffer = (PFNGLISBUFFERPROC) load(userptr, "glIsBuffer");
    glad_glIsQuery = (PFNGLISQUERYPROC) load(userptr, "glIsQuery");
    glad_glMapBuffer = (PFNGLMAPBUFFERPROC) load(userptr, "glMapBuffer");
    glad_glUnmapBuffer = (PFNGLUNMAPBUFFERPROC) load(userptr, "glUnmapBuffer");
}
static void glad_gl_load_GL_VERSION_2_0(GLADuserptrloadfunc load, void * userptr)
{
    if(!GLAD_GL_VERSION_2_0) return;
    glad_glAttachShader = (PFNGLATTACHSHADERPROC) load(userptr, "glAttachShader");
    glad_glBindAttribLocation = (PFNGLBINDATTRIBLOCATIONPROC) load(userptr, "glBindAttribLocation");
    glad_glBlendEquationSeparate = (PFNGLBLENDEQUATIONSEPARATEPROC) load(userptr, "glBlendEquationSeparate");
    glad_glCompileShader = (PFNGLCOMPILESHADERPROC) load(userptr, "glCompileShader");
    glad_glCreateProgram = (PFNGLCREATEPROGRAMPROC) load(userptr, "glCreateProgram");
    glad_glCreateShader = (PFNGLCREATESHADERPROC) load(userptr, "glCreateShader");
    glad_glDeleteProgram = (PFNGLDELETEPROGRAMPROC) load(userptr, "glDeleteProgram");
    glad_glDeleteShader = (PFNGLDELETESHADERPROC) load(userptr, "glDeleteShader");
    glad_glDetachShader = (PFNGLDETACHSHADERPROC) load(userptr, "glDetachShader");
    glad_glDisableVertexAttribArray = (PFNGLDISABLEVERTEXATTRIBARRAYPROC) load(userptr, "glDisableVertexAttribArray");
    glad_glDrawBuffers = (PFNGLDRAWBUFFERSPROC) load(userptr, "glDrawBuffers");
    glad_glEnableVertexAttribArray = (PFNGLENABLEVERTEXATTRIBARRAYPROC) load(userptr, "glEnableVertexAttribArray");
    glad_glGetActiveAttrib = (PFNGLGETACTIVEATTRIBPROC) load(userptr, "glGetActiveAttrib");
    glad_glGetActiveUniform = (PFNGLGETACTIVEUNIFORMPROC) load(userptr, "glGetActiveUniform");
    glad_glGetAttachedShaders = (PFNGLGETATTACHEDSHADERSPROC) load(userptr, "glGetAttachedShaders");
    glad_glGetAttribLocation = (PFNGLGETATTRIBLOCATIONPROC) load(userptr, "glGetAttribLocation");
    glad_glGetProgramInfoLog = (PFNGLGETPROGRAMINFOLOGPROC) load(userptr, "glGetProgramInfoLog");
    glad_glGetProgramiv = (PFNGLGETPROGRAMIVPROC) load(userptr, "glGetProgramiv");
    glad_glGetShaderInfoLog = (PFNGLGETSHADERINFOLOGPROC) load(userptr, "glGetShaderInfoLog");
    glad_glGetShaderSource = (PFNGLGETSHADERSOURCEPROC) load(userptr, "glGetShaderSource");
    glad_glGetShaderiv = (PFNGLGETSHADERIVPROC) load(userptr, "glGetShaderiv");
    glad_glGetUniformLocation = (PFNGLGETUNIFORMLOCATIONPROC) load(userptr, "glGetUniformLocation");
    glad_glGetUniformfv = (PFNGLGETUNIFORMFVPROC) load(userptr, "glGetUniformfv");
    glad_glGetUniformiv = (PFNGLGETUNIFORMIVPROC) load(userptr, "glGetUniformiv");
    glad_glGetVertexAttribPointerv = (PFNGLGETVERTEXATTRIBPOINTERVPROC) load(userptr, "glGetVertexAttribPointerv");
    glad_glGetVertexAttribdv = (PFNGLGETVERTEXATTRIBDVPROC) load(userptr, "glGetVertexAttribdv");
    glad_glGetVertexAttribfv = (PFNGLGETVERTEXATTRIBFVPROC) load(userptr, "glGetVertexAttribfv");
    glad_glGetVertexAttribiv = (PFNGLGETVERTEXATTRIBIVPROC) load(userptr, "glGetVertexAttribiv");
    glad_glIsProgram = (PFNGLISPROGRAMPROC) load(userptr, "glIsProgram");
    glad_glIsShader = (PFNGLISSHADERPROC) load(userptr, "glIsShader");
    glad_glLinkProgram = (PFNGLLINKPROGRAMPROC) load(userptr, "glLinkProgram");
    glad_glShaderSource = (PFNGLSHADERSOURCEPROC) load(userptr, "glShaderSource");
    glad_glStencilFuncSeparate = (PFNGLSTENCILFUNCSEPARATEPROC) load(userptr, "glStencilFuncSeparate");
    glad_glStencilMaskSeparate = (PFNGLSTENCILMASKSEPARATEPROC) load(userptr, "glStencilMaskSeparate");
    glad_glStencilOpSeparate = (PFNGLSTENCILOPSEPARATEPROC) load(userptr, "glStencilOpSeparate");
    glad_glUniform1f = (PFNGLUNIFORM1FPROC) load(userptr, "glUniform1f");
    glad_glUniform1fv = (PFNGLUNIFORM1FVPROC) load(userptr, "glUniform1fv");
    glad_glUniform1i = (PFNGLUNIFORM1IPROC) load(userptr, "glUniform1i");
    glad_glUniform1iv = (PFNGLUNIFORM1IVPROC) load(userptr, "glUniform1iv");
    glad_glUniform2f = (PFNGLUNIFORM2FPROC) load(userptr, "glUniform2f");
    glad_glUniform2fv = (PFNGLUNIFORM2FVPROC) load(userptr, "glUniform2fv");
    glad_glUniform2i = (PFNGLUNIFORM2IPROC) load(userptr, "glUniform2i");
    glad_glUniform2iv = (PFNGLUNIFORM2IVPROC) load(userptr, "glUniform2iv");
    glad_glUniform3f = (PFNGLUNIFORM3FPROC) load(userptr, "glUniform3f");
    glad_glUniform3fv = (PFNGLUNIFORM3FVPROC) load(userptr, "glUniform3fv");
    glad_glUniform3i = (PFNGLUNIFORM3IPROC) load(userptr, "glUniform3i");
    glad_glUniform3iv = (PFNGLUNIFORM3IVPROC) load(userptr, "glUniform3iv");
    glad_glUniform4f = (PFNGLUNIFORM4FPROC) load(userptr, "glUniform4f");
    glad_glUniform4fv = (PFNGLUNIFORM4FVPROC) load(userptr, "glUniform4fv");
    glad_glUniform4i = (PFNGLUNIFORM4IPROC) load(userptr, "glUniform4i");
    glad_glUniform4iv = (PFNGLUNIFORM4IVPROC) load(userptr, "glUniform4iv");
    glad_glUniformMatrix2fv = (PFNGLUNIFORMMATRIX2FVPROC) load(userptr, "glUniformMatrix2fv");
    glad_glUniformMatrix3fv = (PFNGLUNIFORMMATRIX3FVPROC) load(userptr, "glUniformMatrix3fv");
    glad_glUniformMatrix4fv = (PFNGLUNIFORMMATRIX4FVPROC) load(userptr, "glUniformMatrix4fv");
    glad_glUseProgram = (PFNGLUSEPROGRAMPROC) load(userptr, "glUseProgram");
    glad_glValidateProgram = (PFNGLVALIDATEPROGRAMPROC) load(userptr, "glValidateProgram");
    glad_glVertexAttrib1d = (PFNGLVERTEXATTRIB1DPROC) load(userptr, "glVertexAttrib1d");
    glad_glVertexAttrib1dv = (PFNGLVERTEXATTRIB1DVPROC) load(userptr, "glVertexAttrib1dv");
    glad_glVertexAttrib1f = (PFNGLVERTEXATTRIB1FPROC) load(userptr, "glVertexAttrib1f");
    glad_glVertexAttrib1fv = (PFNGLVERTEXATTRIB1FVPROC) load(userptr, "glVertexAttrib1fv");
    glad_glVertexAttrib1s = (PFNGLVERTEXATTRIB1SPROC) load(userptr, "glVertexAttrib1s");
    glad_glVertexAttrib1sv = (PFNGLVERTEXATTRIB1SVPROC) load(userptr, "glVertexAttrib1sv");
    glad_glVertexAttrib2d = (PFNGLVERTEXATTRIB2DPROC) load(userptr, "glVertexAttrib2d");
    glad_glVertexAttrib2dv = (PFNGLVERTEXATTRIB2DVPROC) load(userptr, "glVertexAttrib2dv");
    glad_glVertexAttrib2f = (PFNGLVERTEXATTRIB2FPROC) load(userptr, "glVertexAttrib2f");
    glad_glVertexAttrib2fv = (PFNGLVERTEXATTRIB2FVPROC) load(userptr, "glVertexAttrib2fv");
    glad_glVertexAttrib2s = (PFNGLVERTEXATTRIB2SPROC) load(userptr, "glVertexAttrib2s");
    glad_glVertexAttrib2sv = (PFNGLVERTEXATTRIB2SVPROC) load(userptr, "glVertexAttrib2sv");
    glad_glVertexAttrib3d = (PFNGLVERTEXATTRIB3DPROC) load(userptr, "glVertexAttrib3d");
    glad_glVertexAttrib3dv = (PFNGLVERTEXATTRIB3DVPROC) load(userptr, "glVertexAttrib3dv");
    glad_glVertexAttrib3f = (PFNGLVERTEXATTRIB3FPROC) load(userptr, "glVertexAttrib3f");
    glad_glVertexAttrib3fv = (PFNGLVERTEXATTRIB3FVPROC) load(userptr, "glVertexAttrib3fv");
    glad_glVertexAttrib3s = (PFNGLVERTEXATTRIB3SPROC) load(userptr, "glVertexAttrib3s");
    glad_glVertexAttrib3sv = (PFNGLVERTEXATTRIB3SVPROC) load(userptr, "glVertexAttrib3sv");
    glad_glVertexAttrib4Nbv = (PFNGLVERTEXATTRIB4NBVPROC) load(userptr, "glVertexAttrib4Nbv");
    glad_glVertexAttrib4Niv = (PFNGLVERTEXATTRIB4NIVPROC) load(userptr, "glVertexAttrib4Niv");
    glad_glVertexAttrib4Nsv = (PFNGLVERTEXATTRIB4NSVPROC) load(userptr, "glVertexAttrib4Nsv");
    glad_glVertexAttrib4Nub = (PFNGLVERTEXATTRIB4NUBPROC) load(userptr, "glVertexAttrib4Nub");
    glad_glVertexAttrib4Nubv = (PFNGLVERTEXATTRIB4NUBVPROC) load(userptr, "glVertexAttrib4Nubv");
    glad_glVertexAttrib4Nuiv = (PFNGLVERTEXATTRIB4NUIVPROC) load(userptr, "glVertexAttrib4Nuiv");
    glad_glVertexAttrib4Nusv = (PFNGLVERTEXATTRIB4NUSVPROC) load(userptr, "glVertexAttrib4Nusv");
    glad_glVertexAttrib4bv = (PFNGLVERTEXATTRIB4BVPROC) load(userptr, "glVertexAttrib4bv");
    glad_glVertexAttrib4d = (PFNGLVERTEXATTRIB4DPROC) load(userptr, "glVertexAttrib4d");
    glad_glVertexAttrib4dv = (PFNGLVERTEXATTRIB4DVPROC) load(userptr, "glVertexAttrib4dv");
    glad_glVertexAttrib4f = (PFNGLVERTEXATTRIB4FPROC) load(userptr, "glVertexAttrib4f");
    glad_glVertexAttrib4fv = (PFNGLVERTEXATTRIB4FVPROC) load(userptr, "glVertexAttrib4fv");
    glad_glVertexAttrib4iv = (PFNGLVERTEXATTRIB4IVPROC) load(userptr, "glVertexAttrib4iv");
    glad_glVertexAttrib4s = (PFNGLVERTEXATTRIB4SPROC) load(userptr, "glVertexAttrib4s");
    glad_glVertexAttrib4sv = (PFNGLVERTEXATTRIB4SVPROC) load(userptr, "glVertexAttrib4sv");
    glad_glVertexAttrib4ubv = (PFNGLVERTEXATTRIB4UBVPROC) load(userptr, "glVertexAttrib4ubv");
    glad_glVertexAttrib4uiv = (PFNGLVERTEXATTRIB4UIVPROC) load(userptr, "glVertexAttrib4uiv");
    glad_glVertexAttrib4usv = (PFNGLVERTEXATTRIB4USVPROC) load(userptr, "glVertexAttrib4usv");
    glad_glVertexAttribPointer = (PFNGLVERTEXATTRIBPOINTERPROC) load(userptr, "glVertexAttribPointer");
}
static void glad_gl_load_GL_VERSION_2_1(GLADuserptrloadfunc load, void * userptr)
{
    if(!GLAD_GL_VERSION_2_1) return;
    glad_glUniformMatrix2x3fv = (PFNGLUNIFORMMATRIX2X3FVPROC) load(userptr, "glUniformMatrix2x3fv");
    glad_glUniformMatrix2x4fv = (PFNGLUNIFORMMATRIX2X4FVPROC) load(userptr, "glUniformMatrix2x4fv");
    glad_glUniformMatrix3x2fv = (PFNGLUNIFORMMATRIX3X2FVPROC) load(userptr, "glUniformMatrix3x2fv");
    glad_glUniformMatrix3x4fv = (PFNGLUNIFORMMATRIX3X4FVPROC) load(userptr, "glUniformMatrix3x4fv");
    glad_glUniformMatrix4x2fv = (PFNGLUNIFORMMATRIX4X2FVPROC) load(userptr, "glUniformMatrix4x2fv");
    glad_glUniformMatrix4x3fv = (PFNGLUNIFORMMATRIX4X3FVPROC) load(userptr, "glUniformMatrix4x3fv");
}
static void glad_gl_load_GL_VERSION_3_0(GLADuserptrloadfunc load, void * userptr)
{
    if(!GLAD_GL_VERSION_3_0) return;
    /*
    PFNGLCOLORMASKIPROC glad_glColorMaski = NULL;
    PFNGLGETBOOLEANI_VPROC glad_glGetBooleani_v = NULL;
    PFNGLGETINTEGERI_VPROC glad_glGetIntegeri_v = NULL;
    PFNGLENABLEIPROC glad_glEnablei = NULL;
    PFNGLDISABLEIPROC glad_glDisablei = NULL;
    PFNGLISENABLEDIPROC glad_glIsEnabledi = NULL;
    PFNGLBEGINTRANSFORMFEEDBACKPROC glad_glBeginTransformFeedback = NULL;
    PFNGLENDTRANSFORMFEEDBACKPROC glad_glEndTransformFeedback = NULL;
    PFNGLBINDBUFFERRANGEPROC glad_glBindBufferRange = NULL;
    PFNGLBINDBUFFERBASEPROC glad_glBindBufferBase = NULL;

    glad_glColorMaski = (PFNGLCOLORMASKIPROC)load("glColorMaski");
    glad_glGetBooleani_v = (PFNGLGETBOOLEANI_VPROC)load("glGetBooleani_v");
    glad_glGetIntegeri_v = (PFNGLGETINTEGERI_VPROC)load("glGetIntegeri_v");
    glad_glEnablei = (PFNGLENABLEIPROC)load("glEnablei");
    glad_glDisablei = (PFNGLDISABLEIPROC)load("glDisablei");
    glad_glIsEnabledi = (PFNGLISENABLEDIPROC)load("glIsEnabledi");
    glad_glBeginTransformFeedback = (PFNGLBEGINTRANSFORMFEEDBACKPROC)load("glBeginTransformFeedback");
    glad_glEndTransformFeedback = (PFNGLENDTRANSFORMFEEDBACKPROC)load("glEndTransformFeedback");
    glad_glBindBufferRange = (PFNGLBINDBUFFERRANGEPROC)load("glBindBufferRange");
    glad_glBindBufferBase = (PFNGLBINDBUFFERBASEPROC)load("glBindBufferBase");

    glad_glTransformFeedbackVaryings = (PFNGLTRANSFORMFEEDBACKVARYINGSPROC)load("glTransformFeedbackVaryings");
    glad_glGetTransformFeedbackVarying = (PFNGLGETTRANSFORMFEEDBACKVARYINGPROC)load("glGetTransformFeedbackVarying");
    glad_glClampColor = (PFNGLCLAMPCOLORPROC)load("glClampColor");
    glad_glBeginConditionalRender = (PFNGLBEGINCONDITIONALRENDERPROC)load("glBeginConditionalRender");
    glad_glEndConditionalRender = (PFNGLENDCONDITIONALRENDERPROC)load("glEndConditionalRender");
    glad_glVertexAttribIPointer = (PFNGLVERTEXATTRIBIPOINTERPROC)load("glVertexAttribIPointer");
    glad_glGetVertexAttribIiv = (PFNGLGETVERTEXATTRIBIIVPROC)load("glGetVertexAttribIiv");
    glad_glGetVertexAttribIuiv = (PFNGLGETVERTEXATTRIBIUIVPROC)load("glGetVertexAttribIuiv");
    glad_glVertexAttribI1i = (PFNGLVERTEXATTRIBI1IPROC)load("glVertexAttribI1i");
    glad_glVertexAttribI2i = (PFNGLVERTEXATTRIBI2IPROC)load("glVertexAttribI2i");
    glad_glVertexAttribI3i = (PFNGLVERTEXATTRIBI3IPROC)load("glVertexAttribI3i");
    glad_glVertexAttribI4i = (PFNGLVERTEXATTRIBI4IPROC)load("glVertexAttribI4i");
    glad_glVertexAttribI1ui = (PFNGLVERTEXATTRIBI1UIPROC)load("glVertexAttribI1ui");
    glad_glVertexAttribI2ui = (PFNGLVERTEXATTRIBI2UIPROC)load("glVertexAttribI2ui");
    glad_glVertexAttribI3ui = (PFNGLVERTEXATTRIBI3UIPROC)load("glVertexAttribI3ui");
    glad_glVertexAttribI4ui = (PFNGLVERTEXATTRIBI4UIPROC)load("glVertexAttribI4ui");
    glad_glVertexAttribI1iv = (PFNGLVERTEXATTRIBI1IVPROC)load("glVertexAttribI1iv");
    glad_glVertexAttribI2iv = (PFNGLVERTEXATTRIBI2IVPROC)load("glVertexAttribI2iv");
    glad_glVertexAttribI3iv = (PFNGLVERTEXATTRIBI3IVPROC)load("glVertexAttribI3iv");
    glad_glVertexAttribI4iv = (PFNGLVERTEXATTRIBI4IVPROC)load("glVertexAttribI4iv");
    glad_glVertexAttribI1uiv = (PFNGLVERTEXATTRIBI1UIVPROC)load("glVertexAttribI1uiv");
    glad_glVertexAttribI2uiv = (PFNGLVERTEXATTRIBI2UIVPROC)load("glVertexAttribI2uiv");
    glad_glVertexAttribI3uiv = (PFNGLVERTEXATTRIBI3UIVPROC)load("glVertexAttribI3uiv");
    glad_glVertexAttribI4uiv = (PFNGLVERTEXATTRIBI4UIVPROC)load("glVertexAttribI4uiv");
    glad_glVertexAttribI4bv = (PFNGLVERTEXATTRIBI4BVPROC)load("glVertexAttribI4bv");
    glad_glVertexAttribI4sv = (PFNGLVERTEXATTRIBI4SVPROC)load("glVertexAttribI4sv");
    glad_glVertexAttribI4ubv = (PFNGLVERTEXATTRIBI4UBVPROC)load("glVertexAttribI4ubv");
    glad_glVertexAttribI4usv = (PFNGLVERTEXATTRIBI4USVPROC)load("glVertexAttribI4usv");
    glad_glGetUniformuiv = (PFNGLGETUNIFORMUIVPROC)load("glGetUniformuiv");
    glad_glBindFragDataLocation = (PFNGLBINDFRAGDATALOCATIONPROC)load("glBindFragDataLocation");
    glad_glGetFragDataLocation = (PFNGLGETFRAGDATALOCATIONPROC)load("glGetFragDataLocation");
    glad_glUniform1ui = (PFNGLUNIFORM1UIPROC)load("glUniform1ui");
    glad_glUniform2ui = (PFNGLUNIFORM2UIPROC)load("glUniform2ui");
    glad_glUniform3ui = (PFNGLUNIFORM3UIPROC)load("glUniform3ui");
    glad_glUniform4ui = (PFNGLUNIFORM4UIPROC)load("glUniform4ui");
    glad_glUniform1uiv = (PFNGLUNIFORM1UIVPROC)load("glUniform1uiv");
    glad_glUniform2uiv = (PFNGLUNIFORM2UIVPROC)load("glUniform2uiv");
    glad_glUniform3uiv = (PFNGLUNIFORM3UIVPROC)load("glUniform3uiv");
    glad_glUniform4uiv = (PFNGLUNIFORM4UIVPROC)load("glUniform4uiv");
    glad_glTexParameterIiv = (PFNGLTEXPARAMETERIIVPROC)load("glTexParameterIiv");
    glad_glTexParameterIuiv = (PFNGLTEXPARAMETERIUIVPROC)load("glTexParameterIuiv");
    glad_glGetTexParameterIiv = (PFNGLGETTEXPARAMETERIIVPROC)load("glGetTexParameterIiv");
    glad_glGetTexParameterIuiv = (PFNGLGETTEXPARAMETERIUIVPROC)load("glGetTexParameterIuiv");
    glad_glClearBufferiv = (PFNGLCLEARBUFFERIVPROC)load("glClearBufferiv");
    glad_glClearBufferuiv = (PFNGLCLEARBUFFERUIVPROC)load("glClearBufferuiv");
    glad_glClearBufferfv = (PFNGLCLEARBUFFERFVPROC)load("glClearBufferfv");
    glad_glClearBufferfi = (PFNGLCLEARBUFFERFIPROC)load("glClearBufferfi");
    glad_glGetStringi = (PFNGLGETSTRINGIPROC)load("glGetStringi");
    glad_glIsRenderbuffer = (PFNGLISRENDERBUFFERPROC)load("glIsRenderbuffer");
    glad_glBindRenderbuffer = (PFNGLBINDRENDERBUFFERPROC)load("glBindRenderbuffer");
    glad_glDeleteRenderbuffers = (PFNGLDELETERENDERBUFFERSPROC)load("glDeleteRenderbuffers");
    glad_glGenRenderbuffers = (PFNGLGENRENDERBUFFERSPROC)load("glGenRenderbuffers");
    glad_glRenderbufferStorage = (PFNGLRENDERBUFFERSTORAGEPROC)load("glRenderbufferStorage");
    glad_glGetRenderbufferParameteriv = (PFNGLGETRENDERBUFFERPARAMETERIVPROC)load("glGetRenderbufferParameteriv");
    glad_glIsFramebuffer = (PFNGLISFRAMEBUFFERPROC)load("glIsFramebuffer");
    glad_glBindFramebuffer = (PFNGLBINDFRAMEBUFFERPROC)load("glBindFramebuffer");
    glad_glDeleteFramebuffers = (PFNGLDELETEFRAMEBUFFERSPROC)load("glDeleteFramebuffers");
    glad_glGenFramebuffers = (PFNGLGENFRAMEBUFFERSPROC)load("glGenFramebuffers");
    glad_glCheckFramebufferStatus = (PFNGLCHECKFRAMEBUFFERSTATUSPROC)load("glCheckFramebufferStatus");
    glad_glFramebufferTexture1D = (PFNGLFRAMEBUFFERTEXTURE1DPROC)load("glFramebufferTexture1D");
    glad_glFramebufferTexture2D = (PFNGLFRAMEBUFFERTEXTURE2DPROC)load("glFramebufferTexture2D");
    glad_glFramebufferTexture3D = (PFNGLFRAMEBUFFERTEXTURE3DPROC)load("glFramebufferTexture3D");
    glad_glFramebufferRenderbuffer = (PFNGLFRAMEBUFFERRENDERBUFFERPROC)load("glFramebufferRenderbuffer");
    glad_glGetFramebufferAttachmentParameteriv = (PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVPROC)load("glGetFramebufferAttachmentParameteriv");
    glad_glGenerateMipmap = (PFNGLGENERATEMIPMAPPROC)load("glGenerateMipmap");
    glad_glBlitFramebuffer = (PFNGLBLITFRAMEBUFFERPROC)load("glBlitFramebuffer");
    glad_glRenderbufferStorageMultisample = (PFNGLRENDERBUFFERSTORAGEMULTISAMPLEPROC)load("glRenderbufferStorageMultisample");
    glad_glFramebufferTextureLayer = (PFNGLFRAMEBUFFERTEXTURELAYERPROC)load("glFramebufferTextureLayer");
    glad_glMapBufferRange = (PFNGLMAPBUFFERRANGEPROC)load("glMapBufferRange");
    glad_glFlushMappedBufferRange = (PFNGLFLUSHMAPPEDBUFFERRANGEPROC)load("glFlushMappedBufferRange");
    */
    glad_glBindVertexArray = (PFNGLBINDVERTEXARRAYPROC)load(userptr, "glBindVertexArray");
    glad_glDeleteVertexArrays = (PFNGLDELETEVERTEXARRAYSPROC)load(userptr, "glDeleteVertexArrays");
    glad_glGenVertexArrays = (PFNGLGENVERTEXARRAYSPROC)load(userptr, "glGenVertexArrays");
    glad_glIsVertexArray = (PFNGLISVERTEXARRAYPROC)load(userptr, "glIsVertexArray");
}

static void glad_gl_load_GL_EXT_framebuffer_object(GLADuserptrloadfunc load, void * userptr)
{
    if(!GLAD_GL_EXT_framebuffer_object) return;
    glad_glBindFramebufferEXT = (PFNGLBINDFRAMEBUFFEREXTPROC) load(userptr, "glBindFramebufferEXT");
    glad_glBindRenderbufferEXT = (PFNGLBINDRENDERBUFFEREXTPROC) load(userptr, "glBindRenderbufferEXT");
    glad_glCheckFramebufferStatusEXT = (PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC) load(userptr, "glCheckFramebufferStatusEXT");
    glad_glDeleteFramebuffersEXT = (PFNGLDELETEFRAMEBUFFERSEXTPROC) load(userptr, "glDeleteFramebuffersEXT");
    glad_glDeleteRenderbuffersEXT = (PFNGLDELETERENDERBUFFERSEXTPROC) load(userptr, "glDeleteRenderbuffersEXT");
    glad_glFramebufferRenderbufferEXT = (PFNGLFRAMEBUFFERRENDERBUFFEREXTPROC) load(userptr, "glFramebufferRenderbufferEXT");
    glad_glFramebufferTexture1DEXT = (PFNGLFRAMEBUFFERTEXTURE1DEXTPROC) load(userptr, "glFramebufferTexture1DEXT");
    glad_glFramebufferTexture2DEXT = (PFNGLFRAMEBUFFERTEXTURE2DEXTPROC) load(userptr, "glFramebufferTexture2DEXT");
    glad_glFramebufferTexture3DEXT = (PFNGLFRAMEBUFFERTEXTURE3DEXTPROC) load(userptr, "glFramebufferTexture3DEXT");
    glad_glGenFramebuffersEXT = (PFNGLGENFRAMEBUFFERSEXTPROC) load(userptr, "glGenFramebuffersEXT");
    glad_glGenRenderbuffersEXT = (PFNGLGENRENDERBUFFERSEXTPROC) load(userptr, "glGenRenderbuffersEXT");
    glad_glGenerateMipmapEXT = (PFNGLGENERATEMIPMAPEXTPROC) load(userptr, "glGenerateMipmapEXT");
    glad_glGetFramebufferAttachmentParameterivEXT = (PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVEXTPROC) load(userptr,
                                                                                                           "glGetFramebufferAttachmentParameterivEXT");
    glad_glGetRenderbufferParameterivEXT = (PFNGLGETRENDERBUFFERPARAMETERIVEXTPROC) load(userptr,
                                                                                         "glGetRenderbufferParameterivEXT");
    glad_glIsFramebufferEXT = (PFNGLISFRAMEBUFFEREXTPROC) load(userptr, "glIsFramebufferEXT");
    glad_glIsRenderbufferEXT = (PFNGLISRENDERBUFFEREXTPROC) load(userptr, "glIsRenderbufferEXT");
    glad_glRenderbufferStorageEXT = (PFNGLRENDERBUFFERSTORAGEEXTPROC) load(userptr, "glRenderbufferStorageEXT");
}



static void glad_gl_free_extensions(char ** exts_i)
{
    if(exts_i != NULL) {
        unsigned int index;
        for(index = 0; exts_i[index]; index++) {
            free((void *)(exts_i[index]));
        }
        free((void *)exts_i);
        exts_i = NULL;
    }
}
static int glad_gl_get_extensions(const char ** out_exts, char *** out_exts_i)
{
#if defined(GL_ES_VERSION_3_0) || defined(GL_VERSION_3_0)
    if(glad_glGetStringi != NULL && glad_glGetIntegerv != NULL) {
        unsigned int index = 0;
        unsigned int num_exts_i = 0;
        char ** exts_i = NULL;
        glad_glGetIntegerv(GL_NUM_EXTENSIONS, (int *) &num_exts_i);
        exts_i = (char **) malloc((num_exts_i + 1) * (sizeof * exts_i));
        if(exts_i == NULL) {
            return 0;
        }
        for(index = 0; index < num_exts_i; index++) {
            const char * gl_str_tmp = (const char *) glad_glGetStringi(GL_EXTENSIONS, index);
            size_t len = strlen(gl_str_tmp) + 1;

            char * local_str = (char *) malloc(len * sizeof(char));
            if(local_str == NULL) {
                exts_i[index] = NULL;
                glad_gl_free_extensions(exts_i);
                return 0;
            }

            memcpy(local_str, gl_str_tmp, len * sizeof(char));
            exts_i[index] = local_str;
        }
        exts_i[index] = NULL;

        *out_exts_i = exts_i;

        return 1;
    }
#else
    GLAD_UNUSED(out_exts_i);
#endif
    if(glad_glGetString == NULL) {
        return 0;
    }
    *out_exts = (const char *)glad_glGetString(GL_EXTENSIONS);
    return 1;
}
static int glad_gl_has_extension(const char * exts, char ** exts_i, const char * ext)
{
    if(exts_i) {
        unsigned int index;
        for(index = 0; exts_i[index]; index++) {
            const char * e = exts_i[index];
            if(strcmp(e, ext) == 0) {
                return 1;
            }
        }
    }
    else {
        const char * extensions;
        const char * loc;
        const char * terminator;
        extensions = exts;
        if(extensions == NULL || ext == NULL) {
            return 0;
        }
        while(1) {
            loc = strstr(extensions, ext);
            if(loc == NULL) {
                return 0;
            }
            terminator = loc + strlen(ext);
            if((loc == extensions || *(loc - 1) == ' ') &&
               (*terminator == ' ' || *terminator == '\0')) {
                return 1;
            }
            extensions = terminator;
        }
    }
    return 0;
}

static GLADapiproc glad_gl_get_proc_from_userptr(void * userptr, const char * name)
{
    return (GLAD_GNUC_EXTENSION(GLADapiproc(*)(const char * name)) userptr)(name);
}

static int glad_gl_find_extensions_gl(void)
{
    const char * exts = NULL;
    char ** exts_i = NULL;
    if(!glad_gl_get_extensions(&exts, &exts_i)) return 0;

    GLAD_GL_EXT_framebuffer_object = glad_gl_has_extension(exts, exts_i, "GL_EXT_framebuffer_object");

    glad_gl_free_extensions(exts_i);

    return 1;
}

static int glad_gl_find_core_gl(void)
{
    int i;
    const char * version;
    const char * prefixes[] = {
        "OpenGL ES-CM ",
        "OpenGL ES-CL ",
        "OpenGL ES ",
        "OpenGL SC ",
        NULL
    };
    int major = 0;
    int minor = 0;
    version = (const char *) glad_glGetString(GL_VERSION);
    if(!version) return 0;
    for(i = 0;  prefixes[i];  i++) {
        const size_t length = strlen(prefixes[i]);
        if(strncmp(version, prefixes[i], length) == 0) {
            version += length;
            break;
        }
    }

    GLAD_IMPL_UTIL_SSCANF(version, "%d.%d", &major, &minor);

    GLAD_GL_VERSION_1_0 = (major == 1 && minor >= 0) || major > 1;
    GLAD_GL_VERSION_1_1 = (major == 1 && minor >= 1) || major > 1;
    GLAD_GL_VERSION_1_2 = (major == 1 && minor >= 2) || major > 1;
    GLAD_GL_VERSION_1_3 = (major == 1 && minor >= 3) || major > 1;
    GLAD_GL_VERSION_1_4 = (major == 1 && minor >= 4) || major > 1;
    GLAD_GL_VERSION_1_5 = (major == 1 && minor >= 5) || major > 1;
    GLAD_GL_VERSION_2_0 = (major == 2 && minor >= 0) || major > 2;
    GLAD_GL_VERSION_2_1 = (major == 2 && minor >= 1) || major > 2;
    GLAD_GL_VERSION_3_0 = (major == 3 && minor >= 0) || major > 1;

    return GLAD_MAKE_VERSION(major, minor);
}

int gladLoadGLUserPtr(GLADuserptrloadfunc load, void * userptr)
{
    int version;

    glad_glGetString = (PFNGLGETSTRINGPROC) load(userptr, "glGetString");
    if(glad_glGetString == NULL) return 0;
    version = glad_gl_find_core_gl();

    glad_gl_load_GL_VERSION_1_0(load, userptr);
    glad_gl_load_GL_VERSION_1_1(load, userptr);
    glad_gl_load_GL_VERSION_1_2(load, userptr);
    glad_gl_load_GL_VERSION_1_3(load, userptr);
    glad_gl_load_GL_VERSION_1_4(load, userptr);
    glad_gl_load_GL_VERSION_1_5(load, userptr);
    glad_gl_load_GL_VERSION_2_0(load, userptr);
    glad_gl_load_GL_VERSION_2_1(load, userptr);
    glad_gl_load_GL_VERSION_3_0(load, userptr);

    if(!glad_gl_find_extensions_gl()) return 0;
    glad_gl_load_GL_EXT_framebuffer_object(load, userptr);



    return version;
}


int gladLoadGL(GLADloadfunc load)
{
    return gladLoadGLUserPtr(glad_gl_get_proc_from_userptr, GLAD_GNUC_EXTENSION(void *) load);
}

#ifdef __cplusplus
}
#endif

#endif /*LV_USE_GLFW*/
