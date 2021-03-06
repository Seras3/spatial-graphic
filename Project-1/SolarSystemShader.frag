// Shader-ul de fragment / Fragment shader  
 
#version 400

in vec4 ex_Color;
in vec2 tex_Coord;
out vec4 out_Color;

uniform sampler2D myTexture;
uniform int tex_code;
uniform int col_code;

void main(void)
{
	out_Color = ex_Color;
	
	if(tex_code == 1) {
		vec4 texColor =  texture(myTexture, tex_Coord);
		if(texColor.x == 0 && texColor.y == 0 && texColor.z == 0)
			discard;
		out_Color = texColor;
	}

	if(col_code == -1) {
		out_Color = vec4(0.25, 0.25, 0.25, 1.0);
	}


	// Sun
	if(col_code == 99) {
		out_Color = mix(ex_Color, vec4(1, 0.5, 0.2, 1.0), 0.7);
	}
	
	// Mercury
	if(col_code == 1) {    
		out_Color = mix(ex_Color, vec4(0.71, 0.655, 0.655, 1.0), 0.7);
	}

	// Venus
	if(col_code == 2) {
		out_Color = mix(ex_Color, vec4(0.976, 0.76, 0.102, 1.0), 0.7);
	}

	// Earth
	if(col_code == 3) {
		out_Color = mix(ex_Color, vec4(0.25, 0.4, 0.208, 1.0), 0.7);
	}

	// Moon
	if(col_code == 31) {
		out_Color = mix(ex_Color, vec4(0.94, 0.906, 0.906, 1.0), 0.7);
	}

	// Mars
	if(col_code == 4) {
		out_Color = mix(ex_Color, vec4(0.757, 0.267, 0.055, 1.0), 0.7);
	}

	// Jupiter
	if(col_code == 5) {
		out_Color = mix(ex_Color, vec4(0.65, 0.57, 0.525, 1.0), 0.7);
	}

	// Saturn
	if(col_code == 6) {
		out_Color = mix(ex_Color, vec4(0.9, 0.878, 0.752, 1.0), 0.7);
	}

	// Uranus
	if(col_code == 7) {
		out_Color = mix(ex_Color, vec4(0.31, 0.815, 0.906, 1.0), 0.7);
	}

	// Neptune
	if(col_code == 8) {
		out_Color = mix(ex_Color, vec4(0.247, 0.329, 0.729, 1.0), 0.7);
	}

}
 