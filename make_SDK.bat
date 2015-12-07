cd ".\MSVC\VS2013\x64\Release"
copy "libRTP.dll"   "..\..\..\..\libRTPSDK\bin\64bit"

copy "libRTP.lib"   "..\..\..\..\libRTPSDK\lib\64bit"

cd "..\..\Release"

copy "libRTP.dll"   "..\..\..\libRTPSDK\bin\32bit"

copy "libRTP.lib"   "..\..\..\libRTPSDK\lib\32bit"

cd "..\..\..\include"

copy "*.h"          "..\libRTPSDK\include"