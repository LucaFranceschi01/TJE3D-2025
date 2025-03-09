
varying vec3 v_position;
varying vec3 v_world_position;
varying vec3 v_normal;
varying vec2 v_uv;
varying vec4 v_color;

uniform vec4 u_color;
uniform sampler2D u_texture;
uniform float u_time;
uniform float u_booster_selected;
uniform float u_time_choosing_booster;
uniform bool u_choosing_booster;
uniform float u_time_booster;

void main()
{
	vec2 uv = v_uv;
    uv.x = (uv.x / 3.0);


    // only paint if booster is diferent that none
    if (u_booster_selected != 3.0) {
        if (u_choosing_booster == true) {
            // prompt claude sonnet: make a cycle with time.
            // I would have arrived at a very similar solution but I don't have time.

            // Usamos un valor fijo para ciclar a través de los boosters
            float elapsed = 3.0 - u_time_choosing_booster;

            // Cycles in all the animation
            float cycles = 12.0;
            float cyclePosition = mod(elapsed * cycles, 3.0);

            // Compute witch booster show (0, 1 o 2)
            float boosterIndex = floor(cyclePosition);
            uv.x = uv.x + (boosterIndex / 3.0);

            gl_FragColor = texture2D(u_texture, uv);

        } else {
            uv.x = uv.x + (u_booster_selected / 3.0);
            if (u_time_booster < 4.0) {
                // prompt claude sonnet: make blinking effect
                float frequency = 5.0;
                float blinking = (sin(u_time_booster * frequency) + 1.0) * 0.25;
                gl_FragColor = mix(texture2D(u_texture, uv), vec4(0,0,0,0), blinking);
            } else {
                gl_FragColor = texture2D(u_texture, uv);
            }
        }
    } else {
        gl_FragColor = vec4(1, 1, 1, 0);
    }
}