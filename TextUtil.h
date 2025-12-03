#ifndef TEXT_UTILS
#define TEXT_UTILS

#ifndef __cplusplus
#define __cplusplus
#endif

#include "wingdi.h"

unsigned long Text_Length(char *s)
{
	if (s==NULL) return 0;
	unsigned long r=0;
	char *k=s;
	while (k[r]) r++;
	return r;
}

char * Text_Text(char * s)
{
	if (s==NULL) 
	{
		char*dd=(char*)malloc(1);
		dd[0]=0;
		return dd;
	}
	unsigned long l=Text_Length(s);
	char *k=(char*)malloc(l+1);
	memcpy(k,s,l+1);
	k[l]=0;
	return k;
}

char * IntToStr(int i)
{
	if (i==0)
	{
		char * s=(char*)malloc(2);
		s[0]='0';s[1]=0;
		return s;
	}
	else
	{
		char * s;
		if (i>0)
		{
			int l=0,i1=i;
			while (i1!=0)
			{
				i1=i1/10;
				l++;
			}

			s=(char*)malloc(l+1);
			s[l]=0;
			
			i1=i;
			int k=l-1;
			while (i1!=0)
			{
				s[k]='0'+(i1 % 10);
				i1=i1/10;
				k--;				
			}
			
			return s;
		}
		else
		{
			int l=0,i1=-i;
			while (i1!=0)
			{
				i1=i1/10;
				l++;
			}

			s=(char*)malloc(l+2);
			s[l+1]=0;
			
			i1=-i;
			int k=l;
			while (i1!=0)
			{
				s[k]='0'+(i1 % 10);
				i1=i1/10;
				k--;				
			}

			s[0]='-';
			
			return s;
		}
	}
}

char * Text_Concat(char * s1,char*s2)
{
	if ((s1==NULL) & (s2==NULL)) return NULL;
	if (s1==NULL) return Text_Text(s2);
	if (s2==NULL) return Text_Text(s1);

	int l1=Text_Length(s1);
	int l2=Text_Length(s2);

	char * s3=(char*)malloc(l1+l2+1);
	memcpy(s3,s1,l1);
	memcpy(&s3[l1],s2,l2);
	s3[l1+l2]=0;

	return s3;
}

char * GetPath(char * name)
{
	int fin=0;
	int i;
	for(i=strlen(name)-1;i>=0;i--)
	{
		if (name[i]=='/' || name[i]==92)
		{
			fin=i;		
			break;
		}
	}
	char * Path;
	if(fin!=0)
	{
		Path=(char *)malloc(fin+2);
		Path[fin]='/';
		Path[fin+1]='\0';
		for(i=0;i<fin;i++)
			Path[i]=name[i];
	}
	else
	{
		Path=(char*)malloc(1);
		Path[0]='\0';
	}
	return Path;
}


/*
//////////////////////////////////////////////////////////////////////////
// une Classe qui permet de contenir et manipuler facilement des chaines 
// de caractère
//////////////////////////////////////////////////////////////////////////
typedef class CSTRING
{
private:
	bool loaded; 
	char * info;
public:
	
	void Assign(char* c);	
	char* Value() ;	
	void Concat(char* c);
	
	CSTRING();

	~CSTRING();

} CSTRING,*PCSTRING;


CSTRING::CSTRING()
{
	this->loaded=false;
	this->info=NULL;
}

CSTRING::~CSTRING()
{
	if (this!=NULL)
	{
		if (this->loaded)
		{
			if (this->info!=NULL)
			{
				free(this->info);
				this->info=NULL;
				this->loaded=false;
			}
		}
	}
}

void CSTRING::Assign(char* c)
{
	if (this==NULL) return;
	if (loaded) free(info);
	info=Text_Text(c);
	loaded=true;		
}

char* CSTRING::Value() 
{ 
	if (this==NULL) return NULL;
	if (loaded)
		return info;
	else return NULL;
}

void CSTRING::Concat(char* c)
{
	if (c==NULL) return;
	if (this==NULL) return;
	if ((loaded) & (this->info!=NULL))
	{
		char*old=info;
		info=Text_Concat(info,c);
		free(old);
	}
	else
	{
		info=Text_Text(c);			
	}

	loaded=true;	
}
*/



	





#endif