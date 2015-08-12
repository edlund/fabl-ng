
xcopy /s /R /Y ".\vendor\bin\windows-x86-32" ".\Release"

cd Release\
.\fabl-ng.exe
