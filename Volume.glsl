
-- Quad

in vec4 Position;
out vec3 vPosition;

void main()
{
    gl_Position = Position;
    vPosition = Position.xyz;
}

-- Vertex

in vec4 Position;
out vec3 vPosition;
//uniform mat4 ModelviewProjection;
uniform mat4 Modelview;
uniform mat4 Projection;

void main()
{
    //gl_Position = ModelviewProjection * Position;
	gl_Position = Projection*Modelview*Position;
    vPosition = Position.xyz;
}

-- Endpoints

in vec3 vPosition;
out vec3 FragData[2];

void main()
{
    vec3 pos = vPosition;

    if (gl_FrontFacing) {
        FragData[0] = pos;//0.5 * (pos + 1.0);
        FragData[1] = vec3(0);
    } else {
        FragData[0] = vec3(0);
        FragData[1] = pos;//0.5 * (pos + 1.0);
    }
}

-- Lighting

out vec4 FragColor;
in vec3 vPosition;

uniform sampler2D RayStart;
uniform sampler2D RayStop;
uniform sampler3D Volume;
uniform sampler2D Noise;

uniform float StepLength = 0.01;
uniform float Threshold = 0.45;

uniform vec3 LightPosition;
uniform vec3 DiffuseMaterial;
uniform mat4 Modelview;
uniform mat3 NormalMatrix;
//uniform vec3 Scale=vec3(1.2f,1.2f,3.333f);
uniform vec3 BoxMinCorner;
uniform vec3 BoxExtent;

float lookup(vec3 coord)
{
	//coord.xyz /= Scale.xyz;
    //vec3 V = texture(Volume, coord).xyz;
    //return dot(V, V);
	coord -= BoxMinCorner;
	coord.xyz /= BoxExtent.xyz;
	vec3 V = texture(Volume, coord).xyz;
	return dot(V, V);
}

void main()
{
    vec2 coord = 0.5 * (vPosition.xy + 1.0);
    vec3 rayStart = texture(RayStart, coord).xyz;
    vec3 rayStop = texture(RayStop, coord).xyz;

    if (rayStart == rayStop) {
        discard;
        return;
    }

    vec3 ray = rayStop - rayStart;
    float rayLength = length(ray);
    vec3 stepVector = StepLength * ray / rayLength;

    // Jitter Z to hide "wood grain" artifacts:
    rayStart += stepVector * texture(Noise, gl_FragCoord.xy / 256).r;

    vec3 pos = rayStart;
    vec4 dst = vec4(0);
    while (dst.a < 1 && rayLength > 0) {
        
        float V = lookup(pos);
        if (V > Threshold) {

            // Refine the hitpoint to reduce slicing artifacts:
            vec3 s = -stepVector * 0.5;
            pos += s; 
			V = lookup(pos);
            if (V > Threshold) s *= 0.5; else s *= -0.5;
            pos += s; 
			V = lookup(pos);

            if (V > Threshold) {
                float L = StepLength;
                float E = lookup(pos + vec3(L,0,0));
                float N = lookup(pos + vec3(0,L,0));
                float U = lookup(pos + vec3(0,0,L));
                vec3 normal = normalize(NormalMatrix * vec3(E - V, N - V, U - V));
                vec3 light = LightPosition;

                float df = abs(dot(normal, light));
                vec3 color = df * DiffuseMaterial;

                vec4 src = vec4(color,0.1f);
                dst = (1.0 - dst.a) * src + dst;
				
                break;
            }
        }

        pos += stepVector;
        rayLength -= StepLength;
    }

    FragColor = dst;
}

-- Semitransparent

out vec4 FragColor;
in vec3 vPosition;

uniform sampler2D RayStart;
uniform sampler2D RayStop;
uniform sampler3D Volume;
uniform vec3 LightPosition;
uniform float StepLength = 0.01;
//uniform vec3 Scale=vec3(1.2f,1.2f,3.333f);
uniform vec3 BoxMinCorner;
uniform vec3 BoxExtent;

float lookup(vec3 coord)
{
	//coord.xyz /= Scale.xyz;
    //vec3 V = texture(Volume, coord).xyz;
    //return dot(V, V);
	coord -= BoxMinCorner;
	coord.xyz /= BoxExtent.xyz;
	vec3 V = texture(Volume, coord).xyz;
	return dot(V, V);
}
vec3 lookupinvolume(vec3 pos)
{
	//pos.z /=2.7777f;
	//pos.xyz /= Scale.xyz;
    //return texture(Volume, pos).xyz;

	pos -= BoxMinCorner;
	pos.xyz /= BoxExtent.xyz;
	return texture(Volume, pos).xyz;
}

void main()
{
    vec2 coord = 0.5 * (vPosition.xy + 1.0);
    vec3 rayStart = texture(RayStart, coord).xyz;
    vec3 rayStop = texture(RayStop, coord).xyz;

    if (rayStart == rayStop) {
        discard;
        return;
    }

    vec3 ray = rayStop - rayStart;
    float rayLength = length(ray);
    vec3 stepVector = StepLength * ray / rayLength;

    vec3 pos = rayStart;
    vec4 dst = vec4(0);
    while (dst.a < 1 && rayLength > 0) {

		vec3 V = lookupinvolume(pos);
        vec4 src = vec4(0.5 * (V + 1.0), dot(V, V));
        src.rgb *= src.a;

		float L = StepLength;
        float E = lookup(pos + vec3(L,0,0));
        float N = lookup(pos + vec3(0,L,0));
        float U = lookup(pos + vec3(0,0,L));
        vec3 normal = vec3(E - V.x, N - V.x, U - V.x);
        vec3 light = vec3(0.2,0.2,0.2);//LightPosition;

        float df = abs(dot(normal, light));
                
        dst = (1.0 - dst.a) * src*(1-df) + dst;
        pos += stepVector;
        rayLength -= StepLength;
    }

    FragColor = dst.rrrr;
}
