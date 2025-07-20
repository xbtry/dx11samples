#pragma once
#include "pch.h"

class Shaders
{
public:
	Shaders() = default;
	~Shaders();
	bool Load
	(
		ID3D11Device* device,
		const std::wstring& vertexShaderFilePath,
		const std::wstring& pixelShaderFilePath
		//const D3D11_INPUT_ELEMENT_DESC* layout,
		//UINT layoutCount
	);
	bool LoadTexture(ID3D11Device* device, const std::wstring& textureFilePath);
	void Bind(ID3D11DeviceContext* context);
	void BuildInputLayout(
		ID3D11Device* device, 
		const D3D11_INPUT_ELEMENT_DESC* layout,
		UINT layoutCount
	);
	void CompileShaders(ID3D11Device* device, const std::wstring& vertexShaderFilePath, const std::wstring& pixelShaderFilePath);
private:
	std::wstring path;
	Microsoft::WRL::ComPtr<ID3D11VertexShader> m_vertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> m_pixelShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_textureView;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> m_samplerState;
	Microsoft::WRL::ComPtr<ID3DBlob> m_vertexShaderBlob;
	Microsoft::WRL::ComPtr<ID3DBlob> m_pixelShaderBlob;
};