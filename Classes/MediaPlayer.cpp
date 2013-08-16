#include "MediaPlayer.h"

enum 
{  
	ATTRIB_VERTEX,  
	ATTRIB_TEXTURE,  
};
static const char* VERTEX_SHADER =    
	"varying mediump vec2 tc; \n"
	"attribute vec4 vPosition; \n"
	"attribute vec2 a_texCoord; \n"
	"void main() { \n"
	"   tc = a_texCoord; \n"
	"   gl_Position = vPosition; \n"
	"}\r\n";
static const char* FRAG_SHADER =  
	"varying lowp vec2 tc;\n"  
	"uniform sampler2D SamplerY;\n"  
	"uniform sampler2D SamplerU;\n"  
	"uniform sampler2D SamplerV;\n"  
	"void main(void)\n"  
	"{\n"  
	"mediump vec3 yuv;\n"  
	"lowp vec3 rgb;\n"  
	"yuv.x = texture2D(SamplerY, tc).r;\n"  
	"yuv.y = texture2D(SamplerU, tc).r - 0.5;\n"  
	"yuv.z = texture2D(SamplerV, tc).r - 0.5;\n"  
	"rgb = mat3(1, 1, 1,\n"
	"0, -0.39465, 2.03211,\n"
	"1.13983, -0.58060, 0) * yuv;\n"
	"gl_FragColor = vec4(rgb, 1);\n"
	"}\n";
#define min(a,b)            (((a) < (b)) ? (a) : (b))
using namespace cocos2d;
MediaPlayer::MediaPlayer(void) : nImageWidth_(0), nImageHeight_(0), lpBuffer_(NULL), nBufSize_(0),
	program_(0), texYID_(0), texUID_(0), texVID_(0)
{
}


MediaPlayer::~MediaPlayer(void)
{
	closeGL();
}

GLuint MediaPlayer::compileShader(GLenum type, const char *sources)
{
	GLuint shader = glCreateShader(type);
	if (shader == 0 || shader == GL_INVALID_ENUM) 
	{
		return 0;
	}

	glShaderSource(shader, 1, &sources, NULL);
	glCompileShader(shader);

	GLint status;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE) 
	{
		glDeleteShader(shader);
		return 0;
	}

	return shader;
}

GLuint MediaPlayer::buildProgram(const char* vertexShaderSource,  const char* fragmentShaderSource)
{
	GLuint vertexShader = compileShader(GL_VERTEX_SHADER, VERTEX_SHADER);
 	GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, FRAG_SHADER);
	if (vertexShader == 0 || fragmentShader == 0)
	{
		CCLog("compile shader is failed\r\n");
		return 0;
	}

	GLuint programHandle = glCreateProgram();
	glAttachShader(programHandle, vertexShader);  
 	glAttachShader(programHandle, fragmentShader);  

	if (glGetError() != GL_FALSE) 
	{
		CCLog("attach shader to program is failed");
		glDeleteProgram(programHandle);
		return 0;
	}

	glLinkProgram(programHandle);
	if (glGetError() != 0) 
	{
		CCLog("link program is failed");
		glDeleteProgram(programHandle);
		return 0;
	}

	return programHandle;
}

void MediaPlayer::closeGL(void)
{
	glDeleteProgram(program_);

	if (lpBuffer_)
	{
		delete []lpBuffer_;
		lpBuffer_ = NULL;
	}
	nImageWidth_ = 0;
	nImageHeight_ = 0;
	nBufSize_ = 0;
}

bool MediaPlayer::initGL(EnumDraw drawType)
{
	closeGL();

	CCSize size = CCDirector::sharedDirector()->getWinSize();
	nImageWidth_ = size.width;
	nImageHeight_ = size.height;
	drawTy_ = drawType;

	if (drawType == RGB)
		nBufSize_ = nImageWidth_*nImageHeight_*3;
	else
		nBufSize_ = nImageWidth_*nImageWidth_*3/2;

	lpBuffer_ = new char[nBufSize_];
	memset(lpBuffer_, 0, nBufSize_);

	if (drawType != RGB)
	{
		program_ = buildProgram(VERTEX_SHADER, FRAG_SHADER);

		if (program_ == 0)
		{
			CCLog("build program handler is failed");
			return false;
		}
		glUseProgram(program_);

		glGenTextures(1, &texUID_);
		glGenTextures(1, &texVID_);

		if (glGetError() != 0)
		{
			CCLog("get textures is failed");
			return false;
		}
	}
	glGenTextures(1, &texYID_);

	return true;
}

GLuint MediaPlayer::bindTexture(GLuint texture, const char *buffer, GLuint w , GLuint h)
{
	glBindTexture (GL_TEXTURE_2D, texture);
	if (glGetError() != 0)
	{
		CCLog("bind texture is failed");
		return 0;
	}

	long gl_type = GL_LUMINANCE;
	drawTy_ == RGB ? gl_type = GL_RGB : 0;

	glTexImage2D ( GL_TEXTURE_2D, 0, GL_LUMINANCE, w, h, 0, gl_type, GL_UNSIGNED_BYTE, buffer);  
	glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );  
	glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR ); 
	glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
	glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
	if (glGetError() != 0)
	{
		CCLog("set texture's paramet is failed");
		return 0;
	}

	return texture;  
}

void MediaPlayer::refreshData(const char *lpBuffer, size_t nSize)
{
	size_t nMin = min(nSize, nBufSize_);
	memmove(lpBuffer_, lpBuffer, nMin);
}


void MediaPlayer::draw(void)
{
	if (lpBuffer_ == NULL)
		return;

	static GLfloat squareVertices[] = 
	{  
		0.0f, 0.0f,
		1.0f, 0.0f,
		0.0f,  1.0f,  
		1.0f,  1.0f,  
	};  
	static GLfloat coordVertices[] = 
	{  
		-1.0f, 1.0f,  
		1.0f, 1.0f,  
		-1.0f,  -1.0f,  
		1.0f,  -1.0f,  
	};
	static const GLfloat texCoords[] = 
	{ 
		0.0,1.0, 
		1.0,1.0, 
		0.0,0.0, 
		1.0,0.0 
	}; 
	
	glViewport(0, 0, nImageWidth_, nImageHeight_);  
	glClearColor(0.0f, 0.0f, 0.0f, 1); 
	glClear(GL_COLOR_BUFFER_BIT);

	bindTexture(texYID_, lpBuffer_, nImageWidth_, nImageHeight_);
	if (drawTy_ != RGB)
	{
		bindTexture(texUID_, lpBuffer_ + nImageWidth_ * nImageHeight_, nImageWidth_/2, nImageHeight_/2);  
		bindTexture(texVID_, lpBuffer_ + nImageWidth_ * nImageHeight_*5/4, nImageWidth_/2, nImageHeight_/2);

		GLuint tex_y = glGetUniformLocation(program_, "SamplerY");  
		GLuint tex_u = glGetUniformLocation(program_, "SamplerU");  
		GLuint tex_v = glGetUniformLocation(program_, "SamplerV");
		if (glGetError() != 0)
		{
			CCLog("get uniform location is failed,ID is %d", glGetError());
			return;
		}

		glBindAttribLocation(program_, ATTRIB_VERTEX, "vPosition");  
		glBindAttribLocation(program_, ATTRIB_TEXTURE, "a_texCoord");  
		if (glGetError() != 0)
		{
			CCLog("bind attrib location is failed,ID is %d", glGetError());
			return;
		}

		glVertexAttribPointer(ATTRIB_VERTEX, 2, GL_FLOAT, 0, 0, squareVertices);  
		glEnableVertexAttribArray(ATTRIB_VERTEX);  

		glVertexAttribPointer(ATTRIB_TEXTURE, 2, GL_FLOAT, 0, 0, coordVertices);  
		glEnableVertexAttribArray(ATTRIB_TEXTURE);  
		if (glGetError() != 0)
		{
			CCLog("vertex attrib pointer is failed,ID is %d", glGetError());
			return;
		}

		glActiveTexture(GL_TEXTURE0);  
		glBindTexture(GL_TEXTURE_2D, texYID_);  
		glUniform1i(tex_y, 0);  

		glActiveTexture(GL_TEXTURE1);  
		glBindTexture(GL_TEXTURE_2D, texUID_);  
		glUniform1i(tex_u, 1);  

		glActiveTexture(GL_TEXTURE2);  
		glBindTexture(GL_TEXTURE_2D, texVID_);  
		glUniform1i(tex_v, 2);
	}
	else
	{
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glTexCoordPointer(2, GL_FLOAT, 0, squareVertices);
	}

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}
