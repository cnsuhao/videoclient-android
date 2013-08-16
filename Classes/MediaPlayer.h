#ifndef __MEDIA_PLAYER_H_
#define __MEDIA_PLAYER_H_
#include <cocos2d.h>

class MediaPlayer : public cocos2d::CCSprite
{
public:
	enum EnumDraw
	{
		RGB=0,
		YUVTORGB,
		YUV,
	};
	MediaPlayer(void);
	virtual ~MediaPlayer(void);

	bool initGL(EnumDraw drawType);
	void closeGL(void);

	void refreshData(const char *lpBuffer, size_t nSize);
	virtual void draw(void);
protected:
	GLuint compileShader(GLenum type, const char *sources);
	GLuint buildProgram(const char* vertexShaderSource,  const char* fragmentShaderSource);

	GLuint bindTexture(GLuint texture, const char *buffer, GLuint w , GLuint h);
protected:
	EnumDraw drawTy_;
	int		nImageWidth_;
	int		nImageHeight_;
	char	*lpBuffer_;
	size_t	nBufSize_;
	GLuint	program_;
	GLuint	texYID_;
	GLuint	texUID_;
	GLuint	texVID_;
};
#endif
