#define GLM_ENABLE_EXPERIMENTAL
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cuda_runtime.h>
#include <cuda_gl_interop.h>
#include <iostream>
#include <chrono>

#include "obj_loader.h"
#include "utils.h"
#include "Object.h"
#include "camera.h"
#include "scene.h"




// CUDA kernel
__global__ void render_kernel(cudaSurfaceObject_t surface, int width, int height) {
    int x = blockIdx.x * blockDim.x + threadIdx.x;
    int y = blockIdx.y * blockDim.y + threadIdx.y;

    if (x >= width || y >= height) return;

    uchar4 pixel = make_uchar4(x % 256, y % 256, 128, 255);
    surf2Dwrite(pixel, surface, x * sizeof(uchar4), y);
}

// Globals
GLuint tex;
cudaGraphicsResource* cuda_tex_resource;
int width = 1920, height = 1080;

// Setup GL texture
void createTexture() {
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0,
        GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, 0);

    // Register with CUDA
    cudaGraphicsGLRegisterImage(&cuda_tex_resource, tex,
        GL_TEXTURE_2D, cudaGraphicsRegisterFlagsSurfaceLoadStore);
}

float runCuda() {
    cudaArray_t array;
    cudaGraphicsMapResources(1, &cuda_tex_resource);
    cudaGraphicsSubResourceGetMappedArray(&array, cuda_tex_resource, 0, 0);

    cudaResourceDesc desc{};
    desc.resType = cudaResourceTypeArray;
    desc.res.array.array = array;

    cudaSurfaceObject_t surface = 0;
    cudaCreateSurfaceObject(&surface, &desc);

    //main defn 
    Camera c1(8.0f, 35.0f, height, width);
    Scene s1(c1);
    material m;
    m.albedo = glm::vec3(1.0f, 0.0f, 0.0f);
    Object o1("test2.obj", glm::vec3(0.0f, 0.0f, -10.0f), glm::vec3(0.0f, 0.0f, 0.0f), m);
    o1.BuildBVH(0, o1.Tri_index.size());
    s1.addObj(o1);
    s1.gen_global_BB();




    //end def

    dim3 block(16, 16);
    dim3 grid((width + block.x - 1) / block.x, (height + block.y - 1) / block.y);

    // CUDA Timing
    cudaEvent_t start, stop;
    float milliseconds = 0;
    cudaEventCreate(&start);
    cudaEventCreate(&stop);
    cudaEventRecord(start);

    render_kernel << <grid, block >> > (surface, width, height);

    cudaEventRecord(stop);
    cudaEventSynchronize(stop);
    cudaEventElapsedTime(&milliseconds, start, stop);

    cudaDestroySurfaceObject(surface);
    cudaGraphicsUnmapResources(1, &cuda_tex_resource);

    cudaEventDestroy(start);
    cudaEventDestroy(stop);

    return milliseconds;
}

// Simple fullscreen quad (fixed pipeline for demo)
void drawTexture() {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, tex);
    glBegin(GL_QUADS);
    glTexCoord2f(0, 0); glVertex2f(-1, 1); // bottom-left
    glTexCoord2f(1, 0); glVertex2f(1, 1);  // bottom-right
    glTexCoord2f(1, 1); glVertex2f(1, -1);   // top-right
    glTexCoord2f(0, 1); glVertex2f(-1, -1);  // top-left
    glEnd();
}

int main() {
    if (!glfwInit()) return -1;

    GLFWwindow* window = glfwCreateWindow(width, height, "CUDA-GL Texture", nullptr, nullptr);
    if (!window) return -1;
    glfwMakeContextCurrent(window);

    glewInit();
    createTexture();

    // Timing variables
    using Clock = std::chrono::high_resolution_clock;
    auto lastTime = Clock::now();
    int frameCount = 0;

    while (!glfwWindowShouldClose(window)) {
        auto frameStart = Clock::now();

        float kernelTime = runCuda();

        glClear(GL_COLOR_BUFFER_BIT);
        drawTexture();

        glfwSwapBuffers(window);
        glfwPollEvents();

        frameCount++;
        auto now = Clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastTime).count();
        if (elapsed >= 1000) {
            std::cout << "FPS: " << frameCount << " | Kernel time: " << kernelTime << " ms" << std::endl;
            frameCount = 0;
            lastTime = now;
        }
    }

    cudaGraphicsUnregisterResource(cuda_tex_resource);
    glDeleteTextures(1, &tex);
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
