@ECHO OFF
REM Zip all files in the Src_Dir


SET Src_Dir=Z21_Daten\

SET ZipProg="C:\Program Files\7-Zip\7z.exe"

if Exist MirZ21.z21 DEL MirZ21.z21
if Exist MirZ21.zip DEL MirZ21.zip

pushd %Src_Dir%
%ZipProg% a -r ..\MirZ21.zip
popd

REN MirZ21.zip MirZ21.z21

REM MEmu "Tablet" Downloads directory
Copy MirZ21.z21 "C:\Users\Hardi\Downloads\MEmu Download\"

REM Handy
Copy MirZ21.z21 "\\Dieser PC\WP5 Pro\Interner gemeinsamer Speicher\rocoZ21-layouts\"
