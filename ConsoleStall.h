#ifndef START_CONSOLE_FUNC
#define START_CONSOLE_FUNC
#include <stdio.h>
void startConsole(void) {
	printf("Press enter to start...");
	getchar();
}
#endif

#ifndef CONTINUE_CONSOLE_FUNC
#define CONTINUE_CONSOLE_FUNC
#include <stdio.h>
void continueConsole(bool input) {
	printf("Press enter to continue...");
	getchar();
	if (input)
		getchar();
}
#endif

#ifndef CUSTOM_CONTINUE_CONSOLE_FUNC
#define CUSTOM_CONTINUE_CONSOLE_FUNC
#include <stdio.h>
void customContinueConsole(const char *message, bool input) {
	printf(message);
	getchar();
	if (input) 
		getchar();
}
#endif

#ifndef TIME_STRING_FUNC
#define TIME_STRING_FUNC
#include <stdio.h>
const int SECOND_STR_LEN = 6;
const int MINUTE_STR_LEN = 9;
const int HOUR_STR_LEN = 12;
const int MAX_STR_LEN = 25;
const int TIME_STR_LEN = 32;
int msToTimeString(char * str, size_t t) {
	if (str == NULL)
		return -1;
	size_t d;
	unsigned h, m, s, ms;
	ms = t % 1000; t /= 1000;
	s = t % 60; t /= 60;
	m = t % 60; t /= 60;
	h = t % 24; t /= 24;
	d = t;
	sprintf(str, "%zu:%02u:%02u:%02u.%03u", d, h, m, s, ms);
	int i, c;
	for (i = c = 0; 1; i++) {
		if (str[i] == ':' || str[i] == '.') {
			c++;
			if (c == 1 && str[i - 1] != '0')
				return 0;
			if (c == 2 && (str[i - 1] != '0' || str[i - 2] != '0'))
				return 2;
			if (c == 3 && (str[i - 1] != '0' || str[i - 2] != '0'))
				return 5;
			if (c == 4)
				return 8;
		}
	}
};
#endif

#ifndef CLOCK_TO_MILLIS_FUNC
#define CLOCK_TO_MILLIS_FUNC
#include <time.h>
size_t clockToMillis(clock_t t) {
	return (size_t)(t * (1000.0 / CLOCKS_PER_SEC));
}
#endif


#ifndef TIME_PROCESS_FUNC
#define TIME_PROCESS_FUNC
#include <time.h>
static clock_t _TIME_PROCESS_FUNC_start = 0;
inline void startTimer(void) {
	_TIME_PROCESS_FUNC_start = clock();
}
inline clock_t lapTimer(void) {
	return clock() - _TIME_PROCESS_FUNC_start;
}
#endif