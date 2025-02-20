varying vec2 v_uv;

uniform float u_mask;
uniform vec3 u_color_base;
uniform vec3 u_color_mask;

void main()
{
	vec3 final_color;

	if (v_uv.x > u_mask) { // health-bar style (masks in x direction)
		final_color = vec3(0.1, 0.1, 0.1); // grey
	} else {
		final_color = mix(u_color_mask, u_color_base, u_mask);
	}

	gl_FragColor = vec4(final_color, 1.0);
}
