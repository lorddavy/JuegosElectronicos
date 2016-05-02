#include "scene.h"
#include <iostream>
#include <assert.h>
#include "extra/textparser.h"

//Source code for the Scene
Scene* Scene::instance = NULL;

Scene::Scene()
{
	assert(instance == NULL && "must be only one"); //must be only one, en release no petará
	std::cout << "Scene created" << std::endl;
	instance = this;
	root = new Entity();
}

Scene::~Scene()
{
	/*assert(instance == NULL && "must be only one"); //must be only one, en release no petará
	std::cout << "MeshManager created" << std::endl;
	instance = this;*/
}

bool Scene::createLevel(const char * filename)
{
	//Guardamos la información de cabecer
	header.format[0] = 'S';
	header.format[1] = 'C';
	header.format[2] = 'E';
	header.format[3] = 'N';
	header.format[4] = 'E';
	header.rootSize = 0;

	//Creamos el fichero binario
	std::string pathname = "data/scenes/";
	std::string bin_filename;
	bin_filename = pathname + filename + std::string(".bin");
	
	FILE* f = fopen(bin_filename.c_str(), "wb");

	if (f == NULL)
	{
		return false;
	}

	//Creamos el nivel
	skybox = new EntityMesh();
	skybox->setup("data/meshes/space_cubemap/space_cubemap.ASE", "data/meshes/space_cubemap/nebula_cubemap.tga");
	skybox->global_matrix.scale(1000, 1000, 1000);
	skybox->frustum_text = false;
	//root->addEntity(skybox);
	//header.rootSize += 1;

	planet = new EntityMesh();
	planet->setup("data/meshes/planet/sphere.ASE", "data/meshes/planet/planet_color.tga");
	planet->global_matrix.setTranslation(0, 0, -200);
	planet->global_matrix.scale(10, 10, 10);
	planet->frustum_text = false;
	planet->two_sided = true;
	root->addEntity(planet);
	header.rootSize += 1;

	//Escribimos en el fichero los datos de la escena
	fwrite(&header, sizeof(sSceneBin), 1, f);
	fwrite(&skybox, sizeof(EntityMesh*), 1, f);
	fwrite(&root, sizeof(Entity*), this->header.rootSize, f);

	//Cerramos el fichero
	fclose(f);

	return true;


}

bool Scene::loadLevel(const char* filename)
{
	//Creamos el nivel
	TextParser t;
	sSceneBin header;

	if (FILE* f = fopen(filename, "rb"))
	{

		//Leemos y guardamos cabecera
		fread(&header, sizeof(sSceneBin), 1, f);

		//Comprobamos el formato
		if (header.format[0] == 'S' && header.format[1] == 'C' && header.format[2] == 'E' && header.format[3] == 'N' && header.format[4] == 'E')
		{
			//Leemos y guardamos el contenido de skybox y root
			fread(&this->skybox, sizeof(EntityMesh*), 1, f);
			fread(&this->root, sizeof(Entity*), header.rootSize, f);

		}
		else {

			std::cout << "Archivo binario de formato desconocido" << std::endl;
			return false;
		}
		return true;
	}

	return false;
}