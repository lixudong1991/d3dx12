#include "DrawTopologyTypeTest.h"

DrawTopologyTypeTest::DrawTopologyTypeTest(HINSTANCE hInstance, const std::vector<Vertex>& vertexs, const std::vector<std::uint16_t>& indices, DrawTopologyType type):
	D3DApp(hInstance),
	m_vdata(vertexs),
	m_idata(indices)
{
	switch (type)
	{
	case POINTLIST:
		m_topologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
		m_topology = D3D_PRIMITIVE_TOPOLOGY_POINTLIST;
		break;
	case LINESTRIP:
		m_topologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
		m_topology = D3D_PRIMITIVE_TOPOLOGY_LINESTRIP;
		break;
	case LINELIST:
		m_topologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
		m_topology = D3D_PRIMITIVE_TOPOLOGY_LINELIST;
		break;
	case TRIANGLESTRIP:
		m_topologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		m_topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
		break;
	case TRIANGLELIST:
		m_topologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		m_topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		break;
	default:
		m_topologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		m_topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		break;
	}
}

DrawTopologyTypeTest::~DrawTopologyTypeTest()
{
}

bool DrawTopologyTypeTest::Initialize()
{
	if (!D3DApp::Initialize())
		return false;

	// Reset the command list to prep for initialization commands.
	ThrowIfFailed(mCommandList->Reset(mDirectCmdListAlloc.Get(), nullptr));

	BuildGeometry();
	BuildDescriptorHeaps();
	BuildConstantBuffers();
	BuildRootSignature();
	BuildShadersAndInputLayout();
	BuildPSO();

	// Execute the initialization commands.
	ThrowIfFailed(mCommandList->Close());
	ID3D12CommandList* cmdsLists[] = { mCommandList.Get() };
	mCommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

	// Wait until initialization is complete.
	FlushCommandQueue();

	return true;
}

void DrawTopologyTypeTest::Update(const GameTimer& gt)
{
	// Convert Spherical to Cartesian coordinates.
	float x = mRadius * sinf(mPhi) * cosf(mTheta);
	float z = mRadius * sinf(mPhi) * sinf(mTheta);
	float y = mRadius * cosf(mPhi);

	// Build the view matrix.
	XMVECTOR pos = XMVectorSet(x, y, z, 1.0f);
	XMVECTOR target = XMVectorZero();
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	XMMATRIX view = XMMatrixLookAtLH(pos, target, up);
	XMStoreFloat4x4(&mView, view);

	XMMATRIX world = XMLoadFloat4x4(&mWorld1);
	XMMATRIX proj = XMLoadFloat4x4(&mProj);
	XMMATRIX worldViewProj = world * view * proj;

	// Update the constant buffer with the latest worldViewProj matrix.
	ObjectConstants objConstants;
	XMStoreFloat4x4(&objConstants.WorldViewProj, worldViewProj);
	mObjectCB->CopyData(0, objConstants);

	 world = XMLoadFloat4x4(&mWorld2);
	 proj = XMLoadFloat4x4(&mProj);
	 worldViewProj = world * view * proj;
	XMStoreFloat4x4(&objConstants.WorldViewProj, worldViewProj);
	mObjectCB->CopyData(1, objConstants);
}

void DrawTopologyTypeTest::Draw(const GameTimer& gt)
{
	ThrowIfFailed(mDirectCmdListAlloc->Reset());

	ThrowIfFailed(mCommandList->Reset(mDirectCmdListAlloc.Get(), mPSO.Get()));
	mCommandList->RSSetViewports(1, &mScreenViewport);
	mCommandList->RSSetScissorRects(1, &mScissorRect);

	CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(),
		D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
	D3D12_CPU_DESCRIPTOR_HANDLE rendertargetview = CurrentBackBufferView();
	D3D12_CPU_DESCRIPTOR_HANDLE depthview = DepthStencilView();
	// Indicate a state transition on the resource usage.
	mCommandList->ResourceBarrier(1, &barrier);
	// Clear the back buffer and depth buffer.
	mCommandList->ClearRenderTargetView(rendertargetview, Colors::LightSteelBlue, 0, nullptr);
	mCommandList->ClearDepthStencilView(depthview, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

	// Specify the buffers we are going to render to.
	mCommandList->OMSetRenderTargets(1, &rendertargetview, true, &depthview);

	mCommandList->SetGraphicsRootSignature(mRootSignature.Get());

	D3D12_VERTEX_BUFFER_VIEW vertexView = mBoxGeo->VertexBufferView();
	D3D12_INDEX_BUFFER_VIEW  indexView = mBoxGeo->IndexBufferView();
	mCommandList->IASetVertexBuffers(0, 1, &vertexView);
	mCommandList->IASetIndexBuffer(&indexView);
	mCommandList->IASetPrimitiveTopology(m_topology);

	ID3D12DescriptorHeap* descriptorHeaps[] = { mCbvHeap.Get() };
	mCommandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);
	int cbvDescriptorSize = md3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	CD3DX12_GPU_DESCRIPTOR_HANDLE rtvHandle(mCbvHeap->GetGPUDescriptorHandleForHeapStart(), 0, cbvDescriptorSize);
	mCommandList->SetGraphicsRootDescriptorTable(0, rtvHandle);

	mCommandList->DrawIndexedInstanced(
		mBoxGeo->DrawArgs["quad1"].IndexCount,
		1, mBoxGeo->DrawArgs["quad1"].StartIndexLocation, mBoxGeo->DrawArgs["quad1"].BaseVertexLocation, 0);

	rtvHandle.Offset(1,cbvDescriptorSize);
	mCommandList->SetGraphicsRootDescriptorTable(0, rtvHandle);
	mCommandList->DrawIndexedInstanced(
		mBoxGeo->DrawArgs["quad2"].IndexCount,
		1, mBoxGeo->DrawArgs["quad2"].StartIndexLocation, mBoxGeo->DrawArgs["quad2"].BaseVertexLocation, 0);

	// Indicate a state transition on the resource usage.
	barrier = CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(),
		D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
	mCommandList->ResourceBarrier(1, &barrier);
	// Done recording commands.
	ThrowIfFailed(mCommandList->Close());

	// Add the command list to the queue for execution.
	ID3D12CommandList* cmdsLists[] = { mCommandList.Get() };
	mCommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

	// swap the back and front buffers
	ThrowIfFailed(mSwapChain->Present(0, 0));
	mCurrBackBuffer = (mCurrBackBuffer + 1) % SwapChainBufferCount;

	// Wait until frame commands are complete.  This waiting is inefficient and is
	// done for simplicity.  Later we will show how to organize our rendering code
	// so we do not have to wait per frame.
	FlushCommandQueue();
}

void DrawTopologyTypeTest::OnResize()
{
	D3DApp::OnResize();

	// The window resized, so update the aspect ratio and recompute the projection matrix.
	XMMATRIX P = XMMatrixPerspectiveFovLH(0.25f * MathHelper::Pi, AspectRatio(), 1.0f, 1000.0f);
	XMStoreFloat4x4(&mProj, P);
}

void DrawTopologyTypeTest::OnMouseDown(WPARAM btnState, int x, int y)
{
	mLastMousePos.x = x;
	mLastMousePos.y = y;

	SetCapture(mhMainWnd);
}

void DrawTopologyTypeTest::OnMouseUp(WPARAM btnState, int x, int y)
{
	ReleaseCapture();
}

void DrawTopologyTypeTest::OnMouseMove(WPARAM btnState, int x, int y)
{
	if ((btnState & MK_LBUTTON) != 0)
	{
		// Make each pixel correspond to a quarter of a degree.
		float dx = XMConvertToRadians(0.25f * static_cast<float>(x - mLastMousePos.x));
		float dy = XMConvertToRadians(0.25f * static_cast<float>(y - mLastMousePos.y));

		// Update angles based on input to orbit camera around box.
		mTheta -= dx;
		mPhi -= dy;

		// Restrict the angle mPhi.
		mPhi = MathHelper::Clamp(mPhi, 0.1f, MathHelper::Pi - 0.1f);
	}
	else if ((btnState & MK_RBUTTON) != 0)
	{
		// Make each pixel correspond to 0.005 unit in the scene.
		float dx = 0.005f * static_cast<float>(x - mLastMousePos.x);
		float dy = 0.005f * static_cast<float>(y - mLastMousePos.y);

		// Update the camera radius based on input.
		mRadius += dx - dy;

		// Restrict the radius.
		mRadius = MathHelper::Clamp(mRadius, 3.0f, 15.0f);
	}

	mLastMousePos.x = x;
	mLastMousePos.y = y;
}

void DrawTopologyTypeTest::BuildDescriptorHeaps()
{
	D3D12_DESCRIPTOR_HEAP_DESC cbvHeapDesc;
	cbvHeapDesc.NumDescriptors = 2;
	cbvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	cbvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	cbvHeapDesc.NodeMask = 0;
	ThrowIfFailed(md3dDevice->CreateDescriptorHeap(
		&cbvHeapDesc, IID_PPV_ARGS(mCbvHeap.GetAddressOf())));
}

void DrawTopologyTypeTest::BuildConstantBuffers()
{
	mObjectCB = std::make_unique<UploadBuffer<ObjectConstants>>(md3dDevice.Get(), 2, true);
	UINT objCBByteSize = d3dUtil::CalcConstantBufferByteSize(sizeof(ObjectConstants));

	D3D12_GPU_VIRTUAL_ADDRESS cbAddress = mObjectCB->Resource()->GetGPUVirtualAddress();
	// Offset to the ith object constant buffer in the buffer.
	int boxCBufIndex = 0;
	cbAddress += boxCBufIndex * objCBByteSize;

	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
	cbvDesc.BufferLocation = cbAddress;
	cbvDesc.SizeInBytes = d3dUtil::CalcConstantBufferByteSize(sizeof(ObjectConstants));

	md3dDevice->CreateConstantBufferView(
		&cbvDesc,
		mCbvHeap->GetCPUDescriptorHandleForHeapStart());

	boxCBufIndex = 1;
	cbAddress += boxCBufIndex * objCBByteSize;
	cbvDesc.BufferLocation = cbAddress;

	int cbvDescriptorSize = md3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(mCbvHeap->GetCPUDescriptorHandleForHeapStart());

	rtvHandle.Offset(boxCBufIndex, cbvDescriptorSize);

	md3dDevice->CreateConstantBufferView(
		&cbvDesc,
		rtvHandle);
}

void DrawTopologyTypeTest::BuildRootSignature()
{
	CD3DX12_ROOT_PARAMETER slotRootParameter[1];
	// Create a single descriptor table of CBVs.
	CD3DX12_DESCRIPTOR_RANGE cbvTable[1];
	cbvTable[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);
	slotRootParameter[0].InitAsDescriptorTable(1, cbvTable);

	// A root signature is an array of root parameters.
	CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc(1, slotRootParameter, 0, nullptr,
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT | // we can deny shader stages here for better performance
		D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS);

	// create a root signature with a single slot which points to a descriptor range consisting of a single constant buffer
	ComPtr<ID3DBlob> serializedRootSig = nullptr;
	ComPtr<ID3DBlob> errorBlob = nullptr;
	HRESULT hr = D3D12SerializeRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1,
		serializedRootSig.GetAddressOf(), errorBlob.GetAddressOf());

	if (errorBlob != nullptr)
	{
		::OutputDebugStringA((char*)errorBlob->GetBufferPointer());
	}
	ThrowIfFailed(hr);
	ThrowIfFailed(md3dDevice->CreateRootSignature(
		0,
		serializedRootSig->GetBufferPointer(),
		serializedRootSig->GetBufferSize(),
		IID_PPV_ARGS(&mRootSignature)));
}

void DrawTopologyTypeTest::BuildShadersAndInputLayout()
{
	mvsByteCode = d3dUtil::CompileShader(L"Shaders\\VertexShader.hlsl", nullptr, "main", "vs_5_0");
	mpsByteCode = d3dUtil::CompileShader(L"Shaders\\PixelShader.hlsl", nullptr, "main", "ps_5_0");
	mInputLayout =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	};
}

void DrawTopologyTypeTest::BuildGeometry()
{
	const UINT vbByteSize = (UINT)m_vdata.size() * sizeof(Vertex);
	const UINT ibByteSize = (UINT)m_idata.size() * sizeof(std::uint16_t);

	mBoxGeo = std::make_unique<MeshGeometry>();
	mBoxGeo->Name = "boxGeo";

	ThrowIfFailed(D3DCreateBlob(vbByteSize, &mBoxGeo->VertexBufferCPU));
	CopyMemory(mBoxGeo->VertexBufferCPU->GetBufferPointer(), m_vdata.data(), vbByteSize);

	ThrowIfFailed(D3DCreateBlob(ibByteSize, &mBoxGeo->IndexBufferCPU));
	CopyMemory(mBoxGeo->IndexBufferCPU->GetBufferPointer(), m_idata.data(), ibByteSize);

	mBoxGeo->VertexBufferGPU = d3dUtil::CreateDefaultBuffer(md3dDevice.Get(),
		mCommandList.Get(), m_vdata.data(), vbByteSize, mBoxGeo->VertexBufferUploader);

	mBoxGeo->IndexBufferGPU = d3dUtil::CreateDefaultBuffer(md3dDevice.Get(),
		mCommandList.Get(), m_idata.data(), ibByteSize, mBoxGeo->IndexBufferUploader);

	mBoxGeo->VertexByteStride = sizeof(Vertex);
	mBoxGeo->VertexBufferByteSize = vbByteSize;
	mBoxGeo->IndexFormat = DXGI_FORMAT_R16_UINT;
	mBoxGeo->IndexBufferByteSize = ibByteSize;

	SubmeshGeometry submesh;
	submesh.IndexCount = (UINT)18;
	submesh.StartIndexLocation = 0;
	submesh.BaseVertexLocation = 0;

	mBoxGeo->DrawArgs["quad1"] = submesh;

	submesh.IndexCount = (UINT)m_idata.size()-18;
	submesh.StartIndexLocation = 18;
	submesh.BaseVertexLocation = 5;

	mBoxGeo->DrawArgs["quad2"] = submesh;

	XMVECTOR posVec = XMVectorSet(-2.0f, -1.0f, 0.0f, 0.0f);
	XMMATRIX tmat = XMMatrixTranslationFromVector(posVec);
	XMStoreFloat4x4(&mWorld1, tmat);

	XMVECTOR posVec1 = XMVectorSet(2.0f, 0.0f, 0.0f, 0.0f);
	XMMATRIX tmat1 = XMMatrixTranslationFromVector(posVec1);
	XMStoreFloat4x4(&mWorld2, tmat1);

}

void DrawTopologyTypeTest::BuildPSO()
{
	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc;
	ZeroMemory(&psoDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
	psoDesc.InputLayout = { mInputLayout.data(), (UINT)mInputLayout.size() };
	psoDesc.pRootSignature = mRootSignature.Get();
	psoDesc.VS =
	{
		reinterpret_cast<BYTE*>(mvsByteCode->GetBufferPointer()),
		mvsByteCode->GetBufferSize()
	};
	psoDesc.PS =
	{
		reinterpret_cast<BYTE*>(mpsByteCode->GetBufferPointer()),
		mpsByteCode->GetBufferSize()
	};
	psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	//psoDesc.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;
	//psoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	psoDesc.SampleMask = UINT_MAX;
	psoDesc.PrimitiveTopologyType = m_topologyType;
	psoDesc.NumRenderTargets = 1;
	psoDesc.RTVFormats[0] = mBackBufferFormat;
	psoDesc.SampleDesc.Count =  1;
	psoDesc.SampleDesc.Quality = 0;
	psoDesc.DSVFormat = mDepthStencilFormat;
	ThrowIfFailed(md3dDevice->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&mPSO)));
}
