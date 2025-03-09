
varying vec3 v_position;
varying vec3 v_world_position;
varying vec3 v_normal;
varying vec2 v_uv;
varying vec4 v_color;

uniform vec4 u_color;
uniform sampler2D u_texture;
uniform float u_time;
uniform float u_booster_selected;
uniform float u_choosing_booster_time;
uniform bool u_choosing_booster;

void main()
{
	vec2 uv = v_uv;
    uv.x = (uv.x / 3.0);
    // only paint if booster is diferent that none
    if (u_booster_selected != 3.0) {
        if (u_choosing_booster == true) {
            // prompt claude sonnet: moving the uv with time. I would arrive at a very similar solution but I don't have time.

            // Usamos un valor fijo para ciclar a través de los boosters
            // 3.0 - u_choosing_booster_time es el tiempo transcurrido (asumiendo que comienza en 2.0)
            float elapsed = 3.0 - u_choosing_booster_time;

            // Controlamos cuántos ciclos queremos completar durante toda la animación
            // Por ejemplo, si queremos 6 ciclos completos durante los 2 segundos:
            float cycles = 12.0;
            float cyclePosition = mod(elapsed * cycles, 3.0);

            // Calculamos qué booster mostrar (0, 1 o 2)
            float boosterIndex = floor(cyclePosition);
            uv.x = uv.x + (boosterIndex / 3.0);

            gl_FragColor = texture2D(u_texture, uv);

        } else {
            uv.x = uv.x + (u_booster_selected / 3.0);
            gl_FragColor = texture2D(u_texture, uv);
        }
    } else {
        gl_FragColor = vec4(1, 1, 1, 0);
    }
}