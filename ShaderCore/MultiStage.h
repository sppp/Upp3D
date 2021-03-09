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
	Size res = {0,0};
	int vol_depth = 0;
	const VolatileStream* stream = 0;
	
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
	
	enum {
		TYPE_IMAGE_BUFFER,
		TYPE_SOUND_BUFFER,
		TYPE_IMAGE,
		TYPE_SOUND,
		TYPE_VERTEX,
		TYPE_CTRL,
		TYPE_LIBRARY,
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
	int							type = TYPE_IMAGE_BUFFER;
	bool						is_doublebuf = false;
	
	~Stage() {ClearTex();}
	void ClearTex();
	
	bool IsLibrary() const {return type == TYPE_LIBRARY;}
	
	bool operator()(const Stage& a, const Stage& b) const {
		#define GREATER(x) if (a.type == x) return false; if (b.type == x) return true;
		GREATER(TYPE_SOUND); // sound is faster to calc, so the sync is better by doing sound last
		GREATER(TYPE_SOUND_BUFFER);
		GREATER(TYPE_IMAGE);
		GREATER(TYPE_IMAGE_BUFFER);
		GREATER(TYPE_VERTEX);
		GREATER(TYPE_CTRL);
		GREATER(TYPE_LIBRARY);
		return StdLess<String>()(a.name, b.name);
	}
	
	String GetStageTypeString() const {
		switch (type) {
			case TYPE_IMAGE_BUFFER:	return "Image Buffer";
			case TYPE_SOUND_BUFFER: return "Sound Buffer";
			case TYPE_IMAGE:		return "Image";
			case TYPE_SOUND:		return "Sound";
			case TYPE_VERTEX:		return "Vertex";
			case TYPE_CTRL:			return "Ctrl";
			case TYPE_LIBRARY:		return "Library";
		}
		return "<invalid type>";
	}
};

class MultiStage {
	struct PipelineStream : public VolatileStream {
		double total_seconds = 0;
		double frame_seconds = 0;
		Size size;
		
		PipelineStream() : size(0,0) {}
		void Clear() {total_seconds = 0; frame_seconds = 0;}
		double GetSeconds() const override {return total_seconds;}
		Size GetResolution() const override {return size;}
	};
	
	struct VideoInput : Moveable<VideoInput> {
		MultiStage* ms = 0;
		String path;
		MediaStream* cap = 0;
		GLuint cap_tex[2] = {0,0};
		int cap_tex_i = 0;
		RunningFlagSingle flag;
		Vector<GLuint*> tgt_tex;
		TimeStop step_time;
		
		
		typedef VideoInput CLASSNAME;
		VideoInput() {}
		~VideoInput() {Stop(); Clear();}
		
		void Clear();
		void Start();
		void Stop();
		void Process();
		void PaintOpenGL();
	};
	struct DataBuffer : Moveable<DataBuffer> {
		GLuint tex = 0;
		Vector<GLuint*> tgt_tex;
		Vector<byte> data;
		bool changed = false;
		
		~DataBuffer() {Clear();}
		void Clear();
		void PaintOpenGL();
	};
	enum {
		DATA_IN_KEYBOARD,
		DATA_IN_MIDI,
	};
	
	PipelineStream stream;
	
	TimeStop frame_time, total_time;
	double geometry[4] = {0,0,0,0};
	double mouse[4] = {0,0,0,0};
	int frames = 0;
	int fps_limit = 60;
	bool is_left_down = false;
	
	Vector<uint32> gl_stages;
	Array<Stage> passes;
	String name;
	String description;
	String last_error;
	int last_time = 0;
	bool is_open = false;
	
	// Sound settings
	int audio_sample_size = 512;
	Vector<vec2> sound_buf;
	
	// Video device input
	Array<VideoInput> vid_inputs;
	MediaDeviceManager vidmgr;
	Size def_cap_sz;
	int def_cap_fps;
	
	// Video file input
	
	// Keyboard input
	static const int key_tex_w = 256;
	static const int key_tex_h = 256;
	ArrayMap<int, DataBuffer> data_bufs;
	
	
	int   Ogl_NewTexture(Size res, GLuint* tex, int tex_n, GLenum type, char filter, char repeat);
	int   Ogl_LoadTexture(String filename, GLenum type, StageInput& in, char filter, char repeat, bool flip);
	int   Ogl_LoadVolume(String filename, GLenum *tex_id, char filter, char repeat, bool flip);
	int   Ogl_LoadCubemap(String filename, GLenum *tex_id, char filter, char repeat, bool flip);
	GLint Ogl_CompileShader(const GLenum shader_type, String shader_source);
	bool  Ogl_CompileProgram(Stage& s, String shader_source);
	bool  Ogl_LinkProgram(Stage& s);
	void  Ogl_UpdateTexBuffers();
	void  Ogl_TexFlags(int type, int filter, int repeat);
	void  Ogl_CreateTex(Stage& s, Size sz, int channels);
	void  UpdateTexBuffers();
	int   GetInputTex(Stage& cur_stage, int input_i) const;
	int   GetTexType(Stage& cur_stage, int input_i) const;
	int   GetChCode(int channels, bool is_float=false);
	void  StartMediaThreads();
	void  StopMediaThreads();
	
	bool CheckInputTextures();
	
	bool OpenMedia(String path);
	
protected:
	
	static const int CHANNEL_COUNT = 4;
	
	
	void SetInputCount(int pass, int count);
	//void SetOutputCount(int pass, int count);
	void RealizeCount(int pass, int in);
	void SetPassType(int pass, int type);
	void SetPassCode(int pass, String vtx_glsl, String frag_glsl);
	void SetInputId(int pass, int i, int id);
	void SetInputType(int pass, int i, int type);
	void SetInputFilename(int pass, int i, String filename);
	void SetInputValue(int pass, int i, int filter, int wrap, int vflip);
	void SetOutputId(int pass, int id);
	
	int GetPassCount() const;
	Stage& GetStageById(int id) const;
	VideoInput* FindVideoInput(String path);
	
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
	
	MultiStage() : def_cap_sz(1280,720), def_cap_fps(30) {}
	~MultiStage() {Close();}
	
	bool Open(Size output_sz);
	bool Load(String toy_path);
	void Close();
	void DumpStages();
	
	void Paint();
	void Event(const CtrlEvent& e);
	void LeftDown(Point p, dword keyflags);
	void LeftUp(Point p, dword keyflags);
	void MouseMove(Point p, dword keyflags);
	bool Key(dword key, int count);
	
	void SetSize(Size sz);
	void SetFPS(int fps) {fps_limit = fps;}
	
	String GetLastError() const {return last_error;}
	Size GetSize() const {return stream.size;}
	bool IsOpen() const {return is_open;}
	
};

NAMESPACE_SHADER_END

#endif
