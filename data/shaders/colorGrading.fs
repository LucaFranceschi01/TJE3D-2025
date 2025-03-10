// https://glsl.site/post/post-processing-effects-with-shaders/ cool website

// Color Grading Fragment Shader
uniform sampler2D u_texture;

// Values from ChatGPT
vec3 lift  = vec3(-0.05, -0.02, 0.05); // Slightly lifted blue shadows
vec3 gamma = vec3(1.0, 1.0, 1.1);      // Slightly blue midtones
vec3 gain  = vec3(1.0, 1.05, 1.1);     // Blue-tinted highlights
uniform vec2 u_resolution;

varying vec2 v_uv;

void main() {
    vec2 uv = v_uv;

    float factor = 2.0;
    vec2 pixels = u_resolution / factor;

    uv.x = floor(uv.x * pixels.x) / pixels.x;
    uv.y = floor(uv.y * pixels.y) / pixels.y;

    vec3 color = texture2D(u_texture, uv).rgb;

    // Apply lift, gamma, and gain
    color = lift + color * (1.0 - lift);
    color = pow(color, gamma);
    color = gain * color;

    gl_FragColor = vec4(color, 1.0);
    //gl_FragColor = vec4(texture(u_texture, uv).rgb, 1.0);
}