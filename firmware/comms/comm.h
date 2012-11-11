#ifndef USB_COMM_H_
#define USB_COMM_H_

#define COMMAND_OK "result=\"ok\";"
#define COMMAND_ERROR "result=\"error\";"
#define ERROR_CODE_INVALID_PARAM -1
#define ERROR_CODE_MISSING_PARAMS -2
#define ERROR_CODE_INVALID_COMMAND -3
#define ERROR_CODE_CRITICAL_ERROR -4

void initComms(void);

void processCommand(void);

void ShowHelp();

void SendBytes(char *data, unsigned int length);

void SendChar(char c);

void SendString(const char *s);

void SendInt(int n);

void SendHex(int n);

void SendFloat(float f,int precision);

void SendDouble(double f, int precision);

void SendUint(unsigned int n);


void SendNameUint(const char *s, unsigned int n);

void SendNameSuffixUint(const char *s, const char *suf, unsigned int n);

void SendNameIndexUint(const char *s, int i, unsigned int n);


void SendNameInt(const char *s, int n);

void SendNameSuffixInt(const char *s, const char *suf, int n);

void SendNameIndexInt(const char *s, int i, int n);


void SendNameDouble(const char *s, double n, int precision);

void SendNameSuffixDouble(const char *s, const char *suf, double n, int precision);

void SendNameIndexDouble(const char *s, int i, double n, int precision);


void SendNameFloat(const char *s, float n, int precision);

void SendNameSuffixFloat(const char *s, const char *suf, float n, int precision);

void SendNameIndexFloat(const char *s, int i, float n, int precision);


void SendNameString(const char *s, const char *v);

void SendNameSuffixString(const char *s, const char *suf, const char *v);

void SendNameIndexString(const char *s, int i, const char *v);


void SendNameEscapedString(const char *s, const char *v, int length);

void SendCrlf();

void SendCommandOK();

void SendCommandError(int result);

void SendCommandParamError(char *msg);

#endif /*USB_COMM_H_*/
