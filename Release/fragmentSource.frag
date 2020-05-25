#version 330 core							
in vec4 vertexColor;						
in vec3 FragPos;
in vec3 Normal;

uniform vec3 objColor;
uniform vec3 ambientColor;
uniform vec3 lightPos;//µ∆π‚Œª÷√
uniform vec3 lightColor;

out vec4 FragColor; 						
void main() {		
		vec3 lightDir = normalize(lightPos - FragPos);
		vec3 diffuse = dot( lightDir , Normal) * lightColor;
		//FragColor = vec4 (objColor * ambientColor,1.0);
		FragColor = vec4 ((ambientColor + diffuse) * objColor,1.0);

}