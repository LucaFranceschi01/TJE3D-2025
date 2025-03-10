varying vec3 v_world_position;
varying vec3 v_normal;
varying vec2 v_uv;

uniform vec4 u_background_color;
uniform vec3 u_camera_position;
uniform vec3 u_light_color;
uniform vec3 u_light_position;
uniform float u_fog_factor;
uniform vec3 u_Ka;
uniform vec3 u_Kd;
uniform vec3 u_Ks;
uniform sampler2D u_texture;
uniform sampler2D u_normals_texture;
uniform vec2 u_maps;

vec3 perturbNormal(vec3 normal, vec3 viewDir, vec2 uv, vec3 normalMap) {
    // Decode normal from normal map (assuming the map is in tangent space)
    vec3 encodedNormal = normalize(normalMap * 2.0 - 1.0); // Normal map values are in [0,1] and need to be remapped to [-1,1]
    
    // Return perturbed normal in world space
    return normalize(normal + encodedNormal);  // You may need to transform this to world space depending on your setup
}

vec4 applyLight() {
    vec4 Kd = vec4(u_Kd, 1.0);

    if( u_maps.x ) {
        Kd = texture2D( u_texture, v_uv );
    }

    vec3 L = normalize(u_light_position - v_world_position);
    vec3 V = normalize(u_camera_position - v_world_position);
    vec3 N = normalize(v_normal);

    if( u_maps.y ) {
        vec3 normals_texture = texture2D( u_normals_texture, v_uv );
        N = perturbNormal( normalize(v_normal), -V, v_uv, normals_texture );
    }

    // Ambient component
    vec3 ambient = u_Ka * Kd.rgb * u_light_color * 0.4;

    float steps = 4.0;

    // Diffuse component
    float NdotL = max(0.0, dot(N,L));
    NdotL = floor(NdotL * steps) / steps;
    vec3 diffuse = Kd.rgb * u_light_color * NdotL;

    // Specular component
    vec3 R = reflect(-L, N);
    float RdotV = pow(max(0.0, dot(R,V)), 20);
    RdotV = floor(RdotV * steps) / steps;
    vec3 specular = u_Ks * u_light_color * RdotV;

    // Total Light

    vec3 total_light = ambient + diffuse + specular;

    return vec4(total_light, Kd.a);
}

void main() {
    vec4 final_color = applyLight();

    // if (final_color.a <= 0.3) discard;

    float dist = length( u_camera_position - v_world_position);

    float fogMaxDist = 80.0;
    float fog_value = clamp( 1.0 - (fogMaxDist / dist), 0.0, 1.0);
    fog_value *= u_fog_factor; // is a flag

    final_color.rgb = mix( final_color.rgb, vec3(0.45, 0.25, 0.33), fog_value );

    gl_FragColor = final_color;
}
