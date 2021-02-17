#ifdef flagMAIN
#include "ShaderCore.h"


USING_NAMESPACE_GLSL;


void SimpleDiffuseVertex(
	const vec4& position,
	const vec4& normal,
	const mat4& model_view_proj,
	const mat4& inverse_transpose_view_proj,
	vec3& world_normal,
	vec4& gl_pos
) {
	vec4 worldPosition = model_view_proj * vec4(position.xyz(), 1.0);
	world_normal = (inverse_transpose_view_proj * vec4(normal.xyz(), 0.0)).xyz();
	Float d = distance(worldPosition, vec4(world_normal, 0.0));
	gl_pos = vec4(worldPosition.xyz(), 1.0);
}

void SimpleDiffuseFragment
(
	const vec3& diffuse_color,
	const vec3& light_dir,
	const vec3& world_normal,
	vec4& color_out
) {
	Float d = dot(world_normal, light_dir);
	color_out = vec4(diffuse_color * d, 1.0);
}

CONSOLE_APP_MAIN {
	Sppp::SetCoutLog();
	
	
	Program program(GLSLVersion::GLSL440);
	program.EnableExtension(GL_ARB_explicit_attrib_location);
	
	// vertex shader definitions
	vec4 position(VertexIn, 0);
	vec4 normal(VertexIn, 1);
	
	mat4 model_view_proj(VertexUniform, "ModelViewProjection");
	mat4 inverse_transpose_view_proj(VertexUniform, "InverseTransposeViewProjection");
	
	vec3 world_normal(VertexOut);
	vec4 gl_pos(VertexOut, "gl_Position");
	
	// vertex main()
	program.SetVertexShader([&]() {
		SimpleDiffuseVertex(position, normal, model_view_proj, inverse_transpose_view_proj, world_normal, gl_pos);
	}
						   );
	                       
	// fragment shader definitions
	vec4 color_out(FragmentOut, "ColorOut");
	
	vec3 diffuse_color(FragmentUniform, "DiffuseColor");
	vec3 light_dir(FragmentUniform, "LightDirection");
	
	// fragment main()
	program.SetFragmentShader([&]() {
		SimpleDiffuseFragment(diffuse_color, light_dir, world_normal, color_out);
	}
							 );
	                         
	// print result
	LOG("Vertex source: " << program.GetVertexSource());
	LOG("Fragment source: " << program.GetFragmentSource());
}

#endif
