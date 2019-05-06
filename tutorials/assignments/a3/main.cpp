//#####################################################################
// Main
// Dartmouth COSC 77/177 Computer Graphics, starter code
// Contact: Bo Zhu (bo.zhu@dartmouth.edu)
//#####################################################################
#include <iostream>
#include "LinearBlendSkinning.h"

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
#include "TinyGltfLoader.h"
#include "SceneGraph.h"

template<int d> class ToolkitDriver : public Driver, public OpenGLViewer
{
	using VectorD=Vector<real, d>; using VectorDi=Vector<int, d>; using Base=Driver;
	real dt = 1.0 / 60;

	OpenGLTriangleMesh* opengl_tri_mesh = nullptr;						////
	OpenGLSegmentMesh* rig_vis = nullptr;

	std::shared_ptr < SceneGraph<3>> scenegraph;
	Array<std::shared_ptr<Skeleton<3> > > skeletons;

	Array<glm::vec4> original_vertices;
	Array<glm::vec4> current_vertices;

	double x_pos = -2.0f;
	double x_vel = 1.0f;
	float time = 0;

public:
	virtual void Initialize()
	{
		OpenGLViewer::Initialize();
	}

	// synchronize simulation data to visualization data, called in OpenGLViewer::Initialize()
	virtual void Initialize_Data()
	{
		OpenGLShaderLibrary::Instance()->Add_Shader_From_File("shaders/skinned.vert", "shaders/diffuse.frag", "diffuse");

		OpenGLTextureLibrary::Instance()->Add_Texture_From_File("models/CesiumMan.jpg", "albedo");

		Array<std::shared_ptr<TriangleMesh<3> > > meshes;
		Gltf::Read_From_Gltf_File("models/CesiumMan.glb", scenegraph, meshes, skeletons);

		std::transform(std::begin(meshes[0]->Vertices()), std::end(meshes[0]->Vertices()), std::back_inserter(original_vertices), [](Vector3 v) {
			return glm::vec4(v.x(), v.y(), v.z(), 1.0f);
		});

		current_vertices = original_vertices;

		// initialize meshes
		{
			opengl_tri_mesh = Add_Interactive_Object<OpenGLTriangleMesh>();
			opengl_tri_mesh->mesh = *meshes[0];
			opengl_tri_mesh->model_matrix = glm::translate(glm::mat4(1.0f), glm::vec3(x_pos, 0, 0));

			Set_Polygon_Mode(opengl_tri_mesh, PolygonMode::Fill);
			Set_Shading_Mode(opengl_tri_mesh, ShadingMode::Custom);
			opengl_tri_mesh->Set_Data_Refreshed();
			opengl_tri_mesh->Initialize();
			opengl_tri_mesh->Add_Shader_Program(OpenGLShaderLibrary::Get_Shader("diffuse"));

			opengl_tri_mesh->Add_Texture("albedo", OpenGLTextureLibrary::Get_Texture("albedo"));
		}

		// Create segment mesh for skeleton
		if (skeletons.size() > 0) {

			rig_vis = Add_Interactive_Object<OpenGLSegmentMesh>();
			rig_vis->mesh.Vertices().resize(scenegraph->nodes.size());

			for (int i = 0; i < scenegraph->nodes.size(); i++) {
				for (int c : scenegraph->nodes[i].children_indices) {
					rig_vis->mesh.Elements().push_back({ i, c });
				}
			}

			rig_vis->Set_Data_Refreshed();
			rig_vis->Initialize();
		}


		// set lights
		auto dir_light = OpenGLUbos::Add_Directional_Light(glm::vec3(-1.f, -1.f, -1.f));
		dir_light->dif = glm::vec4(4.0, 4.0, 5.0, 1.0);

		dir_light = OpenGLUbos::Add_Directional_Light(glm::vec3(1.f, -1.f, -1.f));
		dir_light->dif = glm::vec4(4.0, 2.0, 1.0, 1.0);

		OpenGLUbos::Set_Ambient(glm::vec4(.1f, .1f, .1f, 1.f));
		OpenGLUbos::Update_Lights_Ubo();

		Sync_Simulation_And_Visualization_Data();
	}

	void Evaluate_Animation(
		std::shared_ptr<SceneGraph<3>> & scenegraph,
		const SceneGraph<3>::Animation & animation,
		float current_time) {

		for (auto& channel : animation.channels) {

			current_time = fmod(current_time, channel.times[channel.times.size() - 1]);

			auto one_past_it = std::lower_bound(std::begin(channel.times), std::end(channel.times), current_time);


			int next_frame_index = (int)(one_past_it - std::begin(channel.times));
			int prev_frame_index = next_frame_index - 1;

			next_frame_index = glm::clamp(next_frame_index, 0, (int)channel.times.size() - 1);
			prev_frame_index = glm::clamp(prev_frame_index, 0, (int)channel.times.size() - 1);

			glm::vec4 frame_val;
			if (prev_frame_index != next_frame_index) {
				float between = (current_time - channel.times[prev_frame_index]) / (channel.times[next_frame_index] - channel.times[prev_frame_index]);
				glm::vec4 prev_val = channel.frames[prev_frame_index];
				glm::vec4 next_val = channel.frames[next_frame_index];

				frame_val = glm::mix(prev_val, next_val, between);
			}
			else {
				frame_val = channel.frames[next_frame_index];
			}

			switch (channel.target) {
			case SceneGraph<3>::AnimTarget::pos:
			{
				scenegraph->nodes[channel.node_index].pos = glm::vec3(frame_val);
				break;
			}
			case SceneGraph<3>::AnimTarget::scl:
			{
				scenegraph->nodes[channel.node_index].scl = glm::vec3(frame_val);
				break;
			}
			case SceneGraph<3>::AnimTarget::rot:
			{
				scenegraph->nodes[channel.node_index].rot = glm::normalize(glm::quat(frame_val.x, frame_val.y, frame_val.z, frame_val.w));
				break;
			}
			}
		}
	}

	void Sync_Simulation_And_Visualization_Data() {

		// Move the character forwards
		time += (float)dt;
		x_pos = fmod(time * x_vel, 4.0f) - 2.0f;
		opengl_tri_mesh->model_matrix = glm::translate(glm::mat4(1.0f), glm::vec3(x_pos, 0, 0));

		// Play back animation
		if (scenegraph->animations.size() > 0) {
			Evaluate_Animation(scenegraph, scenegraph->animations[0], time);
		}

		// Update world transform of nodes
		Update_Nodes_Recursive(scenegraph.get(), 0, glm::mat4(1));

		// Update skinning matrices
		for (auto& skeleton : skeletons) {
			Update_Skeleton(scenegraph.get(), skeleton.get());
		}
		
		// Compute skinned vertex positions
		SkinVertices(
			original_vertices, 
			opengl_tri_mesh->mesh.Weights(), 
			opengl_tri_mesh->mesh.Joints(), 
			skeletons[0]->skinning_matrices,
			current_vertices
		);

		// Update rig visualization
		for (int i = 0; i < scenegraph->nodes.size(); i++) {
			auto pos = opengl_tri_mesh->model_matrix * scenegraph->node_world_transforms[i] * glm::vec4(0, 0, 0, 1);
			rig_vis->mesh.Vertices()[i] = { pos.x, pos.y, pos.z };
		}
		rig_vis->Set_Data_Refreshed();

		// Update mesh vertices
		for (int i = 0; i < opengl_tri_mesh->mesh.Vertices().size(); i++) {
			opengl_tri_mesh->mesh.Vertices()[i] = { current_vertices[i].x, current_vertices[i].y, current_vertices[i].z };
		}
		opengl_tri_mesh->Set_Data_Refreshed();

	}

	// update simulation and visualization for each time step
	virtual void Toggle_Next_Frame()
	{
		Sync_Simulation_And_Visualization_Data();
		OpenGLViewer::Toggle_Next_Frame();
	}

	virtual void Run()
	{
		OpenGLViewer::Run();
	}

	// Keyboard interaction
	virtual void Initialize_Common_Callback_Keys()
	{
		OpenGLViewer::Initialize_Common_Callback_Keys();
	}
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