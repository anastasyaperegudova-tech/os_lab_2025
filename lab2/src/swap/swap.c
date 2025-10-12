#include "swap.h"
void Swap(char *left, char *right)
{
	char c =*right;
	*right = *left;
	*left = c;

}
