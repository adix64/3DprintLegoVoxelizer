//-------------------------------------------------------------------------------------------------
// Descriere: header in care este implementat un framebuffer
//
// Note:
//		TBD
//
// Autor: Lucian Petrescu
// Data: 18 Oct 2013
//-------------------------------------------------------------------------------------------------

#pragma once
#include <include/gl.h>
#include <iostream>
#include <string>
#include <vector>

namespace lab{
	class Framebuffer{

	
	private:
		//variabile
        unsigned int width, height;
		unsigned int framebuffer_object;
		unsigned int texture_color;
		unsigned int texture_depth;			//ca sa poata rula z-test trebuie sa existe un buffer de adancime!
		
	public:
		//constructor
		Framebuffer(){
			generate(1024,1024);
		}
		//destructor
		~Framebuffer(){
			destroy();
		}

	//private:
		//distruge framebufferul si texturile 
		void destroy(){
			glDeleteFramebuffers(1, &framebuffer_object);
			glDeleteTextures(1, &texture_color);
			glDeleteTextures(1, &texture_depth);
		}

	public:
		//functie ce genereaza tot
		void generate(unsigned int width, unsigned int height){
            this->width = width;
            this->height = height;

			//genereaza un obiect de tip framebuffer si apoi leaga-l la pipeline
			glGenFramebuffers(1, &framebuffer_object);
			glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_object);

			//genereaza textura de culoare, format RGBA8 (4 canale), FARA date, filtrare biliniara
			glGenTextures(1, &texture_color);
			glBindTexture(GL_TEXTURE_2D, texture_color);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
			glTexImage2D(GL_TEXTURE_2D, 0,GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

			//genereaza textura de adancime, format DEPTH (un singur canal), FARA date, filtrare bilininara
			glGenTextures(1, &texture_depth);
			glBindTexture(GL_TEXTURE_2D, texture_depth);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
			glTexImage2D(GL_TEXTURE_2D, 0,GL_DEPTH_COMPONENT24, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);

			//leaga texturi la framebuffer , 0 de la sfarsit se refera la ce nivel din mipmap, 0 fiind cel mai de sus/mare.
			unsigned int attachment_index_color_texture = 0;
			glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0+attachment_index_color_texture, texture_color,0);
			glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, texture_depth,0);

			//comanda care spune care sunt atasamentele 
			std::vector<GLenum> drawbuffers;
			drawbuffers.push_back(GL_COLOR_ATTACHMENT0+attachment_index_color_texture);
			glDrawBuffers(drawbuffers.size(),&drawbuffers[0]);

			//verifica stare
			if(glCheckFramebufferStatus(GL_FRAMEBUFFER)!=GL_FRAMEBUFFER_COMPLETE){
				std::cout<<"EROARE!!! Framebuffer-ul nu este complet. Apasati orice tasta pentru a inchide programul."<<std::endl;
				std::cin.get();
				std::terminate();
			}

			//nu sunt legat la pipeline
			glBindFramebuffer(GL_FRAMEBUFFER,0);
		}

		//returneaza textura de culoare din framebuffer
		unsigned int getColorTexture(){
			return texture_color;
		}
	
		//returneaza textura de adancime din framebuffer
		unsigned int getDepthTexture(){
			return texture_depth;
		}

		//reshape
		void reshape(unsigned int width, unsigned int height){
			destroy();
			generate(width, height);
		}

        //dimensiunea x a framebufferului
        unsigned int GetWidth(){ return width; }

        //dimensiunea y a framebufferului
        unsigned int GetHeight(){return height;}

		//leaga framebuffer la pipeline
		void bind(){
			glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_object);
		}

		//dezleaga framebuffer de la pipeline
		void unbind(){
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}
	};
}