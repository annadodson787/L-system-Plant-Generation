//#####################################################################
// Opengl mesh
// Copyright (c) (2018-), Bo Zhu, boolzhu@gmail.com
// This file is part of SLAX, whose distribution is governed by the LICENSE file.
//#####################################################################
#ifndef __OpenGLObjectMesh_h__
#define __OpenGLObjectMesh_h__
#include <string>
#include "glm.hpp"
#include "gtc/matrix_transform.hpp"
#include "gtc/type_ptr.hpp"
#include "Mesh.h"
#include "Skeleton.h"
#include "OpenGLObject.h"
#include "OpenGLShaderProgram.h"
#include "OpenGLTexture.h"
#include "OpenGLBufferObjects.h"

const OpenGLColor default_mesh_color=OpenGLColor::Blue();

template<class T_MESH> class OpenGLMesh: public OpenGLObject
{public:typedef OpenGLObject Base;typedef T_MESH MESH_TYPE;using Base::Add_Shader_Program;
	T_MESH mesh;
	OpenGLMesh(){}

	virtual void Initialize()
	{
		Base::Initialize();
		Add_Shader_Program(OpenGLShaderLibrary::Get_Shader("vcolor"));
		Add_Shader_Program(OpenGLShaderLibrary::Get_Shader("vnormal_lt"));
	}

	virtual void Update_Data_To_Render()
	{
		if(!Update_Data_To_Render_Pre())return;

		for(auto& p:mesh.Vertices()){
			OpenGL_Vertex4_And_Color4(p,color.rgba,opengl_vertices);}		////position, 4 floats; color, 4 floats
		for(auto& e:mesh.elements)OpenGL_Vertex(e,opengl_elements);

		Set_OpenGL_Vertices();
		Set_OpenGL_Vertex_Attribute(0,4,8,0);	////position
		Set_OpenGL_Vertex_Attribute(1,4,8,4);	////color
		Set_OpenGL_Elements();

		Update_Data_To_Render_Post();
	}

	virtual void Refresh(const int frame)
	{bool is_binary_file=(File::File_Extension_Name(name)!="txt");std::string file_name=output_dir+"/"+std::to_string(frame)+"/"+name;
	if(is_binary_file){
		if(File::File_Exists(file_name)){
			mesh.elements.clear();
			File::Read_Binary_From_File(file_name,mesh);
			Set_Data_Refreshed();
			if(verbose)std::cout<<"Read file "<<file_name<<std::endl;}}
	//else{				////TOIMPL: support txt IO
	//	if(File::File_Exists(file_name)){
	//		mesh.elements.clear();
	//		File::Read_Text_From_File(file_name,mesh);
	//		Set_Data_Refreshed();
	//		if(verbose)std::cout<<"Read file "<<file_name<<std::endl;}}
	}
};

class OpenGLSegmentMesh : public OpenGLMesh<SegmentMesh<3> >
{public:typedef OpenGLMesh<SegmentMesh<3> > Base;
    OpenGLSegmentMesh(){color=default_mesh_color;name="segment_mesh";}

	virtual void Display() const
    {
		if(!visible||mesh.elements.empty())return;
		Update_Polygon_Mode();

		GLfloat old_line_width;glGetFloatv(GL_LINE_WIDTH,&old_line_width);
		{std::shared_ptr<OpenGLShaderProgram> shader=shader_programs[0];
		shader->Begin();
		glLineWidth(line_width);
		OpenGLUbos::Bind_Uniform_Block_To_Ubo(shader,"camera");
		glBindVertexArray(vao);
		glDrawElements(GL_LINES,ele_size,GL_UNSIGNED_INT,0);
		glLineWidth(old_line_width);
		shader->End();}
    }
};

class OpenGLColoredSegmentMesh : public OpenGLMesh<SegmentMesh<3> >
{public:typedef OpenGLMesh<SegmentMesh<3> > Base;
    OpenGLColoredSegmentMesh(){color=default_mesh_color;name="segment_mesh";}
	Array<real> colors;
	OpenGLColorMapper mapper;

	virtual void Initialize()
	{
		Base::Initialize();

		Array<float> v={-.2f,-.1f,0.f,.1f,.2f};
		Array<OpenGLColor> c={OpenGLColor(0,0,.5),OpenGLColor(0,0,1),OpenGLColor(0,1,1),OpenGLColor(1,1,0),OpenGLColor(1,0,0)};
		mapper.Initialize(v,c);
	}

	virtual void Update_Data_To_Render()
	{
		if(!Update_Data_To_Render_Pre())return;

		if(colors.size()==0)for(auto& e:mesh.elements){
			OpenGL_Vertex4_And_Color4(mesh.Vertices()[e[0]],color.rgba,opengl_vertices);
			OpenGL_Vertex4_And_Color4(mesh.Vertices()[e[1]],color.rgba,opengl_vertices);}
		else for(size_type i=0;i<mesh.elements.size();i++){Vector2i e=mesh.elements[i];
			OpenGLColor c=mapper.Get((float)colors[i]);
			OpenGL_Vertex4_And_Color4(mesh.Vertices()[e[0]],c.rgba,opengl_vertices);
			OpenGL_Vertex4_And_Color4(mesh.Vertices()[e[1]],c.rgba,opengl_vertices);}

		Set_OpenGL_Vertices();
		Set_OpenGL_Vertex_Attribute(0,4,8,0);	////position
		Set_OpenGL_Vertex_Attribute(1,4,8,4);	////color

		Update_Data_To_Render_Post();
	}

	virtual void Display() const
    {
		if(!visible||mesh.elements.empty())return;
		Update_Polygon_Mode();

		GLfloat old_line_width;glGetFloatv(GL_LINE_WIDTH,&old_line_width);
        
		{std::shared_ptr<OpenGLShaderProgram> shader=shader_programs[0];
		shader->Begin();
        // On MacOSX only 1.0f is a valid line width
#ifndef __APPLE__
		glLineWidth(line_width);
#endif
		OpenGLUbos::Bind_Uniform_Block_To_Ubo(shader,"camera");
		glBindVertexArray(vao);
		glDrawArrays(GL_LINES,0,vtx_size/8);
        glLineWidth(old_line_width);
		shader->End();}
    }

	virtual void Refresh(const int frame)
	{
		Base::Refresh(frame);
		std::string color_file_name=output_dir+"/"+std::to_string(frame)+"/"+name+"_color";
		if(File::File_Exists(color_file_name)){
			colors.clear();colors.resize(mesh.elements.size());
			File::Read_Binary_Array_From_File(color_file_name,&colors[0],(int)mesh.elements.size());}
	}
};

class OpenGLTriangleMesh : public OpenGLMesh<TriangleMesh<3> >
{public:typedef OpenGLMesh<TriangleMesh<3> > Base;
	std::shared_ptr<OpenGLFbos::OpenGLFbo> fbo;
	glm::mat4 shadow_pv;
	glm::mat4 model_matrix = glm::mat4(1.0f);
	
	bool use_mat=false;
	Material mat;

    OpenGLTriangleMesh(){color=default_mesh_color;name="triangle_mesh";shading_mode=ShadingMode::Lighting;}

	virtual void Set_Shading_Mode(const ShadingMode& _mode)
	{
		shading_mode=_mode;
		switch(shading_mode){
		case ShadingMode::Shadow:{use_preprocess=true;use_depth_fbo=true;}break;}
	}

	virtual void Update_Data_To_Render()
	{
		if(!Update_Data_To_Render_Pre())return;

		switch(shading_mode){
		case ShadingMode::None:
		{use_vtx_color=true;use_vtx_normal=false;use_vtx_tangent=false; use_vtx_tex =false;}break;
		case ShadingMode::Lighting:
		{use_vtx_color=false;use_vtx_normal=true; use_vtx_tangent = false; use_vtx_tex = false; }break;
		case ShadingMode::Custom:
		{use_vtx_color = true; use_vtx_normal = true; use_vtx_tangent = true; use_vtx_tex = true; }break;
		}

		if(use_vtx_normal&&(mesh.Normals().size()<mesh.Vertices().size()||recomp_vtx_normal)){
			Update_Normals(mesh,mesh.Normals());}

		if ((use_vtx_tex || use_vtx_tangent) && (mesh.Uvs().size() < mesh.Vertices().size())) {
			Update_Uvs(mesh, mesh.Uvs());}

		if (use_vtx_tangent && (mesh.Tangents().size() < mesh.Vertices().size() || recomp_vtx_tangent)) {
			Update_Tangents(mesh);}

		bool doSkinning = mesh.Weights().size() != 0;

		GLuint stride_size=4+(use_vtx_color?4:0)+(use_vtx_normal?4:0)+(use_vtx_tex?4:0)+(use_vtx_tangent?4:0) + (doSkinning ? 8 : 0);
		int i=0;for(auto& p:mesh.Vertices()){
			OpenGL_Vertex4(p,opengl_vertices);	////position, 4 floats
			if(use_vtx_color){
				OpenGL_Color4(color.rgba,opengl_vertices);}		////color, 4 floats
			if(use_vtx_normal){
				OpenGL_Vertex4(mesh.Normals()[i],opengl_vertices);}	////normal, 4 floats
			if (use_vtx_tex) {
				OpenGL_Vertex4(mesh.Uvs()[i], opengl_vertices);}	////uvs, 4 floats
			if (use_vtx_tangent) {
				OpenGL_Vertex4(mesh.Tangents()[i], opengl_vertices);}	////tangents, 4 floats
			if (doSkinning) {
				OpenGL_WeightsAndJoints(mesh.Weights()[i], mesh.Joints()[i], opengl_vertices); ////weights, 4 floats; joints, 4 ints
			}
			i++;}
		for(auto& e:mesh.elements)OpenGL_Vertex(e,opengl_elements);

		Set_OpenGL_Vertices();
		int idx=0;{Set_OpenGL_Vertex_Attribute(0,4,stride_size,0);idx++;}	////position
		if (use_vtx_color) { Set_OpenGL_Vertex_Attribute(idx,4,stride_size,idx*4);idx++;}	////color
		if (use_vtx_normal) { Set_OpenGL_Vertex_Attribute(idx, 4, stride_size, idx * 4); idx++; }	////normal
		if (use_vtx_tex) { Set_OpenGL_Vertex_Attribute(idx,4,stride_size,idx*4);idx++;}	////uvs
		if (use_vtx_tangent) { Set_OpenGL_Vertex_Attribute(idx, 4, stride_size, idx * 4); idx++; }	////tangent
		if (doSkinning) {
			Set_OpenGL_Vertex_Attribute(idx, 4, stride_size, idx * 4); idx++; // weights
			Set_OpenGL_Vertex_Attribute(idx, 4, stride_size, idx * 4); idx++; // joints
		}

		Set_OpenGL_Elements();
		Update_Data_To_Render_Post();
	}

	virtual void Display() const
    {
		using namespace OpenGLUbos;using namespace OpenGLFbos;
		if(!visible||mesh.elements.empty())return;
		Update_Polygon_Mode();

		switch(shading_mode){
		case ShadingMode::None:{
			std::shared_ptr<OpenGLShaderProgram> shader=shader_programs[0];
			shader->Begin();
			Bind_Uniform_Block_To_Ubo(shader,"camera");
			glBindVertexArray(vao);
			glDrawElements(GL_TRIANGLES,ele_size,GL_UNSIGNED_INT,0);
			shader->End();
		}break;
		case ShadingMode::Lighting:{
			std::shared_ptr<OpenGLShaderProgram> shader=shader_programs[1];
			shader->Begin();
			if(use_mat)shader->Set_Uniform_Mat(&mat);
			glEnable(GL_POLYGON_OFFSET_FILL);
			glPolygonOffset(1.f,1.f);
			Bind_Uniform_Block_To_Ubo(shader,"camera");
			Bind_Uniform_Block_To_Ubo(shader,"lights");
			glBindVertexArray(vao);
			glDrawElements(GL_TRIANGLES,ele_size,GL_UNSIGNED_INT,0);
            glDisable(GL_POLYGON_OFFSET_FILL);
			shader->End();
		}break;
		case ShadingMode::Custom: {
			if (shader_programs.size() > 2) {
				std::shared_ptr<OpenGLShaderProgram> shader = shader_programs[2];
				shader->Begin();

				for (int i = 0; i < textures.size(); i++) {
					shader->Set_Uniform(textures[i].binding_name, i);
					textures[i].texture->Bind(i);
				}

				if (use_mat)shader->Set_Uniform_Mat(&mat);
				glEnable(GL_POLYGON_OFFSET_FILL);
				glPolygonOffset(1.f, 1.f);
				Bind_Uniform_Block_To_Ubo(shader, "camera");
				Bind_Uniform_Block_To_Ubo(shader, "lights");
				
				if (mesh.Weights().size() > 0) {
					Bind_Uniform_Block_To_Ubo(shader, "skinning");
				}

				shader->Set_Uniform_Matrix4f("model", glm::value_ptr(model_matrix));
				glBindVertexArray(vao);
				glDrawElements(GL_TRIANGLES, ele_size, GL_UNSIGNED_INT, 0);
				glDisable(GL_POLYGON_OFFSET_FILL);
				shader->End();
			}
		}break;}
    }
};
#endif
