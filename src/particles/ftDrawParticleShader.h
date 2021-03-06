
#pragma once

#include "ofMain.h"
#include "ftShader.h"


namespace flowTools {
	
	class ftDrawParticleShader : public ftShader {
	public:
		ftDrawParticleShader() {
			bInitialized = 1;
			
			if (ofIsGLProgrammableRenderer())
				glThree();
			else
				glTwo();
			
			if (bInitialized)
				ofLogNotice("ftDrawParticleShader initialized");
			else
				ofLogWarning("ftDrawParticleShader failed to initialize");
		}
		
	protected:
		void glTwo() {
			vertexShader = GLSL120(
								   uniform sampler2DRect positionTexture;
								   uniform sampler2DRect ALMSTexture;
								   uniform float TwinkleSpeed;
								   uniform float ParticleColorR;
								   uniform float ParticleColorG;
								   uniform float ParticleColorB;
								   uniform float ParticleColorA;

								   
								   void main(){
									   
									   vec2 st = gl_Vertex.xy;
									   
									   vec2 texPos = texture2DRect(positionTexture, st).xy;
									   gl_Position = gl_ModelViewProjectionMatrix * vec4(texPos, 0.0, 1.0);
									   vec4 alms = texture2DRect(ALMSTexture, st);
									   float age = alms.x;
									   float life = alms.y;
									   float mass = alms.z;
									   float size = alms.w;
									   gl_PointSize = size;
									   
									   float alpha = min (0.5 - (age / life) * 0.5,age * 5.);
									   alpha *= 0.5 + (cos((age + size) * TwinkleSpeed * mass) + 1.0) * 0.5;
									   alpha = max(alpha, 0.0);
									   // change color to red
//									   gl_FrontColor = vec4(1.0,.0,.0, alpha);
									   
									   // change color to passing r g b  --------------------- my
									   gl_FrontColor = vec4(ParticleColorR,ParticleColorG,ParticleColorB,alpha);

									   
//									   gl_FrontColor = vec4(vec3(1.0), alpha);

								   }
								   );
			
			
			bInitialized *= shader.setupShaderFromSource(GL_VERTEX_SHADER, vertexShader);
			bInitialized *= shader.linkProgram();

			
		}
		
		void glThree() {
			vertexShader = GLSL150(
								   uniform	mat4 modelViewProjectionMatrix;
								   uniform	mat4 textureMatrix;
								   uniform	sampler2DRect PositionTexture;
								   uniform	sampler2DRect ALMSTexture;
								   uniform float ParticleColorR;
								   uniform float ParticleColorG;
								   uniform float ParticleColorB;
						
								   
								   in vec4	position;
								   in vec2	texcoord;
								   in vec4	color;
								   
								   uniform float TwinkleSpeed;
								   
								   out vec2 texCoordVarying;
								   out vec4 colorVarying;
								   
								   void main()
								   {
									   colorVarying = color;
									   texCoordVarying = (textureMatrix*vec4(texcoord.x,texcoord.y,0,1)).xy;
									   
									   
									   vec2 st = position.xy;
									   vec2 texPos = texture(PositionTexture, st).xy;
									   gl_Position = modelViewProjectionMatrix * vec4(texPos, 0.0, 1.0);
									   
									   vec4 alms = texture(ALMSTexture, st);
									   float age = alms.x;
									   float life = alms.y;
									   float mass = alms.z;
									   float size = alms.w;
									   gl_PointSize = size;
									   
									   float alpha = min (0.5 - (age / life) * 0.5,age * 5.);
									   alpha *= 0.5 + (cos((age + size) * TwinkleSpeed * mass) + 1.0) * 0.5;
									   alpha = max(alpha, 0.0);
									   
									   // change color to red
//									   colorVarying = vec4(1.0,.0,.0, alpha);
									   
									   // change color to passing r g b  --------------------- my
									   colorVarying = vec4(ParticleColorR,ParticleColorG,ParticleColorB,alpha);

									   
									   
									   //colorVarying = vec4(vec3(1.0), alpha);
								   }
								);
			
			
			
			// thanx to: http://mmmovania.blogspot.nl/2010/12/circular-point-sprites-in-opengl-33.html
			
			fragmentShader = GLSL150(
								  in vec4 colorVarying;
								  out vec4 fragColor;
								  void main()
								  {
									  vec2 p = gl_PointCoord * 2.0 - vec2(1.0);
									  float d = dot(p,p);
									  float r = sqrt(d);
									  
									  if(d > r)
										  discard;
									  else
										  // change color to red
										  //fragColor = colorVarying * (1.0, .0, .0, 1.0 - pow(r, 2.5));
										  fragColor = colorVarying * (1.0, 1.0, 1.0, 1.0 - pow(r, 2.5));
								  }
								  );
			
			bInitialized *= shader.setupShaderFromSource(GL_VERTEX_SHADER, vertexShader);
			bInitialized *= shader.setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
			bInitialized *= shader.bindDefaults();
			bInitialized *= shader.linkProgram();
		}
		
	public:
		
		// my add particle color into update
		void update(ofVboMesh &particleVbo, int _numParticles, ofTexture& _positionTexture, ofTexture& _ALMSTexture, float _twinkleSpeed,
			float _particleColorR,
			float _particleColorG,
			float _particleColorB){
			shader.begin();
			shader.setUniformTexture("PositionTexture", _positionTexture, 0);
			shader.setUniformTexture("ALMSTexture", _ALMSTexture, 1);
			shader.setUniform1f("TwinkleSpeed", _twinkleSpeed);
			
			// my
			shader.setUniform1f("ParticleColorR",_particleColorR/255.0);
			shader.setUniform1f("ParticleColorG",_particleColorG/255.0);
			shader.setUniform1f("ParticleColorB",_particleColorB/255.0);


			bool dinges = true;
			//glEnable(GL_POINT_SMOOTH);
			glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
			
			particleVbo.draw();
			
			glDisable(GL_VERTEX_PROGRAM_POINT_SIZE);
			//glDisable(GL_POINT_SMOOTH);
			shader.end();
			
		}
	};
}
