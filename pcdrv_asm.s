# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at https://mozilla.org/MPL/2.0/.

# Note: remember PCDRV shifts the args into A1, A2, A3

    .section .text, "x", @progbits
    .set push
    .set noreorder
    .set noat

    .global PCopen
    .global PCcreat
    .global PClseek
    .global PCread
    .global PCwrite
    .global PCclose
    .global PCinit

PCinit:
    
    break   0,  0x102

    jr      $ra
    nop
    
#int	PCread( int inHandle, char * outBuff, int inLen );
PCread:

    #LIBSN PCRead switches registers to 0, handle, len, buff* internally. 
    #e.g. a0,a1,a2 should become a1,a2,a3, but instead becomes a1,a3,a2   
	move    $a3, $a1
	move	$a2, $a2    
	move	$a1, $a0

    break   0, 0x105
    
    b       finishReadOrWrite
    nop

#int	PCwrite( int inhandle, char * outBuff, int inLen );
PCwrite:

    #LIBSN PCWrite switches registers to 0, handle, len, buff* internally. 
    #e.g. a0,a1,a2 should become a1,a2,a3, but instead becomes a1,a3,a2
	move    $a3, $a1
	move	$a2, $a2    
	move	$a1, $a0

    break   0, 0x106

finishReadOrWrite:
    
    #if successful, v0 (return val) will be bytesread/byteswritten
    beq     $v0, $0, readOrWriteSuccess    
    move    $v0, $v1

    li      $v0, -1

readOrWriteSuccess:

    jr      $ra
    nop


#int	PCcreat( char * inName, int inPerms );
PCcreat:

    # args to a1, a2, a3
    move    $a1, $a0
    move    $a2, $0
    
    break   0,  0x102

    beq     $v0, $0, create_success
    move    $v0, $v1

    li      $v0, -1

create_success:

    jr      $ra
    nop

#int	PCopen( char * inName, int inFlags, int inPerms );
PCopen:

    # args to a1, a2, a3
	move    $a2, $a1
	move	$a1, $a0 

	break	0, 0x103
    
	beq     $v0, $0, open_success
	move	$v0, $v1

	li      $v0, -1

open_success:

	jr      $ra
	nop


#int	PClseek( int inHandle, int inOffset, int inMode );
PClseek:

    # args to a1, a2, a3
	move    $a3, $a2
	move	$a2, $a1    
	move	$a1, $a0

	break	0, 0x0107

    #move 'pos' into v0 if successful
	beq     $v0, $0, seek_success    
	move	$v0, $v1

	li      $v0, -1

seek_success:
	jr      $ra
	nop
    

#int	PCclose( int inHandle );
PCclose:

    # args to a1, a2, a3
	move	$a1, $a0

	break	0, 0x104

	jr      $ra
	nop
    


    .set pop

