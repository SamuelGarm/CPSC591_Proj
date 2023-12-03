#version 330 core
out vec4 out_color;

in vec3 norm;
in vec3 FragPos;  
in vec3 out_orientation;

uniform vec3 cameraPos;

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
//rotate normal
	vec3 lightDir = normalize(vec3(1, 5, 3));
	//color = vec4(vertCol, 1);//vec4(max(0, dot(norm, lightDir)) * vertCol, vertAlpha);
    vec3 cameraDir = normalize(cameraPos - FragPos);
	float camAngle = acos(dot(cameraDir, norm));
    float lightAngle = acos(dot(lightDir, norm));
	float wavelength = ((sin(camAngle) - sin(lightAngle))*1400.f)/1.f;
    wavelength = max(wavelength,0.f);
	out_color = vec4(wavelengthToRGB(wavelength),1);

}
