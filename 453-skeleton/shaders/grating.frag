#version 330 core
out vec4 out_color;

in vec3 norm;
in vec3 FragPos;  
in vec3 cube_norm;
in vec3 out_orientation;
flat in int vert_mat;

uniform vec3 cameraPos;
uniform vec3 lightPos;

uniform float kd;
uniform float ks;
uniform float ka;

vec3 wavelengthToRGB(float wavelength)
{
	float R = 0.f;
    float G = 0.f;
    float B = 0.f;
    float gamma = 0.8;

    if (wavelength >= 380.f && wavelength <= 440.f) {
        float attenuation = 0.3 + 0.7 * (wavelength - 380) / (440 - 380);
        R = pow((-(wavelength - 440) / (440 - 380)) * attenuation, gamma);
        G = 0.0;
        B = pow(1.0 * attenuation, gamma);
        }
    else if (wavelength >= 440 && wavelength <= 490) {
        R = 0.0;
        G = pow((wavelength - 440) / (490 - 440), gamma);
        B = 1.0;
        }
    else if (wavelength >= 490 && wavelength <= 510) {
        R = 0.0;
        G = 1.0;
        B = pow(-(wavelength - 510) / (510 - 490), gamma);
        }
    else if (wavelength >= 510 && wavelength <= 580) {
        R = pow((wavelength - 510) / (580 - 510), gamma);
        G = 1.0;
        B = 0.0;
        }
    else if (wavelength >= 580 && wavelength <= 645) {
        R = 1.0;
        G = pow(-(wavelength - 645) / (645 - 580), gamma);
        B = 0.0;
        }
    else if (wavelength >= 645 && wavelength <= 750) {
        float attenuation = 0.3 + 0.7 * (750 - wavelength) / (750 - 645);
        R = pow(1.0 * attenuation, gamma);
        G = 0.0;
        B = 0.0;
        }
    else {
        R = 0.0;
        G = 0.0;
        B = 0.0;
        }

	return vec3(R,G,B);
}

void main() {
    vec3 lightDir = normalize(lightPos - FragPos);
    vec3 cameraDir = normalize(cameraPos - FragPos);
    vec3 bodyCol = vec3(0.74);
    vec3 stoneColor = vec3(0.54);
    //phong is calculated using the cube norm not the cluster norm
    vec3 diffuse = max(kd * dot(cube_norm, lightDir), 0) * vec3(1);
    vec3 specular = max(ks * pow(dot(reflect(lightDir, cube_norm), cameraDir), 32), 0) * vec3(1);
    vec3 ambiant = ks * vec3(1);
    if(vert_mat == 0) {
        //render the void
        vec3 col = stoneColor * (diffuse + specular + ks);
        out_color = vec4(col, 1);
    } else {
        //render the cluster
	    //vec3 lightDir = normalize(vec3(1, 5, 3));
	    //color = vec4(vertCol, 1);//vec4(max(0, dot(norm, lightDir)) * vertCol, vertAlpha);
	    float camAngle = acos(dot(cameraDir, norm));
        float lightAngle = acos(dot(lightDir, norm));
	    float wavelength = ((sin(camAngle)/* - sin(lightAngle)*/)*1400.f)/1.f;
        wavelength = max(wavelength,0.f);
        //float alpha = wavelength == 0 ? 0.13 : 1;
        vec3 col = wavelength > 0 ? wavelengthToRGB(wavelength) : bodyCol;
	    out_color = vec4(col,1);
    }

}
