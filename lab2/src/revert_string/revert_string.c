#include "revert_string.h"
#include <string.h>

void RevertString(char *str)
{
	int ln = strlen(str);
	for (int i = 0; i < ln/2; i++)
	{
		char c = str[ln - 1 - i];
		str[ln - 1 - i] = str[i];
		str[i] = c;
	}
	
}

