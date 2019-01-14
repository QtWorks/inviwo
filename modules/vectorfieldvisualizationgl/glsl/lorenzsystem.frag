/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2014-2019 Inviwo Foundation
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 *********************************************************************************/


 layout(location = 0) out vec3 velocity;
 layout(location = 1) out vec3 curl;
 layout(location = 2) out float div;

uniform ivec3 volumeSize_;

in vec4 texCoord_;
in vec4 worldPos_;

uniform float rho;
uniform float sigma;
uniform float beta;

uniform vec2 xRange;
uniform vec2 yRange;
uniform vec2 zRange;
 
void main() {
    float x = worldPos_.x;
    float y = worldPos_.y;
    float z = worldPos_.z;
    
    
    mat3 J = transpose(mat3(
        -sigma, sigma, 0.f,
        rho-z, -1.f , -x,
        y, x, -beta
    ));

    velocity = vec3(
        sigma*(y-x),
        x*(rho-z)-y,
        x*y - beta*z
    );

    curl = vec3(
        J[2][1] - J[1][2],
        J[2][0] - J[0][2],
        J[0][1] - J[1][0]
    );

    div = J[0][0] + J[1][1]+ J[2][2];

}
