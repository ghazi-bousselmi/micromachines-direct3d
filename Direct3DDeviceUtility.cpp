#include "Direct3DDeviceUtility.h"

CDirect3DDeviceUtility::CDirect3DDeviceUtility(void)
{
	memset(&m_ViewPort,0,sizeof(D3DMATRIX));
	memset(m_Transform,0,sizeof(m_Transform));
	memset(m_Lights,0,sizeof(m_Lights));
	memset(m_LightEnable,0,sizeof(m_LightEnable));
	memset(m_RenderState,0,sizeof(m_RenderState));
	memset(m_TextureStageState,0,sizeof(m_TextureStageState));
	m_pd3dDevice=0;
}

CDirect3DDeviceUtility::~CDirect3DDeviceUtility(void)
{
}

HRESULT STDMETHODCALLTYPE CDirect3DDeviceUtility::SetViewportEx(CONST D3DVIEWPORT8* pViewport)
{
	m_ViewPort=*pViewport;
	return D3D_OK;
}

HRESULT STDMETHODCALLTYPE CDirect3DDeviceUtility::SetViewport(CONST D3DVIEWPORT8* pViewport)
{
	if (!m_pd3dDevice) throw;
	HRESULT hr;
	hr=m_pd3dDevice->SetViewport(pViewport);
	if (FAILED(hr)) 
		return hr;
	m_ViewPort=*pViewport;
	return D3D_OK;
}
HRESULT STDMETHODCALLTYPE CDirect3DDeviceUtility::GetViewport(D3DVIEWPORT8* pViewport)
{
	if (!m_pd3dDevice) throw;
	*pViewport=m_ViewPort;
	return D3D_OK;
}

HRESULT STDMETHODCALLTYPE CDirect3DDeviceUtility::SetTransform(D3DTRANSFORMSTATETYPE State,CONST D3DMATRIX* pMatrix)
{
	if (!m_pd3dDevice) throw;
	if (State<1 ||State>511) 
		return D3DERR_INVALIDCALL;
	HRESULT hr;
	hr=m_pd3dDevice->SetTransform(State,pMatrix);
	if (FAILED(hr)) 
		return hr;
	m_Transform[State]=*pMatrix;
	return D3D_OK;
}

HRESULT STDMETHODCALLTYPE CDirect3DDeviceUtility::GetTransform(D3DTRANSFORMSTATETYPE State,D3DMATRIX* pMatrix)
{
	if (!m_pd3dDevice) throw;
	if (State<1 ||State>511) 
		return D3DERR_INVALIDCALL;
	*pMatrix=m_Transform[State];
	return D3D_OK;
}

HRESULT STDMETHODCALLTYPE CDirect3DDeviceUtility::SetRenderState(D3DRENDERSTATETYPE State,DWORD Value)
{
	if (!m_pd3dDevice) throw;
	HRESULT hr;
	if (State<1 ||State>171) 
		return D3DERR_INVALIDCALL;
	hr=m_pd3dDevice->SetRenderState(State,Value);
	if (FAILED(hr)) 
		return hr;
	m_RenderState[State]=Value;
	return D3D_OK;
}

HRESULT STDMETHODCALLTYPE CDirect3DDeviceUtility::GetRenderState(D3DRENDERSTATETYPE State,DWORD* pValue)
{
	if (!m_pd3dDevice) throw;
	if (State<1 || State>171) 
		return D3DERR_INVALIDCALL;
	*pValue=m_RenderState[State];
	return D3D_OK;
}

HRESULT STDMETHODCALLTYPE CDirect3DDeviceUtility::SetLight(DWORD Index,CONST D3DLIGHT8*l)
{
	if (!m_pd3dDevice) throw;
	HRESULT hr;
	if (Index<0 || Index>127) 
		return D3DERR_INVALIDCALL;
	hr=m_pd3dDevice->SetLight(Index,l);
	if (FAILED(hr)) 
		return hr;
	m_Lights[Index]=*l;
	return D3D_OK;
}

HRESULT STDMETHODCALLTYPE CDirect3DDeviceUtility::GetLight(DWORD Index,D3DLIGHT8*l)
{
	if (!m_pd3dDevice) throw;
	if (Index<0 || Index>127)
		return D3DERR_INVALIDCALL;
	*l=m_Lights[Index];
	return D3D_OK;
}

HRESULT STDMETHODCALLTYPE CDirect3DDeviceUtility::LightEnable(DWORD Index,BOOL Enable)
{
	if (!m_pd3dDevice) throw;
	HRESULT hr;
	if (Index<0 || Index>127)
		return D3DERR_INVALIDCALL;
	hr=m_pd3dDevice->LightEnable(Index,Enable);
	if (FAILED(hr)) 
		return hr;
	m_LightEnable[Index]=Enable;
	return D3D_OK;
}

HRESULT STDMETHODCALLTYPE CDirect3DDeviceUtility::GetLightEnable(DWORD Index,BOOL* pEnable)
{
	if (!m_pd3dDevice) throw;
	if (Index<0 || Index>127) 
		return D3DERR_INVALIDCALL;
	*pEnable=m_LightEnable[Index];
	return D3D_OK;
}

HRESULT STDMETHODCALLTYPE CDirect3DDeviceUtility::SetTextureStageState(THIS_ DWORD Stage,D3DTEXTURESTAGESTATETYPE Type,DWORD Value)
{
	if (!m_pd3dDevice) throw;
	HRESULT hr;
	if (Stage>7)
		return D3DERR_INVALIDCALL;
	if (Type<1 || Type>28) 
		return D3DERR_INVALIDCALL;
	hr=m_pd3dDevice->SetTextureStageState(Stage,Type,Value);
	m_TextureStageState[Stage][Type]=Value;
	return D3D_OK;
}

HRESULT STDMETHODCALLTYPE CDirect3DDeviceUtility::GetTextureStageState(DWORD Stage,D3DTEXTURESTAGESTATETYPE Type,DWORD* pValue)
{
	if (!m_pd3dDevice) throw;
	if (Stage>7) 
		return D3DERR_INVALIDCALL;
	if (Type<1 || Type>28) 
		return D3DERR_INVALIDCALL;
	*pValue=m_TextureStageState[Stage][Type];
	return D3D_OK;
}

