/*
 * Copyright 2014 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include "gl/GrGLPathRendering.h"
#include "gl/GrGLInterface.h"
#include "gl/GrGLNameAllocator.h"
#include "gl/GrGLUtil.h"

#define GL_CALL(X) GR_GL_CALL(fGLInterface.get(), X)
#define GL_CALL_RET(RET, X) GR_GL_CALL_RET(fGLInterface.get(), RET, X)

class GrGLPathRenderingV12 : public GrGLPathRendering {
public:
    GrGLPathRenderingV12(const GrGLInterface* glInterface)
        : GrGLPathRendering(glInterface) {
    }

    virtual GrGLvoid stencilThenCoverFillPath(GrGLuint path, GrGLenum fillMode,
                                              GrGLuint mask, GrGLenum coverMode) SK_OVERRIDE;
    virtual GrGLvoid stencilThenCoverStrokePath(GrGLuint path, GrGLint reference,
                                                GrGLuint mask, GrGLenum coverMode) SK_OVERRIDE;
    virtual GrGLvoid stencilThenCoverFillPathInstanced(
                         GrGLsizei numPaths, GrGLenum pathNameType, const GrGLvoid *paths,
                         GrGLuint pathBase, GrGLenum fillMode, GrGLuint mask, GrGLenum coverMode,
                         GrGLenum transformType, const GrGLfloat *transformValues) SK_OVERRIDE;
    virtual GrGLvoid stencilThenCoverStrokePathInstanced(
                         GrGLsizei numPaths, GrGLenum pathNameType, const GrGLvoid *paths,
                         GrGLuint pathBase, GrGLint reference, GrGLuint mask, GrGLenum coverMode,
                         GrGLenum transformType, const GrGLfloat *transformValues) SK_OVERRIDE;
};

class GrGLPathRenderingV13 : public GrGLPathRenderingV12 {
public:
    GrGLPathRenderingV13(const GrGLInterface* glInterface)
        : GrGLPathRenderingV12(glInterface) {
        fCaps.fragmentInputGenSupport = true;
    }

    virtual GrGLvoid programPathFragmentInputGen(GrGLuint program, GrGLint location,
                                                 GrGLenum genMode, GrGLint components,
                                                 const GrGLfloat *coeffs) SK_OVERRIDE;
};


GrGLPathRendering* GrGLPathRendering::Create(const GrGLInterface* glInterface) {
    if (NULL == glInterface->fFunctions.fStencilThenCoverFillPath ||
        NULL == glInterface->fFunctions.fStencilThenCoverStrokePath ||
        NULL == glInterface->fFunctions.fStencilThenCoverFillPathInstanced ||
        NULL == glInterface->fFunctions.fStencilThenCoverStrokePathInstanced) {
        return new GrGLPathRendering(glInterface);
    }

    if (NULL == glInterface->fFunctions.fProgramPathFragmentInputGen) {
        return new GrGLPathRenderingV12(glInterface);
    }

    return new GrGLPathRenderingV13(glInterface);
}

GrGLPathRendering::GrGLPathRendering(const GrGLInterface* glInterface)
    : fGLInterface(SkRef(glInterface)) {
    memset(&fCaps, 0, sizeof(fCaps));
}

GrGLPathRendering::~GrGLPathRendering() {
}

void GrGLPathRendering::abandonGpuResources() {
    fPathNameAllocator.reset(NULL);
}


// NV_path_rendering
GrGLuint GrGLPathRendering::genPaths(GrGLsizei range) {
    if (range > 1) {
        GrGLuint name;
        GL_CALL_RET(name, GenPaths(range));
        return name;
    }

    if (NULL == fPathNameAllocator.get()) {
        static const int range = 65536;
        GrGLuint firstName;
        GL_CALL_RET(firstName, GenPaths(range));
        fPathNameAllocator.reset(SkNEW_ARGS(GrGLNameAllocator, (firstName, firstName + range)));
    }

    // When allocating names one at a time, pull from a client-side pool of
    // available names in order to save a round trip to the GL server.
    GrGLuint name = fPathNameAllocator->allocateName();

    if (0 == name) {
        // Our reserved path names are all in use. Fall back on GenPaths.
        GL_CALL_RET(name, GenPaths(1));
    }

    return name;
}

GrGLvoid GrGLPathRendering::deletePaths(GrGLuint path, GrGLsizei range) {
    if (range > 1) {
        // It is not supported to delete names in ranges that were allocated
        // individually using GrGLPathNameAllocator.
        SkASSERT(NULL == fPathNameAllocator.get() ||
                 path + range <= fPathNameAllocator->firstName() ||
                 path >= fPathNameAllocator->endName());
        GL_CALL(DeletePaths(path, range));
        return;
    }

    if (NULL == fPathNameAllocator.get() ||
        path < fPathNameAllocator->firstName() ||
        path >= fPathNameAllocator->endName()) {
        // If we aren't inside fPathNameAllocator's range then this name was
        // generated by the GenPaths fallback (or else was never allocated).
        GL_CALL(DeletePaths(path, 1));
        return;
    }

    // Make the path empty to save memory, but don't free the name in the driver.
    GL_CALL(PathCommands(path, 0, NULL, 0, GR_GL_FLOAT, NULL));
    fPathNameAllocator->free(path);
}

GrGLvoid GrGLPathRendering::pathCommands(GrGLuint path, GrGLsizei numCommands,
                                         const GrGLubyte *commands, GrGLsizei numCoords,
                                         GrGLenum coordType, const GrGLvoid *coords) {
    GL_CALL(PathCommands(path, numCommands, commands, numCoords, coordType, coords));
}

GrGLvoid GrGLPathRendering::pathCoords(GrGLuint path, GrGLsizei numCoords,
                                       GrGLenum coordType, const GrGLvoid *coords) {
    GL_CALL(PathCoords(path, numCoords, coordType, coords));
}

GrGLvoid GrGLPathRendering::pathParameteri(GrGLuint path, GrGLenum pname, GrGLint value) {
    GL_CALL(PathParameteri(path, pname, value));
}

GrGLvoid GrGLPathRendering::pathParameterf(GrGLuint path, GrGLenum pname, GrGLfloat value) {
    GL_CALL(PathParameterf(path, pname, value));
}

GrGLboolean GrGLPathRendering::isPath(GrGLuint path) {
    GrGLboolean ret;
    GL_CALL_RET(ret, IsPath(path));
    return ret;
}

GrGLvoid GrGLPathRendering::pathStencilFunc(GrGLenum func, GrGLint ref, GrGLuint mask) {
    GL_CALL(PathStencilFunc(func, ref, mask));
}

GrGLvoid GrGLPathRendering::stencilFillPath(GrGLuint path, GrGLenum fillMode, GrGLuint mask) {
    GL_CALL(StencilFillPath(path, fillMode, mask));
}

GrGLvoid GrGLPathRendering::stencilStrokePath(GrGLuint path, GrGLint reference, GrGLuint mask) {
    GL_CALL(StencilStrokePath(path, reference, mask));
}

GrGLvoid GrGLPathRendering::stencilFillPathInstanced(
             GrGLsizei numPaths, GrGLenum pathNameType, const GrGLvoid *paths,
             GrGLuint pathBase, GrGLenum fillMode, GrGLuint mask,
             GrGLenum transformType, const GrGLfloat *transformValues) {
    GL_CALL(StencilFillPathInstanced(numPaths, pathNameType, paths, pathBase,
                                     fillMode, mask, transformType, transformValues));
}

GrGLvoid GrGLPathRendering::stencilStrokePathInstanced(
             GrGLsizei numPaths, GrGLenum pathNameType, const GrGLvoid *paths,
             GrGLuint pathBase, GrGLint reference, GrGLuint mask,
             GrGLenum transformType, const GrGLfloat *transformValues) {
    GL_CALL(StencilStrokePathInstanced(numPaths, pathNameType, paths, pathBase,
                                       reference, mask, transformType, transformValues));
}

GrGLvoid GrGLPathRendering::pathTexGen(GrGLenum texCoordSet, GrGLenum genMode,
                                       GrGLint components, const GrGLfloat *coeffs) {
    GL_CALL(PathTexGen(texCoordSet, genMode, components, coeffs));
}

GrGLvoid GrGLPathRendering::coverFillPath(GrGLuint path, GrGLenum coverMode) {
    GL_CALL(CoverFillPath(path, coverMode));
}

GrGLvoid GrGLPathRendering::coverStrokePath(GrGLuint name, GrGLenum coverMode) {
    GL_CALL(CoverStrokePath(name, coverMode));
}

GrGLvoid GrGLPathRendering::coverFillPathInstanced(
             GrGLsizei numPaths, GrGLenum pathNameType, const GrGLvoid *paths, GrGLuint pathBase,
             GrGLenum coverMode, GrGLenum transformType, const GrGLfloat *transformValues) {
    GL_CALL(CoverFillPathInstanced(numPaths, pathNameType, paths, pathBase,
                                   coverMode, transformType, transformValues));
}

GrGLvoid GrGLPathRendering::coverStrokePathInstanced(
             GrGLsizei numPaths, GrGLenum pathNameType, const GrGLvoid *paths, GrGLuint pathBase,
             GrGLenum coverMode, GrGLenum transformType, const GrGLfloat* transformValues) {
    GL_CALL(CoverStrokePathInstanced(numPaths, pathNameType, paths, pathBase,
                                     coverMode, transformType, transformValues));
}

GrGLvoid GrGLPathRendering::stencilThenCoverFillPath(GrGLuint path, GrGLenum fillMode,
                                                     GrGLuint mask, GrGLenum coverMode) {
    GL_CALL(StencilFillPath(path, fillMode, mask));
    GL_CALL(CoverFillPath(path, coverMode));
}

GrGLvoid GrGLPathRendering::stencilThenCoverStrokePath(GrGLuint path, GrGLint reference,
                                                       GrGLuint mask, GrGLenum coverMode) {
    GL_CALL(StencilStrokePath(path, reference, mask));
    GL_CALL(CoverStrokePath(path, coverMode));
}

GrGLvoid GrGLPathRendering::stencilThenCoverFillPathInstanced(
             GrGLsizei numPaths, GrGLenum pathNameType, const GrGLvoid *paths,
             GrGLuint pathBase, GrGLenum fillMode, GrGLuint mask, GrGLenum coverMode,
             GrGLenum transformType, const GrGLfloat *transformValues) {
    GL_CALL(StencilFillPathInstanced(numPaths, pathNameType, paths, pathBase,
                                     fillMode, mask, transformType, transformValues));
    GL_CALL(CoverFillPathInstanced(numPaths, pathNameType, paths, pathBase,
                                   coverMode, transformType, transformValues));
}

GrGLvoid GrGLPathRendering::stencilThenCoverStrokePathInstanced(
             GrGLsizei numPaths, GrGLenum pathNameType, const GrGLvoid *paths,
             GrGLuint pathBase, GrGLint reference, GrGLuint mask, GrGLenum coverMode,
             GrGLenum transformType, const GrGLfloat *transformValues) {
    GL_CALL(StencilStrokePathInstanced(numPaths, pathNameType, paths, pathBase,
                                       reference, mask, transformType, transformValues));
    GL_CALL(CoverStrokePathInstanced(numPaths, pathNameType, paths, pathBase,
                                     coverMode, transformType, transformValues));
}

GrGLvoid GrGLPathRendering::programPathFragmentInputGen(
             GrGLuint program, GrGLint location, GrGLenum genMode,
             GrGLint components, const GrGLfloat *coeffs) {
    SkFAIL("ProgramPathFragmentInputGen not supported in this GL context.");
}


// NV_path_rendering v1.2
GrGLvoid GrGLPathRenderingV12::stencilThenCoverFillPath(GrGLuint path, GrGLenum fillMode,
                                                        GrGLuint mask, GrGLenum coverMode) {
    GL_CALL(StencilThenCoverFillPath(path, fillMode, mask, coverMode));
}

GrGLvoid GrGLPathRenderingV12::stencilThenCoverStrokePath(GrGLuint path, GrGLint reference,
                                                          GrGLuint mask, GrGLenum coverMode) {
    GL_CALL(StencilThenCoverStrokePath(path, reference, mask, coverMode));
}

GrGLvoid GrGLPathRenderingV12::stencilThenCoverFillPathInstanced(
             GrGLsizei numPaths, GrGLenum pathNameType, const GrGLvoid *paths,
             GrGLuint pathBase, GrGLenum fillMode, GrGLuint mask, GrGLenum coverMode,
             GrGLenum transformType, const GrGLfloat *transformValues) {
    GL_CALL(StencilThenCoverFillPathInstanced(numPaths, pathNameType, paths, pathBase, fillMode,
                                               mask, coverMode, transformType, transformValues));
}

GrGLvoid GrGLPathRenderingV12::stencilThenCoverStrokePathInstanced(
             GrGLsizei numPaths, GrGLenum pathNameType, const GrGLvoid *paths,
             GrGLuint pathBase, GrGLint reference, GrGLuint mask, GrGLenum coverMode,
             GrGLenum transformType, const GrGLfloat *transformValues) {
    GL_CALL(StencilThenCoverStrokePathInstanced(numPaths, pathNameType, paths, pathBase, reference,
                                                mask, coverMode, transformType, transformValues));
}


// NV_path_rendering v1.3
GrGLvoid GrGLPathRenderingV13::programPathFragmentInputGen(
             GrGLuint program, GrGLint location, GrGLenum genMode,
             GrGLint components, const GrGLfloat *coeffs) {
    GL_CALL(ProgramPathFragmentInputGen(program, location, genMode, components, coeffs));
}
