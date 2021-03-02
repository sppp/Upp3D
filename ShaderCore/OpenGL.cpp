#include "ShaderCore.h"

NAMESPACE_SHADER_BEGIN


void MultiStage::Ogl_UpdateTexBuffers() {
	bool set_output = true;
	for (int i = passes.GetCount()-1; i >= 0; i--) {
		Stage& pass = passes[i];
		pass.ClearTex();
		if (pass.is_common)
			continue;
		
		if (set_output) {
			set_output = false;
			pass.is_buffer = false;
		}
		else {
			pass.is_buffer = true;
			int buf_count = 1;
			if (pass.is_doublebuf)
				buf_count++;
			for(int bi = 0; bi < buf_count; bi++) {
				GLuint& color_buf = pass.color_buf[bi];
				GLuint& depth_buf = pass.depth_buf[bi];
				GLuint& frame_buf = pass.frame_buf[bi];
				ASSERT(color_buf == 0);
				
				// color buffer
				glGenTextures(1, &color_buf);
				glBindTexture(GL_TEXTURE_2D, color_buf);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, size.cx, size.cy, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glBindTexture(GL_TEXTURE_2D, 0);
				
				// depth buffer
				glGenRenderbuffersEXT(1, &depth_buf);
				glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, depth_buf);
				glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT, size.cx, size.cy);
				glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, 0);
				
				// FBO
				glGenFramebuffersEXT(1, &frame_buf);
				glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, frame_buf);
				
				// combine FBO to color buffer
				glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, color_buf, 0);
				
				// combine FBO to depth buffer
				glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, depth_buf);
				
				// reset FBO
				glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
			}
		}
	}
	
}




int MultiStage::Ogl_LoadTexture(String filename, GLenum type, GLenum *tex_id, char filter, char repeat, bool flip) {
	Image img = StreamRaster::LoadFileAny(filename);
	if (!img) {
		LOG("error: couldn't load file " << filename);
		return 0;
	}
	
	int width = img.GetWidth();
	int height = img.GetHeight();
	if (!width || !height) {
		LOG("error: empty image " << filename);
		return 0;
	}
	
	glGenTextures(1, tex_id);
	glBindTexture(type, *tex_id);
	
	int channels = 0;
	

	// Float or byte based image transfer. Both work.
	// Byte-based might be slightly less resource-demanding.
	if (0) {
		FloatImage fimg = img;
		if (flip)
			fimg.FlipVert();
		channels = fimg.GetChannels();
		ASSERT(channels >= 3 && channels <= 4);
		ASSERT(fimg.GetDataSize() == width * height * channels);
		if (type == GL_TEXTURE_2D) {
			glTexImage2D(type, 0, GL_RGBA32F,
						 width, height,
						 0, channels == 3 ? GL_RGB : GL_RGBA,
						 GL_FLOAT,
						 fimg.Detach());
		}
		else if (type == GL_TEXTURE_3D) {
			glTexImage3D(type, 0, GL_RGBA32F,
						 width, height, 1,
						 0, channels == 3 ? GL_RGB : GL_RGBA,
						 GL_FLOAT,
						 fimg.Detach());
		}
	}
	else {
		ByteImage bimg = img;
		if (flip)
			bimg.FlipVert();
		channels = bimg.GetChannels();
		int sz = bimg.GetSize();
		int exp_sz = width * height * channels;
		ASSERT(channels >= 3 && channels <= 4);
		ASSERT(bimg.GetWidth() == width);
		ASSERT(bimg.GetHeight() == height);
		ASSERT(sz == exp_sz);
		if (type == GL_TEXTURE_2D) {
			glTexImage2D(type, 0, GL_RGBA,
						 width, height,
						 0, channels == 3 ? GL_RGB : GL_RGBA,
						 GL_UNSIGNED_BYTE,
						 bimg.Detach());
		}
		else if (type == GL_TEXTURE_3D) {
			glTexImage3D(type, 0, GL_RGBA,
						 width, height, 1,
						 0, channels == 3 ? GL_RGB : GL_RGBA,
						 GL_UNSIGNED_BYTE,
						 bimg.Detach());
		}
	}
	
	if (filter == 0) {
		glTexParameteri(type, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(type, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	}
	else {
		glTexParameteri(type, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(type, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glGenerateMipmap(type);
	}
	
	if (repeat) {
		glTexParameteri(type, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(type, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}
	else {
		glTexParameteri(type, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(type, GL_TEXTURE_WRAP_T, GL_CLAMP);
	}
	
	LOG("texture: " << filename << ", " << width << "x" << height << " (" << channels << ") --> id " << *tex_id << "\n");
	
	return 1;
}


int MultiStage::Ogl_LoadCubemap(String filename, GLenum *tex_id, char filter, char repeat, bool flip) {
	String dir = GetFileDirectory(filename);
	String title = GetFileTitle(filename);
	String ext = GetFileExt(filename);
	
	glGenTextures(1, tex_id);
	glBindTexture(GL_TEXTURE_CUBE_MAP, *tex_id);
	
	int channels = 0;
	
	for(int i = 0; i < 6; i++) {
		String path;
		if (!i)
			path = filename;
		else
			path = AppendFileName(dir, title + "_" + IntStr(i) + ext);
		
		Image img = StreamRaster::LoadFileAny(path);
		if (!img) {
			LOG("error: couldn't load file " << path);
			return 0;
		}
		
		int width = img.GetWidth();
		int height = img.GetHeight();
		if (!width || !height) {
			LOG("error: empty image " << path);
			return 0;
		}
		
		// Float or byte based image transfer. Both work.
		// Byte-based might be slightly less resource-demanding.
		if (0) {
			FloatImage fimg = img;
			//if (!flip) fimg.FlipVert();
			channels = fimg.GetChannels();
			ASSERT(channels >= 3 && channels <= 4);
			ASSERT(fimg.GetDataSize() == width * height * channels);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA32F,
						 width, height,
						 0, channels == 3 ? GL_RGB : GL_RGBA,
						 GL_FLOAT,
						 fimg.Detach());
		}
		else {
			ByteImage bimg = img;
			//if (!flip) bimg.FlipVert();
			channels = bimg.GetChannels();
			int sz = bimg.GetSize();
			int exp_sz = width * height * channels;
			ASSERT(channels >= 3 && channels <= 4);
			ASSERT(bimg.GetWidth() == width);
			ASSERT(bimg.GetHeight() == height);
			ASSERT(sz == exp_sz);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA,
						 width, height,
						 0, channels == 3 ? GL_RGB : GL_RGBA,
						 GL_UNSIGNED_BYTE,
						 bimg.Detach());
		}
		
		
		LOG("texture #" << i << ": " << path << ", " << width << "x" << height << " (" << channels << ") --> id " << *tex_id);
	}
	
	if (filter == StageInput::FILTER_NEAREST) {
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	}
	else if (filter == StageInput::FILTER_LINEAR) {
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	}
	else if (filter == StageInput::FILTER_MIPMAP) {
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
	}
	
	if (repeat == StageInput::REPEAT_REPEAT) {
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}
	else if (repeat == StageInput::REPEAT_CLAMP) {
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP);
	}
	
	return 1;
}


GLint MultiStage::Ogl_CompileShader(const GLenum shader_type, String shader_source) {
	GLuint shader = glCreateShader(shader_type);
	GLint status = GL_FALSE;
	GLint loglen;
	GLchar *error_message;
	if (shader == 0)
		return -1;
	
	const GLchar* src = shader_source.Begin();
	int len = shader_source.GetCount();
	glShaderSource(shader, 1, &src, &len);
	glCompileShader(shader);
	
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if (status != GL_TRUE) {
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &loglen);
		Vector<GLchar> msg;
		msg.SetCount(loglen);
		glGetShaderInfoLog(shader, loglen, NULL, msg.Begin());
		last_error = "shader failed to compile: " + String(msg.Begin());
		return -1;
	}
	
	return shader;
}


bool MultiStage::Ogl_CompileProgram(Stage& s, String shader_source) {
	GLint frag, program;
	
	frag = Ogl_CompileShader(GL_FRAGMENT_SHADER, shader_source);
	if (frag < 0)
		return false;
		
	program = glCreateProgram();
	glProgramParameteri(program, GL_PROGRAM_SEPARABLE, GL_TRUE);
	
	glAttachShader(program, frag);
	glDeleteShader (frag);
	
	s.prog[Stage::PROG_FRAGMENT] = program;
	
	return true;
}

bool MultiStage::Ogl_LinkProgram(Stage& s) {
	GLint program = s.prog[Stage::PROG_FRAGMENT];
	GLint status = GL_FALSE;
	GLint loglen, n_uniforms;
	
	glLinkProgram(program);
	
	glGetProgramiv(program, GL_LINK_STATUS, &status);
	if (status != GL_TRUE) {
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &loglen);
		Vector<GLchar> msg;
		msg.SetCount(loglen);
		glGetProgramInfoLog(program, loglen, NULL, msg.Begin());
		last_error = msg.Begin();
		return false;
	}
	
	// diagnostics
	glGetProgramiv(program, GL_ACTIVE_UNIFORMS, &n_uniforms);
	LOG("\t\t" << (int)n_uniforms << " uniforms:");
	
	GLchar name[80];
	GLsizei namelen;
	for (int i = 0; i < n_uniforms; i++) {
		GLint size;
		GLenum type;
		
		glGetActiveUniform(program, i, 79, &namelen, &size, &type, name);
		name[namelen] = '\0';
		LOG("\t\t\t" << i << ": " << String(name) << " (type: " << HexStr(type) << ", size: " << (int)size << ")");
	}
	
	return true;
}



NAMESPACE_SHADER_END
