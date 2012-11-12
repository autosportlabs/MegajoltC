#include "comm.h"
#include "mod_string.h"
#include "board.h"
#include "modp_numtoa.h"
#include "modp_atonum.h"
#include "memory.h"
#include "constants.h"
#include "command.h"
#include "usb.h"
#include "io.h"

#define BUFFER_SIZE MEMORY_PAGE_SIZE * 2

const struct cmd_t commands[] = SYSTEM_COMMANDS;

const char cmdPrompt[] = COMMAND_PROMPT;
const char welcomeMsg[] = WELCOME_MSG;

static int menuPadding = 0;


static void calculateMenuPadding(){
	const struct cmd_t * cmd = commands;

	while (cmd->cmd != NULL){
		int len = strlen(cmd->cmd);
		if (len > menuPadding) menuPadding = len;
		cmd++;
	}
	menuPadding++;
}

void ShowHelp(){
	SendString("Available Commands:");
	SendCrlf();
	SendCrlf();

	const struct cmd_t * cmd = commands;
	while (cmd->cmd != NULL){
		SendString(cmd->cmd);
		int padding = menuPadding - strlen(cmd->cmd);
		while (padding-- > 0) SendString(" ");
		SendString(": ");
		SendString(cmd->help);
		SendString(" ");
		SendString("Usage: ");
		SendString(cmd->cmd);
		SendString(" ");
		SendString(cmd->paramHelp);
		SendCrlf();
		cmd++;
	}
}

void SendCommandOK(){
	SendString(COMMAND_OK);
}

void SendCommandParamError(char *msg){
	SendString(COMMAND_ERROR);
	SendString("extended=\"");
	SendString(msg);
	SendString("\";");
}

void SendCommandError(int result){
	SendString(COMMAND_ERROR);
	SendString("code=");
	SendInt(result);
	SendString(";");
}

static void SendHeader(unsigned int len){
	while (len-- > 0){
		SendChar('=');
	}
	SendCrlf();
}

static void ShowWelcome(){
	SendCrlf();
	unsigned int len = strlen(welcomeMsg);
	SendHeader(len);
	SendString(welcomeMsg);
	SendCrlf();
	SendHeader(len);
	SendCrlf();
	ShowHelp();
}

static void CmdParse(char *cmdStr){

	if (strlen(cmdStr) == 0){
		ShowWelcome();
		return;
	}

	unsigned char argc, i = 0;
	char *argv[30];

	argv[i] = strtok(cmdStr, " ");

	do{
		argv[++i] = strtok(NULL, " ");
	}
	while (( i < 30) && (argv[i] != NULL));

	argc = i;
	const struct cmd_t * cmd = commands;

	while (cmd->cmd != NULL){
		if (!strcmp(argv[0], cmd->cmd)){
			cmd->func(argc, argv);
			SendCrlf();
			break;
		}
		cmd++;
	}
	if (NULL == cmd->cmd){
		   toggleLED(LED_2);
		   for (int x=0;x < 1000000; x++){}


		SendString("Unknown Command- Press Enter for Help.");
		SendCrlf();
	}
}

void SendInt(int n){
	char buf[12];
	modp_itoa10(n,buf);
	SendString(buf);
}

void SendFloat(float f,int precision){
	char buf[20];
	modp_ftoa(f,buf,precision);
	SendString(buf);
}

void SendDouble(double f, int precision){
	char buf[30];
	modp_dtoa(f,buf,precision);
	SendString(buf);
}

void SendHex(int n){
	char buf[30];
	modp_itoaX(n,buf,16);
	SendString(buf);
}


void SendUint(unsigned int n){
	char buf[20];
	modp_uitoa10(n,buf);
	SendString(buf);
}

void SendNameIndexUint(const char *s, int i, unsigned int n){
	SendString(s);
	SendString("_");
	SendUint(i);
	SendString("=");
	SendUint(n);
	SendString(";");
}

void SendNameSuffixUint(const char *s, const char *suf, unsigned int n){
	SendString(s);
	SendString("_");
	SendString(suf);
	SendString("=");
	SendUint(n);
	SendString(";");
}

void SendNameUint(const char *s, unsigned int n){
	SendString(s);
	SendString("=");
	SendUint(n);
	SendString(";");
}

void SendNameIndexInt(const char *s, int i, int n){
	SendString(s);
	SendString("_");
	SendUint(i);
	SendString("=");
	SendInt(n);
	SendString(";");
}

void SendNameSuffixInt(const char *s, const char *suf, int n){
	SendString(s);
	SendString("_");
	SendString(suf);
	SendString("=");
	SendInt(n);
	SendString(";");
}

void SendNameInt(const char *s, int n){
	SendString(s);
	SendString("=");
	SendInt(n);
	SendString(";");
}

void SendNameIndexDouble(const char *s, int i, double n, int precision){
	SendString(s);
	SendString("_");
	SendUint(i);
	SendString("=");
	SendDouble(n,precision);
	SendString(";");
}

void SendNameSuffixDouble(const char *s, const char *suf, double n, int precision){
	SendString(s);
	SendString("_");
	SendString(suf);
	SendString("=");
	SendDouble(n,precision);
	SendString(";");
}

void SendNameDouble(const char *s, double n, int precision){
	SendString(s);
	SendString("=");
	SendDouble(n,precision);
	SendString(";");
}

void SendNameIndexFloat(const char *s, int i, float n, int precision){
	SendString(s);
	SendString("_");
	SendUint(i);
	SendString("=");
	SendFloat(n,precision);
	SendString(";");
}

void SendNameSuffixFloat(const char *s, const char *suf, float n, int precision){
	SendString(s);
	SendString("_");
	SendString(suf);
	SendString("=");
	SendFloat(n,precision);
	SendString(";");
}

void SendNameFloat(const char *s, float n, int precision){
	SendString(s);
	SendString("=");
	SendFloat(n, precision);
	SendString(";");
}

void SendNameString(const char *s, const char *v){
	SendString(s);
	SendString("=\"");
	SendString(v);
	SendString("\";");
}

void SendNameSuffixString(const char *s, const char *suf, const char *v){
	SendString(s);
	SendString("_");
	SendString(suf);
	SendString("=\"");
	SendString(v);
	SendString("\";");
}

void SendNameIndexString(const char *s, int i, const char *v){
	SendString(s);
	SendString("_");
	SendUint(i);
	SendString("=\"");
	SendString(v);
	SendString("\";");
}

void SendNameEscapedString(const char *s, const char *v, int length){
	SendString(s);
	SendString("=\"");
	const char *value = v;
	while (value - v < length){
		switch(*value){
			case ' ':
				SendString("\\_");
				break;
			case '\n':
				SendString("\\n");
				break;
			case '\r':
				SendString("\\r");
				break;
			case '"':
				SendString("\\\"");
				break;
			default:
				SendChar(*value);
				break;
		}
		value++;
	}
	SendString("\";");
}


void SendString(const char *s){
	USB_write(s, strlen(s));
}

void SendBytes(char *data, unsigned int length){
	USB_write(data, length);
}

void SendChar(char c){
	USB_write(&c, 1);
}

void SendCrlf(){
	SendString("\r\n");
}

void initComms(void) {
	USB_Open();
	calculateMenuPadding();
}

void processCommand(void) {
	char * line = USB_readLine();

	if (line){
		CmdParse(line);
		SendString(cmdPrompt);
		SendString(" > ");
	}
}
