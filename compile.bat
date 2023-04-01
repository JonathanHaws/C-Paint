
set compiler_path=clang++
set input_file=main.cpp
set output_file=main.exe

taskkill /F /IM %output_file% /T 
if exist %output_file% ( del /F %output_file%)
%compiler_path% %input_file% -o %output_file% -mwindows -luser32 -lgdi32
start %output_file%
