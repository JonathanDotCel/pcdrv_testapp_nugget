// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at https://mozilla.org/MPL/2.0/.
// 
// Standard kinda fare though, no explicit or implied warranty.
// Any issues arising from the use of this software are your own.
// 
// https://github.com/JonathanDotCel
//

// Thanks to icons8.com for the free icons :)

#ifndef  MAINC
#define MAINC

// variadic logging
#include <stdarg.h>
#include "main.h"

//
// Includes
//

#include "littlelibc.h"
#include "utility.h"
#include "drawing.h"
#include "pads.h"
#include "ttyredirect.h"
#include "config.h"
#include "gpu.h"
#include "timloader.h"

#include "pcdrv.h"

//
// Defines
//

#define FILEMODE_READONLY 0
#define FILEMODE_WRITEONLY 1
#define FILEMODE_READWRITE 2

//
// Protos
//

void DoStuff();

//
// Stuff
//

int seekMode = 0;

// PCDrv return val
int lastOpsVal = 0;

// Print printable chars, escape the rest.
void ShowFileContents( char * addr , int length ){
    
    char clamped = 0;
    if ( length > 500 ){
         length = 500;
         clamped = 1;
    }

    while( !Released( PADRdown ) ){

        int i;        
        char * originalAddr = addr;

        DrawBG();
        ClearScreenText();
        Blah( "\n---- %d bytes start ----\n", length );

        for( i = 0; i < length; i++ ){

            unsigned char c = *originalAddr++;

            // vaguely printable range            
            if ( c >= 10 && c <= 'z' ){
                Blah( "%c", c );
            } else {
                Blah( " %%%x ", c );
            }
            
        }
        Blah( "\n---- end ----\n " );

        if ( clamped ){
            Blah( "Clamped at 500 chars\n" );
        }

        MonitorPads();
        Draw();
    }

}


void QuickMessage( char * message, int param0, int param1 ){

    while ( !Released( PADRdown ) ){
        DrawBG();
        ClearScreenText();
        Blah( message, param0, param1 );
        Blah( "\n\n Press X to continue" );
        MonitorPads();
        Draw();
    }
    
}

void ShowStatus(){
    
    QuickMessage( "\n\n\n\n Returned status %d\n", lastOpsVal, 0 );

}

// Handles for each individual file
int handle_bubblenuggets = -1;
int handle_biosdump = -1;
int handle_HELLOTIM = -1;

char didLoadLobster = 0;
TIMData lobsterTimData;

void DrawLoop(){

    NewPrintf( "Hello there...\n" );

	while(1)
	{
        
        // didn't actually implement this.
        #define FILE_DIRECTORY  16

        char L1Held_Open = 0; // PCOpen modifier
        char L2Held_Close = 0; // PCClose modifier
        char R1Held_Create = 0; // PCCreate modifier
        char R2Held_Write = 0; // PCWrite modifier

        DrawBG();
        
        ClearScreenText();

        MonitorPads();

        L1Held_Open = Held( PADL1 );
        L2Held_Close = Held( PADL2 );
        R1Held_Create = Held( PADR1 );
        R2Held_Write = Held( PADR2 );

        Blah( "\n\n" );

        Blah("  PCDrv sample - github.com/JonathanDotCel\n");
        Blah(" \n" );

        Blah("  Before loading this .exe:\n\n" );
        Blah("    put unirom into debugmode (nops /debug or L1+Square)\n");
        Blah("    put nops into monitor mode (nops /m)\n");        
        Blah(" \n" );
        
        Blah( "  Debug:\n\n" );
        Blah( "    LastResponse= %x, Handles= %d,%d,%d, Pad 1= %x\n", lastOpsVal, handle_bubblenuggets, handle_biosdump, handle_HELLOTIM, GetPadVals() );
        Blah(" \n" );

        Blah( "  Key Combos:\n\n" );
        Blah( "    Start: PCInit() (Break 0x101) (not required)\n" );
        Blah(" \n" );
        if ( Released( PADstart ) ){
            lastOpsVal = PCinit();
            ShowStatus();
        }

        // I hate nested inline ifs too. but hey...
        Blah( "     Left/Right: Toggle Write mode %s\n\n", seekMode == 0 ? "[0] 1 2 (from start)" : seekMode == 1 ? "0 [1] 2 (current pos)" : "0 1 [2] (append)" );
        if ( Released( PADLleft ) && seekMode > 0 ){
            seekMode--;
        }
        if ( Released( PADLright ) && seekMode < 2 ){
            seekMode++;
        }
        
        // This should really all be reformed into discrete functions
        // but for the purpose of making a readable demo that can be
        // easily referenced at a glance, we're going linear.

        Blah("     L1, L2, R1, R2 = switch to open/close/read/write/etc \n\n" );
        
        if ( R2Held_Write ){

            int targetHandle = -2;
            char * targetString;

            Blah( "     Write - Break 0x106\n" );
            Blah( "     /\\: PCWrite(bubblenuggets.txt, 'HELLO WORLD!', 12 )\n" );
            Blah( "     []: PCWrite(biosdump, <bios at 0xBFC00000>, 512kb )\n"  );
            Blah( "     X : PCWrite(bins/lobster.tim, gibberish, 20  )\n"  );
            
            if ( Released( PADRup ) ){
                targetHandle = handle_bubblenuggets;
                targetString = "HELLO WORLD!\n";
            }

            if ( Released( PADRleft ) ){
                targetHandle = handle_biosdump;
                targetString = (char*)0xBFC00000;   // the bios
            }

            if ( Released( PADRdown ) ){
                targetHandle = handle_HELLOTIM;
                targetString = "RUMPLE, RUMPLE, RUMPLE CHUNKS!\n";  // shouldn't write - it's read only
            }

            if ( targetHandle == -2 ){

            } else if ( targetHandle == -1 ){
                QuickMessage( "\n\nThat file hasn't been opened yet!", 0, 0 );
            } else {
                
                int seekStatus = PClseek( targetHandle, 0, seekMode );

                if ( seekStatus == -1 ){
                    QuickMessage( "\n\nError seeking!", 0, 0 );
                } else {

                    int len = ( targetHandle == handle_biosdump ? (512*1024) : strlen(targetString));
                    int result = PCwrite( targetHandle, targetString, len );

                    if ( result == -1 ){
                        QuickMessage( "\n\nError writing! Code: %x\n", result, 0 );
                    } else{                        
                        QuickMessage( "\n\nWrote %d bytes!\n", result, 0 );
                    }

                }
                
            }

        } else if ( R1Held_Create ){
            
            char didCreate = 0;

            Blah( "     Create - Break 0x102\n" );
            Blah( "     /\\: PCCreat(bubblenuggets.txt)\n" );
            Blah( "     []: PCCreat(biosdump)\n" );
            Blah( "     X : PCCreat(bins/lobster.tim)\n" );

            if ( Released( PADRup ) ){
                lastOpsVal = handle_bubblenuggets = PCcreat( "bubblenuggets.txt", 0 );
                didCreate = 1;            
            }
            if ( Released( PADRleft ) ){
                lastOpsVal = handle_biosdump = PCcreat( "biosdump", 0 );
                didCreate = 1;
            }        
            if ( Released( PADRdown ) ){
                lastOpsVal = handle_HELLOTIM = PCcreat( "bins/lobster.tim", 0);
                didCreate = 1;
            }

            if ( didCreate ){
                if ( lastOpsVal == -1 ){
                    ShowStatus();
                } else {
                    QuickMessage( "\n\nCreated file handle is %d\n", lastOpsVal, 0 );
                }
            }
            
        } else if ( L2Held_Close ){
            
            char didClose = 0;

            Blah( "     Close - Break 0x104\n" );
            Blah( "     /\\: PCClose(bubblenuggets.txt)\n" );
            Blah( "     []: PCClose(biosdump)\n" );
            Blah( "     X : PCClose(bins/lobster.tim)\n" );

            if ( Released( PADRup ) ){
                lastOpsVal = PCclose( handle_bubblenuggets );
                didClose = 1;
            }
            if ( Released( PADRleft ) ){
                lastOpsVal = PCclose( handle_biosdump );
                didClose = 1;
            }        
            if ( Released( PADRdown ) ){
                lastOpsVal = PCclose( handle_HELLOTIM );
                didClose = 1;
            }

            if ( didClose ){
                if ( lastOpsVal == -1 ){
                    ShowStatus();
                } else {
                    QuickMessage( "\n\nClosed file with return code %x\n", lastOpsVal, 0 );
                }
            }

        } else if ( L1Held_Open ){
            
            char didOpen = 0;

            Blah( "     Open - Break 0x103\n" );
            Blah( "     /\\: PCOpen(bubblenuggets.txt)\n" );
            Blah( "     []: PCOpen(biosdump)\n" );
            Blah( "     X : PCOpen(bins/lobster.tim)   (READONLY)\n" );

            if ( Released( PADRup ) ){
                lastOpsVal = handle_bubblenuggets = PCopen( "bubblenuggets.txt", FILEMODE_READWRITE, 0);
                didOpen = 1;
            }
            if ( Released( PADRleft ) ){
                lastOpsVal = handle_biosdump = PCopen( "biosdump", FILEMODE_READWRITE, 0 );
                didOpen = 1;
            }        
            if ( Released( PADRdown ) ){
                lastOpsVal = handle_HELLOTIM = PCopen( "bins/lobster.tim", FILEMODE_READONLY, 0);
                didOpen = 1;
            }

            if ( didOpen ){
                if ( lastOpsVal == -1 ){
                    ShowStatus();
                } else {
                    QuickMessage( "\n\nOpened file handle %d\n", lastOpsVal, 0 );
                }
            }

        } else {
            
            // PCRead

            int targetHandle = -2;

            // slap bang in the middle of RAM
            #define BUFFER 0x80100000
            #define pBuffer (char*)BUFFER
            
            Blah( "     Read - Break 0x105\n" );
            Blah( "     /\\: Read(bubblenuggets.txt, 0x%x, 100 )\n", BUFFER );
            Blah( "     []: Read(biosdump, 0x%x, 100 )\n", BUFFER );
            Blah( "     X : Read(bins/lobster.tim) 0x%x\n", BUFFER ); // empty/missing file (by default), expect zeros

            if ( Released( PADRup ) ){
                targetHandle = handle_bubblenuggets;                
            }

            if ( Released( PADRleft ) ){
                targetHandle = handle_biosdump;                
            }

            if ( Released( PADRdown ) ){
                targetHandle = handle_HELLOTIM;
            }

            if ( targetHandle == -2 ){

            } else if ( targetHandle == -1 ){
                QuickMessage( "\n\nThat file isn't open yet... Hold L1 to open...", 0 , 0 );
            } else {

                // seek to the end to get the file size
                int fileSize = PClseek( targetHandle, 0, 2 );
                if ( fileSize == -1 ){
                    QuickMessage( "\n\nCouldn't seek to find the file size...", 0, 0 );
                } else {
                    
                    int returnToStart;

                    NewPrintf( "File size 0x%x\n", fileSize );

                    returnToStart = PClseek( targetHandle, 0, 0 );

                    if ( fileSize == -1 ){
                        QuickMessage( "\n\nCouldn't seek back to the start of the file...", 0, 0 );
                    } else {
                        
                        lastOpsVal = PCread( targetHandle, pBuffer, fileSize );

                        if ( lastOpsVal == -1 ){
                            QuickMessage( "\n\nError reading the file!", 0, 0 );
                        } else {
                            if ( targetHandle == handle_HELLOTIM ){
                                didLoadLobster = 1;

                                // "Hello world and flappy credits has a good example
                                // of this code with diagrams and stuff if you want to play
                                // with sprites.
                                UploadTim( pBuffer, &lobsterTimData, SCREEN_WIDTH + TEXPAGE_WIDTH, 1, SCREEN_WIDTH + TEXPAGE_WIDTH, 16 );
                            } else {
                                ShowFileContents( pBuffer, fileSize );
                            }
                            
                        }

                    }

                }
                
            }

        }

        // Reboot
        if ( Released( PADRright ) ){
            goto *(ulong*)0xBFC00000;
        }
        
        if ( didLoadLobster ){

            Sprite * s;
            s->data = &lobsterTimData;

            s->xPos = 20;
            s->yPos = 20;

            s->height = 40;
            s->width = 40;

            DrawSprite( s );

        }


        Draw();
		
	}


}


int main(){
	
	//ResetEntryInt();
	ExitCritical();

	// Clear the text buffer
	InitBuffer();
	
	// Init the pads after the GPU so there are no active
	// interrupts and we get one frame of visual confirmation.
	
	NewPrintf( "Init GPU...\n" );
	InitGPU();

	NewPrintf( "Init Pads...\n" );
	InitPads();

    // Enable this if you're not inheriting a TTY redirect device from Unirom, n00bROM, etc
	//InstallTTY();
	//NewPrintf( "You can now use NewPrintf() functions!\n" );	
    
    DrawLoop();
	
}




#endif // ! MAINC

