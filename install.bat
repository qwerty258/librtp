cd ".\MSVC\VS2013\x64\Release"
copy "libRTP.dll"   "%SystemRoot%\System32"

cd "..\..\Release"

copy "libRTP.dll"   "%SystemRoot%\SysWOW64"