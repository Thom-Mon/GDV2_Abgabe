
//#include "sierpinski_advanced_header.h"
#include "yoshix.h";
#include <iostream>
using namespace gfx;

class Application : public IApplication
{
public:

	Application();
	virtual ~Application();

private:

	struct VSPerFrameConstants
	{
		float m_VSViewProjectionMatrix[16];
	};

	struct VSPerObjectConstants
	{
		float m_VSWorldMatrix[16];
	};

	struct PSPerObjectConstants
	{
		int			m_PSIterations;
		float		m_PSZoomFactor;
		float		m_Padding0[2];
		float		m_PSColor[4];
	};

private:

	float	m_ZoomFactor;
	int		m_Iterations;
	int		minX = -6;
	int		maxX = 3;
	int		minY = -9;
	int		maxY = 3;
	bool	minX_Touched = false;
	bool	minY_Touched = false;
	float   m_FieldOfViewY;             // Vertical view angle of the camera
	float   m_Position[3];
	float   m_ViewMatrix[16];
	float   m_ProjectionMatrix[16];
	BHandle m_pVSPerFrameConstants;
	BHandle m_pVSPerObjectConstants;
	BHandle m_pPSPerObjectConstants;
	BHandle m_pVertexShader;
	BHandle m_pPixelShader;
	BHandle m_pMaterial;
	BHandle m_pMesh;

private:

	virtual bool InternOnCreateConstantBuffers();
	virtual bool InternOnReleaseConstantBuffers();
	virtual bool InternOnCreateShader();
	virtual bool InternOnReleaseShader();
	virtual bool InternOnCreateMaterials();
	virtual bool InternOnReleaseMaterials();
	virtual bool InternOnCreateMeshes();
	virtual bool InternOnReleaseMeshes();
	virtual bool InternOnResize(int _Width, int _Height);
	virtual bool InternOnUpdate();
	virtual bool InternOnFrame();
};

// -----------------------------------------------------------------------------

Application::Application()
	: m_FieldOfViewY(60.0f)			// Set the vertical view angle of the camera to 60 degrees.
	, m_Position{ -1.2f, 0.0f, 1.0f }
	, m_Iterations(0.0f)
	, m_ZoomFactor(500.0f)
{
}

// -----------------------------------------------------------------------------

Application::~Application()
{
}

// -----------------------------------------------------------------------------

bool Application::InternOnCreateConstantBuffers()
{
	CreateConstantBuffer(sizeof(VSPerFrameConstants), &m_pVSPerFrameConstants);
	CreateConstantBuffer(sizeof(VSPerObjectConstants), &m_pVSPerObjectConstants);
	CreateConstantBuffer(sizeof(PSPerObjectConstants), &m_pPSPerObjectConstants);

	return true;
}

// -----------------------------------------------------------------------------

bool Application::InternOnReleaseConstantBuffers()
{
	ReleaseConstantBuffer(m_pVSPerFrameConstants);
	ReleaseConstantBuffer(m_pVSPerObjectConstants);
	ReleaseConstantBuffer(m_pPSPerObjectConstants);

	return true;
}

// -----------------------------------------------------------------------------

bool Application::InternOnCreateShader()
{
	CreateVertexShader("..\\data\\shader\\sierpinski_advanced.fx", "VSMain", &m_pVertexShader);
	CreatePixelShader("..\\data\\shader\\sierpinski_advanced.fx", "PSMain", &m_pPixelShader);

	return true;
}

// -----------------------------------------------------------------------------

bool Application::InternOnReleaseShader()
{
	ReleaseVertexShader(m_pVertexShader);
	ReleasePixelShader(m_pPixelShader);

	return true;
}

// -----------------------------------------------------------------------------

bool Application::InternOnCreateMaterials()
{
	SMaterialInfo Info;

	Info.m_NumberOfTextures = 0;
	Info.m_NumberOfVertexConstantBuffers = 2;
	Info.m_pVertexConstantBuffers[0] = m_pVSPerFrameConstants;
	Info.m_pVertexConstantBuffers[1] = m_pVSPerObjectConstants;
	Info.m_NumberOfPixelConstantBuffers = 1;
	Info.m_pPixelConstantBuffers[0] = m_pPSPerObjectConstants;
	Info.m_pVertexShader = m_pVertexShader;
	Info.m_pPixelShader = m_pPixelShader;
	Info.m_NumberOfInputElements = 2;
	Info.m_InputElements[0].m_Type = SInputElement::Float3;
	Info.m_InputElements[0].m_pName = "OSPOSITION";
	Info.m_InputElements[1].m_Type = SInputElement::Float2;
	Info.m_InputElements[1].m_pName = "POSITION_NORMED";

	CreateMaterial(Info, &m_pMaterial);

	return true;
}

// -----------------------------------------------------------------------------

bool Application::InternOnReleaseMaterials()
{
	ReleaseMaterial(m_pMaterial);

	return true;
}

// -----------------------------------------------------------------------------

bool Application::InternOnCreateMeshes()
{
	float Vertices[][5] =
	{
		// X      Y     Z         U      V
		{ -16.0f, -16.0f, 0.0f,  -4.0f, -4.0f },
		{  16.0f, -16.0f, 0.0f,   4.0f, -4.0f },
		{  16.0f,  16.0f, 0.0f,   4.0f,  4.0f },
		{ -16.0f,  16.0f, 0.0f,  -4.0f,  4.0f },
	};

	int Indices[][3] =
	{
		{ 0, 1, 2 }, // Triangle 0
		{ 0, 2, 3 }, // Triangle 1
	};

	SMeshInfo Info;

	Info.m_pVertices = &Vertices[0][0];
	Info.m_NumberOfVertices = 4;
	Info.m_pIndices = &Indices[0][0];
	Info.m_NumberOfIndices = 6;
	Info.m_pMaterial = m_pMaterial;

	CreateMesh(Info, &m_pMesh);

	return true;
}

// -----------------------------------------------------------------------------

bool Application::InternOnReleaseMeshes()
{
	ReleaseMesh(m_pMesh);

	return true;
}

// -----------------------------------------------------------------------------

bool Application::InternOnResize(int _Width, int _Height)
{
	float AspectRatio = static_cast<float>(_Width) / static_cast<float>(_Height);

	GetProjectionMatrix(100.0f, AspectRatio, 0.01f, 1000.0f, m_ProjectionMatrix);

	return true;
}

// -----------------------------------------------------------------------------

bool Application::InternOnUpdate()
{
	float Eye[6];
	float At[3];
	float Up[3];

	// -----------------------------------------------------------------------------
	// Define position and orientation of the camera in the world. The result is
	// stored in the 'm_ViewMatrix' matrix and uploaded in the 'InternOnFrame'
	// method.
	// -----------------------------------------------------------------------------
	Eye[0] = 2.9f;      Eye[1] = 2.5f;      Eye[2] = -2.5f;   
	At[0] = 2.9f;       At[1] = 2.5f;       At[2] = 0.0f;
	Up[0] = 0.0f;       Up[1] = 1.0f;       Up[2] = 0.0f;

	GetViewMatrix(Eye, At, Up, m_ViewMatrix);

	return true;
}

// -----------------------------------------------------------------------------

bool Application::InternOnFrame()
{
	// using the standards as used in the example
	VSPerFrameConstants PerFrameConstantsVS;
	
	MulMatrix(m_ViewMatrix, m_ProjectionMatrix, PerFrameConstantsVS.m_VSViewProjectionMatrix);
	UploadConstantBuffer(&PerFrameConstantsVS, m_pVSPerFrameConstants);
	
	VSPerObjectConstants PerObjectConstantsVS;
	
	GetTranslationMatrix(m_Position[0], m_Position[1], m_Position[2], PerObjectConstantsVS.m_VSWorldMatrix);
	UploadConstantBuffer(&PerObjectConstantsVS, m_pVSPerObjectConstants);
	
	PSPerObjectConstants PerObjectConstantsPS;
	// --------------------------------------------

	// Move camera over Triangle like the old dvd logo used to do
	if (minY_Touched) 
	{
		m_Position[1] += 0.01f;
	}
	else
	{
		m_Position[1] -= 0.01f;
	}

	if (minX_Touched)
	{
		m_Position[0] += 0.01f;
	}
	else
	{
		m_Position[0] -= 0.01f;
	}

	// Move in a loop over the Triangle -> for better reading other coding style
	if (m_Position[1] < minY){minY_Touched = true;}
	if (m_Position[1] > maxY){minY_Touched = false;}
	if (m_Position[0] < minX){minX_Touched = true;}
	if (m_Position[0] > maxX){minX_Touched = false;}

	// iterate the repeating of the fractal construction
	m_Iterations++;

	// Set Triangles Color dependent on what border was touched last
	if (minX_Touched && minY_Touched)
	{
		PerObjectConstantsPS.m_PSColor[0] = 0.15f; //R
		PerObjectConstantsPS.m_PSColor[1] = 0.05f; //G
		PerObjectConstantsPS.m_PSColor[2] = 0.95f; //B
		PerObjectConstantsPS.m_PSColor[3] = 1.0f;  //A
	}
	if (minX_Touched && !minY_Touched)
	{
		PerObjectConstantsPS.m_PSColor[0] = 0.65f; //R
		PerObjectConstantsPS.m_PSColor[1] = 0.55f; //G
		PerObjectConstantsPS.m_PSColor[2] = 0.15f; //B
		PerObjectConstantsPS.m_PSColor[3] = 1.0f;  //A
	}
	if (!minX_Touched && minY_Touched)
	{
		PerObjectConstantsPS.m_PSColor[0] = 0.25f; //R
		PerObjectConstantsPS.m_PSColor[1] = 0.25f; //G
		PerObjectConstantsPS.m_PSColor[2] = 0.65f; //B
		PerObjectConstantsPS.m_PSColor[3] = 1.0f;  //A
	}
	if (!minX_Touched && !minY_Touched)
	{
		PerObjectConstantsPS.m_PSColor[0] = 0.95f; //R
		PerObjectConstantsPS.m_PSColor[1] = 0.05f; //G
		PerObjectConstantsPS.m_PSColor[2] = 0.15f; //B
		PerObjectConstantsPS.m_PSColor[3] = 1.0f;  //A
	}
	
	PerObjectConstantsPS.m_PSIterations = m_Iterations;
	PerObjectConstantsPS.m_PSZoomFactor = m_ZoomFactor;

	UploadConstantBuffer(&PerObjectConstantsPS, m_pPSPerObjectConstants);

	DrawMesh(m_pMesh);

	return true;
}

void main()
{
	Application Application;

	RunApplication(1400, 800, "Sierpinski Fractal", &Application);
}
