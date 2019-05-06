//#####################################################################
// Main
// Dartmouth COSC 77/177 Computer Graphics, technical project starter code
//#####################################################################
#include <iostream>

#include "Common.h"
#include "Driver.h"
#include "Particles.h"
#include "OpenGLMesh.h"
#include "OpenGLCommon.h"
#include "OpenGLWindow.h"
#include "OpenGLViewer.h"
#include "OpenGLMarkerObjects.h"
#include "OpenGLParticles.h"
#include "TinyObjLoader.h"

class TechProjDriver : public Driver, public OpenGLViewer
{
	using Base = Driver;
public:
	Array<TriangleMesh<3>* > triangle_meshes;

	virtual void Initialize()
	{
		OpenGLViewer::Initialize();
	}

	virtual void Run()
	{
		OpenGLViewer::Run();
	}

	////Initialize data in the scene
	virtual void Initialize_Data()
	{
		Init_Shaders();
		Init_Textures();
		Init_Background();
		Init_Bunny_Mesh();
		Init_Plane_Mesh();
		Init_Lighting();
	}

	//////////////////////////////////////////////////////////////////////////
	////Specifying your own lighting effects in this function
	////In this example we initialized two directional lights, the same as Assignment 2
	void Init_Lighting() {
		auto dir_light = OpenGLUbos::Add_Directional_Light(glm::vec3(-1.f, -1.f, -1.f));
		dir_light->dif = glm::vec4(.9,.8,.7, 1.0);

		dir_light = OpenGLUbos::Add_Directional_Light(glm::vec3(1.f, -1.f, -1.f));
		dir_light->dif = glm::vec4(.7,.7,.7, 1.0);

		OpenGLUbos::Set_Ambient(glm::vec4(.01f, .01f, .02f, 1.f));
		OpenGLUbos::Update_Lights_Ubo();	
	}

	//////////////////////////////////////////////////////////////////////////
	////Write your own vertex shader and fragment shader and add it to the shader library
	void Init_Shaders()
	{
		Add_Shader("shaders/background.vert","shaders/background.frag","background");
		Add_Shader("shaders/shared.vert", "shaders/lamb_textured.frag", "lamb_tex");
		Add_Shader("shaders/shared.vert", "shaders/lamb.frag", "lamb");
	}

	//////////////////////////////////////////////////////////////////////////
	////Read a texture from a image and add it to the texture library
	void Init_Textures()
	{
		Add_Texture("models/bunny.jpg","bunny");
	}

	//////////////////////////////////////////////////////////////////////////
	////Initialize the background shader
	void Init_Background()
	{
		OpenGLBackground* opengl_background=Add_Interactive_Object<OpenGLBackground>();
		opengl_background->shader_name="background";
		opengl_background->Initialize();
	}

	//////////////////////////////////////////////////////////////////////////
	////Init a mesh and bind shader and texture
	////To make your own mesh, copy this function and modify the variables
	void Init_Bunny_Mesh() 
	{
		////Initialize the mesh file, shader, and texture of the mesh
		std::string mesh_file_name = "models/bunny.obj";
		std::string shader_name = "lamb_tex";
		std::string texture_name = "bunny";

		////Read mesh from obj file
		OpenGLTriangleMesh* opengl_tri_mesh = Add_Interactive_Object<OpenGLTriangleMesh>();
		Read_Mesh(mesh_file_name, opengl_tri_mesh->mesh);
		Rescale(opengl_tri_mesh->mesh.Vertices(),1.);
		Translate_Center_To(opengl_tri_mesh->mesh.Vertices(),Vector3::Ones()*.5);

		////Initialize the model matrix
		opengl_tri_mesh->model_matrix = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0));

		////Other mesh initialization setups
		Set_Mesh_Default_Options(opengl_tri_mesh);

		////Bind an initialized shader to the mesh
		opengl_tri_mesh->Add_Shader_Program(OpenGLShaderLibrary::Get_Shader(shader_name));

		////Bind a texture to the mesh
		opengl_tri_mesh->Add_Texture("tex", OpenGLTextureLibrary::Get_Texture(texture_name));

		////Add the triangle mesh to the array to access the mesh later
		TriangleMesh<3>* triangle_mesh=&opengl_tri_mesh->mesh;
		triangle_meshes.push_back(triangle_mesh);
	}

	void Init_Plane_Mesh()
	{
		////Initialize the mesh file, shader, and texture of the mesh
		std::string shader_name = "lamb";

		////Read mesh from obj file
		OpenGLTriangleMesh* opengl_tri_mesh = Add_Interactive_Object<OpenGLTriangleMesh>();
		////Create a mesh with vertices on a 5x5 lattice
		Create_Plane_Mesh(5,5,1.,&opengl_tri_mesh->mesh,0,2);
		Translate_Center_To(opengl_tri_mesh->mesh.Vertices(),Vector3::Zero());

		////Initialize the model matrix
		opengl_tri_mesh->model_matrix = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0));

		////Other mesh initialization setups
		Set_Mesh_Default_Options(opengl_tri_mesh);

		////Bind an initialized shader to the mesh
		opengl_tri_mesh->Add_Shader_Program(OpenGLShaderLibrary::Get_Shader(shader_name));

		////Add the triangle mesh to the array to access the mesh later
		TriangleMesh<3>* triangle_mesh=&opengl_tri_mesh->mesh;
		triangle_meshes.push_back(triangle_mesh);		
	}

	void Sync_Simulation_And_Visualization_Data()
	{
		////if you want to animate your meshes or objects, implement this function
	}

	////update simulation and visualization for each time step
	virtual void Toggle_Next_Frame()
	{
		Sync_Simulation_And_Visualization_Data();
		OpenGLViewer::Toggle_Next_Frame();
	}

	////Keyboard interaction
	virtual void Initialize_Common_Callback_Keys()
	{
		OpenGLViewer::Initialize_Common_Callback_Keys();
		Bind_Callback_Key('a',&Keyboard_Event_A_Func,"press A");
	}

	////Implement your own keyboard function, 
	virtual void Keyboard_Event_A()
	{
		std::cout<<"A pressed"<<std::endl;
	}
	Define_Function_Object(TechProjDriver,Keyboard_Event_A);

protected:
	void Set_Mesh_Default_Options(OpenGLTriangleMesh* opengl_tri_mesh)
	{
		Set_Polygon_Mode(opengl_tri_mesh, PolygonMode::Fill);
		Set_Shading_Mode(opengl_tri_mesh, ShadingMode::Custom);
		opengl_tri_mesh->Set_Data_Refreshed();
		opengl_tri_mesh->Initialize();
	}

	//////////////////////////////////////////////////////////////////////////
	////Add a shader to the shader library
	void Add_Shader(const std::string vtx_shader_file_name,const std::string frag_shader_file_name,const std::string shader_name)
	{
		OpenGLShaderLibrary::Instance()->Add_Shader_From_File(vtx_shader_file_name,frag_shader_file_name,shader_name);
	}

	//////////////////////////////////////////////////////////////////////////
	////Add a texture to the texture library
	void Add_Texture(const std::string texture_file_name,const std::string texture_name)
	{
		OpenGLTextureLibrary::Instance()->Add_Texture_From_File(texture_file_name, texture_name);
	}

	//////////////////////////////////////////////////////////////////////////
	////Read an obj mesh
	void Read_Mesh(const std::string mesh_file_name,TriangleMesh<3>& read_mesh)
	{
		Array<std::shared_ptr<TriangleMesh<3> > > meshes;
		Obj::Read_From_Obj_File(mesh_file_name, meshes);
		read_mesh=*meshes[0];
	}

	//////////////////////////////////////////////////////////////////////////
	////Initialize a plane mesh
	void Create_Plane_Mesh(const int m,const int n,const real dx,TriangleMesh<3>* mesh,int axis_0=0,int axis_1=1)
	{
		mesh->elements.resize(2*(m-1)*(n-1));int t=0;
		for(int i=1;i<=m-1;i++)for(int j=1;j<=n-1;j++){ // counterclockwise node ordering
			if(i%2){mesh->elements[t++]=Vector3i(i+m*(j-1),i+1+m*(j-1),i+m*j);mesh->elements[t++]=Vector3i(i+1+m*(j-1),i+1+m*j,i+m*j);}
			else{mesh->elements[t++]=Vector3i(i+m*(j-1),i+1+m*(j-1),i+1+m*j);mesh->elements[t++]=Vector3i(i+m*(j-1),i+1+m*j,i+m*j);}}
		for(size_type i=0;i<mesh->elements.size();i++){mesh->elements[i]-=Vector3i::Ones();
		/*swap y and z*/int tmp=mesh->elements[i][1];mesh->elements[i][1]=mesh->elements[i][2];mesh->elements[i][2]=tmp;}
		for(int j=0;j<n;j++)for(int i=0;i<m;i++){Vector3 pos=Vector3::Zero();pos[axis_0]=(real)i*dx;pos[axis_1]=(real)j*dx;mesh->Vertices().push_back(pos);}
	}
	

	//////////////////////////////////////////////////////////////////////////
	////Calculate the bounding box of a set of vertices
	void Bounding_Box(const Array<Vector3>& vertices,Vector3& min_corner,Vector3& max_corner)
	{
		min_corner=Vector3::Ones()*(1e7);max_corner=Vector3::Ones()*(-1e7);
		for(auto& v:vertices){
			min_corner=min_corner.cwiseMin(v);
			max_corner=max_corner.cwiseMax(v);}
	}

	//////////////////////////////////////////////////////////////////////////
	////Rescale the points to a box with longest_length
	void Rescale(Array<Vector3>& vertices,const real longest_length)
	{
		Vector3 min_corner,max_corner;
		Bounding_Box(vertices,min_corner,max_corner);
		Vector3 length=max_corner-min_corner;
		int axis=0;if(length[1]>length[axis])axis=1;if(length[2]>length[axis])axis=2;
		real rescale=(length[axis]>(real)0)?longest_length/length[axis]:(real)1;for(auto& v:vertices)v*=rescale;
	}

	//////////////////////////////////////////////////////////////////////////
	////Calculate the center
	Vector3 Center(const Array<Vector3>& vertices)
	{
		Vector3 sum=Vector3::Zero();for(auto& v:vertices)sum+=v;
		return sum/=(real)vertices.size();
	}

	//////////////////////////////////////////////////////////////////////////
	////Translate the center to the target position
	void Translate_Center_To(Array<Vector3>& vertices,const Vector3 target)
	{
		Vector3 center=Center(vertices);
		for(auto& v:vertices)v+=(target-center);
	}
};

int main(int argc,char* argv[])
{
	int driver=1;

	switch(driver){
	case 1:{
		TechProjDriver driver;
		driver.Initialize();
		driver.Run();	
	}break;
	}
}

