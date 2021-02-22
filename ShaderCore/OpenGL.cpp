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
			ASSERT(pass.color_buf == 0);
			
			// color buffer
			glGenTextures(1, &pass.color_buf);
			glBindTexture(GL_TEXTURE_2D, pass.color_buf);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, size.cx, size.cy, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glBindTexture(GL_TEXTURE_2D, 0);
			
			// depth buffer
			glGenRenderbuffersEXT(1, &pass.depth_buf);
			glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, pass.depth_buf);
			glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT, size.cx, size.cy);
			glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, 0);
			
			// FBO
			glGenFramebuffersEXT(1, &pass.frame_buf);
			glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, pass.frame_buf);
			
			// combine FBO to color buffer
			glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, pass.color_buf, 0);
			
			// combine FBO to depth buffer
			glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, pass.depth_buf);
			
			// reset FBO
			glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
		}
	}
	
	for (Stage& s : passes) {
		for (StageInput& in : s.in) {
			if (in.type == INPUT_BUFFER) {
				Stage& is = GetStageById(in.id);
				in.tex = is.color_buf;
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
	
	FloatImage fimg = img;
	
	
	/*
	int rowstride;
	int cpp, bps;
	int x, y, c;
	
	int pitch = fimg.GetPitch();
	int bps = fimg.GetChannels();
	bps = gdk_pixbuf_get_bits_per_sample(pixbuf);
	cpp = gdk_pixbuf_get_n_channels(pixbuf);
	
	if (bps != 8 && bps != 16) {
		fprintf(stderr, "unexpected bits per sample: %d (%s)\n", bps, filename.c_str());
		return 0;
	}
	
	if (cpp != 3 && cpp != 4) {
		fprintf(stderr, "unexpected n_channels: %d\n", cpp);
		return 0;
	}
	
	tex_data = (GLfloat*)malloc(width * height * cpp * sizeof(GLfloat));
	for (y = 0; y < height; y++) {
		int y2 = flip ? (height - 1 - y) : y;
		uint8_t  *cur_row8  = (uint8_t *)(data + y2 * pitch);
		uint16_t *cur_row16 = (uint16_t *)(data + y2 * pitch);
		
		for (x = 0; x < width; x++) {
			for (c = 0; c < cpp; c++) {
				if (bps == 8)
					tex_data[(y * width + x) * cpp + c] = ((GLfloat) cur_row8[x * cpp + c]) / 255.0;
				else
					tex_data[(y * width + x) * cpp + c] = ((GLfloat) cur_row16[x * cpp + c]) / 65535.0;
			}
		}
	}*/
	
	ASSERT(fimg.GetChannels() >= 3);
	ASSERT(fimg.GetSize() == width * height * fimg.GetChannels());
	
	glGenTextures(1, tex_id);
	glBindTexture(type, *tex_id);
	glTexImage2D(type, 0, GL_RGBA8,
				 width, height,
				 0, fimg.GetChannels() == 3 ? GL_RGB32F : GL_RGBA32F,
				 GL_FLOAT,
				 fimg.Detach());
	
	if (filter == 0) {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	}
	else {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	
	if (repeat) {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}
	else {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	}
	
	LOG("texture: " << filename << ", " << width << "x" << height << " (" << fimg.GetChannels() << ") --> id " << *tex_id << "\n");
	
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


GLint MultiStage::Ogl_CompileProgram(String shader_source) {
	GLint frag, program;
	
	
	frag = Ogl_CompileShader(GL_FRAGMENT_SHADER, shader_source);
	if (frag < 0)
		return -1;
		
	program = glCreateProgram();
	glProgramParameteri(program, GL_PROGRAM_SEPARABLE, GL_TRUE);
	
	glAttachShader(program, frag);
	glDeleteShader (frag);
	
	return program;
}

bool MultiStage::Ogl_LinkProgram(Stage& s) {
	GLint program = s.fg_prog;
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
