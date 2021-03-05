#ifndef _ShaderCore_MultiStage_h_
#define _ShaderCore_MultiStage_h_

NAMESPACE_SHADER_BEGIN

struct StageInput {
	int id = -1;
	int type = -1;
	int repeat = REPEAT_REPEAT, filter = FILTER_LINEAR;
	bool vflip = 0;
	String filename;
	GLenum tex = 0;
	
	enum {
		FILTER_NEAREST,
		FILTER_LINEAR,
		FILTER_MIPMAP
	};
	
	enum {
		REPEAT_CLAMP,
		REPEAT_REPEAT,
	};
	
	String GetFilterString() const {
		switch (filter) {
			case FILTER_NEAREST: return "nearest";
			case FILTER_LINEAR: return "linear";
			case FILTER_MIPMAP: return "mipmap";
		}
		return "invalid";
	}
	String GetRepeatString() const {
		switch (repeat) {
			case REPEAT_CLAMP: return "clamp";
			case FILTER_LINEAR: return "repeat";
		}
		return "invalid";
	}
};

/*struct StageOutput {
	int id = -1;
};*/

struct Stage {
	Array<StageInput>			in;
	//Array<StageOutput>		out;
	
	enum {
		PROG_VERTEX,
		PROG_FRAGMENT,
		PROG_GEOMETRY,
		PROG_TESS_CONTROL,
		PROG_TESS_EVALUATION,
		
		PROG_COUNT
	};
	GLint						prog[PROG_COUNT] = {-1,-1};
	GLuint						color_buf[2] = {0,0};
	GLuint						depth_buf[2] = {0,0};
	GLuint						frame_buf[2] = {0,0};
	String						name;
	String						description;
	String						vx_glsl;
	String						fg_glsl;
	int							id = -1;
	int							buf_i = 0;
	bool						is_doublebuf = false;
	bool						is_common = false;
	bool						is_buffer = false;
	
	~Stage() {ClearTex();}
	void ClearTex();
	
	
	bool operator()(const Stage& a, const Stage& b) const {
		if (a.name == "Image") return 0;
		if (b.name == "Image") return 1;
		return StdLess<String>()(a.name, b.name);
	}
};

class MultiStage {
	TimeStop frame_time, total_time;
	double geometry[4] = {0,0,0,0};
	double mouse[4] = {0,0,0,0};
	int frames = 0;
	Size size;
	int fps_limit = 60;
	
	Vector<uint32> gl_stages;
	Array<Stage> passes;
	String name;
	String description;
	String last_error;
	int last_time = 0;
	bool is_open = false;
	
	VideoDeviceManager vidmgr;
	VideoCaptureDevice* cap = 0;
	GLuint cap_tex[2] = {0,0};
	int cap_tex_i = 0;
	Size def_cap_sz;
	int def_cap_fps;
	bool uses_webcam;
	RunningFlagSingle webcam_flag;
	Vector<GLuint*> tgt_tex;
	
	
	int   Ogl_LoadTexture(String filename, GLenum type, GLenum *tex_id, char filter, char repeat, bool flip);
	int   Ogl_LoadVolume(String filename, GLenum *tex_id, char filter, char repeat, bool flip);
	int   Ogl_LoadCubemap(String filename, GLenum *tex_id, char filter, char repeat, bool flip);
	GLint Ogl_CompileShader(const GLenum shader_type, String shader_source);
	bool  Ogl_CompileProgram(Stage& s, String shader_source);
	bool  Ogl_LinkProgram(Stage& s);
	void  Ogl_UpdateTexBuffers();
	void  Ogl_TexFlags(int type, int filter, int repeat);
	void  UpdateTexBuffers();
	int   GetInputTex(Stage& cur_stage, int input_i) const;
	int   GetTexType(Stage& cur_stage, int input_i) const;
	int   GetChCode(int channels);
	
	void StartWebcamThread();
	void StopWebcamThread();
	void ProcessWebcamThread();
	
protected:
	
	
	
	void SetInputCount(int pass, int count);
	//void SetOutputCount(int pass, int count);
	void RealizeCount(int pass, int in);
	void SetPassCommon(int pass, bool b);
	void SetPassCode(int pass, String vtx_glsl, String frag_glsl);
	void SetInputId(int pass, int i, int id);
	void SetInputType(int pass, int i, int type);
	void SetInputFilename(int pass, int i, String filename);
	void SetInputValue(int pass, int i, int filter, int wrap, int vflip);
	void SetOutputId(int pass, int id);
	
	int GetPassCount() const;
	Stage& GetStageById(int id) const;
	
	enum {
		INPUT_INVALID = -1,
		INPUT_TEXTURE,
		INPUT_CUBEMAP,
		INPUT_WEBCAM,
		INPUT_MUSIC,
		INPUT_MUSICSTREAM,
		INPUT_KEYBOARD,
		INPUT_VOLUME,
		INPUT_VIDEO,
		INPUT_BUFFER,
	};
	
	static String GetInputTypeString(int i) {
		switch(i) {
			case INPUT_TEXTURE:		return "texture";
			case INPUT_CUBEMAP:		return "cubemap";
			case INPUT_WEBCAM:		return "webcam";
			case INPUT_MUSIC:		return "music";
			case INPUT_MUSICSTREAM:	return "musicstream";
			case INPUT_KEYBOARD:	return "keyboard";
			case INPUT_VOLUME:		return "volume";
			case INPUT_VIDEO:		return "video";
			case INPUT_BUFFER:		return "buffer";
			default:
			case INPUT_INVALID: return "invalid";
		}
	}
public:
	typedef MultiStage CLASSNAME;
	
	MultiStage() : size(0,0), def_cap_sz(1280,720), def_cap_fps(30) {}
	~MultiStage() {Close();}
	
	bool Open(Size output_sz);
	bool Load(String toy_path);
	void Close();
	void DumpStages();
	
	void Paint();
	void LeftDown(Point p, dword keyflags);
	void LeftUp(Point p, dword keyflags);
	void MouseMove(Point p, dword keyflags);
	void KeyboardHandler(unsigned char key, int x, int y);
	
	void SetSize(Size sz) {if (is_open && size != sz) {size = sz; UpdateTexBuffers();}}
	void SetFPS(int fps) {fps_limit = fps;}
	
	String GetLastError() const {return last_error;}
	Size GetSize() const {return size;}
	bool IsOpen() const {return is_open;}
	
};

NAMESPACE_SHADER_END

#endif
