#include "raper_predefine.h"




int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);

	int ret = 0;

	HANDLE handle_out = ::GetStdHandle(STD_OUTPUT_HANDLE);
	if (handle_out == INVALID_HANDLE_VALUE)
	{
	    return -1;
	}
	//������Ļ�������������Ļ��С
	
	COORD coord = { 121,  301};
	BOOL bret = ::SetConsoleScreenBufferSize(handle_out, coord);
	if (bret == FALSE)
	{
		DWORD ret_error= ::GetLastError();
		//
	}
	SMALL_RECT rect = { 0, 0, 120, 60 };
	bret = ::SetConsoleWindowInfo(handle_out, TRUE, &rect);
	if (bret == FALSE)
	{
	    return -1;
	}

	QString error_tips;
	QString allinone_path = "E:/Courage/illusion.git/example/001";

	const struct option RAPER_LONG_OPTION[]=
	{
		{ "config", required_argument,NULL, 'c' },
		{ "excel", required_argument, NULL,'x' },
		{ "proto", required_argument,NULL, 'p' },
		{ "outer", required_argument,NULL, 'o' },
		{ "help", no_argument,NULL, 'h' },
		{ NULL, no_argument, NULL, 0 }
	};
	int long_index = 0;
	const char RAPER_OPT_STRING[] = "c:x:p:o:h";
	int opt = -1;
	while (-1 != (opt = ZCE_LIB::getopt_long(argc, argv,
											 RAPER_OPT_STRING,
											 RAPER_LONG_OPTION,
											 &long_index)))
	{
		switch (opt) 
		{

		};
	}
	ret = Biko_Read_Config::instance()->init_read_all2(allinone_path,
													   error_tips);
	if (ret != 0)
	{
		Biko_Read_Config::clean_instance();
		return -1;
	}

	ret = Biko_Read_Config::instance()->read_excel(error_tips);
	if (ret != 0)
	{
		Biko_Read_Config::clean_instance();
		return -1;
	}
	Biko_Read_Config::clean_instance();


	return a.exec();
}
