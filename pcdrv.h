// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at https://mozilla.org/MPL/2.0/.

#pragma once

// Replacement functions for PSYQ's LIBSN.h

// Init the file server
// not required
int PCinit();

// Create and open a file on the host
// Note: As per libsn spec, perms are ignored but forwarded to nops anyway
// Note: You can PCOpen to change readonly/writeonly mode after creating a file.
int	PCcreat( char * inName, int inPerms );

// Open a file on the host
// 0 = read only
// 1 = write only
// 2 = readwrite
int	PCopen( char * inName, int inFlags, int inPerms );

// Seek to a position in the file.
// mode 0 = from the start of the file
// mode 1 = from current pos
// mode 2 = from the end
// Returns the current file position
// Hint: seek to (file, 0, 2) to the end of the file to return the length
// Hint: then seek to (file, 0, 0) to get back to the start.
int	PClseek( int inHandle, int inOffset, int inMode );

// Reads a stream of chars into the specified buffer
int	PCread( int inHandle, char * outBuff, int inLen );

// Writes a steam fo chars from a buffer to a specified file
// and immediately flushes the file to disk
int	PCwrite( int inhandle, char * outBuff, int inLen );

// Close a file handle.
int	PCclose( int inHandle );
