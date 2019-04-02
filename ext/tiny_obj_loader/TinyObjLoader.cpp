//#####################################################################
// Tiny obj loader
// Bo Zhu
//#####################################################################
#include <iostream>
#include <memory>
#include "Common.h"
#include "Mesh.h"
#include "tiny_obj_loader.h"
#include "TinyObjLoader.h"

namespace Obj{

template<class T_MESH> void Read_From_Obj_File(const std::string& file_name,Array<std::shared_ptr<T_MESH> >& meshes)
{
	auto file = std::ifstream(file_name);

	std::string warn, err;
	tinyobj::attrib_t attrib; 
	Array<tinyobj::shape_t> shapes; 
	Array<tinyobj::material_t> materials; 
	
	if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, &file)) {
		std::cerr << "Read obj error: " << err << std::endl;
		return;
	}

	std::cout<<"Read obj file: "<<file_name<<", #shapes="<<shapes.size()<<", #materials="<<materials.size()<<std::endl;

    meshes.resize((int)shapes.size());
    for(auto i=0;i<shapes.size();i++){
		meshes[i]=std::make_shared<T_MESH>();
		tinyobj::mesh_t& mesh=shapes[i].mesh;
		 
		for (int f = 0; f < mesh.indices.size(); f++) {

			auto v0 = mesh.indices[f];

			meshes[i]->vertices->push_back(
				Vector3((real)attrib.vertices[v0.vertex_index * 3 + 0],
						(real)attrib.vertices[v0.vertex_index * 3 + 1],
						(real)attrib.vertices[v0.vertex_index * 3 + 2]));

			meshes[i]->normals->push_back(
				Vector3((real)attrib.normals[v0.normal_index * 3 + 0],
						(real)attrib.normals[v0.normal_index * 3 + 1],
						(real)attrib.normals[v0.normal_index * 3 + 2]));

			meshes[i]->uvs->push_back(
				Vector2((real)attrib.texcoords[v0.texcoord_index * 2 + 0],
						1.0 - (real)attrib.texcoords[v0.texcoord_index * 2 + 1]));

			if (f % 3 == 0) {
				meshes[i]->elements.push_back(Vector3i(f + 0, f + 1, f + 2));
			}
		}
	}
}

template void Read_From_Obj_File<TriangleMesh<3> >(const std::string&,Array<std::shared_ptr<TriangleMesh<3> > >&);

};