#include <Judy.h>
#include <assert.h>

int main()
{
	Word_t ret;
	unsigned char key[128];
	PWord_t value;
	Pvoid_t arraySL = (Pvoid_t) NULL;

	// JudySL insert
	JSLI(value, arraySL, "min");
	*value = 10;
	JSLI(value, arraySL, "max");
	*value = 20;
	JSLI(value, arraySL, "hp");
	*value = 100;

	// JudySL get
	JSLG(value, arraySL, "max");
	if (value) printf("max = %lu\n", *value);

	// JudySL delete
	JSLD(ret, arraySL, "max");
	JSLG(value, arraySL, "max");
	if (!value) printf("max deleted!\n");

	// JudySL FindFirst && FindNext
	key[0] = '\0';
	JSLF(value, arraySL, key);
	while (value)
	{
		printf("%s = %lu\n", key, *value);
		JSLN(value, arraySL, key);
	}

	// JudySL free all
	JSLFA(ret, arraySL);
	printf("%lu bytes freed\n", ret);

    return 0;
}
