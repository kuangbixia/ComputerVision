#include "mcv.h"
#include "buffer_management.h"

Bytef* __managed_compressed_buffer = 0;

char* aligned_and_managed_new16b(size_t size)
{
    return aligned_and_managed_new(size, 16);
}

char* aligned_and_managed_new(size_t size, size_t align_size)
{
    char* ptr, * ptr2, * aligned_ptr;
    int align_mask = int(align_size - 1);

    ptr = new char[size + align_size + sizeof(char*) + sizeof(int)];
    if (ptr == 0) return 0;

    ptr2 = ptr + sizeof(char*) + sizeof(int);
    aligned_ptr = ptr2 + (align_size - ((size_t)ptr2 & align_mask));

    ((char**)aligned_ptr)[-1] = ptr;

    return aligned_ptr;
}

void aligned_and_managed_delete(void* aligned_ptr)
{
    if (aligned_ptr != 0) {
        if (MANAGED_MEMORY_DEBUGGING)
            cout << endl
            << "[MANAGED_MEMORY_DEBUGGING: freeing    "
            << *(int*)(((char*)aligned_ptr) - sizeof(void*) - sizeof(int)) << " items at "
            << "\t" << aligned_ptr << "]" << endl;
        delete[]((char**)aligned_ptr)[-1];
        aligned_ptr = 0;
    }
}

bool manage_image(IplImage** image, int width, int height, int depth, int nChannels)
{
    bool just_created = false;

    if (*image == 0) {
        *image = cvCreateImageHeader(cvSize(width, height), depth, nChannels);
        (*image)->imageData = 0;
        just_created = true;
    }

    bool different = (width != (*image)->width ||
        height != (*image)->height ||
        depth != (*image)->depth ||
        nChannels != (*image)->nChannels);

    if (just_created || different) {
        int pixel_size, image_size, width_step;
        switch (depth) {
        case IPL_DEPTH_8U:
        case IPL_DEPTH_8S:
            pixel_size = 1; break;
        case IPL_DEPTH_16U:
        case IPL_DEPTH_16S:
            pixel_size = 2; break;
        case IPL_DEPTH_32F:
        case IPL_DEPTH_32S:
            pixel_size = 4; break;
        default:
            cerr << "manage_aligned_lazy_image: unknown depth type: " << depth << endl;
            pixel_size = 4;
        }
        width_step = width * pixel_size;
        if (width_step % 16 != 0) width_step = 16 * (width_step / 16 + 1);
        image_size = width_step * height * nChannels;
        manage_buffer((*image)->imageData, image_size, char);
        (*image)->widthStep = width_step;
        (*image)->imageSize = image_size;

        (*image)->width = width;
        (*image)->height = height;
        (*image)->depth = depth;
        (*image)->nChannels = nChannels;

        return true;
    }
    else
        return false;
}

bool load_managed_image(char* filename, IplImage** image, int code)
{
    IplImage* tmp_image = mcvLoadImage(filename, code);

    if (tmp_image == 0) return false;

    manage_image(image, tmp_image->width, tmp_image->height, tmp_image->depth, tmp_image->nChannels);
    cvCopy(tmp_image, *image);
    cvReleaseImage(&tmp_image);

    return true;
}

void free_managed_image(IplImage** image)
{
    delete_managed_buffer((*image)->imageData);
    cvFree((void**)image);
}

void release_managed_image(IplImage** image)
{
    free_managed_image(image);
}

void save_image_in_pakfile(ofstream& f, IplImage* image)
{
    f << image->width << " " << image->height << " " << image->depth << " " << image->nChannels
        << " " << image->widthStep << " " << image->imageSize << endl;
    save_buffer_in_pakfile(f, image->imageData, image->imageSize, char);
}

void save_managed_image_in_pakfile(ofstream& f, IplImage* image)
{
    save_image_in_pakfile(f, image);
}

bool load_managed_image_in_pakfile(ifstream& f, IplImage** image)
{
    int width, height, depth, nChannels, width_step, image_size;
    f >> width >> height >> depth >> nChannels
        >> width_step >> image_size;

    //   cout << "> [buffer_management::load_managed_image_in_pakfile] Loading compressed image "
    //        << " size = " << width << "x" << height
    //        << ", depth = " << depth << ", nChannels = " << nChannels
    //        << "." << endl;

    bool result = manage_image(image, width, height, depth, nChannels);
    load_managed_buffer_in_pakfile(f, (*image)->imageData, image_size, char);

    (*image)->widthStep = width_step;
    (*image)->imageSize = image_size;

    return result;
}
