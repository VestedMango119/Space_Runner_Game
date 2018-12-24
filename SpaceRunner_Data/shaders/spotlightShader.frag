#version 400 core

// Structure holding light information:  its position as well as ambient, diffuse, and specular colours
struct LightInfo
{
	vec4 position;
	vec3 La;
	vec3 Ld;
	vec3 Ls;

	vec3 direction; 
	float exponent; 
	float cutoff;

};

// Structure holding material information:  its ambient, diffuse, and specular colours, and shininess
struct MaterialInfo
{
	vec3 Ma;
	vec3 Md;
	vec3 Ms;
	float shininess;
};

// Lights and materials passed in as uniform variables from client programme
uniform LightInfo light1; //lightMode 
uniform LightInfo light2; //Statue coloured
uniform LightInfo light3; //undership light
uniform LightInfo light4; //ship headlight
uniform LightInfo light5; //extra statue light
uniform LightInfo light6;
uniform LightInfo light7;
uniform LightInfo tree[10];
uniform MaterialInfo material1; 

uniform bool reflectOn;

in vec2 vTexCoord;			// Interpolated texture coordinate using texture coordinate from the vertex shader
in vec3 vViewRay;
out vec4 vOutputColour;		// The output colour

uniform sampler2D sampler0;  // The texture sampler
uniform sampler2D bumpMap;
uniform samplerCube CubeMapTex;
uniform bool bUseTexture;    // A flag indicating if texture-mapping should be applied
uniform bool renderSkybox;
uniform bool activate;
uniform sampler2D uTexLinearDepth;

uniform bool bumpMapActive;


in vec3 worldPosition;

in vec3 vEyeNorm;
in vec4 vEyePosition;

in vec3 reflected;



vec3 PhongModel(LightInfo light, vec4 eyePosition, vec3 eyeNorm)
{
	vec3 s = normalize(vec3(light.position - eyePosition));
	vec3 v = normalize(-eyePosition.xyz);
	vec3 r = normalize(reflect(-s, eyeNorm));
	vec3 n = eyeNorm;
	vec3 ambient = light.La * material1.Ma;
	float sDotN = max(dot(s, n), 0.0);
	vec3 diffuse = light.Ld * material1.Md * sDotN;
	vec3 specular = vec3(0.0f);
	float eps = 0.000001f; // add eps to shininess below -- pow not defined if second argument is 0 (as described in GLSL documentation)
	if (sDotN > 0.0f) 
		specular = light.Ls * material1.Ms * pow(max(dot(r, v), 0.0f), material1.shininess + eps);
	

	return ambient + diffuse + specular;

}



vec3 BlinnPhongSpotlightModel(LightInfo light, vec4 p, vec3 n)
{
	vec3 s = normalize(vec3(light.position - p));
	float angle = acos(dot(-s, light.direction));
	float cutoff = radians(clamp(light.cutoff, 0.0, 90.0));
	vec3 ambient = vec3(0.02f);
	if ( angle < cutoff) {
		float spotFactor = pow(dot(-s, light.direction), light.exponent);
		vec3 v = normalize(-p.xyz);
		vec3 h = normalize(v+s);
		n = normalize(n);
		float sDotN = max(dot(s, n), 0.0);
		vec3 diffuse = light.Ld * material1.Md *sDotN;
		vec3 specular = vec3(0.0);
		if(sDotN > 0.0)
			specular = light.Ls *material1.Ms * pow(max(dot(h, n), 0.0), material1.shininess);
		return ambient +spotFactor *(diffuse+ specular);
	}else{
		return ambient;
	}
}


void main()
{
	
	
	vec3 vColour;

	if (renderSkybox) {
		vOutputColour = texture(CubeMapTex, worldPosition);

	}else if(reflectOn){

			vColour = PhongModel(light1, vEyePosition, vEyeNorm);

			vOutputColour =vec4(vColour, 0.5) + texture(CubeMapTex, normalize(reflected));


	} else {

		if (bUseTexture){
			vec4 vTexColour = texture(sampler0, vTexCoord);	
			if(bumpMapActive){		
				
				vec3 normal = 2.0 * texture2D (bumpMap, vTexCoord.st).rgb - 1.0;
				normal = normalize (normal);
				
				if(activate){
					vColour = BlinnPhongSpotlightModel(light2, vEyePosition, normalize(normal*vEyeNorm)) + BlinnPhongSpotlightModel(light3, vEyePosition, normalize(normal*vEyeNorm)) + BlinnPhongSpotlightModel(light4, vEyePosition, normalize(normal*vEyeNorm)) + BlinnPhongSpotlightModel(light5, vEyePosition, normalize(normal*vEyeNorm)) + BlinnPhongSpotlightModel(light6, vEyePosition, normalize(normal*vEyeNorm));
					for(int i = 0 ; i < 10; i++){
						vColour += BlinnPhongSpotlightModel(tree[i], vEyePosition, normalize(vEyeNorm));
					}
				} else{
					vColour = PhongModel(light1, vEyePosition, normalize(normal*vEyeNorm));
				}
			} else {
				if(activate){
					vColour = BlinnPhongSpotlightModel(light2, vEyePosition, normalize(vEyeNorm)) + BlinnPhongSpotlightModel(light3, vEyePosition, normalize(vEyeNorm)) + BlinnPhongSpotlightModel(light4, vEyePosition, normalize(vEyeNorm)) + BlinnPhongSpotlightModel(light5, vEyePosition, normalize(vEyeNorm)) + BlinnPhongSpotlightModel(light6, vEyePosition, normalize(vEyeNorm)) + BlinnPhongSpotlightModel(light7, vEyePosition, normalize(vEyeNorm));
					for(int i = 0 ; i < 10; i++){
						vColour += BlinnPhongSpotlightModel(tree[i], vEyePosition, normalize(vEyeNorm));
					}
				} else {
					vColour = PhongModel(light1, vEyePosition, normalize(vEyeNorm));
				}
			}
			vOutputColour = vTexColour* vec4(vColour, 1.0f);

		} else {	
			if(activate){
				vColour = BlinnPhongSpotlightModel(light2, vEyePosition, normalize(vEyeNorm)) + BlinnPhongSpotlightModel(light3, vEyePosition, normalize(vEyeNorm)) + BlinnPhongSpotlightModel(light4, vEyePosition, normalize(vEyeNorm)) + BlinnPhongSpotlightModel(light5, vEyePosition, normalize(vEyeNorm)) + BlinnPhongSpotlightModel(light6, vEyePosition, normalize(vEyeNorm));
				for(int i = 0 ; i < 10; i++){
						vColour += BlinnPhongSpotlightModel(tree[i], vEyePosition, normalize(vEyeNorm));
					}

			} else{
		
				vColour = PhongModel(light1, vEyePosition, normalize(vEyeNorm));

			}
			vOutputColour = vec4(vColour, 1.0f); // Just use the colour instead
		}
	}

}
