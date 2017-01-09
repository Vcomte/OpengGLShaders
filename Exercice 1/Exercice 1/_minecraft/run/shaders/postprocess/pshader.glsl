uniform sampler2D Texture0;
uniform sampler2D Texture1;
uniform float screen_width;
uniform float screen_height;

//PostProcess

float LinearizeDepth(float z)
{
	float n = 0.7; // camera z near
  	float f = 10000.0; // camera z far
  	return (2.0 * n) / (f + n - z * (f - n));
}

void main(void)
{
	float xstep = 1.0 / screen_width;
	float ystep = 1.0 / screen_height;
	float ratio = screen_width / screen_height;

	vec4 color = texture2D(Texture0, vec2(gl_TexCoord[0]));
	float depth = texture2D(Texture1, vec2(gl_TexCoord[0])).r;


	//Countouring
	//Points utilisés pour détecter les contours (n'a pas été mis en tableau / généré en boucle
	//	pour faciliter ma compréhension personnelle et ma relecture
	float depthTop = texture2D(Texture1, vec2(gl_TexCoord[0].x, gl_TexCoord[0].y + ystep)).r;
	float depthBottom = texture2D(Texture1, vec2(gl_TexCoord[0].x, gl_TexCoord[0].y - ystep)).r;
	float depthRight = texture2D(Texture1, vec2(gl_TexCoord[0].x + xstep, gl_TexCoord[0].y)).r;
	float depthLeft = texture2D(Texture1, vec2(gl_TexCoord[0].x - xstep, gl_TexCoord[0].y)).r;
	float depthTopRight = texture2D(Texture1, vec2(gl_TexCoord[0].x + xstep, gl_TexCoord[0].y + ystep)).r;
	float depthTopLeft = texture2D(Texture1, vec2(gl_TexCoord[0].x - xstep, gl_TexCoord[0].y + ystep)).r;
	float depthBottomRight = texture2D(Texture1, vec2(gl_TexCoord[0].x + xstep, gl_TexCoord[0].y - ystep)).r;
	float depthBottomLeft = texture2D(Texture1, vec2(gl_TexCoord[0].x - xstep, gl_TexCoord[0].y - ystep)).r;

	//Permet de scaler la profondeur
	depth = LinearizeDepth(depth);
	depthTop = LinearizeDepth(depthTop);
	depthBottom = LinearizeDepth(depthBottom);
	depthRight = LinearizeDepth(depthRight);
	depthLeft = LinearizeDepth(depthLeft);
	depthTopRight = LinearizeDepth(depthTopRight);
	depthTopLeft = LinearizeDepth(depthTopLeft);
	depthBottomLeft = LinearizeDepth(depthBottomLeft);
	depthBottomRight = LinearizeDepth(depthBottomRight);

	//Contouring par profondeur
	float gap = 8.0 * depth - depthTop - depthBottom - depthBottomLeft - depthBottomRight - depthLeft -
		depthRight - depthTopLeft - depthTopRight;
	gap = float(abs(gap));

	if (gap >= 0.0007 && color.z < 0.1) {
		color = vec4(0, 0, 0, 1);
	}
	//Blanc pour l'eau
	else if (gap >= 0.005 && color.z > 0.1) {
		color = vec4(0.3, 0.3, 0.3, 0);
	}


	//Vignettage
	//Calcul de la distance au centre de l'écran.
	float distance = sqrt(pow(gl_TexCoord[0].x - 0.5, 2) + pow(gl_TexCoord[0].y - 0.5, 2));
	//Une modification par multiplication directe d'un ratio entre distance et largeur est trop brutale
	//On fait donc un smoothstep, la première valeur représentant la largeur de la vignette, la deuxième sa hauteur.
	color.xyz *= smoothstep(0.7, 0.4, distance);
	
	
	//Profondeur de champ
	//Effet de flou en fonction de la profondeur, valeur inspirée par le filtre gaussien
	float ratioTable[] = { 0.05, 0.15, 0.3, 0.15, 0.05, 0.15, 0.6, 1.0, 0.6, 0.15, 0.3, 1.0, 1.0, 1.0, 0.3, 0.15, 0.6, 1, 0.6, 0.15, 0.05, 0.15, 0.3, 0.15, 0.05 };
	float count = 0;
	for (float i = 0.0; i < 5.0; i++) {
		for (float j = 0.0; j < 5.0; j++) {
			color += texture2D(Texture1, vec2(gl_TexCoord[0].x + i * xstep, gl_TexCoord[0].y + j * ystep)) * ratioTable[i + j] * depth;
			count += ratioTable[i + j];
		}
	}
	//color *= 1 / count;
	gl_FragColor = color * (1.0 - depth);
}