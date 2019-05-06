//#####################################################################
// Main
// Dartmouth COSC 77/177 Computer Graphics, starter code
// Contact: Bo Zhu (bo.zhu@dartmouth.edu)
//#####################################################################
#include <iostream>

#include <random>
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

template<int d> class ToolkitDriver : public Driver, public OpenGLViewer
{
	using VectorD = Vector<real, d>; using VectorDi = Vector<int, d>; using Base = Driver;
	real dt = .02;

	OpenGLTriangleMesh* opengl_tri_mesh_left	= nullptr;						//// blobs
	OpenGLTriangleMesh* opengl_tri_mesh_middle	= nullptr;						//// helmet 1
	OpenGLTriangleMesh* opengl_tri_mesh_right	= nullptr;						//// helmet 2


public:
	virtual void Initialize()
	{
		OpenGLViewer::Initialize();
	}

	////synchronize simulation data to visualization data, called in OpenGLViewer::Initialize()
	virtual void Initialize_Data()
	{
		OpenGLShaderLibrary::Instance()->Add_Shader_From_File("shaders/shared.vert", "shaders/01_phong.frag", "phong");
		OpenGLShaderLibrary::Instance()->Add_Shader_From_File("shaders/shared.vert", "shaders/02_phong_textured.frag", "phong_textured");
		OpenGLShaderLibrary::Instance()->Add_Shader_From_File("shaders/shared.vert", "shaders/03_phong_normal_mapped.frag", "phong_normal_mapped");

		Init_Blob();
		Init_Helmet();

		////set lights
		auto dir_light = OpenGLUbos::Add_Directional_Light(glm::vec3(-1.f, -1.f, -1.f));
		dir_light->dif = glm::vec4(4.0, 4.0, 5.0, 1.0) * 0.2f;

		dir_light = OpenGLUbos::Add_Directional_Light(glm::vec3(1.f, -1.f, -1.f));
		dir_light->dif = glm::vec4(4.0, 2.0, 1.0, 1.0) * 0.2f;

		OpenGLUbos::Set_Ambient(glm::vec4(.01f, .01f, .02f, 1.f));
		OpenGLUbos::Update_Lights_Ubo();
	}

	void Init_Helmet() {
		OpenGLTextureLibrary::Instance()->Add_Texture_From_File("models/Default_albedo.jpg", "helmet_albedo");
		OpenGLTextureLibrary::Instance()->Add_Texture_From_File("models/Default_AO.jpg", "helmet_ao");
		OpenGLTextureLibrary::Instance()->Add_Texture_From_File("models/Default_emissive.jpg", "helmet_emissive");
		OpenGLTextureLibrary::Instance()->Add_Texture_From_File("models/Default_metalRoughness.jpg", "helmet_metalRoughness");
		OpenGLTextureLibrary::Instance()->Add_Texture_From_File("models/Default_normal.jpg", "helmet_normal");


		Array<std::shared_ptr<TriangleMesh<3> > > meshes;
		Obj::Read_From_Obj_File("models/helmet.obj", meshes);

		{
			opengl_tri_mesh_middle = Add_Interactive_Object<OpenGLTriangleMesh>();
			opengl_tri_mesh_middle->mesh = *meshes[0];
			opengl_tri_mesh_middle->model_matrix = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0));

			Set_Polygon_Mode(opengl_tri_mesh_middle, PolygonMode::Fill);
			Set_Shading_Mode(opengl_tri_mesh_middle, ShadingMode::Custom);
			opengl_tri_mesh_middle->Set_Data_Refreshed();
			opengl_tri_mesh_middle->Initialize();
			opengl_tri_mesh_middle->Add_Shader_Program(OpenGLShaderLibrary::Get_Shader("phong_textured"));

			opengl_tri_mesh_middle->Add_Texture("albedo", OpenGLTextureLibrary::Get_Texture("helmet_albedo"));
			opengl_tri_mesh_middle->Add_Texture("ao", OpenGLTextureLibrary::Get_Texture("helmet_ao"));
			opengl_tri_mesh_middle->Add_Texture("emissive", OpenGLTextureLibrary::Get_Texture("helmet_emissive"));
			opengl_tri_mesh_middle->Add_Texture("metal_roughness", OpenGLTextureLibrary::Get_Texture("helmet_metalRoughness"));
			opengl_tri_mesh_middle->Add_Texture("normal", OpenGLTextureLibrary::Get_Texture("helmet_normal"));
		}

		{
			opengl_tri_mesh_right = Add_Interactive_Object<OpenGLTriangleMesh>();
			opengl_tri_mesh_right->mesh = *meshes[0];
			opengl_tri_mesh_right->model_matrix = glm::translate(glm::mat4(1.0f), glm::vec3(2.f, 0, 0));

			Set_Polygon_Mode(opengl_tri_mesh_right, PolygonMode::Fill);
			Set_Shading_Mode(opengl_tri_mesh_right, ShadingMode::Custom);
			opengl_tri_mesh_right->Set_Data_Refreshed();
			opengl_tri_mesh_right->Initialize();
			opengl_tri_mesh_right->Add_Shader_Program(OpenGLShaderLibrary::Get_Shader("phong_normal_mapped"));

			opengl_tri_mesh_right->Add_Texture("albedo", OpenGLTextureLibrary::Get_Texture("helmet_albedo"));
			opengl_tri_mesh_right->Add_Texture("ao", OpenGLTextureLibrary::Get_Texture("helmet_ao"));
			opengl_tri_mesh_right->Add_Texture("emissive", OpenGLTextureLibrary::Get_Texture("helmet_emissive"));
			opengl_tri_mesh_right->Add_Texture("metal_roughness", OpenGLTextureLibrary::Get_Texture("helmet_metalRoughness"));
			opengl_tri_mesh_right->Add_Texture("normal", OpenGLTextureLibrary::Get_Texture("helmet_normal"));
		}
	}

	void Init_Blob() {
		Array<std::shared_ptr<TriangleMesh<3> > > meshes;
		Obj::Read_From_Obj_File("models/blobs.obj", meshes);

		{
			opengl_tri_mesh_middle = Add_Interactive_Object<OpenGLTriangleMesh>();
			opengl_tri_mesh_middle->mesh = *meshes[0];
			opengl_tri_mesh_middle->model_matrix = glm::translate(glm::mat4(1.0f), glm::vec3(-3, 0, 0));

			Set_Polygon_Mode(opengl_tri_mesh_middle, PolygonMode::Fill);
			Set_Shading_Mode(opengl_tri_mesh_middle, ShadingMode::Custom);
			opengl_tri_mesh_middle->Set_Data_Refreshed();
			opengl_tri_mesh_middle->Initialize();
			opengl_tri_mesh_middle->Add_Shader_Program(OpenGLShaderLibrary::Get_Shader("phong"));
		}
	}

	void Sync_Simulation_And_Visualization_Data()
	{
	}

	////update simulation and visualization for each time step
	virtual void Toggle_Next_Frame()
	{
		Sync_Simulation_And_Visualization_Data();
		OpenGLViewer::Toggle_Next_Frame();
	}

	virtual void Run()
	{
		OpenGLViewer::Run();
	}

	////Keyboard interaction
	virtual void Initialize_Common_Callback_Keys()
	{
		OpenGLViewer::Initialize_Common_Callback_Keys();
	}

protected:
	////Helper function to convert a vector to 3d, for c++ template
	Vector3 V3(const Vector2& v2) { return Vector3(v2[0], v2[1], .0); }
	Vector3 V3(const Vector3& v3) { return v3; }
};


int main(int argc,char* argv[])
{
	int driver=1;

	switch(driver){
	case 1:{
		ToolkitDriver<3> driver;
		driver.Initialize();
		driver.Run();	
	}break;
	}
	
}

