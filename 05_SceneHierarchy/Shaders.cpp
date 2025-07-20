#include "Shaders.h"
#include <stdexcept>

Shaders::~Shaders()
{
	// Release resources
	m_vertexShader.Reset();
	m_pixelShader.Reset();
	m_inputLayout.Reset();
	m_textureView.Reset();
	m_samplerState.Reset();
}

void Shaders::CompileShaders
(
	ID3D11Device* device,
	const std::wstring& vertexShaderFilePath,
	const std::wstring& pixelShaderFilePath
)
{
	HRESULT hr;
	// Compile Vertex Shader
	hr = D3DCompileFromFile(vertexShaderFilePath.c_str(), nullptr, nullptr, "VSMain", "vs_5_0",
		D3DCOMPILE_ENABLE_STRICTNESS, 0, m_vertexShaderBlob.GetAddressOf(), nullptr);
	if (FAILED(hr)) {
		throw std::runtime_error(WStringToString(L"Failed to compile vertex shader: " + vertexShaderFilePath));
	}
	// Compile Pixel Shader
	hr = D3DCompileFromFile(pixelShaderFilePath.c_str(), nullptr, nullptr, "PSMain", "ps_5_0",
		D3DCOMPILE_ENABLE_STRICTNESS, 0, m_pixelShaderBlob.GetAddressOf(), nullptr);
	if (FAILED(hr)) {
		throw std::runtime_error(WStringToString(L"Failed to compile pixel shader: " + pixelShaderFilePath));
	}
}

bool Shaders::Load
(
	ID3D11Device* device,
	const std::wstring& vertexShaderFilePath,
	const std::wstring& pixelShaderFilePath
	//const D3D11_INPUT_ELEMENT_DESC* layout,
	//UINT layoutCount
)
{
	HRESULT hr;

	// Load Vertex Shader
	//Microsoft::WRL::ComPtr<ID3DBlob> vertexShaderBlob;
	/*hr = D3DReadFileToBlob(vertexShaderFilePath.c_str(), m_vertexShaderBlob.GetAddressOf());
	if (FAILED(hr)) {

		throw std::runtime_error(WStringToString(L"Failed to load vertex shader file: " + vertexShaderFilePath));
		return false;
	}*/
	hr = device->CreateVertexShader(m_vertexShaderBlob->GetBufferPointer(), m_vertexShaderBlob->GetBufferSize(), nullptr, m_vertexShader.GetAddressOf());
	if (FAILED(hr)) {
		throw std::runtime_error(WStringToString(L"Failed to create vertex shader. " + vertexShaderFilePath));
		return false;
	}
	//// Create Input Layout
	//hr = device->CreateInputLayout(layout, layoutCount, m_vertexShaderBlob->GetBufferPointer(), m_vertexShaderBlob->GetBufferSize(), m_inputLayout.GetAddressOf());
	//if (FAILED(hr)) { 
	//	throw std::runtime_error(WStringToString(L"Failed to create input layout for. " + vertexShaderFilePath));
	//	return false; 
	//}
	// Load Pixel Shader
	//Microsoft::WRL::ComPtr<ID3DBlob> pixelShaderBlob;
	/*hr = D3DReadFileToBlob(pixelShaderFilePath.c_str(), m_pixelShaderBlob.GetAddressOf());
	if (FAILED(hr)) {
		throw std::runtime_error(WStringToString(L"Failed to load pixel shader file: " + pixelShaderFilePath));
		return false;
	}*/
	hr = device->CreatePixelShader(m_pixelShaderBlob->GetBufferPointer(), m_pixelShaderBlob->GetBufferSize(), nullptr, m_pixelShader.GetAddressOf());
	if (FAILED(hr)) {
		throw std::runtime_error(WStringToString(L"Failed to create pixel shader. " + pixelShaderFilePath));
		return false;
	}
	return true;
}

void Shaders::Bind(ID3D11DeviceContext* context)
{
	if (m_vertexShader) {
		context->VSSetShader(m_vertexShader.Get(), nullptr, 0);
		context->IASetInputLayout(m_inputLayout.Get());
	}
	if (m_pixelShader) {
		context->PSSetShader(m_pixelShader.Get(), nullptr, 0);
	}
	if (m_textureView) {
		context->PSSetShaderResources(0, 1, m_textureView.GetAddressOf());
	}
	if (m_samplerState) {
		context->PSSetSamplers(0, 1, m_samplerState.GetAddressOf());
	}
}

bool Shaders::LoadTexture(ID3D11Device* device, const std::wstring& textureFilePath)
{
	HRESULT hr = DirectX::CreateWICTextureFromFile(device, textureFilePath.c_str(), nullptr, m_textureView.GetAddressOf());
	if (FAILED(hr)) {
		throw std::runtime_error(WStringToString(L"Failed to load texture: " + textureFilePath));
		return false;
	}
	// Create sampler state
	D3D11_SAMPLER_DESC samplerDesc = {};
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	hr = device->CreateSamplerState(&samplerDesc, m_samplerState.GetAddressOf());
	if (FAILED(hr)) {
		throw std::runtime_error(WStringToString(L"Failed to create sampler state for texture: " + textureFilePath));
		return false;
	}
	return true;
}

void Shaders::BuildInputLayout(ID3D11Device* device, const D3D11_INPUT_ELEMENT_DESC* layout, UINT layoutCount)
{
	if (!m_vertexShaderBlob) {
		throw std::runtime_error("Vertex shader blob is not initialized.");
	}

	HRESULT hr = device->CreateInputLayout(layout, layoutCount, m_vertexShaderBlob->GetBufferPointer(), m_vertexShaderBlob->GetBufferSize(), m_inputLayout.GetAddressOf());
	if (FAILED(hr)) {
		throw std::runtime_error("Failed to create input layout.");
	}
}