
#include "stdafx.h"
#include "DX12ClothSimulation.h"

_Use_decl_annotations_
int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow )
{
	DX12ClothSimulation sample( 1280, 720, L"D3D12 Fullscreen sample - Press the SPACE bar or ALT+Enter to toggle fullscreen mode" );
	return sample.Run( hInstance, nCmdShow );
}
