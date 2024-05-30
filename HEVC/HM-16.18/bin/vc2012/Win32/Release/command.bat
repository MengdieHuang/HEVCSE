@ echo off

set n=0

:Com

E:
cd E:\HEVC\HM-16.18\bin\vc2012\Win32\Release

::decode
::ffmpeg -i 8_Kimono_1920x1080.bin -c:v rawvideo -pix_fmt yuv420p E:\HEVC\HM-16.18\bin\vc2012\Win32\Release\DecYUV\dec_8_Kimono_1920x1080.yuv
REM ffmpeg -i 6_Kimono1_1920x1080_enc.bin -c:v rawvideo -pix_fmt yuv420p E:\HEVC\HM-16.18\bin\vc2012\Win32\Release\DecYUV\6_Kimono1_1920x1080_enc.yuv

REM ::ssim
REM psnr 1920 1080 420 E:\HEVC\HM-16.18\bin\vc2012\Win32\Release\OriYUV\Ori_Kimono1_1920x1080.yuv E:\HEVC\HM-16.18\bin\vc2012\Win32\Release\DecYUV\dec_6_Kimono1_1920x1080.yuv ssim > E:\HEVC\HM-16.18\bin\vc2012\Win32\Release\SSIM\dec_6_Kimono1_1920x1080_ssim.txt
REM psnr 1920 1080 420 E:\HEVC\HM-16.18\bin\vc2012\Win32\Release\OriYUV\Ori_Kimono1_1920x1080.yuv E:\HEVC\HM-16.18\bin\vc2012\Win32\Release\DecYUV\6_Kimono1_1920x1080_enc.yuv ssim> E:\HEVC\HM-16.18\bin\vc2012\Win32\Release\SSIM\6_Kimono1_1920x1080_enc_ssim.txt

REM ::psnr
REM psnr 1920 1080 420 E:\HEVC\HM-16.18\bin\vc2012\Win32\Release\OriYUV\Ori_Kimono1_1920x1080.yuv E:\HEVC\HM-16.18\bin\vc2012\Win32\Release\DecYUV\dec_6_Kimono1_1920x1080.yuv > E:\HEVC\HM-16.18\bin\vc2012\Win32\Release\SSIM\dec_6_Kimono1_1920x1080_psnr.txt
REM psnr 1920 1080 420 E:\HEVC\HM-16.18\bin\vc2012\Win32\Release\OriYUV\Ori_Kimono1_1920x1080.yuv E:\HEVC\HM-16.18\bin\vc2012\Win32\Release\DecYUV\6_Kimono1_1920x1080_enc.yuv > E:\HEVC\HM-16.18\bin\vc2012\Win32\Release\SSIM\6_Kimono1_1920x1080_enc_psnr.txt

REM set /p input=please input todecode file name:
REM set /p output=please input decoded file name:
REM ffmpeg -i %input% -c:v rawvideo -pix_fmt yuv420p E:\HEVC\HM-16.18\bin\vc2012\Win32\Release\DecYUV\%output%

::cmd传参数
set /p filename=please input todecode file name:
set /p w=please input resolution_w:
set /p h=please input resolution_h:
set /p flag=input flag:

REM ::VS传参数
REM set filename
REM set w
REM set h
REM set flag


set strmulti=x
set str0=_
set str1=.bin
set str2=dec_
set str3=.yuv
set str4=Ori_

set str5=_ssim.txt
set str6=_psnr.txt
set ffIn=%filename%%str0%%w%%strmulti%%h%%str0%%flag%%str1%
echo %ffIn%
set ffOut=%str2%%filename%%str0%%w%%strmulti%%h%%str0%%flag%%str3%
echo %ffOut%

::ffmpeg -i %ffIn% -c:v rawvideo -pix_fmt yuv420p10le E:\HEVC\HM-16.18\bin\vc2012\Win32\Release\DecYUV\IBBBP\%ffOut%
ffmpeg -i %ffIn% -c:v rawvideo -pix_fmt yuv420p E:\HEVC\HM-16.18\bin\vc2012\Win32\Release\DecYUV\%ffOut%

set ssimIn1=%str4%%filename%%str0%%w%%strmulti%%h%%str3%
echo %ssimIn1%

set ssimOut=%str2%%filename%%str0%%w%%strmulti%%h%%str0%%flag%%str5%
echo %ssimOut%

set psnrOut=%str2%%filename%%str0%%w%%strmulti%%h%%str0%%flag%%str6%
echo %psnrOut%
::psnr %w% %h% 420 E:\HEVC\HM-16.18\bin\vc2012\Win32\Release\OriYUV\%ssimIn1% E:\HEVC\HM-16.18\bin\vc2012\Win32\Release\DecYUV\IBBBP\%ffOut% ssim > E:\HEVC\HM-16.18\bin\vc2012\Win32\Release\SSIM\%ssimOut%
::psnr %w% %h% 420 E:\HEVC\HM-16.18\bin\vc2012\Win32\Release\OriYUV\%ssimIn1% E:\HEVC\HM-16.18\bin\vc2012\Win32\Release\DecYUV\IBBBP\%ffOut% > E:\HEVC\HM-16.18\bin\vc2012\Win32\Release\SSIM\%psnrOut%
psnr %w% %h% 420 E:\HEVC\HM-16.18\bin\vc2012\Win32\Release\OriYUV\%ssimIn1% E:\HEVC\HM-16.18\bin\vc2012\Win32\Release\DecYUV\%ffOut% ssim > E:\HEVC\HM-16.18\bin\vc2012\Win32\Release\SSIM\%ssimOut%
psnr %w% %h% 420 E:\HEVC\HM-16.18\bin\vc2012\Win32\Release\OriYUV\%ssimIn1% E:\HEVC\HM-16.18\bin\vc2012\Win32\Release\DecYUV\%ffOut% > E:\HEVC\HM-16.18\bin\vc2012\Win32\Release\SSIM\%psnrOut%


pause
set n=n+1
if n==10 exit
goto Com











pause>null