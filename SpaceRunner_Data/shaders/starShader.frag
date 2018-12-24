#version 400 core

struct LightInfo
{
	vec4 position;
	vec3 La;
	vec3 Ld;
	vec3 Ls;
};

struct MaterialInfo
{
	vec3 Ma;
	vec3 Md;
	vec3 Ms;
	float shininess;
};

uniform LightInfo light1; 
uniform MaterialInfo material1; 



// Note: colour is smoothly interpolated (default)
out vec4 vOutputColour;

in vec2 vTexCoord;
uniform sampler2D sampler0;
uniform LightInfo light;

in vec4 vEyePosition;
in vec3 vEyeNorm;

float fIntensity;

vec3 PhongModel(LightInfo light, vec4 eyePosition, vec3 eyeNorm)
{
	vec3 s = normalize(vec3(light.position - eyePosition));
	vec3 v = normalize(-eyePosition.xyz);
	vec3 r = reflect(-s, eyeNorm);
	vec3 n = eyeNorm;
	vec3 ambient = light.La * material1.Ma;
	float sDotN = max(dot(s, n), 0.0);
	vec3 diffuse = light.Ld * material1.Md * sDotN;
	vec3 specular = vec3(0.0f);
	float specularIntensity = 0.0f;
	float eps = 0.000001f; // add eps to shininess below -- pow not defined if second argument is 0 (as described in GLSL documentation)
	if (sDotN > 0.0f){
		specular = light.Ls * material1.Ms * pow(max(dot(r, v), 0.0f), material1.shininess + eps);
	}
	fIntensity = sDotN + specularIntensity;

	return ambient + diffuse + specular;

}

void main()
{	
		vec3 vColour = vec3(0.0f, 1.0f, 1.0f);

		//edge mask code based on the example edge make code in lecture 8
		vec3 n = normalize(vEyeNorm);
		vec3 v = normalize(-vEyePosition.xyz);	
		float edgeMask = (dot(v, n) < 0.25) ? 0 : 1;

		
		vec4 vTexColour = texture(sampler0, vTexCoord);	
		vColour = PhongModel(light, vEyePosition, vEyeNorm) + vec3(0.0f, 1.0f , 1.0f);
		vOutputColour = vTexColour *vec4(vColour * edgeMask, 0.7);

		if(fIntensity < 0.1f){

				vColour = PhongModel(light, vEyePosition, vEyeNorm) + vec3(0.0f, 0.0f, 0.0f);

		} else if((fIntensity > 0.25) && (fIntensity < 0.5)){

				vColour = PhongModel(light, vEyePosition, vEyeNorm) + vec3(0.0f, 0.5f , 0.5f);

		} else if((fIntensity > 0.5) && (fIntensity <= 0.75)){

				vColour = PhongModel(light, vEyePosition, vEyeNorm) + vec3(0.0f, 1.0f , 1.0f);

		} else if(fIntensity >0.75){

			vColour = PhongModel(light, vEyePosition, vEyeNorm) + vec3(1.0f);
		}
		
}
