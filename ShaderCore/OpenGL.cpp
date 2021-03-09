#include "ShaderCore.h"

NAMESPACE_SHADER_BEGIN


void MultiStage::Ogl_CreateTex(Stage& s, Size sz, int channels) {
	bool create_depth = true;
	bool create_fbo = true;
	int buf_count = 1;
	if (s.is_doublebuf)
		buf_count++;
	
	EnableOpenGLDebugMessages(1);
	
	for(int bi = 0; bi < buf_count; bi++) {
		GLuint& color_buf = s.color_buf[bi];
		GLuint& depth_buf = s.depth_buf[bi];
		GLuint& frame_buf = s.frame_buf[bi];
		ASSERT(color_buf == 0);
		
		// color buffer
		glGenTextures(1, &color_buf);
		glBindTexture(GL_TEXTURE_2D, color_buf);
		glTexImage2D(GL_TEXTURE_2D, 0, GetChCode(channels, true), sz.cx, sz.cy, 0, GetChCode(channels), GL_UNSIGNED_BYTE, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glBindTexture(GL_TEXTURE_2D, 0);
		
		// depth buffer
		if (create_depth) {
			glGenRenderbuffersEXT(1, &depth_buf);
			glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, depth_buf);
			glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT, sz.cx, sz.cy);
			glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, 0);
		}
		
		// FBO
		if (create_fbo) {
			glGenFramebuffersEXT(1, &frame_buf);
			glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, frame_buf);
			
			// combine FBO to color buffer
			glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, color_buf, 0);
			
			// combine FBO to depth buffer
			if (create_depth)
				glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, depth_buf);
			
			// reset FBO
			glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
		}
	}
	
	EnableOpenGLDebugMessages(0);
}

void MultiStage::Ogl_UpdateTexBuffers() {
	bool set_output = true;
	for (int i = passes.GetCount()-1; i >= 0; i--) {
		Stage& pass = passes[i];
		pass.ClearTex();
		if (pass.type == Stage::TYPE_LIBRARY) {
			// do nothing
		}
		else if (pass.type == Stage::TYPE_IMAGE) {
			// do nothing
		}
		else if (pass.type == Stage::TYPE_SOUND || pass.type == Stage::TYPE_SOUND_BUFFER) {
			Ogl_CreateTex(pass, Size(audio_sample_size, 1), 2);
			sound_buf.SetCount(audio_sample_size, vec2(0,0));
		}
		else if (pass.type == Stage::TYPE_IMAGE_BUFFER) {
			Ogl_CreateTex(pass, stream.size, 4);
		}
		else {
			LOG("warning: unhandled stage type " << pass.GetStageTypeString());
		}
	}
	
	// Keyboard buffer
	int key_buf_size = key_tex_w * key_tex_h;
	DataBuffer& key_buf = data_bufs.GetAdd(DATA_IN_KEYBOARD);
	key_buf.data.SetCount(key_buf_size, 0);
	glGenTextures(1, &key_buf.tex);
	glBindTexture(GL_TEXTURE_2D, key_buf.tex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, key_tex_w, key_tex_h, 0, GL_RED, GL_UNSIGNED_BYTE, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	
	
}



int MultiStage::Ogl_NewTexture(Size res, GLuint* tex_, int tex_n, GLenum type, char filter, char repeat) {
	glGenTextures(tex_n, tex_);
	
	for(int i = 0; i < tex_n; i++) {
		GLuint& tex = tex_[i];
		
		glBindTexture(type, tex);
		
		glTexImage2D(type, 0, GL_RGBA32F,
					 res.cx, res.cy,
					 0, GetChCode(4),
					 GL_FLOAT,
					 0);
		
		Ogl_TexFlags(type, filter, repeat);
		
		GLenum err = glGetError();
		if (err != GL_NO_ERROR) {
			last_error = "gl error " + HexStr(err);
			glBindTexture(type, 0);
			return 0;
		}
	}
	
	glBindTexture(type, 0);
	
	return 1;
}

int MultiStage::Ogl_LoadTexture(String filename, GLenum type, StageInput& in, char filter, char repeat, bool flip) {
	Image img = StreamRaster::LoadFileAny(filename);
	if (!img) {
		last_error = "couldn't load file " + filename;
		return 0;
	}
	
	int width = img.GetWidth();
	int height = img.GetHeight();
	in.tex = 0;
	in.res = Size(width, height);
	if (!width || !height) {
		last_error = "empty image " + filename;
		return 0;
	}
	
	glGenTextures(1, &in.tex);
	glBindTexture(type, in.tex);
	
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
						 0, GetChCode(channels),
						 GL_FLOAT,
						 fimg.Detach());
		}
		else if (type == GL_TEXTURE_3D) {
			glTexImage3D(type, 0, GL_RGBA32F,
						 width, height, 1,
						 0, GetChCode(channels),
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
						 0, GetChCode(channels),
						 GL_UNSIGNED_BYTE,
						 bimg.Detach());
		}
		else if (type == GL_TEXTURE_3D) {
			glTexImage3D(type, 0, GL_RGBA,
						 width, height, 1,
						 0, GetChCode(channels),
						 GL_UNSIGNED_BYTE,
						 bimg.Detach());
		}
	}
	
	Ogl_TexFlags(type, filter, repeat);
	
	
	LOG("\t\t\ttexture: " << filename << ", " << width << "x" << height << " (" << channels << ") --> id " << in.tex << "\n");
	
	GLenum err = glGetError();
	if (err != GL_NO_ERROR) {
		last_error = "gl error " + HexStr(err);
		glBindTexture(type, 0);
		return 0;
	}
	
	glBindTexture(type, 0);
	return 1;
}

int MultiStage::Ogl_LoadVolume(String filename, GLenum *tex_id, char filter, char repeat, bool flip) {
	ASSERT(glGetError() == GL_NO_ERROR);
	
	String s = LoadFile(filename);
	if (s.IsEmpty()) {
		last_error = "couldn't load file " + filename;
		return 0;
	}
	Vector<byte> values;
	MemReadStream data(s.Begin(), s.GetCount());
	int magic, w, h, d, channels, sz;
	data.Get(&magic, 4); // "BIN "
	if (magic == 0x004e4942) {
		data.Get(&w, 4);
		data.Get(&h, 4);
		data.Get(&d, 4);
		data.Get(&channels, 4);
		sz = w * h * d * channels;
		values.SetCount(sz);
		data.Get(values.Begin(), sz);
	}
	else {
		int len = 0;
		for(int i = 1; i <= 4; i++) {
			int per_ch = s.GetCount() / i;
			int root = pow(per_ch, 1.0/3.0);
			int test_sz = root * root * root * i;
			if (test_sz == s.GetCount()) {
				channels = i;
				len = root;
				break;
			}
		}
		if (channels <= 0 || len <= 0) {
			last_error = "couldn't get volume data dimensions";
			return 0;
		}
		w = h = d = len;
		sz = w * h * d * channels;
		values.SetCount(sz);
		MemoryCopy(values.Begin(), s.Begin(), sz);
	}
	
	int intl_fmt = GetChCode(channels);
	
	//glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	//glPixelStorei(GL_PACK_ALIGNMENT, 1);
	
	glGenTextures(1, tex_id);
	glBindTexture(GL_TEXTURE_3D, *tex_id);
	
	GLenum err;
	
	err = glGetError();
	if (err != GL_NO_ERROR) {
		last_error = "gl error " + HexStr(err);
		glBindTexture(GL_TEXTURE_3D, 0);
		return 0;
	}
	
	byte* tmp_data = (byte*)malloc(sz);
	memcpy(tmp_data, values.Begin(), sz);
	
	if (1) {
		glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA32F,
					 w, h, d,
					 0, intl_fmt,
					 GL_UNSIGNED_BYTE,
					 tmp_data);
	}
	else {
		glTexStorage3D(GL_TEXTURE_3D, 1, GL_R8, w, h, d);
		glTexSubImage3D(GL_TEXTURE_3D, 0, 0, 0, 0, w, h, d, intl_fmt, GL_UNSIGNED_SHORT, (void*)tmp_data);
	}
	
	err = glGetError();
	if (err != GL_NO_ERROR) {
		last_error = "gl error " + HexStr(err);
		glBindTexture(GL_TEXTURE_3D, 0);
		return 0;
	}
	
	Ogl_TexFlags(GL_TEXTURE_3D, filter, repeat);
	
	err = glGetError();
	if (err != GL_NO_ERROR) {
		last_error = "gl error " + HexStr(err);
		glBindTexture(GL_TEXTURE_3D, 0);
		return 0;
	}
	
	LOG("\t\t\ttexture: " << filename << ", " << w << "x" << h << "x" << d << " (" << channels << ") --> id " << *tex_id << "\n");
	
	glBindTexture(GL_TEXTURE_3D, 0);
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
			last_error = "couldn't load file " + path;
			glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
			return 0;
		}
		
		int width = img.GetWidth();
		int height = img.GetHeight();
		if (!width || !height) {
			last_error = "empty image " + path;
			glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
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
						 0, GetChCode(channels),
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
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA32F,
						 width, height,
						 0, GetChCode(channels),
						 GL_UNSIGNED_BYTE,
						 bimg.Detach());
		}
		
		
		LOG("\t\t\ttexture #" << i << ": " << path << ", " << width << "x" << height << " (" << channels << ") --> id " << *tex_id);
	}
	
	Ogl_TexFlags(GL_TEXTURE_CUBE_MAP, filter, repeat);
	
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	
	return 1;
}

void MultiStage::Ogl_TexFlags(int type, int filter, int repeat) {
	if (filter == StageInput::FILTER_NEAREST) {
		glTexParameteri(type, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(type, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	}
	else if (filter == StageInput::FILTER_LINEAR) {
		glTexParameteri(type, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(type, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	}
	else if (filter == StageInput::FILTER_MIPMAP) {
		glTexParameteri(type, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(type, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glGenerateMipmap(type);
	}
	
	if (repeat == StageInput::REPEAT_REPEAT) {
		glTexParameteri(type, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(type, GL_TEXTURE_WRAP_T, GL_REPEAT);
		if (type == GL_TEXTURE_3D)
			glTexParameteri(type, GL_TEXTURE_WRAP_R, GL_REPEAT);
	}
	else if (repeat == StageInput::REPEAT_CLAMP) {
		glTexParameteri(type, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(type, GL_TEXTURE_WRAP_T, GL_CLAMP);
		if (type == GL_TEXTURE_3D)
			glTexParameteri(type, GL_TEXTURE_WRAP_R, GL_CLAMP);
	}
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
	
	if (program < 0) {
		last_error = "internal error: opengl program was expected to be valid";
		return false;
	}
	
	glLinkProgram(program);
	
	glGetProgramiv(program, GL_LINK_STATUS, &status);
	if (status != GL_TRUE) {
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &loglen);
		Vector<GLchar> msg;
		msg.SetCount(loglen);
		glGetProgramInfoLog(program, loglen, NULL, msg.Begin());
		if (loglen)
			last_error.Set(msg.Begin(), loglen);
		else
			last_error = "linking failed with unknown error";
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


int MultiStage::GetTexType(Stage& cur_stage, int input_i) const {
	StageInput& in = cur_stage.in[input_i];
	if (in.type == INPUT_VOLUME) {
		return GL_TEXTURE_3D;
	}
	else if (in.type == INPUT_CUBEMAP) {
		return GL_TEXTURE_CUBE_MAP;
	}
	else {
		return GL_TEXTURE_2D;
	}
}

int MultiStage::GetChCode(int channels, bool is_float) {
	if (!is_float) {
		switch (channels) {
			case 1: return GL_RED;
			case 2: return GL_RG;
			case 3: return GL_RGB;
			case 4: return GL_RGBA;
		}
	}
	else {
		switch (channels) {
			case 1: return GL_R32F;
			case 2: return GL_RG32F;
			case 3: return GL_RGB32F;
			case 4: return GL_RGBA32F;
		}
	}
	return 0;
}

NAMESPACE_SHADER_END
