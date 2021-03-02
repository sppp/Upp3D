#include "ShaderCore.h"

NAMESPACE_SHADER_BEGIN

ArrayMap<String,String>& CommonHashToName() {
	static ArrayMap<String,String> map;
	if (map.IsEmpty()) {
		map.Add("85a6d68622b36995ccb98a89bbb119edf167c914660e4450d313de049320005c", "grid");
		map.Add("cbcbb5a6cfb55c36f8f021fbb0e3f69ac96339a39fa85cd96f2017a2192821b5", "nyan");
		map.Add("0681c014f6c88c356cf9c0394ffe015acc94ec1474924855f45d22c3e70b5785", "bg1");
		map.Add("793a105653fbdadabdc1325ca08675e1ce48ae5f12e37973829c87bea4be3232", "bg2");
		map.Add("0a40562379b63dfb89227e6d172f39fdce9022cba76623f1054a2c83d6c0ba5d", "noise1");
		map.Add("550a8cce1bf403869fde66dddf6028dd171f1852f4a704a465e1b80d23955663", "bg3");
		map.Add("3083c722c0c738cad0f468383167a0d246f91af2bfa373e9c5c094fb8c8413e0", "noise2");
		map.Add("27012b4eadd0c3ce12498b867058e4f717ce79e10a99568cca461682d84a4b04", "noise3");
		map.Add("488bd40303a2e2b9a71987e48c66ef41f5e937174bf316d3ed0e86410784b919", "bg4");
		map.Add("95b90082f799f48677b4f206d856ad572f1d178c676269eac6347631d4447258", "metal");
		map.Add("0c7bf5fe9462d5bffbd11126e82908e39be3ce56220d900f633d58fb432e56f5", "noise4");
		map.Add("10eb4fe0ac8a7dc348a2cc282ca5df1759ab8bf680117e4047728100969e7b43", "stonewall");
		map.Add("e6e5631ce1237ae4c05b3563eda686400a401df4548d0f9fad40ecac1659c46c", "wall");
		map.Add("8de3a3924cb95bd0e95a443fff0326c869f9d4979cd1d5b6e94e2a01f5be53e9", "uk_street");
		map.Add("585f9546c092f53ded45332b343144396c0b2d70d9965f585ebc172080d8aa58", "yard");
		map.Add("94284d43be78f00eb6b298e6d78656a1b34e2b91b34940d02f1ca8b22310e8a0", "bg5");
		map.Add("ad56fba948dfba9ae698198c109e71f118a54d209c0ea50d77ea546abad89c57", "stone1");
		map.Add("52d2a8f514c4fd2d9866587f4d7b2a5bfa1a11a0e772077d7682deb8b3b517e5", "stone2");
		map.Add("aea6b99da1d53055107966b59ac5444fc8bc7b3ce2d0bbb6a4a3cbae1d97f3aa", "noise5");
		map.Add("bd6464771e47eed832c5eb2cd85cdc0bfc697786b903bfd30f890f9d4fc36657", "brown_grid");
		map.Add("1f7dca9c22f324751f2a5a59c9b181dfe3b5564a04b724c657732d0bf09c99db", "wood1");
		map.Add("92d7758c402f0927011ca8d0a7e40251439fba3a1dac26f5b8b62026323501aa", "stone3");
		map.Add("cd4c518bc6ef165c39d4405b347b51ba40f8d7a065ab0e8d2e4f422cbc1e8a43", "stone4");
		map.Add("8979352a182bde7c3c651ba2b2f4e0615de819585cc37b7175bcefbca15a6683", "stone5");
		map.Add("fb918796edc3d2221218db0811e240e72e340350008338b0c07a52bd353666a6", "wood2");
		map.Add("f735bee5b64ef98879dc618b016ecf7939a5756040c2cde21ccb15e69a6e1cfb", "noise6");
		map.Add("3871e838723dd6b166e490664eead8ec60aedd6b8d95bc8e2fe3f882f0fd90f0", "stone6");
		map.Add("79520a3d3a0f4d3caa440802ef4362e99d54e12b1392973e4ea321840970a88a", "stone7");
		map.Add("08b42b43ae9d3c0605da11d0eac86618ea888e62cdd9518ee8b9097488b31560", "alphabet");
	}
	return map;
}

void RemoveToken(String& glsl, String token) {
	if (glsl.Left(token.GetCount() + 1) == token + " ") glsl = glsl.Mid(token.GetCount() + 1);
	glsl.Replace((String)" " + token + " ", " ");
	glsl.Replace((String)"\n" + token + " ", "\n");
}




void MultiStage::DumpStages() {
	LOG("Name: " << name);
	LOG("Description: " << name);
	for(int i = 0; i < passes.GetCount(); i++) {
		Stage& s = passes[i];
		LOG(i << " stage");
		LOG("\tName: " << s.name);
		LOG("\tDescription: " << s.description);
		LOG("\tInputs:");
		for(int j = 0; j < s.in.GetCount(); j++) {
			StageInput& in = s.in[j];
			LOG("\t" << j << " input");
			if (in.type == INPUT_BUFFER) {
				Stage& ins = GetStageById(in.id);
				LOG("\t\tName: " << ins.name);
			}
			LOG("\t\tType: " << GetInputTypeString(in.type));
			if (in.filename.GetCount()) {
				LOG("\t\tFilename: " << in.filename);
			}
			LOG("\t\tRepeat: " << (in.repeat ? "yes" : "no"));
			LOG("\t\tFilter: " << (in.filter == 0 ? "nearest" : "linear"));
			LOG("\t\tV-flip: " << (in.vflip ? "yes" : "no"));
		}
	}
}

bool MultiStage::Load(String path) {
	if (DirectoryExists(path)) {
		if (path.Right(1) == DIR_SEPS)
			path = path.Left(path.GetCount()-1);
		String name = GetFileName(path);
		path = AppendFileName(path, name + ".toy");
	}
	
	String toy = LoadFile(path);
	if (toy.IsEmpty())
		return false;
	
	String file_dir = GetFileDirectory(path);
	Index<String> dirs;
	dirs.Add( ShareDirFile("imgs") );
	dirs.Add( ShareDirFile("sounds") );
	dirs.Add( ShareDirFile("videos") );
	//DUMPC(dirs);
	
	passes.Clear();
	RealizeCount(0, -1);
	
	Vector<String> lines = Split(toy, "\n");
	for(int i = 0; i < lines.GetCount(); i++) {
		String line = lines[i];
		int j = line.Find("=");
		if (j < 0)
			continue;
		String key = TrimBoth(line.Left(j));
		String value = TrimBoth(line.Mid(j+1));
		
		if (value.IsEmpty())
			continue;
		
		if (key == "name") {
			name = value;
		}
		else if (key == "description") {
			description = value;
		}
		else if (key == "date") {
			
		}
		else if (key == "likes") {
			
		}
		else if (key == "owner") {
			
		}
		else {
			Vector<String> values = Split(key, "-");
			if (values.GetCount() == 3) {
				int pass_i = StrInt(values[1]);
				key = values[2];
				RealizeCount(pass_i, -1);
				Stage& s = passes[pass_i];
				
				if (key == "name") {
					s.name = value;
				}
				else if (key == "description") {
					s.description = value;
				}
				else if (key == "type") {
					if (value == "common") {
						SetPassCommon(pass_i, true);
					}
				}
				else {
					LOG("Invalid key: " << key);
					return false;
				}
			}
			else if (values.GetCount() == 5) {
				int pass_i = StrInt(values[1]);
				int io_i = StrInt(values[3]);
				String io = values[2];
				key = values[4];
				if (io == "input") {
					RealizeCount(pass_i, io_i);
					if (key == "id") {
						SetInputId(pass_i, io_i, StrInt(value));
					}
					else if (key == "type") {
						if (value == "texture")
							SetInputType(pass_i, io_i, MultiStage::INPUT_TEXTURE);
						else if (value == "cubemap")
							SetInputType(pass_i, io_i, MultiStage::INPUT_CUBEMAP);
						else if (value == "webcam")
							SetInputType(pass_i, io_i, MultiStage::INPUT_WEBCAM);
						else if (value == "music")
							SetInputType(pass_i, io_i, MultiStage::INPUT_MUSIC);
						else if (value == "musicstream")
							SetInputType(pass_i, io_i, MultiStage::INPUT_MUSICSTREAM);
						else if (value == "keyboard")
							SetInputType(pass_i, io_i, MultiStage::INPUT_KEYBOARD);
						else if (value == "volume")
							SetInputType(pass_i, io_i, MultiStage::INPUT_VOLUME);
						else if (value == "video")
							SetInputType(pass_i, io_i, MultiStage::INPUT_VIDEO);
						else if (value == "buffer")
							SetInputType(pass_i, io_i, MultiStage::INPUT_BUFFER);
						else {
							LOG("Invalid key + value: " << key << " = " << value);
							return false;
						}
					}
					else if (key == "filename") {
						String filename = GetFileName(value);
						String title = GetFileTitle(filename);
						filename = CommonHashToName().Get(title, title) + GetFileExt(filename);
						
						bool found = false;
						for (String dir : dirs) {
							String filepath = AppendFileName(dir, filename);
							if (FileExists(filepath)) {
								SetInputFilename(pass_i, io_i, filepath.Begin());
								found = true;
								break;
							}
						}
						if (!found) {
							LOG("File doesn't exist: " << filename);
						}
					}
					else if (key == "filter") {
						if (value == "nearest")
							SetInputValue(pass_i, io_i, StageInput::FILTER_NEAREST, -1, -1);
						else if (value == "linear")
							SetInputValue(pass_i, io_i, StageInput::FILTER_LINEAR, -1, -1);
						else if (value == "mipmap")
							SetInputValue(pass_i, io_i, StageInput::FILTER_MIPMAP, -1, -1);
					}
					else if (key == "wrap") {
						if (value == "repeat")
							SetInputValue(pass_i, io_i, -1, StageInput::REPEAT_REPEAT, -1);
						else if (value == "clamp")
							SetInputValue(pass_i, io_i, -1, StageInput::REPEAT_CLAMP, -1);
					}
					else if (key == "vflip") {
						if (value == "true" || value == "1")
							SetInputValue(pass_i, io_i, -1, -1, true);
						else if (value == "false" || value == "0")
							SetInputValue(pass_i, io_i, -1, -1, false);
					}
					else {
						LOG("Invalid key: " << key);
						return false;
					}
				}
				else if (io == "output") {
					RealizeCount(pass_i, -1);
					if (key == "id") {
						SetOutputId(pass_i, StrInt(value));
					}
					else {
						LOG("Invalid key: " << key);
						return false;
					}
				}
				else {
					LOG("Invalid key: " << key);
					return false;
				}
			}
			else {
				LOG("Invalid key: " << key);
				return false;
			}
		}
	}
	
	for(int i = 0; i < GetPassCount(); i++) {
		String glsl_path = AppendFileName(file_dir, (String)"stage" + IntStr(i) + ".glsl");
		String glsl = LoadFile(glsl_path);
		
		if (glsl.IsEmpty()) {
			LOG("Couldn't load shader stage: " << glsl_path);
			return false;
		}
		
		RemoveToken(glsl, "lowp");
		RemoveToken(glsl, "mediump");
		RemoveToken(glsl, "highp");
		
		if (glsl.GetCount() && glsl[glsl.GetCount()-1] == 0)
			glsl = glsl.Left(glsl.GetCount()-1);
		
		SetPassCode(i, "", glsl);
	}
	
	
	Sort(passes, Stage());
	
	DumpStages();
	
	//#endif
	
	
	
	return true;
}

bool MultiStage::Open(Size output_sz) {
	if (is_open)
		Close();
	
	frames = 0;
	
	LOG("MultiStage::Open: size " << output_sz.ToString());
	if (passes.IsEmpty()) {
		last_error = "No stages";
		return false;
	}
	
	
	for(int i = 0; i < passes.GetCount(); i++) {
		Stage& pass = passes[i];
		LOG("\tCompiling stage " << i << ": " << pass.name);
		
		if (pass.is_common) {
			ASSERT(pass.prog[Stage::PROG_FRAGMENT] < 0);
			continue;
		}
		
		if (pass.fg_glsl.GetCount() == 0) {
			Close();
			last_error = "Empty source code";
			return false;
		}
		
		String code;
		
		/*
			TODO
				iFrameRate			image/buffer		Number of frames rendered per second
				iChannelTime		image/buffer		Time for channel (if video or sound), in seconds
				iChannelResolution	image/buffer/sound	Input texture resolution for each channel
				

		*/
		
		code =		"#version 430\n"
					"#define GL_ES\n"
		
					"uniform vec3      iResolution;           // viewport resolution (in pixels)\n"
					"uniform float     iTime;                 // shader playback time (in seconds)\n"
					"uniform float     iTimeDelta;            // duration since the previous frame (in seconds)\n"
					"uniform int       iFrame;                // frames since the shader (re)started\n"
					"uniform vec2      iOffset;               \n"
					"uniform vec4      iMouse;                // mouse pixel coords. xy: current (if MLB down), zw: click\n"
					"uniform vec4      iDate;                 // (year, month, day, time in secs)\n"
					"uniform float     iFrameRate;\n"
					"uniform float     iSampleRate;           // sound sample rate (i.e., 44100)\n"
					"uniform float     iChannelTime[4];       // channel playback time (in seconds)\n"
					"uniform vec3      iChannelResolution[4]; // channel resolution (in pixels)\n"
					;
		
		for(int j = 0; j < pass.in.GetCount(); j++) {
			StageInput& in = pass.in[j];
			if (in.type == INPUT_CUBEMAP)
				code << "uniform samplerCube iChannel" << IntStr(j) << ";\n";
			else
				code << "uniform sampler2D iChannel" << IntStr(j) << ";\n";
		}
		code << "\n";
		
		for(int j = 0; j < passes.GetCount(); j++) {
			Stage& pass0 = passes[j];
			if (pass0.is_common)
				code += pass0.fg_glsl;
		}
		
		code += pass.fg_glsl;
		
		code +=		"\nvoid main (void) {\n"
					"	vec4 color = vec4 (0.0, 0.0, 0.0, 1.0);\n"
					"	mainImage (color, gl_FragCoord.xy);\n"
					"	color.w = 1.0;\n"
					"	gl_FragColor = color;\n"
					"}\n\n";
		
		//LOG(code);
		
		if (!Ogl_CompileProgram(pass, code)) {
			LOG(code);
			Close();
			return false;
		}
	}

	gl_stages.SetCount(passes.GetCount());
	glGenProgramPipelines(gl_stages.GetCount(), gl_stages.Begin());
	
	for(int i = 0; i < passes.GetCount(); i++) {
		Stage& pass = passes[i];
		LOG("\tLinking stage " << i << ": " << pass.name);
		
		if (!Ogl_LinkProgram(pass)) {
			Close();
			return false;
		}
	}
	
	for(int i = 0; i < passes.GetCount(); i++) {
		Stage& pass = passes[i];
		LOG("\tLoading resources stage " << i << ": " << pass.name);
		for(int j = 0; j < pass.in.GetCount(); j++) {
			StageInput& in = pass.in[j];
			LOG("\t\tOpening input " << j << ": " << GetInputTypeString(in.type));
			if (in.type == INPUT_TEXTURE) {
				if (!Ogl_LoadTexture(in.filename, GL_TEXTURE_2D, &in.tex, in.filter, in.repeat, in.vflip)) {
					Close();
					last_error = "Couldn't load texture";
					return false;
				}
			}
			else if (in.type == INPUT_CUBEMAP) {
				if (!Ogl_LoadCubemap(in.filename, &in.tex, in.filter, in.repeat, in.vflip)) {
					Close();
					last_error = "Couldn't load texture";
					return false;
				}
			}
			else if (in.type == INPUT_WEBCAM) {
				LOG("error: not implemented " << GetInputTypeString(in.type));
			}
			else if (in.type == INPUT_MUSIC) {
				LOG("error: not implemented " << GetInputTypeString(in.type));
			}
			else if (in.type == INPUT_MUSICSTREAM) {
				LOG("error: not implemented " << GetInputTypeString(in.type));
			}
			else if (in.type == INPUT_KEYBOARD) {
				LOG("error: not implemented " << GetInputTypeString(in.type));
			}
			else if (in.type == INPUT_VOLUME) {
				LOG("error: not implemented " << GetInputTypeString(in.type));
			}
			else if (in.type == INPUT_VIDEO) {
				LOG("error: not implemented " << GetInputTypeString(in.type));
			}
			else if (in.type == INPUT_BUFFER) {
				if (pass.id == in.id)
					pass.is_doublebuf = true;
			}
			else {
				Close();
				last_error = "Input source is not yet implemented";
				return false;
			}
		}
	}
	
	for(int i = 0; i < passes.GetCount(); i++) {
		Stage& s = passes[i];
		uint32& gl_s = gl_stages[i];
		
		for(int j = 0; j < Stage::PROG_COUNT; j++) {
			GLint& prog = s.prog[j];
			if (prog >= 0) {
				int bit = 1 << j;
				ASSERT(j != Stage::PROG_VERTEX || bit == GL_VERTEX_SHADER_BIT);
				ASSERT(j != Stage::PROG_FRAGMENT || bit == GL_FRAGMENT_SHADER_BIT);
				glUseProgramStages(gl_s, bit, prog);
			}
		}
	}
	
	size = output_sz;
	Ogl_UpdateTexBuffers();
	
	total_time.Reset();
	frame_time.Reset();
	
	is_open = true;
	return true;
}

void MultiStage::Close() {
	for(Stage& s : passes) {
		s.ClearTex();
		for(int i = 0; i < Stage::PROG_COUNT; i++) {
			auto& prog = s.prog[i];
			if (prog >= 0) {
				glDeleteProgram(prog);
				prog = -1;
			}
		}
	}
	
	if (gl_stages.GetCount()) {
		glDeleteProgramPipelines(gl_stages.GetCount(), gl_stages.Begin());
		gl_stages.Clear();
	}
	
	
	if (is_open) {
		
		is_open = false;
	}
}

void MultiStage::LeftDown(Point pt, dword keyflags) {
	if (geometry[0] > 0.1 && geometry[1] > 0.1) {
		mouse[2] = mouse[0] =               geometry[2] + pt.x;
		mouse[3] = mouse[1] = geometry[1] - geometry[3] - pt.y;
	} else {
		mouse[2] = mouse[0] = 0;
		mouse[3] = mouse[1] = size.cy;
	}
}

void MultiStage::LeftUp(Point pt, dword keyflags) {
	mouse[2] = -1;
	mouse[3] = -1;
}

void MultiStage::MouseMove(Point pt, dword keyflags) {
	if (geometry[0] > 0.1 && geometry[1] > 0.1) {
		mouse[0] =               geometry[2] + pt.x;
		mouse[1] = geometry[1] - geometry[3] - pt.y;
	}
	else {
		mouse[0] = 0;
		mouse[1] = size.cy;
	}
}

void MultiStage::Paint() {
	Time now = GetSysTime();
	
	for(int i = 0; i < passes.GetCount(); i++) {
		Stage& pass = passes[i];
		GLuint gl_stage = gl_stages[i];
		
		GLint& fg_prog = pass.prog[Stage::PROG_FRAGMENT];
		if (fg_prog < 0)
			continue;
		
		GLint prog = fg_prog;
		GLint uindex;
		
		int& bi = pass.buf_i;
		if (pass.is_doublebuf)
			bi = (bi + 1) % 2;
		
		if (pass.is_buffer) {
			ASSERT(pass.frame_buf[bi] > 0);
			const GLenum bufs[] = {GL_COLOR_ATTACHMENT0_EXT};
			// combine FBO
		    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, pass.frame_buf[bi]);
		    
		    // set up render target
		    glDrawBuffers(sizeof bufs / sizeof bufs[0], bufs);
		}
		/*else {
			ASSERT(pass.color_buf > 0);
			// activate texturemap
		    glActiveTexture(GL_TEXTURE0);
		    glBindTexture(GL_TEXTURE_2D, pass.color_buf);
		}*/
	    
	    glBindProgramPipeline(gl_stage);
		glUseProgram(prog);
		
		if (frames % 100 == 0) {
			double elapsed = frame_time.Seconds();
			fprintf(stderr, "FPS: %.2f\n", 1.0 / elapsed);
		}
		
		uindex = glGetUniformLocation(prog, "iResolution");
		if (uindex >= 0) {
			if (geometry[0] > 0.1 && geometry[1] > 0.1)
				glUniform3f(uindex, geometry[0], geometry[1], 1.0);
			else
				glUniform3f(uindex, size.cx, size.cy, 1.0);
		}
		
		uindex = glGetUniformLocation(prog, "iTime");
		if (uindex >= 0) {
			float t = total_time.Seconds();
			glUniform1f(uindex, t);
		}
		
		uindex = glGetUniformLocation(prog, "iTimeDelta");
		if (uindex >= 0) {
			float t = frame_time.Seconds();
			glUniform1f(uindex, t);
		}
		
		uindex = glGetUniformLocation(prog, "iFrame");
		if (uindex >= 0) {
			glUniform1i(uindex, frames);
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
							geometry[2],
							geometry[1] - size.cy - geometry[3]);
			} else {
				glUniform2f(uindex, 0.0, 0.0);
			}
		}
		
		uindex = glGetUniformLocation(prog, "iChannel0");
		if (uindex >= 0) {
			if (pass.in.GetCount() >= 1) {
				int tex = GetInputTex(pass, 0);
				glActiveTexture(GL_TEXTURE0 + 0);
				glBindTexture(GL_TEXTURE_2D, tex);
				glUniform1i(uindex, GL_TEXTURE0 + 0);
			}
		}
		
		uindex = glGetUniformLocation(prog, "iChannel1");
		if (uindex >= 0) {
			if (pass.in.GetCount() >= 2) {
				int tex = GetInputTex(pass, 1);
				glActiveTexture(GL_TEXTURE0 + 1);
				glBindTexture(GL_TEXTURE_2D, tex);
				glUniform1i(uindex, GL_TEXTURE0 + 1);
			}
		}
		
		uindex = glGetUniformLocation(prog, "iChannel2");
		if (uindex >= 0) {
			if (pass.in.GetCount() >= 3) {
				int tex = GetInputTex(pass, 2);
				glActiveTexture(GL_TEXTURE0 + 2);
				glBindTexture(GL_TEXTURE_2D, tex);
				glUniform1i(uindex, GL_TEXTURE0 + 2);
			}
		}
		
		uindex = glGetUniformLocation(prog, "iChannel3");
		if (uindex >= 0) {
			if (pass.in.GetCount() >= 4) {
				int tex = GetInputTex(pass, 3);
				glActiveTexture(GL_TEXTURE0 + 3);
				glBindTexture(GL_TEXTURE_2D, tex);
				glUniform1i(uindex, GL_TEXTURE0 + 3);
			}
		}
		
		uindex = glGetUniformLocation(prog, "iFrameRate");
		if (uindex >= 0) {
			glUniform1f(uindex, fps_limit);
		}
		
		uindex = glGetUniformLocation(prog, "iChannelTime");
		if (uindex >= 0) {
			TODO
		}
		
		uindex = glGetUniformLocation(prog, "iChannelResolution");
		if (uindex >= 0) {
			TODO
		}
		
		
		glClear(GL_COLOR_BUFFER_BIT);
		glRectf(-1.0, -1.0, 1.0, 1.0);
		
		
		if (pass.frame_buf[bi] > 0) {
			// backup render target
		    glDrawBuffer(GL_FRONT);
		    
		    // reset FBO
		    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
		}
	}
	
	
	frames++;
	frame_time.Reset();
}

void MultiStage::SetInputCount(int pass, int count) {
	passes[pass].in.SetCount(count);
}

/*void MultiStage::SetOutputCount(int pass, int count) {
	passes[pass].out.SetCount(count);
}*/

void MultiStage::RealizeCount(int pass, int in) {
	if (pass >= passes.GetCount())
		passes.SetCount(pass+1);
	Stage& s = passes[pass];
	
	if (in  >= (int)s.in.GetCount())
		s.in.SetCount(in+1);
	
	//if (out >= (int)s.out.GetCount())
	//	s.out.SetCount(out+1);
}

void MultiStage::SetPassCommon(int pass, bool b) {
	passes[pass].is_common = b;
}

void MultiStage::SetPassCode(int pass, String vtx_glsl, String frag_glsl) {
	auto& s = passes[pass];
	s.vx_glsl = vtx_glsl;
	s.fg_glsl = frag_glsl;
}

void MultiStage::SetInputId(int pass, int i, int id) {
	passes[pass].in[i].id = id;
}

void MultiStage::SetInputType(int pass, int i, int type) {
	passes[pass].in[i].type = type;
}

void MultiStage::SetInputFilename(int pass, int i, String filename) {
	passes[pass].in[i].filename = filename;
}

void MultiStage::SetInputValue(int pass, int i, int filter, int wrap, int vflip) {
	auto& in = passes[pass].in[i];
	if (filter >= 0)
		in.filter = filter;
	if (wrap >= 0)
		in.repeat = wrap;
	if (vflip >= 0)
		in.vflip = vflip;
}

void MultiStage::SetOutputId(int pass, int id) {
	ASSERT(passes[pass].id < 0);
	passes[pass].id = id;
}

int MultiStage::GetPassCount() const {
	return passes.GetCount();
}

Stage& MultiStage::GetStageById(int id) const {
	for (Stage& s : passes)
		if (s.id == id)
			return s;
	throw Exc("Stage not found");
}

void MultiStage::UpdateTexBuffers() {
	Ogl_UpdateTexBuffers();
}

int MultiStage::GetInputTex(Stage& cur_stage, int input_i) const {
	StageInput& in = cur_stage.in[input_i];
	int tex;
	if (in.type == INPUT_BUFFER) {
		Stage& in_stage = GetStageById(in.id);
		int buf_i = in_stage.buf_i;
		if (&in_stage == &cur_stage)
			buf_i = (buf_i + 1) % 2;
		ASSERT(in_stage.color_buf[buf_i] > 0);
		tex = in_stage.color_buf[buf_i];
	}
	else {
		tex = in.tex;
	}
	ASSERT(tex != 0);
	return tex;
}












void Stage::ClearTex() {
	for(int bi = 0; bi < 2; bi++) {
		GLuint& color_buf = this->color_buf[bi];
		GLuint& depth_buf = this->depth_buf[bi];
		GLuint& frame_buf = this->frame_buf[bi];
		
		if (color_buf >= 0) {
			GLuint i[2] = {color_buf, depth_buf};
			glDeleteTextures(2, i);
			glDeleteFramebuffers(1, &frame_buf);
			color_buf = 0;
			depth_buf = 0;
			frame_buf = 0;
		}
	}
}

NAMESPACE_SHADER_END
