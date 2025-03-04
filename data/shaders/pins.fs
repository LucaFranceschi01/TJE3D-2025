
varying vec3 v_position;
varying vec3 v_world_position;
varying vec3 v_normal;
varying vec2 v_uv;
varying vec4 v_color;

uniform vec4 u_color;
uniform sampler2D u_texture;
uniform float u_time;
uniform int u_pin_ID;
uniform int u_pins_collected;
void main()
{
	vec2 uv = v_uv;
	if (u_pin_ID >= u_pins_collected ) {
		gl_FragColor = mix(texture2D(u_texture, uv ),vec4(0,0,0,0), 0.5);
	} else {
		gl_FragColor = texture2D( u_texture, uv );
	}
}
/*
void main()
{
	vec2 uv = v_uv;
	gl_FragColor = texture2D( u_texture, uv );
}

*/