
varying vec3 v_position;
varying vec3 v_world_position;
varying vec3 v_normal;
varying vec2 v_uv;
varying vec4 v_color;

uniform vec4 u_color;
uniform sampler2D u_texture;
uniform float u_time;
uniform int u_lives;

void main()
{
	// the operations are done in texture space [0-1]
	float frameWidth = 1.0 / 3.0; // each sprite is a third of the texture
	float frameOffset = frameWidth * float(u_lives-1); // choose between frames 1-3 e.g.: 0, 1/3, 2/3
	vec2 uv = vec2(v_uv.x * frameWidth + frameOffset, v_uv.y);
	gl_FragColor = texture2D( u_texture, uv );
}