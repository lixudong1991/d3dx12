#include <windows.h>
#include <iostream>
#include <DirectXMath.h>
using namespace std;
using namespace DirectX;

float power_eigenvalue2x2(const XMMATRIX& a, const XMVECTOR& x0, XMVECTOR* eigenv, int k)
{
	XMMATRIX ta = XMMatrixTranspose(a);
	*eigenv = x0;
	float u = 0;
	for (int i=0;i<k;i++)
	{
		*eigenv = XMVector2Transform(*eigenv, ta);
		u = max(fabsf(XMVectorGetX(*eigenv)), fabsf(XMVectorGetY(*eigenv)));
		*eigenv *= (1.0 / u);
	}
	return u;
}

int main()
{
 
	
	XMMATRIX a = XMMatrixSet(6.0, 5.0, 0.0, 0.0,
							 1.0, 2.0, 0.0, 0.0,
							 0.0, 0.0, 0.0, 0.0,
						     0.0, 0.0, 0.0, 0.0);

	XMVECTOR x0=XMVectorSet(0.0,1.0,0.0,0.0);

	XMVECTOR v;
	float u = power_eigenvalue2x2(a, x0, &v, 128);
	XMFLOAT2 vv;
	XMStoreFloat2(&vv, v);
	cout << "eigen value: " << u << endl;
	cout << "eigen vector: (" << vv.x<<", " <<vv.y<<")" << endl;
	return 0;
}

