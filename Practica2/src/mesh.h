/*  by Javi Agenjo 2013 UPF  javi.agenjo@gmail.com
	The Mesh contains the info about how to render a mesh and also how to parse it from a file.
*/

#ifndef MESH_H
#define MESH_H

#include <vector>
#include "framework.h"

class Shader;

class Mesh
{
public:
	std::vector< Vector3 > vertices; //here we store the vertices
	std::vector< Vector3 > normals;	 //here we store the normals
	std::vector< Vector2 > uvs;	 //here we store the texture coordinates
	std::vector< Vector4 > colors;	 //here we store colors by vertex

	unsigned int vertices_vbo_id;
	unsigned int normals_vbo_id;
	unsigned int uvs_vbo_id;
	unsigned int colors_vbo_id;

	std::vector<float> bounds;

	//Definimos la estructura de la cabecera
	struct sMeshBin
	{
		char format[4];
		int num_vertices;
		int num_normals;
		int num_uvs;
		int num_colors;
	};

	//Definimos la bounding box
	struct sBounding
	{
		Vector3 center;
		Vector3 half_size;
	};

	Mesh();
	Mesh( const Mesh& m );
	~Mesh();

	void clear();
	void render(int primitive);
	void render(int primitive, Shader* sh);

	void uploadToVRAM(); //store the mesh info in the GPU RAM so it renders faster

	void createPlane( float size );
	void createQuad(float center_x, float center_y, float w, float h, bool flip_uvs = false);

	bool loadASE(const char * filename);
	bool writeBIN(const char * filename);
	bool readBIN(const char * filename);
	std::vector<float> calcBoundingBox();

	sBounding boundingBox;
};

#endif