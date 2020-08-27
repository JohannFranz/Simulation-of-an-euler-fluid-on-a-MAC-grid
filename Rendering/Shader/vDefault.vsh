#version 430

layout(location=0) in vec2 vPosition;

out vec2 uv;

void main() {
	gl_Position = vec4(vPosition, 0.0f, 1.0f);
	uv = (vPosition.xy+vec2(1.0f,1.0f)) * 0.5f;
}