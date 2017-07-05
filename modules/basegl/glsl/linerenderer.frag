/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2016-2017 Inviwo Foundation
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
#include "utils/structs.glsl"
#include "utils/depth.glsl"

#if !defined(ENABLE_PSEUDO_LIGHTING)
#  define ENABLE_PSEUDO_LIGHTING 0
#endif

// enable conservative depth writes (supported since GLSL 4.20)
#if defined(GLSL_VERSION_450) || defined(GLSL_VERSION_440) || defined(GLSL_VERSION_430) || defined(GLSL_VERSION_420)
layout (depth_less) out float gl_FragDepth;
#endif

uniform vec2 screenDim = vec2(512, 512);
uniform float antialising = 1.0; // width of antialised edged [pixel]
uniform float lineWidth = 2.0; // line width [pixel]
uniform CameraParameters camera;

// line stippling
uniform StipplingParameters stippling = StipplingParameters(30.0, 10.0, 0.0, 4.0);

in float segmentLength_; // total length of the current line segment in screen space
in float lineLengthWorld_; // total length of line segment in world space
in float distanceWorld_; // distance in world coords to segment start
in vec2 texCoord_; // x = distance to segment start, y = orth. distance to center (in screen coords)
in vec4 color_;

void main() {
    vec4 color = color_;

    const float linewidthHalf = lineWidth * 0.5;

    // make joins round by using the texture coords
    float distance = abs(texCoord_.y);
    if (texCoord_.x < 0.0) { 
        distance = length(texCoord_); 
    }
    else if(texCoord_.x > segmentLength_) { 
        distance = length(vec2(texCoord_.x - segmentLength_, texCoord_.y)); 
    }

    float d = distance - linewidthHalf + antialising;

    // apply pseudo lighting
#if ENABLE_PSEUDO_LIGHTING == 1
    color.rgb *= cos(distance / (linewidthHalf + antialising) * 1.2);
#endif

    float alpha = 1.0;

    // line stippling
#if defined ENABLE_STIPPLING

#if STIPPLE_MODE == 2
    // in world space
    float v = (distanceWorld_ * stippling.worldScale);
#else
    // in screen space
    float v = (texCoord_.x + stippling.offset) / stippling.length;    
#endif // STIPPLE_MODE

    float t = fract(v) * (stippling.length) / stippling.spacing;
    if ((t > 0.0) && (t < 1.0)) {
        // renormalize t with respect to stippling length
        t = min(t, 1.0-t) * (stippling.spacing) * 0.5;
        d = max(d, t);
    }
#endif // ENABLE_STIPPLING

    // antialising around the edges
    if( d > 0) {
        // apply antialising by modifying the alpha [Rougier, Journal of Computer Graphics Techniques 2013]
        d /= antialising;
        alpha = exp(-d*d);
    }
    // prevent fragments with low alpha from being rendered
    if (alpha < 0.05) discard;

    color.a = alpha;
    FragData0 = color;

    // correct depth
    const float depth = convertDepthScreenToView(camera, gl_FragCoord.z);
    const float maxDist = (linewidthHalf + antialising);
    // assume circular profile of line
    gl_FragDepth = convertDepthViewToScreen(camera, 
        depth - cos(distance/maxDist) * maxDist / screenDim.x*0.5);
}
