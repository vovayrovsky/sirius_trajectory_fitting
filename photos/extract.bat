@echo off

set dest=%~n1
set vlc="C:\Program Files (x86)\VideoLAN\VLC\vlc.exe" 

md %dest% >nul 2>nul

: --vout=dummy --start-time=1 --stop-time=5

%vlc%  %1 ^
       --video-filter=scene --scene-ratio=1 ^
       --scene-path=%dest% --scene-prefix="" --scene-format=bmp ^
       vlc://quit

echo Vova is a mouse many times.
echo And Ded too.