//
//  main.cpp
//  tinyTiff
//
//  Created by John Robinson on 4/25/20.
//  Copyright © 2020 John Robinson. All rights reserved.
//

#include <iostream>
#include <filesystem>
#include <iostream>
#include <list>
#include <string>
#include <cctype>

#include "tinytiffreader.h"
#include "tinytiffwriter.h"
int main(int argc, const char * argv[]) {
    // Image metadata
    int         Width;
    int         Height;
    int         BitsPerSample;
    int         SamplesPerPixel;
    double      FocalLength;
    double      exposureTime;
    double      fstop;
    double      FocalPlaneXRes;
    double      FocalPlaneYRes;
    uint32_t    ISO;
    uint16_t*   image;
    Width =          4000;
    Height =         6000;
    BitsPerSample =  16;
    SamplesPerPixel= 3;
    FocalLength =    10.6;
    exposureTime =   0.0;
    fstop =          0.0;
    FocalPlaneXRes = 0.0;
    FocalPlaneYRes = 0.0;
    ISO            = 0.0;
    image =          NULL;
    
    // Open the tiff file
    TinyTIFFReaderFile* tiffr=NULL;
    tiffr = TinyTIFFReader_open(argv[1]);
    if (!tiffr) {
        std::cout<<"    ERROR reading (not existent, not accessible or no TIFF file)\n";
    } else {
        // read in some of the metadata
        Width=TinyTIFFReader_getWidth(tiffr);
        Height=TinyTIFFReader_getHeight(tiffr);
        //uint16_t sampleFormat = TinyTIFFReader_getSampleFormat(tiffr);
        BitsPerSample = TinyTIFFReader_getBitsPerSample(tiffr);
        SamplesPerPixel = TinyTIFFReader_getSamplesPerPixel(tiffr);
        if(!TinyTIFFReaderEXIF(exposureTime, tiffr, EXIF_ExposureTime)){
            cout << "Exposure time not found" << endl;
            exposureTime = 1.0;
        };
        if(!TinyTIFFReaderEXIF(FocalLength, tiffr, EXIF_FocalLength)){
            cout << "focal length not found" << endl;
            FocalLength = 17.0;
        };
        if(!TinyTIFFReaderEXIF(fstop, tiffr, EXIF_FNumber)){
             cout << "fstop  not found" << endl;
             fstop = 8.0;
         };
        if(!TinyTIFFReaderEXIF(FocalPlaneXRes, tiffr, EXIF_FocalPlaneXResolution)){
             cout << "Focal Plane X Res  not found" << endl;
             FocalPlaneXRes = 2553.191498;
         };
        if(!TinyTIFFReaderEXIF(FocalPlaneYRes, tiffr, EXIF_FocalPlaneYResolution)){
             cout << "Focal Plane Y Res  not found" << endl;
             FocalPlaneYRes = 2553.191498;
         };
        if(!TinyTIFFReaderEXIF(ISO, tiffr, EXIF_PhotographicSensitivity)){
             cout << "ISO  not found" << endl;
             ISO = 0;
         };
        if (SamplesPerPixel != 3){
            cout << "Image must must be RGB" << endl;
            return 1;
        }
        if (BitsPerSample != 16){
            cout << "Image must have 16 bits per sample" << endl;
            return 1;
        }
        image=(uint16_t*)calloc(Width*Height*SamplesPerPixel, sizeof(uint16_t));
        if (!TinyTIFFReader_getSampleData(tiffr, image, 0)){
            std::cout << TinyTIFFReader_getLastError(tiffr) << std::endl;
            return 1;
        }
    }
    TinyTIFFReader_close(tiffr);
    
    //print out the metadata
    cout << "Width: " << Width << " Height: " << Height << endl;
    if (exposureTime < 1.0)
        cout << "Exposure Time: 1/" << 1.0/exposureTime << endl;
    else
        cout << "Exposure Time: " << exposureTime << "\"" << endl;
    std::cout << "F Stop: " << fstop << std::endl;
    std::cout << "Focal Length: " << FocalLength << std::endl;
    std::cout << "ISO: " << ISO << endl;
    std::cout << "X sensor size: " << Width/FocalPlaneXRes << "  Y sensor Size: " << Height/FocalPlaneYRes << endl;
    std::cout << "Samples per pixel: " << SamplesPerPixel << std::endl;
    std::cout << "Bits per Sample: " << BitsPerSample << std::endl;
    
    // write out the same image
    TinyTIFFFile* tif=TinyTIFFWriter_open(argv[2], 3, 16, Width, Height);
    if (tif) {
        TinyTIFFWriter_writeImage(tif, image);
    }
    TinyTIFFWriter_close(tif);

    return 0;
}
