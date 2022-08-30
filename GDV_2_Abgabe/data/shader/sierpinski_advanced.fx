// -----------------------------------------------------------------------------
// Define the constant buffers.
// -----------------------------------------------------------------------------
cbuffer VSPerFrameConstants : register(b0)
{
    float4x4 c_VSViewProjectionMatrix;
}

cbuffer VSPerObjectConstants : register(b1)
{
    float4x4 c_VSWorldMatrix;
}

cbuffer PSPerObjectConstants : register(b0)
{
    int     c_PSConstant0;
    float   c_PSConstant1;
    float4  c_PSConstant2;
}

#define c_PSIterations     c_PSConstant0
#define c_PSZoomFactor     c_PSConstant1
#define c_PSColoring       c_PSConstant2.xyzw


// -----------------------------------------------------------------------------
// Define input and output data of the vertex shader. (Standard)
// -----------------------------------------------------------------------------
struct VSInput
{
    float3 m_Position : OSPOSITION;
    float2 m_PositionNormed : POSITION_NORMED;
};

struct PSInput
{
    float4 m_Position : SV_POSITION;
    float2 m_PositionNormed : TEXCOORD0;
};

// -----------------------------------------------------------------------------
// Vertex Shader
// -----------------------------------------------------------------------------
PSInput VSMain(VSInput _Input)
{
    float4 WSPosition;
    float4 CSPosition;

    PSInput Result          = (PSInput) 0;

    WSPosition              = mul(float4(_Input.m_Position, 1.0f), c_VSWorldMatrix);
    CSPosition              = mul(WSPosition, c_VSViewProjectionMatrix);   

    Result.m_Position       = CSPosition;
    Result.m_PositionNormed = _Input.m_PositionNormed;

    return Result;
}
// https://www.geeksforgeeks.org/sierpinski-triangle/
// Sierpinski Triangle will be constructed from an equilateral triangle by repeated removal of triangular subsets. 
// Steps for Construction :
// 1.Take any equilateral triangle .
// 2.Divide it into 4 smaller congruent triangle and remove the central triangle .
// 3.Repeat step 2 for each of the remaining smaller triangles forever.
//


float texDrawXY(int _X, int _Y) 
{
    float value = 0.0f;

    if (_X >= 0 && _Y>= 0) 
    {
        if (!(_X & _Y)) 
        {
            value = 1.0f;
        } 
    }

    return value;
}

// return interpolated value on X-Axis
float interpolateXAxis(float _X, int _Y) 
{
    _X -= 0.5f * float(_Y);

    float X_1 = texDrawXY(int(floor(_X)), _Y);
    float X_2 = texDrawXY(int(floor(_X)) + 1, _Y);

    return lerp(X_1, X_2, _X - (int)_X);
}

// return interpolated value on Y-Axis
float interpolateYAxis(float2 _Pos)
{
    float Y_1 = interpolateXAxis(_Pos.x, int(_Pos.y));
    float Y_2 = interpolateXAxis(_Pos.x, int(_Pos.y) + 1);

    return lerp(Y_1, Y_2, _Pos.y- (int)_Pos.y );
}


float4 Sierpinski(float2 _Pos, float _ZoomFactor, float4 _Coloring)
{
    float2 Pos = _Pos*_ZoomFactor;
    int counter = 0;
    
    float Intensity = lerp(interpolateYAxis(Pos), interpolateYAxis(Pos * 4.0f), 0.5f - 1.0f);
    float4 Color = float4(0, 0, 0, 1);

    // -------------------------------------------------------
    // Determine the color by the newly created triangle iteration to 
    // enlighten the base triangle a little more
    // -------------------------------------------------------
  
    if( (Pos.x < 8/2    && Pos.y < 8)       && (Pos.x < 8 * 2 && Pos.y < 8 * 2)      || 
        (Pos.x > 8      && Pos.y > 8 * 2)   && (Pos.x < 8 * 4 && Pos.y < 8 * 4)      ||
        (Pos.x > 8 * 2  && Pos.y > 8 * 4)   && (Pos.x < 8 * 8 && Pos.y < 8 * 8)      ||
        (Pos.x > 8 * 4  && Pos.y > 8 * 8)   && (Pos.x < 8 * 16 && Pos.y < 8 * 16)    ||
        (Pos.x > 8 * 8  && Pos.y > 8 * 16)  && (Pos.x < 8 * 32 && Pos.y < 8 * 32)    ||
        (Pos.x > 8 * 16 && Pos.y > 8 * 32)  && (Pos.x < 8 * 64 && Pos.y < 8 * 64)    ||
        (Pos.x > 8 * 32 && Pos.y > 8 * 64)  && (Pos.x < 8 * 128 && Pos.y < 8 * 128)  ||
        (Pos.x > 8 * 64 && Pos.y > 8 * 128) && (Pos.x < 8 * 256 && Pos.y < 8 * 256))
    {
        Color = float4(
            Intensity * _Coloring.x,
            Intensity * _Coloring.y,
            Intensity * _Coloring.z,
            _Coloring.w);
    }
    else
    {
        Color = float4(
            Intensity * 0.50f,
            Intensity * 0.05f,
            Intensity * 0.05f,
            1.0f);
    }
    
    return Color;
}


// -----------------------------------------------------------------------------
// Pixel Shader
// -----------------------------------------------------------------------------
float4 PSMain(PSInput _Input) : SV_Target
{
    return Sierpinski(_Input.m_PositionNormed, c_PSZoomFactor, c_PSColoring);
}


