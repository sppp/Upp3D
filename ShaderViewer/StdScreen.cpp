#include "ShaderViewer.h"

NAMESPACE_SPPP_BEGIN


StdScreen*& GetStdScreen() {thread_local static StdScreen* s; return s;}

void LocalPaint() {GetStdScreen()->Paint();}
void LocalMousePressHandler(int button, int state, int x, int y) {GetStdScreen()->MousePressHandler(button, state, x, y);}
void LocalMouseMoveHandler(int x, int y) {GetStdScreen()->MouseMoveHandler(x, y);}
void LocalKeyboardHandler(unsigned char key, int x, int y) {GetStdScreen()->KeyboardHandler(key, x, y);}
void LocalSetUpdateInterval(int value) {GetStdScreen()->SetUpdateInterval(value);}




StdScreen::StdScreen() {
	GetStdScreen() = this;
}

bool StdScreen::Init(const String& arg) {
	
	return true;
}

void StdScreen::UpdatePixelBuffers() {
	
}

void StdScreen::InitGlew() {
	GLenum status;
	
	status = glewInit();
	
	if (status != GLEW_OK) {
		fprintf(stderr, "glewInit error: %s\n", glewGetErrorString(status));
		exit(-1);
	}
	
	fprintf(stderr,
			"GL_VERSION   : %s\nGL_VENDOR    : %s\nGL_RENDERER  : %s\n"
			"GLEW_VERSION : %s\nGLSL VERSION : %s\n\n",
			glGetString(GL_VERSION), glGetString(GL_VENDOR),
			glGetString(GL_RENDERER), glewGetString(GLEW_VERSION),
			glGetString(GL_SHADING_LANGUAGE_VERSION));
	        
	if (!GLEW_VERSION_2_1) {
		fprintf(stderr, "OpenGL 2.1 or better required for GLSL support.");
		exit(-1);
	}
}

bool StdScreen::Open() {
	#ifdef flagGLUT
	int argc = 0;
	char* argv[1] = {0};
	glutInit(&argc, argv);
	
	int w = 800, h = 600;
	glutInitWindowSize(w, h);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	glutCreateWindow("Shadertoy");
	
	InitGlew();
	
	
	for(int i = 0; i < passes.GetCount(); i++) {
		StdPass& pass = passes[i];
		
		if (pass.is_common) {
			pass.prog = -1;
			continue;
		}
		
		if (pass.code.GetCount() == 0) {
			last_error = "Empty source code";
			return false;
		}
		
		std::string code;
		
		/*
			TODO
				iChannelTime
				iChannelResolution
		*/
				
		code =		"#define GL_ES\n"
		
					"uniform vec3      iResolution;           // viewport resolution (in pixels)\n"
					"uniform float     iTime;                 // shader playback time (in seconds)\n"
					"uniform float     iTimeDelta;            // duration since the previous frame (in seconds)\n"
					"uniform float     iFrame;                // frames since the shader (re)started\n"
					"uniform vec2      iOffset;               \n"
					"uniform vec4      iMouse;                // mouse pixel coords. xy: current (if MLB down), zw: click\n"
					"uniform sampler2D iChannel0;             // input channel. XX = 2D/Cube\n"
					"uniform sampler2D iChannel1;             // input channel. XX = 2D/Cube\n"
					"uniform sampler2D iChannel2;             // input channel. XX = 2D/Cube\n"
					"uniform sampler2D iChannel3;             // input channel. XX = 2D/Cube\n"
					"uniform vec4      iDate;                 // (year, month, day, time in secs)\n"
					"uniform float     iSampleRate;           // sound sample rate (i.e., 44100)\n"
					"uniform float     iChannelTime[4];       // channel playback time (in seconds)\n"
					"uniform vec3      iChannelResolution[4]; // channel resolution (in pixels)\n"
					"\n";
			
		for(int j = 0; j < passes.GetCount(); j++) {
			StdPass& pass0 = passes[j];
			if (pass0.is_common)
				code += pass0.code;
		}
		
		code += pass.code;
		
		code +=		"\nvoid main (void) {\n"
					"	vec4 color = vec4 (0.0, 0.0, 0.0, 1.0);\n"
					"	mainImage (color, gl_FragCoord.xy);\n"
					"	color.w = 1.0;\n"
					"	gl_FragColor = color;\n"
					"}\n\n";
		
		//std::cout << code << "\n";
		
		pass.prog = LinkProgram(code);
		if (pass.prog < 0) {
			last_error = "Failed to link shader program. Aborting";
			return false;
		}

		for(int j = 0; j < pass.in.GetCount(); j++) {
			StdPassInput& in = pass.in[j];
			if (in.type == INPUT_TEXTURE) {
				if (!LoadTexture(in.filename, GL_TEXTURE_2D, &in.tex, in.filter, in.repeat, in.vflip)) {
					last_error = "Couldn't load texture";
					return false;
				}
			}
			else {
				last_error = "Input source is not yet implemented";
				return false;
			}
		}
	}
	
	UpdateTexBuffers(w, h);
	
	
	cur_w = w;
	cur_h = h;
	
	glutDisplayFunc(LocalPaint);
	glutMouseFunc(LocalMousePressHandler);
	glutMotionFunc(LocalMouseMoveHandler);
	glutKeyboardFunc(LocalKeyboardHandler);
	
	SetUpdateInterval(1000 / 60);
	
	
	#else
	#error Not implemented
	#endif
	
	return true;
}

void StdScreen::Close() {
	glutLeaveMainLoop();
}

void StdScreen::Destroy() {
	glutDestroyWindow(glutGetWindow());
}

void StdScreen::EventLoop() {
	#ifdef flagGLUT
	glutMainLoop();
	#else
	#error Not implemented
	#endif
}

void StdScreen::GetSize(int* w, int* h) const {
	if (w) *w = glutGet(GLUT_WINDOW_WIDTH);
	if (h) *h = glutGet(GLUT_WINDOW_HEIGHT);
}

void StdScreen::GetCursor(int* x, int* y) const {
	if (x) *x = mouse[0];
	if (y) *y = mouse[1];
}




void StdScreen::MousePressHandler(int button, int state, int x, int y) {
	char msg[1000];
	int x0, y0, height;
	
	if (button != GLUT_LEFT_BUTTON)
		return;
		
	if (state == GLUT_DOWN) {
		x0     = glutGet(GLUT_WINDOW_X);
		y0     = glutGet(GLUT_WINDOW_Y);
		height = glutGet(GLUT_WINDOW_HEIGHT);
		
		if (geometry[0] > 0.1 && geometry[1] > 0.1) {
			mouse[2] = mouse[0] =               geometry[2] + x0 + x;
			mouse[3] = mouse[1] = geometry[1] - geometry[3] - y0 - y;
		} else {
			mouse[2] = mouse[0] = x;
			mouse[3] = mouse[1] = height - y;
		}
	} else {
		mouse[2] = -1;
		mouse[3] = -1;
	}
	
}

void StdScreen::MouseMoveHandler(int x, int y) {
	char msg[1000];
	int x0, y0, height;
	
	x0     = glutGet(GLUT_WINDOW_X);
	y0     = glutGet(GLUT_WINDOW_Y);
	height = glutGet(GLUT_WINDOW_HEIGHT);
	
	if (geometry[0] > 0.1 && geometry[1] > 0.1) {
		mouse[0] =               geometry[2] + x0 + x;
		mouse[1] = geometry[1] - geometry[3] - y0 - y;
	} else {
		mouse[0] = x;
		mouse[1] = height - y;
	}
	
}












void StdScreen::KeyboardHandler(unsigned char key, int x, int y) {
	switch (key) {
	case '\x1b':  /* Escape */
	case 'q':
	case 'Q':
		glutLeaveMainLoop();
		break;
		
	case 'f': /* fullscreen */
	case 'F':
		glutFullScreenToggle();
		break;
		
	default:
		break;
	}
}


void StdScreen::SetUpdateInterval(int value) {
	// events
	
	glutPostRedisplay();
	glutTimerFunc(value, LocalSetUpdateInterval, value);
}


void StdScreen::Paint() {
	int x0, y0, width, height;
	
	Time now = GetSysTime();
	
	x0     = glutGet(GLUT_WINDOW_X);
	y0     = glutGet(GLUT_WINDOW_Y);
	width  = glutGet(GLUT_WINDOW_WIDTH);
	height = glutGet(GLUT_WINDOW_HEIGHT);
	
	if (width != cur_w || height != cur_h) {
		cur_w = width;
		cur_h = height;
		UpdatePixelBuffers();
	}
	
	
	for(int i = 0; i < passes.GetCount(); i++) {
		StdPass& pass = passes[i];
		if (pass.prog < 0)
			continue;
		
		GLint prog = pass.prog;
		GLint uindex;
		
		/*if (pass.is_buffer) {
			const GLenum bufs[] = {GL_COLOR_ATTACHMENT0_EXT};
			// combine FBO
		    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, pass.frame_buf);
		    
		    // set up render target
		    glDrawBuffers(sizeof bufs / sizeof bufs[0], bufs);
		}
		else {
			ASSERT(pass.color_buf > 0);
			// activate texturemap
		    glActiveTexture(GL_TEXTURE0);
		    glBindTexture(GL_TEXTURE_2D, pass.color_buf);
		}*/
	    
		glUseProgram(prog);
		
		
		if (frames % 100 == 0) {
			double elapsed = frame_time.Seconds();
			fprintf(stderr, "FPS: %.2f\n", 1.0 / elapsed);
		}
		/*
		TODO
			iChannelTime
			iChannelResolution
		*/
		uindex = glGetUniformLocation(prog, "iResolution");
		if (uindex >= 0) {
			if (geometry[0] > 0.1 && geometry[1] > 0.1)
				glUniform3f(uindex, geometry[0], geometry[1], 1.0);
			else
				glUniform3f(uindex, width, height, 1.0);
		}
		
		uindex = glGetUniformLocation(prog, "iTime");
		if (uindex >= 0) {
			double t = total_time.Seconds();
			glUniform1f(uindex, t);
		}
		
		uindex = glGetUniformLocation(prog, "iTimeDelta");
		if (uindex >= 0) {
			double t = frame_time.Seconds();
			glUniform1f(uindex, t);
		}
		
		uindex = glGetUniformLocation(prog, "iFrame");
		if (uindex >= 0) {
			glUniform1f(uindex, frames);
		}
		
		uindex = glGetUniformLocation(prog, "iMouse");
		if (uindex >= 0)
			glUniform4f(uindex, mouse[0],  mouse[1], mouse[2], mouse[3]);
		
		uindex = glGetUniformLocation(prog, "iDate");
		if (uindex >= 0) {
			int sec = ((int)now.hour * 60 + (int)now.minute) * 60 + (int)now.second;
			glUniform4f(uindex, now.year, now.month, now.day, sec);
		}
		
		uindex = glGetUniformLocation(prog, "iSampleRate");
		if (uindex >= 0)
			glUniform1f(uindex, 44100); // TODO
		
		uindex = glGetUniformLocation(prog, "iOffset");
		if (uindex >= 0) {
			if (geometry[0] > 0.1 && geometry[1] > 0.1) {
				glUniform2f(uindex,
							x0 + geometry[2],
							geometry[1] - (y0 + height) - geometry[3]);
			} else {
				glUniform2f(uindex, 0.0, 0.0);
			}
		}
		
		uindex = glGetUniformLocation(prog, "iChannel0");
		if (uindex >= 0) {
			if (pass.in.GetCount() >= 1) {
				glActiveTexture(GL_TEXTURE0 + 0);
				glBindTexture(GL_TEXTURE_2D, pass.in[0].tex);
				glUniform1i(uindex, GL_TEXTURE0 + 0);
			}
		}
		
		uindex = glGetUniformLocation(prog, "iChannel1");
		if (uindex >= 0) {
			if (pass.in.GetCount() >= 2) {
				glActiveTexture(GL_TEXTURE0 + 1);
				glBindTexture(GL_TEXTURE_2D, pass.in[1].tex);
				glUniform1i(uindex, GL_TEXTURE0 + 1);
			}
		}
		
		uindex = glGetUniformLocation(prog, "iChannel2");
		if (uindex >= 0) {
			if (pass.in.GetCount() >= 3) {
				glActiveTexture(GL_TEXTURE0 + 2);
				glBindTexture(GL_TEXTURE_2D, pass.in[2].tex);
				glUniform1i(uindex, GL_TEXTURE0 + 2);
			}
		}
		
		uindex = glGetUniformLocation(prog, "iChannel3");
		if (uindex >= 0) {
			if (pass.in.GetCount() >= 4) {
				glActiveTexture(GL_TEXTURE0 + 3);
				glBindTexture(GL_TEXTURE_2D, pass.in[3].tex);
				glUniform1i(uindex, GL_TEXTURE0 + 3);
			}
		}
		
		glClear(GL_COLOR_BUFFER_BIT);
		glRectf(-1.0, -1.0, 1.0, 1.0);
		
		
		if (pass.frame_buf > 0) {
			// backup render target
		    glDrawBuffer(GL_FRONT);
		    
		    // reset FBO
		    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
		}
	}
	
	
	glutSwapBuffers();
	
	frames++;
	frame_time.Reset();
}



	








void StdPass::ClearTex() {
	if (color_buf >= 0) {
		GLuint i[2] = {color_buf, depth_buf};
		glDeleteTextures(2, i);
		glDeleteFramebuffers(1, &frame_buf);
		color_buf = 0;
		depth_buf = 0;
		frame_buf = 0;
	}
}




void StdScreen::SetPassCount(int count) {
	passes.SetCount(count);
}

void StdScreen::SetInputCount(int pass, int count) {
	passes[pass].in.SetCount(count);
}

void StdScreen::SetOutputCount(int pass, int count) {
	passes[pass].out.SetCount(count);
}

void StdScreen::RealizeCount(int pass, int in, int out) {
	if (pass >= (int)passes.GetCount())
		passes.SetCount(pass+1);
	
	if (in  >= (int)passes[pass].in.GetCount())
		passes[pass].in.SetCount(in+1);
	
	if (out >= (int)passes[pass].out.GetCount())
		passes[pass].out.SetCount(out+1);
}

void StdScreen::SetPassCommon(int pass, bool b) {
	passes[pass].is_common = b;
}

void StdScreen::SetPassCode(int pass, const char* code) {
	passes[pass].code = code;
}

void StdScreen::SetInputId(int pass, int i, int id) {
	passes[pass].in[i].id = id;
}

void StdScreen::SetInputType(int pass, int i, int type) {
	passes[pass].in[i].type = type;
}

void StdScreen::SetInputFilename(int pass, int i, const char* filename) {
	passes[pass].in[i].filename = filename;
}

void StdScreen::SetOutputId(int pass, int i, int id) {
	passes[pass].out[i].id = id;
}

int StdScreen::GetPassCount() const {
	return passes.GetCount();
}

























void StdScreen::UpdateTexBuffers(int w, int h) {
	bool set_output = true;
	for (int i = passes.GetCount()-1; i >= 0; i--) {
		StdPass& pass = passes[i];
		pass.ClearTex();
		if (pass.is_common)
			continue;
		
		if (set_output) {
			set_output = false;
			pass.is_buffer = false;
		}
		else {
			pass.is_buffer = true;
			ASSERT(pass.color_buf < 0);
			
			// color buffer
			glGenTextures(1, &pass.color_buf);
			glBindTexture(GL_TEXTURE_2D, pass.color_buf);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glBindTexture(GL_TEXTURE_2D, 0);
			
			// depth buffer
			glGenRenderbuffersEXT(1, &pass.depth_buf);
			glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, pass.depth_buf);
			glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT, w, h);
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
}




int StdScreen::LoadTexture(std::string filename, GLenum type, GLenum *tex_id, char filter, char repeat, bool flip) {
	GdkPixbuf *pixbuf;
	int width, height;
	uint8_t *data;
	GLfloat *tex_data;
	int rowstride;
	int cpp, bps;
	int x, y, c;
	
	pixbuf = gdk_pixbuf_new_from_file(filename.c_str(), NULL);
	
	width = gdk_pixbuf_get_width(pixbuf);
	height = gdk_pixbuf_get_height(pixbuf);
	
	data = gdk_pixbuf_get_pixels(pixbuf);
	rowstride = gdk_pixbuf_get_rowstride(pixbuf);
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
		uint8_t  *cur_row8  = (uint8_t *)(data + y2 * rowstride);
		uint16_t *cur_row16 = (uint16_t *)(data + y2 * rowstride);
		
		for (x = 0; x < width; x++) {
			for (c = 0; c < cpp; c++) {
				if (bps == 8)
					tex_data[(y * width + x) * cpp + c] = ((GLfloat) cur_row8[x * cpp + c]) / 255.0;
				else
					tex_data[(y * width + x) * cpp + c] = ((GLfloat) cur_row16[x * cpp + c]) / 65535.0;
			}
		}
	}
	
	glGenTextures(1, tex_id);
	glBindTexture(type, *tex_id);
	glTexImage2D(type, 0, GL_RGBA,
				 width, height,
				 0, cpp == 3 ? GL_RGB : GL_RGBA,
				 GL_FLOAT,
				 tex_data);
	if (filter == 0) {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	} else {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	
	if (repeat) {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	} else {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	}
	
	free(tex_data);
	g_object_unref(pixbuf);
	
	fprintf(stderr, "texture: %s, %dx%d, %d (%d) --> id %d\n",
			filename.c_str(), width, height, cpp, bps, *tex_id);
	        
	return 1;
}


GLint StdScreen::CompileShader(const GLenum shader_type, std::string shader_source) {
	GLuint shader = glCreateShader(shader_type);
	GLint status = GL_FALSE;
	GLint loglen;
	GLchar *error_message;
	
	const GLchar* src = shader_source.c_str();
	glShaderSource(shader, 1, &src, NULL);
	glCompileShader(shader);
	
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if (status != GL_TRUE) {
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &loglen);
		error_message = (GLchar*)calloc(loglen, sizeof(GLchar));
		glGetShaderInfoLog(shader, loglen, NULL, error_message);
		fprintf(stderr, "shader failed to compile:\n   %s\n", error_message);
		free(error_message);
		
		return -1;
	}
	
	return shader;
}


GLint StdScreen::LinkProgram(std::string shader_source) {
	GLint frag, program;
	GLint status = GL_FALSE;
	GLint loglen, n_uniforms;
	GLchar *error_message;
	GLint i;
	
	GLchar name[80];
	GLsizei namelen;
	
	frag = CompileShader(GL_FRAGMENT_SHADER, shader_source);
	if (frag < 0)
		return -1;
		
	program = glCreateProgram();
	
	glAttachShader(program, frag);
	glDeleteShader (frag);
	
	glLinkProgram(program);
	
	
	glGetProgramiv(program, GL_LINK_STATUS, &status);
	if (status != GL_TRUE) {
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &loglen);
		error_message = (GLchar*)calloc(loglen, sizeof(GLchar));
		glGetProgramInfoLog(program, loglen, NULL, error_message);
		fprintf(stderr, "program failed to link:\n   %s\n", error_message);
		free(error_message);
		return -1;
	}
	
	// diagnostics
	glGetProgramiv(program, GL_ACTIVE_UNIFORMS, &n_uniforms);
	fprintf(stderr, "%d uniforms:\n", n_uniforms);
	
	for (i = 0; i < n_uniforms; i++) {
		GLint size;
		GLenum type;
		
		glGetActiveUniform(program, i, 79, &namelen, &size, &type, name);
		name[namelen] = '\0';
		fprintf(stderr, "  %2d: %-20s (type: 0x%04x, size: %d)\n", i, name, type, size);
	}
	
	total_time.Reset();
	frame_time.Reset();
	
	return program;
}




NAMESPACE_SPPP_END
