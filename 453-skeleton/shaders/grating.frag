#version 330 core
out vec4 out_color;

in vec3 norm;
in vec3 FragPos;  
in vec3 cube_norm;
in vec3 out_orientation;
flat in int vert_mat;

uniform vec3 cameraPos;
uniform vec3 lightPos;
uniform vec3 in_center;

uniform float kd;
uniform float ks;
uniform float ka;
uniform float d;

uniform float in_bodyCol;
uniform float particle_diameter;

uniform int flags;

vec3 wavelengthToRGB(float wavelength)
{
	float R = 0.f;
    float G = 0.f;
    float B = 0.f;

    if (wavelength >= 380.f && wavelength <= 440.f) {
        float attenuation = 0.3 + 0.7 * (wavelength - 380) / (440 - 380);
        R = (-(wavelength - 440) / (440 - 380)) * attenuation;
        G = 0.0;
        B = 1.0 * attenuation;
        }
    else if (wavelength >= 440 && wavelength <= 490) {
        R = 0.0;
        G = (wavelength - 440) / (490 - 440);
        B = 1.0;
        }
    else if (wavelength >= 490 && wavelength <= 510) {
        R = 0.0;
        G = 1.0;
        B = -(wavelength - 510) / (510 - 490);
        }
    else if (wavelength >= 510 && wavelength <= 580) {
        R = (wavelength - 510) / (580 - 510);
        G = 1.0;
        B = 0.0;
        }
    else if (wavelength >= 580 && wavelength <= 645) {
        R = 1.0;
        G = -(wavelength - 645) / (645 - 580);
        B = 0.0;
        }
    else if (wavelength >= 645 && wavelength <= 750) {
        float attenuation = 0.3 + 0.7 * (750 - wavelength) / (750 - 645);
        R = 1.0 * attenuation;
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
    vec3 geometricNorm = normalize(FragPos - in_center); //vector from center of opal to voxel, used for shading 
    vec3 lightDir = normalize(lightPos - FragPos);
    vec3 cameraDir = normalize(cameraPos - FragPos);
    vec3 bodyCol = vec3(in_bodyCol);
    vec3 stoneColor = vec3(0.54);
    //phong is calculated using the cube norm not the cluster norm
    vec3 diffuse = max(kd * dot(geometricNorm, lightDir), 0) * vec3(1);
    vec3 specular = max(ks * pow(dot(reflect(lightDir, geometricNorm), cameraDir), 32), 0) * vec3(1);
    vec3 ambiant = ks * vec3(1);
    if(vert_mat == 0) {
        //render the void
        vec3 col = stoneColor * (diffuse + specular + ambiant);
        out_color = vec4(col, 1);
    } else {
        //render the cluster
	    //vec3 lightDir = normalize(vec3(1, 5, 3));
	    //color = vec4(vertCol, 1);//vec4(max(0, dot(norm, lightDir)) * vertCol, vertAlpha);
	    float camAngle = acos(dot(cameraDir, norm));
        float lightAngle = acos(dot(lightDir, norm));
	    float wavelength = ((sin(camAngle)/* - sin(lightAngle)*/)*d)/1.f;
        wavelength = max(wavelength,0.f);

        float maxWavelength = 750;
        float minWaveLength = 380;
        if((flags & 1) == 1) {
            maxWavelength = 0.247 * particle_diameter; //fromSamders 1964 Color of Precious Opal
            minWaveLength = 0.72 * maxWavelength;
        } 

        vec3 col = vec3(0);

        if(wavelength < 380 || wavelength < minWaveLength || wavelength > 750 || wavelength > maxWavelength) {
            col = bodyCol;
        }
        else if (wavelength >= 380.f && wavelength <= 440.f) {
            float attenuation = 0.3 + 0.7 * (wavelength - 380) / (440 - 380);
            float R = (-(wavelength - 440) / (440 - 380)) * attenuation;
            float B = 1.0 * attenuation;
            col = vec3(R + (1-attenuation) * bodyCol.r, bodyCol.g, B + (1-attenuation) * bodyCol.b);

        }
        else if (wavelength >= 645 && wavelength <= 750) {
            float attenuation = 0.3 + 0.7 * (750 - wavelength) / (750 - 645);
            float R = 1.0 * attenuation;
            col = vec3(R + (1-attenuation) * bodyCol.r, bodyCol.g, bodyCol.b);
        } else {
            col = wavelengthToRGB(wavelength);
        }

        
        vec3 phong_col = col * (diffuse + specular + ambiant);
	    out_color = vec4(col + phong_col,1);
    }

}
