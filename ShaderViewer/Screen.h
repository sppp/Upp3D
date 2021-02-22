#if 0
#ifndef _ShaderViewer_Screen_h_
#define _ShaderViewer_Screen_h_


NAMESPACE_SPPP_BEGIN


#if		defined flagSDL
#elif	defined flagGLUT
#else
	#define flagGLUT
#endif

struct StdPassInput {
	int id = -1;
	int type = -1;
	int repeat = 1, filter = 1;
	bool vflip = 0;
	std::string filename;
	GLenum tex;
	
};

struct StdPassOutput {
	int id = -1;
};

struct StdPass {
	Array<StdPassInput>			in;
	Array<StdPassOutput>		out;
	String						code;
	GLint						prog = -1;
	GLuint						color_buf = 0;
	GLuint						depth_buf = 0;
	GLuint						frame_buf = 0;
	bool						is_common = false;
	bool						is_buffer = false;
	
	~StdPass() {ClearTex();}
	void ClearTex();
};



class StdScreen : public IScreen {
	TimeStop frame_time, total_time;
	int cur_w = 0, cur_h = 0;
	double geometry[4] = {0,0,0,0};
	double mouse[4] = {0,0,0,0};
	int frames = 0;
	
	Array<StdPass> passes;
	const char* last_error = 0;
	int last_time = 0;
	
	int LoadTexture(std::string filename, GLenum type, GLenum *tex_id, char filter, char repeat, bool flip);
	GLint CompileShader(const GLenum shader_type, std::string shader_source);
	GLint LinkProgram(std::string shader_source);
	void UpdateTexBuffers(int w, int h);
	
	void InitGlew();
	void UpdatePixelBuffers();

public:
	StdScreen();
	
	bool Init(const String& arg) override;
	bool Open() override;
	void Close() override;
	void Destroy() override;
	void EventLoop() override;
	
	void GetSize(int* w, int* h) const override;
	void GetCursor(int* x, int* y) const override;
	
	void SetUpdateInterval(int value) override;
	void Paint() override;
	void MousePressHandler(int button, int state, int x, int y) override;
	void MouseMoveHandler(int x, int y) override;
	void KeyboardHandler(unsigned char key, int x, int y) override;
	
	
	void SetPassCount(int count);
	void SetInputCount(int pass, int count);
	void SetOutputCount(int pass, int count);
	void RealizeCount(int pass, int in, int out);
	void SetPassCommon(int pass, bool b);
	void SetPassCode(int pass, const char* code);
	void SetInputId(int pass, int i, int id);
	void SetInputType(int pass, int i, int type);
	void SetInputFilename(int pass, int i, const char* filename);
	void SetOutputId(int pass, int i, int id);
	int GetPassCount() const;

	
	enum {
		INPUT_INVALID,
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
};


class Ctrl {};


class Screen : public Ctrl {
	One<StdScreen> s;
	
public:
	Screen();
	
	void Run();
	bool OpenToyShader(String path);
	void RemoveToken(String& glsl, String token);
	
	
};


NAMESPACE_SPPP_END


#endif
#endif
