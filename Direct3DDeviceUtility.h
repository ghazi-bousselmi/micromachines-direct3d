#pragma once

#include <windows.h>
#include <windowsx.h>
#include <basetsd.h>
#include <mmsystem.h>
#include <stdio.h>
#include <tchar.h>
#include <D3D8.h>


/**

I created this interface, as in some directx distributions, all d3d devices are Pure.
Pure devices dont support many Get function, and this application works with Get.

**/

class CDirect3DDeviceUtility
{
	D3DVIEWPORT8 m_ViewPort;
	D3DMATRIX m_Transform[512];		
	D3DLIGHT8 m_Lights[128];		// no information about how many are possible
	BOOL      m_LightEnable[128];
	DWORD     m_RenderState[172];	//1..171
	DWORD     m_TextureStageState[8][29];			// 1..28


public:
	CDirect3DDeviceUtility(void);
	~CDirect3DDeviceUtility(void);

	LPDIRECT3DDEVICE8 m_pd3dDevice;
	//void setDevice(LPDIRECT3DDEVICE8 dev) { m_pd3dDevice=dev; };

	STDMETHOD(SetViewport)(THIS_ CONST D3DVIEWPORT8* pViewport);
	STDMETHOD(SetViewportEx)(THIS_ CONST D3DVIEWPORT8* pViewport);
	STDMETHOD(GetViewport)(THIS_ D3DVIEWPORT8* pViewport);
    STDMETHOD(SetTransform)(THIS_ D3DTRANSFORMSTATETYPE State,CONST D3DMATRIX* pMatrix);
    STDMETHOD(GetTransform)(THIS_ D3DTRANSFORMSTATETYPE State,D3DMATRIX* pMatrix);
    STDMETHOD(SetRenderState)(THIS_ D3DRENDERSTATETYPE State,DWORD Value);
    STDMETHOD(GetRenderState)(THIS_ D3DRENDERSTATETYPE State,DWORD* pValue);
    STDMETHOD(SetLight)(THIS_ DWORD Index,CONST D3DLIGHT8*);
    STDMETHOD(GetLight)(THIS_ DWORD Index,D3DLIGHT8*);
    STDMETHOD(LightEnable)(THIS_ DWORD Index,BOOL Enable);
    STDMETHOD(GetLightEnable)(THIS_ DWORD Index,BOOL* pEnable);
	STDMETHOD(GetTextureStageState)(THIS_ DWORD Stage,D3DTEXTURESTAGESTATETYPE Type,DWORD* pValue);
    STDMETHOD(SetTextureStageState)(THIS_ DWORD Stage,D3DTEXTURESTAGESTATETYPE Type,DWORD Value);
    
};
