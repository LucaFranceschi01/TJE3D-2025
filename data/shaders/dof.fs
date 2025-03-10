// Depth of Field Fragment Shader
uniform sampler2D u_texture;
uniform sampler2D u_depth;

uniform float focalDepth;
uniform float focalRange;
uniform float blurAmount;

void main() {
    vec2 uv = gl_FragCoord.xy / resolution.xy;
    float depthValue = texture(depth, uv).r;
    float blur = clamp(abs(depthValue - focalDepth) / focalRange, 0.0, 1.0) * blurAmount;

    // Accumulate blurred colors
    vec3 color = vec3(0.0);
    int samples = 16;
    for (int i = 0; i < samples; ++i) {
        float angle = float(i) * 3.14159265 * 2.0 / float(samples);
        vec2 offset = vec2(cos(angle), sin(angle)) * blur;
        color += texture(u_texture, uv + offset).rgb;
    }
    color /= samples;

    gl_FragColor = vec4(color, 1.0);
}

