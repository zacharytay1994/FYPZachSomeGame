/******************************************************************************************
*	Chili DirectX Framework Version 16.07.20											  *
*	Graphics.h																			  *
*	Copyright 2016 PlanetChili <http://www.planetchili.net>								  *
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
#pragma once
#include "ChiliWin.h"
#include <d3d11.h>
#include <wrl.h>
#include "ChiliException.h"
#include "Colors.h"
#include "Vec2.h"
#include "Vec3.h"
#include "Surface.h"
#include "TextureVertex.h"
#include "ZBuffer.h"

class Graphics
{
public:
	class Exception : public ChiliException
	{
	public:
		Exception( HRESULT hr,const std::wstring& note,const wchar_t* file,unsigned int line );
		std::wstring GetErrorName() const;
		std::wstring GetErrorDescription() const;
		virtual std::wstring GetFullMessage() const override;
		virtual std::wstring GetExceptionType() const override;
	private:
		HRESULT hr;
	};
private:
	// vertex format for the framebuffer fullscreen textured quad
	struct FSQVertex
	{
		float x,y,z;		// position
		float u,v;			// texcoords
	};
public:
	Graphics( class HWNDKey& key );
	Graphics( const Graphics& ) = delete;
	Graphics& operator=( const Graphics& ) = delete;
	void EndFrame();
	void BeginFrame();
	void PutPixel( int x,int y,int r,int g,int b )
	{
		PutPixel( x,y,{ unsigned char( r ),unsigned char( g ),unsigned char( b ) } );
	}
	void PutPixel( int x,int y,Color c );
	void DrawSprite(int x, int y, Surface surf);
	void DrawLine(Vecf2 point1, Vecf2 point2, Color c) {
		float m;
		// Calculate gradient rise/run
		if (point1.x != point2.x) {
			m = (point2.y - point1.y) / (point2.x - point1.x);
			// Calculate y-intercept c = y - mx
			const float intercept = point1.y - m * point1.x;
			// Check if m is too steep, if so loop with y
			if (abs(m) > 1) {
				// Swap make sure y1 < y0 {
				if (point1.y > point2.y) {
					std::swap(point1, point2);
				}
				for (float i = point1.y; i < point2.y; i++) {
					const float x = (i - intercept) / m;
					if ((int)x > 0 && (int)x < ScreenWidth && (int)i > 0 && (int)i < ScreenHeight) {
						PutPixel((int)x, (int)i, c);
					}
				}
			}
			else {
				// Making sure point1.x < point2.x for loop start-end
				if (point1.x > point2.x) {
					std::swap(point1, point2);
				}
				// Draw line
				for (float i = point1.x; i < point2.x; i++) {
					const float y = m * i + intercept;
					if ((int)i > 0 && (int)i < ScreenWidth && (int)y > 0 && (int)y < ScreenHeight) {
						PutPixel((int)i, (int)y, c);
					}
				}
			}
		}
		else {
			// Swap make sure y1 < y2 {
			if (point1.y > point2.y) {
				std::swap(point1, point2);
			}
			for (float i = point1.y; i < point2.y; i++) {
				if ((int)point1.x > 0 && (int)point1.x < ScreenWidth && (int)i > 0 && (int)i < ScreenHeight) {
					PutPixel((int)point1.x, (int)i, c);
				}
				
			}
		}
	};
	void DrawTriangle(const Vecf3& p1, const Vecf3& p2, const Vecf3& p3, Color c);
	void DrawTriangleTex(const TextureVertex& v0, const TextureVertex& v1, const TextureVertex& v2, Surface& tex);
	~Graphics();
private:
	void DrawFlatTopTriangle(const Vecf2& v0, const Vecf2& v1, const Vecf2& v2, Color c);
	void DrawFlatBottomTriangle(const Vecf2& v0, const Vecf2& v1, const Vecf2& v2, Color c);
	void DrawFlatTopTriangleTex(const TextureVertex& v0, const TextureVertex& v1, const TextureVertex& v2, Surface& tex);
	void DrawFlatBottomTriangleTex(const TextureVertex& v0, const TextureVertex& v1, const TextureVertex& v2, Surface& tex);
private:
	Microsoft::WRL::ComPtr<IDXGISwapChain>				pSwapChain;
	Microsoft::WRL::ComPtr<ID3D11Device>				pDevice;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext>			pImmediateContext;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>		pRenderTargetView;
	Microsoft::WRL::ComPtr<ID3D11Texture2D>				pSysBufferTexture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	pSysBufferTextureView;
	Microsoft::WRL::ComPtr<ID3D11PixelShader>			pPixelShader;
	Microsoft::WRL::ComPtr<ID3D11VertexShader>			pVertexShader;
	Microsoft::WRL::ComPtr<ID3D11Buffer>				pVertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11InputLayout>			pInputLayout;
	Microsoft::WRL::ComPtr<ID3D11SamplerState>			pSamplerState;
	D3D11_MAPPED_SUBRESOURCE							mappedSysBufferTexture;
	Color*                                              pSysBuffer = nullptr;
public:
	static constexpr int ScreenWidth = 960;
	static constexpr int ScreenHeight = 540;
};