//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// This code is licensed under the MIT License (MIT).
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

#include "stdafx.h"
#include "DX12Framework.h"
#include <shellapi.h>

using namespace Microsoft::WRL;

DX12Framework::DX12Framework( UINT width, UINT height, std::wstring name ) :
	m_aspectRatio( 0.0f ),
	m_useWarpDevice( false )
{
	ParseCommandLineArgs();

	m_title = name + ( m_useWarpDevice ? L" (WARP)" : L"" );

	WCHAR assetsPath[512];
	GetAssetsPath( assetsPath, _countof( assetsPath ) );
	m_assetsPath = assetsPath;

	UpdateForSizeChange( width, height );
}

DX12Framework::~DX12Framework()
{
}

int DX12Framework::Run( HINSTANCE hInstance, int nCmdShow )
{
	// Initialize the window class.
	WNDCLASSEX windowClass = { 0 };
	windowClass.cbSize = sizeof( WNDCLASSEX );
	windowClass.style = CS_HREDRAW | CS_VREDRAW;
	windowClass.lpfnWndProc = WindowProc;
	windowClass.hInstance = hInstance;
	windowClass.hCursor = LoadCursor( NULL, IDC_ARROW );
	windowClass.lpszClassName = L"WindowClass1";
	RegisterClassEx( &windowClass );

	RECT windowRect = { 0, 0, static_cast<LONG>( m_width ), static_cast<LONG>( m_height ) };
	AdjustWindowRect( &windowRect, WS_OVERLAPPEDWINDOW, FALSE );

	// Create the window and store a handle to it.
	m_hwnd = CreateWindow(
		L"WindowClass1",
		m_title.c_str(),
		WS_OVERLAPPEDWINDOW,
		300,
		300,
		windowRect.right - windowRect.left,
		windowRect.bottom - windowRect.top,
		NULL,		// We have no parent window, NULL.
		NULL,		// We aren't using menus, NULL.
		hInstance,
		this );

	// Initialize the sample. OnInit is defined in each child-implementation of DXSample.
	OnInit();

	ShowWindow( m_hwnd, nCmdShow );

	// Main sample loop.
	MSG msg = {};
	while ( msg.message != WM_QUIT )
	{
		// Process any messages in the queue.
		if ( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) )
		{
			TranslateMessage( &msg );
			DispatchMessage( &msg );

			// Pass events into our sample.
			OnEvent( msg );
		}
	}

	OnDestroy();

	// Return this part of the WM_QUIT message to Windows.
	return static_cast<char>( msg.wParam );
}

void DX12Framework::UpdateForSizeChange( UINT clientWidth, UINT clientHeight )
{
	m_width = clientWidth;
	m_height = clientHeight;
	m_aspectRatio = static_cast<float>( clientWidth ) / static_cast<float>( clientHeight );
}

// Helper function for resolving the full path of assets.
std::wstring DX12Framework::GetAssetFullPath( LPCWSTR assetName )
{
	return m_assetsPath + assetName;
}

// Helper function for acquiring the first available hardware adapter that supports Direct3D 12.
// If no such adapter can be found, *ppAdapter will be set to nullptr.
_Use_decl_annotations_
void DX12Framework::GetHardwareAdapter( IDXGIFactory4* pFactory, IDXGIAdapter1** ppAdapter )
{
	IDXGIAdapter1* pAdapter = nullptr;
	*ppAdapter = nullptr;

	for ( UINT adapterIndex = 0; DXGI_ERROR_NOT_FOUND != pFactory->EnumAdapters1( adapterIndex, &pAdapter ); ++adapterIndex )
	{
		DXGI_ADAPTER_DESC1 desc;
		pAdapter->GetDesc1( &desc );

		if ( desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE )
		{
			// Don't select the Basic Render Driver adapter.
			// If you want a software adapter, pass in "/warp" on the command line.
			continue;
		}

		// Check to see if the adapter supports Direct3D 12, but don't create the
		// actual device yet.
		if ( SUCCEEDED( D3D12CreateDevice( pAdapter, D3D_FEATURE_LEVEL_11_0, _uuidof( ID3D12Device ), nullptr ) ) )
		{
			break;
		}
	}

	*ppAdapter = pAdapter;
}

// Helper function for setting the window's title text.
void DX12Framework::SetCustomWindowText( LPCWSTR text )
{
	std::wstring windowText = m_title + L": " + text;
	SetWindowText( m_hwnd, windowText.c_str() );
}

// Helper function for parsing any supplied command line args.
void DX12Framework::ParseCommandLineArgs()
{
	int argc;
	LPWSTR *argv = CommandLineToArgvW( GetCommandLineW(), &argc );
	for ( int i = 1; i < argc; ++i )
	{
		if ( _wcsnicmp( argv[i], L"-warp", wcslen( argv[i] ) ) == 0 ||
			 _wcsnicmp( argv[i], L"/warp", wcslen( argv[i] ) ) == 0 )
		{
			m_useWarpDevice = true;
		}
	}
	LocalFree( argv );
}

// Main message handler for the sample.
LRESULT CALLBACK DX12Framework::WindowProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	DX12Framework* pSample = reinterpret_cast<DX12Framework*>( GetWindowLongPtr( hWnd, GWLP_USERDATA ) );

	switch ( message )
	{
	case WM_CREATE:
		{
			// Save a pointer to the DXSample passed in to CreateWindow.
			LPCREATESTRUCT pCreateStruct = reinterpret_cast<LPCREATESTRUCT>( lParam );
			SetWindowLongPtr( hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>( pCreateStruct->lpCreateParams ) );
		}
		return 0;

	case WM_PAINT:
		if ( pSample )
		{
			pSample->OnUpdate();
			pSample->OnRender();
		}
		return 0;

	case WM_SIZE:
		if ( pSample )
		{
			RECT clientRect = {};
			GetClientRect( hWnd, &clientRect );
			pSample->OnSizeChanged( clientRect.right - clientRect.left, clientRect.bottom - clientRect.top, wParam == SIZE_MINIMIZED );
		}
		return 0;

	case WM_DESTROY:
		PostQuitMessage( 0 );
		return 0;
	}

	// Handle any messages the switch statement didn't.
	return DefWindowProc( hWnd, message, wParam, lParam );
}
