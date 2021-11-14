// Shader-ul de fragment / Fragment shader  
 
#version 400

in vec4 ex_Color;
in vec2 tex_Coord;
out vec4 out_Color;

uniform sampler2D myTexture;
uniform int tex_code;

void main(void)
{
	out_Color = ex_Color;
	
	if(tex_code == 1) {
		out_Color = texture(myTexture, tex_Coord);
	}
}
 