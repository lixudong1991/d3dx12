#include "DrawTopologyTypeTest.h"



int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance,
    PSTR cmdLine, int showCmd)
{
    // Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

    try
    {
        /*6.13-3Á·Ï° */
#if  0
		std::vector<Vertex> vertices =
		{
        Vertex({ XMFLOAT3(0.1f, 0.1f, 0.5f), XMFLOAT4(Colors::White) }),
        Vertex({ XMFLOAT3(0.2f, 0.5f, 0.5f), XMFLOAT4(Colors::Black) }),
        Vertex({ XMFLOAT3(0.3f, 0.2f, 0.5f), XMFLOAT4(Colors::Red) }),
        Vertex({ XMFLOAT3(0.5f, 0.4f, 0.5f), XMFLOAT4(Colors::Green) }),
        Vertex({ XMFLOAT3(0.6f, 0.3f, 0.5f), XMFLOAT4(Colors::Blue) }),
        Vertex({ XMFLOAT3(0.7f, 0.38f, 0.5f), XMFLOAT4(Colors::Yellow) }),
        Vertex({ XMFLOAT3(0.8f, 0.35f, 0.5f), XMFLOAT4(Colors::Cyan) }),
        Vertex({ XMFLOAT3(0.9f, 0.7f, 0.5f), XMFLOAT4(Colors::Magenta) })
		};

		std::vector<std::uint16_t> indices =
		{
            0,1,2,3,4,5,6,7
		};
#endif

        /*6.13-4Á·Ï° */
        std::vector<Vertex> vertices =
        {
        Vertex({ XMFLOAT3(0.0f, 2.0f, 0.0f), XMFLOAT4(Colors::Red) }),
        Vertex({ XMFLOAT3(1.0f, 0.0f, -1.0f), XMFLOAT4(Colors::Green) }),
        Vertex({ XMFLOAT3(-1.0f, 0.0f, -1.0f), XMFLOAT4(Colors::Green) }),
        Vertex({ XMFLOAT3(-1.0f, 0.0f, 1.0f), XMFLOAT4(Colors::Green) }),
        Vertex({ XMFLOAT3(1.0f, 0.0f, 1.0f), XMFLOAT4(Colors::Green) }),
        Vertex({ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT4(Colors::White) }),
        Vertex({ XMFLOAT3(-1.0f, +1.0f, -1.0f), XMFLOAT4(Colors::Black) }),
        Vertex({ XMFLOAT3(+1.0f, +1.0f, -1.0f), XMFLOAT4(Colors::Red) }),
        Vertex({ XMFLOAT3(+1.0f, -1.0f, -1.0f), XMFLOAT4(Colors::Green) }),
        Vertex({ XMFLOAT3(-1.0f, -1.0f, +1.0f), XMFLOAT4(Colors::Blue) }),
        Vertex({ XMFLOAT3(-1.0f, +1.0f, +1.0f), XMFLOAT4(Colors::Yellow) }),
        Vertex({ XMFLOAT3(+1.0f, +1.0f, +1.0f), XMFLOAT4(Colors::Cyan) }),
        Vertex({ XMFLOAT3(+1.0f, -1.0f, +1.0f), XMFLOAT4(Colors::Magenta) })

        };

        std::vector<std::uint16_t> indices =
        {
            0,1,2,
            0,2,3,
            0,3,4,
            0,4,1,
            1,3,2,
            1,4,3,
            		// front face
		0, 1, 2,
		0, 2, 3,

		// back face
		4, 6, 5,
		4, 7, 6,

		// left face
		4, 5, 1,
		4, 1, 0,

		// right face
		3, 2, 6,
		3, 6, 7,

		// top face
		1, 5, 6,
		1, 6, 2,

		// bottom face
		4, 0, 3,
		4, 3, 7
        };

        DrawTopologyTypeTest theApp(hInstance, vertices, indices, TRIANGLELIST);
        if (!theApp.Initialize())
            return 0;

        return theApp.Run();
    }
    catch (DxException& e)
    {
        MessageBox(nullptr, e.ToString().c_str(), L"HR Failed", MB_OK);
        return 0;
    }
}