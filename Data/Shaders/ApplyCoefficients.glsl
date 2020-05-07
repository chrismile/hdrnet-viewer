-- Vertex

#version 430 core

attribute vec2 position;
attribute vec2 texcoord;
out vec2 st;

void main()
{
    st = texcoord;
    gl_Position = vec4(position, 0., 1.);
}

-- Fragment

#version 430 core

/*
 * Code for handling weights from: https://github.com/mgharbi/hdrnet/blob/master/benchmark/assets/std.frag

 * Copyright 2016 Google Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 *
 * License of adapted code:
 *
 * BSD 3-Clause License
 *
 * Copyright (c) 2017, Christoph Neuhauser
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 *
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 * * Neither the name of the copyright holder nor the names of its
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

uniform sampler2D image;
uniform sampler3D affineGridRow0;
uniform sampler3D affineGridRow1;
uniform sampler3D affineGridRow2;

// Guidance map data
uniform mat3x4 guideCCM;
uniform vec3 guideShifts[16];
uniform vec3 guideSlopes[16];
uniform vec4 mixMatrix;

in vec2 st;
out vec4 fragColorOut;

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
    
    // 2. Compute sliced coefficients
    vec3 gridCoords = vec3(st.x, st.y, guidanceValue);
    vec4 matRows[3];
    matRows[0] = texture(affineGridRow0, gridCoords);
    matRows[1] = texture(affineGridRow1, gridCoords);
    matRows[2] = texture(affineGridRow2, gridCoords);
    
    // 3. Apply coefficients
    float r = dot(matRows[0], imageColor);
    float g = dot(matRows[1], imageColor);
    float b = dot(matRows[2], imageColor);
    fragColorOut = clamp(vec4(r, g, b, 1.0), 0.0, 1.0);
}
