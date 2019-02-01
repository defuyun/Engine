#version 430

out vec4 fragColor;

float near = 0.1f;
float far = 100.0f;

float linearizeDepth(float z_w) {
	// the process is as follows, z_w is screen coord, the value in depth buffer, we first need to convert this back to ndc
	// then convert ndc back to view space (eye coord)
	// then directly map eye coord to screen coord, 
	// meaning we linearly map near far plane(perpective projection) to 0,1 (the near, far defined in glDepthRangef)

	float ndc = 2 * z_w - 1;
	float eye = (2 * near * far) / (far + near - ndc * (far - near));
	return (eye - near) / (far - near);
} 

float nonLinearizeDepth(float z_w) {
	// non linear depth, similar to what opengl uses to calculate the screen coord
	// basically we convert screen (which is automatically calculated by opengl) into eye then manually back into screen

	// process is as follows, screen to ndc
	// ndc to eye
	// eye to screen using nonlinear function

	float ndc = 2 * z_w - 1;
	float eye = (2 * near * far) / (far + near - ndc * (far - near));
	float window = (1.0f / near - 1.0f / eye) / (1.0f / near - 1.0f / far);
	return window;
}

void main() {
	fragColor = vec4(vec3(nonLinearizeDepth(gl_FragCoord.z)), 1.0f);
}