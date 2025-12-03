In order to compile this project, you need to have the DirectX 8.0 SDK.
Currently, Microsoft is no longuer distributing DirectX SDK 8.0 SDK.
However, you can download the DirectX 9.0 SDK, which includes the 8.0 version (NOT included in the version 10.0).

An executable is present, tested with Norton Antivirus (September).

Latest reviews : 
- modified some variable declaration to suit the latest C++ syntax.
- bug corrected : when sound is off, there was an access violation when starting play.
- added a class 'CDirect3DDeviceUtility' : this class is responsible of holding in memory
  the tranformation matrices, the viwport, the texture states and the light states.
  The application uses the Direct3DDevice Get() function for those information, but
  those functions do not work on PURE D3D device.
