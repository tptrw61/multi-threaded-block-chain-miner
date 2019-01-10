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
void continueConsole(int input) {
	printf("Press enter to continue...");
	getchar();
	if (input)
		getchar();
}
#endif

#ifndef CUSTOM_CONTINUE_CONSOLE_FUNC
#define CUSTOM_CONTINUE_CONSOLE_FUNC
#include <stdio.h>
void customContinueConsole(const char *message, int input) {
	printf("%s", message);
	getchar();
	if (input) 
		getchar();
}
#endif
