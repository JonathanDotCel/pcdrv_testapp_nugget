# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at https://mozilla.org/MPL/2.0/.
	
	.section .text.wrapper, "x", @progbits
	
	.set push
	.set noreorder
    
	.global xfont
		
	.align 4			
xfont:
		.incbin "bins/inc_font.tim"
		.align 4
	
	.set pop


