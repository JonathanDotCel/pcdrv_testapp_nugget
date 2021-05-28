# pcdrv_testapp_nugget
Playstation1 'pcdrv' file server app using unirom + nops. (nugget version)

If you're using the PSYQ SDK, see here: https://github.com/JonathanDotCel/pcdrv_testapp_psyq

## How compile?

- Install docker
- Run buildme.bat

## What do I do?

### Put unirom in debug mode
`nops /debug` or `l1 + square`
The installs unirom's kernel mode handler, so you can do sio stuff while games/apps are running.

### Upload via unirom & monitor
`nops /fast /upload pcdrvtest.exe /m`

the `/m` part leaves the monitor open for the pcdrv stuff

## Where's the `pcdrv:` or `sim:` virtual device?

Uinirom won't provide it due to kernel space limitations.
I'll write some code for that if there's interest.
