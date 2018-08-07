/*

SparkFun Master Display Library

Created: July 2018
Modified: July 2018

Authors:
	Ciara Jekel
	Owen Lyke

Purpose: This library standardizes interfaces to displays of many types. 
         This header provides the most abstract class called 'display'
         Other categories of display such as E-Ink, TFT, and OLED can 
         derive from this class 

*/

#ifndef HYPERDISPLAY_H // We are going to need a cooler name than 'display' in the long run
#define HYPERDISPLAY_H

#include <Arduino.h>
#include "hyperdisplay_conf.h"  // The user-modifiable configuration file for the library

#if HYPERDISPLAY_USE_MATH
    #include <math.h>
#endif

#define SWAP_COORDS(a, b) uint16_t temp = a; \
              						  a = b; \
              					   b = temp; \

typedef void * color_t; 

typedef struct character_info{
	color_t * pdata;		// A pointer to the data to write
	uint32_t numPixels;		// The number of color_t types that pdata points to
	uint16_t xDim;			// Number of pixels in x dimension for rectilinear characters
	uint16_t yDim;			// Number of pixels in y dimension for rectilinear characters
	bool show;				// Whether or not to actually show the character
	bool causedNewline;		// Whether or not the character triggered a new line
}char_info_t;				// Character information structure for placing pixels in a window

typedef struct window_info{
    uint16_t xMin;              // FYI window coordiantes use the hardware frame of reference
    uint16_t xMax;
    uint16_t yMin;
    uint16_t yMax;
    int32_t  cursorX;
    int32_t  cursorY;
    uint16_t xReset;
    uint16_t yReset;
    char_info_t * pLastCharacter;   // A pointer to information about the last character written.
    color_t data;                   // A pointer to pixel data that is specific to the window. Can be left as NULL
}wind_info_t;                       // Window infomation structure for placing text on the display

class hyperdisplay : public Print{
    private:
    protected:

    	// Some protected drawing functions - not to be used directly
    	void lineHigh(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, color_t color, uint16_t colorCycleLength, uint16_t startColorOffset, uint16_t width);
    	void lineLow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, color_t color, uint16_t colorCycleLength, uint16_t startColorOffset, uint16_t width);
    	void circle_Bresenham(uint16_t x0, uint16_t y0, uint16_t radius, color_t color, bool fill);
		void circle_midpoint(uint8_t x0, uint8_t y0, uint8_t radius, color_t color, bool fill);
		void circle_eight(uint8_t xc, uint8_t yc, int16_t dx, int16_t dy, color_t color, bool fill);

        // Utility functions
    	uint16_t getNewColorOffset(uint16_t colorCycleLength, uint16_t startColorOffset, uint16_t numWritten);
        void setupDefaultWindow( void );                                                                        // Fills out the default window structure and associates it to the current window 
        void setupHyperDisplay(uint16_t xSize, uint16_t ySize);                                                 // Call this function in the begin() function of the derived class to ensure that all necessary paramters for the hyperdisplay parent class are set correctly

    public:
    // Parameters
        uint16_t xExt, yExt;        	// The rectilinear extent of the display in two dimensions (number of pixels)
        wind_info_t * pCurrentWindow;	// A pointer to the active window information structure.
        // uint8_t colorDepth;          // The number of bits of color depth for each pixel. // commented out because we implemented color_t to allow for flexible color types
        // color_t * pScratch;          // A pointer to a scratch space for maniuplating data. Can be set to NULL if not being used // Commented out because this ability is transferred to be part of the windows
        

    // Methods
        // A method for dealing with the color_t flexibility:
        virtual color_t getOffsetColor(color_t base, uint32_t numPixels) = 0;  // This pure virtual function is required to get the correct pointer after incrementing by a number of pixels (which could have any amount of data behind them)

        // 'primitive' drawing functions - coordinates are with respect to the current window
        virtual void pixel(uint16_t x0, uint16_t y0, color_t color) = 0; // Made a pure virtual function so that derived classes are forced to implement the pixel function
        virtual void xline(uint16_t x0, uint16_t y0, uint16_t len, color_t data, uint16_t colorCycleLength = 1, uint16_t startColorOffset = 0, uint16_t width = 1); // Default implementation using individual pixels so that user CAN add just a way to write to a pixel,  but highly reccommend optimizing
        virtual void yline(uint16_t x0, uint16_t y0, uint16_t len, color_t data, uint16_t colorCycleLength = 1, uint16_t startColorOffset = 0, uint16_t width = 1); //^
        virtual void rectangle(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, color_t color, uint8_t width = 1, bool filled = false); 
        virtual void fillFromArray(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t size, color_t data); 

        // These use the 'primitive' functions and are available to users - coordinates are with respect to the current window
        void line(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, color_t color, uint16_t width = 1); 
        void polygon(uint16_t x[], uint16_t y[], uint8_t numSides, color_t color, uint16_t width = 1);
        void circle(uint16_t x0, uint16_t y0, uint16_t radius, color_t color, bool filled = false); // Fills the entire current window
        void fillWindow(color_t color);
    
        // Printing
        #if HYPERDISPLAY_USE_PRINT
            virtual size_t write(uint8_t val); 						// This is the implementation of write that is inherited from, left as virtual to be implementation specific
            virtual char_info_t * getCharInfo(uint8_t val) = 0;		// A pure virtual function - you must implement this to be able to instantiate an object
        #else
            virtual size_t write(uint8_t val);
        #endif

        // Mathematical drawing tools
        #if HYPERDISPLAY_USE_MATH
            // Write math functions here
        #endif

};




// Some callback functions available to the user - they can be overriden with whatever implementation is useful at the time
// Note that the pure virtual function pixel() does not have a callback - if it is needed it should be included in the derived class,
// and that if the user provides implementation specific versions of the other primitive functions then these callbacks will not be
// called, so if the functionality is desired it can be re-implemented.
void hyperdisplayXLineCallback(uint16_t x0, uint16_t y0, uint16_t len, color_t color, uint16_t colorCycleLength, uint16_t startColorOffset, uint16_t width)  	__attribute__ ((weak));
void hyperdisplayYLineCallback(uint16_t x0, uint16_t y0, uint16_t len, color_t color, uint16_t colorCycleLength, uint16_t startColorOffset, uint16_t width) 	__attribute__ ((weak));
void hyperdisplayRectangleCallback(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, color_t color, uint8_t width, bool filled) 												__attribute__ ((weak));
void hyperdisplayFillFromArrayCallback(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t size, color_t data) 										__attribute__ ((weak));

#endif /* HYPERDISPLAY_H */


