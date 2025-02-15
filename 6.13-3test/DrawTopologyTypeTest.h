#pragma once
#include "../Common/d3dApp.h"
#include "../Common/UploadBuffer.h"
using Microsoft::WRL::ComPtr;
using namespace DirectX;
using namespace DirectX::PackedVector;

struct Vertex
{
	XMFLOAT3 Pos;
	XMFLOAT4 Color;
};
struct ObjectConstants
{
	XMFLOAT4X4 WorldViewProj = MathHelper::Identity4x4();
};

enum  DrawTopologyType {
	POINTLIST=0,
	LINESTRIP,
	LINELIST,
	TRIANGLESTRIP,
	TRIANGLELIST
};
class DrawTopologyTypeTest:public D3DApp
{
public:
	explicit DrawTopologyTypeTest(HINSTANCE hInstance,const std::vector<Vertex> &vertexs, const std::vector<std::uint16_t>& indices, DrawTopologyType type = TRIANGLELIST);
	~DrawTopologyTypeTest();

	DrawTopologyTypeTest(const DrawTopologyTypeTest& rhs) = delete;
	DrawTopologyTypeTest& operator=(const DrawTopologyTypeTest& rhs) = delete;
	virtual bool Initialize()override;

protected:
	virtual void Update(const GameTimer& gt);
	virtual void Draw(const GameTimer& gt);
	virtual void OnResize();
	virtual void OnMouseDown(WPARAM btnState, int x, int y)override;
	virtual void OnMouseUp(WPARAM btnState, int x, int y)override;
	virtual void OnMouseMove(WPARAM btnState, int x, int y)override;
private:
	void BuildDescriptorHeaps();
	void BuildConstantBuffers();
	void BuildRootSignature();
	void BuildShadersAndInputLayout();
	void BuildGeometry();
	void BuildPSO();

	std::vector<Vertex> m_vdata;
	std::vector<std::uint16_t> m_idata;

	ComPtr<ID3D12RootSignature> mRootSignature = nullptr;

	ComPtr<ID3D12DescriptorHeap> mCbvHeap = nullptr;

	std::unique_ptr<UploadBuffer<ObjectConstants>> mObjectCB = nullptr;

	ComPtr<ID3DBlob> mvsByteCode = nullptr;
	ComPtr<ID3DBlob> mpsByteCode = nullptr;

	std::unique_ptr<MeshGeometry> mBoxGeo = nullptr;
	std::vector<D3D12_INPUT_ELEMENT_DESC> mInputLayout;
	ComPtr<ID3D12PipelineState> mPSO = nullptr;

	D3D12_PRIMITIVE_TOPOLOGY_TYPE m_topologyType;
	D3D_PRIMITIVE_TOPOLOGY m_topology;

	XMFLOAT4X4 mWorld1 = MathHelper::Identity4x4();
	XMFLOAT4X4 mWorld2= MathHelper::Identity4x4();

	XMFLOAT4X4 mView = MathHelper::Identity4x4();
	XMFLOAT4X4 mProj = MathHelper::Identity4x4();

	float mTheta = 1.5f * XM_PI;
	float mPhi = XM_PIDIV4;
	float mRadius = 5.0f;
	POINT mLastMousePos;
};