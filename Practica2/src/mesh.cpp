#include "mesh.h"
#include <cassert>
#include "includes.h"
#include "shader.h"
#include "utils.h"
#include "extra/textparser.h"

Mesh::Mesh()
{
	vertices_vbo_id = 0;
	normals_vbo_id = 0;
	uvs_vbo_id = 0;
	colors_vbo_id = 0;
}

Mesh::Mesh(const Mesh& m)
{
	vertices = m.vertices;
	normals = m.normals;
	uvs = m.uvs;
	colors = m.colors;
}

Mesh::~Mesh()
{
	if (vertices_vbo_id) glDeleteBuffersARB(1, &vertices_vbo_id);
	if (normals_vbo_id) glDeleteBuffersARB(1, &normals_vbo_id);
	if (uvs_vbo_id) glDeleteBuffersARB(1, &uvs_vbo_id);
	if (colors_vbo_id) glDeleteBuffersARB(1, &colors_vbo_id);
}

void Mesh::clear()
{
	vertices.clear();
	normals.clear();
	uvs.clear();
	colors.clear();
}


void Mesh::render(int primitive)
{
	assert(vertices.size() && "No vertices in this mesh");

	glEnableClientState(GL_VERTEX_ARRAY);

	if (vertices_vbo_id)
	{
		glBindBufferARB(GL_ARRAY_BUFFER_ARB, vertices_vbo_id);
		glVertexPointer(3, GL_FLOAT, 0, NULL);
	}
	else
		glVertexPointer(3, GL_FLOAT, 0, &vertices[0]);

	if (normals.size())
	{
		glEnableClientState(GL_NORMAL_ARRAY);
		if (normals_vbo_id)
		{
			glBindBufferARB(GL_ARRAY_BUFFER_ARB, normals_vbo_id);
			glNormalPointer(GL_FLOAT, 0, NULL);
		}
		else
			glNormalPointer(GL_FLOAT, 0, &normals[0]);
	}

	if (uvs.size())
	{
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		if (uvs_vbo_id)
		{
			glBindBufferARB(GL_ARRAY_BUFFER_ARB, uvs_vbo_id);
			glTexCoordPointer(2, GL_FLOAT, 0, NULL);
		}
		else
			glTexCoordPointer(2, GL_FLOAT, 0, &uvs[0]);
	}

	if (colors.size())
	{
		glEnableClientState(GL_COLOR_ARRAY);
		if (colors_vbo_id)
		{
			glBindBufferARB(GL_ARRAY_BUFFER_ARB, colors_vbo_id);
			glColorPointer(4, GL_FLOAT, 0, NULL);
		}
		else
			glColorPointer(4, GL_FLOAT, 0, &colors[0]);
	}

	glDrawArrays(primitive, 0, (GLsizei)vertices.size());
	glDisableClientState(GL_VERTEX_ARRAY);

	if (normals.size())
		glDisableClientState(GL_NORMAL_ARRAY);
	if (uvs.size())
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	if (colors.size())
		glDisableClientState(GL_COLOR_ARRAY);
	glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
}

void Mesh::render(int primitive, Shader* sh)
{
	if (!sh || !sh->compiled)
		return render(primitive);

	assert(vertices.size() && "No vertices in this mesh");

	int vertex_location = sh->getAttribLocation("a_vertex");
	assert(vertex_location != -1 && "No a_vertex found in shader");

	if (vertex_location == -1)
		return;

	glEnableVertexAttribArray(vertex_location);
	if (vertices_vbo_id)
	{
		glBindBufferARB(GL_ARRAY_BUFFER_ARB, vertices_vbo_id);
		glVertexAttribPointer(vertex_location, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	}
	else
		glVertexAttribPointer(vertex_location, 3, GL_FLOAT, GL_FALSE, 0, &vertices[0]);

	int normal_location = -1;
	if (normals.size())
	{
		normal_location = sh->getAttribLocation("a_normal");
		if (normal_location != -1)
		{
			glEnableVertexAttribArray(normal_location);
			if (normals_vbo_id)
			{
				glBindBufferARB(GL_ARRAY_BUFFER_ARB, normals_vbo_id);
				glVertexAttribPointer(normal_location, 3, GL_FLOAT, GL_FALSE, 0, NULL);
			}
			else
				glVertexAttribPointer(normal_location, 3, GL_FLOAT, GL_FALSE, 0, &normals[0]);
		}
	}

	int uv_location = -1;
	if (uvs.size())
	{
		uv_location = sh->getAttribLocation("a_uv");
		if (uv_location != -1)
		{
			glEnableVertexAttribArray(uv_location);
			if (uvs_vbo_id)
			{
				glBindBufferARB(GL_ARRAY_BUFFER_ARB, uvs_vbo_id);
				glVertexAttribPointer(uv_location, 2, GL_FLOAT, GL_FALSE, 0, NULL);
			}
			else
				glVertexAttribPointer(uv_location, 2, GL_FLOAT, GL_FALSE, 0, &uvs[0]);
		}
	}

	int color_location = -1;
	if (colors.size())
	{
		color_location = sh->getAttribLocation("a_color");
		if (color_location != -1)
		{
			glEnableVertexAttribArray(color_location);
			if (colors_vbo_id)
			{
				glBindBufferARB(GL_ARRAY_BUFFER_ARB, colors_vbo_id);
				glVertexAttribPointer(color_location, 4, GL_FLOAT, GL_FALSE, 0, NULL);
			}
			else
				glVertexAttribPointer(color_location, 4, GL_FLOAT, GL_FALSE, 0, &colors[0]);
		}
	}

	glDrawArrays(primitive, 0, (GLsizei)vertices.size());

	glDisableVertexAttribArray(vertex_location);
	if (normal_location != -1) glDisableVertexAttribArray(normal_location);
	if (uv_location != -1) glDisableVertexAttribArray(uv_location);
	if (color_location != -1) glDisableVertexAttribArray(color_location);
	glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
}

void Mesh::uploadToVRAM()
{
	//delete old
	if (vertices_vbo_id) glDeleteBuffersARB(1, &vertices_vbo_id);
	if (normals_vbo_id) glDeleteBuffersARB(1, &normals_vbo_id);
	if (uvs_vbo_id) glDeleteBuffersARB(1, &uvs_vbo_id);
	if (colors_vbo_id) glDeleteBuffersARB(1, &colors_vbo_id);

	glGenBuffersARB(1, &vertices_vbo_id); //generate one handler (id)
	glBindBufferARB(GL_ARRAY_BUFFER_ARB, vertices_vbo_id); //bind the handler
	glBufferDataARB(GL_ARRAY_BUFFER_ARB, vertices.size() * 3 * sizeof(float), &vertices[0], GL_STATIC_DRAW_ARB); //upload data

	if (normals.size())
	{
		glGenBuffersARB(1, &normals_vbo_id); //generate one handler (id)
		glBindBufferARB(GL_ARRAY_BUFFER_ARB, normals_vbo_id); //bind the handler
		glBufferDataARB(GL_ARRAY_BUFFER_ARB, normals.size() * 3 * sizeof(float), &normals[0], GL_STATIC_DRAW_ARB); //upload data
	}

	if (uvs.size())
	{
		glGenBuffersARB(1, &uvs_vbo_id); //generate one handler (id)
		glBindBufferARB(GL_ARRAY_BUFFER_ARB, uvs_vbo_id); //bind the handler
		glBufferDataARB(GL_ARRAY_BUFFER_ARB, uvs.size() * 2 * sizeof(float), &uvs[0], GL_STATIC_DRAW_ARB); //upload data
	}

	if (colors.size())
	{
		glGenBuffersARB(1, &colors_vbo_id); //generate one handler (id)
		glBindBufferARB(GL_ARRAY_BUFFER_ARB, colors_vbo_id); //bind the handler
		glBufferDataARB(GL_ARRAY_BUFFER_ARB, uvs.size() * 4 * sizeof(float), &colors[0], GL_STATIC_DRAW_ARB); //upload data
	}

}

void Mesh::createQuad(float center_x, float center_y, float w, float h, bool flip_uvs)
{
	vertices.clear();
	normals.clear();
	uvs.clear();
	colors.clear();

	//create six vertices (3 for upperleft triangle and 3 for lowerright)

	vertices.push_back(Vector3(center_x + w*0.5f, center_y + h*0.5f, 0.0f));
	vertices.push_back(Vector3(center_x - w*0.5f, center_y - h*0.5f, 0.0f));
	vertices.push_back(Vector3(center_x + w*0.5f, center_y - h*0.5f, 0.0f));
	vertices.push_back(Vector3(center_x - w*0.5f, center_y + h*0.5f, 0.0f));
	vertices.push_back(Vector3(center_x - w*0.5f, center_y - h*0.5f, 0.0f));
	vertices.push_back(Vector3(center_x + w*0.5f, center_y + h*0.5f, 0.0f));

	//texture coordinates
	uvs.push_back(Vector2(1.0f, flip_uvs ? 0.0f : 1.0f));
	uvs.push_back(Vector2(0.0f, flip_uvs ? 1.0f : 0.0f));
	uvs.push_back(Vector2(1.0f, flip_uvs ? 1.0f : 0.0f));
	uvs.push_back(Vector2(0.0f, flip_uvs ? 0.0f : 1.0f));
	uvs.push_back(Vector2(0.0f, flip_uvs ? 1.0f : 0.0f));
	uvs.push_back(Vector2(1.0f, flip_uvs ? 0.0f : 1.0f));

	//all of them have the same normal
	normals.push_back(Vector3(0.0f, 0.0f, 1.0f));
	normals.push_back(Vector3(0.0f, 0.0f, 1.0f));
	normals.push_back(Vector3(0.0f, 0.0f, 1.0f));
	normals.push_back(Vector3(0.0f, 0.0f, 1.0f));
	normals.push_back(Vector3(0.0f, 0.0f, 1.0f));
	normals.push_back(Vector3(0.0f, 0.0f, 1.0f));
}


void Mesh::createPlane(float size)
{
	vertices.clear();
	normals.clear();
	uvs.clear();
	colors.clear();

	//create six vertices (3 for upperleft triangle and 3 for lowerright)

	vertices.push_back(Vector3(size, 0, size));
	vertices.push_back(Vector3(size, 0, -size));
	vertices.push_back(Vector3(-size, 0, -size));
	vertices.push_back(Vector3(-size, 0, size));
	vertices.push_back(Vector3(size, 0, size));
	vertices.push_back(Vector3(-size, 0, -size));

	//all of them have the same normal
	normals.push_back(Vector3(0, 1, 0));
	normals.push_back(Vector3(0, 1, 0));
	normals.push_back(Vector3(0, 1, 0));
	normals.push_back(Vector3(0, 1, 0));
	normals.push_back(Vector3(0, 1, 0));
	normals.push_back(Vector3(0, 1, 0));

	//texture coordinates
	uvs.push_back(Vector2(1, 1));
	uvs.push_back(Vector2(1, 0));
	uvs.push_back(Vector2(0, 0));
	uvs.push_back(Vector2(0, 1));
	uvs.push_back(Vector2(1, 1));
	uvs.push_back(Vector2(0, 0));
}

bool Mesh::loadASE(const char* filename)
{
	float parseTime = 0;
	long initialTime = getTime();

	vertices.clear();
	normals.clear();
	uvs.clear();
	colors.clear();

	TextParser t;
	std::vector<Vector3> uniqueVertices;
	std::vector<Vector2> uniquetVertices;

	//Creamos el Parser de la Malla
	if (t.create(filename) == false)
	{
		return false;
	}

	//Número de Vértices de la Malla
	t.seek("*MESH_NUMVERTEX");
	int num_vertices = t.getint();

	//Número de caras de la Malla
	t.seek("*MESH_NUMFACES");
	int num_faces = t.getint();

	//Añadimos los vertices de la Malla al vector de uniqueVertices
	uniqueVertices.resize(num_vertices);
	//std::cout << num_vertices << std::endl;

	for (int i = 0; i < num_vertices; i++)
	{
		t.seek("*MESH_VERTEX");
		t.getint();
		Vector3 v;
		v.x = t.getfloat();
		v.z = -t.getfloat();
		v.y = t.getfloat();
		uniqueVertices[i] = v;
	}

	//Sacamos los vertices para las diferentes caras de la Malla
	for (int i = 0; i < num_faces; i++)
	{
		t.seek("*MESH_FACE");
		t.getword();
		t.getword();

		int A = t.getint();
		t.getword();
		int B = t.getint();
		t.getword();
		int C = t.getint();

		vertices.push_back(uniqueVertices[A]);
		vertices.push_back(uniqueVertices[B]);
		vertices.push_back(uniqueVertices[C]);
		//t.getword();

	}
	
	//Número de Vértices para texturas de la Malla
	t.seek("*MESH_NUMTVERTEX");
	int num_tvertices = t.getint();

	uniquetVertices.resize(num_tvertices);
	//std::cout << num_tvertices << std::endl;

	for (int i = 0; i < num_tvertices; i++)
	{
		t.seek("*MESH_TVERT");
		t.getint();
		Vector2 v;
		v.x = t.getfloat();
		v.y = t.getfloat();
		uniquetVertices[i] = v;
	}

	//Número de caras para texturas de la Malla
	t.seek("*MESH_NUMTVFACES");
	int num_tfaces = t.getint();

	//Sacamos los vertices para las diferentes caras de la Malla (para texturas)
	for (int i = 0; i < num_tfaces; i++)
	{
		t.seek("*MESH_TFACE");
		t.getint();

		int A = t.getint();
		int B = t.getint();
		int C = t.getint();

		uvs.push_back(uniquetVertices[A]);

		uvs.push_back(uniquetVertices[B]);

		uvs.push_back(uniquetVertices[C]);
		//t.getword();

	}

	//Sacamos las normales para las diferentes caras de la Malla
	for (int i = 0; i < num_faces*3; i++)
	{
		t.seek("*MESH_VERTEXNORMAL");
		t.getint();
		Vector3 v;
		v.x = t.getfloat();		
		v.z = -t.getfloat();
		v.y = t.getfloat();
		normals.push_back(v);

	}

	//Probamos si las uvs son correctas
	/*
	for (int i = 0; i < uvs.size(); i++)
	{
		Vector4 v;
		v.x = uvs[i].x;
		v.y = uvs[i].y;
		v.z = 0;
		v.w = 0;
		colors.push_back(v);
	}*/

	//Probamos si las normales son correctas
	/*
	for (int i = 0; i < normals.size(); i++)
	{
		Vector4 v;
		v.x = normals[i].x;
		v.y = normals[i].y;
		v.z = normals[i].z;
		v.w = 0;
		colors.push_back(v);
	}
	*/

	//Guardamos archivo binario con la información
	std::string bin_filename;
	bin_filename = filename + std::string(".bin");
	writeBIN(bin_filename.c_str());

	//Calculamos tiempo
	float finalTime = getTime();
	parseTime = float(finalTime - initialTime) * 0.001;	

	//Printamos tiempo transcurrido
	std::cout << "Tiempo de parseo de la Malla: " << parseTime << " segundos" << std::endl;

	return true;
}

bool Mesh::writeBIN(const char * filename)
{
	
	//Guardamos la información de cabecera
	sMeshBin header;
	header.format[0] = 'M';
	header.format[1] = 'E';
	header.format[2] = 'S';
	header.format[3] = 'H';
	header.num_vertices = vertices.size();
	header.num_normals = normals.size();
	header.num_uvs = uvs.size();
	//header.num_colors = colors.size();

	//Guardamos Bounding box	
	bounds = calcBoundingBox();

	boundingBox.center = Vector3((bounds[0] + bounds[1])/2, (bounds[2] + bounds[3]) / 2, (bounds[4] + bounds[5]) / 2);
	boundingBox.half_size = Vector3(bounds[1] - boundingBox.center.x, bounds[3] - boundingBox.center.y, bounds[5] - boundingBox.center.z);

	//Creamos el fichero binario
	
	std::string pathname = "../bin/";
	FILE* f = fopen(filename, "wb");

		if (f == NULL)
		{
			return false;
		}

	//Escribimos en el fichero los datos de la mesh
	fwrite(&header, sizeof(sMeshBin), 1, f);
	fwrite(&boundingBox, sizeof(sBounding), 1, f);
	fwrite(&vertices[0], sizeof(Vector3), vertices.size(), f);
	fwrite(&normals[0], sizeof(Vector3), normals.size(), f);
	fwrite(&uvs[0], sizeof(Vector2), uvs.size(), f);
	//fwrite(&colors[0], sizeof(Vector3), colors.size(), f);

	//Cerramos el fichero
	fclose(f);

	return true;
}

std::vector<float> Mesh::calcBoundingBox()
{
	std::vector<float> bounds;
	float maxX = -INFINITE;
	float minX = INFINITE;
	float maxY = -INFINITE;
	float minY = INFINITE;
	float maxZ = -INFINITE;
	float minZ = INFINITE;

	for (int i = 0; i < vertices.size(); i++)
	{
		Vector3 v = vertices[i];

		if (v.x > maxX)
		{
			maxX = v.x;
		}
		if (v.x < minX)
		{
			minX = v.x;
		}
		if (v.y > maxY)
		{
			maxY = v.y;
		}
		if (v.y < minY)
		{
			minY = v.z;
		}
		if (v.z > maxZ)
		{
			maxZ = v.z;
		}
		if (v.z < minZ)
		{
			minZ = v.y;
		}

	}
	bounds.push_back(minX);
	bounds.push_back(maxX);
	bounds.push_back(minY);
	bounds.push_back(maxX);
	bounds.push_back(minZ);
	bounds.push_back(maxZ);
	return bounds;
}

bool Mesh::readBIN(const char * filename)
{
	sMeshBin header;
	sBounding boundingBox;

	//Abrimos el fichero binario
	if (FILE* f = fopen(filename, "rb"))
	{

		//Leemos y guardamos cabecera
		fread(&header, sizeof(sMeshBin), 1, f);

		//Comprobamos el formato
		if (header.format[0] == 'M' && header.format[1] == 'E' && header.format[2] == 'S' && header.format[3] == 'H')
		{
			//Leemos y guardamos la bounding box
			fread(&boundingBox, sizeof(sBounding), 1, f);

			//Leemos y guardamos vertices
			vertices.resize(header.num_vertices);
			fread(&vertices[0], sizeof(Vector3), header.num_vertices, f);

			//Leemos y guardamos normales
			normals.resize(header.num_normals);
			if (normals.size())
				fread(&normals[0], sizeof(Vector3), header.num_normals, f);

			//Leemos y guardamos uvs
			uvs.resize(header.num_uvs);
			if (uvs.size())
				fread(&uvs[0], sizeof(Vector2), header.num_uvs, f);

			/*if (colors.size())
			{
				colors.resize(header.num_colors);
				fread(&colors[0], sizeof(Vector3), header.num_colors, f);
			}*/
		}
		else {

			std::cout << "Archivo binario de formato desconocido" << std::endl;
			return false;
		}
		return true;
	}

	return false;
	
}


