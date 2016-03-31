#include "/root/tz_uc_4.3/libs/aio2/aio2/aio2.h"
#include <vector>
#include <iostream>
#include <string>
#include <unistd.h>
#include <cstdio>
#include <cstdlib>
#include <fcntl.h>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>

using namespace aio2;
using namespace aio2::io;
using namespace aio2::codec;
using namespace std;

#define PARAM_ERR		100
#define FILE_NOT_EXIST	101
#define OPEN_SRC_ERROR	102
struct Arg_t{
	public:
		string srcFileName;
		string dstFileName;
		string dstPath;
		Arg_t():dstPath("./"){}
		void info(){
			cout<<"src:"<<srcFileName<<" dst:"<<getPathName()<<endl;
		}
		string getPathName(){ return dstPath+"/"+dstFileName;}

};

static Arg_t globalArgs;
static const char *optString = "i:o:d:h";

/* Display program usage, and exit.
 */
void display_usage( void )
{
	/* ... */
	printf( "\nuas2wav - convert audio format from .uas to .wav\n" );
	printf( "Usage:\n\t uas2wav -i srcFile [-o dstFile] [-d dir]\n"
			"Example:\n\t uas2wav -i test.uas -o test2.wav -d ./wav \n");
	exit( EXIT_FAILURE );
}

/* init globalArgs 
*/
int parseArgs(int argc, char* argv[]){
	int opt =-1;	
	opterr = 0; //使getopt不向stderr输出错误信息

	while( (opt = getopt( argc, argv, optString )) != -1 ) {
		switch( opt ) {
			case 'i':
				globalArgs.srcFileName = optarg; 
				break;
			case 'o':
				globalArgs.dstFileName = optarg; 
				break;
			case 'd':
				globalArgs.dstPath = optarg; 
				break;
			case 'h':
			case '?':
				display_usage();
				break;
			default:
				;//do nothing.
		}
	}

	if(globalArgs.srcFileName.empty())
		display_usage();	
	//check access of file.
	if((access(globalArgs.srcFileName.c_str(), F_OK)) == -1)   
	{   
		printf(" src file not exist, ");
		printf("exit code %d\n",FILE_NOT_EXIST);
		return FILE_NOT_EXIST;
	}

	if(globalArgs.dstFileName.empty())	{
		/* use default filename.(replace suffix to wav.) */
		int npos = globalArgs.srcFileName.find_last_of(".");
		if(npos > 0)
		{
			globalArgs.dstFileName = globalArgs.srcFileName.substr(0,npos);
			globalArgs.dstFileName += ".wav";
		}else
			globalArgs.dstFileName = globalArgs.srcFileName + ".wav";
	}

	if((opendir(globalArgs.dstPath.c_str())) == NULL ){
		if( errno != ENOENT) 
			return -1;
		string cmd = "mkdir -p "+ globalArgs.dstPath;
		if(-1 == system(cmd.c_str()))
			return -1;
	}

	if( (access(globalArgs.getPathName().c_str(),F_OK)) == 0)   
	{   
		printf("dest file already exist. exit normally.\n");
		return -1;
	}

	return 0;

}

int main(int argc, char* argv[])
{

	//parse args
	if(parseArgs(argc, argv ) != 0) return -1;
	//globalArgs.info(); //debug

	// start trans format.
	IFileSourcePtr fileSrc(IFileSource::New());
	Format format(8000,16,1);
	if( !fileSrc->Open(globalArgs.srcFileName.c_str(), format) ){
		printf("open src file error\n");
		printf("exit code %d\n",OPEN_SRC_ERROR);
		return OPEN_SRC_ERROR;
	}

	IWaveFileRenderPtr wav = IWaveFileRender::New();
	string dstPathName = globalArgs.dstPath + "/"+globalArgs.dstFileName;
	wav->Open(dstPathName.c_str(), format);
	ISourcePtr srcPtr(fileSrc, dynamic_cast_tag());
	IRenderPtr rnd(wav, dynamic_cast_tag());
	unsigned char buf[1000];
	for(;;){
		Buffer b(buf, sizeof(buf));
		if(!srcPtr->Read(b))
			break;

		if(b.size == sizeof(buf))
			break;

		Buffer_C bufc(buf, sizeof(buf));

		if(!rnd->Write(bufc))
			break;
	}

	fileSrc->Close();
	wav->Close();

	return 0;
}
