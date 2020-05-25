#version 330 core
layout (location = 6) in vec3 aPos;
//layout (location = 7) in vec3 aColor;//u
//Œ∆¿Ì
//layout (location = 8) in vec2 aTexCoord;//v
layout (location = 9) in vec3 aNormal;

//uniform mat4 transform;
uniform mat4 modelMat;
uniform mat4 viewMat;
uniform mat4 projMat;

//out vec2 TexCoord;
//out vec4 vertexColor;
out vec3 FragPos;
out vec3 Normal;

void main(){
    //gl_Position = transform * vec4(aPos.x, aPos.y, aPos.z, 1.0);
	//¥””“œÚ◊Û
	gl_Position = projMat * viewMat * modelMat * vec4(aPos.xyz, 1.0);
	Normal = mat3(modelMat) * aNormal;
	//vertexColor = vec4(aColor.x,aColor.y,aColor.z,1.0f);
	//TexCoord = aTexCoord;
}
