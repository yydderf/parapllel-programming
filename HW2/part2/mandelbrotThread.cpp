#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <thread>

#include "CycleTimer.h"

typedef struct
{
    float x0, x1;
    float y0, y1;
    unsigned int width;
    unsigned int height;
    unsigned int offset;
    unsigned int nrows;
    int maxIterations;
    int *output;
    int threadId;
    int numThreads;
} WorkerArgs;

// extern void mandelbrotSerial(
//     float x0, float y0, float x1, float y1,
//     int width, int height,
//     int startRow, int numRows,
//     int maxIterations,
//     int output[]);

static inline int mandel(float c_re, float c_im, int count)
{
  float z_re = c_re, z_im = c_im;
  int i;
  for (i = 0; i < count; ++i)
  {

    if (z_re * z_re + z_im * z_im > 4.f)
      break;

    float new_re = z_re * z_re - z_im * z_im;
    float new_im = 2.f * z_re * z_im;
    z_re = c_re + new_re;
    z_im = c_im + new_im;
  }

  return i;
}

void mandelbrotSerialMod(
    float x0, float y0, float x1, float y1,
    int width, int height,
    int startRow, int totalRows,
    int maxIterations,
    int output[],
    int threadId,
    int numThreads)
{
    // long long int cnt = 0;
    // long long int avg = 0;
    float dx = (x1 - x0) / width;
    float dy = (y1 - y0) / height;

    int total_iter = width * height;
    for (int i = threadId; i < total_iter; i += numThreads) {
        float x = x0 + (i % width) * dx;
        float y = y0 + (i / width) * dy;
        output[i] = mandel(x, y, maxIterations);
    }

    // int endRow = startRow + totalRows;

    // for (int j = startRow; j < endRow; j++)
    // {
    //     for (int i = 0; i < width; ++i)
    //     {
    //         float x = x0 + i * dx;
    //         float y = y0 + j * dy;

    //         int index = (j * width + i);
    //         output[index] = mandel(x, y, maxIterations);
    //         // avg += output[index];
    //         // cnt += 1;
    //     }
    // }
    // printf("ThreadId: %d, avg: %lld\n", threadId, avg/cnt);
}


//
// workerThreadStart --
//
// Thread entrypoint.
void workerThreadStart(WorkerArgs *const args)
{

    // TODO FOR PP STUDENTS: Implement the body of the worker
    // thread here. Each thread could make a call to mandelbrotSerial()
    // to compute a part of the output image. For example, in a
    // program that uses two threads, thread 0 could compute the top
    // half of the image and thread 1 could compute the bottom half.
    // Of course, you can copy mandelbrotSerial() to this file and
    // modify it to pursue a better performance.

    // printf("%f %f %f %f %d %d %d %d | %d\n", 
    // args->x0, args->y0, args->x1, args->y1,
    // args->width, args->height, args->maxIterations, args->numThreads, args->threadId);
    double startTime = CycleTimer::currentSeconds();
    mandelbrotSerialMod(args->x0, args->y0, args->x1, args->y1, args->width, args->height,
                        args->offset, args->nrows, args->maxIterations, args->output, args->threadId, args->numThreads);
    double endTime = CycleTimer::currentSeconds();
    printf("[Thread number %d]:\t\t[%.3f] ms\n", args->threadId, (endTime - startTime) * 1000);
}

//
// MandelbrotThread --
//
// Multi-threaded implementation of mandelbrot set image generation.
// Threads of execution are created by spawning std::threads.
void mandelbrotThread(
    int numThreads,
    float x0, float y0, float x1, float y1,
    int width, int height,
    int maxIterations, int output[])
{
    static constexpr int MAX_THREADS = 32;

    if (numThreads > MAX_THREADS)
    {
        fprintf(stderr, "Error: Max allowed threads is %d\n", MAX_THREADS);
        exit(1);
    }

    // Creates thread objects that do not yet represent a thread.
    std::thread workers[MAX_THREADS];
    WorkerArgs args[MAX_THREADS] = {};

    int loan = 0;
    int height_per_thread = height / numThreads;
    int divisible = (height % numThreads) ? 0 : 1;
    int acc_height = 0;

    // float dy = (y1 - y0) / height;

    for (int i = 0; i < numThreads; i++)
    {
        // TODO FOR PP STUDENTS: You may or may not wish to modify
        // the per-thread arguments here.  The code below copies the
        // same arguments for each thread
        args[i].x0 = x0;
        // args[i].y0 = y0 + dy * (acc_height);
        args[i].y0 = y0;
        args[i].x1 = x1;
        // args[i].y1 = y0 + dy * (acc_height + height_per_thread + loan);
        args[i].y1 = y1;
        args[i].width = width;
        // args[i].height = (i + 1 == numThreads) ? (height - acc_height): height_per_thread + loan;
        args[i].height = height;
        args[i].maxIterations = maxIterations;
        args[i].numThreads = numThreads;
        args[i].output = output;
        args[i].offset = (acc_height);
        args[i].nrows = height_per_thread + loan;

        args[i].threadId = i;

        acc_height += height_per_thread + loan;
        if (!divisible) loan = !loan;
    }

    // Spawn the worker threads.  Note that only numThreads-1 std::threads
    // are created and the main application thread is used as a worker
    // as well.
    for (int i = 1; i < numThreads; i++)
    {
        workers[i] = std::thread(workerThreadStart, &args[i]);
    }

    workerThreadStart(&args[0]);

    // join worker threads
    for (int i = 1; i < numThreads; i++)
    {
        workers[i].join();
    }
}
