@ echo off

set n=0

:Com

E:
cd E:\HEVC\HM-16.18\bin\vc2012\Win32\Release


set /p filename=please input todecode file name:
set /p w=please input resolution_w:
set /p h=please input resolution_h:
set /p flag=input flag:


set resolution=%w%x%h%
set str1=.yuv
set str2="%d.jpg"
set in=dec_%filename%_%resolution%_%flag%%str1%
set out=dec_%filename%_%resolution%_%flag%_%str2%

print %in%
print %out%
ffmpeg -s %resolution% -i E:\HEVC\HM-16.18\bin\vc2012\Win32\Release\DecYUV\%in% E:\HEVC\HM-16.18\bin\vc2012\Win32\Release\DecYUV\Picture\%out%

pause

goto Com











pause>null