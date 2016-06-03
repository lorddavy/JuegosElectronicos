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

bool Mesh::loadASE(const char* filename, const char* path_bin)
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

	/*std::stringstream ss;
	ss << filename << ".bin";
	std::string str = ss.str();
	const char* path = str.c_str();*/

	writeBIN(path_bin);

	//Calculamos tiempo
	float finalTime = getTime();
	parseTime = float(finalTime - initialTime) * 0.001;	

	//Printamos tiempo transcurrido
	std::cout << "Tiempo de parseo de la Malla: " << parseTime << " segundos" << std::endl;

	return true;
}

/*bool Mesh::loadOBJ(const char* filename, const char* path_bin) {
	float parseTime = 0;
	long initialTime = getTime();
	std::string word;

	vertices.clear();
	normals.clear();
	uvs.clear();
	colors.clear();

	TextParser t;
	std::vector<Vector3> uniqueVertices;
	std::vector<Vector2> textureVertices;

	struct stat stbuffer;
	stat(filename, &stbuffer);

	//Creamos el Parser de la Malla
	if (t.create(filename) == false)
	{
		return false;
	}

	while (!t.eof()) {
		word = t.getword();
		if (word == "#")
		{
			std::cout << "Find #" << std::endl;
		}
		else if (word == "V")
		{
			//Vertices
			Vector3 v;
			v.x = t.getfloat();
			v.z = -t.getfloat();
			v.y = t.getfloat();

			uniqueVertices.push_back(v);

			std::cout << "Find vertex: " << v.x << ", " << -v.z << ", " << v.y << std::endl;
		}
		else if (word == "VT")
		{
			//Texture Vertices
			Vector2 v;
			v.x = t.getfloat();
			v.y = t.getfloat();

			textureVertices.push_back(v);

			std::cout << "Find UVs: " << v.x << ", " << v.y << std::endl;
		}
		else if (word == "VN")
		{
			//Normals
			Vector3 v;
			v.x = t.getfloat();
			v.z = -t.getfloat();
			v.y = t.getfloat();

			uniqueVertices.push_back(v);

			std::cout << "Find vertex: " << v.x << ", " << -v.z << ", " << v.y << std::endl;
		}
		else
		{
			std::cout << "Delete word " << word << std::endl;
		}
	}
}*/

/*bool Mesh::loadBIN(const char* filename) {
	sMeshBin header;

	FILE* f = fopen(filename, "rb");
	if (f == NULL) {
		std::cout << "loadBIN()::Error al abrir archivo" << std::endl;
		return false;
	}
	fread(&header, sizeof(sMeshBin), 1, f);
	vertices.resize(header.num_vertices);
	fread(&vertices[0], sizeof(Vector3), header.num_vertices, f);
	if (header.num_normals) {
		normals.resize(header.num_normals);
		fread(&normals[0], sizeof(Vector3), header.num_normals, f);
	}
	if (header.num_uvs) {
		uvs.resize(header.num_uvs);
		fread(&uvs[0], sizeof(Vector2), header.num_uvs, f);
	}
	sBounding.center = header.c;
	sBounding.halfSize = header.hS;
	fclose(f);

	return true;
}*/

std::vector<std::string> Mesh::tokenize(const std::string& source, const char* delimiters, bool process_strings)
{
	std::vector<std::string> tokens;

	std::string str;
	char del_size = strlen(delimiters);
	const char* pos = source.c_str();
	char in_string = 0;
	int i = 0;
	while (*pos != 0)
	{
		bool split = false;

		if (!process_strings || (process_strings && in_string == 0))
		{
			for (i = 0; i < del_size && *pos != delimiters[i]; i++);
			if (i != del_size) split = true;
		}

		if (process_strings && (*pos == '\"' || *pos == '\''))
		{
			if (!str.empty() && in_string == 0) //some chars remaining
			{
				tokens.push_back(str);
				str.clear();
			}

			in_string = (in_string != 0 ? 0 : *pos);
			if (in_string == 0)
			{
				str += *pos;
				split = true;
			}
		}

		if (split)
		{
			if (!str.empty())
			{
				tokens.push_back(str);
				str.clear();
			}
		}
		else
			str += *pos;
		pos++;
	}
	if (!str.empty())
		tokens.push_back(str);
	return tokens;
}

Vector3 Mesh::parseVector3(const char* text, const char separator)
{
	int pos = 0;
	char num[255];
	const char* start = text;
	const char* current = text;
	Vector3 result;

	while (1)
	{
		if (*current == separator || (*current == '\0' && current != text))
		{
			strncpy(num, start, current - start);
			num[current - start] = '\0';
			start = current + 1;
			if (num[0] != 'x') //¿?
				switch (pos)
				{
				case 0: result.x = (float)atof(num); break;
				case 1: result.y = (float)atof(num); break;
				case 2: result.z = (float)atof(num); break;
				default: return result; break;
				}

			++pos;
			if (*current == '\0')
				break;
		}

		++current;
	}

	return result;
};

bool Mesh::loadOBJ(const char* filename, const char* path_bin)
{
	std::string bin_filename;
	bin_filename = path_bin + std::string(filename) + std::string(".bin");

	if (readBIN(bin_filename.c_str())) {
		std::cout << "Existe BIN" << std::endl;
		return true;
	}

	struct stat stbuffer;

	FILE* f = fopen(filename, "rb");
	if (f == NULL)
	{
		std::cerr << "File not found: " << filename << std::endl;
		return false;
	}

	stat(filename, &stbuffer);

	unsigned int size = stbuffer.st_size;
	char* data = new char[size + 1];
	fread(data, size, 1, f);
	fclose(f);
	data[size] = 0;

	char* pos = data;
	char line[255];
	int i = 0;

	std::vector<Vector3> indexed_positions;
	std::vector<Vector3> indexed_normals;
	std::vector<Vector2> indexed_uvs;

	const float max_float = 10000000;
	const float min_float = -10000000;

	unsigned int vertex_i = 0;

	//parse file
	while (*pos != 0)
	{
		if (*pos == '\n') pos++;
		if (*pos == '\r') pos++;

		//read one line
		i = 0;
		while (i < 255 && pos[i] != '\n' && pos[i] != '\r' && pos[i] != 0) i++;
		memcpy(line, pos, i);
		line[i] = 0;
		pos = pos + i;

		//std::cout << "Line: \"" << line << "\"" << std::endl;
		if (*line == '#' || *line == 0) continue; //comment

												  //tokenize line
		std::vector<std::string> tokens = tokenize(line, " ");

		if (tokens.empty()) continue;

		if (tokens[0] == "v" && tokens.size() == 4)
		{
			Vector3 v(atof(tokens[1].c_str()), atof(tokens[2].c_str()), atof(tokens[3].c_str()));
			indexed_positions.push_back(v);
		}
		else if (tokens[0] == "vt" && tokens.size() == 4)
		{
			Vector2 v(atof(tokens[1].c_str()), atof(tokens[2].c_str()));
			indexed_uvs.push_back(v);
		}
		else if (tokens[0] == "vn" && tokens.size() == 4)
		{
			Vector3 v(atof(tokens[1].c_str()), atof(tokens[2].c_str()), atof(tokens[3].c_str()));
			indexed_normals.push_back(v);
		}
		else if (tokens[0] == "s") //surface? it appears one time before the faces
		{
			//process mesh
			if (uvs.size() == 0 && indexed_uvs.size())
				uvs.resize(1);
		}
		else if (tokens[0] == "f" && tokens.size() >= 4)
		{
			Vector3 v1, v2, v3;
			v1 = parseVector3(tokens[1].c_str(), '/');

			for (int iPoly = 2; iPoly < tokens.size() - 1; iPoly++)
			{
				v2 = parseVector3(tokens[iPoly].c_str(), '/');
				v3 = parseVector3(tokens[iPoly + 1].c_str(), '/');

				vertices.push_back(indexed_positions[unsigned int(v1.x) - 1]);
				vertices.push_back(indexed_positions[unsigned int(v2.x) - 1]);
				vertices.push_back(indexed_positions[unsigned int(v3.x) - 1]);
				//triangles.push_back( VECTOR_INDICES_TYPE(vertex_i, vertex_i+1, vertex_i+2) ); //not needed
				vertex_i += 3;

				if (indexed_uvs.size() > 0)
				{
					uvs.push_back(indexed_uvs[unsigned int(v1.y) - 1]);
					uvs.push_back(indexed_uvs[unsigned int(v2.y) - 1]);
					uvs.push_back(indexed_uvs[unsigned int(v3.y) - 1]);
				}

				if (indexed_normals.size() > 0)
				{
					normals.push_back(indexed_normals[unsigned int(v1.z) - 1]);
					normals.push_back(indexed_normals[unsigned int(v2.z) - 1]);
					normals.push_back(indexed_normals[unsigned int(v3.z) - 1]);
				}
			}
		}
	}

	Vector3 max = Vector3(-FLT_MAX, -FLT_MAX, -FLT_MAX);
	Vector3 min = Vector3(FLT_MAX, FLT_MAX, FLT_MAX);

	for (int i = 0; i < vertices.size(); i++) {
		Vector3 v = vertices[i];
		if (v.x > max.x) { max.x = v.x; }
		if (v.y > max.y) { max.y = v.y; }
		if (v.z > max.z) { max.z = v.z; }
		if (v.x < min.x) { min.x = v.x; }
		if (v.y < min.y) { min.y = v.y; }
		if (v.z < min.z) { min.z = v.z; }
	}

	sBounding boundingBox;
	boundingBox.center = (max + min) * 0.5;
	boundingBox.half_size = max - boundingBox.center;
	std::cout << "half_size = " << boundingBox.half_size.length() << std::endl;

	writeBIN(bin_filename.c_str());
	return true;
}

bool Mesh::writeBIN(const char * pathname)
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
	
	FILE* f = fopen(pathname, "wb");

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

bool Mesh::readBIN(const char * path)
{
	sMeshBin header;
	
	//Abrimos el fichero binario
	if (FILE* f = fopen(path, "rb"))
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

bool Mesh::createCollisionModel()
{	
		collision_model = newCollisionModel3D();
		collision_model->setTriangleNumber(vertices.size() / 3);
		for (int i = 0; i < (vertices.size()/3); i += 3)

			collision_model->addTriangle(vertices[i].x, vertices[i].y, vertices[i].z,
				vertices[i + 1].x, vertices[i + 1].y, vertices[i + 1].z,
				vertices[i + 2].x, vertices[i + 2].y, vertices[i + 2].z);
		collision_model->finalize();

	return true;
}

//Tests de colisiones
bool Mesh::testIntRayMesh(Matrix44 model, Vector3 start, Vector3 front, bool closest, float min, float max)
{
	collision_model->setTransform(model.m);
	
	if (collision_model->rayCollision(start.v, front.v)) return true;
	return false;
}

bool Mesh::testIntSphereMesh(Matrix44 model, Vector3 origin, float radius)
{
	float point[3];
	point[0] = origin.x;
	point[1] = origin.y;
	point[2] = origin.z;
	collision_model->setTransform(model.m);	
	if (collision_model->sphereCollision(point, radius)) return true;

	return false;
}