// bmp.h
// =====
// BMP image loader
// It reads only 8/24/32-bit uncompressed and 8-bit RLE compression format.
//
// 2022-09-28: Added BITFIELDS=3 compression mode (RGBA with bit masks)
// 2019-07-20: Fixed clearing memory in getColorCount()
// 2018-08-10: Fixed dealloc memory in save()
// 2016-11-09: Fixed errors when height < 0 in read()/save().
// 2013-03-23: Changed the type of dataSize to std::size_t for 64bit support.
// 2006-10-17: Improved flipImage()
// 2006-10-10: Added getError() to return the last error message.
// 2006-10-07: Fixed handling paddings if the width is not divisible by 4.
// 2006-09-25: Added 8-bit grayscale read and save (it is indexed mode).
//
//  AUTHOR: Song Ho Ahn (song.ahn@gmail.com)
// CREATED: 2006-05-08
// UPDATED: 2022-09-30
///////////////////////////////////////////////////////////////////////////////

#ifndef IMAGE_BMP_H
#define IMAGE_BMP_H

#include <string>

namespace Image
{
    class bmp
    {
    public:
        // ctor/dtor
        bmp();
        bmp(const bmp &rhs);
        ~bmp();

        bmp& operator=(const bmp &rhs);             // assignment operator

        // load image header and data from a bmp file
        bool read(const char* fileName);

        // save an image as BMP format
        // It assumes the color order of input image is RGB, so it will convert to BGR order before save
        bool save(const char* fileName, int width, int height, int channelCount, const unsigned char* data);

        // getters
        int getWidth() const;                       // return width of image in pixel
        int getHeight() const;                      // return height of image in pixel
        int getBitCount() const;                    // return the number of bits per pixel (8, 24, or 32)
        int getDataSize() const;                    // return data size in bytes
        const unsigned char* getData() const;       // return the pointer to image data
        const unsigned char* getDataRGB() const;    // return image data as RGB order

        void printSelf() const;                     // print itself for debug purpose
        const char* getError() const;               // return last error message

    protected:


    private:
        // member functions
        void init();                                // clear the existing values

        // shared functions (only 1 copy of the function, even if there are multiple instances of this class)
        static bool decodeRLE8(const unsigned char *encData, unsigned char *data);              // decode BMP 8-bit RLE to uncompressed
        static void flipImage(unsigned char *data, int width, int height, int channelCount);    // flip the vertical orientation
        static void swapRedBlue(unsigned char *data, int dataSize, int channelCount);           // swap the position of red and blue components
        static int  getColorCount(const unsigned char *data, int dataSize);                     // get the number of colors used in 8-bit grayscale image
        static void buildGrayScalePalette(unsigned char *palette, int paletteSize);
        static std::string orderBitMasks(unsigned int r, unsigned int g, unsigned int b, unsigned int a);

        // member variables
        int width;
        int height;
        int bitCount;
        int dataSize;
        unsigned char *data;                        // data with default BGR order
        unsigned char *dataRGB;                     // extra copy of image data with RGB order
        std::string errorMessage;
    };



    ///////////////////////////////////////////////////////////////////////////
    // inline functions
    ///////////////////////////////////////////////////////////////////////////
    inline int bmp::getWidth() const { return width; }
    inline int bmp::getHeight() const { return height; }

    // return bits per pixel, 8 means grayscale, 24 means RGB color, 32 means RGBA
    inline int bmp::getBitCount() const { return bitCount; }

    inline int bmp::getDataSize() const { return dataSize; }
    inline const unsigned char* bmp::getData() const { return data; }
    inline const unsigned char* bmp::getDataRGB() const { return dataRGB; }

    inline const char* bmp::getError() const { return errorMessage.c_str(); }
}

#endif // IMAGE_BMP_H
