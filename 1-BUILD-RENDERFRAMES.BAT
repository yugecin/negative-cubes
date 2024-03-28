@ECHO OFF
SET name=negative-cubes-renderframes
gcc -x c -c %name%.c -o %name%.o
IF %ERRORLEVEL% NEQ 0 GOTO:END
ECHO compile ok
Crinkler\crinkler %name%.o /OUT:%name%.exe "/LIBPATH:C:\Program Files (x86)\Microsoft SDKs\Windows\v7.1A\Lib" kernel32.lib user32.lib opengl32.lib gdi32.lib /SUBSYSTEM:WINDOWS
IF %ERRORLEVEL% NEQ 0 GOTO:END
ECHO link ok
:END
PAUSE
