#version 120

void main() {
    gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);
}

// flat in vec2 centre;
// flat in float radiusPixels;
// 
// out vec4 fragColour;
// 
// void main()
// {
//     vec2 coord = (gl_FragCoord.xy - centre) / radiusPixels;
//     float l = length(coord);
//     if (l > 1.0)
//         discard;
//     vec3 pos = vec3(coord, sqrt(1.0-l*l));
//     fragColour = vec4(vec3(pos.z), 1.0);
// }
