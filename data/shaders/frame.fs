varying vec2 v_uv;

uniform vec4 u_color;
uniform sampler2D u_texture;
uniform float u_booster_selected;

void main()
{
    if (u_booster_selected != 3.0) {
	    gl_FragColor = mix(texture2D(u_texture, v_uv ),vec4(0,0,0,0), 0.25);
	} else  {
	    gl_FragColor = vec4(1, 1, 1, 0);
	}
}
