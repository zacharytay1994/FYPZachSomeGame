/******************************************************************************************
*	Chili DirectX Framework Version 16.07.20											  *
*	Graphics.cpp																		  *
*	Copyright 2016 PlanetChili.net <http://www.planetchili.net>							  *
*																						  *
*	This file is part of The Chili DirectX Framework.									  *
*																						  *
*	The Chili DirectX Framework is free software: you can redistribute it and/or modify	  *
*	it under the terms of the GNU General Public License as published by				  *
*	the Free Software Foundation, either version 3 of the License, or					  *
*	(at your option) any later version.													  *
*																						  *
*	The Chili DirectX Framework is distributed in the hope that it will be useful,		  *
*	but WITHOUT ANY WARRANTY; without even the implied warranty of						  *
*	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the						  *
*	GNU General Public License for more details.										  *
*																						  *
*	You should have received a copy of the GNU General Public License					  *
*	along with The Chili DirectX Framework.  If not, see <http://www.gnu.org/licenses/>.  *
******************************************************************************************/
#include "MainWindow.h"
#include "Graphics.h"
#include "DXErr.h"
#include "ChiliException.h"
#include <assert.h>
#include <string>
#include <array>

// Ignore the intellisense error "cannot open source file" for .shh files.
// They will be created during the build sequence before the preprocessor runs.
namespace FramebufferShaders
{
#include "FramebufferPS.shh"
#include "FramebufferVS.shh"
}

#pragma comment( lib,"d3d11.lib" )

#define CHILI_GFX_EXCEPTION( hr,note ) Graphics::Exception( hr,note,_CRT_WIDE(__FILE__),__LINE__ )

using Microsoft::WRL::ComPtr;

Graphics::Graphics( HWNDKey& key )
{
	assert( key.hWnd != nullptr );

	//////////////////////////////////////////////////////
	// create device and swap chain/get render target view
	DXGI_SWAP_CHAIN_DESC sd = {};
	sd.BufferCount = 1;
	sd.BufferDesc.Width = Graphics::ScreenWidthDisplay;
	sd.BufferDesc.Height = Graphics::ScreenHeightDisplay;
	sd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 1;
	sd.BufferDesc.RefreshRate.Denominator = 60;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = key.hWnd;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = TRUE;

	HRESULT				hr;
	UINT				createFlags = 0u;
#ifdef CHILI_USE_D3D_DEBUG_LAYER
#ifdef _DEBUG
	createFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
#endif
	
	// create device and front/back buffers
	if( FAILED( hr = D3D11CreateDeviceAndSwapChain( 
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		createFlags,
		nullptr,
		0,
		D3D11_SDK_VERSION,
		&sd,
		&pSwapChain,
		&pDevice,
		nullptr,
		&pImmediateContext ) ) )
	{
		throw CHILI_GFX_EXCEPTION( hr,L"Creating device and swap chain" );
	}

	// get handle to backbuffer
	ComPtr<ID3D11Resource> pBackBuffer;
	if( FAILED( hr = pSwapChain->GetBuffer(
		0,
		__uuidof( ID3D11Texture2D ),
		(LPVOID*)&pBackBuffer ) ) )
	{
		throw CHILI_GFX_EXCEPTION( hr,L"Getting back buffer" );
	}

	// create a view on backbuffer that we can render to
	if( FAILED( hr = pDevice->CreateRenderTargetView( 
		pBackBuffer.Get(),
		nullptr,
		&pRenderTargetView ) ) )
	{
		throw CHILI_GFX_EXCEPTION( hr,L"Creating render target view on backbuffer" );
	}


	// set backbuffer as the render target using created view
	pImmediateContext->OMSetRenderTargets( 1,pRenderTargetView.GetAddressOf(),nullptr );


	// set viewport dimensions
	D3D11_VIEWPORT vp;
	vp.Width = float( Graphics::ScreenWidthDisplay );
	vp.Height = float( Graphics::ScreenHeightDisplay );
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0.0f;
	vp.TopLeftY = 0.0f;
	pImmediateContext->RSSetViewports( 1,&vp );


	///////////////////////////////////////
	// create texture for cpu render target
	D3D11_TEXTURE2D_DESC sysTexDesc;
	sysTexDesc.Width = Graphics::ScreenWidthDisplay;
	sysTexDesc.Height = Graphics::ScreenHeightDisplay;
	sysTexDesc.MipLevels = 1;
	sysTexDesc.ArraySize = 1;
	sysTexDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	sysTexDesc.SampleDesc.Count = 1;
	sysTexDesc.SampleDesc.Quality = 0;
	sysTexDesc.Usage = D3D11_USAGE_DYNAMIC;
	sysTexDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	sysTexDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	sysTexDesc.MiscFlags = 0;
	// create the texture
	if( FAILED( hr = pDevice->CreateTexture2D( &sysTexDesc,nullptr,&pSysBufferTexture ) ) )
	{
		throw CHILI_GFX_EXCEPTION( hr,L"Creating sysbuffer texture" );
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = sysTexDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	// create the resource view on the texture
	if( FAILED( hr = pDevice->CreateShaderResourceView( pSysBufferTexture.Get(),
		&srvDesc,&pSysBufferTextureView ) ) )
	{
		throw CHILI_GFX_EXCEPTION( hr,L"Creating view on sysBuffer texture" );
	}


	////////////////////////////////////////////////
	// create pixel shader for framebuffer
	// Ignore the intellisense error "namespace has no member"
	if( FAILED( hr = pDevice->CreatePixelShader(
		FramebufferShaders::FramebufferPSBytecode,
		sizeof( FramebufferShaders::FramebufferPSBytecode ),
		nullptr,
		&pPixelShader ) ) )
	{
		throw CHILI_GFX_EXCEPTION( hr,L"Creating pixel shader" );
	}
	

	/////////////////////////////////////////////////
	// create vertex shader for framebuffer
	// Ignore the intellisense error "namespace has no member"
	if( FAILED( hr = pDevice->CreateVertexShader(
		FramebufferShaders::FramebufferVSBytecode,
		sizeof( FramebufferShaders::FramebufferVSBytecode ),
		nullptr,
		&pVertexShader ) ) )
	{
		throw CHILI_GFX_EXCEPTION( hr,L"Creating vertex shader" );
	}
	

	//////////////////////////////////////////////////////////////
	// create and fill vertex buffer with quad for rendering frame
	const FSQVertex vertices[] =
	{
		{ -1.0f,1.0f,0.5f,0.0f,0.0f },
		{ 1.0f,1.0f,0.5f,1.0f,0.0f },
		{ 1.0f,-1.0f,0.5f,1.0f,1.0f },
		{ -1.0f,1.0f,0.5f,0.0f,0.0f },
		{ 1.0f,-1.0f,0.5f,1.0f,1.0f },
		{ -1.0f,-1.0f,0.5f,0.0f,1.0f },
	};
	D3D11_BUFFER_DESC bd = {};
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof( FSQVertex ) * 6;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0u;
	D3D11_SUBRESOURCE_DATA initData = {};
	initData.pSysMem = vertices;
	if( FAILED( hr = pDevice->CreateBuffer( &bd,&initData,&pVertexBuffer ) ) )
	{
		throw CHILI_GFX_EXCEPTION( hr,L"Creating vertex buffer" );
	}

	
	//////////////////////////////////////////
	// create input layout for fullscreen quad
	const D3D11_INPUT_ELEMENT_DESC ied[] =
	{
		{ "POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 },
		{ "TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0,12,D3D11_INPUT_PER_VERTEX_DATA,0 }
	};

	// Ignore the intellisense error "namespace has no member"
	if( FAILED( hr = pDevice->CreateInputLayout( ied,2,
		FramebufferShaders::FramebufferVSBytecode,
		sizeof( FramebufferShaders::FramebufferVSBytecode ),
		&pInputLayout ) ) )
	{
		throw CHILI_GFX_EXCEPTION( hr,L"Creating input layout" );
	}


	////////////////////////////////////////////////////
	// Create sampler state for fullscreen textured quad
	D3D11_SAMPLER_DESC sampDesc = {};
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	if( FAILED( hr = pDevice->CreateSamplerState( &sampDesc,&pSamplerState ) ) )
	{
		throw CHILI_GFX_EXCEPTION( hr,L"Creating sampler state" );
	}

	// allocate memory for sysbuffer (16-byte aligned for faster access)
	pSysBuffer = reinterpret_cast<Color*>( 
		_aligned_malloc( sizeof( Color ) * Graphics::ScreenWidthDisplay * Graphics::ScreenHeightDisplay,16u ) );
}

void Graphics::DrawTriangle(const Vecf3 & p1, const Vecf3 & p2, const Vecf3 & p3, Color c)
{
	// Don't want the values to change but we want to swap them, so pointers
	const Vecf3* v0 = &p1;
	const Vecf3* v1 = &p2;
	const Vecf3* v2 = &p3;

	// Ensure vertices are in order
	if (v0->y > v1->y) {
		std::swap(v0, v1);
	}
	if (v1->y > v2->y) {
		std::swap(v1, v2);
	}
	if (v0->y > v1->y) {
		std::swap(v0, v1);
	}
	/*if (v1->y < v0->y) std::swap(v0, v1);
	if (v2->y < v1->y) std::swap(v1, v2);
	if (v1->y < v0->y) std::swap(v0, v1);*/

	// Check if triangle is flat top/bottom
	if (v0->y == v1->y) {
		// Flattop
		// Sorting x to draw left to right
		if (v0->x > v1->x) {
			std::swap(v0, v1);
			DrawFlatTopTriangle(*v0, *v1, *v2, c);
		}
	}
	else if (v1->y == v2->y) {
		// Flatbottom
		if (v1->x > v2->x) {
			std::swap(v1, v2);
			DrawFlatBottomTriangle(*v0, *v1, *v2, c);
		}
	}
	else {
		// Split
		const float alpha = (v1->y - v0->y)/(v2->y - v0->y);
		const Vecf2 v3(v0->x + (v2->x - v0->x) * alpha, v1->y);

		if (v1->x < v3.x) {
			// Major right
			DrawFlatBottomTriangle(*v0, *v1, v3, c);
			DrawFlatTopTriangle(*v1, v3, *v2, c);
		}
		else {
			// Major left
			DrawFlatBottomTriangle(*v0, v3, *v1, c);
			DrawFlatTopTriangle(v3, *v1, *v2, c);
		}
		const float alphaSplit =
			(v1->y - v0->y) /
			(v2->y - v0->y);
		const Vecf2 vi = *v0 + (*v2 - *v0) * alphaSplit;

		//if (v1->x < vi.x) // major right
		//{
		//	DrawFlatBottomTriangle(*v0, *v1, vi, c);
		//	DrawFlatTopTriangle(*v1, vi, *v2, c);
		//}
		//else // major left
		//{
		//	DrawFlatBottomTriangle(*v0, vi, *v1, c);
		//	DrawFlatTopTriangle(vi, *v1, *v2, c);
		//}
	}
}

void Graphics::DrawTriangleTex(const TextureVertex & v0, const TextureVertex & v1, const TextureVertex & v2, Surface & tex)
{
	// create pointers
	const TextureVertex* pv0 = &v0;
	const TextureVertex* pv1 = &v1;
	const TextureVertex* pv2 = &v2;

	// sort
	if (pv0->pos.y > pv1->pos.y) { std::swap(pv0, pv1); }
	if (pv1->pos.y > pv2->pos.y) { std::swap(pv1, pv2); }
	if (pv0->pos.y > pv1->pos.y) { std::swap(pv0, pv1); }

	// if flattop
	if (pv0->pos.y == pv1->pos.y) {
		//draw flat top
		if (pv0->pos.x > pv1->pos.x) {
			std::swap(pv0, pv1);
		}
		DrawFlatTopTriangleTex(*pv0, *pv1, *pv2, tex);
	}
	else if (pv1->pos.y == pv2->pos.y) { // if flat bottom
		// draw flat bottom
		if (pv1->pos.x > pv2->pos.x) {
			std::swap(pv1, pv2);
		}
		DrawFlatBottomTriangleTex(*pv0, *pv1, *pv2, tex);
	}
	else {
		// split
		// return splitting vertex
		const TextureVertex pv3 = pv0->InterpolateTo(*pv2, pv1->pos.y - pv0->pos.y);
		// determine major left/right
		if (pv1->pos.x > pv3.pos.x) {
			// major left
			DrawFlatTopTriangleTex(pv3, *pv1, *pv2, tex);
			DrawFlatBottomTriangleTex(*pv0, pv3, *pv1, tex);
		}
		else {
			// major right
			DrawFlatTopTriangleTex(*pv1, pv3, *pv2, tex);
			DrawFlatBottomTriangleTex(*pv0, *pv1, pv3, tex);
		}
	}
}

Graphics::~Graphics()
{
	// free sysbuffer memory (aligned free)
	if( pSysBuffer )
	{
		_aligned_free( pSysBuffer );
		pSysBuffer = nullptr;
	}
	// clear the state of the device context before destruction
	if( pImmediateContext ) pImmediateContext->ClearState();
}

void Graphics::DrawFlatTopTriangle(const Vecf2 & v0, const Vecf2 & v1, const Vecf2 & v2, Color c)
{
	// Calculate gradients run/rise since triangles can have x-x = 0 but not y-y for slopes
	float m0 = (v2.x - v0.x) / (v2.y - v0.y);
	float m1 = (v2.x - v1.x) / (v2.y - v1.y);

	// Calculate the start and end of scanlines, LINES not the pixels
	const int yStart = (int)ceilf(v0.y - 0.5f);
	const int yEnd = (int)ceilf(v2.y - 0.5f);

	for (int y = yStart; y < yEnd; y++) {
		// Calculate xStart xEnd
		const float x0 = m0 * (float(y) + 0.5f - v0.y) + v0.x;
		const float x1 = m1 * (float(y) + 0.5f - v1.y) + v1.x;

		// calculate start and end pixels for left rast rule
		const int xStart = (int)ceilf(x0 - 0.5f);
		const int xEnd = (int)ceilf(x1 - 0.5f);

		for (int x = xStart; x < xEnd; x++) {
			PutPixel(x, y, c);
		}
	}
}

void Graphics::DrawFlatBottomTriangle(const Vecf2 & v0, const Vecf2 & v1, const Vecf2 & v2, Color c)
{
	// Calculate gradients run/rise since triangles can have x-x = 0 but not y-y for slopes
	float m0 = (v1.x - v0.x) / (v1.y - v0.y);
	float m1 = (v2.x - v0.x) / (v2.y - v0.y);

	// Calculate the start and end of scanlines, LINES not the pixels
	const int yStart = (int)ceil(v0.y - 0.5f);
	const int yEnd = (int)ceil(v1.y - 0.5f);

	for (int y = yStart; y < yEnd; y++) {
		// Calculate xStart xEnd
		const float x0 = m0 * (float(y) + 0.5f - v0.y) + v0.x;
		const float x1 = m1 * (float(y) + 0.5f - v0.y) + v0.x;

		// calculate start and end pixels for left rast rule
		const int xStart = (int)ceilf(x0 - 0.5f);
		const int xEnd = (int)ceilf(x1 - 0.5f);

		for (int x = xStart; x < xEnd; x++) {
			PutPixel(x, y, c);
		}
	}
}

void Graphics::DrawFlatTopTriangleTex(const TextureVertex & v0, const TextureVertex & v1, const TextureVertex & v2, Surface & tex)
{
	//// calculate change of various coords per unit y
	//const TextureVertex unitChangeLeft = (v2 - v0) / (v2.pos.y - v0.pos.y);
	//const TextureVertex unitChangeRight = (v2 - v1) / (v2.pos.y - v0.pos.y);
	//// calculate xystart
	//const int ystart = (int)ceil(v0.pos.y - 0.5);
	//const int yend = (int)ceil(v2.pos.y - 0.5);

	//// interpolant
	//TextureVertex itEdge0 = v0;
	//TextureVertex itEdge1 = v1;

	//// interpolant prestep
	//itEdge0 += unitChangeLeft * ((float)ystart + 0.5f - v1.pos.y);
	//itEdge1 += unitChangeRight * ((float)ystart + 0.5f - v1.pos.y);

	//// init tex width/height and clamp values
	//const float tex_width = float(tex.GetWidth());
	//const float tex_height = float(tex.GetHeight());
	//const float tex_clamp_x = tex_width - 1.0f;
	//const float tex_clamp_y = tex_height - 1.0f;

	//for (int y = ystart; y < yend; y++, itEdge0+=unitChangeLeft, itEdge1+=unitChangeRight) {
	//	const int xstart = (int)ceil(itEdge0.pos.x - 0.5f);
	//	const int xend = (int)ceil(itEdge1.pos.x - 0.5f);
	//	const Vecf2 unitChangeX = (itEdge1.texpos - itEdge0.texpos) / (itEdge1.pos.x - itEdge0.pos.x);
	//	// prestep
	//	Vecf2 TexCoord = itEdge0.texpos + (unitChangeX * (float(xstart) + 0.5f - itEdge0.pos.x));
	//	for (int x = xstart; x < xend; x++, TexCoord += unitChangeX) {
	//		PutPixel(x, y, tex.GetPixel(
	//			int(std::min(TexCoord.x * tex_width, tex_clamp_x)),
	//			int(std::min(TexCoord.y * tex_height, tex_clamp_y))));
	//	}
	//}

	// get the rate of changes
	const TextureVertex changeRight = (v2 - v1) / (v2.pos.y - v1.pos.y);
	const TextureVertex changeLeft = (v2 - v0) / (v2.pos.y - v0.pos.y);

	// create interpolants
	TextureVertex changeRightInterpolant = v1;
	TextureVertex changeLeftInterpolant = v0;

	// get y start and end
	int yStart = (int)ceil(v0.pos.y - 0.5f);
	int yEnd = (int)ceil(v2.pos.y - 0.5f);

	// get prestep
	float prestep = ((float)yStart + 0.5f) - v0.pos.y;

	// prestep interpolants
	changeRightInterpolant += changeRight * prestep;
	changeLeftInterpolant += changeLeft * prestep;

	// get texture information
	const float tex_width = float(tex.GetWidth());
	const float tex_height = float(tex.GetHeight());
	const float tex_clamp_x = tex_width - 1.0f;
	const float tex_clamp_y = tex_height - 1.0f;

	// loop for y
	for (int y = yStart; y < yEnd; y++, changeRightInterpolant += changeRight, changeLeftInterpolant += changeLeft) {
		// get x start and end
		int xStart = (int)ceil(changeLeftInterpolant.pos.x - 0.5f);
		int xEnd = (int)ceil(changeRightInterpolant.pos.x - 0.5f);

		// create dxtex/dxscreen
		const Vecf2 changeInX = (changeRightInterpolant.texpos - changeLeftInterpolant.texpos) / (changeRightInterpolant.pos.x - changeLeftInterpolant.pos.x);

		// create interpolant x
		Vecf2 interpolantX = changeLeftInterpolant.texpos;

		// get prestep
		float xPrestep = ((float)xStart + 0.5f) - changeLeftInterpolant.pos.x;

		// pre step interpolant
		interpolantX += changeInX * xPrestep;

		// loop for x
		for (int x = xStart; x < xEnd; x++, interpolantX += changeInX) {
			PutPixel(x, y, tex.GetPixel(
				(int)std::min(interpolantX.x * tex_width, tex_clamp_x),
				(int)std::min(interpolantX.y * tex_height, tex_clamp_y)));
		}
	}
}

void Graphics::DrawFlatBottomTriangleTex(const TextureVertex & v0, const TextureVertex & v1, const TextureVertex & v2, Surface & tex)
{
	// get the rate of changes
	const TextureVertex changeRight = (v2 - v0) / (v2.pos.y - v0.pos.y);
	const TextureVertex changeLeft = (v1 - v0) / (v1.pos.y - v0.pos.y);

	// create interpolants
	TextureVertex changeRightInterpolant = v0;
	TextureVertex changeLeftInterpolant = v0;

	// get y start and end
	int yStart = (int)ceil(v0.pos.y - 0.5f);
	int yEnd = (int)ceil(v1.pos.y - 0.5f);

	// get prestep
	float prestep = ((float)yStart + 0.5f) - v0.pos.y;

	// prestep interpolants
	changeRightInterpolant += changeRight * prestep;
	changeLeftInterpolant += changeLeft * prestep;

	// get texture information
	const float tex_width = float(tex.GetWidth());
	const float tex_height = float(tex.GetHeight());
	const float tex_clamp_x = tex_width - 1.0f;
	const float tex_clamp_y = tex_height - 1.0f;

	// loop for y
	for (int y = yStart; y < yEnd; y++, changeRightInterpolant += changeRight, changeLeftInterpolant += changeLeft) {
		// get x start and end
		int xStart = (int)ceil(changeLeftInterpolant.pos.x - 0.5f);
		int xEnd = (int)ceil(changeRightInterpolant.pos.x - 0.5f);

		// create dxtex/dxscreen
		const Vecf2 changeInX = (changeRightInterpolant.texpos - changeLeftInterpolant.texpos) / (changeRightInterpolant.pos.x - changeLeftInterpolant.pos.x);
		
		// create interpolant x
		Vecf2 interpolantX = changeLeftInterpolant.texpos;

		// get prestep
		float xPrestep = ((float)xStart + 0.5f) - changeLeftInterpolant.pos.x;

		// pre step interpolant
		interpolantX += changeInX * xPrestep;

		// loop for x
		for (int x = xStart; x < xEnd; x++, interpolantX += changeInX) {
			PutPixel(x, y, tex.GetPixel(
				(int)std::min(interpolantX.x * tex_width, tex_clamp_x),
				(int)std::min(interpolantX.y * tex_height, tex_clamp_y)));
		}
	}
}

void Graphics::EndFrame()
{
	HRESULT hr;

	// lock and map the adapter memory for copying over the sysbuffer
	if( FAILED( hr = pImmediateContext->Map( pSysBufferTexture.Get(),0u,
		D3D11_MAP_WRITE_DISCARD,0u,&mappedSysBufferTexture ) ) )
	{
		throw CHILI_GFX_EXCEPTION( hr,L"Mapping sysbuffer" );
	}
	// setup parameters for copy operation
	Color* pDst = reinterpret_cast<Color*>(mappedSysBufferTexture.pData );
	const size_t dstPitch = mappedSysBufferTexture.RowPitch / sizeof( Color );
	const size_t srcPitch = Graphics::ScreenWidthDisplay;
	const size_t rowBytes = srcPitch * sizeof( Color );
	// perform the copy line-by-line
	for( size_t y = 0u; y < Graphics::ScreenHeightDisplay; y++ )
	{
		memcpy( &pDst[ y * dstPitch ],&pSysBuffer[y * srcPitch],rowBytes );
	}
	// release the adapter memory
	pImmediateContext->Unmap( pSysBufferTexture.Get(),0u );

	// render offscreen scene texture to back buffer
	pImmediateContext->IASetInputLayout( pInputLayout.Get() );
	pImmediateContext->VSSetShader( pVertexShader.Get(),nullptr,0u );
	pImmediateContext->PSSetShader( pPixelShader.Get(),nullptr,0u );
	pImmediateContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
	const UINT stride = sizeof( FSQVertex );
	const UINT offset = 0u;
	pImmediateContext->IASetVertexBuffers( 0u,1u,pVertexBuffer.GetAddressOf(),&stride,&offset );
	pImmediateContext->PSSetShaderResources( 0u,1u,pSysBufferTextureView.GetAddressOf() );
	pImmediateContext->PSSetSamplers( 0u,1u,pSamplerState.GetAddressOf() );
	pImmediateContext->Draw( 6u,0u );

	// flip back/front buffers
	if( FAILED( hr = pSwapChain->Present( 1u,0u ) ) )
	{
		if( hr == DXGI_ERROR_DEVICE_REMOVED )
		{
			throw CHILI_GFX_EXCEPTION( pDevice->GetDeviceRemovedReason(),L"Presenting back buffer [device removed]" );
		}
		else
		{
			throw CHILI_GFX_EXCEPTION( hr,L"Presenting back buffer" );
		}
	}
}

void Graphics::BeginFrame()
{
	// clear the sysbuffer
	memset( pSysBuffer,0u,sizeof( Color ) * Graphics::ScreenHeightDisplay * Graphics::ScreenWidthDisplay );
}

void Graphics::PutPixel( int x,int y,Color c )
{
	assert( x >= 0 );
	assert( x < int( Graphics::ScreenWidthDisplay ) );
	assert( y >= 0 );
	assert( y < int( Graphics::ScreenHeightDisplay ) );
	//pSysBuffer[Graphics::ScreenWidthDisplay * y + x] = c;
	for (int X = 0; X < screenWidthRatio; X++) {
		for (int Y = 0; Y < screenHeightRatio; Y++) {
			pSysBuffer[int(y*screenHeightRatio + Y + pixelOffsetY)*Graphics::ScreenWidthDisplay + int(x*screenWidthRatio + X + pixelOffsetX)] = c;
		}
	}
	//pSysBuffer[(int(y*screenHeightRatio) + pixelOffsetY)*Graphics::ScreenWidthDisplay + (int(x*screenWidthRatio) + pixelOffsetX)] = c;
}

void Graphics::PutPixelGUI(int x, int y, Color c)
{
	assert(x >= 0);
	assert(x < int(Graphics::ScreenWidthDisplay));
	assert(y >= 0);
	assert(y < int(Graphics::ScreenHeightDisplay));
	pSysBuffer[Graphics::ScreenWidthDisplay * y + x] = c;
}

void Graphics::DrawSprite(int x, int y, Surface surf)
{
	const int width = surf.GetWidth();
	const int height = surf.GetHeight();
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			PutPixel(x + j, y + i, surf.GetPixel(j, i));
		}
	}
}


//////////////////////////////////////////////////
//           Graphics Exception
Graphics::Exception::Exception( HRESULT hr,const std::wstring& note,const wchar_t* file,unsigned int line )
	:
	ChiliException( file,line,note ),
	hr( hr )
{}

std::wstring Graphics::Exception::GetFullMessage() const
{
	const std::wstring empty = L"";
	const std::wstring errorName = GetErrorName();
	const std::wstring errorDesc = GetErrorDescription();
	const std::wstring& note = GetNote();
	const std::wstring location = GetLocation();
	return    (!errorName.empty() ? std::wstring( L"Error: " ) + errorName + L"\n"
		: empty)
		+ (!errorDesc.empty() ? std::wstring( L"Description: " ) + errorDesc + L"\n"
			: empty)
		+ (!note.empty() ? std::wstring( L"Note: " ) + note + L"\n"
			: empty)
		+ (!location.empty() ? std::wstring( L"Location: " ) + location
			: empty);
}

std::wstring Graphics::Exception::GetErrorName() const
{
	return DXGetErrorString( hr );
}

std::wstring Graphics::Exception::GetErrorDescription() const
{
	std::array<wchar_t,512> wideDescription;
	DXGetErrorDescription( hr,wideDescription.data(),wideDescription.size() );
	return wideDescription.data();
}

std::wstring Graphics::Exception::GetExceptionType() const
{
	return L"Chili Graphics Exception";
}