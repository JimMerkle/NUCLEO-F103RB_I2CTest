// Copyright Jim Merkle, 3/26/2020
// Module: hexdump.c
// This hexdump() function writes all output into a buffer, such that each
//  line is fully composed.  Some systems append a CR / LF after every print() function.
// This may not be the most efficient, but it gets the job done.

#include <stdio.h>

// Here's what I want for a hexdump() routine:
//00000000  02 03 1f 00 0d 00 00 00  00 00 00 00 00 00 00 00  |................|
//00000010  00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00  |................|
void hexdump(const void* address, unsigned size) {
    char outbuf[90]; // each line of output gets composed in outbuf
    unsigned remaining = size; // bytes remaining to be displayed
    unsigned char * data = (unsigned char*)address;
    unsigned displayaddr = 0; // starting address to display (this may be a parameter for other versions of this function)
    unsigned i=0; // otput buffer index to write next
    unsigned j;
    while (remaining) {
        unsigned thisline = remaining < 16?remaining : 16; // number of bytes to process for this line of output
        i=sprintf(outbuf,"%08X  ",displayaddr); // index 0-9
        // Display a row of data
        for(j=0;j<thisline;j++) {
            i+=sprintf(&outbuf[i],"%02X ",data[j]);
            // We like that little gap between each set of 8 displayed bytes
            if(j==7) i+=sprintf(&outbuf[i]," ");
        }
        // if we didn't display a complete line, write spaces to the buffer
        if(thisline < 16) {
            for(j=thisline;j<16;j++)
                i+=sprintf(&outbuf[i],"   ");
            if(thisline < 8)
                i+=sprintf(&outbuf[i]," "); // add another space for that little gap
        }
        // Add some space before ASCII displays
        i+=sprintf(&outbuf[i],"  |");
        // Display ASCII
        for(j=0;j<thisline;j++) {
            if(data[j] >= ' ' && data[j] <= '~')
                i+=sprintf(&outbuf[i],"%c",(char)data[j]); // printable ascii
            else
                i+=sprintf(&outbuf[i],".");  // non-printable
        } // for-loop
        
        // Print the line buffer with end marker and line feed
        printf("%s|\n",outbuf);
        
        // Line by line updates
        data += thisline;
        remaining -= thisline;
        displayaddr += thisline;
    }
    // Add an additional line feed if necessary
    printf("\n");
} // hexdump()
