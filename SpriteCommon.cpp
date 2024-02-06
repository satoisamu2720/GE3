#include "SpriteCommon.h"

#include <cassert>


#pragma comment(lib,"dxcompiler.lib")

using namespace Microsoft::WRL;

void SpriteCommon::Initialize(DirectXCommon* dxCommon)
{
	HRESULT result{};

	dxCommon_ = dxCommon;

	//DXC初期化
	ComPtr<IDxcUtils> dxcUtils;

	ComPtr<IDxcCompiler3> dxcCompiler;
	ComPtr<IDxcIncludeHandler> includeHandler;

	result = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&dxcUtils));
	assert(SUCCEEDED(result));
	result = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&dxcCompiler));
	assert(SUCCEEDED(result));

	result = dxcUtils->CreateDefaultIncludeHandler(&includeHandler);
	assert(SUCCEEDED(result));

	//RootSignatureも作成
	D3D12_ROOT_SIGNATURE_DESC descriptorRootSignature{};
	descriptorRootSignature.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	//RooParameter作成
	D3D12_ROOT_PARAMETER rootParameters[1]{};
	// 色
	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameters[0].Descriptor.ShaderRegister = 0;

	descriptorRootSignature.pParameters = rootParameters;
	descriptorRootSignature.NumParameters = _countof(rootParameters);


		//シリアライズとしてバイナリにする
		ComPtr<ID3D10Blob> signatureBlob;
	ComPtr<ID3D10Blob> errorBlob;

	result = D3D12SerializeRootSignature(&descriptorRootSignature, D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
	if (FAILED(result)) {
		assert(false);
	}

	//バイナリを元に作成
	result = dxCommon_->GetDevice()->CreateRootSignature(0, signatureBlob->GetBufferPointer(),
		signatureBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignature));
	assert(SUCCEEDED(result));

	//InPutLayout
	D3D12_INPUT_ELEMENT_DESC inputElementDesc[1] = {};
	inputElementDesc[0].SemanticName = "POSITION";
	inputElementDesc[0].SemanticIndex = 0;
	inputElementDesc[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDesc[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
	inputLayoutDesc.pInputElementDescs = inputElementDesc;
	inputLayoutDesc.NumElements = _countof(inputElementDesc);

	//BlendState
	D3D12_BLEND_DESC blendDesc{};
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	//Rasterizer
	D3D12_RASTERIZER_DESC rasterizerDesc{};
	//裏表を描画しない
	rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	//塗りつぶす
	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;

	//読み込み処理
	ComPtr<IDxcBlob> vertexShaderBlob = CompileShader(L"Resources/shaders/BasicVS.hlsl",
		L"vs_6_0", dxcUtils.Get(), dxcCompiler.Get(), includeHandler.Get());
	assert(SUCCEEDED(result));

	ComPtr<IDxcBlob> pixcelShaderBlob = CompileShader(L"Resources/shaders/BasicPS.hlsl",
		L"ps_6_0", dxcUtils.Get(), dxcCompiler.Get(), includeHandler.Get());

	//PipelineState
	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
	graphicsPipelineStateDesc.pRootSignature = rootSignature.Get();
	graphicsPipelineStateDesc.InputLayout = inputLayoutDesc;
	graphicsPipelineStateDesc.VS = { vertexShaderBlob->GetBufferPointer(),vertexShaderBlob->GetBufferSize() };
	graphicsPipelineStateDesc.PS = { pixcelShaderBlob->GetBufferPointer(),pixcelShaderBlob->GetBufferSize() };
	graphicsPipelineStateDesc.BlendState = blendDesc;
	graphicsPipelineStateDesc.RasterizerState = rasterizerDesc;

	graphicsPipelineStateDesc.NumRenderTargets = 1;
	graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;

	graphicsPipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	graphicsPipelineStateDesc.SampleDesc.Count = 1;
	graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

	result = dxCommon_->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc, IID_PPV_ARGS(&pipelineState));
	assert(SUCCEEDED(result));

}

IDxcBlob* SpriteCommon::CompileShader(const std::wstring& filePath, const wchar_t* profile, IDxcUtils* dxcUtils, IDxcCompiler3* dxcCompiler, IDxcIncludeHandler* dxcIncludeHandler)
{
	//hlsl
	IDxcBlobEncoding* shaderSource = nullptr;
	HRESULT result = dxcUtils->LoadFile(filePath.c_str(), nullptr, &shaderSource);
	assert(SUCCEEDED(result));

	//読み込んだファイル
	DxcBuffer shaderSourceBeffer;
	shaderSourceBeffer.Ptr = shaderSource->GetBufferPointer();
	shaderSourceBeffer.Size = shaderSource->GetBufferSize();
	shaderSourceBeffer.Encoding = DXC_CP_UTF8;

	LPCWSTR arguments[] = {
		filePath.c_str(),
		L"-E",L"main",
		L"-T",profile,
		L"-Zi",L"-Qembed_debug",
		L"-Od",
		L"-Zpr"
	};

	IDxcResult* shaderResult = nullptr;
	result = dxcCompiler->Compile(
		&shaderSourceBeffer,
		arguments,
		_countof(arguments),
		dxcIncludeHandler,
		IID_PPV_ARGS(&shaderResult)
	);
	assert(SUCCEEDED(result));

	IDxcBlobUtf8* shaderError = nullptr;
	shaderResult->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&shaderError), nullptr);
	if (shaderError != nullptr && shaderError->GetStringLength() != 0) {
		//Log(shaderError->GetStringPointer());
		assert(SUCCEEDED(result));
	}

	IDxcBlob* shaderBlob = nullptr;
	result = shaderResult->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&shaderBlob), nullptr);
	assert(SUCCEEDED(result));
	//Log(ConvertString(std::format(L"Compile Succeeded, path:{},profile:{}\n", filePath, profile)));

	shaderSource->Release();
	shaderResult->Release();

	return shaderBlob;
}