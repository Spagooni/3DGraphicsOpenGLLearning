#version 410 core

in vec3 outVertexColors;
out vec4 FragColor;

void main(){
FragColor = vec4(outVertexColors.r,
                 outVertexColors.g,
                 outVertexColors.b,
                 1.0);
}