varying vec3 normal;
varying vec3 vertex_to_light_vector;
varying vec4 color;

uniform float elapsed;
uniform mat4 invertView;

float getOffset(float x, float y)
{
	return 7*sin(y/10 + elapsed);
}

void main()
{	
	// Transforming The Vertex
	
	//Retour dans l'espace monde
	mat4 model = invertView * gl_ModelViewMatrix;
	vec4 new_gl_Vertex = model * gl_Vertex;

	//Calcul de l'offset
	float offset = 0;
	if(gl_Color.z == 1)
	{
		offset = getOffset(new_gl_Vertex[0], new_gl_Vertex[1]);
	} 
	
	//Calcul des normales
	if(gl_Color.z == 1)
	{
		new_gl_Vertex = model * gl_Vertex;

		vec3 current = vec3(new_gl_Vertex.x, new_gl_Vertex.y, new_gl_Vertex.z);
		vec3 neig1 = vec3(current.x + 1, current.y, current.z + getOffset(current.x + 1, current.y));
		vec3 neig2 = vec3(current.x, current.y + 1, current.z + getOffset(current.x, current.y + 1));
		
		current.z += offset;
		
		vec3 tmp1 = normalize(neig1 - current);
		vec3 tmp2 = normalize(neig2 - current);
		
		vec3 normale = normalize(cross(tmp1,tmp2));
		
		// Transforming The Normal To ModelView-Space
		normal = gl_NormalMatrix * normale; 
	}
	else
	{
		normal = gl_NormalMatrix * gl_Normal;
	}
	
	new_gl_Vertex = gl_Vertex;
	new_gl_Vertex.z = new_gl_Vertex.z + offset;
	gl_Position = gl_ModelViewProjectionMatrix * new_gl_Vertex;
	
	//Direction lumiere
	vertex_to_light_vector = vec3(gl_LightSource[0].position);

	//Couleur
	color = gl_Color;
}