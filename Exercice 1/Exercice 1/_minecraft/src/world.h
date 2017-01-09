#ifndef __WORLD_H__
#define __WORLD_H__

#include "gl/glew.h"
#include "gl/freeglut.h" 
#include "engine/utils/types_3d.h"
#include "cube.h"
#include "chunk.h"
#include "PerlinNoise.h"

typedef uint8 NYAxis;
#define NY_AXIS_X 0x01
#define NY_AXIS_Y 0x02
#define NY_AXIS_Z 0x04

#define MAT_SIZE 2 //en nombre de chunks
#define MAT_HEIGHT 2 //en nombre de chunks
#define MAT_SIZE_CUBES (MAT_SIZE * NYChunk::CHUNK_SIZE)
#define MAT_HEIGHT_CUBES (MAT_HEIGHT * NYChunk::CHUNK_SIZE)


class NYWorld
{
public :
	NYChunk * _Chunks[MAT_SIZE][MAT_SIZE][MAT_HEIGHT];
	int _MatriceHeights[MAT_SIZE_CUBES][MAT_SIZE_CUBES];
	float _FacteurGeneration;
	PerlinNoise pn;

	NYWorld()
	{
		pn = PerlinNoise(237);

		_FacteurGeneration = 1.0;

		//On cr�e les chunks
		for(int x=0;x<MAT_SIZE;x++)
			for(int y=0;y<MAT_SIZE;y++)
				for(int z=0;z<MAT_HEIGHT;z++)
					_Chunks[x][y][z] = new NYChunk();

		for(int x=0;x<MAT_SIZE;x++)
			for(int y=0;y<MAT_SIZE;y++)
				for(int z=0;z<MAT_HEIGHT;z++)
				{
					NYChunk * cxPrev = NULL;
					if(x > 0)
						cxPrev = _Chunks[x-1][y][z];
					NYChunk * cxNext = NULL;
					if(x < MAT_SIZE-1)
						cxNext = _Chunks[x+1][y][z];

					NYChunk * cyPrev = NULL;
					if(y > 0)
						cyPrev = _Chunks[x][y-1][z];
					NYChunk * cyNext = NULL;
					if(y < MAT_SIZE-1)
						cyNext = _Chunks[x][y+1][z];

					NYChunk * czPrev = NULL;
					if(z > 0)
						czPrev = _Chunks[x][y][z-1];
					NYChunk * czNext = NULL;
					if(z < MAT_HEIGHT-1)
						czNext = _Chunks[x][y][z+1];

					_Chunks[x][y][z]->setVoisins(cxPrev,cxNext,cyPrev,cyNext,czPrev,czNext);
				}

					
	}

	inline NYCube * getCube(int x, int y, int z)
	{	
		if(x < 0)x = 0;
		if(y < 0)y = 0;
		if(z < 0)z = 0;
		if(x >= MAT_SIZE * NYChunk::CHUNK_SIZE) x = (MAT_SIZE * NYChunk::CHUNK_SIZE)-1;
		if(y >= MAT_SIZE * NYChunk::CHUNK_SIZE) y = (MAT_SIZE * NYChunk::CHUNK_SIZE)-1;
		if(z >= MAT_HEIGHT * NYChunk::CHUNK_SIZE) z = (MAT_HEIGHT * NYChunk::CHUNK_SIZE)-1;

		return &(_Chunks[x / NYChunk::CHUNK_SIZE][y / NYChunk::CHUNK_SIZE][z / NYChunk::CHUNK_SIZE]->_Cubes[x % NYChunk::CHUNK_SIZE][y % NYChunk::CHUNK_SIZE][z % NYChunk::CHUNK_SIZE]);
	}

	void updateCube(int x, int y, int z)
	{	
		if(x < 0)x = 0;
		if(y < 0)y = 0;
		if(z < 0)z = 0;
		if(x >= MAT_SIZE * NYChunk::CHUNK_SIZE)x = (MAT_SIZE * NYChunk::CHUNK_SIZE)-1;
		if(y >= MAT_SIZE * NYChunk::CHUNK_SIZE)y = (MAT_SIZE * NYChunk::CHUNK_SIZE)-1;
		if(z >= MAT_HEIGHT * NYChunk::CHUNK_SIZE)z = (MAT_HEIGHT * NYChunk::CHUNK_SIZE)-1;
		_Chunks[x / NYChunk::CHUNK_SIZE][y / NYChunk::CHUNK_SIZE][z / NYChunk::CHUNK_SIZE]->toVbo();
	}

	void deleteCube(int x, int y, int z)
	{
		NYCube * cube = getCube(x,y,z);
		cube->_Draw = false;
		cube = getCube(x-1,y,z);
		updateCube(x,y,z);	
	}

	//Cr�ation d'une pile de cubes
	//only if zero permet de ne g�n�rer la  pile que si sa hauteur actuelle est de 0 (et ainsi de ne pas reg�n�rer de piles existantes)
	void load_pile(int x, int y, int height, bool onlyIfZero = true)
	{
		for (int z = 1; z < height; z++)
			getCube(x, y, z)->_Type = CUBE_TERRE;
		getCube(x, y, 0)->_Type = CUBE_EAU;
		getCube(x, y, height)->_Type = CUBE_HERBE;
	}

	void generate_cube(int x, int y, int z, float noizOffset)
	{
		float noiz = pn.noise(x * 0.05, y * 0.05, z * 0.05 + noizOffset);
		float noizUp = pn.noise(x * 0.05, y * 0.05, (z + 1) * 0.05 + noizOffset);
		float noiz2 = pn.noise(x * 0.05, y * 0.05, 0 + noizOffset);
		NYCube* cube = getCube(x, y, z);
		if (noiz > 0.5 && z < noiz2 * MAT_HEIGHT_CUBES)
			if (noizUp <= 0.5 || z + 1 >= noiz2 * MAT_HEIGHT_CUBES)
				cube->_Type = CUBE_HERBE;
			else
				cube->_Type = CUBE_TERRE;

		else {
			if (z > 10)
				cube->_Type = CUBE_AIR;
			else
				cube->_Type = CUBE_EAU;
		}
	}

	void frame_cube_update()
	{
		static int x = 0;
		static int y = 0;
		static int z = 0;
		static float noizOffset = 0;

		//printf("%d, %d, %d\n", x, y, z);
		for (int i = 0; i < MAT_SIZE_CUBES * MAT_HEIGHT_CUBES; i++)
		{
			x++;
			if (x == MAT_SIZE_CUBES)
			{
				y++;
				x = 0;
				if (y == MAT_SIZE_CUBES)
				{
					z++;
					y = 0;
					if (z == MAT_HEIGHT_CUBES)
					{
						z = 0;
						noizOffset += 0.01;
						printf("%f\n", noizOffset);
					}
				}
			}
			generate_cube(x, y, z, noizOffset);
		}
	}

	void generate_piles_simplex()
	{
		for (int x = 0; x < MAT_SIZE_CUBES; x++)
			for (int y = 0; y < MAT_SIZE_CUBES; y++)
				for (int z = 0; z < MAT_HEIGHT_CUBES; z++)
					generate_cube(x, y, z, 0);			
	}

	//Creation du monde entier, en utilisant le mouvement brownien fractionnaire
	void generate_piles(int x1, int y1,
		int x2, int y2, 
		int x3, int y3,
		int x4, int y4, int prof, int profMax = -1)
	{

	}


	void lisse(void)
	{

	}

	


	void init_world(int profmax = -1)
	{
		_cprintf("Creation du monde %f \n",_FacteurGeneration);

		srand(6665);

		//Reset du monde
		for(int x=0;x<MAT_SIZE;x++)
			for(int y=0;y<MAT_SIZE;y++)
				for(int z=0;z<MAT_HEIGHT;z++)
					_Chunks[x][y][z]->reset();
		memset(_MatriceHeights,0x00,MAT_SIZE_CUBES*MAT_SIZE_CUBES*sizeof(int));

		//On charge les 4 coins
		load_pile(0,0,MAT_HEIGHT_CUBES/2);
		load_pile(MAT_SIZE_CUBES-1,0,MAT_HEIGHT_CUBES/2);
		load_pile(MAT_SIZE_CUBES-1,MAT_SIZE_CUBES-1,MAT_HEIGHT_CUBES/2);	
		load_pile(0,MAT_SIZE_CUBES-1,MAT_HEIGHT_CUBES/2);

		//On g�n�re a partir des 4 coins
		/* generate_piles(0,0,
			MAT_SIZE_CUBES-1,0,
			MAT_SIZE_CUBES-1,MAT_SIZE_CUBES-1,
			0,MAT_SIZE_CUBES-1,1,profmax);	*/
		generate_piles_simplex();

		for(int x=0;x<MAT_SIZE;x++)
			for(int y=0;y<MAT_SIZE;y++)
				for(int z=0;z<MAT_HEIGHT;z++)
					_Chunks[x][y][z]->disableHiddenCubes();

		/*for (int x = 0; x<MAT_SIZE_CUBES; x++)
			for (int y = 0; y<MAT_SIZE_CUBES; y++)
				for (int z = 0; z < MAT_HEIGHT_CUBES; z++)
				{
					NYCube* cube = getCube(x, y, z);
					float tmp = randf();
					if (tmp > 0.75)
						cube->_Type = CUBE_TERRE;
					else if (tmp > 0.5)
						cube->_Type = CUBE_EAU;
					else if (tmp > 0.25)
						cube->_Type = CUBE_HERBE;
					else
						cube->_Type = CUBE_AIR;
				}*/
					
	}

	NYCube * pick(NYVert3Df  pos, NYVert3Df  dir, NYPoint3D * point)
	{
		return NULL;
	}

	//Boites de collisions plus petites que deux cubes
	NYAxis getMinCol(NYVert3Df pos, float width, float height, float & valueColMin, int i)
	{
		NYAxis axis = 0x00;
		return axis;
	}


	void render_world_vbo(void)
	{
		for(int x=0;x<MAT_SIZE;x++)
			for(int y=0;y<MAT_SIZE;y++)
				for(int z=0;z<MAT_HEIGHT;z++)
				{
					glPushMatrix();
					glTranslatef((float)(x*NYChunk::CHUNK_SIZE*NYCube::CUBE_SIZE),(float)(y*NYChunk::CHUNK_SIZE*NYCube::CUBE_SIZE),(float)(z*NYChunk::CHUNK_SIZE*NYCube::CUBE_SIZE));
					_Chunks[x][y][z]->render();	
					glPopMatrix();
				}
	}

	void add_world_to_vbo(void)
	{
		int totalNbVertices = 0;
		
		for(int x=0;x<MAT_SIZE;x++)
			for(int y=0;y<MAT_SIZE;y++)
				for(int z=0;z<MAT_HEIGHT;z++)
				{
					_Chunks[x][y][z]->toVbo();
					totalNbVertices += _Chunks[x][y][z]->_NbVertices;
				}

		Log::log(Log::ENGINE_INFO,(toString(totalNbVertices) + " vertices in VBO").c_str());
	}

	void render_world_old_school(void)
	{
		//frame_cube_update();
		glEnable(GL_COLOR_MATERIAL);
		for (int x = 0; x<MAT_SIZE_CUBES; x++)
			for (int y = 0; y<MAT_SIZE_CUBES; y++)
				for (int z = 0; z<MAT_HEIGHT_CUBES; z++)
				{
					NYCube* cube = getCube(x, y, z);
					if (cube->_Draw)
					{
						if (cube->_Type != CUBE_AIR) {
							
							glPushMatrix();
							glTranslatef(x * cube->CUBE_SIZE, y * cube->CUBE_SIZE, z * cube->CUBE_SIZE);
							switch (cube->_Type) {
								case CUBE_EAU:
									glColor3d(0, 0, 1);
								break;
								case CUBE_HERBE:
									glColor3d(0, 1,0);
									break;
								case CUBE_TERRE:
									glColor3d(1, 0, 0);
									break;
							}
							glutSolidCube(cube->CUBE_SIZE);
							glPopMatrix();
						}
					}
				}
		glDisable(GL_COLOR_MATERIAL);
	}	
};



#endif