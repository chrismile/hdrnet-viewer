-- Vertex

attribute vec2 position;
attribute vec2 texcoord;
varying vec2 st;

void main()
{
    st = texcoord;
    gl_Position = vec4(position, 0., 1.);
}

-- Fragment

// Refers to https://github.com/mgharbi/hdrnet/blob/master/benchmark/src/renderer.cc

uniform sampler2D image;
uniform sampler3D affineGridRow0;
uniform sampler3D affineGridRow1;
uniform sampler3D affineGridRow2;
varying vec2 st;

// Guidance map data
uniform mat3x4 guideCCM;
uniform vec3 guideShifts[16];
uniform vec3 guideSlopes[16];
uniform vec4 mixMatrix;

void main()
{
    vec4 imageColor = vec4(texture2D(image, st).rgb, 1.0);
    
    // 1. Compute guidance map value
    vec3 temp = imageColor * guideCCM;
    vec3 acc = vec3(0);
    for (int i = 0; i < 16; ++i) {
        acc += guideSlopes[i].xyz * max(vec3(0), temp - guideShifts[i].xyz);
    }
    float guidanceValue = clamp(dot(mixMatrix, vec4(acc, 1.0)), 0, 1);
    
    // 2. Compute sliced Coefficients
    vec3 gridCoords = vec3(st.x, st.y, guidanceValue);
    vec4 matRows[3];
    matRows[0] = texture3D(affineGridRow0, gridCoords);
    matRows[1] = texture3D(affineGridRow1, gridCoords);
    matRows[2] = texture3D(affineGridRow2, gridCoords);
    
    // 3. Apply coefficients
    float r = dot(matRows[0], imageColor);
    float g = dot(matRows[1], imageColor);
    float b = dot(matRows[2], imageColor);
    gl_FragColor = clamp(vec4(r, g, b, 1.0), 0.0, 1.0);
    
}
