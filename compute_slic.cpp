#include "common.hpp"
#include "opencl.hpp"
#include "slicsuperpixels.hpp"
#include "utils.hpp"

int
main(int argc, char const *argv[])
{
    std::string inFName = argv[1];
    std::string outFName = argv[2];

    Image img(inFName);
    int superPixelSpacing = 64;
    int nIters = 10;
    float relWeight = 0.5;

    bool useGPU = false;
    OpenCL opencl(useGPU);

    Size spSize = superPixelGridSize(img.size(), superPixelSpacing);

    // 3 dims for color + 2 for x and y
    Memory clusterCenters(opencl, CL_MEM_READ_WRITE, sizeof(float) * spSize.width * spSize.height * 5);
    Memory clusterAssig(opencl, CL_MEM_READ_WRITE, sizeof(int) * img.size().width * img.size().height);
    Memory imgDevice(opencl, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, img.stride() * img.size().height, img(0, 0));

    slicSuperPixels(opencl, img.size(), img.stride(), superPixelSpacing, nIters, relWeight, imgDevice, clusterCenters, clusterAssig);

    int clusterAssig_[img.size().width * img.size().height];
    clusterAssig.readBuffer(opencl, clusterAssig_);
    writePgm(outFName, clusterAssig_, img.size());

    return EXIT_SUCCESS;
}
