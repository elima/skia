#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <GL/glfw.h>

#include "GrContext.h"
#include "GrRenderTarget.h"
#include "GrGLInterface.h"
#include "SkGpuDevice.h"
#include "SkCanvas.h"
#include "SkGraphics.h"

#include <sys/time.h>

#define WIDTH 640
#define HEIGHT 480

static void draw(SkCanvas *canvas)
{
    canvas->save();
    int r = drand48() * 255;
    int g = drand48() * 255;
    int b = drand48() * 255;
    int a = drand48() * 255;
    SkColor color(a << 24 | r << 16 | g << 8 | b);

    SkPaint paint;
    paint.setColor(color);
    paint.setAntiAlias(true);
    paint.setStyle(SkPaint::kFill_Style);

    double x =  drand48() * WIDTH;
    double y = drand48() * HEIGHT;
    double radius = drand48() * 200;
    canvas->translate(x, y);
    double degree = drand48() * 180;
    canvas->rotate(degree);

    double scale_x = drand48() * 2;
    double scale_y = drand48() * 2;
    canvas->scale(scale_x, scale_y);

    canvas->drawCircle(0, 0, radius, paint);
    canvas->restore();
}

static double
get_tick (void)
{
    struct timeval now;
    gettimeofday(&now, NULL);
    return now.tv_sec + now.tv_usec / 1000000.0;
}

static void clear(SkCanvas *canvas)
{
    canvas->save();
    SkColor color(255 << 24 | 255 << 16 | 255 << 8 | 255);

    canvas->clear(color);
    canvas->restore();
}

int main(int argc, char* argv[])
{
    // Create window by using GLFW framework
    if (glfwInit() == GL_FALSE) {
      printf("Could not initialize GLFW. Aborting.\n");
      exit(-1);
    }

    if (glfwOpenWindow(WIDTH, HEIGHT, 8, 8, 8, 8, 24, 8, GLFW_WINDOW) == GL_FALSE) {
      printf("Could not open GLFW window. Aborting.\n");
      exit(-2);
    }

    const GrGLInterface* fGL = GrGLCreateNativeInterface();
    GrContext* ctx = GrContext::Create(kOpenGL_GrBackend, (GrBackendContext)fGL);
    if (!ctx) {
      printf("fGrContext was null\n");
      exit(-3);
    }

    GrRenderTarget* renderTarget;
    GrBackendRenderTargetDesc desc;
    desc.fWidth = WIDTH;
    desc.fHeight = HEIGHT;
    desc.fConfig = kSkia8888_GrPixelConfig;
    desc.fOrigin = kBottomLeft_GrSurfaceOrigin;
    desc.fSampleCnt = 4;
    //desc.fSampleCnt = 0;
    desc.fStencilBits = 1;
    desc.fRenderTargetHandle = 0;
    renderTarget = ctx->wrapBackendRenderTarget(desc);

    // Create Canvas
    SkGpuDevice *device = new SkGpuDevice(ctx, renderTarget);
    SkCanvas *canvas = new SkCanvas(device);

    int loop = 200;
    int count = 0;
    int total = 100;
    double start, stop;
    int i;

    for (;;) {
      if (count == 0)
        start = get_tick();

      clear(canvas);
      for (i = 0; i < loop; i++) {
        draw(canvas);
      }
      ctx->flush();

      glfwSwapBuffers();

      count++;

      if (count == total) {
        count = 0;
        stop = get_tick();
        fprintf(stdout, "fps = %0.2f\n", total / (stop - start));
      }
    }

    // Close OpenGL window and terminate GLFW
    glfwTerminate();
    return 0;
}
